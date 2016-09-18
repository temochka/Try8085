/*
  Copyright (C) 2009, 2016  Artem Chistyakov <chistyakov.artem@gmail.com>
  Copyright (C) 2009  Ilya Beda <ir4y.ix@gmail.com>
  Copyright (C) 2009  Daniil Zapyatoy <zapper.kb@gmail.com>

  This file is part of Try8085.

  Try8085 is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Try8085 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Try8085.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "translator.h"
#include "asm_id.h"

Translator::Translator(QString sourceText)
{
  // Replace quoted characters with codes
  QRegExp chrRegex("\\'(.)\\'");
  int pos = 0;
  while ((pos = chrRegex.indexIn(sourceText, pos)) != -1) {
    QStringList list = chrRegex.capturedTexts();
    QString chr = list.at(1);
    char c = chr.toStdString().c_str()[0];
    sourceText.replace(pos, chrRegex.matchedLength(),
                       QString().sprintf("%d", c));
    pos += chrRegex.matchedLength();
  }

  sourceText = sourceText.toUpper();

  QRegExp hexRegex("\\b([0-9]{1}[0-9A-F]*H)\\b");
  pos = 0;
  while ((pos = hexRegex.indexIn(sourceText, pos)) != -1) {
    QStringList list = hexRegex.capturedTexts();
    QString symb = list.at(1);
    int value = 0;
    if (asm_util_parse_number((char*)symb.toStdString().c_str(), &value)) {
      sourceText.replace(QRegExp("\\b" + symb + "\\b"),
                         QString().sprintf("%d", value));
      pos += QString().sprintf("%d", value).length();
    } else {
      pos += hexRegex.matchedLength();
    }
  }

  QRegExp binRegex("\\b([01]+B)\\b");
  pos = 0;
  while ((pos = binRegex.indexIn(sourceText, pos)) != -1) {
    QStringList list = binRegex.capturedTexts();
    QString symb = list.at(1);
    int value = 0;
    if (asm_util_parse_number((char*)symb.toStdString().c_str(), &value)) {
      sourceText.replace(symb, QString().sprintf("%d", value));
      pos += QString().sprintf("%d", value).length();
    } else {
      pos += binRegex.matchedLength();
    }
  }

  this->sourceText = sourceText.split("\n");

  this->initRegexp();
  this->errorsList.clear();
  this->bytecode = new unsigned char[0xFFFF];
  memset(this->bytecode, 0, 0xFFFF * sizeof(unsigned char));
}

void
Translator::initRegexp()
{
  QString labelRexp = "([A-Za-z]{1}[A-Za-z0-9\\_]*\\:){0,1}";
  QString commentRexp = "(\\;.*){0,1}";

  zeroArg = new QRegExp("^[ \\t]*" + labelRexp +
                        "[ \\t]*([A-Za-z]{2,5})[ \\t]*" + commentRexp + "$");
  oneArg = new QRegExp("^[ \\t]*" + labelRexp + "[ \\t]*([A-Za-z]{2,5})[ "
                                                "\\t]+([A-Za-z0-9\\_\\-\\(\\)"
                                                "\\+\\*\\/\\']+)[ \\t]*" +
                       commentRexp + "$");
  twoArg = new QRegExp("^[ \\t]*" + labelRexp +
                       "[ \\t]*([A-Za-z]{2,5})[ \\t]*([A-Za-z0-9]+)[ \\t]*,[ "
                       "\\t]*([A-Za-z0-9\\_\\-\\+\\(\\)\\*\\/\\']+)[ \\t]*" +
                       commentRexp + "$");
  orgRexp =
    new QRegExp("^[ \\t]*\\#([A-Za-z]{1,5})[ \\t]*([^ \\t\\n\\r]+)[ \\t]*" +
                commentRexp + "$");
  endrRexp = new QRegExp("^[ \\t]*\\#ENDR[ \\t]*" + commentRexp + "$");
  varRexp = new QRegExp("^[ \\t]*([A-Za-z]{1}[A-Za-z0-9\\_]*)[ \\t]*\\=[ "
                        "\\t]*([A-Za-z0-9\\_\\-\\+\\(\\)\\*\\/\\']+)[ \\t]*" +
                        commentRexp + "$");
  commentExpr =
    new QRegExp("^[ \\t]*" + labelRexp + "[ \\t]*" + commentRexp + "$");
}

QList<TranslatorError>*
Translator::parse()
{
  bool lastLabel = false;
  bool need_operand = false;
  bool emptyEntry = true;
  int curAddress = 0x0000;
  bool forceNopCommand = false;
  char cCmd[ASM_DS_MAX_OPCODE_LENGTH];
  char cArg1[ASM_DS_MAX_OPERAND];
  char cArg2[ASM_DS_MAX_OPERAND];
  int realLineNumber = 0;
  IdOpcode* id_opcode;
  IdPseudo* id_pseudo;

  AsmSourceEntry* entry = new AsmSourceEntry;
  memset(entry, 0, sizeof(AsmSourceEntry));

  /* Evaluate repetition blocks */
  for (realLineNumber = 0; realLineNumber < this->sourceText.size();
       realLineNumber++) {
    QString line = this->sourceText.at(realLineNumber).trimmed();
    if (orgRexp->exactMatch(line)) {
      QStringList matches = this->orgRexp->capturedTexts();
      QString cCmd = matches.at(1);
      if (cCmd == "REP") {
        emptyEntry = true;
        int numReps = 0;
        if (!asm_util_parse_number((char*)matches.at(2).toStdString().c_str(),
                                   &numReps)) {
          QString err(QObject::tr("Unexpected value for REP given."));
          this->addError(err, realLineNumber + 1);
          continue;
        }
        int len = 0;
        QString err("");
        for (int ind = realLineNumber + 1; ind < this->sourceText.size();
             ind++) {
          QString bufLine = this->sourceText.at(ind).trimmed();
          if (!bufLine.length())
            continue;
          if (endrRexp->exactMatch(bufLine)) {
            len = ind - realLineNumber - 1;
            this->sourceText.removeAt(ind);
            break;
          } else if (orgRexp->exactMatch(bufLine)) {
            err =
              QObject::tr("Macrocommands are not allowed in repeat blocks.");
            this->addError(err, ind + 1);
            break;
          } else {
            continue;
          }
          err = QObject::tr("No ENDR found for repeat block.");
          this->addError(err, realLineNumber + 1);
        }
        if (!len) {
          QString err(QObject::tr("Empty repeat block."));
          this->addError(err, realLineNumber + 1);
          continue;
        } else {
          QStringList fragment = this->sourceText.mid(realLineNumber + 1, len);
          int ind = realLineNumber + len + 1;
          for (int rep = 0; rep < numReps - 1; rep++) {
            for (int i = 0; i < fragment.size(); i++) {
              this->sourceText.insert(ind++, fragment.at(i));
            }
          }
        }
        this->sourceText.removeAt(realLineNumber);
        realLineNumber--;
      }
    }
  }

  for (realLineNumber = 0; realLineNumber < this->sourceText.size();
       realLineNumber++) {
    QString badLine = this->sourceText.at(realLineNumber);
    need_operand = false;
    // When the preceeding line contains a standalone label
    if ((!lastLabel && !emptyEntry) || forceNopCommand) {
      entry->line_number = realLineNumber;
      entry->address = curAddress;
      if (forceNopCommand) {
        forceNopCommand = false;
        lastLabel = false;
      }
      if (entry->s_op_id_opcode) {
        curAddress += asm_command_size(entry->s_op);
      } else if (entry->s_op_id_pseudo) {
        curAddress++;
      }
      this->source.append(*entry);
      memset(entry, 0, sizeof(AsmSourceEntry));
      entry->label_parsed = false;
    }

    cCmd[0] = '\0';
    cArg1[0] = '\0';
    cArg2[0] = '\0';

    QString line = badLine.trimmed();

    // Empty line
    if (!line.length()) {
      emptyEntry = true;
      continue;
      // [Label] [Comment]
    } else if (this->commentExpr->exactMatch(line)) {
      emptyEntry = true;
      QStringList matches = this->commentExpr->capturedTexts();
      if (!matches.at(1).isEmpty()) {
        emptyEntry = false;
        if (lastLabel) {
          QString buf(QObject::tr("Unexpected label given."));
          this->addError(buf, realLineNumber + 1);
          continue;
        }
        lastLabel = true;
        QString label = matches.at(1);
        label = label.remove(":");
        strcpy(entry->s_id, label.toStdString().c_str());
      }
      // [Label] <Command> <Arg> [Comment]
    } else if (this->oneArg->exactMatch(line)) {
      emptyEntry = false;
      QStringList matches = this->oneArg->capturedTexts();
      if (!matches.at(1).isEmpty()) {
        if (lastLabel) {
          QString buf(QObject::tr("Unexpected label given."));
          this->addError(buf, realLineNumber + 1);
        }
        QString label = matches.at(1);
        label = label.remove(":");
      }
      lastLabel = false;
      strcpy(cCmd, matches.at(2).toStdString().c_str());

      if (!asm_existing_command(cCmd)) {
        QString buf(QObject::tr("Unrecognized command given (") +
                    QString(cCmd) + ")");
        this->addError(buf, realLineNumber + 1);
        continue;
      }
      strcpy(cArg1, matches.at(3).toStdString().c_str());
      id_opcode = asm_id_opcode_lookup(cCmd, cArg1, NULL);
      if (id_opcode) {
        entry->s_op_id_opcode = id_opcode;
        entry->s_op = id_opcode->op_num;
        entry->b_op = id_opcode->op_num;
        entry->s_opnd_size = id_opcode->user_args;
        if (id_opcode->user_args) {
          need_operand = true;
        }
      } else if ((id_pseudo = asm_id_pseudo_lookup(cCmd))) {
        entry->s_op_id_pseudo = id_pseudo;
        entry->s_op = id_pseudo->op_num;
        entry->b_op = id_pseudo->op_num;
        entry->s_opnd_size = id_pseudo->user_args;
        if (id_pseudo->user_args) {
          need_operand = true;
        }
      } else {
        QString buf(QObject::tr("Unrecognized command given (") +
                    QString(cCmd) + ")");
        this->addError(buf, realLineNumber + 1);
      }
      if (need_operand) {
        strcpy(entry->s_opnd, cArg1);
      }
      // [Label] <Command> [Comment]
    } else if (this->zeroArg->exactMatch(line)) {
      emptyEntry = false;
      QStringList matches = this->zeroArg->capturedTexts();
      if (!matches.at(1).isEmpty()) {
        if (lastLabel) {
          QString buf(QObject::tr("Unexpected label given."));
          this->addError(buf, realLineNumber + 1);
          continue;
        }
        QString label = matches.at(1);
        label = label.remove(":");
      }
      lastLabel = false;
      strcpy(cCmd, matches.at(2).toStdString().c_str());
      strcpy(cCmd, matches.at(2).toStdString().c_str());
      if (!asm_existing_command(cCmd)) {
        QString buf(QObject::tr("Unrecognized command given (") +
                    QString(cCmd) + ")");
        this->addError(buf, realLineNumber + 1);
        continue;
      }

      id_opcode = asm_id_opcode_lookup(cCmd, NULL, NULL);
      if (id_opcode) {
        entry->s_op_id_opcode = id_opcode;
        entry->s_op = id_opcode->op_num;
        entry->b_op = id_opcode->op_num;
        entry->s_opnd_size = id_opcode->user_args;
      } else if (id_pseudo = asm_id_pseudo_lookup(cCmd)) {
        entry->s_op_id_pseudo = id_pseudo;
        entry->s_op = id_pseudo->op_num;
        entry->b_op = id_pseudo->op_num;
        entry->s_opnd_size = id_pseudo->user_args;
      } else {
        QString buf(QObject::tr("Unrecognized command given (") +
                    QString(cCmd) + ")");
        this->addError(buf, realLineNumber + 1);
      }
      // [Label] <Command> <Arg1> <Arg2> [Comment]
    } else if (this->twoArg->exactMatch(line)) {
      emptyEntry = false;
      QStringList matches = this->twoArg->capturedTexts();
      if (!matches.at(1).isEmpty()) {
        if (lastLabel) {
          QString buf(QObject::tr("Unexpected label given."));
          this->addError(buf, realLineNumber + 1);
          continue;
        }
        lastLabel = true;
        QString label = matches.at(1);
        label = label.remove(":");
      }
      lastLabel = false;

      strcpy(cCmd, matches.at(2).toStdString().c_str());
      strcpy(cCmd, matches.at(2).toStdString().c_str());

      if (!asm_existing_command(cCmd)) {
        QString buf(QObject::tr("Unrecognized command given (") +
                    QString(cCmd) + ")");
        this->addError(buf, realLineNumber + 1);
        continue;
      }
      strcpy(cArg1, matches.at(3).toStdString().c_str());
      strcpy(cArg2, matches.at(4).toStdString().c_str());

      id_opcode = asm_id_opcode_lookup(cCmd, cArg1, cArg2);
      if (id_opcode) {
        entry->s_op_id_opcode = id_opcode;
        entry->s_op = id_opcode->op_num;
        entry->b_op = id_opcode->op_num;
        entry->s_opnd_size = id_opcode->user_args;
        if (id_opcode->user_args) {
          need_operand = true;
        }
      } else if (id_pseudo = asm_id_pseudo_lookup(cCmd)) {
        entry->s_op_id_pseudo = id_pseudo;
        entry->s_op = id_pseudo->op_num;
        entry->b_op = id_pseudo->op_num;
        entry->s_opnd_size = id_pseudo->user_args;
      } else {
        QString buf(QObject::tr("Unrecognized command given (") +
                    QString(cCmd) + ")");
        this->addError(buf, realLineNumber + 1);
      }

      if (need_operand) {
        strcpy(entry->s_opnd, cArg2);
      }
      // #<Macrocommand> <Arg>
    } else if (orgRexp->exactMatch(line)) {
      QStringList matches = this->orgRexp->capturedTexts();
      QString cCmd = matches.at(1);
      if (cCmd == "ORG") {
        emptyEntry = true;
        int offset;
        QString offsetStr = matches.at(2);
        if (asm_util_parse_number((char*)offsetStr.toStdString().c_str(),
                                  &offset)) {
          if (curAddress > offset) {
            QString err(
              QObject::tr("Offset must be bigger then current address."));
            this->addError(err, realLineNumber + 1);
          } else {
            curAddress = offset;
          }
        } else {
          QString err(QObject::tr("Unexpected value for ORG given."));
          this->addError(err, realLineNumber + 1);
        }
        if (lastLabel) {
          realLineNumber--;
          forceNopCommand = true;
          continue;
        }
      } else {
        emptyEntry = true;
        QString err(QObject::tr("Unknown macrocommand"));
        this->addError(err, realLineNumber + 1);
      }
      // <Variable> = <Arg> [Comment]
    } else if (varRexp->exactMatch(line)) {
      emptyEntry = true;
      lastLabel = false;
      QStringList matches = this->varRexp->capturedTexts();
      if (!matches.at(1).isEmpty() && !matches.at(2).isEmpty()) {
        if (matches.at(1) == matches.at(2)) {
          QString err(
            QObject::tr("Trying to declare translator's variable as itself."));
          this->addError(err, realLineNumber + 1);
        } else {
          emptyEntry = false;
          entry->s_op_id_macro = new IdMacro(matches.at(1));
          strcpy(entry->s_opnd, matches.at(2).toStdString().c_str());
        }
      } else {
        QString err(QObject::tr("Incorrect translator's variable definition."));
        this->addError(err, realLineNumber + 1);
      }
      // Unexpected
    } else {
      QString buf(QObject::tr("Translator can not translate the line."));
      this->addError(buf, realLineNumber + 1);
    }
  }

  if (lastLabel || !emptyEntry) {
    entry->line_number = realLineNumber;
    entry->address = curAddress;
    if (entry->s_op_id_opcode) {
      curAddress += asm_command_size(entry->s_op);
    } else {
      curAddress++;
    }
    this->source.append(*entry);
  }

  delete entry;

  // First pass: resolve labels into addresses, turns symbols into codes
  for (int i = 0; i < this->source.size(); i++) {
    AsmSourceEntry buf = this->source.at(i);
    if (buf.s_id[0]) {
      if (asm_id_keywords_lookup(buf.s_id)) {
        QString err(QObject::tr("Reserved words can't be used as labels."));
        this->addError(err, buf.line_number);
        continue;
      }
      for (int j = 0; j < this->source.size(); j++) {
        AsmSourceEntry buf2 = this->source.at(j);
        if (buf2.s_op_id_opcode && buf2.s_op_id_opcode->user_args &&
            QString(buf2.s_opnd)
              .contains(QRegExp(QString().sprintf("\\b%s\\b", buf.s_id)))) {
          QString tmp(buf2.s_opnd);
          tmp = tmp.replace(
            QRegExp(QString().sprintf("\\b%s\\b", buf.s_id)),
            QString().sprintf("%d", buf.address).toStdString().c_str());
          strcpy(buf2.s_opnd, tmp.toStdString().c_str());
          this->source.replace(j, buf2);
        }
      }
    }
    // Process macro variables
    if (buf.s_op_id_macro) {
      if (asm_id_keywords_lookup(
            (char*)buf.s_op_id_macro->name.toStdString().c_str())) {
        QString err(QObject::tr(
          "Reserved words can't be used as translator variables names."));
        this->addError(err, buf.line_number);
        continue;
      }
      for (int j = i; j < this->source.size(); j++) {
        AsmSourceEntry buf2 = this->source.at(j);
        if (((buf2.s_op_id_opcode && buf2.s_op_id_opcode->user_args) ||
             buf2.s_op_id_macro || (buf2.s_op_id_pseudo)) &&
            QString(buf2.s_opnd)
              .contains(
                QRegExp(QString("\\b" + buf.s_op_id_macro->name + "\\b")))) {
          QString tmp(buf2.s_opnd);
          tmp = tmp.replace(
            QRegExp(QString("\\b" + buf.s_op_id_macro->name + "\\b")),
            QString(buf.s_opnd));
          strcpy(buf2.s_opnd, tmp.toStdString().c_str());
          this->source.replace(j, buf2);
          if (buf2.s_op_id_macro &&
              (buf2.s_op_id_macro->name == buf.s_op_id_macro->name)) {
            break;
          }
        }
      }
    }
  }

  // Second pass
  for (int i = 0; i < this->source.size(); i++) {
    int number = 0;
    AsmSourceEntry buf = this->source.at(i);
    if (buf.s_opnd[0] && !asm_util_parse_number(buf.s_opnd, &number)) {
      if (QString(buf.s_opnd).contains(QRegExp("[\\/\\*\\+\\-]"))) {
        number = get_result(buf.s_opnd);
        strcpy(buf.s_opnd,
               QString().sprintf("%d", number).toStdString().c_str());
        this->source.replace(i, buf);
      } else {
        QString err(QObject::tr("Undefined label"));
        this->addError(err, buf.line_number);
      }
    }
  }

  this->assemble();

  if (this->errorsList.size()) {
    return &this->errorsList;
  }
  return NULL;
}

void
Translator::addError(QString& str, int line)
{
  this->errorsList.append(TranslatorError(str, line));
}

bool
Translator::assemble()
{
  memset(this->bytecode, 0, 0xFFFF * sizeof(unsigned char));
  for (int i = 0; i < this->source.size(); i++) {
    AsmSourceEntry buf = this->source.at(i);
    int value = 0;
    if (buf.s_op_id_opcode) {
      this->bytecode[buf.address] = buf.s_op;
      if (buf.s_op_id_opcode->user_args == 1) {
        if (asm_util_parse_number(buf.s_opnd, &value)) {
          this->bytecode[buf.address + 1] = (unsigned char)value;
        } else {
          QString err(QObject::tr("Incorrect argument or code"));
          this->addError(err, buf.line_number);
          return false;
        }
      } else if (buf.s_op_id_opcode->user_args == 2) {
        if (asm_util_parse_number(buf.s_opnd, &value)) {
          this->bytecode[buf.address + 1] = value % 256;
          this->bytecode[buf.address + 2] = value / 256;
        } else {
          QString err(QObject::tr("Incorrect argument or code"));
          this->addError(err, buf.line_number);
          return false;
        }
      }
    } else if (buf.s_op_id_pseudo) {
      switch (buf.s_op_id_pseudo->op_num) {
        case ID_PSEUDO_DB:
          if (asm_util_parse_number(buf.s_opnd, &value)) {
            this->bytecode[buf.address] = value;
          }
          break;
        default:
          QString err("Incorrect argument or code");
          this->addError(err, buf.line_number);
          return false;
      }
    }
  }
  return true;
}

unsigned char*
Translator::getMemory()
{
  return this->bytecode;
}

int
Translator::disassemble(int addr, QString* disassembled)
{
  if (!this->bytecode[addr]) {
    *disassembled = QString("NOP");
    return 1;
  }
  IdOpcode* bufOp;
  short size = 0;
  if (bufOp = asm_id_opcode_lookup_by_code(this->bytecode[addr])) {
    *disassembled = QString(bufOp->op_str);
    short num_args = bufOp->num_args + (bufOp->user_args > 0);

    // 1 arg
    if (num_args == 1) {
      // Built-in argument
      if (bufOp->num_args == 1) {
        *disassembled += QString().sprintf(" %s", bufOp->arg1);
        // User provided argument
      } else if (bufOp->user_args) {
        switch (bufOp->user_args) {
          case 1: // 1 byte
            *disassembled += QString().sprintf(
              " 0%Xh", (unsigned char)this->bytecode[addr + 1]);
            break;
          case 2: // 2 bytes
            short tb =
              this->bytecode[addr + 1] + this->bytecode[addr + 2] * 256;
            *disassembled += QString().sprintf(" 0%Xh", tb);
            break;
        }
      }
      // 2 args
    } else if (num_args == 2) {
      if (bufOp->num_args == 2) {
        *disassembled += QString().sprintf(" %s, %s", bufOp->arg1, bufOp->arg2);
      } else if (bufOp->num_args == 1 && bufOp->user_args) {
        short t = 0;
        switch (bufOp->user_args) {
          case 1:
            t = this->bytecode[addr + 1];
            break;
          case 2:
            t = this->bytecode[addr + 1] + this->bytecode[addr + 2] * 256;
            break;
        }
        *disassembled +=
          QString().sprintf(" %s, 0%Xh", bufOp->arg1,
                            bufOp->user_args == 1 ? (unsigned char)t : t);
      }
    }
    size = asm_command_size(this->bytecode[addr]);
  } else {
    *disassembled = QString("DB ");
    *disassembled += QString().sprintf("0%Xh", this->bytecode[addr]);
    size = 1;
  }
  return size;
}
