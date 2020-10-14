#include <CQRangeScroll.h>
#include <QVBoxLayout>
#include <QScrollBar>
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QStyle>
#include <iostream>

CQRangeScroll::
CQRangeScroll(QWidget *parent, Qt::Orientation orientation) :
 QFrame(parent), orientation_(orientation)
{
  init();
}

CQRangeScroll::
CQRangeScroll(Qt::Orientation orientation, QWidget *parent) :
 QFrame(parent), orientation_(orientation)
{
  init();
}

void
CQRangeScroll::
init()
{
  setObjectName("rangeScroll");

  //--

  QBoxLayout *layout;

  if (orientation_ == Qt::Horizontal) {
    layout = new QVBoxLayout(this);
    layout->setMargin(0); layout->setSpacing(0);
  }
  else {
    layout = new QHBoxLayout(this);
    layout->setMargin(0); layout->setSpacing(0);
  }

  //---

  canvas_ = new CQRangeScrollCanvas(this);

  canvas_->setObjectName("canvas");

  layout->addWidget(canvas_);

  //---

  scroll_ = new QScrollBar;

  scroll_->setObjectName("scroll");

  if (orientation_ == Qt::Horizontal)
    scroll_->setOrientation(Qt::Horizontal);
  else
    scroll_->setOrientation(Qt::Vertical);

  layout->addWidget(scroll_);

  connect(scroll_, SIGNAL(valueChanged(int)), this, SLOT(scrollSlot(int)));

  //---

  margin_ = style()->pixelMetric(QStyle::PM_ScrollBarExtent);
}

void
CQRangeScroll::
setRange(double min, double max)
{
  min_ = min;
  max_ = max;
  pos_ = min_;
  len_ = max_ - min_;
}

void
CQRangeScroll::
setPos(double r, bool notify)
{
  pos_ = r;

  if (notify) {
    updateScroll();

    emitWindowChanged();
  }
}

void
CQRangeScroll::
setLen(double r, bool notify)
{
  len_ = r;

  if (notify) {
    updateScroll();

    emitWindowChanged();
  }
}

void
CQRangeScroll::
drawBackground(QPainter *)
{
}

void
CQRangeScroll::
scrollSlot(int pos)
{
  double pos1;

  if (orientation_ == Qt::Horizontal)
    pos1 = pixelToPos(pos + margin_);
  else
    pos1 = pixelToPos(pos + scroll_->pageStep() + margin_);

  setPos(pos1);
}

void
CQRangeScroll::
updateScroll()
{
  int pixelSize;

  if (orientation_ == Qt::Horizontal)
    pixelSize = width() - 2*margin_;
  else
    pixelSize = height() - 2*margin_;

  int pos, size;

  if (orientation_ == Qt::Horizontal) {
    pos  = posToPixel(pos_);
    size = posToPixel(pos_ + len_) - pos;
  }
  else {
    pos  = posToPixel(pos_ + len_);
    size = posToPixel(pos_) - pos;
  }

  scroll_->setRange(0, pixelSize - size);
  scroll_->setPageStep(size);

  disconnect(scroll_, SIGNAL(valueChanged(int)), this, SLOT(scrollSlot(int)));

  scroll_->setValue(pos - margin_);

  connect(scroll_, SIGNAL(valueChanged(int)), this, SLOT(scrollSlot(int)));

  canvas_->updateHandles();

  update();
}

void
CQRangeScroll::
emitWindowChanged()
{
  emit windowChanged();
}

int
CQRangeScroll::
posToPixel(double pos) const
{
  if (orientation_ != Qt::Horizontal)
    pos = 1.0 - pos;

  int pixelSize;

  if (orientation_ == Qt::Horizontal)
    pixelSize = width() - 2*margin_;
  else
    pixelSize = height() - 2*margin_;

  double len = max_ - min_;

  double pixel = margin_ + (len > 0 ? pixelSize*pos/len : 0);

  return pixel;
}

double
CQRangeScroll::
pixelToPos(int pixel) const
{
  int pixelSize;

  if (orientation_ == Qt::Horizontal)
    pixelSize = width() - 2*margin_;
  else
    pixelSize = height() - 2*margin_;

  double len = max_ - min_;

  double pos = (pixelSize > 0 ? (pixel - margin_)*len/pixelSize : 0);

  if (orientation_ != Qt::Horizontal)
    pos = 1.0 - pos;

  return pos;
}

void
CQRangeScroll::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.fillRect(rect(), Qt::white);
}

void
CQRangeScroll::
resizeEvent(QResizeEvent *)
{
  updateScroll();
}

QSize
CQRangeScroll::
sizeHint() const
{
  return minimumSizeHint();
}

QSize
CQRangeScroll::
minimumSizeHint() const
{
  QSize s = scroll_->minimumSizeHint();

  if (orientation_ == Qt::Horizontal)
    return QSize(s.width(), s.height() + 32);
  else
    return QSize(s.width() + 32, s.height());
}

//------

CQRangeScrollCanvas::
CQRangeScrollCanvas(CQRangeScroll *scroll) :
 scroll_(scroll)
{
  startHandle_ = new CQRangeScrollHandle(scroll_);
  endHandle_   = new CQRangeScrollHandle(scroll_);

  startHandle_->setObjectName("startHandle");
  endHandle_  ->setObjectName("endHandle");

  connect(startHandle_, SIGNAL(valueChanged(double)), this, SLOT(startHandleSlot(double)));
  connect(endHandle_  , SIGNAL(valueChanged(double)), this, SLOT(endHandleSlot(double)));

  startHandle_->setParent(this);
  endHandle_  ->setParent(this);
}

void
CQRangeScrollCanvas::
startHandleSlot(double pos)
{
  pos = std::min(std::max(pos, scroll_->min()), scroll_->pos() + scroll_->len() - 0.001);

  double d = pos - scroll_->pos();

  if (d > 0) {
    scroll_->setLen(scroll_->len() - d, /*notify*/false);
    scroll_->setPos(pos               , /*notify*/false);
  }
  else {
    scroll_->setPos(pos               , /*notify*/false);
    scroll_->setLen(scroll_->len() - d, /*notify*/false);
  }

  scroll_->updateScroll();

  scroll_->emitWindowChanged();
}

void
CQRangeScrollCanvas::
endHandleSlot(double pos)
{
  pos = std::min(std::max(pos, scroll_->pos() + 0.001), scroll_->max());

  scroll_->setLen(pos - scroll_->pos());
}

void
CQRangeScrollCanvas::
updateHandles()
{
  int hw = startHandle_->width ();
  int hh = startHandle_->height();

  int pixel1 = scroll_->posToPixel(scroll_->pos()                 );
  int pixel2 = scroll_->posToPixel(scroll_->pos() + scroll_->len());

  if (scroll_->orientation() == Qt::Horizontal) {
    startHandle_->move(pixel1 - hw/2, height()/2 - hh/2);
    endHandle_  ->move(pixel2 - hw/2, height()/2 - hh/2);
  }
  else {
    startHandle_->move(width()/2 - hw/2, pixel1 - hh/2);
    endHandle_  ->move(width()/2 - hw/2, pixel2 - hh/2);
  }

  startHandle_->show();
  endHandle_  ->show();

  update();
}

void
CQRangeScrollCanvas::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  p.fillRect(rect(), Qt::white);

  //---

  scroll_->drawBackground(&p);

  // draw current range
  int pixel1 = scroll_->posToPixel(scroll_->pos()                 );
  int pixel2 = scroll_->posToPixel(scroll_->pos() + scroll_->len());

  QColor c = scroll_->rangeColor();
  double a = scroll_->rangeAlpha();

  c.setAlphaF(a);

  if (scroll_->orientation() == Qt::Horizontal)
    p.fillRect(QRect(pixel1, 0, pixel2 - pixel1, height()), c);
  else
    p.fillRect(QRect(0, pixel1, width(), pixel2 - pixel1), c);
}

void
CQRangeScrollCanvas::
resizeEvent(QResizeEvent *)
{
  updateHandles();
}

//------

CQRangeScrollHandle::
CQRangeScrollHandle(CQRangeScroll *scroll) :
 scroll_(scroll)
{
  setFixedWidth (16);
  setFixedHeight(16);
}

void
CQRangeScrollHandle::
paintEvent(QPaintEvent *)
{
  QPainter p(this);

  QBrush b = palette().window();

  p.fillRect(rect(), b);

  QColor c = palette().text().color();

  p.setPen(c);

  int xc = width ()/2;
  int yc = height()/2;

  if (scroll_->orientation() == Qt::Horizontal) {
    p.drawLine(xc - 2, yc - 4, xc - 2, yc + 4);
    p.drawLine(xc + 2, yc - 4, xc + 2, yc + 4);
  }
  else {
    p.drawLine(xc - 4, yc - 2, xc + 4, yc - 2);
    p.drawLine(xc - 4, yc + 2, xc + 4, yc + 2);
  }
}

void
CQRangeScrollHandle::
mousePressEvent(QMouseEvent *e)
{
  pressed_  = true;
  pressPos_ = e->pos();
}

void
CQRangeScrollHandle::
mouseMoveEvent(QMouseEvent *e)
{
  if (pressed_) {
    mousePos_ = scroll_->canvas()->mapFromGlobal(e->globalPos());

    double pos;

    if (scroll_->orientation() == Qt::Horizontal)
      pos = scroll_->pixelToPos(mousePos_.x());
    else
      pos = scroll_->pixelToPos(mousePos_.y());

    emit valueChanged(pos);
  }
}

void
CQRangeScrollHandle::
mouseReleaseEvent(QMouseEvent *)
{
  pressed_ = false;
}
