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
#include "c80.h"
#include <QDebug>
#include <iostream>

C80::C80(QString fileName)
{
  this->c80 = new QDomDocument;
  this->input = new QFile(fileName);
  if (this->input->open(QFile::ReadOnly)) {
    this->c80->setContent(this->input);
  }
}

QString
C80::getSource()
{
  QDomNodeList nodes = this->c80->elementsByTagName("file");
  QDomNode node = nodes.at(0);
  QDomElement source = node.firstChildElement("source");
  if (!source.isNull()) {
    return source.text();
  }
  return "";
}

void
C80::getModules(QMap<QString, ModuleSlotWidget>* activeModules)
{
  QDomNodeList nodes = this->c80->elementsByTagName("module");
  for (int i = 0; i < nodes.size(); i++) {
    QDomElement node = nodes.at(i).toElement();
    QString key = node.attribute("id", "");
    if (key != "") {
      bool ok;
      if (activeModules->contains(key)) {
        ModuleSlotWidget buf = activeModules->value(key);
        buf.enabled = node.text().toInt(&ok, 2);
        activeModules->insert(key, buf);
      }
    }
  }
}

void
C80::setSource(QString fileName, QString source,
               QMap<QString, ModuleSlotWidget>* activeModules)
{
  this->output = new QFile(fileName);
  if (this->output->open(QFile::WriteOnly)) {
    QString ret;
    ret += "<?xml version='1.0' encoding='utf-8' ?><file>";
    QMapIterator<QString, ModuleSlotWidget> i(*activeModules);
    while (i.hasNext()) {
      i.next();
      ret += QString().sprintf("<module id='%s'>%d</module>",
                               i.key().toStdString().c_str(),
                               (int)i.value().enabled);
    }
    ret += "<source><![CDATA[";
    ret += source;
    ret += "]]></source></file>";
    this->output->write(ret.toLatin1());
    this->output->close();
  }
}

C80::~C80()
{
  this->input->close();
}
