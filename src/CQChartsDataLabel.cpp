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

  //---

  addBasicProperties(path, desc);

  addProp(path, "position"   , "", desc + " position");
  addProp(path, "clip"       , "", desc + " is clipped");
  addProp(path, "moveClipped", "", desc + " move clipped");

  addTextProperties(path, desc);

  QString boxPath = path + "/box";

  addBoxProperties(plot()->propertyModel(), boxPath, desc);
}

void
CQChartsDataLabel::
addBasicProperties(const QString &path, const QString &desc)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(plot()->addProperty(path, this, name, alias)->setDesc(desc));
  };

  addProp(path, "visible", "", desc + " visible");
}

void
CQChartsDataLabel::
addTextProperties(const QString &path, const QString &desc)
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(plot()->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc) {
    auto *item = addProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  QString textPath = path + "/text";

  addStyleProp(textPath, "textColor"        , "color"        , desc + " text color");
  addStyleProp(textPath, "textAlpha"        , "alpha"        , desc + " text alpha");
  addStyleProp(textPath, "textFont"         , "font"         , desc + " text font");
  addStyleProp(textPath, "textAngle"        , "angle"        , desc + " text angle");
  addStyleProp(textPath, "textContrast"     , "contrast"     , desc + " text is contrast");
  addStyleProp(textPath, "textContrastAlpha", "contrastAlpha", desc + " text contrast alpha");
  addStyleProp(textPath, "textHtml"         , "html"         , desc + " text is HTML");
}

void
CQChartsDataLabel::
draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox, const QString &str) const
{
  draw(device, bbox, str, position());
}

void
CQChartsDataLabel::
draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox, const QString &str,
     const Position &position) const
{
  if (! isVisible())
    return;

  CQChartsPenBrush penBrush;

  QColor tc = interpTextColor(ColorInd());

  plot()->setPenBrush(penBrush, CQChartsPenData(true, tc, textAlpha()), CQChartsBrushData(false));

  draw(device, bbox, str, position, penBrush);
}

void
CQChartsDataLabel::
draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &bbox, const QString &ystr,
     const Position &position, const CQChartsPenBrush &penBrush) const
{
  bbox_ = bbox;

  //---

  device->save();

  plot()->view()->setPlotPainterFont(plot(), device, textFont());

  //---

  Position position1 = adjustPosition(position);

  //---

  if (CMathUtil::isZero(textAngle().value())) {
    auto pbbox = device->windowToPixel(bbox);

    double xm = 2; // pixels
    double ym = 2; // pixels

    // get inner padding
    double pxlp = lengthPixelWidth (CQChartsBoxObj::padding().left  ());
    double pxrp = lengthPixelWidth (CQChartsBoxObj::padding().right ());
    double pytp = lengthPixelHeight(CQChartsBoxObj::padding().top   ());
    double pybp = lengthPixelHeight(CQChartsBoxObj::padding().bottom());

    // get outer margin
    double pxlm = lengthPixelWidth (CQChartsBoxObj::margin().left  ());
    double pxrm = lengthPixelWidth (CQChartsBoxObj::margin().right ());
    double pytm = lengthPixelHeight(CQChartsBoxObj::margin().top   ());
    double pybm = lengthPixelHeight(CQChartsBoxObj::margin().bottom());

    //---

    QFontMetricsF fm(device->font());

    double tw = fm.width(ystr);
    double th = fm.descent() + fm.ascent();

    // calc text pixel position
    double px = 0.0, py = 0.0;

    if      (position1 == Position::TOP_INSIDE) {
      px = pbbox.getXMid() - tw/2;
      py = pbbox.getYMin() + fm.ascent () + ym + pytp;
    }
    else if (position1 == Position::TOP_OUTSIDE) {
      px = pbbox.getXMid() - tw/2;
      py = pbbox.getYMin() - fm.descent() - ym - pytp;
    }
    else if (position1 == Position::BOTTOM_INSIDE) {
      px = pbbox.getXMid() - tw/2;
      py = pbbox.getYMax() - fm.descent() - ym - pybp;
    }
    else if (position1 == Position::BOTTOM_OUTSIDE) {
      px = pbbox.getXMid() - tw/2;
      py = pbbox.getYMax() + fm.ascent () + ym + pybp;
    }
    else if (position1 == Position::LEFT_INSIDE) {
      px = pbbox.getXMin() + xm + pxlp;
      py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::LEFT_OUTSIDE) {
      px = pbbox.getXMin() - tw - xm - pxlp;
      py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::RIGHT_INSIDE) {
      px = pbbox.getXMax() - tw - xm - pxrp;
      py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::RIGHT_OUTSIDE) {
      px = pbbox.getXMax() + xm + pxrp;
      py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::CENTER) {
      px = pbbox.getXMid() - tw/2;
      py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;
    }

    // clip if needed
    bool clipped = false;

    if (tw >= pbbox.getWidth()) {
      clipped = true;
    }

    if (th >= pbbox.getHeight()) {
      clipped = true;
    }

    if (moveClipped()) {
      if      (position1 == Position::LEFT_INSIDE) {
        position1 = Position::LEFT_OUTSIDE;

        px = pbbox.getXMin() - tw - xm - pxlp;
        py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;

        clipped = false;
      }
      else if (position1 == Position::RIGHT_INSIDE) {
        position1 = Position::RIGHT_OUTSIDE;

        px = pbbox.getXMax() + xm + pxrp;
        py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;

        clipped = false;
      }
      else if      (position1 == Position::TOP_INSIDE) {
        position1 = Position::TOP_OUTSIDE;

        px = pbbox.getXMid() - tw/2;
        py = pbbox.getYMin() - fm.descent() - ym - pytp;

        clipped = false;
      }
      else if (position1 == Position::BOTTOM_INSIDE) {
        position1 = Position::BOTTOM_OUTSIDE;

        px = pbbox.getXMid() - tw/2;
        py = pbbox.getYMax() + fm.ascent () + ym + pybp;

        clipped = false;
      }
    }

    if (isClip()) {
      if (position1 == Position::TOP_INSIDE  || position1 == Position::BOTTOM_INSIDE ||
          position1 == Position::LEFT_INSIDE || position1 == Position::RIGHT_INSIDE ||
          position1 == Position::CENTER) {
        device->setClipRect(bbox);
      }
    }

    // draw box
    CQChartsGeom::BBox tpbbox(px      - pxlm, py - fm.ascent () - pybm,
                              px + tw + pxrm, py + fm.descent() + pytm);

    CQChartsBoxObj::draw(device, device->pixelToWindow(tpbbox));

    // draw text
    if (! clipped) {
      if (ystr.length()) {
        device->setPen(penBrush.pen);

        auto p1 = device->pixelToWindow(CQChartsGeom::Point(px, py));

        CQChartsTextOptions options;

        options.angle         = CQChartsAngle(0);
        options.align         = Qt::AlignLeft;
        options.contrast      = isTextContrast();
        options.contrastAlpha = textContrastAlpha();

        CQChartsDrawUtil::drawTextAtPoint(device, p1, ystr, options);
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
    double px = 0.0, py = 0.0;

    Qt::Alignment align = textAlignment();

    if      (position1 == Position::TOP_INSIDE) {
      px = bbox.getXMid();
      py = bbox.getYMax() + ybm + ytm;
    }
    else if (position1 == Position::TOP_OUTSIDE) {
      px = bbox.getXMid();
      py = bbox.getYMax() - ybm - ytm;
    }
    else if (position1 == Position::BOTTOM_INSIDE) {
      px = bbox.getXMid();
      py = bbox.getYMin() - ybm - ytm;
    }
    else if (position1 == Position::BOTTOM_OUTSIDE) {
      px = bbox.getXMid();
      py = bbox.getYMin() + ybm + ytm;
    }
    else if (position1 == Position::LEFT_INSIDE) {
      px = bbox.getXMin() + xlm + xrm;
      py = bbox.getYMid();
    }
    else if (position1 == Position::LEFT_OUTSIDE) {
      px = bbox.getXMin() - xlm - xrm;
      py = bbox.getYMid();
    }
    else if (position1 == Position::RIGHT_INSIDE) {
      px = bbox.getXMax() - xlm - xrm;
      py = bbox.getYMid();
    }
    else if (position1 == Position::RIGHT_OUTSIDE) {
      px = bbox.getXMax() + xlm + xrm;
      py = bbox.getYMid();
    }
    else if (position1 == Position::CENTER) {
      px = bbox.getXMid();
      py = bbox.getYMid();
    }

    CQChartsGeom::BBox          pbbox;
    CQChartsRotatedText::Points ppoints;

    CQChartsGeom::Margin border(xlm, ytm, xrm, ybm);

    CQChartsTextOptions options;

    options.angle = textAngle();
    options.align = align;

    auto pt = device->windowToPixel(CQChartsGeom::Point(px, py));

    CQChartsRotatedText::calcBBoxData(pt.x, pt.y, ystr, device->font(), options, border,
                                      pbbox, ppoints, /*alignBBox*/ true);

    // draw polygon
    CQChartsGeom::Polygon poly;

    for (std::size_t i = 0; i < ppoints.size(); ++i)
      poly.addPoint(device->pixelToWindow(ppoints[i]));

    CQChartsBoxObj::draw(device, poly);

    device->setPen(penBrush.pen);

    // draw text
    if (ystr.length()) {
      auto p1 = device->pixelToWindow(CQChartsGeom::Point(px, py));

      CQChartsTextOptions options;

      options.angle         = textAngle();
      options.align         = align;
      options.contrast      = isTextContrast();
      options.contrastAlpha = textContrastAlpha();

//    CQChartsRotatedText::draw(device, device->pixelToWindow(p1), ystr, options,
//                              /*alignBBox*/ true);

      CQChartsDrawUtil::drawTextAtPoint(device, p1, ystr, options, /*centered*/true);
    }
  }

  //---

  if (plot()->showBoxes()) {
    plot()->drawWindowColorBox(device, bbox);
  }

  //---

  device->restore();
}

//---

CQChartsGeom::BBox
CQChartsDataLabel::
calcRect(const CQChartsGeom::BBox &bbox, const QString &ystr) const
{
  return calcRect(bbox, ystr, position());
}

CQChartsGeom::BBox
CQChartsDataLabel::
calcRect(const CQChartsGeom::BBox &bbox, const QString &ystr, const Position &position) const
{
  if (! isVisible())
    return CQChartsGeom::BBox();

  //---

  auto pbbox = windowToPixel(bbox);

  double xm = 2;
  double ym = 2;

  // get inner padding
  double xlp = lengthPixelWidth (CQChartsBoxObj::padding().left  ());
  double xrp = lengthPixelWidth (CQChartsBoxObj::padding().right ());
  double ytp = lengthPixelHeight(CQChartsBoxObj::padding().top   ());
  double ybp = lengthPixelHeight(CQChartsBoxObj::padding().bottom());

  // get outer margin
  double xlm = lengthPixelWidth (CQChartsBoxObj::margin().left  ());
  double xrm = lengthPixelWidth (CQChartsBoxObj::margin().right ());
  double ytm = lengthPixelHeight(CQChartsBoxObj::margin().top   ());
  double ybm = lengthPixelHeight(CQChartsBoxObj::margin().bottom());

  //---

  QFont font = plot()->view()->plotFont(plot(), textFont());

  Position position1 = adjustPosition(position);

  //---

  CQChartsGeom::BBox wbbox;

  if (CMathUtil::isZero(textAngle().value())) {
    QFontMetricsF fm(font);

    double tw = fm.width(ystr);

    // calc text pixel position
    double px = 0.0, py = 0.0;

    if      (position1 == Position::TOP_INSIDE) {
      px = pbbox.getXMid() - tw/2;
      py = pbbox.getYMin() + fm.ascent () + ym + ytp;
    }
    else if (position1 == Position::TOP_OUTSIDE) {
      px = pbbox.getXMid() - tw/2;
      py = pbbox.getYMin() - fm.descent() - ym - ytp;
    }
    else if (position1 == Position::BOTTOM_INSIDE) {
      px = pbbox.getXMid() - tw/2;
      py = pbbox.getYMax() - fm.descent() - ym - ybp;
    }
    else if (position1 == Position::BOTTOM_OUTSIDE) {
      px = pbbox.getXMid() - tw/2;
      py = pbbox.getYMax() + fm.ascent () + ym + ybp;
    }
    else if (position1 == Position::LEFT_INSIDE) {
      px = pbbox.getXMin() + xm + xlp;
      py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::LEFT_OUTSIDE) {
      px = pbbox.getXMin() - tw - xm - xlp;
      py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::RIGHT_INSIDE) {
      px = pbbox.getXMax() - tw - xm - xrp;
      py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::RIGHT_OUTSIDE) {
      px = pbbox.getXMax() + xm + xrp;
      py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;
    }
    else if (position1 == Position::CENTER) {
      px = pbbox.getXMid() - tw/2;
      py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;
    }

    CQChartsGeom::BBox pbbox1(px -      xlm, py - fm.ascent () - ybm,
                              px + tw + xrm, py + fm.descent() + ytm);

    wbbox = plot()->pixelToWindow(pbbox1);
  }
  else {
    // calc pixel position
    double px = 0.0, py = 0.0;

    Qt::Alignment align = textAlignment();

    if      (position1 == Position::TOP_INSIDE) {
      px = pbbox.getXMid();
      py = pbbox.getYMax() + ybm + ytm;
    }
    else if (position1 == Position::TOP_OUTSIDE) {
      px = pbbox.getXMid();
      py = pbbox.getYMax() - ybm - ytm;
    }
    else if (position1 == Position::BOTTOM_INSIDE) {
      px = pbbox.getXMid();
      py = pbbox.getYMin() - ybm - ytm;
    }
    else if (position1 == Position::BOTTOM_OUTSIDE) {
      px = pbbox.getXMid();
      py = pbbox.getYMin() + ybm + ytm;
    }
    else if (position1 == Position::LEFT_INSIDE) {
      px = pbbox.getXMin() + xlm + xrm;
      py = pbbox.getYMid();
    }
    else if (position1 == Position::LEFT_OUTSIDE) {
      px = pbbox.getXMin() - xlm - xrm;
      py = pbbox.getYMid();
    }
    else if (position1 == Position::RIGHT_INSIDE) {
      px = pbbox.getXMax() - xlm - xrm;
      py = pbbox.getYMid();
    }
    else if (position1 == Position::RIGHT_OUTSIDE) {
      px = pbbox.getXMax() + xlm + xrm;
      py = pbbox.getYMid();
    }
    else if (position1 == Position::CENTER) {
      px = pbbox.getXMid();
      py = pbbox.getYMid();
    }

    CQChartsGeom::BBox          pbbox1;
    CQChartsRotatedText::Points ppoints;

    CQChartsGeom::Margin border(xlm, ytm, xrm, ybm);

    CQChartsTextOptions options;

    options.angle = textAngle();
    options.align = align;

    CQChartsRotatedText::calcBBoxData(px, py, ystr, font, options, border,
                                      pbbox1, ppoints, /*alignBBox*/ true);

    wbbox = plot()->pixelToWindow(pbbox1);
  }

  //---

  return wbbox;
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

bool
CQChartsDataLabel::
contains(const CQChartsGeom::Point &p) const
{
  return bbox_.inside(p);
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
