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
#ifndef TRY8085_VM_THREAD_H
#define TRY8085_VM_THREAD_H

#include "vm.h"
#include <QDebug>
#include <QThread>
#define EXEC 1
#define NOTHING 0

#define SLEEPTIME 10

class VMThread : public QThread
{
  Q_OBJECT
public:
  VMThread(VM* proc, QObject* parent = 0);
  short getState();

public slots:
  void step();
  void exec();
  void pauseExecution();
  void setSleepTime(int);

protected:
  void run();

private:
  VM* proc;
  int action;
  int sleepTime;
signals:
  void stoped();
};

#endif // TRY8085_VM_THREAD_H
