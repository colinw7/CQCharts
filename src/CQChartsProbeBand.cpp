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
  tip_   = new CQChartsProbeLabel(this);

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
showVertical(CQChartsPlot *plot, const QString &text,
             double px, double py1, double py2, double py3)
{
  orientation_ = Qt::Vertical;
  pos_         = px;
  value_       = py2;

  Point p1(px           , py1);
  Point p2(px           , py2);
  Point p3(px + tickLen_, py2);

  vband_->setGeometry(BBox(p1, p2).qrecti());
  hband_->setGeometry(BBox(p2, p3).qrecti());

  vband_->show();
  hband_->show();

  if (text.length()) {
    tip_->setText(text);

    int pl = px + tickLen_ + 2;

    Point p4;

    if      (labelPos_ == LabelPos::MIN)
      p4 = Point(pl, py1 - tip_->sizeHint().height());
    else if (labelPos_ == LabelPos::VALUE)
      p4 = Point(pl, py2 - tip_->sizeHint().height()/2.0);
    else if (labelPos_ == LabelPos::MAX)
      p4 = Point(pl, py3);

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

  visible_ = true;
}

void
CQChartsProbeBand::
showHorizontal(CQChartsPlot *plot, const QString &text,
               double px1, double px2, double px3, double py)
{
  orientation_ = Qt::Horizontal;
  pos_         = py;
  value_       = px2;

  Point p1(px1, py           );
  Point p2(px2, py           );
  Point p3(px2, py - tickLen_);

  hband_->setGeometry(BBox(p1, p2).qrecti());
  vband_->setGeometry(BBox(p2, p3).qrecti());

  hband_->show();
  vband_->show();

  if (text.length()) {
    tip_->setText(text);

    int pl = py - tickLen_ - 2 - tip_->sizeHint().height();

    Point p4;

    if      (labelPos_ == LabelPos::MIN)
      p4 = Point(px1                               , pl);
    else if (labelPos_ == LabelPos::VALUE)
      p4 = Point(px2 - tip_->sizeHint().width()/2.0, pl);
    else if (labelPos_ == LabelPos::MAX)
      p4 = Point(px3 - tip_->sizeHint().width()    , pl);

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

  visible_ = true;
}

void
CQChartsProbeBand::
hide()
{
  visible_ = false;

  vband_->hide();
  hband_->hide();
  tip_  ->hide();
}

QRect
CQChartsProbeBand::
labelRect() const
{
  return tip_->geometry();
}

void
CQChartsProbeBand::
moveLabel(int dx, int dy)
{
  tip_->move(tip_->pos() + QPoint(dx, dy));
}

//---

CQChartsProbeLabel::
CQChartsProbeLabel(CQChartsProbeBand *band) :
 QLabel(""), band_(band)
{
  setObjectName("tipLabel");
}

CQChartsProbeLabel::
~CQChartsProbeLabel()
{
}
