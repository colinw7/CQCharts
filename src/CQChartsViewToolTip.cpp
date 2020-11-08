#include <CQChartsViewToolTip.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#ifndef CQCHARTS_FLOAT_TIP
CQChartsViewToolTip::
CQChartsViewToolTip(CQChartsView *view) :
 view_(view) {
}

CQChartsViewToolTip::
~CQChartsViewToolTip()
{
  delete widget_;
}

QWidget *
CQChartsViewToolTip::
showWidget(const QPoint &gpos)
{
  if (! widget_)
    widget_ = CQUtil::makeLabelWidget<QLabel>("", "label");

  if (! updateWidget(gpos))
    return nullptr;

  return widget_;
}

void
CQChartsViewToolTip::
hideWidget()
{
  delete widget_;

  widget_ = 0;
}

bool
CQChartsViewToolTip::
updateWidget(const QPoint &gpos)
{
  if (! widget_) return false;

  if (view_->mode() != CQChartsView::Mode::SELECT)
    return false;

  gpos_ = gpos;

  return showTip(gpos);
}

bool
CQChartsViewToolTip::
showTip(const QPoint &gpos)
{
  QString tip;

  if (! view_->calcTip(gpos, tip))
    return false;

  widget_->setText("<font></font>" + tip);

  return true;
}

bool
CQChartsViewToolTip::
isHideKey(int key, Qt::KeyboardModifiers mod) const
{
  if (key == Qt::Key_Tab || key == Qt::Key_Backtab || key == Qt::Key_F1)
    return false;

  return CQToolTipIFace::isHideKey(key, mod);
}

QSize
CQChartsViewToolTip::
sizeHint() const
{
  return widget_->sizeHint();
}
#else
CQChartsViewToolTip::
CQChartsViewToolTip(CQChartsView *view) :
 CQFloatTip(view), view_(view) {
}

void
CQChartsViewToolTip::
showTip(const QPoint &gpos)
{
  tipPos_ = gpos;

  updateTip();

  CQFloatTip::showTip(gpos);
}

void
CQChartsViewToolTip::
updateTip()
{
  QString tip;

  if (! view_->calcTip(tipPos_, tip))
    return;

  CQFloatTip::setText("<font></font>" + tip);
}

void
CQChartsViewToolTip::
hideTip()
{
  CQFloatTip::hideTip();
}

bool
CQChartsViewToolTip::
isIgnoreKey(Qt::Key key, Qt::KeyboardModifiers modifiers) const
{
  if (key == Qt::Key_Tab || key == Qt::Key_Backtab || key == Qt::Key_F1)
    return true;

  return CQFloatTip::isIgnoreKey(key, modifiers);
}

#endif
