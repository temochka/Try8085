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
#ifndef TRY8085_ERRORS_LIST_WIDGET_H
#define TRY8085_ERRORS_LIST_WIDGET_H

#include "translator.h"
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QModelIndex>

class ErrorsListWidget : public QWidget
{
  Q_OBJECT
public:
  ErrorsListWidget(QWidget* parent = 0);
  void updateRedLines(QList<TranslatorError>* err);
  void setCompilationResult(QString result);
  QListWidget* errors;

private:
  int* redLines;
  int redLinesSize;
  QLabel* compileInfo;
  QLabel* compilationResut;
  QVBoxLayout* elLayout;
private slots:
  void onSelectRedLine(QModelIndex);
signals:
  void redLineSelected(int);
};

#endif // TRY8085_ERRORS_LIST_WIDGET_H
