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
#include "flags_state_widget.h"

FlagsStateWidget::FlagsStateWidget(QString title, QWidget* parent)
  : QGroupBox(title, parent)
{
  this->s = new QLabel("<b>S</b>");
  this->sValue = new QLabel("0");

  this->sLayout = new QHBoxLayout;
  this->sLayout->addWidget(this->s);
  this->sLayout->addWidget(this->sValue);

  this->z = new QLabel("<b>Z</b>");
  this->zValue = new QLabel("0");
  this->zLayout = new QHBoxLayout;
  this->zLayout->addWidget(this->z);
  this->zLayout->addWidget(this->zValue);

  this->ac = new QLabel("<b>AC</b>");
  this->acValue = new QLabel("0");
  this->acLayout = new QHBoxLayout;
  this->acLayout->addWidget(this->ac);
  this->acLayout->addWidget(this->acValue);

  this->p = new QLabel("<b>P</b>");
  this->pValue = new QLabel("0");
  this->pLayout = new QHBoxLayout;
  this->pLayout->addWidget(this->p);
  this->pLayout->addWidget(this->pValue);

  this->c = new QLabel("<b>C</b>");
  this->cValue = new QLabel("0");
  this->cLayout = new QHBoxLayout;
  this->cLayout->addWidget(this->c);
  this->cLayout->addWidget(this->cValue);

  this->flagsLayout = new QVBoxLayout;
  this->flagsLayout->addLayout(this->sLayout);
  this->flagsLayout->addLayout(this->zLayout);
  this->flagsLayout->addLayout(this->acLayout);
  this->flagsLayout->addLayout(this->pLayout);
  this->flagsLayout->addLayout(this->cLayout);

  this->setLayout(this->flagsLayout);
  this->setFixedWidth(this->sizeHint().width() + 10);
}

void
FlagsStateWidget::setFlags(char s, char z, char ac, char p, char c)
{
  this->sValue->setText(QString().sprintf("%02X", s));
  this->zValue->setText(QString().sprintf("%02X", z));
  this->acValue->setText(QString().sprintf("%02X", ac));
  this->pValue->setText(QString().sprintf("%02X", p));
  this->cValue->setText(QString().sprintf("%02X", c));
}
