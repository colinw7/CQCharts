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

void
CQChartsDataLabel::
draw(CQChartsPaintDevice *device, const QRectF &qrect, const QString &ystr) const
{
  draw(device, qrect, ystr, position());
}

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

void
CQChartsDataLabel::
draw(CQChartsPaintDevice *device, const QRectF &qrect, const QString &ystr,
     const Position &position, const QPen &tpen) const
{
  device->save();

  plot()->view()->setPlotPainterFont(plot(), device, textFont());

  QRectF qrect1 = qrect;

  //---

  Position position1 = adjustPosition(position);

  //---

  if (CMathUtil::isZero(textAngle())) {
    QRectF pqrect = device->windowToPixel(qrect);

    double xm = 2; // pixels
    double ym = 2; // pixels

    double padding = CQChartsBoxObj::padding(); // pixels

    // get external margin
    double pxlm = lengthPixelWidth (CQChartsBoxObj::margin().left  ());
    double pxrm = lengthPixelWidth (CQChartsBoxObj::margin().right ());
    double pytm = lengthPixelHeight(CQChartsBoxObj::margin().top   ());
    double pybm = lengthPixelHeight(CQChartsBoxObj::margin().bottom());

    //---

    QFontMetricsF fm(device->font());

    double tw = fm.width(ystr);

    // calc text pixel position
    double px = 0.0, py = 0.0;

    if      (position1 == Position::TOP_INSIDE) {
      px = pqrect.center().x() - tw/2;
      py = pqrect.top   () + fm.ascent () + ym + padding;
    }
    else if (position1 == Position::TOP_OUTSIDE) {
      px = pqrect.center().x() - tw/2;
      py = pqrect.top   () - fm.descent() - ym - padding;
    }
    else if (position1 == Position::BOTTOM_INSIDE) {
      px = pqrect.center().x() - tw/2;
      py = pqrect.bottom() - fm.descent() - ym - padding;
    }
    else if (position1 == Position::BOTTOM_OUTSIDE) {
      px = pqrect.center().x() - tw/2;
      py = pqrect.bottom() + fm.ascent () + ym + padding;
    }
    else if (position1 == Position::LEFT_INSIDE) {
      px = pqrect.left  () + xm + padding;
      py = pqrect.center().y() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::LEFT_OUTSIDE) {
      px = pqrect.left  () - tw - xm - padding;
      py = pqrect.center().y() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::RIGHT_INSIDE) {
      px = pqrect.right () - tw - xm - padding;
      py = pqrect.center().y() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::RIGHT_OUTSIDE) {
      px = pqrect.right () + xm + padding;
      py = pqrect.center().y() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::CENTER) {
      px = pqrect.center().x() - tw/2;
      py = pqrect.center().y() + (fm.ascent() - fm.descent())/2;
    }

    // clip if needed
    bool clipped = false;

    if (isClip()) {
      if (tw >= pqrect.width())
        clipped = true;
    }

    if (isClip()) {
      if (position1 == Position::TOP_INSIDE  || position1 == Position::BOTTOM_INSIDE ||
          position1 == Position::LEFT_INSIDE || position1 == Position::RIGHT_INSIDE ||
          position1 == Position::CENTER) {
        device->setClipRect(device->pixelToWindow(pqrect));
      }
    }

    // draw box
    QRectF prect = QRectF(px - pxlm, py - fm.ascent() - pybm,
                          tw + pxlm + pxrm, fm.height() + pybm + pytm);

    CQChartsBoxObj::draw(device, device->pixelToWindow(prect));

    if (! clipped) {
      if (ystr.length()) {
        device->setPen(tpen);

        QPointF p1 = device->pixelToWindow(QPointF(px, py));

        CQChartsTextOptions options;

        options.angle         = 0;
        options.align         = Qt::AlignLeft;
        options.contrast      = isTextContrast();
        options.contrastAlpha = textContrastAlpha();

        CQChartsDrawUtil::drawTextAtPoint(device, p1, ystr, options);

#if 0
        if (isTextContrast())
          CQChartsDrawUtil::drawContrastText(device, p1, ystr, textContrastAlpha());
        else
          CQChartsDrawUtil::drawSimpleText(device, p1, ystr);
#endif
      }
    }
  }
  else {
    // get external margin
    double xlm = lengthParentWidth (CQChartsBoxObj::margin().left  ());
    double xrm = lengthParentWidth (CQChartsBoxObj::margin().right ());
    double ytm = lengthParentHeight(CQChartsBoxObj::margin().top   ());
    double ybm = lengthParentHeight(CQChartsBoxObj::margin().bottom());

    // TODO: handle horizontal and angle

    // calc pixel position
    double x = 0.0, y = 0.0;

    Qt::Alignment align = textAlignment();

    if      (position1 == Position::TOP_INSIDE) {
      x = qrect.center().x();
      y = qrect.top   () + ybm + ytm;
    }
    else if (position1 == Position::TOP_OUTSIDE) {
      x = qrect.center().x();
      y = qrect.top   () - ybm - ytm;
    }
    else if (position1 == Position::BOTTOM_INSIDE) {
      x = qrect.center().x();
      y = qrect.bottom() - ybm - ytm;
    }
    else if (position1 == Position::BOTTOM_OUTSIDE) {
      x = qrect.center ().x();
      y = qrect.bottom() + ybm + ytm;
    }
    else if (position1 == Position::LEFT_INSIDE) {
      x = qrect.left  () + xlm + xrm;
      y = qrect.center().y();
    }
    else if (position1 == Position::LEFT_OUTSIDE) {
      x = qrect.left  () - xlm - xrm;
      y = qrect.center().y();
    }
    else if (position1 == Position::RIGHT_INSIDE) {
      x = qrect.right () - xlm - xrm;
      y = qrect.center().y();
    }
    else if (position1 == Position::RIGHT_OUTSIDE) {
      x = qrect.right () + xlm + xrm;
      y = qrect.center().y();
    }
    else if (position1 == Position::CENTER) {
      x = qrect.center().x();
      y = qrect.center().y();
    }

    CQChartsRotatedText::Points points;

    CQChartsGeom::Margin border(xlm, ytm, xrm, ybm);

    CQChartsTextOptions options;

    options.angle = textAngle();
    options.align = align;

    CQChartsRotatedText::calcBBoxData(x, y, ystr, device->font(), options, border,
                                      qrect1, points, /*alignBBox*/ true);

    QPolygonF poly;

    for (std::size_t i = 0; i < points.size(); ++i)
      poly << device->pixelToWindow(points[i]);

    CQChartsBoxObj::draw(device, poly);

    device->setPen(tpen);

    if (ystr.length()) {
      QPointF p1(x, y);

      CQChartsTextOptions options;

      options.angle         = textAngle();
      options.align         = align;
      options.contrast      = isTextContrast();
      options.contrastAlpha = textContrastAlpha();

      CQChartsRotatedText::draw(device, device->pixelToWindow(p1), ystr, options,
                                /*alignBBox*/ true);
    }
  }

  //---

  if (plot()->showBoxes()) {
    plot()->drawWindowColorBox(device, CQChartsGeom::BBox(qrect1));
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

  double xm = 2;
  double ym = 2;

  // get external margin
  double xlm = lengthParentWidth (CQChartsBoxObj::margin().left  ());
  double xrm = lengthParentWidth (CQChartsBoxObj::margin().right ());
  double ytm = lengthParentHeight(CQChartsBoxObj::margin().top   ());
  double ybm = lengthParentHeight(CQChartsBoxObj::margin().bottom());

  double padding = CQChartsBoxObj::padding();

  QRectF qrect1 = qrect;

  //---

  QFont font = plot()->view()->plotFont(plot(), textFont());

  Position position1 = adjustPosition(position);

  //---

  if (CMathUtil::isZero(textAngle())) {
    QRectF pqrect = plot()->windowToPixel(qrect);

    //---

    QFontMetricsF fm(font);

    double tw = fm.width(ystr);

    // calc text pixel position
    double px = 0.0, py = 0.0;

    if      (position1 == Position::TOP_INSIDE) {
      px = pqrect.center().x() - tw/2;
      py = pqrect.top   () + fm.ascent () + ym + padding;
    }
    else if (position1 == Position::TOP_OUTSIDE) {
      px = pqrect.center().x() - tw/2;
      py = pqrect.top   () - fm.descent() - ym - padding;
    } else if (position1 == Position::BOTTOM_INSIDE) {
      px = pqrect.center().x() - tw/2;
      py = pqrect.bottom() - fm.descent() - ym - padding;
    }
    else if (position1 == Position::BOTTOM_OUTSIDE) {
      px = pqrect.center().x() - tw/2;
      py = pqrect.bottom() + fm.ascent () + ym + padding;
    }
    else if (position1 == Position::LEFT_INSIDE) {
      px = pqrect.left  () + xm + padding;
      py = pqrect.center().y() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::LEFT_OUTSIDE) {
      px = pqrect.left  () - tw - xm - padding;
      py = pqrect.center().y() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::RIGHT_INSIDE) {
      px = pqrect.right () - tw - xm - padding;
      py = pqrect.center().y() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::RIGHT_OUTSIDE) {
      px = pqrect.right () + xm + padding;
      py = pqrect.center().y() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::CENTER) {
      px = pqrect.center().x() - tw/2;
      py = pqrect.center().y() + (fm.ascent() - fm.descent())/2;
    }

    QRectF prect = QRectF(px - tw/2 - xlm, py - fm.ascent() - ybm,
                          tw + xlm + xrm, fm.height() + ybm + ytm);

    qrect1 = plot()->pixelToWindow(prect);
  }
  else {
    // calc pixel position
    double x = 0.0, y = 0.0;

    Qt::Alignment align = textAlignment();

    if      (position1 == Position::TOP_INSIDE) {
      x = qrect.center().x();
      y = qrect.top   () + ybm + ytm;
    }
    else if (position1 == Position::TOP_OUTSIDE) {
      x = qrect.center().x();
      y = qrect.top   () - ybm - ytm;
    }
    else if (position1 == Position::BOTTOM_INSIDE) {
      x = qrect.center().x();
      y = qrect.bottom() - ybm - ytm;
    }
    else if (position1 == Position::BOTTOM_OUTSIDE) {
      x = qrect.center ().x();
      y = qrect.bottom() + ybm + ytm;
    }
    else if (position1 == Position::LEFT_INSIDE) {
      x = qrect.left  () + xlm + xrm;
      y = qrect.center().y();
    }
    else if (position1 == Position::LEFT_OUTSIDE) {
      x = qrect.left  () - xlm - xrm;
      y = qrect.center().y();
    }
    else if (position1 == Position::RIGHT_INSIDE) {
      x = qrect.right () - xlm - xrm;
      y = qrect.center().y();
    }
    else if (position1 == Position::RIGHT_OUTSIDE) {
      x = qrect.right () + xlm + xrm;
      y = qrect.center().y();
    }
    else if (position1 == Position::CENTER) {
      x = qrect.center().x();
      y = qrect.center().y();
    }

    CQChartsRotatedText::Points points;

    CQChartsGeom::Margin border(xlm, ytm, xrm, ybm);

    CQChartsTextOptions options;

    options.angle = textAngle();
    options.align = align;

    CQChartsRotatedText::calcBBoxData(x, y, ystr, font, options, border,
                                      qrect1, points, /*alignBBox*/ true);
  }

  //---

  CQChartsGeom::BBox wrect(qrect1);

  return wrect;
}

CQChartsDataLabel::Position
CQChartsDataLabel::
adjustPosition(Position position) const
{
  Position position1 = position;

  if (direction() == Qt::Vertical) {
    if (plot()->isInvertX()) {
      if      (position1 == Position::LEFT_INSIDE  ) position1 = Position::RIGHT_INSIDE;
      else if (position1 == Position::LEFT_OUTSIDE ) position1 = Position::RIGHT_OUTSIDE;
      else if (position1 == Position::RIGHT_INSIDE ) position1 = Position::LEFT_INSIDE;
      else if (position1 == Position::RIGHT_OUTSIDE) position1 = Position::LEFT_OUTSIDE;
    }

    if (plot()->isInvertY()) {
      if      (position1 == Position::BOTTOM_INSIDE ) position1 = Position::TOP_INSIDE;
      if      (position1 == Position::BOTTOM_OUTSIDE) position1 = Position::TOP_OUTSIDE;
      else if (position1 == Position::TOP_INSIDE    ) position1 = Position::BOTTOM_INSIDE;
      else if (position1 == Position::TOP_OUTSIDE   ) position1 = Position::BOTTOM_OUTSIDE;
    }
  }
  else {
    if (! plot()->isInvertY()) {
      if      (position1 == Position::LEFT_INSIDE  ) position1 = Position::TOP_INSIDE;
      else if (position1 == Position::LEFT_OUTSIDE ) position1 = Position::TOP_OUTSIDE;
      else if (position1 == Position::RIGHT_INSIDE ) position1 = Position::BOTTOM_INSIDE;
      else if (position1 == Position::RIGHT_OUTSIDE) position1 = Position::BOTTOM_OUTSIDE;
    }
    else {
      if      (position1 == Position::LEFT_INSIDE  ) position1 = Position::BOTTOM_INSIDE;
      else if (position1 == Position::LEFT_OUTSIDE ) position1 = Position::BOTTOM_OUTSIDE;
      else if (position1 == Position::RIGHT_INSIDE ) position1 = Position::TOP_INSIDE;
      else if (position1 == Position::RIGHT_OUTSIDE) position1 = Position::TOP_OUTSIDE;
    }

    if (! plot()->isInvertX()) {
      if      (position1 == Position::BOTTOM_INSIDE ) position1 = Position::LEFT_INSIDE;
      if      (position1 == Position::BOTTOM_OUTSIDE) position1 = Position::LEFT_OUTSIDE;
      else if (position1 == Position::TOP_INSIDE    ) position1 = Position::RIGHT_INSIDE;
      else if (position1 == Position::TOP_OUTSIDE   ) position1 = Position::RIGHT_OUTSIDE;
    }
    else {
      if      (position1 == Position::BOTTOM_INSIDE ) position1 = Position::RIGHT_INSIDE;
      if      (position1 == Position::BOTTOM_OUTSIDE) position1 = Position::RIGHT_OUTSIDE;
      else if (position1 == Position::TOP_INSIDE    ) position1 = Position::LEFT_INSIDE;
      else if (position1 == Position::TOP_OUTSIDE   ) position1 = Position::LEFT_OUTSIDE;
    }
  }

  return position1;
}

Qt::Alignment
CQChartsDataLabel::
textAlignment() const
{
  Position position1 = adjustPosition(position());

  return textAlignment(position1);
}

Qt::Alignment
CQChartsDataLabel::
textAlignment(const Position &position)
{
  Qt::Alignment align = Qt::AlignHCenter | Qt::AlignVCenter;

  if      (position == Position::TOP_INSIDE    ) align = Qt::AlignTop     | Qt::AlignHCenter;
  else if (position == Position::TOP_OUTSIDE   ) align = Qt::AlignBottom  | Qt::AlignHCenter;
  else if (position == Position::BOTTOM_INSIDE ) align = Qt::AlignBottom  | Qt::AlignHCenter;
  else if (position == Position::BOTTOM_OUTSIDE) align = Qt::AlignTop     | Qt::AlignHCenter;
  else if (position == Position::LEFT_INSIDE   ) align = Qt::AlignLeft    | Qt::AlignVCenter;
  else if (position == Position::LEFT_OUTSIDE  ) align = Qt::AlignRight   | Qt::AlignVCenter;
  else if (position == Position::RIGHT_INSIDE  ) align = Qt::AlignRight   | Qt::AlignVCenter;
  else if (position == Position::RIGHT_OUTSIDE ) align = Qt::AlignLeft    | Qt::AlignVCenter;
  else if (position == Position::CENTER        ) align = Qt::AlignHCenter | Qt::AlignVCenter;

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
