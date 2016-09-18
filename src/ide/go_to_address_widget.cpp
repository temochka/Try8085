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
#include "go_to_address_widget.h"

GoToAddressWidget::GoToAddressWidget(QString title, QWidget* parent)
  : QGroupBox(title, parent)
{
  // First widgets row
  this->goToAddressLabel = new QLabel(tr("Address"));
  this->goToAddressEdit = new QLineEdit;
  this->goToAddressHLayout = new QHBoxLayout;
  this->goToAddressHLayout->addWidget(this->goToAddressLabel);
  this->goToAddressHLayout->addWidget(this->goToAddressEdit);

  // Second widgets row
  this->goToAddressButton = new QPushButton(tr("Go"));
  this->goToAddressButtonLayout = new QHBoxLayout;
  this->goToAddressButtonLayout->addWidget(this->goToAddressButton);

  // Layout
  this->goToAddressVLayout = new QVBoxLayout;
  this->goToAddressVLayout->addLayout(this->goToAddressHLayout);
  this->goToAddressVLayout->addLayout(this->goToAddressButtonLayout);

  this->setLayout(this->goToAddressVLayout);
  this->setFixedWidth(this->sizeHint().width());
  this->setFixedHeight(this->sizeHint().height());

  this->connect(this->goToAddressButton, SIGNAL(clicked()), this,
                SLOT(sendTrigger()));
  this->connect(this->goToAddressEdit, SIGNAL(returnPressed()), this,
                SLOT(sendTrigger()));
}

void
GoToAddressWidget::sendTrigger()
{
  bool ok;
  int addr = this->goToAddressEdit->text().toInt(&ok, 16);
  if (ok && addr >= 0x0 && addr <= 0xFFFF) {
    emit this->triggered(addr);
  } else {
    QMessageBox msgBox;
    msgBox.setText(tr("Bad address value!"));
    msgBox.setInformativeText(
      tr("Address value must be a hexademical number."));
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    int ret = msgBox.exec();
  }
}
