/*
  Copyright (C) 2009, 2016-2017  Artem Chistyakov <chistyakov.artem@gmail.com>
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
#include "ide.h"
#include "ui_ide.h"
#define EDITOR_UI 0
#define VM_UI 1

IDE::IDE(QApplication* app, QWidget* parent)
  : QMainWindow(parent)
  , ui(new Ui::IDE)
{
  ui->setupUi(this);
  this->setWindowIcon(QIcon(":/Logo/images/icon32x32.png"));

  // Editor
  this->editorLayout = new QVBoxLayout;
  this->editor = new EditorWidget();
  this->errorsList = new ErrorsListWidget;
  this->errorsList->setFixedHeight(128);
  this->editorLayout->addWidget(this->editor);
  this->editorLayout->addWidget(this->errorsList);
  this->ui->mainLayout->addLayout(this->editorLayout);
  this->editor->hide();
  this->errorsList->hide();

  // Connections
  this->initModules();
  this->connections = new ConnectionsWidget(&this->activeModules, this);

  // Defaults
  this->fileInfo = "";
  this->lastOpenedPath = ".";
  this->findDialog = NULL;
  this->debug = false;
  this->currentFileName = "";

  // VM control widgets
  this->vmControls = new VMControlWidget();
  this->vmControls->hide();
  this->ui->mainLayout->addWidget(this->vmControls);

  // Settings
  this->settingsDialog = new SettingsWidget(this);
  connect(this->settingsDialog, SIGNAL(settingsUpdated()), this,
          SLOT(loadSettings()));

  // Signals
  connect(ui->menuExit, SIGNAL(triggered()), this, SLOT(close()));
  connect(ui->menuOpen, SIGNAL(triggered()), this, SLOT(fileOpenRequest()));
  connect(ui->menuSave, SIGNAL(triggered()), this, SLOT(fileSaveRequest()));
  connect(ui->menuSaveAs, SIGNAL(triggered()), this, SLOT(fileSaveAsRequest()));
  connect(ui->menuUndo, SIGNAL(triggered()), this->editor, SLOT(undo()));
  connect(ui->menuRedo, SIGNAL(triggered()), this->editor, SLOT(redo()));
  connect(ui->menuCopy, SIGNAL(triggered()), this->editor, SLOT(copy()));
  connect(ui->menuPaste, SIGNAL(triggered()), this->editor, SLOT(paste()));
  connect(ui->menuCut, SIGNAL(triggered()), this->editor, SLOT(cut()));
  connect(ui->menuSelectAll, SIGNAL(triggered()), this->editor,
          SLOT(selectAll()));
  connect(ui->menuFind, SIGNAL(triggered()), this, SLOT(showFindDialog()));
  connect(ui->menuCompile, SIGNAL(triggered()), this, SLOT(translateAndRun()));
  connect(ui->menuToEditor, SIGNAL(triggered()), this, SLOT(toEditor()));
  connect(ui->menuNew, SIGNAL(triggered()), this, SLOT(fileNewRequest()));
  connect(ui->menuNew, SIGNAL(triggered()), this->editor, SLOT(show()));
  connect(ui->menuNew, SIGNAL(triggered()), this->errorsList, SLOT(show()));
  connect(this->editor, SIGNAL(textChanged()), this, SLOT(fileModified()));
  connect(ui->menuStep, SIGNAL(triggered()), this, SLOT(setDebug()));
  connect(ui->menuStep, SIGNAL(triggered()), this, SLOT(step()));
  connect(ui->menuStep, SIGNAL(triggered(bool)), this, SLOT(onStepClick()));
  connect(ui->menuAbout, SIGNAL(triggered()), this, SLOT(showAboutBox()));
  connect(ui->menuConnections, SIGNAL(triggered()), this,
          SLOT(showConnectionsDialog()));
  connect(this->errorsList, SIGNAL(redLineSelected(int)), this->editor,
          SLOT(goToLine(int)));
  connect(this->ui->menuStop, SIGNAL(triggered()), this->vmControls->vmThread,
          SLOT(pauseExecution()));
  connect(this->ui->menuStop, SIGNAL(triggered(bool)), this->ui->menuStop,
          SLOT(setEnabled(bool)));
  connect(this->ui->menuStart, SIGNAL(triggered()), this->vmControls->vmThread,
          SLOT(exec()));
  connect(this->ui->menuStart, SIGNAL(triggered(bool)), this,
          SLOT(onStartClick()));
  connect(this->vmControls->vmThread, SIGNAL(stoped()), this,
          SLOT(procStopped()));
  connect(this->ui->menuSettings, SIGNAL(triggered()), this->settingsDialog,
          SLOT(show()));

  // Toolbar
  this->initFileToolbar();
  this->ui->menuSave->setDisabled(true);
}

IDE::~IDE()
{
  delete ui;
}

bool
IDE::fileNewRequest()
{
  if (!this->saveModified()) {
    return false;
  }
  this->ui->menuConnections->setEnabled(true);
  this->currentFileName = "";
  this->initModules();
  this->clearEditor();
  this->loadEditToolbars(false);
  return true;
}

bool
IDE::fileOpenRequest()
{
  QString fileName =
    QFileDialog::getOpenFileName(this, tr("Open File"), this->lastOpenedPath,
                                 tr("Try8085 Source Files (*.c80)"));
  if (fileName.isEmpty())
    return false;

  if (!this->saveModified()) {
    return false;
  }

  QFileInfo finfo(fileName);

  this->lastOpenedPath = finfo.filePath();

  return loadFile(fileName);
}

bool
IDE::fileSaveRequest()
{
  if (this->currentFileName.isEmpty()) {
    return this->fileSaveAsRequest();
  }

  return saveFile(this->currentFileName);
}

bool
IDE::fileSaveAsRequest()
{
  QString fileName = QFileDialog::getSaveFileName(
    this, tr("Save File As..."), this->lastOpenedPath,
    tr("Try8085 Source Files (*.c80)"));
  if (fileName.isEmpty())
    return false;

  if (this->currentFileName != fileName) {
    QFileInfo finfo(fileName);
    this->setProgramTitle(finfo.fileName());
    this->currentFileName = fileName;
    this->currentFileNameShort = finfo.fileName();
  }

  return saveFile(fileName);
}

bool
IDE::loadFile(QString fileName)
{
  C80 f(fileName);

  QFileInfo finfo(fileName);
  this->loadEditToolbars(false);
  this->setProgramTitle(finfo.fileName());
  this->currentFileName = fileName;
  this->currentFileNameShort = finfo.fileName();
  this->errorsList->setCompilationResult("");

  QString source = f.getSource();
  this->editor->setPlainText(source);

  this->initModules();
  f.getModules(&this->activeModules);
  this->ui->menuSave->setDisabled(true);
  this->ui->menuConnections->setEnabled(true);
  if (this->editor->isHidden()) {
    this->switchInterface(EDITOR_UI);
  }
  return true;
}

bool
IDE::saveFile(QString fileName)
{
  C80 f(fileName);
  f.setSource(fileName, this->editor->toPlainText(), &this->activeModules);
  this->ui->menuSave->setEnabled(false);
  return true;
}

void
IDE::initFileToolbar()
{
  ui->fileToolBar->addAction(ui->menuNew);
  ui->fileToolBar->addAction(ui->menuOpen);
  ui->fileToolBar->addAction(ui->menuSave);
  ui->fileToolBar->addSeparator();
  ui->fileToolBar->addAction(ui->menuCut);
  ui->fileToolBar->addAction(ui->menuCopy);
  ui->fileToolBar->addAction(ui->menuPaste);
  ui->fileToolBar->addSeparator();
  ui->fileToolBar->addAction(ui->menuUndo);
  ui->fileToolBar->addAction(ui->menuRedo);
  ui->fileToolBar->addSeparator();
  ui->fileToolBar->addAction(ui->menuCompile);
  ui->fileToolBar->addAction(ui->menuStop);
}

void
IDE::showFindDialog()
{
  if (!this->findDialog) {
    this->findDialog = new FindDialogWidget;
    connect(this->findDialog, SIGNAL(findNext(QString, Qt::CaseSensitivity)),
            this, SLOT(findNext(QString, Qt::CaseSensitivity)));
    connect(this->findDialog, SIGNAL(findPrev(QString, Qt::CaseSensitivity)),
            this, SLOT(findPrev(QString, Qt::CaseSensitivity)));
  }
  this->findDialog->show();
  this->findDialog->activateWindow();
}

void
IDE::findNext(QString text, Qt::CaseSensitivity cs)
{
  if (cs == Qt::CaseSensitive) {
    this->editor->find(text, QTextDocument::FindCaseSensitively);
  } else {
    this->editor->find(text);
  }
}

void
IDE::findPrev(QString text, Qt::CaseSensitivity cs)
{
  if (cs == Qt::CaseSensitive) {
    this->editor->find(text, QTextDocument::FindCaseSensitively |
                               QTextDocument::FindBackward);
  } else {
    this->editor->find(text, QTextDocument::FindBackward);
  }
}

void
IDE::setProgramTitle(QString title)
{
  this->setWindowTitle("Try8085 — " + title);
}

void
IDE::clearEditor()
{
  this->editor->clear();
  this->setProgramTitle(tr("New File"));
}

void
IDE::translateAndRun()
{
  QList<TranslatorError>* err =
    this->vmControls->toTranslator(this->editor->toPlainText());
  if (!err) {
    this->errorsList->errors->clear();
    this->switchInterface(VM_UI);
  } else {
    this->updateErrorsList(err);
  }
}

void
IDE::toEditor()
{
  this->switchInterface(EDITOR_UI);
}

void
IDE::switchInterface(short mode)
{
  if (mode == VM_UI) {
    this->vmControls->loadModules(&this->activeModules);
    this->editor->hide();
    this->errorsList->hide();
    this->vmControls->show();
    this->vmControls->loadMemory();
    this->vmControls->updateTable(this->debug, !this->debug);
    this->loadEditToolbars(true);
    if (!this->debug) {
      this->ui->menuStop->setDisabled(false);
    }
    this->ui->menuStart->setDisabled(false);
  } else {
    if (this->vmControls->vmThread->getState()) {
      this->vmControls->vmThread->pauseExecution();
    }
    this->vmControls->closeModules();
    this->vmControls->vm->clear_flags();
    this->vmControls->vm->clear_registers();
    this->loadEditToolbars(false);
    this->editor->show();
    this->errorsList->show();
    this->vmControls->hide();
    this->debug = false;
  }
}

void
IDE::fileModified()
{
  if (!this->ui->menuSave->isEnabled() && !this->editor->isHidden()) {
    this->ui->menuSave->setEnabled(true);
  }
}

void
IDE::updateErrorsList(QList<TranslatorError>* errors)
{
  this->errorsList->errors->clear();
  if (errors->size()) {
    this->errorsList->setCompilationResult(
      QString().sprintf("Compilation failed with %d errors.", errors->size()));
  } else {
    this->errorsList->setCompilationResult(
      tr("Compilation successfully finished."));
  }
  for (int i = 0; i < errors->size(); i++) {
    TranslatorError err = errors->at(i);
    QString buf = QString().sprintf("Line %d: %s", err.getLine(),
                                    err.getDescription().toStdString().c_str());
    this->errorsList->errors->addItem(new QListWidgetItem(
      QIcon(":/Actions/tango/10x10/actions/close.png"), buf));
  }
  this->errorsList->updateRedLines(errors);
}

void
IDE::setDebug()
{
  this->debug = true;
}

void
IDE::step()
{
  if (!this->editor->isHidden()) {
    this->translateAndRun();
  }
  this->vmControls->updateTable(this->debug, true);
}

void
IDE::start()
{
  this->vmControls->updateTable(this->debug, true);
  this->ui->menuStart->setEnabled(false);
}

bool
IDE::saveModified()
{
  if (!this->ui->menuSave->isEnabled()) {
    return true;
  } else {
    QMessageBox msgBox;
    msgBox.setText(tr("The document has been modified since last save."));
    msgBox.setInformativeText(
      tr("Do you want to exit without saving your changes?"));
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard |
                              QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    switch (ret) {
      case QMessageBox::Save:
        ui->menuSave->trigger();
      case QMessageBox::Discard:
        return true;
      case QMessageBox::Cancel:
        break;
      default:
        break;
    }
  }
  return false;
}

void
IDE::closeEvent(QCloseEvent* event)
{
  if (this->saveModified()) {
    event->accept();
  } else {
    event->ignore();
  }
  this->saveSettings();
  this->vmControls->closeModules();
}

void
IDE::saveSettings()
{
  QSettings settings(Globals::getSettingsOrg(), Globals::getSettingsApp());
  settings.setValue("geometry", this->geometry());
  settings.setValue("maximized", this->isMaximized());
}

void
IDE::loadSettings()
{
  QSettings settings(Globals::getSettingsOrg(), Globals::getSettingsApp());
  QRect rect = settings.value("geometry", QRect(200, 200, 500, 700)).toRect();
  if (!this->isMaximized()) {
    this->move(rect.topLeft());
    this->resize(rect.size());
  }

  this->vmControls->vmThread->setSleepTime(
    settings.value("sleeptime", int(10)).toInt());

  bool fullscreen = settings.value("maximized", false).toBool();
  if (fullscreen) {
    this->showMaximized();
  }

  QFont font(settings.value("font-family", QString("CourierNew")).toString(),
             settings.value("font-size", int(12)).toInt());
  this->editor->setFont(font);

  this->editor->setTabStopWidth(settings.value("tabsize", 4).toInt() *
                                this->editor->fontMetrics().width('M'));
}

void
IDE::showAboutBox()
{
  QMessageBox::about(
    this, tr("About Try8085"),
    tr("<h2>Try8085 (" GIT_VERSION ")</h2>"
       "<p>Copyright &copy; 2009-2017 Artem Chistyakov, Ilya Beda, Daniil "
       "Zapyatoy</p>"
       "<p>"
       "Try8085 is free software distributed under "
       "<a href=\"http://www.gnu.org/licenses/\">GNU General Public License "
       "v3</a>. See attached COPYING file for additional details."
       "</p>"
       "<h3>Special thanks</h3>"
       "<p>Project Supervisor - Ikonnikov A. V., PhD, The Department of "
       "Computing Hardware</p>"
       "<p>Institute of Space and Computer Technologies of Siberian Federal "
       "University, Russia, Krasnoyarsk</p>"));
}

void
IDE::initModules()
{
  this->activeModules.insert(
    "indicators", ModuleSlotWidget(tr("Block of 8-segment indicators"),
                                   "indicators", indicatorsModulePath, false));
  this->activeModules.insert(
    "display", ModuleSlotWidget(tr("Text display and keyboard"), "display",
                                displayModulePath, false));
  this->activeModules.insert(
    "keyboard", ModuleSlotWidget(tr("Digital keyboard"), "keyboard",
                                 keyboardModulePath, false));
  this->activeModules.insert(
    "adc", ModuleSlotWidget(tr("ADC/DAC"), "adc", adcModulePath, false));
}

void
IDE::showConnectionsDialog()
{
  this->connections->show();
  this->fileModified();
}

void
IDE::loadEditToolbars(bool fl)
{
  this->ui->menuStep->setEnabled(true);
  this->ui->menuToEditor->setDisabled(!fl);
  this->ui->menuCut->setDisabled(fl);
  this->ui->menuCopy->setDisabled(fl);
  this->ui->menuPaste->setDisabled(fl);
  this->ui->menuSelectAll->setDisabled(fl);
  this->ui->menuFind->setDisabled(fl);
  this->ui->menuUndo->setDisabled(fl);
  this->ui->menuRedo->setDisabled(fl);
  this->ui->menuCompile->setDisabled(fl);
  this->ui->menuStop->setDisabled(!fl);
  this->ui->menuStart->setDisabled(!fl);
}

void
IDE::procStopped()
{
  this->ui->menuStop->setDisabled(true);
}

void
IDE::onStartClick()
{
  this->ui->menuStop->setDisabled(false);
  this->vmControls->lockTable(true);
}

void
IDE::onStepClick()
{
  this->ui->menuStart->setEnabled(true);
}

#ifdef MAC_OS_X
const char* IDE::adcModulePath = "../Resources/adc.app/Contents/MacOS/adc";
const char* IDE::displayModulePath =
  "../Resources/display.app/Contents/MacOS/display";
const char* IDE::indicatorsModulePath =
  "../Resources/indicators.app/Contents/MacOS/indicators";
const char* IDE::keyboardModulePath =
  "../Resources/keyboard.app/Contents/MacOS/keyboard";
#else
const char* IDE::adcModulePath = "module/adc";
const char* IDE::displayModulePath = "module/display";
const char* IDE::indicatorsModulePath = "module/indicators";
const char* IDE::keyboardModulePath = "module/keyboard";
#endif
