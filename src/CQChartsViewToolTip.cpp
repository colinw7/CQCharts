#include <CQChartsViewToolTip.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

//#include <QTextEdit>
#include <QLabel>

#include <iostream>

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
widget() const
{
  return widget_;
}

void
CQChartsViewToolTip::
setFont(const QFont &font)
{
  font_ = font;

  if (widget_)
    widget_->setFont(font_);
}

QWidget *
CQChartsViewToolTip::
showWidget(const QPoint &gpos)
{
  if (! widget_) {
    widget_ = CQUtil::makeLabelWidget<QLabel>("", "label");
  //widget_ = CQUtil::makeLabelWidget<QTextEdit>("", "label");
  //widget_->setReadOnly(true);

    widget_->setFont(font_);
  }

  if (! updateWidget(gpos))
    return nullptr;

  return widget_;
}

void
CQChartsViewToolTip::
hideWidget()
{
  delete widget_;

  widget_ = nullptr;
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

  gpos_ = gpos;

  widget_->setText("<font></font>" + tip);

  return true;
}

bool
CQChartsViewToolTip::
isHideKey(int key, Qt::KeyboardModifiers mod) const
{
  if (key == Qt::Key_Tab || key == Qt::Key_Backtab ||
      key == Qt::Key_F1 || key == Qt::Key_Question)
    return false;

  return CQToolTipIFace::isHideKey(key, mod);
}

bool
CQChartsViewToolTip::
isVisible() const
{
  return widget_ && widget_->isVisible();
}

QSize
CQChartsViewToolTip::
sizeHint() const
{
  return widget_->sizeHint();
}

//------

CQChartsViewFloatTip::
CQChartsViewFloatTip(CQChartsView *view) :
 CQFloatTip(view), view_(view)
{
}

void
CQChartsViewFloatTip::
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
CQChartsViewFloatTip::
updateTip()
{
  if (view_->mode() != CQChartsView::Mode::SELECT)
    return false;

  QString tip;

  if (! view_->calcGlobalTip(tipPos_, tip))
    return false;

  CQFloatTip::setText("<font></font>" + tip, /*resize*/false);

  if (CQFloatTip::resizeFit() && ! CQFloatTip::isOnScreen())
    CQFloatTip::place();
 
  return true;
}

void
CQChartsViewFloatTip::
hideTip()
{
  CQFloatTip::hideTip();
}

void
CQChartsViewFloatTip::
showQuery(const QPoint &)
{
  auto pos = view_->mapFromGlobal(tipPos_);

  auto wpos = view_->pixelToWindow(CQChartsGeom::Point(pos));

  view_->showQueryAt(wpos);
}

bool
CQChartsViewFloatTip::
isIgnoreKey(Qt::Key key, Qt::KeyboardModifiers modifiers) const
{
  if (key == Qt::Key_Tab || key == Qt::Key_Backtab ||
      key == Qt::Key_F1 || key == Qt::Key_Question)
    return true;

  return CQFloatTip::isIgnoreKey(key, modifiers);
}
