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
#ifndef TRY8085_DIGITALKEYBOARD_H
#define TRY8085_DIGITALKEYBOARD_H

#include <QCloseEvent>
#include <QDebug>
#include <QDialog>
#include <QEvent>
#include <QHBoxLayout>
#include <QIcon>
#include <QMessageBox>
#include <QPushButton>
#include <QSharedMemory>
#include <QVBoxLayout>

class DigitalKeyboard : public QDialog
{
  Q_OBJECT

public:
  DigitalKeyboard(QWidget* parent = 0);
  ~DigitalKeyboard();

private:
  // Interface
  QPushButton* _c;
  QPushButton* _div;
  QPushButton* _mul;
  QPushButton* _sub;
  QHBoxLayout* _first;

  QHBoxLayout* _second;
  QPushButton* _seven;
  QPushButton* _eight;
  QPushButton* _nine;

  QHBoxLayout* _third;
  QPushButton* _four;
  QPushButton* _five;
  QPushButton* _six;

  QVBoxLayout* _grv1;
  QHBoxLayout* _grh1;
  QVBoxLayout* _grv2;
  QHBoxLayout* _grh2;

  QHBoxLayout* _fourth;
  QPushButton* _one;
  QPushButton* _two;
  QPushButton* _three;

  QHBoxLayout* _fifth;
  QPushButton* _mode;
  QPushButton* _zero;
  QPushButton* _dot;

  QVBoxLayout* _ml;

  QPushButton* _add;
  QPushButton* _equ;

  // System
  QSharedMemory sharedPorts;
  QSharedMemory sharedInterrupts;

  void clearMemory();
  void interrupt(unsigned char keycode);
  void closeEvent(QCloseEvent*);

  short portKeyboard;
  short numInterrupt;
  short handlerAddress;
  unsigned char translate(char);
private slots:
  void c_c();
  void c_div();
  void c_mul();
  void c_sub();
  void c_add();
  void c_equ();
  void c_dot();
  void c_zero();
  void c_one();
  void c_two();
  void c_three();
  void c_four();
  void c_five();
  void c_six();
  void c_seven();
  void c_eight();
  void c_nine();
  void switchMode();
};

#endif // TRY8085_DIGITALKEYBOARD_H
