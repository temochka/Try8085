/*
  Copyright (C) 2009, 2016  Artem Chistyakov <chistyakov.artem@gmail.com>

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
#ifndef DISPLAY_H
#define DISPLAY_H

#include <QDebug>
#include <QHBoxLayout>
#include <QIcon>
#include <QLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSharedMemory>
#include <QtWidgets/QDialog>

class Display : public QDialog
{
  Q_OBJECT

public:
  Display(QWidget* parent = 0);
  ~Display();

private:
  static const int SIZE = 640;
  int portKeyboard;
  int portDisplay;
  int memoryStartAddress;
  unsigned char colorPort;
  QPlainTextEdit screen;
  QHBoxLayout hnd;
  QSharedMemory sharedMemory;
  QSharedMemory sharedPorts;
  QSharedMemory sharedInterrupts;
  void timerEvent(QTimerEvent* event);
  void closeEvent(QCloseEvent* event);
  QString getColor(char cl);
  void keyPressEvent(QKeyEvent* event);
  void updateScreen(unsigned char* memory);
  void freeSharedMemory();

  enum DEFAULT_ADDRESSES
  {
    DefaultVideo = 0x0300,
    DefaultKeyboard = 0x10,
    DefaultDisplay = 0x11
  };
public slots:
  void setColor(unsigned char color);
};

#endif // DISPLAY_H
