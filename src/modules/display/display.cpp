/*
  Copyright (C) 2009, 2016  Artem Chistyakov <chistyakov.artem@gmail.com>

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
#include "display.h"

Display::Display(QWidget* parent)
  : QDialog(parent)
  , sharedMemory("try8085_memory")
  , sharedPorts("try8085_ports")
  , sharedInterrupts("try8085_interrupts")
{
  this->setWindowTitle(tr("Try8085 Display"));
  this->setWindowIcon(QIcon(":/Icons/icons/display_64x64.png"));
  this->freeSharedMemory();
  this->hnd.addWidget(&this->screen);
  this->screen.setReadOnly(true);
  this->screen.setStyleSheet(
    "font-family: Courier New, monospace;font-size: 11pt;");
  this->setLayout(&this->hnd);
  this->screen.setFixedWidth(510);
  this->screen.setFixedHeight(135);

  this->portKeyboard = Display::DefaultKeyboard;
  this->portDisplay = Display::DefaultDisplay;
  this->memoryStartAddress = Display::DefaultVideo;

  this->colorPort = 0x0;
  this->startTimer(100);
}

Display::~Display()
{
  this->freeSharedMemory();
}

void
Display::timerEvent(QTimerEvent* event)
{
  this->freeSharedMemory();
  if (!this->sharedMemory.attach() || !this->sharedPorts.attach()) {
    QMessageBox::warning(this, tr("Try8085 Display"),
                         tr("Unable to attach to shared memory segment.\n"
                            "Load Try8085 first or try to restart."),
                         QMessageBox::Ok);
    return;
  }

  unsigned char buf[this->SIZE];
  memset(buf, 0, this->SIZE);
  this->sharedMemory.lock();
  memcpy(buf, (unsigned char*)this->sharedMemory.data() + Display::DefaultVideo,
         this->SIZE);
  this->sharedMemory.unlock();
  this->sharedMemory.detach();

  this->sharedPorts.lock();
  this->colorPort =
    *((unsigned char*)this->sharedPorts.data() + this->portDisplay);
  this->sharedPorts.unlock();
  this->sharedPorts.detach();

  this->updateScreen(buf);
  this->setColor(this->colorPort);
}

void
Display::closeEvent(QCloseEvent* event)
{
  event->ignore();
}

void
Display::setColor(unsigned char color)
{
  char foreground = color >> 4;
  char background = color & 15;
  this->screen.setStyleSheet(
    QString().sprintf("background-color: %s;"
                      "color: %s;",
                      this->getColor(background).toStdString().c_str(),
                      this->getColor(foreground).toStdString().c_str()));
}

QString
Display::getColor(char cl)
{
  switch (cl) {
    case 0:
      return "#000000";
    case 1:
      return "#0000FF";
    case 2:
      return "#00FF00";
    case 3:
      return "#99CCFF";
    case 4:
      return "#FF0000";
    // purple
    case 5:
      return "660099";
    case 6:
      return "#FFFF00";
    case 7:
      return "#EEEEEE";
    case 8:
      return "#999999";
    case 9:
      return "#3333CC";
    // light green
    case 10:
      return "#99CC99";
    // light blue
    case 11:
      return "#CCFFFF";
    // light red
    case 12:
      return "#FF6666";
    // light purple
    case 13:
      return "#CC99FF";
    // light yellow
    case 14:
      return "#FFFFCC";
    // white
    case 15:
      return "#FFFFFF";
    default:
      return "";
  }
}

void
Display::keyPressEvent(QKeyEvent* event)
{
  this->freeSharedMemory();
  if (!this->sharedPorts.attach() || !this->sharedInterrupts.attach()) {
    int ret =
      QMessageBox::warning(this, tr("Try8085 Display"),
                           tr("Unable to attach to shared memory segment.\n"
                              "Load Try8085 first or try to restart."),
                           QMessageBox::Ok);
    return;
  }

  this->sharedInterrupts.lock();
  *((unsigned char*)this->sharedInterrupts.data() + 2) = 1;
  this->sharedInterrupts.unlock();
  this->sharedInterrupts.detach();

  char* ptr = (char*)event->text().toStdString().c_str();
  this->sharedPorts.lock();
  *((unsigned char*)this->sharedPorts.data() + this->portKeyboard) = *ptr;
  this->sharedPorts.unlock();
  this->sharedInterrupts.detach();
}

void
Display::updateScreen(unsigned char* memory)
{
  QString str("");
  for (int i = 0; i < this->SIZE - 1; i++) {
    if (!(i % 80) && i > 0) {
      str += QString().sprintf("%s\n", memory + i - 80);
    }
    if (!memory[i]) {
      memory[i] = ' ';
    }
  }
  str += QString().sprintf("%s", memory + SIZE - 80);
  this->screen.setPlainText(str);
}

void
Display::freeSharedMemory()
{
  if (this->sharedMemory.isAttached()) {
    this->sharedMemory.detach();
  }
  if (this->sharedPorts.isAttached()) {
    this->sharedPorts.detach();
  }
  if (this->sharedInterrupts.isAttached()) {
    this->sharedInterrupts.detach();
  }
}
