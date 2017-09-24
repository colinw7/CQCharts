#include <CQChartsDataLabel.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQRotatedText.h>
#include <QPainter>

CQChartsDataLabel::
CQChartsDataLabel(CQChartsPlot *plot) :
 plot_(plot)
{
}

void
CQChartsDataLabel::
addProperties(const QString &path)
{
  plot_->addProperty(path, this, "visible" );
  plot_->addProperty(path, this, "position");
  plot_->addProperty(path, this, "clip"    );
  plot_->addProperty(path, this, "angle"   );
  plot_->addProperty(path, this, "font"    );
  plot_->addProperty(path, this, "color"   );

  QString boxPath = plot_->id() + "/" + path + "/box";

  CQChartsBoxObj::addProperties(plot_->propertyView(), boxPath);
}

void
CQChartsDataLabel::
redrawBoxObj()
{
  update();
}

void
CQChartsDataLabel::
update()
{
  plot_->update();
}

void
CQChartsDataLabel::
draw(QPainter *p, const QRectF &qrect, const QString &ystr)
{
  if (! isVisible())
    return;

  p->save();

  p->setFont(font());

  QFontMetrics fm(p->font());

  double ym = 2;

  double b1 = CQChartsBoxObj::margin();
  double b2 = CQChartsBoxObj::padding();

  double b = b1 + b2;

  if (fabs(angle()) <= 1E-3) {
    int tw = fm.width(ystr);

    double x = 0.0, y = 0.0;

    if      (position() == Position::TOP_INSIDE) {
      x = qrect.center().x();
      y = qrect.top() + fm.ascent() + ym;
    }
    else if (position() == Position::TOP_OUTSIDE) {
      x = qrect.center().x();
      y = qrect.top() - fm.descent() - ym;
    }
    else if (position() == Position::BOTTOM_INSIDE) {
      x = qrect.center().x();
      y = qrect.bottom() - fm.descent() - ym;
    }
    else if (position() == Position::BOTTOM_OUTSIDE) {
      x = qrect.center().x();
      y = qrect.bottom() + fm.ascent() + ym;
    }
    else if (position() == Position::CENTER) {
      x = qrect.center().x();
      y = qrect.center().y() - (fm.ascent() - fm.descent())/2;
    }

    bool clipped = false;

    if (isClip()) {
      if (tw >= qrect.width())
        clipped = true;
    }

    if (isClip()) {
      if (position() == Position::TOP_INSIDE ||
          position() == Position::BOTTOM_INSIDE ||
          position() == Position::CENTER) {
        p->setClipRect(qrect, Qt::ReplaceClip);
      }
    }

    QRectF brect(x - tw/2 - b, y - fm.ascent() - b, tw + 2*b, fm.ascent() + fm.descent() + 2*b);

    CQChartsBoxObj::draw(p, brect);

    if (! clipped) {
      p->setPen(color());

      p->drawText(x - tw/2, y, ystr);
    }
  }
  else {
    double x = 0.0, y = 0.0;

    if      (position() == Position::TOP_INSIDE) {
      x = qrect.center().x();
      y = qrect.top();
    }
    else if (position() == Position::TOP_OUTSIDE) {
      x = qrect.center().x();
      y = qrect.top();
    }
    else if (position() == Position::BOTTOM_INSIDE) {
      x = qrect.center().x();
      y = qrect.bottom();
    }
    else if (position() == Position::BOTTOM_OUTSIDE) {
      x = qrect.center().x();
      y = qrect.bottom();
    }
    else if (position() == Position::CENTER) {
      x = qrect.center().x();
      y = qrect.center().y();
    }

    QRectF                bbox;
    CQRotatedText::Points points;

    CQRotatedText::bboxData(x, y, ystr, p->font(), angle(), b1, bbox, points);

    double dx = (bbox.center().x() - x);
    double dy = 0.0;

    if      (position() == Position::TOP_INSIDE) {
      dy = (bbox.top() - y) + ym;
    }
    else if (position() == Position::TOP_OUTSIDE) {
      dy = (bbox.bottom() - y) - ym;
    }
    else if (position() == Position::BOTTOM_INSIDE) {
      dy = (bbox.bottom() - y) - ym;
    }
    else if (position() == Position::BOTTOM_OUTSIDE) {
      dy = (bbox.top() - y) + ym;
    }
    else if (position() == Position::CENTER) {
      dy = (bbox.center().y() - y);
    }

    x -= dx;
    y -= dy;

    double a1 = CQChartsUtil::Deg2Rad(angle());

    double c = cos(a1);
    double s = sin(a1);

    double b1x = b1;
    double b1y = b1 + fm.descent();

    double b1xc = c*b1x;
    double b1xs = s*b1x;
    double b1yc = c*b1y;
    double b1ys = s*b1y;

    double b1dx = b1xc - b1ys;
    double b1dy = b1xs + b1yc;

    x += b1dx;
    y -= b1dy;

    p->setPen(color());

    if (CQChartsUtil::isInteger(angle()/90)) {
      bbox.translate(-dx, -dy);

      CQChartsBoxObj::draw(p, bbox);
    }
    else {
      QPolygonF poly;

      for (std::size_t i = 0; i < points.size(); ++i)
        poly << points[i] + QPointF(-dx, -dy);

      CQChartsBoxObj::draw(p, poly);
    }

    CQRotatedText::drawRotatedText(p, x, y, ystr, -angle());

    //p->setPen(Qt::red);
    //p->setBrush(Qt::NoBrush);

    //p->drawRect(bbox);
  }

  p->restore();
}
