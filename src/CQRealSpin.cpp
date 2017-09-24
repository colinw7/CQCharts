#include <CQRealSpin.h>
#include <QLineEdit>
//#include <iostream>
//#include <cassert>
#include <cmath>

CQRealSpin::
CQRealSpin(QWidget *parent, double value) :
 QDoubleSpinBox(parent)
{
  init(value);
}

CQRealSpin::
CQRealSpin(double value) :
 QDoubleSpinBox(0)
{
  init(value);
}

void
CQRealSpin::
init(double value)
{
  setObjectName("realSpin");

  setRange(-1E6, 1E6);

  setValue(value);

  connect(lineEdit(), SIGNAL(cursorPositionChanged(int,int)), this, SLOT(updateStep()));

  updateStep();
}

void
CQRealSpin::
updateStep()
{
  int pos = lineEdit()->cursorPosition();

  double s = posToStep(pos);

  if (fabs(step() - s) >= 1E-6) {
    step_ = s;

    emit stepChanged(step());
  }
}

double
CQRealSpin::
posToStep(int pos) const
{
  bool negative = isNegative();

  int dotPos = this->dotPos();

  //---

  // if no dot then power is length - pos
  if (dotPos < 0) {
    if (! negative) {
      if (pos < 1)
        pos = 1;
    }
    else {
      if (pos < 2)
        pos = 2;
    }

    int d = text().length() - pos;

    return pow(10, d);
  }

  //---

  // dot on right (1)
  if (pos == dotPos)
    return 1;

  //---

  // dot on left (0.1)
  if (pos == dotPos + 1)
    return 0.1;

  if (! negative) {
    if (pos < 1)
      pos = 1;
  }
  else {
    if (pos < 2)
      pos = 2;
  }

  if (pos > dotPos)
    --pos;

  return pow(10, dotPos - pos);
}

void
CQRealSpin::
stepBy(int n)
{
  double v = value();
  double s = step();

  int pos    = lineEdit()->cursorPosition();
  int dotPos = this->dotPos();

  bool negative = isNegative();

  if (! negative) {
    if (pos < 1)
      pos = 1;
  }
  else {
    if (pos < 2)
      pos = 2;
  }

  setValue(v + n*s);

  int dotPos1 = this->dotPos();

  int pos1 = dotPos1 - dotPos + pos;

  if (pos1 != pos)
    lineEdit()->setCursorPosition(pos1);

  updateStep();
}

bool
CQRealSpin::
isNegative() const
{
  const QString &text = lineEdit()->text();

  return (text.length() && text[0] == '-');
}

int
CQRealSpin::
dotPos() const
{
  const QString &text = lineEdit()->text();

  for (int i = 0; i < text.length(); ++i)
    if (text[i] == '.')
      return i;

  return -1;
}
