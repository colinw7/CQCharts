#include <CQChartsViewToolTip.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsKey.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>
#include <QLabel>

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
  QPoint p = view_->mapFromGlobal(gpos);

  CQChartsGeom::Point wpos = view_->pixelToWindow(CQChartsGeom::Point(p.x(), p.y()));

  CQChartsView::Plots plots;

  view_->plotsAt(wpos, plots);

  if (plots.empty())
    return false;

  QString tip;

  for (const auto &plot : plots) {
    CQChartsGeom::Point w = plot->pixelToWindow(CQChartsGeom::Point(QPointF(p)));

    QString tip1;

    if (plot->tipText(w, tip1)) {
      if (tip.length())
        tip += "\n";

      tip += tip1;
    }
  }

  if (! tip.length()) {
    for (const auto &plot : plots) {
      CQChartsPlotKey *key = plot->key();
      if (! key) continue;

      CQChartsGeom::Point w = plot->pixelToWindow(CQChartsGeom::Point(QPointF(p)));

      if (key->contains(w)) {
        QString tip1;

        if (key->tipText(w, tip1)) {
          if (tip.length())
            tip += "\n";

          tip += tip1;
        }
      }
    }
  }

  if (! tip.length())
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
