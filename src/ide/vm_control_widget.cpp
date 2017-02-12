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
#include "vm_control_widget.h"

VMControlWidget::VMControlWidget(QWidget* parent)
  : QWidget(parent)
{
  this->vm = new VM();
  this->vmThread = new VMThread(vm);
  this->vmThread->start();

  this->connect(this->vmThread, SIGNAL(stoped()), this, SLOT(redrawTable()));

  this->tabs = new QTabWidget;
  this->cmpPage = new QWidget;
  this->hexPage = new QWidget;
  this->tabs->addTab(this->cmpPage, tr("Disassembler"));
  this->tabs->addTab(this->hexPage, tr("Hex"));
  this->leftColumn = new QVBoxLayout;
  this->rightColumn = new QVBoxLayout;
  this->columnLayout = new QHBoxLayout;
  this->leftColumn->addWidget(this->tabs);

  this->stateLayout = new QHBoxLayout;
  this->registersState = new RegistersStateWidget(tr("Registers"));
  this->stateLayout->addWidget(this->registersState);

  this->flagsState = new FlagsStateWidget(tr("Flags"));
  this->stateLayout->addWidget(this->flagsState);
  this->rightColumn->addLayout(this->stateLayout);

  this->decToHex = new BaseConverterWidget(tr("Converter"));
  this->rightColumn->addWidget(this->decToHex);

  this->goToAddress = new GoToAddressWidget(tr("Go to address"));
  this->rightColumn->addWidget(this->goToAddress);
  this->connect(this->goToAddress, SIGNAL(triggered(int)), this,
                SLOT(goToCell(int)));

  this->rightColumn->setAlignment(Qt::AlignTop);
  this->columnLayout->addLayout(this->leftColumn);
  this->columnLayout->addLayout(this->rightColumn);

  this->cmpTableLayout = new QVBoxLayout;
  this->cmpTable = new QTableWidget;
  this->cmpTableLayout->addWidget(this->cmpTable);
  this->cmpPage->setLayout(this->cmpTableLayout);
  this->cmpTable->setColumnCount(2);
  this->cmpTable->setHorizontalHeaderItem(0, new QTableWidgetItem(tr("Hex")));
  this->cmpTable->setHorizontalHeaderItem(1,
                                          new QTableWidgetItem(tr("Command")));
  this->setLayout(this->columnLayout);
  this->cmpTable->setRowCount(0xFFFF + 0x1);

  this->hexTableLayout = new QVBoxLayout;
  this->hexTable = new QTableWidget;
  this->hexTableLayout->addWidget(this->hexTable);
  this->hexPage->setLayout(this->hexTableLayout);
  this->hexTable->setColumnCount(17);
  this->hexTable->setRowCount(0xFFFF / 0x10 + 1);

  for (int i = 0; i < 16; i++) {
    this->hexTable->setColumnWidth(i, 30);
    this->hexTable->setHorizontalHeaderItem(
      i, new QTableWidgetItem(QString().sprintf("+%X", i)));
  }
  this->hexTable->setColumnWidth(16, 140);
  this->hexTable->setHorizontalHeaderItem(16,
                                          new QTableWidgetItem(tr("String")));

  for (int i = 0x0; i <= 0xFFFF; i++) {
    this->cmpTable->setVerticalHeaderItem(
      i, new QTableWidgetItem(QString().sprintf("%04X", i)));
    if (i < 0xFFFF / 0x10 + 1) {
      this->hexTable->setVerticalHeaderItem(
        i, new QTableWidgetItem(QString().sprintf("%04X", 0x10 * i)));
    }
    this->hexTable->setItem(i / 16, i % 16, new QTableWidgetItem("00"));
    this->hexTable->item(i / 16, i % 16)
      ->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    this->hexTable->setItem(i / 16, 16, new QTableWidgetItem());
    this->hexTable->item(i / 16, 16)
      ->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

    this->cmpTable->setItem(i, 0, new QTableWidgetItem("00"));
    this->cmpTable->setItem(i, 1, new QTableWidgetItem("NOP"));
    this->cmpTable->item(i, 0)->setFlags(Qt::ItemIsSelectable |
                                         Qt::ItemIsEnabled);
    this->cmpTable->item(i, 1)->setFlags(Qt::ItemIsSelectable |
                                         Qt::ItemIsEnabled);
  }
}

QList<TranslatorError>*
VMControlWidget::toTranslator(QString sourceText)
{
  this->translator = new Translator(sourceText);
  QList<TranslatorError>* errorsList = this->translator->parse();
  return errorsList;
}

void
VMControlWidget::loadMemory()
{
  this->vm->set_memory(this->translator->getMemory());
  this->redrawTable();
}

void
VMControlWidget::updateTable(bool debug, bool exec)
{
  if (exec) {
    this->lockTable(true);
    if (debug) {
      this->lockTable(true);
      vmThread->step();
    } else {
      vmThread->exec();
    }
  }
}

void
VMControlWidget::showProcState()
{
  this->registersState->setRegisters(this->vm->A, this->vm->B, this->vm->C,
                                     this->vm->D, this->vm->E, this->vm->H,
                                     this->vm->L, this->vm->SP, this->vm->IP);
  this->goToCell(this->vm->IP);
  this->flagsState->setFlags(this->vm->flag.S, this->vm->flag.Z,
                             this->vm->flag.I, this->vm->flag.P,
                             this->vm->flag.C);
}

VMControlWidget::~VMControlWidget()
{
}

void
VMControlWidget::goToCell(int address)
{
  this->cmpTable->selectRow(address);
  int row = (int)(address / 16);
  int col = (int)(address % 16);
  this->hexTable->setCurrentCell(row, col);
}

void
VMControlWidget::loadModules(QMap<QString, ModuleSlotWidget>* activeModules)
{
  QMapIterator<QString, ModuleSlotWidget> i(*activeModules);
  QFileInfo executableFilePath = QFileInfo(QCoreApplication::applicationFilePath());
  QDir executablePath = QDir(executableFilePath.absolutePath());

  while (i.hasNext()) {
    i.next();
    if (i.value().enabled) {
      this->loadedModules.insert(i.key(), new QProcess(this));
      this->loadedModules.value(i.key())->start(executablePath.filePath(i.value().fileName),
                                                QProcess::ReadWrite);
    }
  }
}

void
VMControlWidget::closeModules()
{
  QMapIterator<QString, QProcess*> i(this->loadedModules);
  while (i.hasNext()) {
    i.next();
    this->loadedModules.value(i.key())->kill();
    this->loadedModules.remove(i.key());
  }
}

void
VMControlWidget::redrawTable()
{
  char str[16];
  memset(str, 0, 16);
  QString command;
  short disassembleIndex = 0;
  this->showProcState();
  this->vm->get_memory(this->translator->getMemory());
  for (int i = 0x0; i <= 0xFFFF / 0x10; i++) {
    for (int j = 0; j < 16; j++) {
      this->hexTable->item(i, j)->setText(
        QString().sprintf("%02X", vm->memory[i * 16 + j]));
      this->cmpTable->item(i * 16 + j, 0)
        ->setText(QString().sprintf("%02X", vm->memory[i * 16 + j]));
      if (!disassembleIndex) {
        disassembleIndex =
          this->translator->disassemble(i * 16 + j, &command) - 1;
        this->cmpTable->item(i * 16 + j, 1)->setText(command);
      } else {
        disassembleIndex--;
        this->cmpTable->item(i * 16 + j, 1)->setText("-");
      }
    }
    strncpy(str, (const char*)vm->memory + i * 16, 16);
    for (int k = 0; k < 15; k++) {
      if (!str[k]) {
        str[k] = ' ';
      }
    }
    this->hexTable->item(i, 16)->setText(str);
  }
  this->lockTable(false);
}

void
VMControlWidget::lockTable(bool state)
{
  if (state) {
    this->cmpTable->setStyleSheet("background-color:#701717;");
    this->hexTable->setStyleSheet("background-color:#701717;");
    this->cmpTable->setEnabled(false);
    this->hexTable->setEnabled(false);
  } else {
    this->cmpTable->setStyleSheet("background-color:auto;");
    this->hexTable->setStyleSheet("background-color:auto;");
    this->cmpTable->setEnabled(true);
    this->hexTable->setEnabled(true);
  }
}
