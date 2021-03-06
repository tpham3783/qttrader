/*
 *  QtTrader stock charter
 *
 *  Copyright (C) 2001-2010 Stefan S. Stratigakos
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307,
 *  USA.
 */

#include "MarkerVLineDialog.h"
#include "Doc.h"
#include "Global.h"

#include <QtDebug>
#include <QSettings>
#include <QLayout>
#include <QFormLayout>


MarkerVLineDialog::MarkerVLineDialog (QWidget *p) : QDialog (p)
{
  _settings = 0;
  _modified = 0;
  _helpFile = "main.html";
  connect(this, SIGNAL(finished(int)), this, SLOT(deleteLater()));

  // main vbox
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setSpacing(2);
  vbox->setMargin(5);
  setLayout(vbox);
  
  _tabs = new QTabWidget;
  vbox->addWidget(_tabs);

  // buttonbox
  _buttonBox = new QDialogButtonBox(QDialogButtonBox::Help);
  connect(_buttonBox, SIGNAL(accepted()), this, SLOT(done()));
  connect(_buttonBox, SIGNAL(rejected()), this, SLOT(cancel()));
  vbox->addWidget(_buttonBox);

  // ok button
  _okButton = _buttonBox->addButton(QDialogButtonBox::Ok);
  _okButton->setText(tr("&OK"));

  // cancel button
  _cancelButton = _buttonBox->addButton(QDialogButtonBox::Cancel);
  _cancelButton->setText(tr("&Cancel"));
  _cancelButton->setDefault(TRUE);
  _cancelButton->setFocus();

  // help button
  QPushButton *b = _buttonBox->button(QDialogButtonBox::Help);
  b->setText(tr("&Help"));
  connect(b, SIGNAL(clicked()), this, SLOT(help()));
}

void
MarkerVLineDialog::done ()
{
  saveSettings();
  save();
  accept();
}

void
MarkerVLineDialog::help ()
{
  Doc *doc = new Doc;
  doc->showDocumentation(_helpFile);
}

void
MarkerVLineDialog::cancel ()
{
  saveSettings();
  reject();
}

void
MarkerVLineDialog::loadSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);

  QSize sz = settings.value("marker_vline_dialog_size", QSize(200,200)).toSize();
  resize(sz);

  // restore the position of the app
  QPoint p = settings.value("marker_vline_dialog_pos").toPoint();
  if (! p.isNull())
    move(p);
}

void
MarkerVLineDialog::saveSettings ()
{
  QSettings settings(g_settings);
  settings.beginGroup(g_session);
  settings.setValue("marker_vline_dialog_size", size());
  settings.setValue("marker_vline_dialog_pos", pos());
}

void
MarkerVLineDialog::setGUI (Entity *settings)
{
  _settings = settings;

  // OHLC page
  QWidget *w = new QWidget;
  
  QFormLayout *form = new QFormLayout;
  w->setLayout(form);
  
  // color
  QVariant *set = _settings->get(QString("color"));
  if (! set)
  {
    qDebug() << "MarkerVLineDialog::setGUI: no color setting";
    return;
  }
  
  _color = new ColorButton(this, QColor(set->toString()));
  connect(_color, SIGNAL(valueChanged(QColor)), this, SLOT(valueChanged()));
  form->addRow(tr("Color"), _color);
  
  // date
  set = _settings->get(QString("date"));
  if (! set)
  {
    qDebug() << "MarkerVLineDialog::setGUI: no date setting";
    return;
  }
  
  _date = new QDateTimeEdit;
  _date->setDateTime(set->toDateTime());
  connect(_date, SIGNAL(dateTimeChanged(const QDateTime &)), this, SLOT(valueChanged()));
  form->addRow(tr("Date"), _date);

  _tabs->addTab(w, tr("Settings"));
}

void MarkerVLineDialog::save ()
{
  if (! _modified)
    return;

  QVariant *set = _settings->get(QString("color"));
  set->setValue(_color->color().name());

  set = _settings->get(QString("date"));
  set->setValue(_date->dateTime());
}

void MarkerVLineDialog::valueChanged ()
{
  _modified = 1;
}
