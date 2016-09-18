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
#include "digital_keyboard.h"

DigitalKeyboard::DigitalKeyboard(QWidget* parent)
  : QDialog(parent)
  , sharedPorts("try8085_ports")
  , sharedInterrupts("try8085_interrupts")
{
  this->setWindowTitle(tr("Try8085 Keyboard"));
  this->setWindowIcon(QIcon(":/Icons/icons/numpad_64x64.png"));
  this->_c = new QPushButton("^");
  this->_div = new QPushButton("/");
  this->_mul = new QPushButton("*");
  this->_sub = new QPushButton("-");
  this->_seven = new QPushButton("7");
  this->_eight = new QPushButton("8");
  this->_nine = new QPushButton("9");
  this->_four = new QPushButton("4");
  this->_five = new QPushButton("5");
  this->_six = new QPushButton("6");
  this->_one = new QPushButton("1");
  this->_two = new QPushButton("2");
  this->_three = new QPushButton("3");
  this->_add = new QPushButton("+");
  this->_equ = new QPushButton("=");
  this->_zero = new QPushButton("0");
  this->_mode = new QPushButton("A-F");
  this->_dot = new QPushButton(".");

  this->_first = new QHBoxLayout;
  this->_second = new QHBoxLayout;
  this->_third = new QHBoxLayout;
  this->_fourth = new QHBoxLayout;
  this->_fifth = new QHBoxLayout;

  this->_grh1 = new QHBoxLayout;
  this->_grh2 = new QHBoxLayout;

  this->_grv1 = new QVBoxLayout;
  this->_grv2 = new QVBoxLayout;
  this->_ml = new QVBoxLayout;

  connect(this->_one, SIGNAL(clicked()), this, SLOT(c_one()));
  connect(this->_two, SIGNAL(clicked()), this, SLOT(c_two()));
  connect(this->_three, SIGNAL(clicked()), this, SLOT(c_three()));
  connect(this->_four, SIGNAL(clicked()), this, SLOT(c_four()));
  connect(this->_five, SIGNAL(clicked()), this, SLOT(c_five()));
  connect(this->_six, SIGNAL(clicked()), this, SLOT(c_six()));
  connect(this->_seven, SIGNAL(clicked()), this, SLOT(c_seven()));
  connect(this->_eight, SIGNAL(clicked()), this, SLOT(c_eight()));
  connect(this->_nine, SIGNAL(clicked()), this, SLOT(c_nine()));
  connect(this->_div, SIGNAL(clicked()), this, SLOT(c_div()));
  connect(this->_mul, SIGNAL(clicked()), this, SLOT(c_mul()));
  connect(this->_sub, SIGNAL(clicked()), this, SLOT(c_sub()));
  connect(this->_add, SIGNAL(clicked()), this, SLOT(c_add()));
  connect(this->_equ, SIGNAL(clicked()), this, SLOT(c_equ()));
  connect(this->_dot, SIGNAL(clicked()), this, SLOT(c_dot()));
  connect(this->_c, SIGNAL(clicked()), this, SLOT(c_c()));
  connect(this->_mode, SIGNAL(clicked()), this, SLOT(switchMode()));
  connect(this->_zero, SIGNAL(clicked()), this, SLOT(c_zero()));

  this->_first->addWidget(this->_c);
  this->_first->addWidget(this->_div);
  this->_first->addWidget(this->_mul);
  this->_first->addWidget(this->_sub);

  this->_second->addWidget(this->_seven);
  this->_second->addWidget(this->_eight);
  this->_second->addWidget(this->_nine);

  this->_third->addWidget(this->_four);
  this->_third->addWidget(this->_five);
  this->_third->addWidget(this->_six);

  this->_grv1->addLayout(this->_second);
  this->_grv1->addLayout(this->_third);

  this->_grh1->addLayout(this->_grv1);
  this->_grh1->addWidget(this->_add);

  this->_fourth->addWidget(this->_one);
  this->_fourth->addWidget(this->_two);
  this->_fourth->addWidget(this->_three);

  this->_fifth->addWidget(this->_mode);
  this->_fifth->addWidget(this->_zero);
  this->_fifth->addWidget(this->_dot);

  this->_grv2->addLayout(this->_fourth);
  this->_grv2->addLayout(this->_fifth);
  this->_grh2->addLayout(this->_grv2);
  this->_grh2->addWidget(this->_equ);

  this->_ml->addLayout(this->_first);
  this->_ml->addLayout(this->_grh1);
  this->_ml->addLayout(this->_grh2);

  this->_add->setFixedHeight(60);
  this->_equ->setFixedHeight(60);
  this->_add->setFixedWidth(30);
  this->_equ->setFixedWidth(30);
  this->_sub->setFixedWidth(30);

  this->setLayout(this->_ml);
  this->startTimer(400);
  this->numInterrupt = 3;
  this->handlerAddress = 0x0040;
  this->portKeyboard = 0x40;
}

DigitalKeyboard::~DigitalKeyboard()
{
  this->clearMemory();
}

void
DigitalKeyboard::interrupt(unsigned char keycode)
{
  qDebug() << keycode << "\n";
  this->clearMemory();
  if (!this->sharedPorts.attach() || !this->sharedInterrupts.attach()) {
    QMessageBox::warning(this, tr("Try8085 Keyboard"),
                         tr("Unable to attach to shared memory segment.\n"
                            "Load Try8085 first or try to restart."),
                         QMessageBox::Ok);
    return;
  }
  this->sharedPorts.lock();
  *((unsigned char*)this->sharedPorts.data() + this->portKeyboard) = keycode;
  this->sharedPorts.unlock();
  this->sharedPorts.detach();

  this->sharedInterrupts.lock();
  *((unsigned short*)((unsigned char*)this->sharedInterrupts.data() +
                      this->numInterrupt)) = this->handlerAddress;
  this->sharedInterrupts.unlock();
  this->sharedInterrupts.detach();
}

void
DigitalKeyboard::switchMode()
{
  if (this->_mode->text() == "A-F") {
    this->_four->setText("A");
    this->_five->setText("B");
    this->_six->setText("C");
    this->_seven->setText("D");
    this->_eight->setText("E");
    this->_nine->setText("F");
    this->_mode->setText("0-9");
  } else {
    this->_four->setText("4");
    this->_five->setText("5");
    this->_six->setText("6");
    this->_seven->setText("7");
    this->_eight->setText("8");
    this->_nine->setText("9");
    this->_mode->setText("A-F");
  }
}

unsigned char
DigitalKeyboard::translate(char letter)
{
  switch (letter) {
    case '0':
      return 0x0;
    case '1':
      return 0x1;
    case '2':
      return 0x2;
    case '3':
      return 0x3;
    case '4':
      return 0x4;
    case '5':
      return 0x5;
    case '6':
      return 0x6;
    case '7':
      return 0x7;
    case '8':
      return 0x8;
    case '9':
      return 0x9;
    case 'A':
      return 0xA;
    case 'B':
      return 0xB;
    case 'C':
      return 0xC;
    case 'D':
      return 0xD;
    case 'E':
      return 0xE;
    case 'F':
      return 0xF;
    case '.':
      return 0x10;
    case '=':
      return 0x11;
    case '+':
      return 0x12;
    case '-':
      return 0x13;
    case '*':
      return 0x14;
    case '/':
      return 0x15;
    case '^':
      return 0x16;
  }
  return 0x17;
}

void
DigitalKeyboard::clearMemory()
{
  if (this->sharedPorts.isAttached()) {
    this->sharedPorts.detach();
  }
  if (this->sharedInterrupts.isAttached()) {
    this->sharedInterrupts.detach();
  }
}

void
DigitalKeyboard::c_div()
{
  this->interrupt(this->translate(*(this->_div->text().toStdString().c_str())));
}

void
DigitalKeyboard::c_mul()
{
  this->interrupt(this->translate(*(this->_mul->text().toStdString().c_str())));
}

void
DigitalKeyboard::c_sub()
{
  this->interrupt(this->translate(*(this->_sub->text().toStdString().c_str())));
}
void
DigitalKeyboard::c_add()
{
  this->interrupt(this->translate(*(this->_add->text().toStdString().c_str())));
}
void
DigitalKeyboard::c_equ()
{
  this->interrupt(this->translate(*(this->_equ->text().toStdString().c_str())));
}
void
DigitalKeyboard::c_dot()
{
  this->interrupt(this->translate(*(this->_dot->text().toStdString().c_str())));
}
void
DigitalKeyboard::c_one()
{
  this->interrupt(this->translate(*(this->_one->text().toStdString().c_str())));
}
void
DigitalKeyboard::c_two()
{
  this->interrupt(this->translate(*(this->_two->text().toStdString().c_str())));
}
void
DigitalKeyboard::c_three()
{
  this->interrupt(
    this->translate(*(this->_three->text().toStdString().c_str())));
}
void
DigitalKeyboard::c_four()
{
  this->interrupt(
    this->translate(*(this->_four->text().toStdString().c_str())));
}
void
DigitalKeyboard::c_five()
{
  this->interrupt(
    this->translate(*(this->_five->text().toStdString().c_str())));
}
void
DigitalKeyboard::c_six()
{
  this->interrupt(this->translate(*(this->_six->text().toStdString().c_str())));
}
void
DigitalKeyboard::c_seven()
{
  this->interrupt(
    this->translate(*(this->_seven->text().toStdString().c_str())));
}
void
DigitalKeyboard::c_eight()
{
  this->interrupt(
    this->translate(*(this->_eight->text().toStdString().c_str())));
}

void
DigitalKeyboard::c_nine()
{
  this->interrupt(
    this->translate(*(this->_nine->text().toStdString().c_str())));
}

void
DigitalKeyboard::c_c()
{
  this->interrupt(this->translate(*(this->_c->text().toStdString().c_str())));
}

void
DigitalKeyboard::c_zero()
{
  this->interrupt(
    this->translate(*(this->_zero->text().toStdString().c_str())));
}

void
DigitalKeyboard::closeEvent(QCloseEvent* event)
{
  event->ignore();
}
