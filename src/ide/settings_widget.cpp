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
#include "settings_widget.h"

SettingsWidget::SettingsWidget(QWidget* parent)
  : QDialog(parent)
{
  this->setWindowTitle(tr("Try8085 settings"));
  QSettings registry(Globals::getSettingsOrg(), Globals::getSettingsApp());
  this->mainLayout = new QVBoxLayout;
  this->settingsTabs = new QTabWidget;

  this->appearanceSettings = new QWidget;
  this->settingsTabs->addTab(this->appearanceSettings, tr("Appearance"));

  QString fontFamily = registry.value("font-family", "Courier New").toString();
  int fontSize = registry.value("font-size", 12).toInt();
  this->currentFontHandler = new QFont(fontFamily, fontSize);
  this->currentFont = new QGroupBox(tr("Font settings"));
  this->currentFontLabel =
    new QLabel(this->currentFontHandler->family() + ", " +
               QString::number(this->currentFontHandler->pointSize(), 10));
  this->currentFontSelect = new QPushButton(tr("Select font..."));
  this->currentFontLayout = new QHBoxLayout;
  this->currentFontLayout->addWidget(this->currentFontLabel);
  this->currentFontLayout->addWidget(this->currentFontSelect);
  this->currentFont->setLayout(this->currentFontLayout);
  this->connect(this->currentFontSelect, SIGNAL(clicked()), this,
                SLOT(changeFont()));

  this->currentLanguageSelect = new QGroupBox(tr("Language settings"));
  this->currentLanguageLabel = new QLabel(tr("Application language:"));
  this->currentLanguageCombo = new QComboBox();
  this->currentLanguageCombo->addItem(tr("English"));
  this->currentLanguageCombo->addItem(tr("Russian"));
  this->currentLanguageCombo->setCurrentIndex(
    registry.value("language").toInt());
  this->currentLanguageHandler = registry.value("language").toInt();
  this->currentLanguageLayout = new QHBoxLayout;
  this->currentLanguageLayout->addWidget(this->currentLanguageLabel);
  this->currentLanguageLayout->addWidget(this->currentLanguageCombo);
  this->currentLanguageSelect->setLayout(this->currentLanguageLayout);

  this->appearanceLayout = new QVBoxLayout;
  this->appearanceLayout->addWidget(this->currentLanguageSelect);
  this->appearanceLayout->addWidget(this->currentFont);
  this->appearanceSettings->setLayout(this->appearanceLayout);

  this->procSettings = new QWidget;
  this->settingsTabs->addTab(this->procSettings, tr("Try8085 settings"));

  this->procFrequencySlider = new QSlider(Qt::Horizontal, this->procSettings);
  this->procFrequencySlider->setMinimum(0);
  this->procFrequencySlider->setMaximum(50);

  this->procFrequencyLabel = new QLabel(tr("Sleep time:"));
  this->procFrequency = new QHBoxLayout;
  this->procFrequencyValue = new QSpinBox;
  this->procFrequencyValue->setValue(
    registry.value("sleeptime", int(10)).toInt());
  this->procFrequencyValue->setMaximum(50);
  this->procFrequencyValue->setMinimum(0);
  this->procFrequency->addWidget(this->procFrequencyLabel);
  this->procFrequency->addWidget(this->procFrequencySlider);
  this->procFrequency->addWidget(this->procFrequencyValue);
  this->connect(this->procFrequencySlider, SIGNAL(valueChanged(int)),
                this->procFrequencyValue, SLOT(setValue(int)));
  this->connect(this->procFrequencyValue, SIGNAL(valueChanged(int)),
                this->procFrequencySlider, SLOT(setValue(int)));

  this->procSettings->setLayout(this->procFrequency);

  this->buttonsLayout = new QHBoxLayout;
  this->okButton = new QPushButton(tr("OK"));
  this->applyButton = new QPushButton(tr("Apply"));
  this->cancelButton = new QPushButton(tr("Cancel"));

  this->buttonsLayout->addWidget(this->okButton);
  this->buttonsLayout->addWidget(this->applyButton);
  this->buttonsLayout->addWidget(this->cancelButton);

  this->mainLayout->addWidget(this->settingsTabs);
  this->mainLayout->addLayout(this->buttonsLayout);

  connect(this->okButton, SIGNAL(clicked()), this, SLOT(saveChanges()));
  connect(this->okButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(this->applyButton, SIGNAL(clicked()), this, SLOT(saveChanges()));
  connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(close()));
  connect(this->cancelButton, SIGNAL(clicked()), this, SLOT(loadSettings()));
  this->setLayout(this->mainLayout);
}

void
SettingsWidget::saveChanges()
{
  QSettings settings(Globals::getSettingsOrg(), Globals::getSettingsApp());
  settings.setValue("sleeptime", this->procFrequencyValue->value());
  settings.setValue("font-family", this->currentFontHandler->family());
  settings.setValue("font-size", this->currentFontHandler->pointSize());
  if (this->currentLanguageCombo->currentIndex() !=
      this->currentLanguageHandler) {
    settings.setValue("language", this->currentLanguageCombo->currentIndex());
    this->currentLanguageHandler = this->currentLanguageCombo->currentIndex();
    QMessageBox::warning(
      NULL, tr("Try8085 Settings"),
      QString(
        QObject::tr("Language settings will affect after program restart. ")),
      QMessageBox::Ok);
  }
  emit settingsUpdated();
}

void
SettingsWidget::changeFont()
{
  bool ok;
  QFont font = QFontDialog::getFont(&ok, *(this->currentFontHandler), this);
  if (ok) {
    *(this->currentFontHandler) = font;
    this->currentFontLabel->setText(
      this->currentFontHandler->family() + ", " +
      QString::number(this->currentFontHandler->pointSize(), 10));
  }
}

void
SettingsWidget::loadSettings()
{
  QSettings settings(Globals::getSettingsOrg(), Globals::getSettingsApp());
  int frequency = settings.value("sleeptime", int(10)).toInt();
  this->procFrequencyValue->setValue(frequency);
  this->procFrequencySlider->setValue(frequency);

  delete this->currentFontHandler;
  this->currentFontHandler =
    new QFont(settings.value("font-family", QString("CourierNew")).toString(),
              settings.value("font-size", int(12)).toInt());
  this->currentFontLabel->setText(
    this->currentFontHandler->family() + ", " +
    QString::number(this->currentFontHandler->pointSize(), 10));
  this->currentLanguageCombo->setCurrentIndex(
    settings.value("language").toInt());
  this->currentLanguageHandler = settings.value("language").toInt();
}
