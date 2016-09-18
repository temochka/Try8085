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
#include "errors_list_widget.h"

ErrorsListWidget::ErrorsListWidget(QWidget* parent)
  : QWidget(parent)
{
  this->elLayout = new QVBoxLayout;
  this->compileInfo = new QLabel(tr("Compilation info:"));
  this->errors = new QListWidget;
  this->compilationResut = new QLabel;
  this->elLayout->addWidget(this->compileInfo);
  this->elLayout->addWidget(this->errors);
  this->elLayout->addWidget(this->compilationResut);
  this->setLayout(this->elLayout);
  this->redLines = NULL;
  connect(this->errors, SIGNAL(doubleClicked(QModelIndex)), this,
          SLOT(onSelectRedLine(QModelIndex)));
}

void
ErrorsListWidget::updateRedLines(QList<TranslatorError>* err)
{
  this->redLines = new int[err->size()];
  this->redLinesSize = err->size();
  for (int i = 0; i < err->size(); i++) {
    TranslatorError buf = err->at(i);
    this->redLines[i] = buf.getLine();
  }
}

void
ErrorsListWidget::onSelectRedLine(QModelIndex i)
{
  int index = i.row();
  emit this->redLineSelected(this->redLines[index]);
}

void
ErrorsListWidget::setCompilationResult(QString result)
{
  this->compilationResut->setText(result);
}
