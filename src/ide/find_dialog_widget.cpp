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
#include "find_dialog_widget.h"

FindDialogWidget::FindDialogWidget(QWidget* parent)
  : QDialog(parent)
{
  label = new QLabel(tr("Find &what:"));
  lineEdit = new QLineEdit;
  label->setBuddy(lineEdit);

  caseCheckBox = new QCheckBox(tr("Case &sensetive"));
  backwardCheckBox = new QCheckBox(tr("Backward"));

  findButton = new QPushButton(tr("&Find"));
  findButton->setDefault(true);
  findButton->setEnabled(false);

  closeButton = new QPushButton(tr("Close"));

  connect(lineEdit, SIGNAL(textChanged(const QString&)), this,
          SLOT(enableFindButton(const QString&)));
  connect(findButton, SIGNAL(clicked()), this, SLOT(findClicked()));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));

  QHBoxLayout* topLeftLayout = new QHBoxLayout;
  topLeftLayout->addWidget(label);
  topLeftLayout->addWidget(lineEdit);

  QVBoxLayout* leftLayout = new QVBoxLayout;
  leftLayout->addLayout(topLeftLayout);
  leftLayout->addWidget(caseCheckBox);
  leftLayout->addWidget(backwardCheckBox);

  QVBoxLayout* rightLayout = new QVBoxLayout;
  rightLayout->addWidget(findButton);
  rightLayout->addWidget(closeButton);
  rightLayout->addStretch();

  QHBoxLayout* mainLayout = new QHBoxLayout;
  mainLayout->addLayout(leftLayout);
  mainLayout->addLayout(rightLayout);
  setLayout(mainLayout);

  setWindowTitle(tr("Find..."));
  setFixedHeight(sizeHint().height());
}

void
FindDialogWidget::findClicked()
{
  QString text = lineEdit->text();
  Qt::CaseSensitivity cs =
    caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
  if (backwardCheckBox->isChecked()) {
    emit findPrev(text, cs);
  } else {
    emit findNext(text, cs);
  }
}

void
FindDialogWidget::enableFindButton(const QString& text)
{
  findButton->setEnabled(!text.isEmpty());
}
