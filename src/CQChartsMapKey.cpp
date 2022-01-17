#include <CQChartsMapKey.h>
#include <CQChartsPlot.h>
#include <CQChartsEditHandles.h>
#include <CQChartsSymbolSet.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColorsPalette.h>

#include <QScrollArea>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPaintDevice>

CQChartsMapKey::
CQChartsMapKey(Plot *plot) :
 CQChartsTextBoxObj(plot)
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
      double bm = this->margin();

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
    align = 0;

    if      (location().onLeft   ()) {
      x = bbox.getXMin(); align |= (isInsideX() ? Qt::AlignLeft : Qt::AlignRight); }
    else if (location().onHCenter()) {
      x = bbox.getXMid(); align |=  Qt::AlignHCenter; }
    else if (location().onRight  ()) {
      x = bbox.getXMax(); align |= (isInsideX() ? Qt::AlignRight : Qt::AlignLeft); }

    if      (location().onTop    ()) {
      y = bbox.getYMax(); align |= (isInsideY() ? Qt::AlignTop  : Qt::AlignBottom); }
    else if (location().onVCenter()) {
      y = bbox.getYMid(); align |= Qt::AlignVCenter; }
    else if (location().onBottom ()) {
      y = bbox.getYMin(); align |= (isInsideY() ? Qt::AlignBottom : Qt::AlignTop); }

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

  addProp("location", "Location");
  addProp("insideX" , "Inside Plot X");
  addProp("insideY" , "Inside Plot Y");
  addProp("margin"  , "Margin");
  addProp("position", "Position");
  addProp("align"   , "Alignment");

  //---

  CQChartsTextBoxObj::addProperties(model, path, desc);
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

void
CQChartsColorMapKey::
draw(PaintDevice *device, const DrawData &drawData, DrawType drawType)
{
  auto &itemBoxes = itemBoxes_[drawType];

  itemBoxes.clear();

  //---

  drawData_ = drawData;

  if (isNumeric())
    drawContiguous(device);
  else
    drawDiscreet(device, drawType);
}

void
CQChartsColorMapKey::
drawContiguous(PaintDevice *device)
{
  (void) calcContiguousSize();

  //---

  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  double bm = this->margin();
  double bw = fm.width("X") + 16; // color box width

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
  CQChartsTextBoxObj::draw(device, bbox());

  //---

  // draw gradient
  BBox gbbox(pbbox_.getXMin() + bm     , pbbox_.getYMin() + bm + fm.height()/2.0,
             pbbox_.getXMin() + bw + bm, pbbox_.getYMax() - bm - fm.height()/2.0);

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
  (void) calcDiscreetSize();

  //---

  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  double bm = this->margin();
  double bs = fm.width("X") + 16; // color box size

  //---

  calcCenter();

  calcAlignedBBox();

  auto *th = const_cast<CQChartsColorMapKey *>(this);

  th->setBBox(device->pixelToWindow(pbbox_));

  //---

  // draw box border and background
  CQChartsTextBoxObj::draw(device, bbox());

  //---

  CQColorsPalette *colorsPalette = nullptr;

  if (paletteName().isValid())
    colorsPalette = paletteName().palette();
  else
    colorsPalette = view()->themePalette();

  double bx = pbbox_.getXMin() + bm;
  double by = pbbox_.getYMin() + bm;

  double bmi = 2;
  double bsi = bs - 2*bmi;

  int n = numUnique();

  //---

  auto bg = bgColor(device);

  // draw boxes
  auto &itemBoxes = itemBoxes_[drawType];

  double min = this->mapMin();
  double max = this->mapMax();

  for (int i = 0; i < n; ++i) {
    double r = CMathUtil::map(i, 0, n - 1, min, max);

    auto c = colorsPalette->getColor(r);

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

  double df = (fm.ascent() - fm.descent())/2.0;

  for (int i = 0; i < n; ++i) {
    double r = CMathUtil::map(i, 0, n - 1, min, max);

    auto c = colorsPalette->getColor(r);

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

    auto name = uniqueValues()[i].toString();

    drawTextLabel(Point(tx, ty + (i + 0.5)*bs + df), name);
  }
}

QSize
CQChartsColorMapKey::
calcSize(const DrawData &drawData) const
{
  drawData_ = drawData;

  if (isNumeric())
    return calcContiguousSize();
  else
    return calcDiscreetSize();
}

QSize
CQChartsColorMapKey::
calcContiguousSize() const
{
  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  double bm = this->margin();

  double bw = fm.width("X") + 16; // color box width
  double bh = fm.height()*5;      // color box height

  //---

  // calc text width
  auto dataMid = CMathUtil::avg(dataMin(), dataMax());

  auto dataMinStr = valueText(dataMin());
  auto dataMidStr = valueText(dataMid  );
  auto dataMaxStr = valueText(dataMax());

  double tw = std::max(std::max(fm.width(dataMinStr), fm.width(dataMidStr)), fm.width(dataMaxStr));

  //---

  kw_ = bw + tw + 3*bm;
  kh_ = bh + fm.height() + 2*bm;

  return QSize(kw_, kh_);
}

QSize
CQChartsColorMapKey::
calcDiscreetSize() const
{
  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  double bm = this->margin();

  double bs = fm.width("X") + 16; // color box size

  //---

  int n = numUnique();

  double tw = 0.0;

  for (int i = 0; i < n; ++i) {
    auto name = uniqueValues()[i].toString();

    tw = std::max(tw, fm.width(name));
  }

  //---

  kw_ = bs + tw + 3*bm;
  kh_ = n*bs + 2*bm;

  return QSize(kw_, kh_);
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

  int ni = itemBoxes.size();

  std::vector<int> oldItemVisible;

  oldItemVisible.resize(ni);

  for (int i = 0; i < ni; ++i)
    oldItemVisible[i] = plot()->colorVisible(itemBoxes[i].color);

  auto newItemVisible = oldItemVisible;

  for (int i = 0; i < ni; ++i) {
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

  for (int i = 0; i < ni; ++i)
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

void
CQChartsSymbolSizeMapKey::
draw(PaintDevice *device, const DrawData &drawData, DrawType drawType)
{
  auto &itemBoxes = itemBoxes_[drawType];

  itemBoxes.clear();

  //---

  drawData_ = drawData;

  initDraw(device);

  if (isNumeric())
    drawContiguous(device, drawType);
  else
    drawDiscreet(device, drawType);
}

void
CQChartsSymbolSizeMapKey::
initDraw(PaintDevice *device)
{
  calcCenter();

  pcenter_ = Point(xm_, ym_);
  center_  = device->pixelToWindow(pcenter_);

  (void) calcSize(drawData_);

  if (isNumeric())
    alignBoxes(device);
}

void
CQChartsSymbolSizeMapKey::
drawContiguous(PaintDevice *device, DrawType drawType)
{
  drawBorder(device, drawData_.usePenBrush);

  //---

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
  (void) calcDiscreetSize();

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

  double bm = this->margin();
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

  double tx1 = pbbox_.getXMin() + bm;
  double tx2 = tx1 + twl_ + bm;
  double ty  = pbbox_.getYMin() + bm;

  double df = (fm.ascent() - fm.descent())/2.0;

  int n = numUnique();

  for (int i = 0; i < n; ++i) {
    double r = CMathUtil::map(i, 0, n - 1, min, max);

    auto rstr = QString("%1").arg(r, 0, 'f', ndp_);
    auto name = uniqueValues()[i].toString();

    auto l = Length::pixel(r);

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

    auto dxl = twl_ - fm.width(rstr);

    auto ty1 = ty + (i + 0.5)*bh;

    drawTextLabel(Point(tx1 + dxl, ty1 + df), rstr);
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
  CQChartsTextBoxObj::draw(device, bbox());
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
  double dy = (symbolBoxes_.size() > 1 ? (ymax - ymin)/(symbolBoxes_.size() - 1) : 0.0);

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

    double tw = fm.width(text);

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
  double dy = (symbolBoxes_.size() > 1 ? 1.0/(symbolBoxes_.size() - 1) : 0.0);

  // outer margin
  double pm = this->margin();

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

  if (isNumeric())
    return calcContiguousSize();
  else
    return calcDiscreetSize();
}

QSize
CQChartsSymbolSizeMapKey::
calcContiguousSize() const
{
  calcSymbolBoxes();
  calcTextBBox   ();

  pbbox_ = psbbox_ + ptbbox_;

  return QSize(pbbox_.getWidth(), pbbox_.getHeight());
}

QSize
CQChartsSymbolSizeMapKey::
calcDiscreetSize() const
{
  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  double bm = this->margin();
  double bh = fm.height() + 1; // row height

  //---

  auto calcNdp = [](const QString &s) {
    auto i = s.indexOf('.');
    if (i == -1) return 0;
    auto ndp = s.size() - i - 1;
    while (s[ndp + i] == '0')
      --ndp;
    return ndp;
  };

  int n = numUnique();

  double min = this->mapMin();
  double max = this->mapMax();

  ndp_ = 0;

  for (int i = 0; i < n; ++i) {
    double r = CMathUtil::map(i, 0, n - 1, min, max);

    auto rstr = QString("%1").arg(r, 0, 'f', 6);

    ndp_ = std::max(ndp_, calcNdp(rstr));
  }

  twl_ = 0.0;
  twr_ = 0.0;

  for (int i = 0; i < n; ++i) {
    double r = CMathUtil::map(i, 0, n - 1, min, max);

    auto rstr = QString("%1").arg(r, 0, 'f', ndp_);
    auto name = uniqueValues()[i].toString();

    twl_ = std::max(twl_, fm.width(rstr));
    twr_ = std::max(twr_, fm.width(name));
  }

  //---

  kw_ = twl_ + twr_ + 3*bm;
  kh_ = n*bh + 2*bm;

  pbbox_ = BBox(0, 0, kw_, kh_);

  return QSize(kw_, kh_);
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
  double pm = this->margin();

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
  double pm = this->margin();

  double pxt1 = (! isStacked() ? pbbox1.getXMid() : pbbox1.getXMax() + pm);
  double pyt1 = (! isStacked() ? pbbox1.getYMid() : pbbox1.getYMin()     );
  double pxt2 = pxt1;
  double pyt2 = pyt1;

  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  double fh = fm.height();
  double fa = fm.ascent();

  double y  = 1.0;
  double dy = (symbolBoxes_.size() > 1 ? 1.0/(symbolBoxes_.size() - 1) : 0.0);

  for (const auto &pbbox : symbolBoxes_) {
    double r = CMathUtil::map(y, 0.0, 1.0, min, max);

    auto text = valueText(r);

    double tw = fm.width(text);

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
  sbbox_ = device->pixelToWindow(psbbox_);
  tbbox_ = device->pixelToWindow(ptbbox_);
  bbox_  = device->pixelToWindow(pbbox_);

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

  bbox_  =  bbox_.translated(dx, dy);
  sbbox_ = sbbox_.translated(dx, dy);
  tbbox_ = tbbox_.translated(dx, dy);

  double pdx = device->windowToSignedPixelWidth (dx);
  double pdy = device->windowToSignedPixelHeight(dy);

  for (auto &pbbox : symbolBoxes_)
    pbbox = pbbox.translated(pdx, pdy);

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

  int ni = itemBoxes.size();

  std::vector<int> oldItemVisible;

  oldItemVisible.resize(ni);

  for (int i = 0; i < ni; ++i)
    oldItemVisible[i] = plot()->symbolSizeVisible(itemBoxes[i].size);

  auto newItemVisible = oldItemVisible;

  for (int i = 0; i < ni; ++i) {
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

  for (int i = 0; i < ni; ++i)
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

  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  double bm = this->margin();
  double bw = fm.width("X") + 4;

  //---

  calcCenter();

  calcAlignedBBox();

  auto *th = const_cast<CQChartsSymbolTypeMapKey *>(this);

  th->setBBox(device->pixelToWindow(pbbox_));

  //---

  // draw box border and background
  CQChartsTextBoxObj::draw(device, bbox());

  //---

  auto bg = bgColor(device);

  const auto *symbolSetMgr = charts()->symbolSetMgr();

  auto *symbolSet = symbolSetMgr->symbolSet(symbolSet_);

  // draw symbols and labels
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

  double fh = fm.height();
  double df = (fm.ascent() - fm.descent())/2.0;

  auto symbolFillColor   = plot()->interpPaletteColor(ColorInd());
  auto symbolStrokeColor = plot()->interpThemeColor(ColorInd(1.0));

  if (isNumeric()) {
    for (int i = mapMin(); i <= mapMax(); ++i) {
      // get symbol
      CQChartsSymbolSet::SymbolData symbolData;

      if (symbolSet)
        symbolData = symbolSet->symbolData(i);
      else
        symbolData.symbol = Symbol(CQChartsSymbolType((CQChartsSymbolType::Type) i));

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

      auto py = CMathUtil::map(i, mapMin(), mapMax(), pbbox_.getYMax() - fh/2.0 - 1,
                               pbbox_.getYMin() + fh/2.0 + 1);

      auto p1 = device->pixelToWindow(Point(pbbox_.getXMin() + ss/2 + bm + 2, py));

      CQChartsDrawUtil::drawSymbol(device, symbolPenBrush, symbolData.symbol, p1,
                                   CQChartsLength::pixel(ss));

      //---

      // draw value
      auto dataStr = valueText(CMathUtil::map(i, mapMin(), mapMax(), dataMin(), dataMax()));

      drawTextLabel(Point(pbbox_.getXMin() + bw + 2*bm, py + df), dataStr);

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
        if (symbolSet)
          symbolData = symbolSet->interpI(i + mapMin(), mapMin(), mapMax());
        else
          symbolData.symbol = Symbol::interpOutlineWrap(i + mapMin(), mapMin(), mapMax());
      }
      else
        symbolData.symbol = Symbol(name);

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

      auto py = CMathUtil::map(i, 0, n - 1, pbbox_.getYMax() - fh/2.0 - 1,
                               pbbox_.getYMin() + fh/2.0 + 1);

      auto p1 = device->pixelToWindow(Point(pbbox_.getXMin() + ss/2 + bm + 2, py));

      CQChartsDrawUtil::drawSymbol(device, symbolPenBrush, symbolData.symbol, p1,
                                   CQChartsLength::pixel(ss));

      //---

      // draw unique name
      drawTextLabel(Point(pbbox_.getXMin() + bw + 2*bm, py + df), name, hidden);

      //---

      BBox pbbox1(pbbox_.getXMin(), py - fh/2.0, pbbox_.getXMax(), py + fh/2.0);

      itemBoxes.push_back(ItemBox(symbolData.symbol, device->pixelToWindow(pbbox1)));
    }
  }
}

QSize
CQChartsSymbolTypeMapKey::
calcSize(const DrawData &drawData) const
{
  drawData_ = drawData;

  auto font = calcDrawFont(textFont());

  QFontMetricsF fm(font);

  double bm = this->margin();
  double bw = fm.width("X") + 4;

  double tw = 0.0;

  kh_ = 0.0;

  if      (isNumeric()) {
    kh_ = (fm.height() + 2)*(mapMax() - mapMin() + 1) + 2*bm;

    for (int i = mapMin(); i <= mapMax(); ++i) {
      auto name = valueText(CMathUtil::map(i, mapMin(), mapMax(), dataMin(), dataMax()));

      tw = std::max(tw, fm.width(name));
    }
  }
  else {
    int n = numUnique();

    if (n) {
      kh_ = (fm.height() + 2)*n + 2*bm;

      for (int i = 0; i < n; ++i) {
        auto name = uniqueValues()[i].toString();

        tw = std::max(tw, fm.width(name));
      }
    }
    else {
      return QSize(1, 1);
    }
  }

  kw_ = bw + tw + 3*bm;

  return QSize(kw_, kh_);
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

  int ni = itemBoxes.size();

  std::vector<int> oldItemVisible;

  oldItemVisible.resize(ni);

  for (int i = 0; i < ni; ++i)
    oldItemVisible[i] = plot()->symbolTypeVisible(itemBoxes[i].symbol);

  auto newItemVisible = oldItemVisible;

  for (int i = 0; i < ni; ++i) {
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

  for (int i = 0; i < ni; ++i)
    if (oldItemVisible[i] != newItemVisible[i])
      emit itemSelected(itemBoxes[i].symbol, newItemVisible[i]);

  return false;
}

//-----

#include <CQChartsViewPlotPaintDevice.h>

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
mousePressEvent(QMouseEvent *e)
{
  auto *key = w_->key();
  if (! key) return;

  //---

  auto p = e->pos();

  auto selMod = CQChartsUtil::modifiersToClickMod(e->modifiers());

  key->selectPressType(CQChartsGeom::Point(p), selMod, CQChartsMapKey::DrawType::WIDGET);
}
