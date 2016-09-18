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
#ifndef TRY8085_IDE_H
#define TRY8085_IDE_H

#include "c80.h"
#include "connections_widget.h"
#include "editor_widget.h"
#include "errors_list_widget.h"
#include "find_dialog_widget.h"
#include "settings_widget.h"
#include "vm_control_widget.h"
#include <QCloseEvent>
#include <QDebug>
#include <QFileDialog>
#include <QListWidget>
#include <QMap>
#include <QMessageBox>
#include <QSettings>
#include <QTextCodec>
#include <QTranslator>
#include <QtWidgets/QMainWindow>

namespace Ui {
class IDE;
}

class IDE : public QMainWindow
{
  Q_OBJECT

public:
  IDE(QApplication* app, QWidget* parent = 0);
  ~IDE();
  bool loadFile(QString fileName);
  bool saveFile(QString fileName);
  void initHighlighting();

private:
  Ui::IDE* ui;
  EditorWidget* editor;
  ErrorsListWidget* errorsList;
  ConnectionsWidget* connections;
  QApplication* appHnd;
  SettingsWidget* settingsDialog;
  QVBoxLayout* editorLayout;
  QString currentFileName;
  QString currentFileNameShort;
  QString lastOpenedPath;
  QString fileInfo;
  FindDialogWidget* findDialog;
  VMControlWidget* vmControls;
  QMap<QString, ModuleSlotWidget> activeModules;
  bool debug;
  void initFileToolbar();
  void setProgramTitle(QString title);
  void clearEditor();
  void switchInterface(short mode);
  void updateErrorsList(QList<TranslatorError>* errors);
  void closeEvent(QCloseEvent* event);
  bool saveModified();
  void saveSettings();
  void loadEditToolbars(bool fl);
private slots:
  void setDebug();
  void showAboutBox();
  void showConnectionsDialog();
  void initModules();

public slots:
  void loadSettings();
  bool fileOpenRequest();
  bool fileSaveRequest();
  bool fileSaveAsRequest();
  bool fileNewRequest();
  void fileModified();
  void showFindDialog();
  void findNext(QString text, Qt::CaseSensitivity cs);
  void findPrev(QString text, Qt::CaseSensitivity cs);
  void translateAndRun();
  void toEditor();
  void step();
  void start();
  void procStopped();
  void onStartClick();
  void onStepClick();
};

#endif // TRY8085_IDE_H
