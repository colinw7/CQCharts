#include <CQChartsMapKey.h>
#include <CQChartsPlot.h>
#include <CQChartsEditHandles.h>
#include <CQChartsSymbolSet.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColorsPalette.h>

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
setMargin(double r)
{
  margin_ = r;

  invalidate();
}

void
CQChartsMapKey::
setPosition(const Position &p)
{
  position_ = p;

  invalidate();
}

void
CQChartsMapKey::
setAlign(const Qt::Alignment &a)
{
  align_ = a;

  invalidate();
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

  if      (align() & Qt::AlignLeft ) x = editHandles()->bbox().getXMin();
  else if (align() & Qt::AlignRight) x = editHandles()->bbox().getXMax();

  if      (align() & Qt::AlignBottom) y = editHandles()->bbox().getYMin();
  else if (align() & Qt::AlignTop   ) y = editHandles()->bbox().getYMax();

  setPosition(CQChartsPosition(Point(x, y), CQChartsUnits::PLOT));

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

//---

CQChartsColorMapKey::
CQChartsColorMapKey(Plot *plot) :
 CQChartsMapKey(plot)
{
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

  addProp("position", "Key Position");

  addProp("paletteName", "Palette Name");

  //---

  CQChartsTextBoxObj::addProperties(model, path, desc);
}

void
CQChartsColorMapKey::
draw(PaintDevice *device, bool /*usePenBrush*/)
{
  auto font = calcFont(textFont());

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

  double kw = bw + tw + 3*bm;
  double kh = bh + fm.height() + 2*bm;

  // calc center
  Point pos;

  if (position().isValid()) {
    pos = positionToPixel(position());
  }
  else {
    auto pbbox = plot()->calcPlotPixelRect();

    double px = pbbox.getXMax() - kw/2.0 - bm;
    double py = pbbox.getYMax() - kh/2.0 - bm;

    pos = Point(px, py);
  }

  double xm = pos.x;
  double ym = pos.y;

  //---

  // calc bbox and align
  Point p1(xm - kw/2.0, ym - kh/2.0);
  Point p2(xm + kw/2.0, ym + kh/2.0);

  BBox pbbox(p1.x, p1.y, p2.x, p2.y);

  double dx = 0.0;
  double dy = 0.0;

  if      (align() & Qt::AlignLeft ) dx =  kw/2.0;
  else if (align() & Qt::AlignRight) dx = -kw/2.0;

  if      (align() & Qt::AlignBottom) dy = -kh/2.0;
  else if (align() & Qt::AlignTop   ) dy =  kh/2.0;

  pbbox = pbbox.translated(dx, dy);

  auto *th = const_cast<CQChartsColorMapKey *>(this);

  th->setBBox(device->pixelToWindow(pbbox));

  //---

  // draw box border and background
  CQChartsTextBoxObj::draw(device, bbox());

  //---

  // draw gradient
  BBox gbbox(pbbox.getXMin() + bm     , pbbox.getYMin() + bm + fm.height()/2.0,
             pbbox.getXMin() + bw + bm, pbbox.getYMax() - bm - fm.height()/2.0);

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
  setPainterFont(device, textFont());

  //---

  // draw labels
  auto drawTextLabel = [&](const Point &p, const QString &label) {
    auto p1 = pixelToWindow(p);

    CQChartsTextOptions textOptions;

    textOptions.align         = Qt::AlignLeft;
    textOptions.contrast      = isTextContrast();
    textOptions.contrastAlpha = textContrastAlpha();

    CQChartsDrawUtil::drawTextAtPoint(device, p1, label, textOptions);
  };

  double df = (fm.ascent() - fm.descent())/2.0;

  double tx  = gbbox.getXMax() + bm;
  double ty1 = gbbox.getYMin() + df;
  double ty2 = gbbox.getYMid() + df;
  double ty3 = gbbox.getYMax() + df;

  drawTextLabel(Point(tx, ty3), dataMinStr);
  drawTextLabel(Point(tx, ty2), dataMidStr);
  drawTextLabel(Point(tx, ty1), dataMaxStr);
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

//------

CQChartsSymbolSizeMapKey::
CQChartsSymbolSizeMapKey(Plot *plot) :
 CQChartsMapKey(plot)
{
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

  addProp("dataMin", "Model Data Min");
  addProp("dataMax", "Model Data Max");

  addProp("mapMin", "Symbol Size Min");
  addProp("mapMax", "Symbol Size Max");

  addProp("position", "Key Position");
  addProp("align"   , "Key Align");

  addProp("scale"  , "Scale Factor");
  addProp("stacked", "Stacked Vertical instead of overlaid");
  addProp("rows"   , "Number of symbol rows");

  addProp("textAlign"  , "Text Align");
  addProp("paletteName", "Palette Name");

  addProp("alpha", "Alpha");

  //---

  CQChartsTextBoxObj::addProperties(model, path, desc);
}

void
CQChartsSymbolSizeMapKey::
draw(PaintDevice *device, bool usePenBrush)
{
  initDraw();

  drawParts(device, usePenBrush);
}

void
CQChartsSymbolSizeMapKey::
initDraw()
{
  initCenter();

  calcSymbolBoxes();

  calcTextBBox();

  alignBoxes();
}

void
CQChartsSymbolSizeMapKey::
drawParts(PaintDevice *device, bool usePenBrush)
{
  drawBorder(device, usePenBrush);

  //---

  drawCircles(device, usePenBrush);

  //---

  // set text pen
  CQChartsPenBrush tpenBrush;

  auto tc = interpTextColor(ColorInd());

  setPenBrush(tpenBrush, PenData(true, tc, textAlpha()), BrushData(false));

  CQChartsDrawUtil::setPenBrush(device, tpenBrush);

  //---

  // set font
  setPainterFont(device, textFont());

  //---

  CQChartsTextOptions textOptions;

  textOptions.contrast      = isTextContrast();
  textOptions.contrastAlpha = textContrastAlpha();

  drawText(device, textOptions, usePenBrush);
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
drawCircles(PaintDevice *device, bool usePenBrush)
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

    device->drawEllipse(pixelToWindow(pbbox));
  };

  //---

  if (symbolBoxes_.empty())
    return;

  //---

  // draw ellipse for min, mean, max radii
  auto *palette = (paletteName_.isValid() ? paletteName_.palette() : nullptr);

  double y  = 1.0;
  double dy = (symbolBoxes_.size() > 1 ? 1.0/(symbolBoxes_.size() - 1) : 0.0);

  for (const auto &pbbox : symbolBoxes_) {
    auto fillColor = (palette ?
      palette->getColor(y, /*scale*/false, /*invert*/false) :
      plot()->interpPaletteColor(ColorInd(y)));

    drawEllipse(fillColor, pbbox);

    y -= dy;
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

  auto font = calcFont(textFont());

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

    auto p2 = pixelToWindow(p1);

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

void
CQChartsSymbolSizeMapKey::
invalidate()
{
  emit dataChanged();
}

void
CQChartsSymbolSizeMapKey::
initCenter() const
{
  // calc center
  if (position().isValid()) {
    pcenter_ = positionToPixel(position());
  }
  else {
    auto pbbox = plot()->calcPlotPixelRect();

    double prmin = scale()*this->mapMin();

    double px = pbbox.getXMax() - prmin;
    double py = pbbox.getYMax() - prmin;

    pcenter_ = Point(px, py);
  }

  center_ = pixelToWindow(pcenter_);
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

  double y  = 1.0;
  double sy = (rows() > 1 ? 1.0/(rows() - 1) : 0.0);

  // outer margin
  double pm = this->margin();

  //---

  double xm = pcenter_.x;
  double ym = pcenter_.y;

  if (! isStacked()) {
    double yb = ym + prmax;

    for (int i = 0; i < rows(); ++i) {
      double pr = CMathUtil::map(y, 0.0, 1.0, prmin, prmax);

      auto pbbox = BBox(xm - pr, yb - 2*pr, xm + pr, yb);

      symbolBoxes_.push_back(pbbox);

      y -= sy;
    }

    sbbox_ = pixelToWindow(BBox(xm - prmax - pm, ym - prmax - pm,
                                xm + prmax + pm, ym + prmax + pm));
  }
  else {
    double h = (rows() - 1)*pm;

    for (int i = 0; i < rows(); ++i) {
      h += 2*CMathUtil::map(y, 0.0, 1.0, prmin, prmax);

      y -= sy;
    }

    y = 1.0;

    double yt = ym - h/2;

    for (int i = 0; i < rows(); ++i) {
      double pr = CMathUtil::map(y, 0.0, 1.0, prmin, prmax);

      auto pbbox = BBox(xm - pr, yt + 2*pr, xm + pr, yt);

      symbolBoxes_.push_back(pbbox);

      yt += 2*pr + pm;

      y -= sy;
    }

    sbbox_ = pixelToWindow(BBox(xm - prmax - pm, ym - h/2 - pm,
                                xm + prmax + pm, ym + h/2 + pm));
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

  auto font = calcFont(textFont());

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

  tbbox_ = pixelToWindow(BBox(pxt1, pyt1, pxt2, pyt2));
}

void
CQChartsSymbolSizeMapKey::
alignBoxes() const
{
  bbox_ = sbbox_ + tbbox_;

  auto c = bbox_.getCenter();

  double dx = center_.x - c.x;
  double dy = center_.y - c.y;

  double w = bbox_.getWidth ();
  double h = bbox_.getHeight();

  if      (align() & Qt::AlignLeft ) dx += w/2.0;
  else if (align() & Qt::AlignRight) dx -= w/2.0;

  if      (align() & Qt::AlignBottom) dy += h/2.0;
  else if (align() & Qt::AlignTop   ) dy -= h/2.0;

  bbox_  =  bbox_.translated(dx, dy);
  sbbox_ = sbbox_.translated(dx, dy);
  tbbox_ = tbbox_.translated(dx, dy);

  double pdx = plot()->windowToSignedPixelWidth (dx);
  double pdy = plot()->windowToSignedPixelHeight(dy);

  for (auto &pbbox : symbolBoxes_)
    pbbox = pbbox.translated(pdx, pdy);

  auto *th = const_cast<CQChartsSymbolSizeMapKey *>(this);

  th->setBBox(bbox_);
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

//------

CQChartsSymbolTypeMapKey::
CQChartsSymbolTypeMapKey(Plot *plot) :
 CQChartsMapKey(plot)
{
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

  addProp("position", "Key Position");

  //---

  CQChartsTextBoxObj::addProperties(model, path, desc);
}

void
CQChartsSymbolTypeMapKey::
draw(PaintDevice *device, bool /*usePenBrush*/)
{
  auto ks = pixelSize();

  auto font = calcFont(textFont());

  QFontMetricsF fm(font);

  double bm = this->margin();
  double bw = fm.width("X") + 4;

  double kw = ks.width ();
  double kh = ks.height();

  // calc center
  Point pos;

  if (position().isValid()) {
    pos = positionToPixel(position());
  }
  else {
    auto pbbox = plot()->calcPlotPixelRect();

    double px = pbbox.getXMax() - kw/2.0 - bm;
    double py = pbbox.getYMax() - kh/2.0 - bm;

    pos = Point(px, py);
  }

  double xm = pos.x;
  double ym = pos.y;

  //---

  // calc bbox and align
  Point p1(xm - kw/2.0, ym - kh/2.0);
  Point p2(xm + kw/2.0, ym + kh/2.0);

  BBox pbbox(p1.x, p1.y, p2.x, p2.y);

  double dx = 0.0;
  double dy = 0.0;

  if      (align() & Qt::AlignLeft ) dx =  kw/2.0;
  else if (align() & Qt::AlignRight) dx = -kw/2.0;

  if      (align() & Qt::AlignBottom) dy = -kh/2.0;
  else if (align() & Qt::AlignTop   ) dy =  kh/2.0;

  pbbox = pbbox.translated(dx, dy);

  auto *th = const_cast<CQChartsSymbolTypeMapKey *>(this);

  th->setBBox(device->pixelToWindow(pbbox));

  //---

  // draw box border and background
  CQChartsTextBoxObj::draw(device, bbox());

  //---

  const auto *symbolSetMgr = charts()->symbolSetMgr();

  auto *symbolSet = symbolSetMgr->symbolSet(symbolSet_);

  // draw symbols and labels
  auto drawTextLabel = [&](const Point &p, const QString &label) {
    // set text pen
    CQChartsPenBrush tpenBrush;

    auto tc = interpTextColor(ColorInd());

    setPenBrush(tpenBrush, PenData(true, tc, textAlpha()), BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, tpenBrush);

    //---

    // set font
    setPainterFont(device, textFont());

    //---

    auto p1 = pixelToWindow(p);

    CQChartsTextOptions textOptions;

    textOptions.align         = Qt::AlignLeft;
    textOptions.contrast      = isTextContrast();
    textOptions.contrastAlpha = textContrastAlpha();

    CQChartsDrawUtil::drawTextAtPoint(device, p1, label, textOptions);
  };

  double df = (fm.ascent() - fm.descent())/2.0;

  auto symbolFillColor   = plot()->interpPaletteColor(ColorInd());
  auto symbolStrokeColor = plot()->interpThemeColor(ColorInd(1.0));

  for (int i = mapMin(); i <= mapMax(); ++i) {
    auto dataStr = valueText(CMathUtil::map(i, mapMin(), mapMax(), dataMin(), dataMax()));

    auto y = CMathUtil::map(i, mapMin(), mapMax(), pbbox.getYMax() - fm.height()/2.0 - 1,
                            pbbox.getYMin() + fm.height()/2.0 + 1);

    CQChartsSymbolSet::SymbolData symbolData;

    if (symbolSet)
      symbolData = symbolSet->symbol(i);
    else
      symbolData.symbol = CQChartsSymbol((CQChartsSymbol::Type) i);

    //---

    PenBrush symbolPenBrush;

    if (symbolData.filled)
      setPenBrush(symbolPenBrush, PenData(true, symbolStrokeColor),
                  BrushData(true, symbolFillColor));
    else
      setPenBrush(symbolPenBrush, PenData(true, symbolStrokeColor),
                  BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, symbolPenBrush);

    //---

    double ss = 8;

    CQChartsDrawUtil::drawSymbol(device, symbolPenBrush, symbolData.symbol,
                                 device->pixelToWindow(Point(pbbox.getXMin() + ss/2 + bm + 2, y)),
                                 CQChartsLength(ss, CQChartsUnits::PIXEL));

    //---

    drawTextLabel(Point(pbbox.getXMin() + bw + 2*bm, y + df), dataStr);
  }
}

QSize
CQChartsSymbolTypeMapKey::
pixelSize() const
{
  auto font = calcFont(textFont());

  QFontMetricsF fm(font);

  double bm = this->margin();
  double bw = fm.width("X") + 4;
  double kh = (fm.height() + 2)*(mapMax() - mapMin() + 1) + 2*bm;

  double tw = 0.0;

  for (int i = mapMin(); i <= mapMax(); ++i) {
    auto dataStr = valueText(CMathUtil::map(i, mapMin(), mapMax(), dataMin(), dataMax()));

    tw = std::max(tw, fm.width(dataStr));
  }

  double kw = bw + tw + 3*bm;

  return QSize(kw, kh);
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

  if      (CMathUtil::realEq(value, 0.0))
    text = "0.0";
  else if (CMathUtil::realEq(value, 1.0))
    text = "1.0";
  else
    text.setNum(value);

  return text;
}
