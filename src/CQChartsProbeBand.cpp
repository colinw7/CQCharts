#include <CQChartsProbeBand.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <QRubberBand>

#include <QLabel>

CQChartsProbeBand::
CQChartsProbeBand(CQChartsView *view) :
 view_(view)
{
  vband_ = new QRubberBand(QRubberBand::Line, view);
  hband_ = new QRubberBand(QRubberBand::Line, view);
  tip_   = new QLabel;

  tip_->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
}

CQChartsProbeBand::
~CQChartsProbeBand()
{
  delete vband_;
  delete hband_;
  delete tip_;
}

void
CQChartsProbeBand::
show(CQChartsPlot *plot, const QString &text, double px, double py1, double py2)
{
  int tickLen = 8;

  CPoint2D p1(px          , py1);
  CPoint2D p2(px          , py2);
  CPoint2D p3(px + tickLen, py2);

  vband_->setGeometry(CQChartsUtil::toQRectI(CBBox2D(p1, p2)));
  hband_->setGeometry(CQChartsUtil::toQRectI(CBBox2D(p2, p3)));

  vband_->show();
  hband_->show();

  tip_->setText(text);

  CPoint2D p4(px + tickLen + 2, py2 - tip_->sizeHint().height()/2);

  QPoint pos = CQChartsUtil::toQPointI(p4);

  QPoint gpos = view_->mapToGlobal(pos);

  double wx, wy;

  view_->pixelToWindow(pos.x(), pos.y(), wx, wy);

  CBBox2D bbox = view_->plotBBox(plot);

  if (bbox.inside(CPoint2D(wx, wy))) {
    QFontMetrics fm(font());

    tip_->move(gpos);

    tip_->resize(fm.width(text) + 4, fm.height() + 4);

    tip_->show();
  }
  else
    tip_->hide();
}

void
CQChartsProbeBand::
hide()
{
  vband_->hide();
  hband_->hide();
  tip_  ->hide();
}
