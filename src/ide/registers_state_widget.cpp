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
#include "registers_state_widget.h"
#include <iostream>

RegistersStateWidget::RegistersStateWidget(QString title, QWidget* parent)
  : QGroupBox(title, parent)
{
  this->a = new QLabel("<b>A</b>");
  this->aValue = new QLabel("00");
  this->aLayout = new QHBoxLayout;
  this->aLayout->addWidget(this->a);
  this->aLayout->addWidget(this->aValue);

  this->bc = new QLabel("<b>BC</b>");
  this->bValue = new QLabel("00");
  this->cValue = new QLabel("00");
  this->bcLayout = new QHBoxLayout;
  this->bcLayout->addWidget(this->bc);
  this->bcLayout->addWidget(this->bValue);
  this->bcLayout->addWidget(this->cValue);

  this->de = new QLabel("<b>DE</b>");
  this->dValue = new QLabel("00");
  this->eValue = new QLabel("00");
  this->deLayout = new QHBoxLayout;
  this->deLayout->addWidget(this->de);
  this->deLayout->addWidget(this->dValue);
  this->deLayout->addWidget(this->eValue);

  this->hl = new QLabel("<b>HL</b>");
  this->hlValue1 = new QLabel("00");
  this->hlValue2 = new QLabel("00");
  this->hlLayout = new QHBoxLayout;
  this->hlLayout->addWidget(this->hl);
  this->hlLayout->addWidget(this->hlValue1);
  this->hlLayout->addWidget(this->hlValue2);

  this->sp = new QLabel("<b>SP</b>");
  this->spValue1 = new QLabel("00");
  this->spValue2 = new QLabel("00");
  this->spLayout = new QHBoxLayout;
  this->spLayout->addWidget(this->sp);
  this->spLayout->addWidget(this->spValue1);
  this->spLayout->addWidget(this->spValue2);

  this->ip = new QLabel("<b>IP</b>");
  this->ipValue1 = new QLabel("00");
  this->ipValue2 = new QLabel("00");
  this->ipLayout = new QHBoxLayout;
  this->ipLayout->addWidget(this->ip);
  this->ipLayout->addWidget(this->ipValue1);
  this->ipLayout->addWidget(this->ipValue2);

  this->registersLayout = new QVBoxLayout;
  this->registersLayout->addLayout(this->aLayout);
  this->registersLayout->addLayout(this->bcLayout);
  this->registersLayout->addLayout(this->deLayout);
  this->registersLayout->addLayout(this->hlLayout);
  this->registersLayout->addLayout(this->spLayout);
  this->registersLayout->addLayout(this->ipLayout);

  this->setLayout(this->registersLayout);

  this->setFixedWidth(this->sizeHint().width() + 50);
  this->setFixedHeight(this->sizeHint().height());
}

void
RegistersStateWidget::setRegisters(short a, short b, short c, short d, short e,
                                   short h, short l, short sp, short ip)
{
  this->aValue->setText(QString().sprintf("%02X", (unsigned char)a));
  this->bValue->setText(QString().sprintf("%02X", (unsigned char)b));
  this->cValue->setText(QString().sprintf("%02X", (unsigned char)c));
  this->dValue->setText(QString().sprintf("%02X", (unsigned char)d));
  this->eValue->setText(QString().sprintf("%02X", (unsigned char)e));
  this->hlValue1->setText(QString().sprintf("%02X", (unsigned char)h));
  this->hlValue2->setText(QString().sprintf("%02X", (unsigned char)l));
  this->spValue1->setText(QString().sprintf("%02X", (unsigned char)sp / 256));
  this->spValue2->setText(QString().sprintf("%02X", (unsigned char)sp % 256));
  this->ipValue1->setText(QString().sprintf("%02X", (unsigned char)ip / 256));
  this->ipValue2->setText(QString().sprintf("%02X", (unsigned char)ip % 256));
}
