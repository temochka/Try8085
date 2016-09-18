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
#include "indicators.h"

Indicators::Indicators(QWidget* parent)
  : QWidget(parent)
  , sharedMemory("try8085_ports")
{
  this->setWindowTitle(tr("Try8085 Indicators"));
  this->setWindowIcon(QIcon(":/Icons/icons/8_64x64.png"));

  this->status = new QLabel(this);
  this->handler = new QHBoxLayout;
  this->indicators = new Indicator*[this->N];
  for (int i = 0; i < this->N; i++) {
    this->indicators[i] = new Indicator(this);
    this->handler->addWidget(this->indicators[i]);
  }
  this->setLayout(this->handler);
  this->setFixedWidth(880);
  this->setFixedHeight(210);
  this->show();

  if (this->sharedMemory.isAttached()) {
    this->sharedMemory.detach();
  }
  memset(this->state, 0, 8);
  this->startTimer(50);
}

Indicators::~Indicators()
{
}

void
Indicators::updateIndicators()
{
  for (int i = 0; i < this->N; i++) {
    char buf[8];
    buf[0] = this->state[i] & 128;
    buf[1] = this->state[i] & 64;
    buf[2] = this->state[i] & 32;
    buf[3] = this->state[i] & 16;
    buf[4] = this->state[i] & 8;
    buf[5] = this->state[i] & 4;
    buf[6] = this->state[i] & 2;
    buf[7] = this->state[i] & 1;
    this->indicators[i]->setState(buf);
    this->indicators[i]->repaint(
      QRect(0, 0, this->indicators[i]->width(), this->indicators[i]->height()));
  }
}

void
Indicators::timerEvent(QTimerEvent* event)
{
  if (!this->sharedMemory.attach()) {
    QMessageBox::warning(this, tr("Try8085 Indicators"),
                         tr("Unable to attach to shared memory segment.\n"
                            "Load Try8085 first."),
                         QMessageBox::Ok);
    return;
  }

  this->sharedMemory.lock();
  memcpy(this->state, (unsigned char*)this->sharedMemory.data() + 0x81, 8);
  this->sharedMemory.unlock();

  sharedMemory.detach();
  this->updateIndicators();
}

void
Indicators::closeEvent(QCloseEvent* event)
{
  event->ignore();
}
