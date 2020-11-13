#include <CQChartsViewToolTip.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <iostream>

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

  return showTip(gpos);
}

bool
CQChartsViewToolTip::
showTip(const QPoint &gpos)
{
  QString tip;

  if (! view_->calcGlobalTip(gpos, tip))
    return false;

  qpos_ = gpos;

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
  auto oldPos = tipPos_;

  tipPos_ = gpos;

  if (! updateTip()) {
    tipPos_ = oldPos;
    return;
  }

  //std::cerr << "Tip Pos : " << tipPos_.x() << " " << tipPos_.y() << "\n";

  CQFloatTip::showTip(tipPos_);
}

bool
CQChartsViewToolTip::
updateTip()
{
  QString tip;

  if (! view_->calcGlobalTip(tipPos_, tip))
    return false;

  CQFloatTip::setText("<font></font>" + tip);

  return true;
}

void
CQChartsViewToolTip::
hideTip()
{
  CQFloatTip::hideTip();
}

void
CQChartsViewToolTip::
showQuery(const QPoint &)
{
  auto pos = view_->mapFromGlobal(tipPos_);

  auto wpos = view_->pixelToWindow(CQChartsGeom::Point(pos));

  view_->showQueryAt(wpos);
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
