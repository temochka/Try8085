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
#include "base_converter_widget.h"

BaseConverterWidget::BaseConverterWidget(QString title, QWidget* parent)
  : QGroupBox(title, parent)
{
  // First row of widgets
  this->decToHexHexLabel = new QLabel(tr("Hex"));
  this->decToHexHexEdit = new QLineEdit;
  this->decToHexHexHLayout = new QHBoxLayout;
  this->decToHexHexHLayout->addWidget(this->decToHexHexLabel);
  this->decToHexHexHLayout->addWidget(this->decToHexHexEdit);

  this->decToHexDecLabel = new QLabel(tr("Dec"));
  this->decToHexDecEdit = new QLineEdit;
  this->decToHexDecHLayout = new QHBoxLayout;
  this->decToHexDecHLayout->addWidget(this->decToHexDecLabel);
  this->decToHexDecHLayout->addWidget(this->decToHexDecEdit);

  this->decToHexBinLabel = new QLabel(tr("Bin"));
  this->decToHexBinEdit = new QLineEdit;
  this->decToHexBinHLayout = new QHBoxLayout;
  this->decToHexBinHLayout->addWidget(this->decToHexBinLabel);
  this->decToHexBinHLayout->addWidget(this->decToHexBinEdit);

  // Second row of widgets
  this->decToHexButton = new QPushButton(tr("Go"));
  this->decToHexButtonLayout = new QHBoxLayout;
  this->decToHexButtonLayout->addWidget(this->decToHexButton);

  // Объединение строк
  this->decToHexVLayout = new QVBoxLayout;
  this->decToHexVLayout->addLayout(this->decToHexHexHLayout);
  this->decToHexVLayout->addLayout(this->decToHexDecHLayout);
  this->decToHexVLayout->addLayout(this->decToHexBinHLayout);
  this->decToHexVLayout->addLayout(this->decToHexButtonLayout);

  connect(this->decToHexHexEdit, SIGNAL(returnPressed()), this,
          SLOT(convertFromHex()));
  connect(this->decToHexDecEdit, SIGNAL(returnPressed()), this,
          SLOT(convertFromDec()));
  connect(this->decToHexBinEdit, SIGNAL(returnPressed()), this,
          SLOT(convertFromBin()));
  connect(this->decToHexButton, SIGNAL(clicked()), this,
          SLOT(convertFromHex()));

  this->setLayout(this->decToHexVLayout);
  this->setFixedWidth(this->sizeHint().width());
  this->setFixedHeight(this->sizeHint().height());
}

void
BaseConverterWidget::convertFromHex()
{
  bool ok;
  int addr = this->decToHexHexEdit->text().toInt(&ok, 16);
  if (ok) {
    this->decToHexDecEdit->setText(QString::number(addr, 10));
    this->decToHexBinEdit->setText(QString::number(addr, 2));
  }
}

void
BaseConverterWidget::convertFromDec()
{
  bool ok;
  int addr = this->decToHexDecEdit->text().toInt(&ok, 10);
  if (ok) {
    this->decToHexHexEdit->setText(QString::number(addr, 16));
    this->decToHexBinEdit->setText(QString::number(addr, 2));
  }
}

void
BaseConverterWidget::convertFromBin()
{
  bool ok;
  int addr = this->decToHexBinEdit->text().toInt(&ok, 2);
  if (ok) {
    this->decToHexHexEdit->setText(QString::number(addr, 16));
    this->decToHexDecEdit->setText(QString::number(addr, 10));
  }
}
