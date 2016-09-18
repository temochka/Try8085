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
#include "vm_thread.h"

VMThread::VMThread(VM* proc, QObject* parent)
  : QThread(parent)
{
  this->proc = proc;
  this->action = NOTHING;
  this->sleepTime = SLEEPTIME;
}

void
VMThread::step()
{
  proc->exec();
  emit stoped();
}

void
VMThread::exec()
{
  action = EXEC;
}

void
VMThread::run()
{
  forever
  {
    msleep(this->sleepTime);
    switch (this->action) {
      case EXEC:
        if (!proc->exec()) {
          this->action = NOTHING;
          emit stoped();
        }
        break;
      default:
        break;
    }
  }
}

void
VMThread::pauseExecution()
{
  this->action = NOTHING;
  emit stoped();
}

short
VMThread::getState()
{
  return this->action;
}

void
VMThread::setSleepTime(int sTime)
{
  this->sleepTime = sTime;
}
