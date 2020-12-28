#include <CQScrollArea.h>

#include <QApplication>
#include <QGridLayout>
#include <QScrollBar>
#include <QWheelEvent>

CQScrollArea::
CQScrollArea(QWidget *parent, QWidget *widget) :
 QFrame(parent), widget_(widget)
{
  init();
}

CQScrollArea::
CQScrollArea(QWidget *widget) :
 QFrame(nullptr), widget_(widget)
{
  init();
}

void
CQScrollArea::
init()
{
  setObjectName("scrollArea");

  auto *grid = new QGridLayout(this);
  grid->setMargin(0); grid->setSpacing(0);

  hbar_ = new QScrollBar(Qt::Horizontal);
  vbar_ = new QScrollBar(Qt::Vertical  );

  hbar_->setObjectName("hbar");
  vbar_->setObjectName("vbar");

  if (widget_) {
    grid->addWidget(widget_, 0, 0);

    setFocusProxy(widget_);
  }

  grid->addWidget(hbar_, 1, 0);
  grid->addWidget(vbar_, 0, 1);

  QObject::connect(hbar_, SIGNAL(valueChanged(int)), this, SLOT(hscrollSlot(int)));
  QObject::connect(vbar_, SIGNAL(valueChanged(int)), this, SLOT(vscrollSlot(int)));
}

void
CQScrollArea::
setWidget(QWidget *widget)
{
  assert(widget && ! widget_);

  auto *grid = qobject_cast<QGridLayout *>(layout());
  assert(grid);

  widget_ = widget;

  grid->addWidget(widget_, 0, 0);

  setFocusProxy(widget_);
}

void
CQScrollArea::
showHBar(bool flag)
{
  flag ? hbar_->show() : hbar_->hide();
}

void
CQScrollArea::
showVBar(bool flag)
{
  flag ? vbar_->show() : vbar_->hide();
}

void
CQScrollArea::
setCornerWidget(QWidget *w)
{
  QGridLayout *grid = qobject_cast<QGridLayout *>(layout());

  grid->addWidget(w, 1, 1);
}

void
CQScrollArea::
setXSize(int x_size)
{
  x_size_ = x_size;

  updateScrollbars();
}

void
CQScrollArea::
setYSize(int y_size)
{
  y_size_ = y_size;

  updateScrollbars();
}

void
CQScrollArea::
setXOffset(int x_offset)
{
  x_offset_ = x_offset;

  updateScrollbars();
}

void
CQScrollArea::
setYOffset(int y_offset)
{
  y_offset_ = y_offset;

  updateScrollbars();
}

void
CQScrollArea::
setXSingleStep(int x)
{
  hbar_->setSingleStep(x);
}

void
CQScrollArea::
setYSingleStep(int y)
{
  vbar_->setSingleStep(y);
}

void
CQScrollArea::
scrollUp(bool page)
{
  vbar_->triggerAction(page ? QAbstractSlider::SliderPageStepSub :
                              QAbstractSlider::SliderSingleStepSub);
}

void
CQScrollArea::
scrollDown(bool page)
{
  vbar_->triggerAction(page ? QAbstractSlider::SliderPageStepAdd :
                              QAbstractSlider::SliderSingleStepAdd);
}

void
CQScrollArea::
scrollLeft(bool page)
{
  hbar_->triggerAction(page ? QAbstractSlider::SliderPageStepSub :
                              QAbstractSlider::SliderSingleStepSub);
}

void
CQScrollArea::
scrollRight(bool page)
{
  hbar_->triggerAction(page ? QAbstractSlider::SliderPageStepAdd :
                              QAbstractSlider::SliderSingleStepAdd);
}

void
CQScrollArea::
ensureVisible(int x, int y, int xmargin, int ymargin)
{
  int xmin = -getXOffset();
  int xmax = xmin + width();
  int ymin = -getYOffset();
  int ymax = ymin + height();

  if      (y < ymin)
    setYOffset(-y + ymargin);
  else if (y > ymax)
    setYOffset(-y + height() - ymargin);

  if      (x < xmin)
    setXOffset(-x + xmargin);
  else if (x > xmax)
    setXOffset(-x + width() - xmargin);
}

void
CQScrollArea::
updateScrollbars()
{
  int w = (widget_ ? widget_->width () : 100);
  int h = (widget_ ? widget_->height() : 100);

  int dx = std::max(0, getXSize() - w);
  int dy = std::max(0, getYSize() - h);

  hbar_->setPageStep(w);

  hbar_->setMinimum(0);
  hbar_->setMaximum(dx);

  hbar_->setValue(-getXOffset());

  vbar_->setPageStep(h);

  vbar_->setMinimum(0);
  vbar_->setMaximum(dy);

  vbar_->setValue(-getYOffset());
}

void
CQScrollArea::
hscrollSlot(int value)
{
  setXOffset(-value);

  updateContents();

  emit updateArea();
}

void
CQScrollArea::
vscrollSlot(int value)
{
  setYOffset(-value);

  updateContents();

  emit updateArea();
}

void
CQScrollArea::
handleWheelEvent(QWheelEvent *e)
{
  if (static_cast<QWheelEvent*>(e)->orientation() == Qt::Horizontal)
    QApplication::sendEvent(hbar_, e);
  else
    QApplication::sendEvent(vbar_, e);
}
