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
CQChartsDataLabel(Plot *plot) :
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
  addProp(path, "drawClipped", "", desc + " draw clipped");
  addProp(path, "moveClipped", "", desc + " move clipped");

  addTextProperties(path, desc);

  auto boxPath = path + "/box";

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

  auto textPath = path + "/text";

  addStyleProp(textPath, "textColor"        , "color"        , desc + " text color");
  addStyleProp(textPath, "textAlpha"        , "alpha"        , desc + " text alpha");
  addStyleProp(textPath, "textFont"         , "font"         , desc + " text font");
  addStyleProp(textPath, "textAngle"        , "angle"        , desc + " text angle");
  addStyleProp(textPath, "textContrast"     , "contrast"     , desc + " text is contrast");
  addStyleProp(textPath, "textContrastAlpha", "contrastAlpha", desc + " text contrast alpha");
  addStyleProp(textPath, "textHtml"         , "html"         , desc + " text is HTML");
  addStyleProp(textPath, "textClipLength"   , "clipLength"   , desc + " text clip length");
  addStyleProp(textPath, "textClipElide"    , "clipElide"    , desc + " text clip elide");
}

void
CQChartsDataLabel::
draw(PaintDevice *device, const BBox &bbox, const QString &str) const
{
  draw(device, bbox, str, position());
}

void
CQChartsDataLabel::
draw(PaintDevice *device, const BBox &bbox, const QString &str, const Position &position) const
{
  if (! isVisible())
    return;

  PenBrush penBrush;

  auto tc = interpTextColor(ColorInd());

  plot()->setPenBrush(penBrush, PenData(true, tc, textAlpha()), BrushData(false));

  draw(device, bbox, str, position, penBrush);
}

void
CQChartsDataLabel::
draw(PaintDevice *device, const BBox &bbox, const QString &ystr,
     const Position &position, const PenBrush &penBrush) const
{
  bbox_ = bbox;

  //---

  device->save();

  plot()->view()->setPlotPainterFont(plot(), device, textFont());

  //---

  auto position1 = adjustPosition(position);

  //---

  if (textAngle().isZero()) {
    auto pbbox = device->windowToPixel(bbox);

    double xm = 2; // pixels
    double ym = 2; // pixels

    // get inner padding
    double pxlp = lengthPixelWidth (BoxObj::padding().left  ());
    double pxrp = lengthPixelWidth (BoxObj::padding().right ());
    double pytp = lengthPixelHeight(BoxObj::padding().top   ());
    double pybp = lengthPixelHeight(BoxObj::padding().bottom());

    // get outer margin
    double pxlm = lengthPixelWidth (BoxObj::margin().left  ());
    double pxrm = lengthPixelWidth (BoxObj::margin().right ());
    double pytm = lengthPixelHeight(BoxObj::margin().top   ());
    double pybm = lengthPixelHeight(BoxObj::margin().bottom());

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
    bool hclipped = false;
    bool vclipped = false;

    if (position1 == Position::TOP_INSIDE  || position1 == Position::BOTTOM_INSIDE ||
        position1 == Position::LEFT_INSIDE || position1 == Position::RIGHT_INSIDE ||
        position1 == Position::CENTER) {
      if (tw >= pbbox.getWidth())
        hclipped = true;

      if (th >= pbbox.getHeight())
        vclipped = true;
    }

    if (hclipped && moveClipped()) {
      if      (position1 == Position::LEFT_INSIDE) {
        position1 = Position::LEFT_OUTSIDE;

        px = pbbox.getXMin() - tw - xm - pxlp;
        py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;

        hclipped = false;
      }
      else if (position1 == Position::RIGHT_INSIDE) {
        position1 = Position::RIGHT_OUTSIDE;

        px = pbbox.getXMax() + xm + pxrp;
        py = pbbox.getYMid() + (fm.ascent() - fm.descent())/2;

        hclipped = false;
      }
    }

    if (vclipped && moveClipped()) {
      if      (position1 == Position::TOP_INSIDE) {
        position1 = Position::TOP_OUTSIDE;

        px = pbbox.getXMid() - tw/2;
        py = pbbox.getYMin() - fm.descent() - ym - pytp;

        vclipped = false;
      }
      else if (position1 == Position::BOTTOM_INSIDE) {
        position1 = Position::BOTTOM_OUTSIDE;

        px = pbbox.getXMid() - tw/2;
        py = pbbox.getYMax() + fm.ascent () + ym + pybp;

        vclipped = false;
      }
    }

    // draw box
    BBox tpbbox(px      - pxlm, py - fm.ascent () - pybm,
                px + tw + pxrm, py + fm.descent() + pytm);

    BoxObj::draw(device, plot()->pixelToWindow(tpbbox));

    // draw text
    if (ystr.length()) {
      bool clipped = false;

      // clip to rectangle
      if (isClip()) {
        if (position1 == Position::TOP_INSIDE  || position1 == Position::BOTTOM_INSIDE ||
            position1 == Position::LEFT_INSIDE || position1 == Position::RIGHT_INSIDE ||
            position1 == Position::CENTER) {
          device->setClipRect(bbox);

          clipped = true;
        }
      }

      // if no clip rect set and clipped in h/v direction then text is clipped
      bool textClipped = (! clipped && (hclipped || vclipped));

      if (! textClipped || drawClipped()) {
        device->setPen(penBrush.pen);

        auto p1 = plot()->pixelToWindow(Point(px, py));

        TextOptions options;

        options.angle         = Angle();
        options.align         = Qt::AlignLeft;
        options.contrast      = isTextContrast();
        options.contrastAlpha = textContrastAlpha();
        options.clipLength    = lengthPixelWidth(textClipLength());
        options.clipElide     = textClipElide();

        CQChartsDrawUtil::drawTextAtPoint(device, p1, ystr, options);
      }
    }
  }
  else {
    // get external margin
    double xlm = lengthParentWidth (BoxObj::margin().left  ());
    double xrm = lengthParentWidth (BoxObj::margin().right ());
    double ytm = lengthParentHeight(BoxObj::margin().top   ());
    double ybm = lengthParentHeight(BoxObj::margin().bottom());

    // TODO: handle horizontal and angle

    // calc pixel position
    double px = 0.0, py = 0.0;

    auto align = textAlignment();

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

    using RotatedTextPoints = CQChartsRotatedText::Points;

    BBox              pbbox;
    RotatedTextPoints ppoints;

    Margin border(xlm, ytm, xrm, ybm);

    TextOptions options;

    options.angle = textAngle();
    options.align = align;

    auto pt = device->windowToPixel(Point(px, py));

    CQChartsRotatedText::calcBBoxData(pt.x, pt.y, ystr, device->font(), options, border,
                                      pbbox, ppoints, /*alignBBox*/ true);

    // draw polygon
    Polygon poly;

    for (std::size_t i = 0; i < ppoints.size(); ++i)
      poly.addPoint(plot()->pixelToWindow(ppoints[i]));

    BoxObj::draw(device, poly);

    device->setPen(penBrush.pen);

    // draw text
    if (ystr.length()) {
      auto p1 = plot()->pixelToWindow(Point(px, py));

      TextOptions options;

      options.angle         = textAngle();
      options.align         = align;
      options.contrast      = isTextContrast();
      options.contrastAlpha = textContrastAlpha();
      options.clipLength    = lengthPixelWidth(textClipLength());
      options.clipElide     = textClipElide();

//    CQChartsRotatedText::draw(device, plot()->pixelToWindow(p1), ystr, options,
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

bool
CQChartsDataLabel::
isAdjustedPositionOutside(const BBox &pbbox, const QString &ystr) const
{
  auto position1 = adjustPosition(position());

  //---

  if (textAngle().isZero()) {
    auto font = plot()->view()->plotFont(plot(), textFont());

    QFontMetricsF fm(font);

    double tw = fm.width(ystr);
    double th = fm.descent() + fm.ascent();

    // clip if needed
    bool hclipped = false;
    bool vclipped = false;

    if (position1 == Position::TOP_INSIDE  || position1 == Position::BOTTOM_INSIDE ||
        position1 == Position::LEFT_INSIDE || position1 == Position::RIGHT_INSIDE ||
        position1 == Position::CENTER) {
      if (tw >= pbbox.getWidth())
        hclipped = true;

      if (th >= pbbox.getHeight())
        vclipped = true;
    }

    if (hclipped && moveClipped()) {
      if      (position1 == Position::LEFT_INSIDE)
        position1 = Position::LEFT_OUTSIDE;
      else if (position1 == Position::RIGHT_INSIDE)
        position1 = Position::RIGHT_OUTSIDE;
    }

    if (vclipped && moveClipped()) {
      if      (position1 == Position::TOP_INSIDE)
        position1 = Position::TOP_OUTSIDE;
      else if (position1 == Position::BOTTOM_INSIDE)
        position1 = Position::BOTTOM_OUTSIDE;
    }
  }

  //---

  return isPositionOutside(position1);
}

//---

CQChartsGeom::BBox
CQChartsDataLabel::
calcRect(const BBox &bbox, const QString &ystr) const
{
  return calcRect(bbox, ystr, position());
}

CQChartsGeom::BBox
CQChartsDataLabel::
calcRect(const BBox &bbox, const QString &ystr, const Position &position) const
{
  if (! isVisible())
    return BBox();

  //---

  auto pbbox = windowToPixel(bbox);

  double xm = 2;
  double ym = 2;

  // get inner padding
  double xlp = lengthPixelWidth (BoxObj::padding().left  ());
  double xrp = lengthPixelWidth (BoxObj::padding().right ());
  double ytp = lengthPixelHeight(BoxObj::padding().top   ());
  double ybp = lengthPixelHeight(BoxObj::padding().bottom());

  // get outer margin
  double xlm = lengthPixelWidth (BoxObj::margin().left  ());
  double xrm = lengthPixelWidth (BoxObj::margin().right ());
  double ytm = lengthPixelHeight(BoxObj::margin().top   ());
  double ybm = lengthPixelHeight(BoxObj::margin().bottom());

  //---

  auto font = plot()->view()->plotFont(plot(), textFont());

  auto position1 = adjustPosition(position);

  //---

  BBox wbbox;

  if (textAngle().isZero()) {
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

    BBox pbbox1(px - xlm, py - fm.ascent () - ybm, px + tw + xrm, py + fm.descent() + ytm);

    wbbox = plot()->pixelToWindow(pbbox1);
  }
  else {
    // calc pixel position
    double px = 0.0, py = 0.0;

    auto align = textAlignment();

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

    using RotatedTextPoints = CQChartsRotatedText::Points;

    BBox              pbbox1;
    RotatedTextPoints ppoints;

    Margin border(xlm, ytm, xrm, ybm);

    TextOptions options;

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
  auto position1 = position;

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
  auto position1 = adjustPosition(position());

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
contains(const Point &p) const
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
