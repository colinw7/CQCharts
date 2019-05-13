#include <CQChartsDataLabel.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQChartsRotatedText.h>
#include <CQChartsDrawUtil.h>

#include <CQPropertyViewItem.h>

#include <QPainter>

CQChartsDataLabel::
CQChartsDataLabel(CQChartsPlot *plot) :
 CQChartsTextBoxObj(plot)
{
  setVisible(false);
}

void
CQChartsDataLabel::
addPathProperties(const QString &path, const QString &desc)
{
  QString desc1 = (desc.length() ? desc : "Data label");

  plot_->addProperty(path, this, "visible" )->setDesc(desc + " is visible");
  plot_->addProperty(path, this, "position")->setDesc(desc + " position");
  plot_->addProperty(path, this, "clip"    )->setDesc(desc + " is clipped");

  QString textPath = path + "/text";

  QString textDesc = (desc.length() ? desc + " text" : "Text");

  plot_->addProperty(textPath, this, "textColor"   , "color"   )->
    setDesc(textDesc + " color");
  plot_->addProperty(textPath, this, "textAlpha"   , "alpha"   )->
    setDesc(textDesc + " alpha");
  plot_->addProperty(textPath, this, "textFont"    , "font"    )->
    setDesc(textDesc + " font");
  plot_->addProperty(textPath, this, "textAngle"   , "angle"   )->
    setDesc(textDesc + " angle");
  plot_->addProperty(textPath, this, "textContrast", "contrast")->
    setDesc(textDesc + " is contrast");
  plot_->addProperty(textPath, this, "textHtml"    , "html"    )->
    setDesc(textDesc + " is HTML");

  QString boxPath = path + "/box";

  CQChartsBoxObj::addProperties(plot_->propertyModel(), boxPath, desc);
}

void
CQChartsDataLabel::
draw(QPainter *painter, const QRectF &qrect, const QString &ystr) const
{
  draw(painter, qrect, ystr, position());
}

void
CQChartsDataLabel::
draw(QPainter *painter, const QRectF &qrect, const QString &ystr, const Position &position) const
{
  if (! isVisible())
    return;

  QPen tpen;

  QColor tc = interpTextColor(ColorInd());

  plot_->setPen(tpen, true, tc, textAlpha());

  draw(painter, qrect, ystr, position, tpen);
}

void
CQChartsDataLabel::
draw(QPainter *painter, const QRectF &qrect, const QString &ystr,
     const Position &position, const QPen &tpen) const
{
  painter->save();

  plot_->view()->setPlotPainterFont(plot_, painter, textFont());

  //---

  QRectF prect;

  double xm = 2;
  double ym = 2;

  double b1 = CQChartsBoxObj::margin();
  double b2 = CQChartsBoxObj::padding();

  //double b = b1 + b2;

  if (CMathUtil::isZero(textAngle())) {
    QFontMetricsF fm(painter->font());

    double tw = fm.width(ystr);

    // calc text position
    double x, y;

    if (direction() == Qt::Vertical) {
      x = qrect.center().x() - tw/2;
      y = 0.0;
    }
    else {
      x = 0.0;
      y = qrect.center().y() + (fm.ascent() - fm.descent())/2;
    }

    if      (position == Position::TOP_INSIDE) {
      if (direction() == Qt::Vertical) {
        if (! plot_->isInvertY())
          y = qrect.top   () + fm.ascent () + ym + b2;
        else
          y = qrect.bottom() - fm.descent() - ym - b2;
      }
      else {
        if (! plot_->isInvertX())
          x = qrect.right() - tw - xm - b2;
        else
          x = qrect.left () + ym + b2;
      }
    }
    else if (position == Position::TOP_OUTSIDE) {
      if (direction() == Qt::Vertical) {
        if (! plot_->isInvertY())
          y = qrect.top   () - fm.descent() - ym - b2;
        else
          y = qrect.bottom() + fm.ascent () + ym + b2;
      }
      else {
        if (! plot_->isInvertX())
          x = qrect.right() + xm + b2;
        else
          x = qrect.left () - tw - ym - b2;
      }
    }
    else if (position == Position::BOTTOM_INSIDE) {
      if (direction() == Qt::Vertical) {
        if (! plot_->isInvertY())
          y = qrect.bottom() - fm.descent() - ym - b2;
        else
          y = qrect.top   () + fm.ascent () + ym + b2;
      }
      else {
        if (! plot_->isInvertX())
          x = qrect.left() + xm + b2;
        else
          x = qrect.right() - tw - ym - b2;
      }
    }
    else if (position == Position::BOTTOM_OUTSIDE) {
      if (direction() == Qt::Vertical) {
        if (! plot_->isInvertY())
          y = qrect.bottom() + fm.ascent () + ym + b2;
        else
          y = qrect.top   () - fm.descent() - ym - b2;
      }
      else {
        if (! plot_->isInvertX())
          x = qrect.left() - tw - xm - b2;
        else
          x = qrect.right() + ym + b2;
      }
    }
    else if (position == Position::CENTER) {
      if (direction() == Qt::Vertical)
        y = qrect.center().y() + (fm.ascent() - fm.descent())/2;
      else
        x = qrect.center().x() - tw/2;
    }

    // clip if needed
    bool clipped = false;

    if (isClip()) {
      if (tw >= qrect.width())
        clipped = true;
    }

    if (isClip()) {
      if (position == Position::TOP_INSIDE ||
          position == Position::BOTTOM_INSIDE ||
          position == Position::CENTER) {
        painter->setClipRect(qrect);
      }
    }

    // draw box
    prect = QRectF(x - b1, y - fm.ascent() - b1, tw + 2*b1, fm.height() + 2*b1);

    CQChartsBoxObj::draw(painter, prect);

    if (! clipped) {
      if (ystr.length()) {
        painter->setPen(tpen);

        if (isTextContrast()) {
#if 0
          if (direction() == Qt::Vertical)
            CQChartsDrawUtil::drawContrastText(painter, x, y, ystr);
          else
            CQChartsDrawUtil::drawContrastText(painter, x, y, ystr);
#else
          CQChartsDrawUtil::drawContrastText(painter, x, y, ystr);
#endif
        }
        else {
#if 0
          if (direction() == Qt::Vertical)
            CQChartsDrawUtil::drawSimpleText(painter, QPointF(x, y), ystr);
          else
            CQChartsDrawUtil::drawSimpleText(painter, QPointF(x, y), ystr);
#else
          CQChartsDrawUtil::drawSimpleText(painter, QPointF(x, y), ystr);
#endif
        }
      }
    }
  }
  else {
    // TODO: handle horizontal and angle

    // calc text position
    double x = qrect.center().x();
    double y = 0.0;

    Qt::Alignment align = textAlignment();

    if      (position == Position::TOP_INSIDE)
      y = qrect.top   () + 2*b1;
    else if (position == Position::TOP_OUTSIDE)
      y = qrect.top   () - 2*b1;
    else if (position == Position::BOTTOM_INSIDE)
      y = qrect.bottom() - 2*b1;
    else if (position == Position::BOTTOM_OUTSIDE)
      y = qrect.bottom() + 2*b1;
    else if (position == Position::CENTER)
      y = qrect.center().y();

    CQChartsRotatedText::Points points;

    CQChartsRotatedText::bboxData(x, y, ystr, painter->font(), textAngle(), b1,
                                  prect, points, align, /*alignBBox*/ true);

    QPolygonF poly;

    for (std::size_t i = 0; i < points.size(); ++i)
      poly << points[i];

    CQChartsBoxObj::draw(painter, poly);

    painter->setPen(tpen);

    if (ystr.length())
      CQChartsRotatedText::draw(painter, x, y, ystr, textAngle(), align,
                                /*alignBBox*/ true, isTextContrast());
  }

  //---

  CQChartsGeom::BBox wrect = plot_->pixelToWindow(CQChartsUtil::fromQRect(prect));

  if (plot_->showBoxes()) {
    plot_->drawWindowColorBox(painter, wrect);
  }

  //---

  painter->restore();
}

CQChartsGeom::BBox
CQChartsDataLabel::
calcRect(const QRectF &qrect, const QString &ystr) const
{
  return calcRect(qrect, ystr, position());
}

CQChartsGeom::BBox
CQChartsDataLabel::
calcRect(const QRectF &qrect, const QString &ystr, const Position &position) const
{
  if (! isVisible())
    return CQChartsGeom::BBox();

  //---

  QRectF prect;

  double ym = 2;

  double b1 = CQChartsBoxObj::margin();
  double b2 = CQChartsBoxObj::padding();

  //double b = b1 + b2;

  QFont font = plot_->view()->plotFont(plot_, textFont());

  if (CMathUtil::isZero(textAngle())) {
    QFontMetricsF fm(font);

    double tw = fm.width(ystr);

    double x = qrect.center().x();
    double y = 0.0;

    if      (position == Position::TOP_INSIDE) {
      if (! plot_->isInvertY())
        y = qrect.top   () + fm.ascent () + ym + b2;
      else
        y = qrect.bottom() - fm.descent() - ym - b2;
    }
    else if (position == Position::TOP_OUTSIDE) {
      if (! plot_->isInvertY())
        y = qrect.top   () - fm.descent() - ym - b2;
      else
        y = qrect.bottom() + fm.ascent () + ym + b2;
    }
    else if (position == Position::BOTTOM_INSIDE) {
      if (! plot_->isInvertY())
        y = qrect.bottom() - fm.descent() - ym - b2;
      else
        y = qrect.top   () + fm.ascent () + ym + b2;
    }
    else if (position == Position::BOTTOM_OUTSIDE) {
      if (! plot_->isInvertY())
        y = qrect.bottom() + fm.ascent () + ym + b2;
      else
        y = qrect.top   () - fm.descent() - ym - b2;
    }
    else if (position == Position::CENTER) {
      y = qrect.center().y() + (fm.ascent() - fm.descent())/2;
    }

    prect = QRectF(x - tw/2 - b1, y - fm.ascent() - b1, tw + 2*b1, fm.height() + 2*b1);
  }
  else {
    double x = qrect.center().x();
    double y = 0.0;

    Qt::Alignment align = textAlignment();

    if      (position == Position::TOP_INSIDE)
      y = qrect.top   () + 2*b1;
    else if (position == Position::TOP_OUTSIDE)
      y = qrect.top   () - 2*b1;
    else if (position == Position::BOTTOM_INSIDE)
      y = qrect.bottom() - 2*b1;
    else if (position == Position::BOTTOM_OUTSIDE)
      y = qrect.bottom() + 2*b1;
    else if (position == Position::CENTER)
      y = qrect.center().y();

    CQChartsRotatedText::Points points;

    CQChartsRotatedText::bboxData(x, y, ystr, font, textAngle(), b1,
                                  prect, points, align, /*alignBBox*/ true);
  }

  //---

  CQChartsGeom::BBox wrect = plot_->pixelToWindow(CQChartsUtil::fromQRect(prect));

  return wrect;
}

Qt::Alignment
CQChartsDataLabel::
textAlignment() const
{
  return textAlignment(position());
}

Qt::Alignment
CQChartsDataLabel::
textAlignment(const Position &position)
{
  Qt::Alignment align = Qt::AlignHCenter;

  if      (position == Position::TOP_INSIDE)
    align |= Qt::AlignTop;
  else if (position == Position::TOP_OUTSIDE)
    align |= Qt::AlignBottom;
  else if (position == Position::BOTTOM_INSIDE)
    align |= Qt::AlignBottom;
  else if (position == Position::BOTTOM_OUTSIDE)
    align |= Qt::AlignTop;
  else if (position == Position::CENTER)
    align |= Qt::AlignVCenter;

  return align;
}

void
CQChartsDataLabel::
textBoxDataInvalidate()
{
  if (isSendSignal())
    emit dataChanged();
  else
    CQChartsTextBoxObj::textBoxDataInvalidate();
}
