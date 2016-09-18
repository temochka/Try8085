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
#include "globals.h"
#include "ide.h"
#include <QSettings>
#include <QSplashScreen>
#include <QTranslator>
#include <QtWidgets/QApplication>

int
main(int argc, char* argv[])
{
  QApplication a(argc, argv);
  QSettings settings(Globals::getSettingsOrg(), Globals::getSettingsApp());
  QTranslator translator;
  int language = settings.value("language", int(0)).toInt();
  if (language == 1) {
    translator.load("emul_ru", "lang");
  } else {
    translator.load("emul_en", "lang");
  }
  a.installTranslator(&translator);
  QSplashScreen* splash = new QSplashScreen;
  splash->setPixmap(QPixmap(":/Logo/images/splashscreen.png"));
  splash->show();
  Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
  splash->showMessage(QObject::tr("Loading main window..."), topRight,
                      Qt::black);
  IDE w(&a);
  w.loadSettings();
  splash->showMessage(QObject::tr("Loading settings..."), topRight, Qt::black);
  w.show();
  splash->finish(&w);
  delete splash;
  return a.exec();
}
