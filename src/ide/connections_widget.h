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
#ifndef TRY8085_CONNECTIONS_WIDGET_H
#define TRY8085_CONNECTIONS_WIDGET_H

#include <QCheckBox>
#include <QDialog>
#include <QLayout>
#include <QMap>
#include <QMapIterator>
#include <QPushButton>

struct ModuleSlotWidget
{
public:
  ModuleSlotWidget(QString mName, QString mId, QString fName, bool e)
    : moduleName(mName)
    , moduleId(mId)
    , fileName(fName)
    , enabled(e)
  {
  }
  ModuleSlotWidget()
    : moduleName("")
    , moduleId("")
    , fileName("")
    , enabled(false)
  {
  }
  QString moduleName;
  QString moduleId;
  QString fileName;
  bool enabled;
};

class ConnectionsWidget : public QDialog
{
  Q_OBJECT
public:
  ConnectionsWidget(QMap<QString, ModuleSlotWidget>* activeModules,
                    QWidget* parent = 0, Qt::WindowFlags f = 0);
  ~ConnectionsWidget();

private:
  void showEvent(QShowEvent* event);
  QMap<QString, QCheckBox*> neededModules;
  QPushButton* ok;
  QVBoxLayout* cl;
  bool indicators;
  QMap<QString, ModuleSlotWidget>* activeModules;
private slots:
  void saveChanges();
  void updateState();
};

#endif // TRY8085_CONNECTIONS_WIDGET_H
