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
#ifndef TRY8085_C80_H
#define TRY8085_C80_H
#include "connections_widget.h"
#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QDomNodeList>
#include <QFile>
#include <QMap>
#include <QMapIterator>

class C80
{
public:
  C80(QString fileName);
  ~C80();
  QString getSource();
  void getModules(QMap<QString, ModuleSlotWidget>* activeModules);
  void setSource(QString fileName, QString source,
                 QMap<QString, ModuleSlotWidget>* activeModules);

private:
  QDomDocument* c80;
  QFile* input;
  QFile* output;
};

#endif // TRY8085_C80_H
