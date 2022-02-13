#include <CQChartsMapKey.h>
#include <CQChartsPlot.h>
#include <CQChartsEditHandles.h>
#include <CQChartsSymbolSet.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColorsPalette.h>

#include <QScrollArea>
#include <QVBoxLayout>
#include <QMouseEvent>

CQChartsMapKey::
CQChartsMapKey(Plot *plot) :
 CQChartsBoxObj(plot),
 CQChartsObjTextData      <CQChartsMapKey>(this),
 CQChartsObjHeaderTextData<CQChartsMapKey>(this)
{
  setEditable(true);

  setFilled(true);
  setFillAlpha(Alpha(0.5));

  //---

  Font font;

  font.decFontSize(4);

  setTextFont(font);

  setTextColor(Color(Color::Type::INTERFACE_VALUE, 1.0));
}

void
CQChartsMapKey::
setLocation(const Location &l)
{
  CQChartsUtil::testAndSet(location_, l, [&]() { invalidate(); } );
}

void
CQChartsMapKey::
setInsideX(bool b)
{
  CQChartsUtil::testAndSet(insideX_, b, [&]() { invalidate(); } );
}

void
CQChartsMapKey::
setInsideY(bool b)
{
  CQChartsUtil::testAndSet(insideY_, b, [&]() { invalidate(); } );
}

void
CQChartsMapKey::
setMargin(double m)
{
  CQChartsUtil::testAndSet(margin_, m, [&]() { invalidate(); } );
}

void
CQChartsMapKey::
setPadding(double p)
{
  CQChartsUtil::testAndSet(padding_, p, [&]() { invalidate(); } );
}

void
CQChartsMapKey::
setPosition(const Position &p)
{
  CQChartsUtil::testAndSet(position_, p, [&]() { invalidate(); } );
}

void
CQChartsMapKey::
setAlign(const Qt::Alignment &a)
{
  CQChartsUtil::testAndSet(align_, a, [&]() { invalidate(); } );
}

//---

void
CQChartsMapKey::
setHeaderStr(const QString &s)
{
  CQChartsUtil::testAndSet(header_, s, [&]() { invalidate(); } );
}

//---

void
CQChartsMapKey::
calcPosition(Position &pos, Qt::Alignment &align) const
{
  auto ibbox = plot_->displayRangeBBox();
  auto obbox = plot_->calcGroupedDataRange(CQChartsPlot::RangeTypes().setAxes().setTitle());

  auto bbox = ibbox;

  if (! isInsideX()) {
    bbox.setXMin(obbox.getXMin());
    bbox.setXMax(obbox.getXMax());
  }

  if (! isInsideY()) {
    bbox.setYMin(obbox.getYMin());
    bbox.setYMax(obbox.getYMax());
  }

  double x = bbox.getXMin();
  double y = bbox.getYMin();

  if (location().type() == Location::Type::ABSOLUTE_POSITION) {
    if (! position().isValid()) {
      double bm = this->padding();

      auto pbbox = plot()->calcPlotPixelRect();

      double px = pbbox.getXMax() - kw_/2.0 - bm;
      double py = pbbox.getYMax() - kh_/2.0 - bm;

      auto *th = const_cast<CQChartsMapKey *>(this);

      th->setPosition(Position::pixel(Point(px, py)));
    }

    pos   = this->position();
    align = this->align   ();
  }
  else {
    double xpad = 0, ypad = 0;

    if (! drawData_.isWidget) {
      xpad = plot()->pixelToWindowWidth (this->margin());
      ypad = plot()->pixelToWindowHeight(this->margin());
    }

    align = Qt::Alignment();

    if      (location().onLeft   ()) {
      x = bbox.getXMin() + xpad; align |= (isInsideX() ? Qt::AlignLeft : Qt::AlignRight); }
    else if (location().onHCenter()) {
      x = bbox.getXMid(); align |=  Qt::AlignHCenter; }
    else if (location().onRight  ()) {
      x = bbox.getXMax() - xpad; align |= (isInsideX() ? Qt::AlignRight : Qt::AlignLeft); }

    if      (location().onTop    ()) {
      y = bbox.getYMax() - ypad; align |= (isInsideY() ? Qt::AlignTop  : Qt::AlignBottom); }
    else if (location().onVCenter()) {
      y = bbox.getYMid(); align |= Qt::AlignVCenter; }
    else if (location().onBottom ()) {
      y = bbox.getYMin() + ypad; align |= (isInsideY() ? Qt::AlignBottom : Qt::AlignTop); }

    pos = Position::plot(Point(x, y));
  }
}

void
CQChartsMapKey::
calcCenter()
{
  if (drawData_.isWidget) {
    xm_ = kw_/2.0;
    ym_ = kh_/2.0;

    talign_ = Qt::AlignHCenter | Qt::AlignVCenter;

    return;
  }

  // calc center
  Position      position;
  Qt::Alignment align;

  calcPosition(position, align);

  Point pos = positionToPixel(position);

  xm_ = pos.x;
  ym_ = pos.y;

  talign_ = align;
}

void
CQChartsMapKey::
calcAlignedBBox()
{
  // calc bbox and align
  Point p1(xm_ - kw_/2.0, ym_ - kh_/2.0);
  Point p2(xm_ + kw_/2.0, ym_ + kh_/2.0);

  pbbox_ = BBox(p1.x, p1.y, p2.x, p2.y);

  if (drawData_.isWidget)
    return;

  double dx = 0.0;
  double dy = 0.0;

  if      (talign_ & Qt::AlignLeft ) dx =  kw_/2.0;
  else if (talign_ & Qt::AlignRight) dx = -kw_/2.0;

  if      (talign_ & Qt::AlignBottom) dy = -kh_/2.0;
  else if (talign_ & Qt::AlignTop   ) dy =  kh_/2.0;

  pbbox_ = pbbox_.translated(dx, dy);
}

void
CQChartsMapKey::
addProperties(PropertyModel *model, const QString &path, const QString &desc)
{
  auto addProp = [&](const QString &name, const QString &desc, bool hidden=false) {
    auto *item = model->addProperty(path, this, name);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

#if 0
  auto addStyleProp = [&](const QString &name, const QString &desc) {
    auto *item = addProp(name, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };
#endif

  //---

  addProp("location", "Location");
  addProp("insideX" , "Inside Plot X");
  addProp("insideY" , "Inside Plot Y");
  addProp("margin"  , "Margin");
  addProp("padding" , "Padding");
  addProp("position", "Position");
  addProp("align"   , "Alignment");

  //addProp("numeric"  , "Is Numeric");
  //addProp("integral" , "Is Integral");
  //addProp("native"   , "Is Native");
  //addProp("mapped"   , "Is Mapped");
  //addProp("numUnique", "Number of Unique Values");

  //---

  // header text
  auto headerPath     = path + "/header";
  auto headerTextPath = headerPath + "/text";

  auto addHeaderProp = [&](const QString &name, const QString &alias,
                           const QString &desc, bool hidden=false) {
    auto *item = model->addProperty(headerTextPath, this, name, alias);
    item->setDesc("Key header text " + desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  auto addHeaderStyleProp = [&](const QString &name, const QString &alias,
                                const QString &desc, bool hidden=false) {
    auto *item = addHeaderProp(name, alias, desc, hidden);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  addHeaderProp("header", "string", "string");

  addHeaderStyleProp("headerTextColor"     , "color"     , "header text color");
  addHeaderStyleProp("headerTextAlpha"     , "alpha"     , "header text alpha");
  addHeaderStyleProp("headerTextFont"      , "font"      , "header text font");
//addHeaderStyleProp("headerTextAngle"     , "angle"     , "header text angle");
  addHeaderStyleProp("headerTextContrast"  , "contrast"  , "header text contrast");
//addHeaderStyleProp("headerTextAlign"     , "align"     , "header text align");
//addHeaderStyleProp("headerTextFormatted" , "formatted" , "header text formatted to fit box");
//addHeaderStyleProp("headerTextScaled"    , "scaled"    , "header text scaled to box");
//addHeaderStyleProp("headerTextHtml"      , "html"      , "header text is html");
  addHeaderStyleProp("headerTextClipLength", "clipLength", "header text clipped to length");
  addHeaderStyleProp("headerTextClipElide" , "clipElide" , "header text clip elide");

  //---

  // stroke, fill
  addBoxProperties(model, path, desc);

  //---

  // key text
  auto textPath = path + "/text";

  auto addTextProp = [&](const QString &name, const QString &alias,
                         const QString &desc, bool hidden=false) {
    auto *item = model->addProperty(textPath, this, name, alias);
    item->setDesc("Key text " + desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  auto addTextStyleProp = [&](const QString &name, const QString &alias,
                              const QString &desc, bool hidden=false) {
    auto *item = addTextProp(name, alias, desc, hidden);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  addTextStyleProp("textColor"     , "color"     , "text color");
  addTextStyleProp("textAlpha"     , "alpha"     , "text alpha");
  addTextStyleProp("textFont"      , "font"      , "text font");
//addTextStyleProp("textAngle"     , "angle"     , "text angle");
  addTextStyleProp("textContrast"  , "contrast"  , "text contrast");
//addTextStyleProp("textAlign"     , "align"     , "text align");
//addTextStyleProp("textFormatted" , "formatted" , "text formatted to fit box");
//addTextStyleProp("textScaled"    , "scaled"    , "text scaled to box");
//addTextStyleProp("textHtml"      , "html"      , "text is html");
  addTextStyleProp("textClipLength", "clipLength", "text clipped to length");
  addTextStyleProp("textClipElide" , "clipElide" , "text clip elide");
}

QFont
CQChartsMapKey::
calcDrawFont(const Font &textFont) const
{
  if (drawData_.isWidget)
    return textFont.calcFont(drawData_.font);
  else
    return calcFont(textFont);
}

void
CQChartsMapKey::
setDrawPainterFont(PaintDevice *device, const Font &textFont)
{
  if (drawData_.isWidget)
    device->setFont(textFont.calcFont(drawData_.font));
  else
    setPainterFont(device, textFont);
}

bool
CQChartsMapKey::
editPress(const Point &p)
{
  editHandles()->setDragPos(p);

  return true;
}

bool
CQChartsMapKey::
editMove(const Point &p)
{
  const auto &dragPos  = editHandles()->dragPos();
  const auto &dragSide = editHandles()->dragSide();

  if (dragSide != CQChartsResizeSide::MOVE)
    return false;

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  editHandles()->updateBBox(dx, dy);

  double x = editHandles()->bbox().getXMid();
  double y = editHandles()->bbox().getYMid();

  if (location().type() == Location::Type::ABSOLUTE_POSITION) {
    if      (talign_ & Qt::AlignLeft ) x = editHandles()->bbox().getXMin();
    else if (talign_ & Qt::AlignRight) x = editHandles()->bbox().getXMax();

    if      (talign_ & Qt::AlignBottom) y = editHandles()->bbox().getYMin();
    else if (talign_ & Qt::AlignTop   ) y = editHandles()->bbox().getYMax();
  }
  else {
    setAlign(Qt::AlignHCenter | Qt::AlignVCenter);
  }

  setLocation(Location(Location::Type::ABSOLUTE_POSITION));
  setPosition(CQChartsPosition::plot(Point(x, y)));

  editHandles()->setDragPos(p);

  plot_->invalidateLayer(CQChartsBuffer::Type::FOREGROUND);

  return true;
}

bool
CQChartsMapKey::
editMotion(const Point &p)
{
  return editHandles()->selectInside(p);
}

QColor
CQChartsMapKey::
bgColor(PaintDevice *device) const
{
  auto *w = dynamic_cast<QWidget *>(device->painter()->device());
  if (! w) return Qt::white;

  return w->palette().window().color();
}

void
CQChartsMapKey::
setEditHandlesBBox() const
{
  auto rect = this->rect();
  if (! rect.isValid()) return;

  auto *th = const_cast<CQChartsMapKey *>(this);

  th->editHandles()->setBBox(th->typeBBox_[DrawType::VIEW]);
}

//---

CQChartsColorMapKey::
CQChartsColorMapKey(Plot *plot) :
 CQChartsMapKey(plot)
{
  setLocation(Location(Location::Type::BOTTOM_RIGHT));
}

void
CQChartsColorMapKey::
addProperties(PropertyModel *model, const QString &path, const QString &desc)
{
  auto addProp = [&](const QString &name, const QString &desc, bool hidden=false) {
    auto *item = model->addProperty(path, this, name);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  addProp("dataMin", "Model Data Min");
  addProp("dataMax", "Model Data Max");

  addProp("mapMin", "Color Value Min");
  addProp("mapMax", "Color Value Max");

  addProp("paletteName", "Palette Name");

  //---

  CQChartsMapKey::addProperties(model, path, desc);
}

bool
CQChartsColorMapKey::
isContiguous() const
{
  int n = numUnique();

  if (isIntegral() && (n > 0 && n <= plot()->maxMappedValues()))
    return false;

  return isNumeric();
}

bool
CQChartsColorMapKey::
inside(const Point &p, DrawType drawType) const
{
  auto pb = itemBoxes_.find(drawType);
  if (pb == itemBoxes_.end()) return false;

  const auto &itemBoxes = (*pb).second;

  for (const auto &itemBox : itemBoxes)
    if (itemBox.rect.inside(p))
      return true;

  return false;
}

void
CQChartsColorMapKey::
draw(PaintDevice *device, const DrawData &drawData, DrawType drawType)
{
  auto &itemBoxes = itemBoxes_[drawType];

  itemBoxes.clear();

  //---

  calcSize(drawData);

  //---

  if (isContiguous())
    drawContiguous(device);
  else
    drawDiscreet(device, drawType);

  drawHeader(device);

  //---

  auto *th = const_cast<CQChartsColorMapKey *>(this);

  th->typeBBox_[drawType] = th->bbox();
}

void
CQChartsColorMapKey::
drawHeader(PaintDevice *device)
{
  if (! headerStr().length())
    return;

  auto bg = bgColor(device);

  //---

  auto drawHeaderTextLabel = [&](const Point &p, const QString &label, bool hidden=false) {
    // set text pen
    auto tc = interpHeaderTextColor(ColorInd());

    auto tc1 = tc;

    if (hidden)
      tc1 = CQChartsUtil::blendColors(tc1, bg, 0.5);

    CQChartsPenBrush tpenBrush;

    setPenBrush(tpenBrush, PenData(true, tc1, headerTextAlpha()), BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, tpenBrush);

    //---

    // set font
    setDrawPainterFont(device, headerTextFont());

    //---

    auto p1 = device->pixelToWindow(p);

    auto textOptions = this->headerTextOptions();

    textOptions.angle = Angle();
    textOptions.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, label, textOptions);
  };

  //---

  double bm = this->padding();

  auto headerFont = calcDrawFont(headerTextFont());

  QFontMetricsF hfm(headerFont);

  auto py1 = pbbox_.getYMin() + bm; // top

  auto hx = pbbox_.getXMid() - hfm.horizontalAdvance(headerStr())/2.0;
  auto hy = py1 + hfm.ascent();

  drawHeaderTextLabel(Point(hx, hy), headerStr());
}

void
CQChartsColorMapKey::
drawContiguous(PaintDevice *device)
{
  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  double bm = this->padding();
  double bw = fm.horizontalAdvance("X") + 16; // color box width

  //---

  // calc text width
  auto dataMid = CMathUtil::avg(dataMin(), dataMax());

  //---

  calcCenter();

  calcAlignedBBox();

  auto *th = const_cast<CQChartsColorMapKey *>(this);

  th->setBBox(device->pixelToWindow(pbbox_));

  //---

  // draw box border and background
  CQChartsBoxObj::draw(device, bbox());

  //---

  double gy1 = pbbox_.getYMin() + bm;
  double gy2 = pbbox_.getYMax() - bm;

  gy1 += yoffset_;

  // draw gradient
  BBox gbbox(pbbox_.getXMin() + bm     , gy1 + fm.height()/2.0,
             pbbox_.getXMin() + bw + bm, gy2 - fm.height()/2.0);

  QLinearGradient lg(gbbox.getXMid(), gbbox.getYMax(), gbbox.getXMid(), gbbox.getYMin());

  CQColorsPalette *colorsPalette = nullptr;

  if (paletteName().isValid())
    colorsPalette = paletteName().palette();
  else
    colorsPalette = view()->themePalette();

  colorsPalette->setLinearGradient(lg, 1.0, mapMin(), mapMax());

  QBrush brush(lg);

  device->setBrush(brush);

  device->fillRect(device->pixelToWindow(gbbox));

  //---

  // set text pen
  CQChartsPenBrush tpenBrush;

  auto tc = interpTextColor(ColorInd());

  setPenBrush(tpenBrush, PenData(true, tc, textAlpha()), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, tpenBrush);

  //---

  // set font
  setDrawPainterFont(device, textFont());

  //---

  // draw labels
  auto drawTextLabel = [&](const Point &p, const QString &label) {
    auto p1 = device->pixelToWindow(p);

    auto textOptions = this->textOptions();

    textOptions.angle = Angle();
    textOptions.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, label, textOptions);
  };

  double df = (fm.ascent() - fm.descent())/2.0;

  double tx  = gbbox.getXMax() + bm;
  double ty1 = gbbox.getYMin() + df;
  double ty2 = gbbox.getYMid() + df;
  double ty3 = gbbox.getYMax() + df;

  auto dataMinStr = valueText(dataMin());
  auto dataMidStr = valueText(dataMid  );
  auto dataMaxStr = valueText(dataMax());

  drawTextLabel(Point(tx, ty3), dataMinStr);
  drawTextLabel(Point(tx, ty2), dataMidStr);
  drawTextLabel(Point(tx, ty1), dataMaxStr);
}

void
CQChartsColorMapKey::
drawDiscreet(PaintDevice *device, DrawType drawType)
{
  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  double bm = this->padding();
  double bs = fm.horizontalAdvance("X") + 16; // color box size

  //---

  calcCenter();

  calcAlignedBBox();

  auto *th = const_cast<CQChartsColorMapKey *>(this);

  th->setBBox(device->pixelToWindow(pbbox_));

  //---

  // draw box border and background
  CQChartsBoxObj::draw(device, bbox());

  //---

  CQColorsPalette *colorsPalette = nullptr;

  if (paletteName().isValid())
    colorsPalette = paletteName().palette();
  else
    colorsPalette = view()->themePalette();

  double bx = pbbox_.getXMin() + bm;
  double by = pbbox_.getYMin() + bm;

  by += yoffset_;

  double bmi = 2;
  double bsi = bs - 2*bmi;

  //---

  auto bg = bgColor(device);

  // draw boxes
  auto &itemBoxes = itemBoxes_[drawType];

  double min = this->mapMin();
  double max = this->mapMax();

  int n = numUnique();

  for (int i = 0; i < n; ++i) {
    auto name = uniqueValues()[i].toString();

    Color  color;
    QColor c;

    if (! colorData_.colorMap.valueToColor(name, color)) {
      double r = CMathUtil::map(i, 0, n - 1, min, max);

      c = colorsPalette->getColor(r);
    }
    else
      c = color.color();

    //---

    auto hidden = ! plot()->colorVisible(c);

    auto c1 = c;

    if (hidden)
      c1 = CQChartsUtil::blendColors(c1, bg, 0.5);

    //---

    double bxi = bx + bmi;
    double byi = by + i*bs + bmi;

    BBox pbbox(bxi, byi, bxi + bsi, byi + bsi);

    device->setPen  (QColor(Qt::black));
    device->setBrush(c1);

    auto bbox = device->pixelToWindow(pbbox);

    device->drawRect(bbox);

    //---

    BBox ipbbox(pbbox_.getXMin(), byi - bmi, pbbox_.getXMax(), byi - bmi + bs);

    itemBoxes.push_back(ItemBox(c, device->pixelToWindow(ipbbox)));
  }

  //---

  // draw labels
  auto drawTextLabel = [&](const Point &p, const QString &label) {
    auto p1 = device->pixelToWindow(p);

    auto textOptions = this->textOptions();

    textOptions.angle = Angle();
    textOptions.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, label, textOptions);
  };

  //---

  // set font
  setDrawPainterFont(device, textFont());

  auto tc = interpTextColor(ColorInd());

  //---

  double tx = pbbox_.getXMin() + bs + 2*bm;
  double ty = pbbox_.getYMin() + bm;

  ty += yoffset_;

  double df = (fm.ascent() - fm.descent())/2.0;

  for (int i = 0; i < n; ++i) {
    auto name = uniqueValues()[i].toString();

    Color  color;
    QColor c;

    if (! colorData_.colorMap.valueToColor(name, color)) {
      double r = CMathUtil::map(i, 0, n - 1, min, max);

      c = colorsPalette->getColor(r);
    }
    else
      c = color.color();

    //---

    // set text pen
    auto hidden = ! plot()->colorVisible(c);

    auto tc1 = tc;

    if (hidden)
      tc1 = CQChartsUtil::blendColors(tc, bg, 0.5);

    CQChartsPenBrush tpenBrush;

    setPenBrush(tpenBrush, PenData(true, tc1, textAlpha()), BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, tpenBrush);

    //---

    drawTextLabel(Point(tx, ty + (i + 0.5)*bs + df), name);
  }
}

QSize
CQChartsColorMapKey::
calcSize(const DrawData &drawData) const
{
  drawData_ = drawData;

  if (isContiguous())
    (void) calcContiguousSize();
  else
    (void) calcDiscreetSize();

  auto hh = calcHeaderSize();

  xoffset_ = 0.0;

  if (hh.height() > 0) {
    double bm = this->padding();

    double hw = hh.width() + 2*bm;

    if (hw > kw_) {
      xoffset_ = hw - kw_;
      kw_      = hw;
    }

    kh_ += hh.height() + bm;
  }

  return QSize(int(kw_), int(kh_));
}

QSize
CQChartsColorMapKey::
calcHeaderSize() const
{
  yoffset_ = 0.0;

  double hw = 0, hh = 0;

  if (headerStr().length()) {
    double bm = this->padding();

    auto headerFont = calcDrawFont(headerTextFont());

    QFontMetricsF hfm(headerFont);

    hw = hfm.horizontalAdvance(headerStr());
    hh = hfm.height();

    yoffset_ = hh + bm;
  }

  return QSize(int(hw), int(hh));
}

QSize
CQChartsColorMapKey::
calcContiguousSize() const
{
  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  double bm = this->padding();

  double bw = fm.horizontalAdvance("X") + 16; // color box width
  double bh = fm.height()*5;                  // color box height

  //---

  // calc text width
  auto dataMid = CMathUtil::avg(dataMin(), dataMax());

  auto dataMinStr = valueText(dataMin());
  auto dataMidStr = valueText(dataMid  );
  auto dataMaxStr = valueText(dataMax());

  double tw = std::max(std::max(fm.horizontalAdvance(dataMinStr),
                                fm.horizontalAdvance(dataMidStr)),
                       fm.horizontalAdvance(dataMaxStr));

  //---

  kw_ = bw + tw + 3*bm;
  kh_ = bh + fm.height() + 2*bm;

  return QSize(int(kw_), int(kh_));
}

QSize
CQChartsColorMapKey::
calcDiscreetSize() const
{
  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  double bm = this->padding();

  double bs = fm.horizontalAdvance("X") + 16; // color box size

  //---

  int n = numUnique();

  double tw = 0.0;

  for (int i = 0; i < n; ++i) {
    auto name = uniqueValues()[i].toString();

    tw = std::max(tw, fm.horizontalAdvance(name));
  }

  //---

  kw_ = bs + tw + 3*bm;
  kh_ = n*bs + 2*bm;

  return QSize(int(kw_), int(kh_));
}

void
CQChartsColorMapKey::
invalidate()
{
  emit dataChanged();
}

QString
CQChartsColorMapKey::
valueText(double value) const
{
  QString text;

  if (! isIntegral()) {
    if      (CMathUtil::realEq(value, 0.0))
      text = "0.0";
    else if (CMathUtil::realEq(value, 1.0))
      text = "1.0";
    else
      text.setNum(value);
  }
  else {
    int ivalue = int(value);

    text.setNum(ivalue);
  }

  return text;
}

bool
CQChartsColorMapKey::
selectPress(const Point &p, SelMod selMod)
{
  return selectPressType(p, selMod, DrawType::VIEW);
}

bool
CQChartsColorMapKey::
selectPressType(const Point &p, SelMod selMod, DrawType drawType)
{
  const auto &itemBoxes = itemBoxes_[drawType];

  auto ni = itemBoxes.size();

  std::vector<int> oldItemVisible;

  oldItemVisible.resize(ni);

  for (size_t i = 0; i < ni; ++i)
    oldItemVisible[i] = plot()->colorVisible(itemBoxes[i].color);

  auto newItemVisible = oldItemVisible;

  for (size_t i = 0; i < ni; ++i) {
    bool inside = itemBoxes[i].rect.inside(p);

    if      (selMod == CQChartsSelMod::ADD) {
      if (inside)
        newItemVisible[i] = true;
    }
    else if (selMod == CQChartsSelMod::REPLACE)
      newItemVisible[i] = inside;
    else if (selMod == CQChartsSelMod::REMOVE)
      newItemVisible[i] = ! inside;
    else if (selMod == CQChartsSelMod::TOGGLE) {
      if (inside)
        newItemVisible[i] = ! newItemVisible[i];
    }
  }

  for (size_t i = 0; i < ni; ++i)
    if (oldItemVisible[i] != newItemVisible[i])
      emit itemSelected(itemBoxes[i].color, newItemVisible[i]);

  return false;
}

//------

CQChartsSymbolSizeMapKey::
CQChartsSymbolSizeMapKey(Plot *plot) :
 CQChartsMapKey(plot)
{
  setLocation(Location(Location::Type::BOTTOM_CENTER));
}

void
CQChartsSymbolSizeMapKey::
addProperties(PropertyModel *model, const QString &path, const QString &desc)
{
  auto addProp = [&](const QString &name, const QString &desc, bool hidden=false) {
    auto *item = model->addProperty(path, this, name);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  auto addStyleProp = [&](const QString &name, const QString &desc, bool hidden=false) {
    auto *item = addProp(name, desc, hidden);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  addProp("dataMin", "Model Data Min");
  addProp("dataMax", "Model Data Max");

  addProp("mapMin", "Symbol Size Min");
  addProp("mapMax", "Symbol Size Max");

  addProp("scale"  , "Scale Factor");
  addProp("stacked", "Stacked Vertical instead of overlaid");
  addProp("rows"   , "Number of symbol rows");

  addProp("textAlign"  , "Text Align");
  addProp("paletteName", "Palette Name");

  addStyleProp("alpha", "Alpha");

  //---

  CQChartsMapKey::addProperties(model, path, desc);
}

bool
CQChartsSymbolSizeMapKey::
isContiguous() const
{
  int n = numUnique();

  if (isIntegral() && (n > 0 && n <= plot()->maxMappedValues()))
    return false;

  return isNumeric();
}

bool
CQChartsSymbolSizeMapKey::
inside(const Point &p, DrawType drawType) const
{
  auto pb = itemBoxes_.find(drawType);
  if (pb == itemBoxes_.end()) return false;

  const auto &itemBoxes = (*pb).second;

  for (const auto &itemBox : itemBoxes)
    if (itemBox.rect.inside(p))
      return true;

  return false;
}

void
CQChartsSymbolSizeMapKey::
draw(PaintDevice *device, const DrawData &drawData, DrawType drawType)
{
  auto &itemBoxes = itemBoxes_[drawType];

  itemBoxes.clear();

  //---

  drawData_ = drawData;

  initDraw(device);

  //---

  drawBorder(device, drawData_.usePenBrush);

  drawHeader(device);

  if (isContiguous())
    drawContiguous(device, drawType);
  else
    drawDiscreet(device, drawType);

  //---

  auto *th = const_cast<CQChartsSymbolSizeMapKey *>(this);

  th->typeBBox_[drawType] = th->bbox();
}

void
CQChartsSymbolSizeMapKey::
initDraw(PaintDevice *device)
{
  calcCenter();

  pcenter_ = Point(xm_, ym_);
  center_  = device->pixelToWindow(pcenter_);

  (void) calcSize(drawData_);

  //---

  double bm = this->padding();

  auto hs = calcHeaderSize();

  if (hs.height() > 0) {
    kw_  = std::max(kw_, hs.width() + 2*bm);
    kh_ += hs.height() + bm;

    if (isContiguous()) {
      auto xc = pbbox_.getXMid();
      auto yc = pbbox_.getYMid();

      pbbox_ = BBox(xc - kw_/2.0, yc - kh_/2.0, xc + kw_/2.0, yc + kh_/2.0);
    }
    else
      pbbox_ = BBox(0, 0, kw_, kh_);
  }

  //---

  if (headerStr().length()) {
    auto headerFont = calcDrawFont(headerTextFont());

    QFontMetricsF hfm(headerFont);

    yoffset_ = hfm.height() + bm;
  }
  else
    yoffset_ = 0.0;

  //---

  alignBoxes(device);
}

void
CQChartsSymbolSizeMapKey::
drawHeader(PaintDevice *device)
{
  if (! headerStr().length())
    return;

  auto bg = bgColor(device);

  //---

  auto drawHeaderTextLabel = [&](const Point &p, const QString &label, bool hidden=false) {
    // set text pen
    auto tc = interpHeaderTextColor(ColorInd());

    auto tc1 = tc;

    if (hidden)
      tc1 = CQChartsUtil::blendColors(tc1, bg, 0.5);

    CQChartsPenBrush tpenBrush;

    setPenBrush(tpenBrush, PenData(true, tc1, headerTextAlpha()), BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, tpenBrush);

    //---

    // set font
    setDrawPainterFont(device, headerTextFont());

    //---

    auto p1 = device->pixelToWindow(p);

    auto textOptions = this->headerTextOptions();

    textOptions.angle = Angle();
    textOptions.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, label, textOptions);
  };

  //---

  double bm = this->padding();

  auto headerFont = calcDrawFont(headerTextFont());

  QFontMetricsF hfm(headerFont);

  auto py1 = pbbox_.getYMin() + bm; // top

  auto hx = pbbox_.getXMid() - hfm.horizontalAdvance(headerStr())/2.0;
  auto hy = py1 + hfm.ascent();

  drawHeaderTextLabel(Point(hx, hy), headerStr());
}

void
CQChartsSymbolSizeMapKey::
drawContiguous(PaintDevice *device, DrawType drawType)
{
  drawCircles(device, drawType, drawData_.usePenBrush);

  //---

  // set text pen
  CQChartsPenBrush tpenBrush;

  auto tc = interpTextColor(ColorInd());

  setPenBrush(tpenBrush, PenData(true, tc, textAlpha()), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, tpenBrush);

  //---

  // set font
  setDrawPainterFont(device, textFont());

  //---

  auto textOptions = this->textOptions();

  textOptions.angle = Angle();
  textOptions.align = Qt::AlignRight | Qt::AlignVCenter;

  drawText(device, textOptions, drawData_.usePenBrush);
}

void
CQChartsSymbolSizeMapKey::
drawDiscreet(PaintDevice *device, DrawType drawType)
{
  //(void) calcDiscreetSize();

  //---

  // draw labels
  auto drawTextLabel = [&](const Point &p, const QString &label) {
    auto p1 = device->pixelToWindow(p);

    auto textOptions = this->textOptions();

    textOptions.angle = Angle();
    textOptions.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, label, textOptions);
  };

  //---

  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  // outer margin
  double bm = this->padding();
  double bh = fm.height() + 1; // row height

  //---

  auto bg = bgColor(device);

  double min = this->mapMin();
  double max = this->mapMax();

  //---

  // draw labels
  auto &itemBoxes = itemBoxes_[drawType];

  //---

  // set font
  setDrawPainterFont(device, textFont());

  auto tc = interpTextColor(ColorInd());

  //---

  double tw = (twl_ + bm + twr_);

  double tx1 = pbbox_.getXMid() - tw/2;
  double tx2 = tx1 + twl_ + bm;
  double ty  = pbbox_.getYMin() + bm;

  ty += yoffset_;

  double df = (fm.ascent() - fm.descent())/2.0;

  int n = numUnique();

  for (int i = 0; i < n; ++i) {
    auto name = uniqueValues()[i].toString();

    Length  l;
    QString lstr;

    if (! symbolSizeData_.sizeMap.valueToLength(name, l)) {
      double r = CMathUtil::map(i, 0, n - 1, min, max);

      l    = Length::pixel(r);
      lstr = QString("%1").arg(r, 0, 'f', ndp_);
    }
    else {
      lstr = l.toString();
    }

    //---

    // set text pen
    auto hidden = ! plot()->symbolSizeVisible(l);

    auto tc1 = tc;

    if (hidden)
      tc1 = CQChartsUtil::blendColors(tc, bg, 0.5);

    CQChartsPenBrush tpenBrush;

    setPenBrush(tpenBrush, PenData(true, tc1, textAlpha()), BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, tpenBrush);

    //---

    auto dxl = twl_ - fm.horizontalAdvance(lstr);

    auto ty1 = ty + (i + 0.5)*bh;

    drawTextLabel(Point(tx1 + dxl, ty1 + df), lstr);
    drawTextLabel(Point(tx2      , ty1 + df), name);

    //---

    //device->drawLine(Point(tx1, pbbox_.getXMin()), Point(tx1, pbbox_.getXMax()));
    //device->drawLine(Point(tx2, pbbox_.getXMin()), Point(tx2, pbbox_.getXMax()));

    //---

    BBox ipbbox(pbbox_.getXMin(), ty1 - bh/2.0, pbbox_.getXMax(), ty1 + bh/2.0);

    itemBoxes.push_back(ItemBox(l, device->pixelToWindow(ipbbox)));
  }
}

void
CQChartsSymbolSizeMapKey::
drawBorder(PaintDevice *device, bool /*usePenBrush*/)
{
  // draw box border and background
  CQChartsBoxObj::draw(device, bbox());
}

void
CQChartsSymbolSizeMapKey::
drawCircles(PaintDevice *device, DrawType drawType, bool usePenBrush)
{
  auto drawEllipse = [&](const QColor &c, const BBox &pbbox) {
    PenBrush penBrush;

    if (! usePenBrush) {
      auto strokeColor = plot()->interpThemeColor(ColorInd(1.0));

      setPenBrush(penBrush, PenData(true, strokeColor), BrushData(true, c, this->alpha()));
    }
    else {
      penBrush.pen   = device->pen();
      penBrush.brush = device->brush();

      penBrush.brush.setColor(c);
    }

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    device->drawEllipse(device->pixelToWindow(pbbox));
  };

  //---

  if (symbolBoxes_.empty())
    return;

  //---

  // draw ellipse for min, mean, max radii
  auto *palette = (paletteName_.isValid() ? paletteName_.palette() : nullptr);

  if (! palette)
    palette = plot()->charts()->themePalette(0);

  double ymin = (palette ? paletteMinMax_.min() : 0.0);
  double ymax = (palette ? paletteMinMax_.max() : 1.0);

  double y  = ymax;
  double dy = (symbolBoxes_.size() > 1 ? (ymax - ymin)/double(symbolBoxes_.size() - 1) : 0.0);

  auto &itemBoxes = itemBoxes_[drawType];

  for (const auto &pbbox : symbolBoxes_) {
    auto fillColor = (palette ?
      palette->getColor(y, /*scale*/false, /*invert*/false) :
      plot()->interpPaletteColor(ColorInd(y)));

    drawEllipse(fillColor, pbbox);

    y -= dy;

    itemBoxes.push_back(ItemBox(Length(), device->pixelToWindow(pbbox)));
  }
}

void
CQChartsSymbolSizeMapKey::
drawText(PaintDevice *device, const CQChartsTextOptions &textOptions, bool usePenBrush)
{
  if (! usePenBrush) {
    PenBrush penBrush;

    auto strokeColor = plot()->interpThemeColor(ColorInd(1.0));

    setPenBrush(penBrush, PenData(true, strokeColor), BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, penBrush);
  }

  //---

  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  auto drawText = [&](const Point &p, double value, Qt::Alignment align) {
    auto text = valueText(value);

    double tw = fm.horizontalAdvance(text);

    auto textOptions1 = textOptions;

    textOptions1.align = Qt::AlignLeft;

    Point p1;

    if (! isStacked()) {
      p1 = Point(p.x - tw/2, p.y);
    }
    else {
      if      (align & Qt::AlignLeft)
        p1 = Point(p.x - tw    , p.y + (fm.ascent() - fm.descent())/2.0);
      else if (align & Qt::AlignRight)
        p1 = Point(p.x         , p.y + (fm.ascent() - fm.descent())/2.0);
      else
        p1 = Point(p.x - tw/2.0, p.y + (fm.ascent() - fm.descent())/2.0);
    }

    auto p2 = device->pixelToWindow(p1);

    CQChartsDrawUtil::drawTextAtPoint(device, p2, text, textOptions1);
  };

  //---

  if (symbolBoxes_.empty())
    return;

  const auto &pbbox1 = symbolBoxes_[0];

  //---

  double min = this->dataMin();
  double max = this->dataMax();

  double y  = 1.0;
  double dy = (symbolBoxes_.size() > 1 ? 1.0/double(symbolBoxes_.size() - 1) : 0.0);

  // outer margin
  double pm = this->padding();

  for (const auto &pbbox : symbolBoxes_) {
    double r = CMathUtil::map(y, 0.0, 1.0, min, max);

    if (! isStacked()) {
      drawText(Point(pbbox.getXMid(), pbbox.getYMin() - 2), r, textAlign());
    }
    else {
      if      (textAlign() & Qt::AlignLeft)
        drawText(Point(pbbox1.getXMin() - pm, pbbox.getYMid()), r, textAlign());
      else if (textAlign() & Qt::AlignRight)
        drawText(Point(pbbox1.getXMax() + pm, pbbox.getYMid()), r, textAlign());
      else
        drawText(Point(pbbox1.getXMid(), pbbox.getYMid()), r, textAlign());
    }

    y -= dy;
  }
}

QSize
CQChartsSymbolSizeMapKey::
calcSize(const DrawData &drawData) const
{
  drawData_ = drawData;

  if (isContiguous())
    return calcContiguousSize();
  else
    return calcDiscreetSize();
}

QSize
CQChartsSymbolSizeMapKey::
calcHeaderSize() const
{
  double hw = 0, hh = 0;

  if (headerStr().length()) {
    auto headerFont = calcDrawFont(headerTextFont());

    QFontMetricsF hfm(headerFont);

    hw = hfm.horizontalAdvance(headerStr());
    hh = hfm.height();
  }

  return QSize(int(hw), int(hh));
}

QSize
CQChartsSymbolSizeMapKey::
calcContiguousSize() const
{
  calcSymbolBoxes();
  calcTextBBox   ();

  pbbox_ = psbbox_ + ptbbox_;

  kw_ = pbbox_.getWidth();
  kh_ = pbbox_.getHeight();

  return QSize(int(kw_), int(kh_));
}

QSize
CQChartsSymbolSizeMapKey::
calcDiscreetSize() const
{
  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  // outer margin
  double bm = this->padding();
  double bh = fm.height() + 1; // row height

  //---

  // calc mapped values
  int n = numUnique();

  double min = this->mapMin();
  double max = this->mapMax();

  std::vector<double> mappedValues;

  for (int i = 0; i < n; ++i) {
    auto name = uniqueValues()[i].toString();

    Length l;

    if (! symbolSizeData_.sizeMap.valueToLength(name, l)) {
      double r = CMathUtil::map(i, 0, n - 1, min, max);

      mappedValues.push_back(r);
    }
    else
      mappedValues.push_back(0.0);
  }

  // get common ndp
  ndp_ = CQChartsUtil::valuesNdp(mappedValues);

  // get name/value max widths
  twl_ = 0.0;
  twr_ = 0.0;

  for (int i = 0; i < n; ++i) {
    auto name = uniqueValues()[i].toString();

    Length  l;
    QString lstr;

    if (! symbolSizeData_.sizeMap.valueToLength(name, l)) {
      auto r = mappedValues[size_t(i)];

      lstr = QString("%1").arg(r, 0, 'f', ndp_);
    }
    else {
      lstr = l.toString();
    }

    twl_ = std::max(twl_, fm.horizontalAdvance(lstr));
    twr_ = std::max(twr_, fm.horizontalAdvance(name));
  }

  //---

  kw_ = twl_ + twr_ + 3*bm;
  kh_ = n*bh + 2*bm;

  pbbox_ = BBox(0, 0, kw_, kh_);

  return QSize(int(kw_), int(kh_));
}

void
CQChartsSymbolSizeMapKey::
calcSymbolBoxes() const
{
  symbolBoxes_.clear();

  //---

  // symbol sizes
  double prmin = scale()*this->mapMin();
  double prmax = scale()*this->mapMax();

  //---

  if (rows() < 1)
    return;

  double sy = (rows() > 1 ? 1.0/(rows() - 1) : 0.0);

  // outer margin
  double pm = this->padding();

  //---

  if (! isStacked()) {
    double y = 1.0;

    double yb = prmax;

    for (int i = 0; i < rows(); ++i) {
      // row radius
      double pr = CMathUtil::map(y, 0.0, 1.0, prmin, prmax);

      auto pbbox = BBox(-pr, yb - 2*pr, pr, yb);

      symbolBoxes_.push_back(std::move(pbbox));

      y -= sy;
    }

    double ps = prmax + pm;

    psbbox_ = BBox(-ps, -ps, ps, ps);
  }
  else {
    double y = 1.0;

    double h = (rows() - 1)*pm;

    for (int i = 0; i < rows(); ++i) {
      h += 2*CMathUtil::map(y, 0.0, 1.0, prmin, prmax);

      y -= sy;
    }

    y = 1.0;

    double yt = -h/2;

    for (int i = 0; i < rows(); ++i) {
      double pr = CMathUtil::map(y, 0.0, 1.0, prmin, prmax);

      auto pbbox = BBox(-pr, yt + 2*pr, pr, yt);

      symbolBoxes_.push_back(std::move(pbbox));

      yt += 2*pr + pm;

      y -= sy;
    }

    double psx = prmax + pm;
    double psy = h/2 + pm;

    psbbox_ = BBox(-psx, psy, psx, psy);
  }
}

void
CQChartsSymbolSizeMapKey::
calcTextBBox() const
{
  if (symbolBoxes_.empty())
    return;

  const auto &pbbox1 = symbolBoxes_[0];

  //---

  double min = this->dataMin();
  double max = this->dataMax();

  // outer margin
  double pm = this->padding();

  double pxt1 = (! isStacked() ? pbbox1.getXMid() : pbbox1.getXMax() + pm);
  double pyt1 = (! isStacked() ? pbbox1.getYMid() : pbbox1.getYMin()     );
  double pxt2 = pxt1;
  double pyt2 = pyt1;

  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  double fh = fm.height();
  double fa = fm.ascent();

  double y  = 1.0;
  double dy = (symbolBoxes_.size() > 1 ? 1.0/double(symbolBoxes_.size() - 1) : 0.0);

  for (const auto &pbbox : symbolBoxes_) {
    double r = CMathUtil::map(y, 0.0, 1.0, min, max);

    auto text = valueText(r);

    double tw = fm.horizontalAdvance(text);

    if (! isStacked()) {
      pxt1 = std::min(pxt1, pbbox.getXMid() - tw/2.0 - pm);
      pxt2 = std::max(pxt2, pbbox.getXMid() + tw/2.0 + pm);

      pyt1 = std::min(pyt1, pbbox.getYMin() - 2 - fa - pm);
      pyt2 = std::max(pyt2, pbbox.getYMin() - 2      + pm);
    }
    else {
      if      (textAlign() & Qt::AlignLeft) {
        pxt1 = std::min(pxt1, pbbox1.getXMin() - pm - tw - pm);
      }
      else if (textAlign() & Qt::AlignRight) {
        pxt2 = std::max(pxt2, pbbox1.getXMax() + pm + tw + pm);
      }
      else {
        pxt1 = std::min(pxt2, pbbox1.getXMid() - tw/2.0 - pm);
        pxt2 = std::max(pxt2, pbbox1.getXMid() + tw/2.0 + pm);
      }

      pyt1 = std::min(pyt1, pbbox1.getYMid() - fh/2.0 - pm);
      pyt2 = std::max(pyt2, pbbox1.getYMid() - fh/2.0 + pm);
    }

    y -= dy;
  }

  //---

  ptbbox_ = BBox(pxt1, pyt1, pxt2, pyt2);
}

void
CQChartsSymbolSizeMapKey::
alignBoxes(PaintDevice *device) const
{
  bbox_ = device->pixelToWindow(pbbox_);

  if (isContiguous()) {
    sbbox_ = device->pixelToWindow(psbbox_);
    tbbox_ = device->pixelToWindow(ptbbox_);
  }

  //--

  double dx, dy;

  if (drawData_.isWidget) {
    dx = -bbox_.getXMin();
    dy = -bbox_.getYMin();
  }
  else {
    auto c = bbox_.getCenter();

    dx = center_.x - c.x;
    dy = center_.y - c.y;

    double w = bbox_.getWidth ();
    double h = bbox_.getHeight();

    if      (talign_ & Qt::AlignLeft ) dx += w/2.0;
    else if (talign_ & Qt::AlignRight) dx -= w/2.0;

    if      (talign_ & Qt::AlignBottom) dy += h/2.0;
    else if (talign_ & Qt::AlignTop   ) dy -= h/2.0;
  }

  bbox_ = bbox_.translated(dx, dy);

  if (isContiguous()) {
    sbbox_ = sbbox_.translated(dx, dy);
    tbbox_ = tbbox_.translated(dx, dy);

    double pdx = device->windowToSignedPixelWidth (dx);
    double pdy = device->windowToSignedPixelHeight(dy) + yoffset_/2.0;

    for (auto &pbbox : symbolBoxes_)
      pbbox = pbbox.translated(pdx, pdy);
  }

  //---

  pbbox_ = device->windowToPixel(bbox_);

  //---

  auto *th = const_cast<CQChartsSymbolSizeMapKey *>(this);

  th->setBBox(bbox_);
}

void
CQChartsSymbolSizeMapKey::
invalidate()
{
  emit dataChanged();
}

QString
CQChartsSymbolSizeMapKey::
valueText(double value) const
{
  QString text;

  if      (CMathUtil::realEq(value, 0.0))
    text = "0.0";
  else if (CMathUtil::realEq(value, 1.0))
    text = "1.0";
  else
    text.setNum(value);

  return text;
}

bool
CQChartsSymbolSizeMapKey::
selectPress(const Point &p, SelMod selMod)
{
  return selectPressType(p, selMod, DrawType::VIEW);
}

bool
CQChartsSymbolSizeMapKey::
selectPressType(const Point &p, SelMod selMod, DrawType drawType)
{
  const auto &itemBoxes = itemBoxes_[drawType];

  auto ni = itemBoxes.size();

  std::vector<int> oldItemVisible;

  oldItemVisible.resize(ni);

  for (size_t i = 0; i < ni; ++i)
    oldItemVisible[i] = plot()->symbolSizeVisible(itemBoxes[i].size);

  auto newItemVisible = oldItemVisible;

  for (size_t i = 0; i < ni; ++i) {
    bool inside = itemBoxes[i].rect.inside(p);

    if      (selMod == CQChartsSelMod::ADD) {
      if (inside)
        newItemVisible[i] = true;
    }
    else if (selMod == CQChartsSelMod::REPLACE)
      newItemVisible[i] = inside;
    else if (selMod == CQChartsSelMod::REMOVE)
      newItemVisible[i] = ! inside;
    else if (selMod == CQChartsSelMod::TOGGLE) {
      if (inside)
        newItemVisible[i] = ! newItemVisible[i];
    }
  }

  for (size_t i = 0; i < ni; ++i)
    if (oldItemVisible[i] != newItemVisible[i])
      emit itemSelected(itemBoxes[i].size, newItemVisible[i]);

  return false;
}

//------

CQChartsSymbolTypeMapKey::
CQChartsSymbolTypeMapKey(Plot *plot) :
 CQChartsMapKey(plot)
{
  setLocation(Location(Location::Type::BOTTOM_LEFT));
}

void
CQChartsSymbolTypeMapKey::
addProperties(PropertyModel *model, const QString &path, const QString &desc)
{
  auto addProp = [&](const QString &name, const QString &desc, bool hidden=false) {
    auto *item = model->addProperty(path, this, name);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  addProp("dataMin", "Model Data Min");
  addProp("dataMax", "Model Data Max");

  addProp("mapMin", "Symbol Type Value Min");
  addProp("mapMax", "Symbol Type Value Max");

  //---

  CQChartsMapKey::addProperties(model, path, desc);
}

bool
CQChartsSymbolTypeMapKey::
isContiguous() const
{
  int n = numUnique();

  if (isIntegral() && (n > 0 && n <= plot()->maxMappedValues()))
    return false;

  return isNumeric();
}

bool
CQChartsSymbolTypeMapKey::
inside(const Point &p, DrawType drawType) const
{
  auto pb = itemBoxes_.find(drawType);
  if (pb == itemBoxes_.end()) return false;

  const auto &itemBoxes = (*pb).second;

  for (const auto &itemBox : itemBoxes)
    if (itemBox.rect.inside(p))
      return true;

  return false;
}

void
CQChartsSymbolTypeMapKey::
draw(PaintDevice *device, const DrawData &drawData, DrawType drawType)
{
  auto &itemBoxes = itemBoxes_[drawType];

  itemBoxes.clear();

  //---

  drawData_ = drawData;

  calcSize(drawData_);

  //---

  calcCenter();

  calcAlignedBBox();

  auto *th = const_cast<CQChartsSymbolTypeMapKey *>(this);

  th->setBBox(device->pixelToWindow(pbbox_));

  //---

  // draw box border and background
  CQChartsBoxObj::draw(device, bbox());

  //---

  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  // outer margin
  double bm = this->padding();
  double bw = fm.horizontalAdvance("X") + 4;

  //---

  auto bg = bgColor(device);

  //---

  // draw symbols and labels
  auto drawHeaderTextLabel = [&](const Point &p, const QString &label, bool hidden=false) {
    // set text pen
    auto tc = interpHeaderTextColor(ColorInd());

    auto tc1 = tc;

    if (hidden)
      tc1 = CQChartsUtil::blendColors(tc1, bg, 0.5);

    CQChartsPenBrush tpenBrush;

    setPenBrush(tpenBrush, PenData(true, tc1, headerTextAlpha()), BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, tpenBrush);

    //---

    // set font
    setDrawPainterFont(device, headerTextFont());

    //---

    auto p1 = device->pixelToWindow(p);

    auto textOptions = this->headerTextOptions();

    textOptions.angle = Angle();
    textOptions.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, label, textOptions);
  };

  auto drawTextLabel = [&](const Point &p, const QString &label, bool hidden=false) {
    // set text pen
    auto tc = interpTextColor(ColorInd());

    auto tc1 = tc;

    if (hidden)
      tc1 = CQChartsUtil::blendColors(tc1, bg, 0.5);

    CQChartsPenBrush tpenBrush;

    setPenBrush(tpenBrush, PenData(true, tc1, textAlpha()), BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, tpenBrush);

    //---

    // set font
    setDrawPainterFont(device, textFont());

    //---

    auto p1 = device->pixelToWindow(p);

    auto textOptions = this->textOptions();

    textOptions.angle = Angle();
    textOptions.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, label, textOptions);
  };

  //---

  double fh = fm.height();
  double df = (fm.ascent() - fm.descent())/2.0;

  //---

  auto px  = pbbox_.getXMin() + bm;
  auto py1 = pbbox_.getYMin() + bm; // top
  auto py2 = pbbox_.getYMax() - bm; // bottom

  //---

  // draw header
  if (headerStr().length()) {
    auto headerFont = calcDrawFont(headerTextFont());

    QFontMetricsF hfm(headerFont);

    auto hx = pbbox_.getXMid() - hfm.horizontalAdvance(headerStr())/2.0;
    auto hy = py1 + hfm.ascent();

    drawHeaderTextLabel(Point(hx, hy), headerStr());

    py1 = py1 + hfm.height() + bm;
  }

  //---

  const auto *symbolSetMgr = charts()->symbolSetMgr();

  auto *symbolSet = symbolSetMgr->symbolSet(this->symbolSet());

  auto symbolFillColor   = plot()->interpPaletteColor(ColorInd());
  auto symbolStrokeColor = plot()->interpThemeColor(ColorInd(1.0));

  if (isContiguous()) {
    for (long i = mapMin(); i <= mapMax(); ++i) {
      // get symbol
      CQChartsSymbolSet::SymbolData symbolData;

      if (symbolSet)
        symbolData = symbolSet->symbolData(int(i));
      else
        symbolData.symbol = Symbol(CQChartsSymbolType(static_cast<CQChartsSymbolType::Type>(i)));

      if (! symbolData.symbol.isValid())
        symbolData.symbol = Symbol::circle();

      //---

      // set pen brush
      PenBrush symbolPenBrush;

      auto c = symbolStrokeColor;

      if (symbolData.symbol.isFilled())
        setPenBrush(symbolPenBrush, PenData(true, c), BrushData(true, symbolFillColor));
      else
        setPenBrush(symbolPenBrush, PenData(true, c), BrushData(false));

      CQChartsDrawUtil::setPenBrush(device, symbolPenBrush);

      //---

      // draw symbol
      double ss = std::max(fh/2.0 - 4.0, 3.0);

      auto py = CMathUtil::map(double(i), double(mapMin()), double(mapMax()),
                               py2 - fh/2.0, py1 + fh/2.0);

      auto p1 = device->pixelToWindow(Point(px + bw/2, py));

      CQChartsDrawUtil::drawSymbol(device, symbolPenBrush, symbolData.symbol, p1,
                                   Length::pixel(ss));

      //---

      // draw value
      auto dataStr = valueText(CMathUtil::map(double(i), double(mapMin()), double(mapMax()),
                                              double(dataMin()), double(dataMax())));

      drawTextLabel(Point(px + bw + bm, py + df), dataStr);

      //---

      BBox pbbox1(pbbox_.getXMin(), py - fh/2.0, pbbox_.getXMax(), py + fh/2.0);

      itemBoxes.push_back(ItemBox(symbolData.symbol, device->pixelToWindow(pbbox1)));
    }
  }
  else {
    int n = numUnique();

    for (int i = 0; i < n; ++i) {
      auto name = uniqueValues()[i].toString();

      //---

      // get symbol
      CQChartsSymbolSet::SymbolData symbolData;

      if (isMapped()) {
        if (! symbolTypeData_.typeMap.valueToSymbol(name, symbolData.symbol)) {
          if (symbolSet)
            symbolData = symbolSet->interpI(int(i + mapMin()), int(mapMin()), int(mapMax()));
          else
            symbolData.symbol =
              Symbol::interpOutlineWrap(int(i + mapMin()), int(mapMin()), int(mapMax()));
        }
      }
      else {
        symbolData.symbol = Symbol(name);
      }

      if (! symbolData.symbol.isValid())
        symbolData.symbol = Symbol::circle();

      //---

      auto hidden = ! plot()->symbolTypeVisible(symbolData.symbol);

      //---

      // set pen brush
      PenBrush symbolPenBrush;

      auto c = symbolStrokeColor;

      if (hidden)
        c = CQChartsUtil::blendColors(c, bg, 0.5);

      if (symbolData.symbol.isFilled())
        setPenBrush(symbolPenBrush, PenData(true, c), BrushData(true, symbolFillColor));
      else
        setPenBrush(symbolPenBrush, PenData(true, c), BrushData(false));

      CQChartsDrawUtil::setPenBrush(device, symbolPenBrush);

      //---

      // draw symbol
      double ss = std::max(fh/2.0 - 4.0, 3.0);

      auto py = CMathUtil::map(i, 0, n - 1, py2 - fh/2.0, py1 + fh/2.0);

      auto p1 = device->pixelToWindow(Point(px + bw/2, py));

      CQChartsDrawUtil::drawSymbol(device, symbolPenBrush, symbolData.symbol, p1,
                                   Length::pixel(ss));

      //---

      // draw unique name
      drawTextLabel(Point(px + bw + bm, py + df), name, hidden);

      //---

      BBox pbbox1(pbbox_.getXMin(), py - fh/2.0, pbbox_.getXMax(), py + fh/2.0);

      itemBoxes.push_back(ItemBox(symbolData.symbol, device->pixelToWindow(pbbox1)));
    }
  }

  //---

  th->typeBBox_[drawType] = th->bbox();
}

QSize
CQChartsSymbolTypeMapKey::
calcSize(const DrawData &drawData) const
{
  drawData_ = drawData;

  //---

  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  // outer margin
  double bm = this->padding();
  double bw = fm.horizontalAdvance("X") + 4;

  //---

  double hw = 0, hh = 0;

  if (headerStr().length()) {
    auto headerFont = calcDrawFont(headerTextFont());

    QFontMetricsF hfm(headerFont);

    hw = hfm.horizontalAdvance(headerStr());
    hh = hfm.height();
  }

  //---

  double tw = 0.0;

  kh_ = 0.0;

  if (isContiguous()) {
    kh_ = (fm.height() + 2)*double(mapMax() - mapMin() + 1) + 2*bm;

    for (long i = mapMin(); i <= mapMax(); ++i) {
      auto name = valueText(CMathUtil::map(double(i), double(mapMin()), double(mapMax()),
                                           double(dataMin()), double(dataMax())));

      tw = std::max(tw, fm.horizontalAdvance(name));
    }
  }
  else {
    int n = numUnique();

    if (n) {
      kh_ = (fm.height() + 2)*n + 2*bm;

      for (int i = 0; i < n; ++i) {
        auto name = uniqueValues()[i].toString();

        tw = std::max(tw, fm.horizontalAdvance(name));
      }
    }
    else {
      return QSize(1, 1);
    }
  }

  kw_ = std::max(bw + tw + 3*bm, hw + 2*bm);

  if (hh > 0)
    kh_ += hh + bm;

  return QSize(int(kw_), int(kh_));
}

void
CQChartsSymbolTypeMapKey::
invalidate()
{
  emit dataChanged();
}

QString
CQChartsSymbolTypeMapKey::
valueText(double value) const
{
  QString text;

  if (! isIntegral()) {
    if      (CMathUtil::realEq(value, 0.0))
      text = "0.0";
    else if (CMathUtil::realEq(value, 1.0))
      text = "1.0";
    else
      text.setNum(value);
  }
  else {
    int ivalue = int(value);

    text.setNum(ivalue);
  }

  return text;
}

bool
CQChartsSymbolTypeMapKey::
selectPress(const Point &p, SelMod selMod)
{
  return selectPressType(p, selMod, DrawType::VIEW);
}

bool
CQChartsSymbolTypeMapKey::
selectPressType(const Point &p, SelMod selMod, DrawType drawType)
{
  const auto &itemBoxes = itemBoxes_[drawType];

  auto ni = itemBoxes.size();

  std::vector<int> oldItemVisible;

  oldItemVisible.resize(ni);

  for (size_t i = 0; i < ni; ++i)
    oldItemVisible[i] = plot()->symbolTypeVisible(itemBoxes[i].symbol);

  auto newItemVisible = oldItemVisible;

  for (size_t i = 0; i < ni; ++i) {
    bool inside = itemBoxes[i].rect.inside(p);

    if      (selMod == CQChartsSelMod::ADD) {
      if (inside)
        newItemVisible[i] = true;
    }
    else if (selMod == CQChartsSelMod::REPLACE)
      newItemVisible[i] = inside;
    else if (selMod == CQChartsSelMod::REMOVE)
      newItemVisible[i] = ! inside;
    else if (selMod == CQChartsSelMod::TOGGLE) {
      if (inside)
        newItemVisible[i] = ! newItemVisible[i];
    }
  }

  for (size_t i = 0; i < ni; ++i)
    if (oldItemVisible[i] != newItemVisible[i])
      emit itemSelected(itemBoxes[i].symbol, newItemVisible[i]);

  return false;
}

//-----

CQChartsMapKeyWidget::
CQChartsMapKeyWidget(CQChartsMapKey *key) :
 key_(key)
{
  setObjectName("mapKey");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  scrollArea_ = CQUtil::makeWidget<QScrollArea>("scrollArea");

  layout->addWidget(scrollArea_);

  keyFrame_ = new CQChartsMapKeyFrame(this);

  scrollArea_->setWidget(keyFrame_);
}

void
CQChartsMapKeyWidget::
setKey(CQChartsMapKey *key)
{
  key_ = key;

  if (key_) {
    if (key_->plot())
      key_->plot()->updateMapKey(key_);

    //---

    if (keyFrame_->updateSize()) {
      QFontMetrics fm(font());

      auto s = keyFrame_->size();

      auto h = std::min(s.height() + 4, fm.height()*8);

      setFixedSize(QSize(s.width() + 20, h));
    }
  }

  update();

  keyFrame_->update();
}

//---

CQChartsMapKeyFrame::
CQChartsMapKeyFrame(CQChartsMapKeyWidget *w) :
 w_(w)
{
  setObjectName("keyFrame");

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

bool
CQChartsMapKeyFrame::
updateSize()
{
  auto *key = w_->key();
  if (! key) return false;

  if (! isVisible())
    return false;

  CQChartsMapKey::DrawData drawData;

  drawData.isWidget = true;
  drawData.font     = font();

  auto s  = this->size();
  auto ks = key->calcSize(drawData);

  if (ks.width() > 0 && ks.height() > 0 && (ks.width() != s.width() || ks.height() != s.height())) {
    setFixedSize(ks);
    return true;
  }

  return false;
}

void
CQChartsMapKeyFrame::
paintEvent(QPaintEvent *)
{
  auto *key = w_->key();
  if (! key) return;

  //---

  QPainter p(this);

  CQChartsPixelPaintDevice device(&p);

  CQChartsMapKey::DrawData drawData;

  drawData.isWidget = true;
  drawData.font     = font();

  key->draw(&device, drawData, CQChartsMapKey::DrawType::WIDGET);
}

void
CQChartsMapKeyFrame::
mousePressEvent(QMouseEvent *me)
{
  auto *key = w_->key();
  if (! key) return;

  //---

  if (me->button() == Qt::LeftButton) {
    auto p = me->pos();

    auto selMod = CQChartsUtil::modifiersToClickMod(me->modifiers());

    key->selectPressType(CQChartsGeom::Point(p), selMod, CQChartsMapKey::DrawType::WIDGET);
  }
}

void
CQChartsMapKeyFrame::
contextMenuEvent(QContextMenuEvent *e)
{
  auto *menu = new QMenu;

  CQUtil::addCheckedAction(menu, "Plot Key", w_->key()->isVisible(),
                           this, SLOT(showKeySlot(bool)));

  (void) menu->exec(e->globalPos());

  delete menu;
}

void
CQChartsMapKeyFrame::
showKeySlot(bool b)
{
  w_->key()->setVisible(b);

  w_->key()->plot()->drawObjs();
}
