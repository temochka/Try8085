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
#ifndef ACP_H_8085
#define ACP_H_8085

#define ACP_MEM_SIZE 0xffff
#define ACP_PARAM_SIZE 0x20
#define FILENAME_SIZE (ACP_PARAM_SIZE - 0x9)

#define MAX_AMPL 0x80
#define ZERO 0x80

#include "adc_graph.h"
#include <QCloseEvent>
#include <QDebug>
#include <QFile>
#include <QFile>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QSharedMemory>
#include <QSlider>
#include <QThread>
#include <QtWidgets/QMainWindow>
#include <math.h>

class ADC : public QWidget
{
  Q_OBJECT

public:
  ADC(QWidget* parent = 0);
  ~ADC();

private:
  QVBoxLayout* mainLayout;

  QGridLayout* statusLayout;
  QLabel* acpStatusLabel;
  QLabel* acpStatusValue;
  QLabel* capStatusLabel;
  QLabel* capStatusValue;
  QLabel* signalTypeLabel;
  QLabel* signalTypeValue;
  QLabel* signalParamLabel;
  QLabel* signalParamValue;
  QLabel* capacityLabel;
  QLabel* capacityValue;
  QLabel* discretizationRateLabel;
  QLabel* discretizationRateValue;
  QLabel* startvLabel;
  QLabel* startvValue;
  QLabel* zoomLabel;
  QSlider* zoomSlider;

  QHBoxLayout* handler;

  QGroupBox* acpColumn;
  QVBoxLayout* acpColumnLayout;
  ADCGraph* acpGraph;

  QGroupBox* capColumn;
  QVBoxLayout* capColumnLayout;
  ADCGraph* capGraph;

  QLabel* status;
  QSharedMemory sharedMemory;
  void timerEvent(QTimerEvent* event);
  void closeEvent(QCloseEvent* event);
  void initAcpCap();

  double* output_data;
  unsigned char* input_data;
  unsigned char* param;

  unsigned char acpON;
  unsigned char capON;
  unsigned char signalType;

  unsigned char samplingFreq;

  // 1 to 16 bit
  unsigned char bitDepth;
  // starting phase for a straight line or a sine wave (port A5)
  unsigned char startValue;
  // x coefficient (port A6):
  // (-128...127) for a straight line
  // (1...255) for a sine wave
  unsigned char signalParam;
  // non-linearity degree (port A7):
  // (0...255)
  unsigned char nonLinearity;

  int dataCount;
  int acpMemoryIterator;
  double posX;
  double posY;

  int updateTimerId;
  int initTimerId;

  bool initFinished();

  char inputFileName[FILENAME_SIZE];
public slots:
  void updateAcp();
  void updateStatus();
};

#endif // ACP_H_8085
