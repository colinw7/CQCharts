#include <CQRealScroll.h>
#include <cmath>

CQRealScroll::
CQRealScroll(QWidget *parent) :
 QScrollBar(parent)
{
  init();
}

CQRealScroll::
CQRealScroll(Qt::Orientation orientation, QWidget *parent) :
 QScrollBar(orientation, parent)
{
  init();
}

void
CQRealScroll::
init()
{
  setObjectName("rscroll");

  connect(this, SIGNAL(valueChanged(int)), this, SLOT(scrollSlot(int)));
}

void
CQRealScroll::
setRange(double min, double max)
{
  auto value = this->value();

  min_ = min;
  max_ = max;

  updateRange(value);
}

void
CQRealScroll::
setPageStep(double d)
{
  auto value = this->value();

  pageStep_ = d;

  updateRange(value);
}

void
CQRealScroll::
setSingleStep(double d)
{
  auto value = this->value();

  singleStep_ = d;

  updateRange(value);
}

void
CQRealScroll::
scrollSlot(int)
{
  valueChanged(value());
}

double
CQRealScroll::
value() const
{
  double pos = double(QScrollBar::value())/precision_;

  return min_ + pos*(max_ - min_);
}

void
CQRealScroll::
setValue(double r)
{
  double pos = (max_ != min_ ? (r - min_)/(max_ - min_) : 0.0);

  QScrollBar::setValue(int(std::round(pos*precision_)));
}

void
CQRealScroll::
updateRange(double value)
{
  auto range = (max_ - min_);

  if (range == 0.0) {
    max_  = min_ + 1.0;
    range = max_ - min_;
  }

  if (singleStep_ == 0.0)
    singleStep_ = range/1000.0;

  precision_ = range/singleStep_;

  auto step = pageStep_/singleStep_;

  QScrollBar::setRange(0, int(std::round(precision_ - step)));

  QScrollBar::setPageStep  (int(std::round(step)));
  QScrollBar::setSingleStep(1);

  disconnect(this, SIGNAL(valueChanged(int)), this, SLOT(scrollSlot(int)));

  setValue(value);

  connect(this, SIGNAL(valueChanged(int)), this, SLOT(scrollSlot(int)));
}
