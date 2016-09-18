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
#ifndef TRY8085_INDICATOR_H
#define TRY8085_INDICATOR_H

#include <QDebug>
#include <QLCDNumber>
#include <QPainter>
#include <QWidget>

class Indicator : public QWidget
{
public:
  Indicator(QWidget* parent = 0);
  ~Indicator();
  QLCDNumber* lcd;
  void paintEvent(QPaintEvent* event);
  void setState(char* str);

private:
  void a(QPainter&);
  void b(QPainter&);
  void c(QPainter&);
  void d(QPainter&);
  void e(QPainter&);
  void f(QPainter&);
  void g(QPainter&);
  void h(QPainter&);

  char state[8];
};

#endif // TRY8085_INDICATOR_H
