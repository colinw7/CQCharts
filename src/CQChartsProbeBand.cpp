#include <CQChartsProbeBand.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>

#include <QRubberBand>
#include <QLabel>

using Point = CQChartsGeom::Point;
using BBox  = CQChartsGeom::BBox;

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

  Point p1(px          , py1);
  Point p2(px          , py2);
  Point p3(px + tickLen, py2);

  vband_->setGeometry(BBox(p1, p2).qrecti());
  hband_->setGeometry(BBox(p2, p3).qrecti());

  vband_->show();
  hband_->show();

  if (text.length()) {
    tip_->setText(text);

    Point p4(px + tickLen + 2, py2 - tip_->sizeHint().height()/2.0);

    auto pos = p4.qpointi();

    auto gpos = view_->mapToGlobal(pos);

    auto wpos = view_->pixelToWindow(Point(pos.x(), pos.y()));

    auto bbox = view_->plotBBox(plot);

    if (bbox.inside(wpos)) {
      QFontMetrics fm(font());

      tip_->move(gpos);

      tip_->resize(fm.width(text) + 4, fm.height() + 4);

      tip_->show();
    }
    else
      tip_->hide();
  }
  else
    tip_->hide();
}

void
CQChartsProbeBand::
showHorizontal(CQChartsPlot *plot, const QString &text, double px1, double px2, double py)
{
  int tickLen = 8;

  Point p1(px1, py);
  Point p2(px2, py);
  Point p3(px2, py - tickLen);

  hband_->setGeometry(BBox(p1, p2).qrecti());
  vband_->setGeometry(BBox(p2, p3).qrecti());

  hband_->show();
  vband_->show();

  if (text.length()) {
    tip_->setText(text);

    Point p4(px2 -  tip_->sizeHint().width()/2.0, py - tickLen - 2 - tip_->sizeHint().height());

    auto pos = p4.qpointi();

    auto gpos = view_->mapToGlobal(pos);

    auto wpos = view_->pixelToWindow(Point(pos.x(), pos.y()));

    auto bbox = view_->plotBBox(plot);

    if (bbox.inside(wpos)) {
      QFontMetrics fm(font());

      tip_->move(gpos);

      tip_->resize(fm.width(text) + 4, fm.height() + 4);

      tip_->show();
    }
    else
      tip_->hide();
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
