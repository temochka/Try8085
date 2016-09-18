/*
  Copyright (C) 2009, 2016  Artem Chistyakov <chistyakov.artem@gmail.com>
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
#include "adc.h"

ADC::ADC(QWidget* parent)
  : QWidget(parent)
  , sharedMemory("try8085_ports")
{
  this->setWindowTitle(tr("Try8085 ADC & DAC"));
  this->status = new QLabel(this);

  this->statusLayout = new QGridLayout;

  this->acpStatusLabel = new QLabel(tr("ADC status"));
  this->acpStatusValue = new QLabel(tr("n/a"));
  this->statusLayout->addWidget(this->acpStatusLabel, 0, 0);
  this->statusLayout->addWidget(this->acpStatusValue, 0, 1);

  this->capStatusLabel = new QLabel(tr("DAC status"));
  this->capStatusValue = new QLabel(tr("n/a"));
  this->statusLayout->addWidget(this->capStatusLabel, 0, 2);
  this->statusLayout->addWidget(this->capStatusValue, 0, 3);

  this->signalTypeLabel = new QLabel(tr("Signal type"));
  this->signalTypeValue = new QLabel(tr("n/a"));
  this->statusLayout->addWidget(this->signalTypeLabel, 1, 0);
  this->statusLayout->addWidget(this->signalTypeValue, 1, 1);

  this->signalParamLabel = new QLabel(tr("Signal param"));
  this->signalParamValue = new QLabel(tr("n/a"));
  this->statusLayout->addWidget(this->signalParamLabel, 1, 2);
  this->statusLayout->addWidget(this->signalParamValue, 1, 3);

  this->capacityLabel = new QLabel(tr("Capacity"));
  this->capacityValue = new QLabel(tr("n/a"));
  this->statusLayout->addWidget(this->capacityLabel, 2, 0);
  this->statusLayout->addWidget(this->capacityValue, 2, 1);

  this->discretizationRateLabel = new QLabel(tr("Discretization frequency"));
  this->discretizationRateValue = new QLabel(tr("n/a"));
  this->statusLayout->addWidget(this->discretizationRateLabel, 2, 2);
  this->statusLayout->addWidget(this->discretizationRateValue, 2, 3);

  this->zoomLabel = new QLabel(tr("Zoom"));
  this->zoomSlider = new QSlider(Qt::Horizontal);
  this->zoomSlider->setMinimum(1);
  this->zoomSlider->setMaximum(6);
  this->zoomSlider->setFixedWidth(300);
  this->statusLayout->addWidget(this->zoomLabel, 3, 0);
  this->statusLayout->addWidget(this->zoomSlider, 3, 1);

  this->acpColumn = new QGroupBox(tr("Analog to digital converter"));
  this->acpGraph = new ADCGraph(this->acpColumn);
  this->acpColumnLayout = new QVBoxLayout;
  this->acpColumnLayout->addWidget(this->acpGraph);
  this->acpColumn->setLayout(this->acpColumnLayout);

  this->capColumn = new QGroupBox(tr("Digital to analog converter"));
  this->capGraph = new ADCGraph(this->capColumn);
  this->capColumnLayout = new QVBoxLayout;
  this->capColumnLayout->addWidget(this->capGraph);
  this->capColumn->setLayout(this->capColumnLayout);

  this->handler = new QHBoxLayout;
  this->handler->addWidget(this->acpColumn);
  this->handler->addWidget(this->capColumn);

  this->mainLayout = new QVBoxLayout;
  this->mainLayout->addLayout(this->statusLayout);
  this->mainLayout->addLayout(this->handler);
  this->setLayout(this->mainLayout);

  connect(this->zoomSlider, SIGNAL(valueChanged(int)), this->acpGraph,
          SLOT(setZoom(int)));
  connect(this->zoomSlider, SIGNAL(valueChanged(int)), this->capGraph,
          SLOT(setZoom(int)));

  this->setFixedWidth(900);
  this->setFixedHeight(400);
  this->show();
  if (this->sharedMemory.isAttached()) {
    this->sharedMemory.detach();
  }

  this->output_data = new double[ACP_MEM_SIZE];
  this->input_data = new unsigned char[ACP_MEM_SIZE];
  this->param = new unsigned char[ACP_PARAM_SIZE];
  memset(this->output_data, 0, ACP_MEM_SIZE * sizeof(double));
  memset(this->input_data, 0, ACP_MEM_SIZE);

  this->initTimerId = this->startTimer(500);
  this->updateTimerId = 0;
}

ADC::~ADC()
{
  if (this->sharedMemory.isAttached()) {
    this->sharedMemory.detach();
  }
}

void
ADC::updateAcp()
{
  if (!this->acpON && !this->capON) {
    this->posX = 0;
    return;
  }

  if (this->param[1] == 2) {
    // TODO: File input
  }

  if (this->acpON) {
    for (int i = 0; i < this->bitDepth; i++) {
      this->posX += 5.0f;
      switch (this->signalType) {
        case 0: // flat line
          this->posY = this->startValue + this->signalParam * this->posX;
          this->output_data[this->acpMemoryIterator++] =
            this->posY +
            (25) * (qrand() % 5 + 1) * (qrand() % 2 ? 1 : -1) / 100;
          this->acpGraph->drawLineTo(this->posX, this->posY);
          if (this->capON) {
            this->capGraph->drawLineTo(
              this->posX, this->output_data[this->acpMemoryIterator - 1]);
          }
          break;
        case 1: // sine wave
          this->posY = this->startValue + sin(this->signalParam * this->posX);
          this->output_data[this->acpMemoryIterator++] =
            this->posY + 25 * (qrand() % 15 + 1) * (qrand() % 2 ? 1 : -1) / 100;
          this->acpGraph->drawLineTo(this->posX, this->posY);
          if (this->capON) {
            this->capGraph->drawLineTo(
              this->posX, this->output_data[this->acpMemoryIterator - 1]);
          }
          break;
        case 2: // user signal
          this->posY = this->input_data[this->dataCount];
          this->dataCount++;
          this->acpGraph->drawLineTo(this->posX, this->posY);
          if (this->capON) {
            this->capGraph->drawLineTo(
              this->posX, this->output_data[this->acpMemoryIterator - 1]);
          }
          break;
        default:;
      }
    }
  }
}

void
ADC::timerEvent(QTimerEvent* event)
{
  this->initAcpCap();
  if (this->updateTimerId) {
    if (this->acpMemoryIterator == ACP_MEM_SIZE) {
      this->killTimer(this->updateTimerId);
      return;
    }
    if (!this->sharedMemory.attach()) {
      int ret =
        QMessageBox::warning(this, tr("Try8085 ADC & DAC"),
                             tr("Unable to attach to shared memory segment.\n"
                                "Load Try8085 first."),
                             QMessageBox::Ok);
      return;
    }

    this->sharedMemory.lock();
    memcpy(this->param, (unsigned char*)this->sharedMemory.data() + 0xA0,
           ACP_PARAM_SIZE); // (port A0)
    this->sharedMemory.unlock();

    sharedMemory.detach();
    this->updateAcp();
    this->updateStatus();
  }
}

void
ADC::closeEvent(QCloseEvent* event)
{
}

void
ADC::initAcpCap()
{
  if (!this->sharedMemory.attach()) {
    QMessageBox::warning(this, tr("Try8085 ADC & DAC"),
                         tr("Unable to attach to shared memory segment.\n"
                            "Load Try8085 first."),
                         QMessageBox::Ok);
    return;
  }

  this->sharedMemory.lock();
  memcpy(this->param, (unsigned char*)this->sharedMemory.data() + 0xA0,
         ACP_PARAM_SIZE); // (port A0)
  this->sharedMemory.unlock();

  // Reset bit installed
  if (param[8]) {
    this->signalType = param[2];
    this->samplingFreq = param[3];
    this->bitDepth = param[4];
    this->startValue = param[5];
    this->signalParam = param[6];
    this->nonLinearity = param[7];

    this->input_data = 0;
    this->posX = 0;
    this->acpMemoryIterator = 0;
    this->sharedMemory.lock();

    int ret;
    if (this->samplingFreq != 1 && this->samplingFreq != 2 &&
        this->samplingFreq != 4 && this->samplingFreq != 8 &&
        this->samplingFreq != 16) {
      ret = QMessageBox::warning(
        this, tr("Try8085 ADC & DAC"),
        tr("Invalid discretization frequency value! Please try one of the "
           "following: 1, 2, 4, 8 or 16.\n"),
        QMessageBox::Ok);
      return;
    }

    if (this->bitDepth <= 1 || this->bitDepth >= 16) {
      ret = QMessageBox::warning(
        this, tr("Try8085 ADC & DAC"),
        tr("Invalid bit A / D! Please try one of the following: 1...16.\n"),
        QMessageBox::Ok);
      return;
    }

    if (this->signalType == 1 && !this->signalParam) {
      ret = QMessageBox::warning(
        this, tr("Try8085 ADC & DAC"),
        tr("Invalid parameter! Please try one of the following: 1...255.\n"),
        QMessageBox::Ok);
      return;
    }
    if (this->updateTimerId) {
      this->killTimer(this->updateTimerId);
    }
    this->updateTimerId = this->startTimer(
      (int)1000 * this->samplingFreq / 0xff * 10); // change sampling frequency
    *((unsigned char*)this->sharedMemory.data() + 0xA0 + 8) = 0;
    if (this->initTimerId) {
      this->killTimer(this->initTimerId);
    }
  }

  sharedMemory.detach();

  this->acpON = param[0];
  this->capON = param[1];
}

void
ADC::updateStatus()
{
  if (this->acpON) {
    this->acpStatusValue->setText(tr("On"));
  } else {
    this->acpStatusValue->setText(tr("Off"));
  }
  if (this->capON) {
    this->capStatusValue->setText(tr("On"));
  } else {
    this->capStatusValue->setText(tr("Off"));
  }
  if (this->signalType == 0) {
    this->signalTypeValue->setText(tr("Line"));
  } else if (this->signalType == 1) {
    this->signalTypeValue->setText(tr("Sin"));
  } else if (this->signalType == 2) {
    this->signalTypeValue->setText(tr("User data"));
  }
  this->signalParamValue->setText(QString::number(this->signalParam, 10));
  this->capacityValue->setText(QString::number(this->bitDepth, 10));
  this->discretizationRateValue->setText(
    QString::number(this->samplingFreq, 10));
}
