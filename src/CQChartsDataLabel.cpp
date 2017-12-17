#include <CQChartsDataLabel.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQChartsRotatedText.h>
#include <QPainter>

CQChartsDataLabel::
CQChartsDataLabel(CQChartsPlot *plot) :
 CQChartsTextBoxObj(plot)
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

  CQChartsBoxObj::addProperties(plot_->propertyModel(), boxPath);
}

void
CQChartsDataLabel::
update()
{
  // update range ?
  plot_->update();
}

void
CQChartsDataLabel::
draw(QPainter *painter, const QRectF &qrect, const QString &ystr)
{
  if (! isVisible())
    return;

  painter->save();

  painter->setFont(font());

  //---

  QRectF prect;

  double ym = 2;

  double b1 = CQChartsBoxObj::margin();
  double b2 = CQChartsBoxObj::padding();

  //double b = b1 + b2;

  if (CQChartsUtil::isZero(angle())) {
    QFontMetricsF fm(painter->font());

    double tw = fm.width(ystr);

    double x = qrect.center().x();
    double y = 0.0;

    if      (position() == Position::TOP_INSIDE) {
      if (! plot_->isInvertY())
        y = qrect.top   () + fm.ascent () + ym + b2;
      else
        y = qrect.bottom() - fm.descent() - ym - b2;
    }
    else if (position() == Position::TOP_OUTSIDE) {
      if (! plot_->isInvertY())
        y = qrect.top   () - fm.descent() - ym - b2;
      else
        y = qrect.bottom() + fm.ascent () + ym + b2;
    }
    else if (position() == Position::BOTTOM_INSIDE) {
      if (! plot_->isInvertY())
        y = qrect.bottom() - fm.descent() - ym - b2;
      else
        y = qrect.top   () + fm.ascent () + ym + b2;
    }
    else if (position() == Position::BOTTOM_OUTSIDE) {
      if (! plot_->isInvertY())
        y = qrect.bottom() + fm.ascent () + ym + b2;
      else
        y = qrect.top   () - fm.descent() - ym - b2;
    }
    else if (position() == Position::CENTER) {
      y = qrect.center().y() + (fm.ascent() - fm.descent())/2;
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
        painter->setClipRect(qrect);
      }
    }

    prect = QRectF(x - tw/2 - b1, y - fm.ascent() - b1, tw + 2*b1, fm.height() + 2*b1);

    CQChartsBoxObj::draw(painter, prect);

    if (! clipped) {
      painter->setPen(interpColor(0, 1));

      if (ystr.length())
        painter->drawText(QPointF(x - tw/2, y), ystr);
    }
  }
  else {
    double x = qrect.center().x();
    double y = 0.0;

    Qt::Alignment align = textAlignment();

    if      (position() == Position::TOP_INSIDE)
      y = qrect.top   () + 2*b1;
    else if (position() == Position::TOP_OUTSIDE)
      y = qrect.top   () - 2*b1;
    else if (position() == Position::BOTTOM_INSIDE)
      y = qrect.bottom() - 2*b1;
    else if (position() == Position::BOTTOM_OUTSIDE)
      y = qrect.bottom() + 2*b1;
    else if (position() == Position::CENTER)
      y = qrect.center().y();

    CQChartsRotatedText::Points points;

    CQChartsRotatedText::bboxData(x, y, ystr, painter->font(), angle(), b1,
                                  prect, points, align, /*alignBBox*/ true);

    painter->setPen(interpColor(0, 1));

    QPolygonF poly;

    for (std::size_t i = 0; i < points.size(); ++i)
      poly << points[i];

    CQChartsBoxObj::draw(painter, poly);

    painter->setPen(interpColor(0, 1));

    if (ystr.length())
      CQChartsRotatedText::drawRotatedText(painter, x, y, ystr, angle(), align,
                                           /*alignBBox*/ true);
  }

  //---

  CQChartsGeom::BBox wrect;

  plot_->pixelToWindow(CQChartsUtil::fromQRect(prect), wrect);

  if (plot_->showBoxes()) {
    plot_->drawWindowRedBox(painter, wrect);
  }

  //---

  painter->restore();
}

CQChartsGeom::BBox
CQChartsDataLabel::
calcRect(const QRectF &qrect, const QString &ystr) const
{
  if (! isVisible())
    return CQChartsGeom::BBox();

  //---

  QRectF prect;

  double ym = 2;

  double b1 = CQChartsBoxObj::margin();
  double b2 = CQChartsBoxObj::padding();

  //double b = b1 + b2;

  if (CQChartsUtil::isZero(angle())) {
    QFontMetricsF fm(font());

    double tw = fm.width(ystr);

    double x = qrect.center().x();
    double y = 0.0;

    if      (position() == Position::TOP_INSIDE) {
      if (! plot_->isInvertY())
        y = qrect.top   () + fm.ascent () + ym + b2;
      else
        y = qrect.bottom() - fm.descent() - ym - b2;
    }
    else if (position() == Position::TOP_OUTSIDE) {
      if (! plot_->isInvertY())
        y = qrect.top   () - fm.descent() - ym - b2;
      else
        y = qrect.bottom() + fm.ascent () + ym + b2;
    }
    else if (position() == Position::BOTTOM_INSIDE) {
      if (! plot_->isInvertY())
        y = qrect.bottom() - fm.descent() - ym - b2;
      else
        y = qrect.top   () + fm.ascent () + ym + b2;
    }
    else if (position() == Position::BOTTOM_OUTSIDE) {
      if (! plot_->isInvertY())
        y = qrect.bottom() + fm.ascent () + ym + b2;
      else
        y = qrect.top   () - fm.descent() - ym - b2;
    }
    else if (position() == Position::CENTER) {
      y = qrect.center().y() + (fm.ascent() - fm.descent())/2;
    }

    prect = QRectF(x - tw/2 - b1, y - fm.ascent() - b1, tw + 2*b1, fm.height() + 2*b1);
  }
  else {
    double x = qrect.center().x();
    double y = 0.0;

    Qt::Alignment align = textAlignment();

    if      (position() == Position::TOP_INSIDE)
      y = qrect.top   () + 2*b1;
    else if (position() == Position::TOP_OUTSIDE)
      y = qrect.top   () - 2*b1;
    else if (position() == Position::BOTTOM_INSIDE)
      y = qrect.bottom() - 2*b1;
    else if (position() == Position::BOTTOM_OUTSIDE)
      y = qrect.bottom() + 2*b1;
    else if (position() == Position::CENTER)
      y = qrect.center().y();

    CQChartsRotatedText::Points points;

    CQChartsRotatedText::bboxData(x, y, ystr, font(), angle(), b1,
                                  prect, points, align, /*alignBBox*/ true);
  }

  //---

  CQChartsGeom::BBox wrect;

  plot_->pixelToWindow(CQChartsUtil::fromQRect(prect), wrect);

  return wrect;
}

Qt::Alignment
CQChartsDataLabel::
textAlignment() const
{
  Qt::Alignment align = Qt::AlignHCenter;

  if      (position() == Position::TOP_INSIDE)
    align |= Qt::AlignTop;
  else if (position() == Position::TOP_OUTSIDE)
    align |= Qt::AlignBottom;
  else if (position() == Position::BOTTOM_INSIDE)
    align |= Qt::AlignBottom;
  else if (position() == Position::BOTTOM_OUTSIDE)
    align |= Qt::AlignTop;
  else if (position() == Position::CENTER)
    align |= Qt::AlignVCenter;

  return align;
}
