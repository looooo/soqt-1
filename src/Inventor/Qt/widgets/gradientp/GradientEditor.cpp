/**************************************************************************\
 *
 *  This file is part of the Coin 3D visualization library.
 *  Copyright (C) 1998-2003 by Systems in Motion.  All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  ("GPL") version 2 as published by the Free Software Foundation.
 *  See the file LICENSE.GPL at the root directory of this source
 *  distribution for additional information about the GNU GPL.
 *
 *  For using Coin with software that can not be combined with the GNU
 *  GPL, and for taking advantage of the additional benefits of our
 *  support services, please contact Systems in Motion about acquiring
 *  a Coin Professional Edition License.
 *
 *  See <URL:http://www.coin3d.org> for more information.
 *
 *  Systems in Motion, Teknobyen, Abels Gate 5, 7030 Trondheim, NORWAY.
 *  <URL:http://www.sim.no>.
 *
\**************************************************************************/

#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlayout.h>
#include <qcolordialog.h>
#include <qlabel.h>
#include <qcanvas.h>
  
#include "GradientEditor.h"
#include "GradientView.h"
#include "Gradient.h"

#include <Inventor/Qt/widgets/moc_GradientEditor.icc>


GradientEditor::GradientEditor(Gradient * grad, 
                               QWidget * parent, 
                               bool modal, 
                               const char* name, 
                               WFlags f)
: QWidget(parent, name, f)
{
  if (grad != NULL) {
    this->grad = new Gradient(*grad);
  }
  else this->grad = new Gradient();

  this->min = 0;
  this->max = 0;

  QVBoxLayout * topLayout = new QVBoxLayout(this);
  QHBoxLayout * buttonOutputLayout = new QHBoxLayout();
  QVBoxLayout * outputLayout = new QVBoxLayout();
  QHBoxLayout * buttonLayout = new QHBoxLayout();

  buttonOutputLayout->addLayout(outputLayout);
  buttonOutputLayout->addLayout(buttonLayout);

  this->gradientWidget = new QWidget(this, "gradientWidget");
  this->gradientWidget->setMinimumHeight(80);
  topLayout->addWidget(this->gradientWidget);
  topLayout->addLayout(buttonOutputLayout);

  QCanvas * canvas = new QCanvas(450,58);
  this->gradView = new GradientView(canvas, this->grad, gradientWidget, "GradientView");
  this->gradView->setMargin(10);
  this->gradView->setFrameStyle(QFrame::Sunken);

  QGroupBox * dataValues = new QGroupBox(this, "dataValues");
  dataValues->setTitle("Data Values");
  dataValues->setColumns(3);
  dataValues->setOrientation(Qt::Vertical);
  outputLayout->addWidget(dataValues);

  this->paramValue = new QLabel(dataValues, "paramValue");
  this->dataValue = new QLabel(dataValues, "dataValue");
  this->colorLabel = new QLabel(dataValues, "colorLabel");

  buttonLayout->setAlignment(Qt::AlignRight | Qt::AlignBottom);

  QPushButton * okButton = new QPushButton(this, "okButton");
  okButton->setText("OK");
  buttonLayout->addWidget(okButton, 0, 1);

  QPushButton * cancelButton = new QPushButton(this, "cancelButton");
  cancelButton->setText("Cancel");
  buttonLayout->addWidget(cancelButton, 0, 1);

  connect(this->gradView, SIGNAL(viewChanged()), this, SLOT(updateValueLabels()));
  connect(this->gradView, SIGNAL(tickSelected()), this, SLOT(updateValueLabels()));

  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));

  this->updateValueLabels();
}

GradientEditor::~GradientEditor()
{
  delete this->grad;
  delete this->gradView;
}

void GradientEditor::resizeEvent(QResizeEvent * e)
{
  this->gradientWidget->resize(QSize(e->size().width(), 80));
  this->gradView->resize(QSize(e->size().width(), 80));
  this->repaint();
}

void GradientEditor::setMin(float min)
{
  this->min = min;
}

void GradientEditor::setMax(float max)
{
  this->max = max;
}

void GradientEditor::updateValueLabels()
{
  float t = this->gradView->getSelectedPos();
  float value = t * (this->max - this->min) + this->min;

  QRgb col = this->grad->eval(t);


  this->paramValue->setText(QString(" Parameter value: ") + QString().setNum(t));

  this->dataValue->setText(QString(" Data value: ") + QString().setNum(value));

  this->colorLabel->setText(QString(" Color: ") + 
                      QString(" R: ") + QString().setNum(qRed(col)) +
                      QString(" G: ") + QString().setNum(qGreen(col)) +
                      QString(" B: ") + QString().setNum(qBlue(col)));
}

const Gradient& GradientEditor::getGradient() const
{
  return *this->grad;
}

void GradientEditor::setGradient(Gradient * grad)
{
  *this->grad = *grad;
  this->gradView->updateTicks();
  this->gradView->updateView();
}

void GradientEditor::updateAll()
{  
  this->gradView->updateTicks();
  this->gradView->updateView();
}

void GradientEditor::accept()
{
  emit this->accepted();
}

void GradientEditor::reject()
{
  emit this->rejected();
}
