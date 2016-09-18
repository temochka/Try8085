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
#include "indicator.h"

Indicator::Indicator(QWidget* parent)
  : QWidget(parent)
{
  this->setFixedWidth(250);
  this->setFixedHeight(200);
  memset(this->state, 0, 8);
  this->show();
}

Indicator::~Indicator()
{
}

void
Indicator::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
  painter.setPen(QPen(Qt::white, 2, Qt::SolidLine));
  painter.drawRect(QRect(0, 0, 110, 195));

  painter.setPen(QPen(Qt::green, 1, Qt::SolidLine));

  if (!this->state[0]) {
    painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
  } else {
    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
  }
  this->a(painter);
  if (!this->state[1]) {
    painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
  } else {
    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
  }
  this->b(painter);
  if (!this->state[2]) {
    painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
  } else {
    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
  }
  this->c(painter);
  if (!this->state[3]) {
    painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
  } else {
    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
  }
  this->d(painter);
  if (!this->state[4]) {
    painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
  } else {
    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
  }
  this->e(painter);
  if (!this->state[5]) {
    painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
  } else {
    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
  }
  this->f(painter);
  if (!this->state[6]) {
    painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
  } else {
    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
  }
  this->g(painter);
  if (!this->state[7]) {
    painter.setBrush(QBrush(Qt::black, Qt::SolidPattern));
  } else {
    painter.setBrush(QBrush(Qt::green, Qt::SolidPattern));
  }
  this->h(painter);
}

void
Indicator::a(QPainter& painter)
{
  painter.drawRect(15, 5, 70, 10);
}

void
Indicator::b(QPainter& painter)
{
  painter.drawRect(85, 15, 10, 75);
}

void
Indicator::c(QPainter& painter)
{
  painter.drawRect(85, 100, 10, 75);
}

void
Indicator::d(QPainter& painter)
{
  painter.drawRect(15, 175, 70, 10);
}

void
Indicator::e(QPainter& painter)
{
  painter.drawRect(5, 100, 10, 75);
}

void
Indicator::f(QPainter& painter)
{
  painter.drawRect(5, 15, 10, 75);
}

void
Indicator::g(QPainter& painter)
{
  painter.drawRect(15, 90, 70, 10);
}

void
Indicator::h(QPainter& painter)
{
  painter.drawRect(96, 175, 10, 10);
}

void
Indicator::setState(char* str)
{
  memcpy(this->state, str, 8);
}
