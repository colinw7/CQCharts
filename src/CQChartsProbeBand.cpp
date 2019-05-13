#include <CQChartsProbeBand.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <QRubberBand>
#include <QLabel>

CQChartsProbeBand::
CQChartsProbeBand(CQChartsView *view) :
 view_(view)
{
  vband_ = new QRubberBand(QRubberBand::Line, view);
  hband_ = new QRubberBand(QRubberBand::Line, view);
  tip_   = CQUtil::makeLabelWidget<QLabel>("", "tipLabel");

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
showVertical(CQChartsPlot *plot, const QString &text, double px, double py1, double py2)
{
  int tickLen = 8;

  CQChartsGeom::Point p1(px          , py1);
  CQChartsGeom::Point p2(px          , py2);
  CQChartsGeom::Point p3(px + tickLen, py2);

  vband_->setGeometry(CQChartsUtil::toQRectI(CQChartsGeom::BBox(p1, p2)));
  hband_->setGeometry(CQChartsUtil::toQRectI(CQChartsGeom::BBox(p2, p3)));

  vband_->show();
  hband_->show();

  tip_->setText(text);

  CQChartsGeom::Point p4(px + tickLen + 2, py2 - tip_->sizeHint().height()/2);

  QPoint pos = CQChartsUtil::toQPointI(p4);

  QPoint gpos = view_->mapToGlobal(pos);

  CQChartsGeom::Point wpos = view_->pixelToWindow(CQChartsGeom::Point(pos.x(), pos.y()));

  CQChartsGeom::BBox bbox = view_->plotBBox(plot);

  if (bbox.inside(wpos)) {
    QFontMetricsF fm(font());

    tip_->move(gpos);

    tip_->resize(fm.width(text) + 4, fm.height() + 4);

    tip_->show();
  }
  else
    tip_->hide();
}

void
CQChartsProbeBand::
showHorizontal(CQChartsPlot *plot, const QString &text, double px1, double px2, double py)
{
  int tickLen = 8;

  CQChartsGeom::Point p1(px1, py);
  CQChartsGeom::Point p2(px2, py);
  CQChartsGeom::Point p3(px2, py - tickLen);

  hband_->setGeometry(CQChartsUtil::toQRectI(CQChartsGeom::BBox(p1, p2)));
  vband_->setGeometry(CQChartsUtil::toQRectI(CQChartsGeom::BBox(p2, p3)));

  hband_->show();
  vband_->show();

  tip_->setText(text);

  CQChartsGeom::Point p4(px2 -  tip_->sizeHint().width()/2,
                         py - tickLen - 2 - tip_->sizeHint().height());

  QPoint pos = CQChartsUtil::toQPointI(p4);

  QPoint gpos = view_->mapToGlobal(pos);

  CQChartsGeom::Point wpos = view_->pixelToWindow(CQChartsGeom::Point(pos.x(), pos.y()));

  CQChartsGeom::BBox bbox = view_->plotBBox(plot);

  if (bbox.inside(wpos)) {
    QFontMetricsF fm(font());

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
