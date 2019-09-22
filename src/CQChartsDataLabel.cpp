#include <CQChartsDataLabel.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQChartsRotatedText.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsVariant.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

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
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(plot()->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc) {
    CQPropertyViewItem *item = addProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  addProp(path, "visible" , "", desc + " visible");
  addProp(path, "position", "", desc + " position");
  addProp(path, "clip"    , "", desc + " is clipped");

  QString textPath = path + "/text";

  addStyleProp(textPath, "textColor"   , "color"   , desc + " text color");
  addStyleProp(textPath, "textAlpha"   , "alpha"   , desc + " text alpha");
  addStyleProp(textPath, "textFont"    , "font"    , desc + " text font");
  addStyleProp(textPath, "textAngle"   , "angle"   , desc + " text angle");
  addStyleProp(textPath, "textContrast", "contrast", desc + " text is contrast");
  addStyleProp(textPath, "textHtml"    , "html"    , desc + " text is HTML");

  QString boxPath = path + "/box";

  CQChartsBoxObj::addProperties(plot()->propertyModel(), boxPath, desc);
}

#if 0
void
CQChartsDataLabel::
draw(QPainter *painter, const QRectF &qrect, const QString &ystr) const
{
  draw(painter, qrect, ystr, position());
}
#endif

void
CQChartsDataLabel::
draw(CQChartsPaintDevice *device, const QRectF &qrect, const QString &ystr) const
{
  draw(device, qrect, ystr, position());
}

#if 0
void
CQChartsDataLabel::
draw(QPainter *painter, const QRectF &qrect, const QString &ystr, const Position &position) const
{
  if (! isVisible())
    return;

  QPen tpen;

  QColor tc = interpTextColor(ColorInd());

  plot()->setPen(tpen, true, tc, textAlpha());

  draw(painter, qrect, ystr, position, tpen);
}
#endif

void
CQChartsDataLabel::
draw(CQChartsPaintDevice *device, const QRectF &qrect, const QString &ystr,
     const Position &position) const
{
  if (! isVisible())
    return;

  QPen tpen;

  QColor tc = interpTextColor(ColorInd());

  plot()->setPen(tpen, true, tc, textAlpha());

  draw(device, qrect, ystr, position, tpen);
}

#if 0
void
CQChartsDataLabel::
draw(QPainter *painter, const QRectF &qrect, const QString &ystr,
     const Position &position, const QPen &tpen) const
{
  painter->save();

  plot()->view()->setPlotPainterFont(plot(), painter, textFont());

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
        if (! plot()->isInvertY())
          y = qrect.top   () + fm.ascent () + ym + b2;
        else
          y = qrect.bottom() - fm.descent() - ym - b2;
      }
      else {
        if (! plot()->isInvertX())
          x = qrect.right() - tw - xm - b2;
        else
          x = qrect.left () + ym + b2;
      }
    }
    else if (position == Position::TOP_OUTSIDE) {
      if (direction() == Qt::Vertical) {
        if (! plot()->isInvertY())
          y = qrect.top   () - fm.descent() - ym - b2;
        else
          y = qrect.bottom() + fm.ascent () + ym + b2;
      }
      else {
        if (! plot()->isInvertX())
          x = qrect.right() + xm + b2;
        else
          x = qrect.left () - tw - ym - b2;
      }
    }
    else if (position == Position::BOTTOM_INSIDE) {
      if (direction() == Qt::Vertical) {
        if (! plot()->isInvertY())
          y = qrect.bottom() - fm.descent() - ym - b2;
        else
          y = qrect.top   () + fm.ascent () + ym + b2;
      }
      else {
        if (! plot()->isInvertX())
          x = qrect.left() + xm + b2;
        else
          x = qrect.right() - tw - ym - b2;
      }
    }
    else if (position == Position::BOTTOM_OUTSIDE) {
      if (direction() == Qt::Vertical) {
        if (! plot()->isInvertY())
          y = qrect.bottom() + fm.ascent () + ym + b2;
        else
          y = qrect.top   () - fm.descent() - ym - b2;
      }
      else {
        if (! plot()->isInvertX())
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

        CQChartsPlotPainter device(plot(), painter);

        QPointF p1 = device.pixelToWindow(QPointF(x, y));

        if (isTextContrast()) {
#if 0
          if (direction() == Qt::Vertical)
            CQChartsDrawUtil::drawContrastText(&device, p1, ystr);
          else
            CQChartsDrawUtil::drawContrastText(&device, p1, ystr);
#else
          CQChartsDrawUtil::drawContrastText(&device, p1, ystr);
#endif
        }
        else {
#if 0
          if (direction() == Qt::Vertical)
            CQChartsDrawUtil::drawSimpleText(&device, p1, ystr);
          else
            CQChartsDrawUtil::drawSimpleText(&device, p1, ystr);
#else
          CQChartsDrawUtil::drawSimpleText(&device, p1, ystr);
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

    if (ystr.length()) {
      QPointF p1(x, y);

      CQChartsRotatedText::draw(painter, p1, ystr, textAngle(), align,
                                /*alignBBox*/ true, isTextContrast());
    }
  }

  //---

  CQChartsGeom::BBox wrect = plot()->pixelToWindow(CQChartsGeom::BBox(prect));

  if (plot()->showBoxes()) {
    CQChartsPlotPainter device(plot(), painter);

    plot()->drawWindowColorBox(&device, wrect);
  }

  //---

  painter->restore();
}
#endif

void
CQChartsDataLabel::
draw(CQChartsPaintDevice *device, const QRectF &qrect, const QString &ystr,
     const Position &position, const QPen &tpen) const
{
  device->save();

  plot()->view()->setPlotPainterFont(plot(), device, textFont());

  //---

  QRectF prect;

  double xm = 2;
  double ym = 2;

  double b1 = CQChartsBoxObj::margin();
  double b2 = CQChartsBoxObj::padding();

  //double b = b1 + b2;

  if (CMathUtil::isZero(textAngle())) {
    QFontMetricsF fm(device->font());

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
        if (! plot()->isInvertY())
          y = qrect.top   () + fm.ascent () + ym + b2;
        else
          y = qrect.bottom() - fm.descent() - ym - b2;
      }
      else {
        if (! plot()->isInvertX())
          x = qrect.right() - tw - xm - b2;
        else
          x = qrect.left () + ym + b2;
      }
    }
    else if (position == Position::TOP_OUTSIDE) {
      if (direction() == Qt::Vertical) {
        if (! plot()->isInvertY())
          y = qrect.top   () - fm.descent() - ym - b2;
        else
          y = qrect.bottom() + fm.ascent () + ym + b2;
      }
      else {
        if (! plot()->isInvertX())
          x = qrect.right() + xm + b2;
        else
          x = qrect.left () - tw - ym - b2;
      }
    }
    else if (position == Position::BOTTOM_INSIDE) {
      if (direction() == Qt::Vertical) {
        if (! plot()->isInvertY())
          y = qrect.bottom() - fm.descent() - ym - b2;
        else
          y = qrect.top   () + fm.ascent () + ym + b2;
      }
      else {
        if (! plot()->isInvertX())
          x = qrect.left() + xm + b2;
        else
          x = qrect.right() - tw - ym - b2;
      }
    }
    else if (position == Position::BOTTOM_OUTSIDE) {
      if (direction() == Qt::Vertical) {
        if (! plot()->isInvertY())
          y = qrect.bottom() + fm.ascent () + ym + b2;
        else
          y = qrect.top   () - fm.descent() - ym - b2;
      }
      else {
        if (! plot()->isInvertX())
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
        device->setClipRect(device->pixelToWindow(qrect));
      }
    }

    // draw box
    prect = QRectF(x - b1, y - fm.ascent() - b1, tw + 2*b1, fm.height() + 2*b1);

    CQChartsBoxObj::draw(device, device->pixelToWindow(prect));

    if (! clipped) {
      if (ystr.length()) {
        device->setPen(tpen);

        QPointF p1 = device->pixelToWindow(QPointF(x, y));

        if (isTextContrast()) {
#if 0
          if (direction() == Qt::Vertical)
            CQChartsDrawUtil::drawContrastText(device, p1, ystr);
          else
            CQChartsDrawUtil::drawContrastText(device, p1, ystr);
#else
          CQChartsDrawUtil::drawContrastText(device, p1, ystr);
#endif
        }
        else {
#if 0
          if (direction() == Qt::Vertical)
            CQChartsDrawUtil::drawSimpleText(device, p1, ystr);
          else
            CQChartsDrawUtil::drawSimpleText(device, p1, ystr);
#else
          CQChartsDrawUtil::drawSimpleText(device, p1, ystr);
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

    CQChartsRotatedText::bboxData(x, y, ystr, device->font(), textAngle(), b1,
                                  prect, points, align, /*alignBBox*/ true);

    QPolygonF poly;

    for (std::size_t i = 0; i < points.size(); ++i)
      poly << device->pixelToWindow(points[i]);

    CQChartsBoxObj::draw(device, poly);

    device->setPen(tpen);

    if (ystr.length()) {
      QPointF p1(x, y);

      CQChartsRotatedText::draw(device, device->pixelToWindow(p1), ystr, textAngle(), align,
                                /*alignBBox*/ true, isTextContrast());
    }
  }

  //---

  if (plot()->showBoxes()) {
    plot()->drawWindowColorBox(device, CQChartsGeom::BBox(prect));
  }

  //---

  device->restore();
}

//---

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

  QFont font = plot()->view()->plotFont(plot(), textFont());

  if (CMathUtil::isZero(textAngle())) {
    QFontMetricsF fm(font);

    double tw = fm.width(ystr);

    double x = qrect.center().x();
    double y = 0.0;

    if      (position == Position::TOP_INSIDE) {
      if (! plot()->isInvertY())
        y = qrect.top   () + fm.ascent () + ym + b2;
      else
        y = qrect.bottom() - fm.descent() - ym - b2;
    }
    else if (position == Position::TOP_OUTSIDE) {
      if (! plot()->isInvertY())
        y = qrect.top   () - fm.descent() - ym - b2;
      else
        y = qrect.bottom() + fm.ascent () + ym + b2;
    }
    else if (position == Position::BOTTOM_INSIDE) {
      if (! plot()->isInvertY())
        y = qrect.bottom() - fm.descent() - ym - b2;
      else
        y = qrect.top   () + fm.ascent () + ym + b2;
    }
    else if (position == Position::BOTTOM_OUTSIDE) {
      if (! plot()->isInvertY())
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

  CQChartsGeom::BBox wrect = plot()->pixelToWindow(CQChartsGeom::BBox(prect));

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

//---

void
CQChartsDataLabel::
write(std::ostream &os, const QString &varName) const
{
  auto plotName = [&]() {
    return (varName != "" ? varName : "plot");
  };

  CQPropertyViewModel::NameValues nameValues;

  plot()->propertyModel()->getChangedNameValues(this, nameValues, /*tcl*/true);

  if (! nameValues.empty())
    os << "\n";

  for (const auto &nv : nameValues) {
    QString str;

    if (! CQChartsVariant::toString(nv.second, str))
      str = "";

    os << "set_charts_property -plot $" << plotName().toStdString();

    os << " -name " << nv.first.toStdString() << " -value {" << str.toStdString() << "}\n";
  }
}
