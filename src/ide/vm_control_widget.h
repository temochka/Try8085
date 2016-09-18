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
#ifndef TRY8085_VM_CONTROL_WIDGET_H
#define TRY8085_VM_CONTROL_WIDGET_H
#include "base_converter_widget.h"
#include "connections_widget.h"
#include "flags_state_widget.h"
#include "go_to_address_widget.h"
#include "registers_state_widget.h"
#include "translator.h"
#include "ui_ide.h"
#include "vm.h"
#include "vm_thread.h"
#include <QDebug>
#include <QHeaderView>
#include <QProcess>
#include <QSharedMemory>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>

class VMControlWidget : public QWidget
{
  Q_OBJECT
  friend class IDE;

public:
  VMControlWidget(QWidget* parent = 0);
  ~VMControlWidget();
  int make(const char*);
  VM* vm;

private:
  QTabWidget* tabs;
  QWidget* cmpPage;
  QWidget* hexPage;
  QHBoxLayout* columnLayout;
  QVBoxLayout* leftColumn;
  QVBoxLayout* rightColumn;
  QVBoxLayout* cmpTableLayout;
  QVBoxLayout* hexTableLayout;
  QTableWidget* cmpTable;
  QTableWidget* hexTable;
  GoToAddressWidget* goToAddress;
  RegistersStateWidget* registersState;
  FlagsStateWidget* flagsState;
  BaseConverterWidget* decToHex;
  VMThread* vmThread;
  QHBoxLayout* stateLayout;
  QMap<QString, QProcess*> loadedModules;
  Translator* translator;

  void showProcState();

public slots:
  void lockTable(bool state);
  void closeModules();
  void updateTable(bool debug, bool exec);
  void loadModules(QMap<QString, ModuleSlotWidget>* activeModules);
  void loadMemory();
  void goToCell(int);
  QList<TranslatorError>* toTranslator(QString sourceText);
private slots:
  void redrawTable();
};

#endif // TRY8085_VM_CONTROL_WIDGET_H
