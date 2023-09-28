#include <CQChartsDataLabel.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQChartsRotatedText.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsVariant.h>
#include <CQChartsTextPlacer.h>

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

  draw(device, bbox, str, position, penBrush, textFont());
}

void
CQChartsDataLabel::
draw(PaintDevice *device, const BBox &bbox, const QString &ystr,
     const Position &position, const PenBrush &penBrush, const Font &font) const
{
  const_cast<CQChartsDataLabel *>(this)->setBBox(bbox);

  //---

  device->save();

  plot()->setPainterFont(device, font);

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

    double tw = fm.horizontalAdvance(ystr);
    double th = fm.descent() + fm.ascent();
    double tm = (fm.ascent() - fm.descent())/2.0;

    // calc text pixel position
    Point ppos;

    if      (position1 == Position::TOP_INSIDE)
      ppos = Point(pbbox.getXMid() - tw/2, pbbox.getYMin() + fm.ascent () + ym + pytp);
    else if (position1 == Position::TOP_OUTSIDE)
      ppos = Point(pbbox.getXMid() - tw/2, pbbox.getYMin() - fm.descent() - ym - pytp);
    else if (position1 == Position::BOTTOM_INSIDE)
      ppos = Point(pbbox.getXMid() - tw/2, pbbox.getYMax() - fm.descent() - ym - pybp);
    else if (position1 == Position::BOTTOM_OUTSIDE)
      ppos = Point(pbbox.getXMid() - tw/2, pbbox.getYMax() + fm.ascent () + ym + pybp);
    else if (position1 == Position::LEFT_INSIDE)
      ppos = Point(pbbox.getXMin() + xm + pxlp, pbbox.getYMid() + tm);
    else if (position1 == Position::LEFT_OUTSIDE)
      ppos = Point(pbbox.getXMin() - tw - xm - pxlp, pbbox.getYMid() + tm);
    else if (position1 == Position::RIGHT_INSIDE)
      ppos = Point(pbbox.getXMax() - tw - xm - pxrp, pbbox.getYMid() + tm);
    else if (position1 == Position::RIGHT_OUTSIDE)
      ppos = Point(pbbox.getXMax() + xm + pxrp, pbbox.getYMid() + tm);
    else if (position1 == Position::CENTER)
      ppos = Point(pbbox.getXMid() - tw/2, pbbox.getYMid() + tm);

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

        ppos.x = pbbox.getXMin() - tw - xm - pxlp;
        ppos.y = pbbox.getYMid() + tm;

        hclipped = false;
      }
      else if (position1 == Position::RIGHT_INSIDE) {
        position1 = Position::RIGHT_OUTSIDE;

        ppos.x = pbbox.getXMax() + xm + pxrp;
        ppos.y = pbbox.getYMid() + tm;

        hclipped = false;
      }
    }

    if (vclipped && moveClipped()) {
      if      (position1 == Position::TOP_INSIDE) {
        position1 = Position::TOP_OUTSIDE;

        ppos.x = pbbox.getXMid() - tw/2;
        ppos.y = pbbox.getYMin() - fm.descent() - ym - pytp;

        vclipped = false;
      }
      else if (position1 == Position::BOTTOM_INSIDE) {
        position1 = Position::BOTTOM_OUTSIDE;

        ppos.x = pbbox.getXMid() - tw/2;
        ppos.y = pbbox.getYMax() + fm.ascent () + ym + pybp;

        vclipped = false;
      }
    }

    // draw box
    BBox tpbbox(ppos.x      - pxlm, ppos.y - fm.ascent () - pybm,
                ppos.x + tw + pxrm, ppos.y + fm.descent() + pytm);

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

        auto pos = plot()->pixelToWindow(ppos);

        auto textOptions = this->textOptions();

        textOptions.angle = Angle();
        textOptions.align = Qt::AlignLeft;

        if (textPlacer_)
          textPlacer_->addDrawText(device, ystr, pos, textOptions, bbox.getCenter());
        else
          CQChartsDrawUtil::drawTextAtPoint(device, pos, ystr, textOptions);
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

    // calc position
    Point pos;

    auto align = textAlignment();

    if      (position1 == Position::TOP_INSIDE)
      pos = Point(bbox.getXMid(), bbox.getYMax() + ybm + ytm);
    else if (position1 == Position::TOP_OUTSIDE)
      pos = Point(bbox.getXMid(), bbox.getYMax() - ybm - ytm);
    else if (position1 == Position::BOTTOM_INSIDE)
      pos = Point(bbox.getXMid(), bbox.getYMin() - ybm - ytm);
    else if (position1 == Position::BOTTOM_OUTSIDE)
      pos = Point(bbox.getXMid(), bbox.getYMin() + ybm + ytm);
    else if (position1 == Position::LEFT_INSIDE)
      pos = Point(bbox.getXMin() + xlm + xrm, bbox.getYMid());
    else if (position1 == Position::LEFT_OUTSIDE)
      pos = Point(bbox.getXMin() - xlm - xrm, bbox.getYMid());
    else if (position1 == Position::RIGHT_INSIDE)
      pos = Point(bbox.getXMax() - xlm - xrm, bbox.getYMid());
    else if (position1 == Position::RIGHT_OUTSIDE)
      pos = Point(bbox.getXMax() + xlm + xrm, bbox.getYMid());
    else if (position1 == Position::CENTER)
      pos = Point(bbox.getXMid(), bbox.getYMid());

    using RotatedTextPoints = CQChartsRotatedText::Points;

    BBox              pbbox;
    RotatedTextPoints ppoints;

    Margin border(xlm, ytm, xrm, ybm);

    TextOptions textOptions;

    textOptions.angle = textAngle();
    textOptions.align = align;

    auto ppos = device->windowToPixel(pos);

    CQChartsRotatedText::calcBBoxData(ppos.x, ppos.y, ystr, device->font(), textOptions, border,
                                      pbbox, ppoints, /*alignBBox*/ true);

    // draw polygon
    Polygon poly;

    for (std::size_t i = 0; i < ppoints.size(); ++i)
      poly.addPoint(plot()->pixelToWindow(ppoints[i]));

    BoxObj::draw(device, poly);

    device->setPen(penBrush.pen);

    // draw text
    if (ystr.length()) {
      auto textOptions = this->textOptions();

      textOptions.align = align;

//    CQChartsRotatedText::draw(device, pos, ystr, textOptions, /*alignBBox*/ true);

      if (textPlacer_)
        textPlacer_->addDrawText(device, ystr, pos, textOptions, bbox.getCenter(),
                                 /*margin*/0, /*centered*/true);
      else
        CQChartsDrawUtil::drawTextAtPoint(device, pos, ystr, textOptions, /*centered*/true);
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
    auto pfont = plot()->view()->plotFont(plot(), textFont());

    QFontMetricsF fm(pfont);

    double tw = fm.horizontalAdvance(ystr);
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

  return calcRect(bbox, ystr, position, textFont());
}

CQChartsGeom::BBox
CQChartsDataLabel::
calcRect(const BBox &bbox, const QString &ystr, const Position &position,
         const Font &font) const
{
  auto pfont = plot()->view()->plotFont(plot(), font);

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

  auto position1 = adjustPosition(position);

  //---

  BBox wbbox;

  if (textAngle().isZero()) {
    QFontMetricsF fm(pfont);

    double tw = fm.horizontalAdvance(ystr);
    double tm = (fm.ascent() - fm.descent())/2.0;

    // calc text position
    Point pos;

    if      (position1 == Position::TOP_INSIDE)
      pos = Point(pbbox.getXMid() - tw/2, pbbox.getYMin() + fm.ascent () + ym + ytp);
    else if (position1 == Position::TOP_OUTSIDE)
      pos = Point(pbbox.getXMid() - tw/2, pbbox.getYMin() - fm.descent() - ym - ytp);
    else if (position1 == Position::BOTTOM_INSIDE)
      pos = Point(pbbox.getXMid() - tw/2, pbbox.getYMax() - fm.descent() - ym - ybp);
    else if (position1 == Position::BOTTOM_OUTSIDE)
      pos = Point(pbbox.getXMid() - tw/2, pbbox.getYMax() + fm.ascent () + ym + ybp);
    else if (position1 == Position::LEFT_INSIDE)
      pos = Point(pbbox.getXMin() + xm + xlp, pbbox.getYMid() + tm);
    else if (position1 == Position::LEFT_OUTSIDE)
      pos = Point(pbbox.getXMin() - tw - xm - xlp, pbbox.getYMid() + tm);
    else if (position1 == Position::RIGHT_INSIDE)
      pos = Point(pbbox.getXMax() - tw - xm - xrp, pbbox.getYMid() + tm);
    else if (position1 == Position::RIGHT_OUTSIDE)
      pos = Point(pbbox.getXMax() + xm + xrp, pbbox.getYMid() + tm);
    else if (position1 == Position::CENTER)
      pos = Point(pbbox.getXMid() - tw/2, pbbox.getYMid() + tm);

    BBox pbbox1(pos.x      - xlm, pos.y - fm.ascent () - ybm,
                pos.x + tw + xrm, pos.y + fm.descent() + ytm);

    wbbox = plot()->pixelToWindow(pbbox1);
  }
  else {
    // calc position
    Point pos;

    auto align = textAlignment();

    if      (position1 == Position::TOP_INSIDE)
      pos = Point(pbbox.getXMid(), pbbox.getYMax() + ybm + ytm);
    else if (position1 == Position::TOP_OUTSIDE)
      pos = Point(pbbox.getXMid(), pbbox.getYMax() - ybm - ytm);
    else if (position1 == Position::BOTTOM_INSIDE)
      pos = Point(pbbox.getXMid(), pbbox.getYMin() - ybm - ytm);
    else if (position1 == Position::BOTTOM_OUTSIDE)
      pos = Point(pbbox.getXMid(), pbbox.getYMin() + ybm + ytm);
    else if (position1 == Position::LEFT_INSIDE)
      pos = Point(pbbox.getXMin() + xlm + xrm, pbbox.getYMid());
    else if (position1 == Position::LEFT_OUTSIDE)
      pos = Point(pbbox.getXMin() - xlm - xrm, pbbox.getYMid());
    else if (position1 == Position::RIGHT_INSIDE)
      pos = Point(pbbox.getXMax() - xlm - xrm, pbbox.getYMid());
    else if (position1 == Position::RIGHT_OUTSIDE)
      pos = Point(pbbox.getXMax() + xlm + xrm, pbbox.getYMid());
    else if (position1 == Position::CENTER)
      pos = Point(pbbox.getXMid(), pbbox.getYMid());

    using RotatedTextPoints = CQChartsRotatedText::Points;

    BBox              pbbox1;
    RotatedTextPoints ppoints;

    Margin border(xlm, ytm, xrm, ybm);

    TextOptions textOptions;

    textOptions.angle = textAngle();
    textOptions.align = align;

    CQChartsRotatedText::calcBBoxData(pos.x, pos.y, ystr, pfont, textOptions, border,
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
  auto align = Qt::Alignment(Qt::AlignHCenter | Qt::AlignVCenter);

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

//---

void
CQChartsDataLabel::
connectDataChanged(const QObject *obj, const char *slotName) const
{
  connect(this, SIGNAL(dataChanged()), obj, slotName);
}

void
CQChartsDataLabel::
textBoxObjInvalidate()
{
  if (isSendSignal())
    Q_EMIT dataChanged();
  else
    CQChartsTextBoxObj::textBoxObjInvalidate();
}

//---

bool
CQChartsDataLabel::
contains(const Point &p) const
{
  return bbox().inside(p);
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
      str.clear();

    os << "set_charts_property -plot $" << plotName().toStdString();

    os << " -name " << nv.first.toStdString() << " -value {" << str.toStdString() << "}\n";
  }
}
