/*
  Copyright (C) 2009, 2016  Artem Chistyakov <chistyakov.artem@gmail.com>
  Copyright (C) 2009  Ilya Beda <ir4y.ix@gmail.com>
  Copyright (C) 2009  Daniil Zapyatoy <zapper.kb@gmail.com>
  Copyright (C) 2003  Sridhar Ratnakumar <sridhar.ratna@gmail.com>

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
#ifndef TRY8085_TRANSLATOR_H
#define TRY8085_TRANSLATOR_H

#include "arithmetic.h"
#include "asm_id.h"
#include "asm_source.h"
#include <QDebug>
#include <QList>
#include <QString>
#include <QStringList>
#include <iostream>

class TranslatorError
{
public:
  TranslatorError(QString str, int line)
  {
    this->description = str;
    this->line = line;
  }
  QString getDescription() { return description; }
  int getLine() { return line; }
private:
  int line;
  QString description;
};

class Translator
{
private:
  QStringList sourceText;
  QStringList zeroArgCommands;
  QStringList oneArgCommands;
  QStringList twoArgCommands;
  QList<TranslatorError> errorsList;
  int IP;
  unsigned char* bytecode;
  QList<AsmSourceEntry> source;

  QRegExp *zeroArg, *oneArg, *twoArg, *commentExpr, *orgRexp, *varRexp,
    *endrRexp;
  void initRegexp();
  void addError(QString&, int);
  bool assemble();

public:
  Translator(QString sourceText);
  QList<TranslatorError>* parse();
  unsigned char* getMemory();
  int disassemble(int addr, QString* disassemble);
};

#endif // TRY8085_TRANSLATOR_H
