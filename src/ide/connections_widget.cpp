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
#include "connections_widget.h"

ConnectionsWidget::ConnectionsWidget(
  QMap<QString, ModuleSlotWidget>* activeModules, QWidget* parent,
  Qt::WindowFlags f)
  : QDialog(parent, f)
{
  this->setWindowTitle(tr("Module connection settings"));
  this->activeModules = activeModules;
  this->cl = new QVBoxLayout;
  this->ok = new QPushButton(tr("OK"));
  QMapIterator<QString, ModuleSlotWidget> i(*activeModules);
  while (i.hasNext()) {
    i.next();
    QCheckBox* buf = new QCheckBox(i.value().moduleName);
    this->neededModules.insert(i.key(), buf);
    this->neededModules.value(i.key())->setChecked(i.value().enabled);
    this->cl->addWidget(this->neededModules.value(i.key()));
  }
  this->cl->addWidget(this->ok);
  this->setLayout(this->cl);

  connect(this->ok, SIGNAL(clicked()), this, SLOT(saveChanges()));
  connect(this->ok, SIGNAL(clicked()), this, SLOT(close()));
}

ConnectionsWidget::~ConnectionsWidget()
{
}

void
ConnectionsWidget::saveChanges()
{
  QMapIterator<QString, QCheckBox*> i(this->neededModules);
  while (i.hasNext()) {
    i.next();
    ModuleSlotWidget buf = activeModules->value(i.key());
    buf.enabled = i.value()->isChecked();
    activeModules->insert(i.key(), buf);
  }
}

void
ConnectionsWidget::updateState()
{
  QMapIterator<QString, ModuleSlotWidget> i(*activeModules);
  while (i.hasNext()) {
    i.next();
    this->neededModules.value(i.key())->setChecked(i.value().enabled);
  }
}

void
ConnectionsWidget::showEvent(QShowEvent* event)
{
  this->updateState();
}
