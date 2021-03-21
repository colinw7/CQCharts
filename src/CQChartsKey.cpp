#include <CQChartsKey.h>
#include <CQChartsPlot.h>
#include <CQChartsCompositePlot.h>
#include <CQChartsAxis.h>
#include <CQChartsView.h>
#include <CQChartsEditHandles.h>
#include <CQChartsModelDetails.h>
#include <CQChartsVariant.h>
#include <CQColorsPalette.h>
#include <CQChartsUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsSymbolSet.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColors.h>

#include <QScrollBar>
#include <QStylePainter>
#include <QStyleOptionSlider>

CQChartsKey::
CQChartsKey(View *view) :
 CQChartsBoxObj(view),
 CQChartsObjTextData      <CQChartsKey>(this),
 CQChartsObjHeaderTextData<CQChartsKey>(this)
{
  init();

  setStateColoring(false);
}

CQChartsKey::
CQChartsKey(Plot *plot) :
 CQChartsBoxObj(plot),
 CQChartsObjTextData      <CQChartsKey>(this),
 CQChartsObjHeaderTextData<CQChartsKey>(this)
{
  init();

  setFilled(true);
  setFillAlpha(Alpha(0.5));
}

void
CQChartsKey::
init()
{
  setObjectName("key");

  setEditable(true);

  setHeaderTextAlign(Qt::AlignHCenter | Qt::AlignVCenter);

  //---

  Font font;

  font.decFontSize(4);

  setTextFont(font);
}

CQChartsKey::
~CQChartsKey()
{
}

QString
CQChartsKey::
calcId() const
{
  if      (plot())
    return plot()->id() + "/key";
  else if (view())
    return view()->id() + "/key";
  else
    return "key";
}

void
CQChartsKey::
setSelected(bool b)
{
  CQChartsUtil::testAndSet(selected_, b, [&]() {
    if      (plot()) {
      plot()->drawObjs();
    }
    else if (view()) {
      view()->invalidateObjects();
      view()->invalidateOverlay();

      view()->doUpdate();
    }
  } );
}

//---

void
CQChartsKey::
setOrientation(const Qt::Orientation &orientation)
{
  CQChartsUtil::testAndSet(orientation_, orientation, [&]() { updateLayout(); } );
}

//---

void
CQChartsKey::
setAutoHide(bool b)
{
  CQChartsUtil::testAndSet(autoHide_, b, [&]() { redraw(); } );
}

void
CQChartsKey::
setClipped(bool b)
{
  CQChartsUtil::testAndSet(clipped_, b, [&]() { redraw(); } );
}

void
CQChartsKey::
setAbove(bool b)
{
  CQChartsUtil::testAndSet(above_, b, [&]() { updateLayout(); } );
}

void
CQChartsKey::
setLocation(const CQChartsKeyLocation &l)
{
  CQChartsUtil::testAndSet(location_, l, [&]() { updatePosition(); } );
}

void
CQChartsKey::
setHeaderStr(const QString &s)
{
  CQChartsUtil::testAndSet(header_, s, [&]() { updateLayout(); } );
}

void
CQChartsKey::
setHiddenAlpha(const Alpha &a)
{
  CQChartsUtil::testAndSet(hiddenAlpha_, a, [&]() { redraw(); } );
}

void
CQChartsKey::
setColumns(int i)
{
  CQChartsUtil::testAndSet(columns_, i, [&]() { updateLayout(); } );
}

void
CQChartsKey::
setMaxRows(int i)
{
  CQChartsUtil::testAndSet(maxRows_, i, [&]() { updateLayout(); } );
}

void
CQChartsKey::
setInteractive(bool b)
{
  CQChartsUtil::testAndSet(interactive_, b, [&]() { } );
}

void
CQChartsKey::
setPressBehavior(const KeyBehavior &v)
{
  CQChartsUtil::testAndSet(pressBehavior_, v, [&]() { } );
}

//---

void
CQChartsKey::
nextRowCol(int &row, int &col) const
{
  if (! isHorizontal()) {
    // across columns and then next row
    ++col;

    if (col >= columns()) {
      col = 0;

      ++row;
    }
  }
  else {
    // across rows and then next column
    ++row;

    if (row >= columns()) {
      row = 0;

      ++col;
    }
  }
}

//------

CQChartsViewKey::
CQChartsViewKey(View *view) :
 CQChartsKey(view)
{
}

CQChartsViewKey::
~CQChartsViewKey()
{
}

void
CQChartsViewKey::
updatePosition(bool /*queued*/)
{
  redraw();
}

void
CQChartsViewKey::
updateLayout()
{
  redraw();
}

void
CQChartsViewKey::
doLayout()
{
  if (location() == Location::Type::ABSOLUTE_POSITION)
    return;

  //----

  auto font = view()->viewFont(textFont());

  QFontMetricsF fm(font);

  double bs = fm.height() + 4.0;

  //----

  double x = 0.0, y = 0.0;
//double dx = 0.0, dy = 0.0;

  if      (location().onLeft   ()) { x =   0.0; /*dx =  1.0; */ }
  else if (location().onHCenter()) { x =  50.0; /*dx =  0.0; */ }
  else if (location().onRight  ()) { x = 100.0; /*dx = -1.0; */ }

  if      (location().onTop    ()) { y = 100.0; /*dy =  1.0; */ }
  else if (location().onVCenter()) { y =  50.0; /*dy =  0.0; */ }
  else if (location().onBottom ()) { y =   0.0; /*dy = -1.0; */ }

  //----

  auto p = view()->windowToPixel(Point(x, y));

  //p.x += dx*bs;
  //p.y += dy*bs;

  //---

  // get external margin
  double xlm = view()->lengthViewWidth (margin().left  ());
  double xrm = view()->lengthViewWidth (margin().right ());
  double ytm = view()->lengthViewHeight(margin().top   ());
  double ybm = view()->lengthViewHeight(margin().bottom());

  // get internal padding
//double xlp = view()->lengthViewWidth (padding().left  ());
//double xrp = view()->lengthViewWidth (padding().right ());
//double ytp = view()->lengthViewHeight(padding().top   ());
//double ybp = view()->lengthViewHeight(padding().bottom());

  //---

  numPlots_ = view()->numPlots();

  if (numPlots_ > 0) {
    double pw = 0.0;
    double ph = 0.0;

    for (int i = 0; i < numPlots_; ++i) {
      auto *plot = view()->plot(i);

      auto name = plot->keyText();

      double tw = fm.width(name) + bs + xlm + xrm;

      pw = std::max(pw, tw);

      ph += bs;
    }

    size_ = Size(pw + xlm + xrm, ph + ybm + ytm + (numPlots_ - 1)*2);

    //---

    double pxr = 0.0, pyr = 0.0;

    if      (location().onLeft   ()) pxr = p.x                   + xlm;
    else if (location().onHCenter()) pxr = p.x - size_.width()/2;
    else if (location().onRight  ()) pxr = p.x - size_.width()   - xrm;

    if      (location().onTop    ()) pyr = p.y                    + ytm;
    else if (location().onVCenter()) pyr = p.y - size_.height()/2;
    else if (location().onBottom ()) pyr = p.y - size_.height()   - ybm;

    pposition_ = Point(pxr, pyr);
  }
  else {
    size_      = Size(0.0, 0.0);
    pposition_ = Point(0.0, 0.0);
  }

  wposition_ = view()->pixelToWindow(pposition_);
}

void
CQChartsViewKey::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &name, const QString &desc, bool hidden=false) {
    auto *item = model->addProperty(path, this, name);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

  addProp("visible"    , "Is visible");
  addProp("selected"   , "Is selected");
  addProp("editable"   , "Is editable");
  addProp("orientation", "Layout direction");
  addProp("autoHide"   , "Auto hide key when too large");
  addProp("clipped"    , "Clip key to view");

  addProp("location", "Key placement location");

  addProp("interactive"  , "Key supports click", true);
  addProp("pressBehavior", "Key click behavior", true);

  addProp("hiddenAlpha", "Alpha for hidden items");

  addProp("columns", "Number of item columns");

  //---

  // header text
  auto headerPath     = path + "/header";
  auto headerTextPath = headerPath + "/text";

  auto addHeaderProp = [&](const QString &name, const QString &alias, const QString &desc) {
    return &(model->addProperty(headerTextPath, this, name, alias)->
              setDesc("Key header text " + desc));
  };

  auto addHeaderStyleProp = [&](const QString &name, const QString &alias, const QString &desc) {
    auto *item = addHeaderProp(name, alias, desc);
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
  addHeaderStyleProp("headerTextAlign"     , "align"     , "header text align");
  addHeaderStyleProp("headerTextFormatted" , "formatted" , "header text formatted to fit box");
  addHeaderStyleProp("headerTextScaled"    , "scaled"    , "header text scaled to box");
  addHeaderStyleProp("headerTextHtml"      , "html"      , "header text is html");
  addHeaderStyleProp("headerTextClipLength", "clipLength", "header text clipped to length");
  addHeaderStyleProp("headerTextClipElide" , "clipElide" , "header text clip elide");

  //---

  // stroke, fill
  addBoxProperties(model, path, "");

  //---

  // key text
  auto textPath = path + "/text";

  auto addTextProp = [&](const QString &name, const QString &alias, const QString &desc) {
    return &(model->addProperty(textPath, this, name, alias)->setDesc("Key text " + desc));
  };

  auto addTextStyleProp = [&](const QString &name, const QString &alias, const QString &desc) {
    auto *item = addTextProp(name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  addTextStyleProp("textColor"     , "color"     , "text color");
  addTextStyleProp("textAlpha"     , "alpha"     , "text alpha");
  addTextStyleProp("textFont"      , "font"      , "text font");
//addTextStyleProp("textAngle"     , "angle"     , "text angle");
  addTextStyleProp("textContrast"  , "contrast"  , "text contrast");
  addTextStyleProp("textAlign"     , "align"     , "text align");
  addTextStyleProp("textFormatted" , "formatted" , "text formatted to fit box");
  addTextStyleProp("textScaled"    , "scaled"    , "text scaled to box");
  addTextStyleProp("textHtml"      , "html"      , "text is html");
  addTextStyleProp("textClipLength", "clipLength", "text clipped to length");
  addTextStyleProp("textClipElide" , "clipElide" , "text clipp elide");
}

bool
CQChartsViewKey::
contains(const Point &p) const
{
  return pbbox_.inside(p);
}

void
CQChartsViewKey::
draw(PaintDevice *device) const
{
  if (! isVisible())
    return;

  //---

  auto *th = const_cast<CQChartsViewKey *>(this);

  th->doLayout();

  if (numPlots_ <= 0)
    return;

  //---

  // pixel position & size (TODO: using view/units)
  double px = pposition_.x; // left
  double py = pposition_.y; // top

  double pw = size_.width ();
  double ph = size_.height();

  wbbox_ = BBox(px, py - ph, px + pw, py);

  //---

  auto p1 = view()->pixelToWindow(Point(px     , py     ));
  auto p2 = view()->pixelToWindow(Point(px + pw, py + ph));

  pbbox_ = BBox(p1.x, p2.y, p2.x, p1.y);

  //---

  BBox bbox(px, py, px + pw, py + ph);

  CQChartsBoxObj::draw(device, bbox);

  //---

  // get external margin
  double xlm = view()->lengthViewWidth (margin().left  ());
  double xrm = view()->lengthViewWidth (margin().right ());
  double ytm = view()->lengthViewHeight(margin().top   ());
  double ybm = view()->lengthViewHeight(margin().bottom());

  // get internal padding
//double xlp = view()->lengthViewWidth (padding().left  ());
//double xrp = view()->lengthViewWidth (padding().right ());
//double ytp = view()->lengthViewHeight(padding().top   ());
//double ybp = view()->lengthViewHeight(padding().bottom());

  //---

  view()->setPainterFont(device, textFont());

  QFontMetricsF fm(device->font());

  double px1 = px + xlm;
  double py1 = py + ybm;

  double bs = fm.height() + 4.0;

  //double dth = (bs - fm.height())/2;

  prects_.clear();

  for (int i = 0; i < numPlots_; ++i) {
    double py2 = py1 + bs + 2;

    auto *plot = view()->plot(i);

    bool checked = plot->isVisible();

    //---

    drawCheckBox(device, px1, (py1 + py2)/2.0 - bs/2.0, int(bs), checked);

    //---

    device->setPen(interpTextColor(ColorInd()));

    auto name = plot->keyText();

    double px2 = px1 + bs + xrm;

    //double tw = fm.width(name);

    BBox rect1(px2, py1, px2 + pw - bs - ybm - ytm, py2);

    CQChartsTextOptions textOptions;

    textOptions.align         = textAlign();
    textOptions.contrast      = isTextContrast();
    textOptions.contrastAlpha = textContrastAlpha();
    textOptions.formatted     = isTextFormatted();
    textOptions.scaled        = isTextScaled();
    textOptions.html          = isTextHtml();
    textOptions.clipLength    = lengthPixelWidth(textClipLength());
    textOptions.clipElide     = textClipElide();

    CQChartsDrawUtil::drawTextInBox(device, view()->pixelToWindow(rect1), name, textOptions);

    //---

    // save view key item (plot) rect
    auto p1 = view()->pixelToWindow(Point(px     , py1));
    auto p2 = view()->pixelToWindow(Point(px + pw, py2));

    BBox prect(p1.x, p2.y, p2.x, p1.y);

    prects_.push_back(prect);

    //---

    py1 = py2;
  }
}

void
CQChartsViewKey::
drawEditHandles(QPainter *painter) const
{
  assert(view()->mode() == CQChartsView::Mode::EDIT || isSelected());

  setEditHandlesBBox();

  editHandles()->draw(painter);
}

void
CQChartsViewKey::setEditHandlesBBox() const
{
  editHandles()->setMode(EditHandles::Mode::MOVE);

  editHandles()->setBBox(pbbox_);
}

void
CQChartsViewKey::
drawCheckBox(PaintDevice *device, double x, double y, int bs, bool checked) const
{
  auto cimage = CQChartsUtil::initImage(QSize(bs, bs));

  cimage.fill(QColor(0, 0, 0, 0));

  BBox bbox(0, 0, bs, bs);

  QStylePainter spainter(&cimage, view());

  spainter.setPen(interpTextColor(ColorInd()));

  QStyleOptionButton opt;

  opt.initFrom(view());

  opt.rect = bbox.qrect().toRect();

  opt.state |= (checked ? QStyle::State_On : QStyle::State_Off);

  spainter.drawControl(QStyle::CE_CheckBox, opt);

  device->drawImage(Point(x, y), cimage);
}

//------

bool
CQChartsViewKey::
selectPress(const Point &w, SelMod selMod)
{
  int n = std::min(view()->numPlots(), int(prects_.size()));

  for (int i = 0; i < n; ++i) {
    if (! prects_[i].inside(w))
      continue;

    if      (pressBehavior() == KeyBehavior::Type::SHOW)
      doShow(i, selMod);
    else if (pressBehavior() == KeyBehavior::Type::SELECT)
      doSelect(i, selMod);

    break;
  }

  redraw();

  return true;
}

//------

bool
CQChartsViewKey::
editPress(const Point &w)
{
  editHandles()->setDragPos(w);

  return true;
}

bool
CQChartsViewKey::
editMove(const Point &w)
{
  const auto &dragPos = editHandles()->dragPos();

  double dx = w.x - dragPos.x;
  double dy = w.y - dragPos.y;

  location_ = Location::Type::ABSOLUTE_POSITION;

  wposition_ = wposition_ + Point(dx, dy);
  pposition_ = view()->windowToPixel(wposition_);

  editHandles()->setDragPos(w);

  updatePosition(/*queued*/false);

  return true;
}

bool
CQChartsViewKey::
editMotion(const Point &w)
{
  return editHandles()->selectInside(w);
}

//------

void
CQChartsViewKey::
doShow(int i, SelMod selMod)
{
  auto *plot = view()->plot(i);

  if      (selMod == SelMod::REPLACE) {
    View::Plots plots;

    view()->getPlots(plots);

    for (auto &plot1 : plots)
      plot1->setVisible(plot1 == plot);
  }
  else if (selMod == SelMod::ADD)
    plot->setVisible(true);
  else if (selMod == SelMod::REMOVE)
    plot->setVisible(false);
  else if (selMod == SelMod::TOGGLE)
    plot->setVisible(! plot->isVisible());
}

void
CQChartsViewKey::
doSelect(int, SelMod)
{
}

void
CQChartsViewKey::
redraw(bool /*queued*/)
{
  view()->invalidateObjects();
  view()->invalidateOverlay();

  view()->doUpdate();
}

//------

CQChartsPlotKey::
CQChartsPlotKey(Plot *plot) :
 CQChartsKey(plot)
{
  autoHide_ = false;

  //---

  setStroked(true);

  clearItems();

  //---

  // create scroll bar
  scrollData_.hbar = new QScrollBar(Qt::Horizontal, plot->view());
  scrollData_.hbar->setObjectName("keyHBar");

  scrollData_.vbar = new QScrollBar(Qt::Vertical  , plot->view());
  scrollData_.vbar->setObjectName("keyVBar");

  scrollData_.hbar->hide();
  scrollData_.vbar->hide();

  connect(scrollData_.hbar, SIGNAL(valueChanged(int)), this, SLOT(hscrollSlot(int)));
  connect(scrollData_.vbar, SIGNAL(valueChanged(int)), this, SLOT(vscrollSlot(int)));

  scrollData_.pixelBarSize = view()->style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 2;
}

CQChartsPlotKey::
~CQChartsPlotKey()
{
  delete scrollData_.hbar;
  delete scrollData_.vbar;

  for (auto &item : items_)
    delete item;
}

//---

CQChartsPlot *
CQChartsPlotKey::
drawPlot() const
{
  if (plot()->isComposite())
    return dynamic_cast<CQChartsCompositePlot *>(plot())->currentPlot();

  return plot();
}

bool
CQChartsPlotKey::
isEmpty() const
{
  return items_.empty();
}

bool
CQChartsPlotKey::
isVisibleAndNonEmpty() const
{
  return isVisible() && ! isEmpty();
}

bool
CQChartsPlotKey::
isOverlayVisible() const
{
  if (plot()->isOverlay()) {
    const auto *key = plot()->getFirstPlotKey();

    return (key == this);
  }

  return isVisibleAndNonEmpty();
}

//---

void
CQChartsPlotKey::
hscrollSlot(int v)
{
  scrollData_.hpos = v;

  redraw(/*queued*/ false);
}

void
CQChartsPlotKey::
vscrollSlot(int v)
{
  scrollData_.vpos = v;

  redraw(/*queued*/ false);
}

void
CQChartsPlotKey::
redraw(bool queued)
{
  if (queued) {
    plot()->drawBackground();
    plot()->drawForeground();
  }
  else {
    plot()->invalidateLayer(CQChartsBuffer::Type::BACKGROUND);
    plot()->invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
  }
}

void
CQChartsPlotKey::
updateKeyItems()
{
  plot()->resetKeyItems();

  redraw();
}

void
CQChartsPlotKey::
updateLayout()
{
  invalidateLayout(/*reset*/true);

  redraw();
}

void
CQChartsPlotKey::
updatePosition(bool queued)
{
  plot()->updateKeyPosition();

  redraw(queued);
}

void
CQChartsPlotKey::
updatePositionAndLayout(bool queued)
{
  invalidateLayout();

  plot()->updateKeyPosition();

  redraw(queued);
}

void
CQChartsPlotKey::
updatePlotLocation()
{
  auto bbox = plot()->calcDataRange();

  if (! bbox.isSet())
    return;

  updateLocation(bbox);
}

void
CQChartsPlotKey::
updateLocation(const BBox &bbox)
{
  auto *drawPlot = this->drawPlot();

  // get external margin
  double xlm = drawPlot->lengthPlotWidth (margin().left  ());
  double xrm = drawPlot->lengthPlotWidth (margin().right ());
  double ytm = drawPlot->lengthPlotHeight(margin().top   ());
  double ybm = drawPlot->lengthPlotHeight(margin().bottom());

  // get internal padding
  double xlp = drawPlot->lengthPlotWidth (padding().left  ());
  double xrp = drawPlot->lengthPlotWidth (padding().right ());
  double ytp = drawPlot->lengthPlotHeight(padding().top   ());
  double ybp = drawPlot->lengthPlotHeight(padding().bottom());

  //---

  auto *xAxis = drawPlot->xAxis();
  auto *yAxis = drawPlot->yAxis();

  // get key contents size
  auto ks = calcSize();

  // calc key contents position
  // (bbox is plot bbox)
  double kx { 0.0 }, ky { 0.0 };

  if (location().isAuto()) {
    auto fitBBox = drawPlot->findEmptyBBox(ks.width(), ks.height());

    if (fitBBox.isSet()) {
      kx = fitBBox.getXMid() - ks.width ()/2;
      ky = fitBBox.getYMid() + ks.height()/2;

      location_.setType(CQChartsKeyLocation::Type::ABSOLUTE_POSITION);

      setAbsolutePlotPosition(Point(kx, ky));
    }
    else
      location_.setType(CQChartsKeyLocation::Type::TOP_RIGHT);
  }

  if      (location().onLeft()) {
    if (isInsideX())
      kx = bbox.getXMin() + xlm + xlp;
    else {
      kx = bbox.getXMin() - ks.width() - xrm - xrp;

      // offset by left y axis width
      if (yAxis)
        kx -= plot_->calcGroupedYAxisRange(CQChartsAxisSide::Type::BOTTOM_LEFT).
                getOptWidth() + xlm;
    }
  }
  else if (location().onHCenter()) {
    kx = bbox.getXMid() - ks.width()/2;
  }
  else if (location().onRight()) {
    if (isInsideX())
      kx = bbox.getXMax() - ks.width() - xrm - xrp;
    else {
      kx = bbox.getXMax() + xrm + xrp;

      // offset by right y axis width
      if (yAxis)
        kx += plot_->calcGroupedYAxisRange(CQChartsAxisSide::Type::TOP_RIGHT).
                getOptWidth() + xrm;
    }
  }

  if      (location().onTop()) {
    if (isInsideY())
      ky = bbox.getYMax() - ytm - ytp;
    else {
      ky = bbox.getYMax() + ks.height() + ytm + ytp;

      // offset by top x axis height
      if (xAxis)
        ky += plot_->calcGroupedXAxisRange(CQChartsAxisSide::Type::TOP_RIGHT).
                getOptHeight() + ytm;
    }
  }
  else if (location().onVCenter()) {
    ky = bbox.getYMid() + ks.height()/2;
  }
  else if (location().onBottom()) {
    if (isInsideY())
      ky = bbox.getYMin() + ks.height() + ybm + ybp;
    else {
      ky = bbox.getYMin() - ybm - ybp;

      // offset by bottom x axis height
      if (xAxis)
        ky -= plot_->calcGroupedXAxisRange(CQChartsAxisSide::Type::BOTTOM_LEFT).
                getOptHeight() + ybm;
    }
  }

  Point kp(kx, ky);

  //---

  // update location for absolute position/rectangle
  auto locationType = this->location().type();

  if      (locationType == CQChartsKeyLocation::Type::ABSOLUTE_POSITION) {
    kp = absolutePlotPosition();
  }
  else if (locationType == CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE) {
    auto bbox = absolutePlotRectangle();

    if (bbox.isValid())
      kp = Point(bbox.getUL());
  }

  //---

  setPosition(kp);
}

void
CQChartsPlotKey::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &name, const QString &desc) {
    return &(model->addProperty(path, this, name)->setDesc(desc));
  };

  auto addStyleProp = [&](const QString &name, const QString &desc) {
    auto *item = addProp(name, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  addProp("visible"    , "Is visible");
  addProp("selected"   , "Is selected");
  addProp("editable"   , "Is editable");
  addProp("orientation", "Layout direction");
  addProp("flipped"    , "Draw name value flipped");
  addProp("autoHide"   , "Auto hide key when too large");
  addProp("clipped"    , "Clip key to plot");

  addProp("above"   , "Draw key above plot");
  addProp("insideX" , "Key placed inside plot in x direction");
  addProp("insideY" , "Key placed inside plot in y direction");
  addProp("location", "Key placement location");

  addProp("absolutePosition" , "Key placement absolute position in view coordinates");
  addProp("absoluteRectangle", "Key placement absolute rectangle in view coordinates");

  addProp("interactive"  , "Key supports click");
  addProp("pressBehavior", "Key click behavior");

  addStyleProp("hiddenAlpha", "Alpha for hidden items");

  addProp("columns", "Number of item columns");
  addProp("maxRows", "Max rows for key");
  addProp("spacing", "Spacing between rows in pixels");

  //---

  auto scrollPath = path + "/scroll";

  auto addScrollProp = [&](const QString &name, const QString &alias, const QString &desc) {
    model->addProperty(scrollPath, this, name, alias)->setDesc(desc);
  };

  addScrollProp("scrollWidth" , "width" , "Key has fixed width and will scroll when larger");
  addScrollProp("scrollHeight", "height", "Key has fixed height and will scroll when larger");

  //---

  // header text
  auto headerPath     = path + "/header";
  auto headerTextPath = headerPath + "/text";

  auto addHeaderProp = [&](const QString &name, const QString &alias, const QString &desc) {
    return &(model->addProperty(headerTextPath, this, name, alias)->
              setDesc("Key header text " + desc));
  };

  auto addHeaderStyleProp = [&](const QString &name, const QString &alias, const QString &desc) {
    auto *item = addHeaderProp(name, alias, desc);
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
  addHeaderStyleProp("headerTextAlign"     , "align"     , "header text align");
  addHeaderStyleProp("headerTextFormatted" , "formatted" , "header text formatted to fit box");
  addHeaderStyleProp("headerTextScaled"    , "scaled"    , "header text scaled to box");
  addHeaderStyleProp("headerTextHtml"      , "html"      , "header text is html");
  addHeaderStyleProp("headerTextClipLength", "clipLength", "header text clipped to length");
  addHeaderStyleProp("headerTextClipElide" , "clipElide" , "header text clip elide");

  //---

  // stroke, fill
  addBoxProperties(model, path, "");

  //---

  // key text
  auto textPath = path + "/text";

  auto addTextProp = [&](const QString &name, const QString &alias, const QString &desc) {
    return &(model->addProperty(textPath, this, name, alias)->setDesc("Key text " + desc));
  };

  auto addTextStyleProp = [&](const QString &name, const QString &alias, const QString &desc) {
    auto *item = addTextProp(name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  addTextStyleProp("textColor"     , "color"     , "text color");
  addTextStyleProp("textAlpha"     , "alpha"     , "text alpha");
  addTextStyleProp("textFont"      , "font"      , "text font");
//addTextStyleProp("textAngle"     , "angle"     , "text angle");
  addTextStyleProp("textContrast"  , "contrast"  , "text contrast");
  addTextStyleProp("textAlign"     , "align"     , "text align");
  addTextStyleProp("textFormatted" , "formatted" , "text formatted to fit box");
  addTextStyleProp("textScaled"    , "scaled"    , "text scaled to box");
  addTextStyleProp("textHtml"      , "html"      , "text is html");
  addTextStyleProp("textClipLength", "clipLength", "text clipped to length");
  addTextStyleProp("textClipElide" , "clipElide" , "text clipp elide");
}

void
CQChartsPlotKey::
invalidateLayout(bool reset)
{
  needsLayout_ = true;

  if (reset)
    plot()->resetKeyItems();
}

void
CQChartsPlotKey::
clearItems()
{
  for (auto &item : items_)
    delete item;

  items_.clear();

  invalidateLayout();

  maxRow_ = 0;
  maxCol_ = 0;
}

void
CQChartsPlotKey::
addItem(KeyItem *item, int row, int col, int nrows, int ncols)
{
  item->setKey(this);

  item->setRow(row);
  item->setCol(col);

  item->setRowSpan(nrows);
  item->setColSpan(ncols);

  items_.push_back(item);

  invalidateLayout();

  maxRow_ = std::max(maxRow_, row + nrows);
  maxCol_ = std::max(maxCol_, col + ncols);
}

void
CQChartsPlotKey::
doLayout()
{
  if (! needsLayout_)
    return;

  needsLayout_ = false;

  //---

  // get items in each cell and dimension of grid
  using ColItems    = std::map<int, KeyItems>;
  using RowColItems = std::map<int, ColItems>;

  RowColItems rowColItems;

  numRows_ = 0;
  numCols_ = 0;

  for (const auto &item : items_) {
    numRows_ = std::max(numRows_, item->row() + item->rowSpan());
    numCols_ = std::max(numCols_, item->col() + item->colSpan());
  }

  for (const auto &item : items_) {
    int col = item->col();

    if (isFlipped())
      col = numCols_ - 1 - col;

    rowColItems[item->row()][col].push_back(item);
  }

  //---

  // get max number of rows
  int numRows = calcNumRows();

  //---

  // get size of each cell
  rowColCell_.clear();

  for (int r = 0; r < numRows; ++r) {
    for (int c = 0; c < numCols_; ++c) {
      const auto &items = rowColItems[r][c];

      for (const auto &item : items) {
        auto size = item->size();

        double width  = size.width ()/item->colSpan();
        double height = size.height()/item->rowSpan();

        rowColCell_[r][c].width  = std::max(rowColCell_[r][c].width , width );
        rowColCell_[r][c].height = std::max(rowColCell_[r][c].height, height);
      }
    }
  }

  //---

  auto *drawPlot = this->drawPlot();

  // get spacing, margin and padding in plot coords
  xs_ = drawPlot->pixelToWindowWidth (spacing());
  ys_ = drawPlot->pixelToWindowHeight(spacing());

  pmargin_.xl = drawPlot->lengthPlotWidth (margin().left  ());
  pmargin_.xr = drawPlot->lengthPlotWidth (margin().right ());
  pmargin_.yt = drawPlot->lengthPlotHeight(margin().top   ());
  pmargin_.yb = drawPlot->lengthPlotHeight(margin().bottom());

  ppadding_.xl = drawPlot->lengthPlotWidth (padding().left  ());
  ppadding_.xr = drawPlot->lengthPlotWidth (padding().right ());
  ppadding_.yt = drawPlot->lengthPlotHeight(padding().top   ());
  ppadding_.yb = drawPlot->lengthPlotHeight(padding().bottom());

  //---

  // get size of each row and column
  rowHeights_.clear();
  colWidths_ .clear();

  for (int r = 0; r < numRows; ++r) {
    for (int c = 0; c < numCols_; ++c) {
      rowHeights_[r] = std::max(rowHeights_[r], rowColCell_[r][c].height);
      colWidths_ [c] = std::max(colWidths_ [c], rowColCell_[r][c].width );
    }
  }

  //----

  // get header text size
  layoutData_.headerWidth  = 0;
  layoutData_.headerHeight = 0;

  if (headerStr().length()) {
    // set text options
    CQChartsTextOptions textOptions;

    textOptions.html = isHeaderTextHtml();

    // get font
    auto font = view()->plotFont(plot(), headerTextFont());

    // get text size
    auto tsize = CQChartsDrawUtil::calcTextSize(headerStr(), font, textOptions);

    layoutData_.headerWidth  = drawPlot->pixelToWindowWidth (tsize.width ()) + 2*xs_;
    layoutData_.headerHeight = drawPlot->pixelToWindowHeight(tsize.height()) + 2*ys_;
  }

  //---

  // update cell positions and sizes
  double y = -(pmargin_.yb + ppadding_.yb);

  y -= layoutData_.headerHeight;

  for (int r = 0; r < numRows; ++r) {
    double x = pmargin_.xl + ppadding_.xl;

    double rh = rowHeights_[r] + 2*ys_;

    for (int c = 0; c < numCols_; ++c) {
      double cw = colWidths_[c] + 2*xs_;

      auto &cell = rowColCell_[r][c];

      cell.x      = x;
      cell.y      = y;
      cell.width  = cw;
      cell.height = rh;

      x += cell.width;
    }

    y -= rh; // T->B
  }

  //----

  // calc full size (unscrolled)
  double w = 0, h = 0;

  for (int c = 0; c < numCols_; ++c) {
    auto &cell = rowColCell_[0][c];

    w += cell.width;
  }

  w += pmargin_.xl + ppadding_.xl + pmargin_.xr + ppadding_.xr;

  for (int r = 0; r < numRows; ++r) {
    auto &cell = rowColCell_[r][0];

    h += cell.height;
  }

  h += pmargin_.yb + ppadding_.yb + pmargin_.yt + ppadding_.yt + layoutData_.headerHeight;

  w = std::max(w, layoutData_.headerWidth);

  layoutData_.fullSize = Size(w, h);

  //---

  double vbw = drawPlot->pixelToWindowWidth (scrollData_.pixelBarSize);
  double hbh = drawPlot->pixelToWindowHeight(scrollData_.pixelBarSize);

  //---

  // check if key size exceeds plot pixel size (auto hide if needed)
  auto plotPixelRect = drawPlot->calcPlotPixelRect();

  double maxPixelWidth  = plotPixelRect.getWidth ()*0.8;
  double maxPixelHeight = plotPixelRect.getHeight()*0.8;

  auto pixelWidth  = drawPlot->windowToPixelWidth (layoutData_.fullSize.width ());
  auto pixelHeight = drawPlot->windowToPixelHeight(layoutData_.fullSize.height());

  layoutData_.pixelWidthExceeded  = (pixelWidth  > maxPixelWidth );
  layoutData_.pixelHeightExceeded = (pixelHeight > maxPixelHeight);

  //---

  // calc if horizontally, vertically scrolled and scroll area sizes
  layoutData_.hscrolled       = false;
  layoutData_.scrollAreaWidth = w;

  if      (scrollData_.width.isSet()) {
    double sw = drawPlot->lengthPlotWidth(scrollData_.width.length());

    layoutData_.hscrolled       = (w > sw);
    layoutData_.scrollAreaWidth = sw;
  }
  else if (layoutData_.pixelWidthExceeded) {
    layoutData_.hscrolled       = true;
    layoutData_.scrollAreaWidth = drawPlot->pixelToWindowWidth(maxPixelWidth);
  }

  layoutData_.vscrolled        = false;
  layoutData_.scrollAreaHeight = h - layoutData_.headerHeight;

  if      (scrollData_.height.isSet()) {
    double sh = drawPlot->lengthPlotHeight(scrollData_.height.length());

    layoutData_.vscrolled        = (h > sh + layoutData_.headerHeight);
    layoutData_.scrollAreaHeight = sh;
  }
  else if (layoutData_.pixelHeightExceeded) {
    layoutData_.vscrolled        = true;
    layoutData_.scrollAreaHeight = drawPlot->pixelToWindowHeight(maxPixelHeight) -
                                   layoutData_.headerHeight;
  }

  //---

  // adjust size for displayed scroll bar sizes
  layoutData_.vbarWidth  = 0.0;
  layoutData_.hbarHeight = 0.0;

  if (layoutData_.vscrolled)
    layoutData_.vbarWidth = vbw;

  if (layoutData_.hscrolled)
    layoutData_.hbarHeight = hbh;

  //---

  w = layoutData_.scrollAreaWidth  + layoutData_.vbarWidth;
  h = layoutData_.scrollAreaHeight + layoutData_.headerHeight + layoutData_.hbarHeight;

  //---

  layoutData_.size = Size(w, h);
}

CQChartsGeom::Point
CQChartsPlotKey::
absolutePlotPosition() const
{
  auto *drawPlot = this->drawPlot();

  return drawPlot->viewToWindow(absolutePosition());
}

void
CQChartsPlotKey::
setAbsolutePlotPosition(const Point &p)
{
  auto *drawPlot = this->drawPlot();

  setAbsolutePosition(drawPlot->windowToView(p));
}

CQChartsGeom::BBox
CQChartsPlotKey::
absolutePlotRectangle() const
{
  auto bbox = absoluteRectangle();

  if (! bbox.isValid())
    return bbox;

  auto *drawPlot = this->drawPlot();

  return drawPlot->viewToWindow(bbox);
}

void
CQChartsPlotKey::
setAbsolutePlotRectangle(const BBox &bbox)
{
  auto *drawPlot = this->drawPlot();

  setAbsoluteRectangle(drawPlot->windowToView(bbox));
}

int
CQChartsPlotKey::
calcNumRows() const
{
  // get max number of rows
  int numRows = 0;

  for (const auto &item : items_)
    numRows = std::max(numRows, item->row() + item->rowSpan());

#if 0
  // limit rows if height (and this scrolled) not defined
  if (! scrollData_.height.isSet())
    numRows = std::min(numRows, maxRows());
#endif

  return numRows;
}

CQChartsGeom::Size
CQChartsPlotKey::
calcSize()
{
  doLayout();

  return layoutData_.size;
}

bool
CQChartsPlotKey::
contains(const Point &p) const
{
  if (! isOverlayVisible() || isEmpty())
    return false;

  return bbox().inside(p);
}

void
CQChartsPlotKey::
boxObjInvalidate()
{
  redraw();
}

CQChartsKeyItem *
CQChartsPlotKey::
getItemAt(const Point &p) const
{
  if (! isOverlayVisible())
    return nullptr;

  for (auto &item : items_) {
    if (item->bbox().inside(p))
      return item;
  }

  return nullptr;
}

//------

bool
CQChartsPlotKey::
selectMove(const Point &w)
{
  bool changed = false;

  if (contains(w)) {
    auto *item = getItemAt(w);

    bool handled = false;

    if (item) {
      changed = setInsideItem(item);

      handled = item->selectMove(w);
    }

    if (changed)
      redraw();

    if (handled)
      return true;
  }

  changed = setInsideItem(nullptr);

  if (changed)
    redraw();

  return false;
}

//------

bool
CQChartsPlotKey::
editPress(const Point &p)
{
  editHandles()->setDragPos(p);

  auto locationType = this->location().type();

  if (locationType != CQChartsKeyLocation::Type::ABSOLUTE_POSITION &&
      locationType != CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE) {
    location_ = CQChartsKeyLocation::Type::ABSOLUTE_POSITION;

    setAbsolutePlotPosition(position());
  }

  return true;
}

bool
CQChartsPlotKey::
editMove(const Point &p)
{
  const auto &dragPos  = editHandles()->dragPos();
  const auto &dragSide = editHandles()->dragSide();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  if (dragSide == CQChartsResizeSide::MOVE) {
    location_ = CQChartsKeyLocation::Type::ABSOLUTE_POSITION;

    setAbsolutePlotPosition(absolutePlotPosition() + Point(dx, dy));
  }
  else {
    location_ = CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE;

    editHandles()->updateBBox(dx, dy);

    wbbox_ = editHandles()->bbox();

    setAbsolutePlotRectangle(wbbox_);

    CQChartsLength width (wbbox_.getWidth ()                           , CQChartsUnits::PLOT);
    CQChartsLength height(wbbox_.getHeight() - layoutData_.headerHeight, CQChartsUnits::PLOT);

    setScrollWidth (CQChartsOptLength(width ));
    setScrollHeight(CQChartsOptLength(height));

    invalidateLayout();
  }

  editHandles()->setDragPos(p);

  updatePosition(/*queued*/false);

  return true;
}

bool
CQChartsPlotKey::
editMotion(const Point &p)
{
  return editHandles()->selectInside(p);
}

void
CQChartsPlotKey::
editMoveBy(const Point &f)
{
  auto locationType = this->location().type();

  if (locationType != CQChartsKeyLocation::Type::ABSOLUTE_POSITION &&
      locationType != CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE) {
    location_ = CQChartsKeyLocation::Type::ABSOLUTE_POSITION;

    setAbsolutePlotPosition(position() + f);
  }

  updatePosition();
}

//------

bool
CQChartsPlotKey::
tipText(const Point &p, QString &tip) const
{
  bool rc = false;

  auto *item = getItemAt(p);

  if (item) {
    QString tip1;

    if (item->tipText(p, tip1)) {
      if (! tip.length())
        tip += "\n";

      tip += tip1;

      rc = true;
    }
  }

  return rc;
}

//------

bool
CQChartsPlotKey::
setInsideItem(CQChartsKeyItem *item)
{
  bool changed = false;

  for (auto &item1 : items_) {
    if (! item1) continue;

    if (item1 == item) {
      if (! item1->isInside()) {
        item1->setInside(true);

        changed = true;
      }
    }
    else {
      if (item1->isInside()) {
        item1->setInside(false);

        changed = true;
      }
    }
  }

  return changed;
}

void
CQChartsPlotKey::
setFlipped(bool b)
{
  if (b == flipped_)
    return;

  flipped_ = b;

  updateLayout();
}

//------

void
CQChartsPlotKey::
draw(CQChartsPaintDevice *device) const
{
  if (! plot()->isVisible() || ! isOverlayVisible() || isEmpty()) {
    scrollData_.hbar->hide();
    scrollData_.vbar->hide();
    return;
  }

  if (location().isAuto())
    return;

  //---

  auto *th = const_cast<CQChartsPlotKey *>(this);

  th->doLayout();

  //---

  auto locationType = this->location().type();

  //---

  // calc plot bounding box (full size)
  double x = position().x; // left
  double y = position().y; // top
  double w = layoutData_.size.width ();
  double h = layoutData_.size.height();

  if (locationType == CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE) {
    auto bbox = absolutePlotRectangle();

    if (bbox.isValid()) {
      w = bbox.getWidth ();
      h = bbox.getHeight();
    }
  }

  //---

  // calc key drawing area (not including scrollbars)
  double sw = layoutData_.scrollAreaWidth;
  double sh = layoutData_.scrollAreaHeight;

  BBox sbbox(x, y - sh, x + sw, y);

  //---

  // set displayed bbox
  if (locationType != CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE) {
    wbbox_ = BBox(x, y - h, x + w, y);
  }
  else {
    auto bbox = absolutePlotRectangle();

    if (bbox.isValid())
      wbbox_ = bbox;
  }

  //---

  auto *drawPlot = this->drawPlot();

  // calc pixel bounding box
  auto p1 = drawPlot->windowToPixel(Point(x     + pmargin_.xl, y     - pmargin_.yt));
  auto p2 = drawPlot->windowToPixel(Point(x + w - pmargin_.xl, y - h + pmargin_.yb));

  BBox pixelRect(p1, p2);

  //---

  // check if key size exceeds plot pixel size (auto hide if needed)
  if (isAutoHide()) {
    if (layoutData_.pixelWidthExceeded || layoutData_.pixelHeightExceeded) {
      scrollData_.hbar->hide();
      scrollData_.vbar->hide();
      return;
    }
  }

  // set clip rect to plot pixel rect
  auto plotPixelRect = drawPlot->calcPlotPixelRect();

  bool clipped  = false;
  auto clipRect = plotPixelRect;

  //---

  // get plot data rect
  auto dataPixelRect = drawPlot->calcDataPixelRect();

  auto dataRect = dataPixelRect;

  //---

  // get max number of rows
  int numRows = calcNumRows();

  //---

  auto *painter = dynamic_cast<CQChartsPlotPaintDevice *>(device)->painter();

  CQChartsPlotPaintDevice device1(drawPlot, painter);

  device1.save();

  //---

  // place scroll bar
  sx_ = 0.0;
  sy_ = 0.0;

  auto phh = drawPlot->windowToPixelHeight(layoutData_.headerHeight);

  double vspw = 0.0;
  double hsph = 0.0;

  if (layoutData_.vscrolled)
    vspw = scrollData_.pixelBarSize;

  if (layoutData_.hscrolled)
    hsph = scrollData_.pixelBarSize;

  if (layoutData_.vscrolled) {
    scrollData_.vbar->show();

    scrollData_.vbar->move(int(p2.x - scrollData_.pixelBarSize - 1), int(p1.y + phh));
    scrollData_.vbar->resize(int(scrollData_.pixelBarSize - 2), int(p2.y - p1.y - phh - hsph - 1));

    //---

    // count number of rows in height
    int    scrollRows   = 0;
    double scrollHeight = sh - pmargin_.yb - ppadding_.yb -
                               pmargin_.yt - ppadding_.yt - layoutData_.hbarHeight;

    for (int i = 0; i < numRows; ++i) {
      if (scrollHeight <= 0)
        break;

      ++scrollRows;

      int rh = int(rowHeights_[i] + 2*ys_);

      scrollHeight -= rh;
    }

    //---

    // update scroll bar
    if (scrollData_.vbar->pageStep() != scrollRows)
      scrollData_.vbar->setPageStep(scrollRows);

    int smax = std::max(numRows - scrollRows, 1);

    if (scrollData_.vbar->maximum() != smax)
      scrollData_.vbar->setRange(0, smax);

    if (scrollData_.vbar->value() != scrollData_.vpos)
      scrollData_.vbar->setValue(scrollData_.vpos);

    for (int i = 0; i < scrollData_.vpos; ++i)
      sy_ += rowHeights_[i] + 2*ys_;
  }
  else {
    if (scrollData_.vbar)
      scrollData_.vbar->hide();
  }

  //---

  if (layoutData_.hscrolled) {
    scrollData_.hbar->show();

    scrollData_.hbar->move(int(p1.x + 1), int(p2.y - scrollData_.pixelBarSize - 1));
    scrollData_.hbar->resize(int(p2.x - p1.x - vspw), int(scrollData_.pixelBarSize - 2));

    //---

    // update scroll bar
    auto pageStep  = (int) drawPlot->windowToPixelWidth(sw - layoutData_.vbarWidth);
    auto fullWidth = (int) drawPlot->windowToPixelWidth(layoutData_.fullSize.width());

    if (scrollData_.hbar->pageStep() != pageStep)
      scrollData_.hbar->setPageStep(pageStep);

    int smax = std::max(fullWidth - pageStep, 1);

    if (scrollData_.hbar->maximum() != smax)
      scrollData_.hbar->setRange(0, smax);

    if (scrollData_.hbar->value() != scrollData_.hpos)
      scrollData_.hbar->setValue(scrollData_.hpos);

    sx_ = drawPlot->pixelToWindowWidth(scrollData_.hpos);
  }
  else {
    if (scrollData_.hbar)
      scrollData_.hbar->hide();
  }

  //---

  if (layoutData_.vscrolled || layoutData_.hscrolled) {
    clipped  = true;
    clipRect = BBox(p1.x, p1.y + phh, p2.x - vspw, p2.y - hsph);
  }
  else {
    if      (locationType != CQChartsKeyLocation::Type::ABSOLUTE_POSITION &&
             locationType != CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE) {
      if (isInsideX()) {
        clipRect.setXMin(dataRect.getXMin());
        clipRect.setXMax(dataRect.getXMax());
      }

      if (isInsideY()) {
        clipRect.setYMin(dataRect.getYMin());
        clipRect.setYMax(dataRect.getYMax());
      }
    }
    else if (locationType == CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE) {
      auto bbox = absolutePlotRectangle();

      if (bbox.isValid()) {
        clipped  = true;
        clipRect = drawPlot->windowToPixel(bbox);
      }
    }

    clipped = isClipped();
  }

  //---

  // draw box (background)
  CQChartsBoxObj::draw(&device1, drawPlot->pixelToWindow(pixelRect));

  //---

  if (clipped) {
    auto cr = drawPlot->pixelToWindow(clipRect);

    device1.setClipRect(cr);
  }

  //---

  // draw header
  if (headerStr().length()) {
    // set text options
    CQChartsTextOptions textOptions;

    textOptions.align         = headerTextAlign();
    textOptions.contrast      = isHeaderTextContrast();
    textOptions.contrastAlpha = headerTextContrastAlpha();
    textOptions.formatted     = isHeaderTextFormatted();
    textOptions.scaled        = isHeaderTextScaled();
    textOptions.html          = isHeaderTextHtml();
    textOptions.clipLength    = lengthPixelWidth(headerTextClipLength());
    textOptions.clipElide     = headerTextClipElide();

    textOptions = plot()->adjustTextOptions(textOptions);

    //---

    // get font
    auto font = view()->plotFont(plot(), headerTextFont());

    device1.setFont(font);

    //---

    // get key top left, width (pixels), margins
    auto p = drawPlot->windowToPixel(Point(x, y)); // top left

    auto pw = drawPlot->windowToPixelWidth(sw);

    //---

    // get external margin
  //double xlm = drawPlot->lengthPlotWidth (margin().left  ());
  //double xrm = drawPlot->lengthPlotWidth (margin().right ());
    double ytm = drawPlot->lengthPlotHeight(margin().top   ());
    double ybm = drawPlot->lengthPlotHeight(margin().bottom());

    // get internal padding
  //double xlp = drawPlot->lengthPlotWidth (padding().left  ());
  //double xrp = drawPlot->lengthPlotWidth (padding().right ());
  //double ytp = drawPlot->lengthPlotHeight(padding().top   ());
  //double ybp = drawPlot->lengthPlotHeight(padding().bottom());

    //---

    // calc text rect
    auto tsize = CQChartsDrawUtil::calcTextSize(headerStr(), font, textOptions);

    double tw = pw;
    double th = tsize.height() + ybm + ytm;

    BBox trect(p.x, p.y, p.x + tw, p.y + th);

    //---

    // set text pen
    CQChartsPenBrush tPenBrush;

    auto tc = interpHeaderTextColor(ColorInd());

    plot()->setPen(tPenBrush, PenData(true, tc, headerTextAlpha()));

    plot()->updateObjPenBrushState(this, tPenBrush);

    //---

    // draw text
    device1.setPen(tPenBrush.pen);

    CQChartsDrawUtil::drawTextInBox(&device1, drawPlot->pixelToWindow(trect),
                                    headerStr(), textOptions);
  }

  //---

  // draw items
  for (const auto &item : items_) {
    int col = item->col();

    if (isFlipped())
      col = numCols_ - 1 - col;

    auto &cell = rowColCell_[item->row()][col];

    double x1 = cell.x - sx_;
    double y1 = cell.y + sy_;
    double w1 = cell.width;
    double h1 = cell.height;

    for (int c = 1; c < item->colSpan(); ++c) {
      auto &cell1 = rowColCell_[item->row()][col + c];

      w1 += cell1.width;
    }

    for (int r = 1; r < item->rowSpan(); ++r) {
      auto &cell1 = rowColCell_[item->row() + r][col];

      h1 += cell1.height;
    }

    BBox bbox(x1 + x      + xs_/2.0, y1 + y - h1 + ys_/2.0,
              x1 + x + w1 - xs_/2.0, y1 + y      - ys_/2.0);

    item->setBBox(bbox);

    if (wbbox_.overlaps(bbox)) {
      item->draw(&device1, bbox);

      if (plot()->showBoxes())
        drawPlot->drawWindowColorBox(&device1, bbox);
    }
  }

  //---

  // draw box
  if (plot()->showBoxes()) {
    drawPlot->drawWindowColorBox(&device1, wbbox_);

    BBox headerBox(x, y - layoutData_.headerHeight, x + sw, y);

    drawPlot->drawWindowColorBox(&device1, headerBox);
  }

  //---

  device1.restore();
}

void
CQChartsPlotKey::
drawEditHandles(QPainter *painter) const
{
  assert(plot()->view()->mode() == CQChartsView::Mode::EDIT || isSelected());

  setEditHandlesBBox();

  editHandles()->draw(painter);
}

void
CQChartsPlotKey::
setEditHandlesBBox() const
{
  if (isEditResize())
    editHandles()->setMode(EditHandles::Mode::RESIZE);
  else
    editHandles()->setMode(EditHandles::Mode::MOVE);

  editHandles()->setBBox(this->bbox());
}

bool
CQChartsPlotKey::
isEditResize() const
{
  return(scrollData_.height.isSet() || scrollData_.width.isSet() ||
         layoutData_.vscrolled || layoutData_.hscrolled);
}

QColor
CQChartsPlotKey::
interpBgColor() const
{
  if (isFilled())
    return interpFillColor(ColorInd());

  auto locationType = this->location().type();

  if (locationType != CQChartsKeyLocation::Type::ABSOLUTE_POSITION &&
      locationType != CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE) {
    if      (isInsideX() && isInsideY()) {
      if (plot()->isDataFilled())
        return plot()->interpDataFillColor(ColorInd());
    }
    else if (isInsideX()) {
      if (locationType == CQChartsKeyLocation::Type::CENTER_LEFT ||
          locationType == CQChartsKeyLocation::Type::CENTER_CENTER ||
          locationType == CQChartsKeyLocation::Type::CENTER_RIGHT) {
        if (plot()->isDataFilled())
          return plot()->interpDataFillColor(ColorInd());
      }
    }
    else if (isInsideY()) {
      if (locationType == CQChartsKeyLocation::Type::TOP_CENTER ||
          locationType == CQChartsKeyLocation::Type::CENTER_CENTER ||
          locationType == CQChartsKeyLocation::Type::BOTTOM_CENTER) {
        if (plot()->isDataFilled())
          return plot()->interpDataFillColor(ColorInd());
      }
    }
  }

  if (plot()->isPlotFilled())
    return plot()->interpPlotFillColor(ColorInd());

  return plot()->interpThemeColor(ColorInd());
}

//------

CQChartsColumnKey::
CQChartsColumnKey(CQChartsPlot *plot) :
 CQChartsPlotKey(plot)
{
}

void
CQChartsColumnKey::
setColumn(const CQChartsColumn &c)
{
  if (c != column_) {
    column_ = c;

    updateItems();

    redraw();
  }
}

void
CQChartsColumnKey::
updatePosition(bool queued)
{
  updatePlotLocation();

  redraw(queued);
}

void
CQChartsColumnKey::
updateLayout()
{
  updateItems();

  redraw();
}

void
CQChartsColumnKey::
updateItems()
{
  // start at next row (vertical) or next column (horizontal) from previous key
  int row = 0;
  int col = 0;

  auto addKeyRow = [&](const QString &name, const ColorInd &ic) {
    auto *colorItem = new CQChartsKeyColorBox(this, ColorInd(), ColorInd(), ic);
    auto *textItem  = new CQChartsKeyText    (this, name, ic);

    auto *groupItem = new CQChartsKeyItemGroup(this);

    groupItem->addItem(colorItem);
    groupItem->addItem(textItem );

    addItem(groupItem, row, col);

    nextRowCol(row, col);

    return std::pair<CQChartsKeyColorBox *, CQChartsKeyText*>(colorItem, textItem);
  };

  clearItems();

  auto *details = plot_->columnDetails(column_);
  if (! details) return;

  int ng = details->numUnique();

  for (const auto &value : details->uniqueValues()) {
    int ig = details->uniqueId(value); // always 0 -> n - 1 ?

    addKeyRow(value.toString(), ColorInd(ig, ng));
  }
}

void
CQChartsColumnKey::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &desc)
{
  auto addProp = [&](const QString &name, const QString &desc, bool hidden=false) {
    auto *item = model->addProperty(path, this, name);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

  addProp("column", "Column");

  CQChartsPlotKey::addProperties(model, path, desc);
}

//------

CQChartsKeyItem::
CQChartsKeyItem(PlotKey *key, const ColorInd &ic) :
 key_(key), ic_(ic)
{
  assert(key_);
}

bool
CQChartsKeyItem::
selectPress(const Point &, SelMod selMod)
{
  if (isClickable()) {
    if      (key_->pressBehavior() == CQChartsKeyPressBehavior::Type::SHOW)
      doShow(selMod);
    else if (key_->pressBehavior() == CQChartsKeyPressBehavior::Type::SELECT)
      doSelect(selMod);
  }

  return true;
}

bool
CQChartsKeyItem::
selectMove(const Point &)
{
  return isClickable();
}

void
CQChartsKeyItem::
doShow(SelMod selMod)
{
  auto *plot = key_->plot();

  auto ic = calcColorInd();

  if      (selMod == SelMod::REPLACE) {
    for (int i = 0; i < ic.n; ++i)
      plot->setSetHidden(i, i != ic.i);
  }
  else if (selMod == SelMod::ADD)
    plot->setSetHidden(ic.i, false);
  else if (selMod == SelMod::REMOVE)
    plot->setSetHidden(ic.i, true);
  else if (selMod == SelMod::TOGGLE)
    plot->setSetHidden(ic.i, ! plot->isSetHidden(ic.i));

  plot->updateObjs();
}

void
CQChartsKeyItem::
doSelect(SelMod)
{
}

bool
CQChartsKeyItem::
tipText(const Point &, QString &) const
{
  return false;
}

//------

CQChartsKeyItemGroup::
CQChartsKeyItemGroup(Plot *plot) :
 CQChartsKeyItem(plot->key(), ColorInd()), plot_(plot)
{
}

CQChartsKeyItemGroup::
CQChartsKeyItemGroup(PlotKey *key) :
 CQChartsKeyItem(key, ColorInd()), plot_(key->plot())
{
}

CQChartsKeyItemGroup::
~CQChartsKeyItemGroup()
{
  for (auto &item : items_)
    delete item;
}

void
CQChartsKeyItemGroup::
setKey(PlotKey *key)
{
  CQChartsKeyItem::setKey(key);

  for (auto &item : items_)
    item->setKey(key);
}

void
CQChartsKeyItemGroup::
addItem(KeyItem *item)
{
  if (item->group())
    const_cast<CQChartsKeyItemGroup *>(item->group())->removeItem(item, /*keep*/true);

  items_.push_back(item);

  item->setGroup(this);

  item->setKey(const_cast<PlotKey *>(this->key()));
}

void
CQChartsKeyItemGroup::
removeItem(KeyItem *item, bool keep)
{
  assert(item->group() == this);

  KeyItems items;

  for (auto &item1 : items_)
    if (item1 != item)
      items_.push_back(item);

  std::swap(items_, items);

  if (! keep)
    delete item;
}

CQChartsGeom::Size
CQChartsKeyItemGroup::
size() const
{
  double w = 0.0;
  double h = 0.0;

  for (auto &item : items_) {
    auto s = item->size();

    w += s.width();

    h = std::max(h, s.height());
  }

  return Size(w, h);
}

bool
CQChartsKeyItemGroup::
tipText(const Point &p, QString &tip) const
{
  for (auto &item : items_) {
    if (item->tipText(p, tip))
      return true;
  }

  return false;
}

bool
CQChartsKeyItemGroup::
selectPress(const Point &p, SelMod selMod)
{
  for (auto &item : items_) {
    if (item->selectPress(p, selMod))
      return true;
  }

  return false;
}

bool
CQChartsKeyItemGroup::
selectMove(const Point &p)
{
  for (auto &item : items_) {
    if (item->selectMove(p))
      return true;
  }

  return false;
}

//---

void
CQChartsKeyItemGroup::
doShow(SelMod selMod)
{
  for (auto &item : items_)
    item->doShow(selMod);
}

void
CQChartsKeyItemGroup::
doSelect(SelMod selMod)
{
  for (auto &item : items_)
    item->doSelect(selMod);
}

//---

void
CQChartsKeyItemGroup::
draw(PaintDevice *device, const BBox &rect) const
{
  double x = rect.getXMin();
  double y = rect.getYMin();
  double h = rect.getHeight();

  for (auto &item : items_) {
    auto s = item->size();

    double dy = (h - s.height())/2;

    BBox rect1(x, y + dy, x + s.width(), y + s.height() + dy);

    item->draw(device, rect1);

    x += s.width();
  }
}

//------

CQChartsKeyText::
CQChartsKeyText(Plot *plot, const QString &text, const ColorInd &ic) :
 CQChartsKeyItem(plot->key(), ic), plot_(plot), text_(text)
{
}

CQChartsKeyText::
CQChartsKeyText(PlotKey *key, const QString &text, const ColorInd &ic) :
 CQChartsKeyItem(key, ic), plot_(key->plot()), text_(text)
{
}

CQChartsGeom::Size
CQChartsKeyText::
size() const
{
  auto *plot     = key_->plot();
  auto *drawPlot = key_->drawPlot();

  auto font = plot->view()->plotFont(plot, key_->textFont());

  QFontMetricsF fm(font);

  double clipLength = drawPlot->lengthPixelWidth(key_->textClipLength());
  auto   clipElide  = key_->textClipElide();

  auto text = CQChartsDrawUtil::clipTextToLength(text_, font, clipLength, clipElide);

  double w = fm.width(text);
  double h = fm.height();

  double ww = drawPlot->pixelToWindowWidth (w + 4);
  double wh = drawPlot->pixelToWindowHeight(h + 4);

  return Size(ww, wh);
}

QColor
CQChartsKeyText::
interpTextColor(const ColorInd &ind) const
{
  return key_->interpTextColor(ind);
}

void
CQChartsKeyText::
draw(PaintDevice *device, const BBox &rect) const
{
  auto *plot     = key_->plot();
  auto *drawPlot = key_->drawPlot();

  //---

  plot->view()->setPlotPainterFont(plot, device, key_->textFont());

  auto tc = interpTextColor(ColorInd());

  device->setPen(tc);

  CQChartsTextOptions textOptions;

  textOptions.align         = key_->textAlign();
  textOptions.contrast      = key_->isTextContrast();
  textOptions.contrastAlpha = key_->textContrastAlpha();
  textOptions.formatted     = key_->isTextFormatted();
  textOptions.scaled        = key_->isTextScaled();
  textOptions.html          = key_->isTextHtml();
  textOptions.clipLength    = drawPlot->lengthPixelWidth(key_->textClipLength());
  textOptions.clipElide     = key_->textClipElide();

  textOptions = plot->adjustTextOptions(textOptions);

  CQChartsDrawUtil::drawTextInBox(device, rect, text_, textOptions);
}

//------

CQChartsKeyColorBox::
CQChartsKeyColorBox(Plot *plot, const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
                    const RangeValue &xv, const RangeValue &yv) :
 CQChartsKeyItem(plot->key(), iv), plot_(plot), is_(is), ig_(ig), iv_(iv), xv_(xv), yv_(yv)
{
  assert(is_.isValid());
  assert(ig_.isValid());
  assert(iv_.isValid());

  setClickable(true);
}

CQChartsKeyColorBox::
CQChartsKeyColorBox(PlotKey *key, const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
                    const RangeValue &xv, const RangeValue &yv) :
 CQChartsKeyItem(key, iv), plot_(key->plot()), is_(is), ig_(ig), iv_(iv), xv_(xv), yv_(yv)
{
  assert(is_.isValid());
  assert(ig_.isValid());
  assert(iv_.isValid());

  setClickable(true);
}

QColor
CQChartsKeyColorBox::
interpStrokeColor(const ColorInd &ic) const
{
  return plot()->interpColor(strokeColor(), ic);
}

CQChartsGeom::Size
CQChartsKeyColorBox::
size() const
{
  auto *plot     = key_->plot();
  auto *drawPlot = key_->drawPlot();

  auto font = plot->view()->plotFont(plot, key_->textFont());

  QFontMetricsF fm(font);

  double h = fm.height();

  double ww = drawPlot->pixelToWindowWidth (h + 2);
  double wh = drawPlot->pixelToWindowHeight(h + 2);

  return Size(ww, wh);
}

bool
CQChartsKeyColorBox::
selectPress(const Point &w, SelMod selMod)
{
  if (! value_.isValid())
    return CQChartsKeyItem::selectPress(w, selMod);

  if (isClickable()) {
    if      (key_->pressBehavior() == CQChartsKeyPressBehavior::Type::SHOW) {
      auto *plot = key_->plot();

      if (CQChartsVariant::cmp(value_, plot->hideValue()) != 0)
        plot->setHideValue(value_);
      else
        plot->setHideValue(QVariant());

      plot->updateRangeAndObjs();
    }
    else if (key_->pressBehavior() == CQChartsKeyPressBehavior::Type::SELECT) {
    }
  }

  return true;
}

void
CQChartsKeyColorBox::
draw(PaintDevice *device, const BBox &rect) const
{
  auto *plot     = key_->plot();
  auto *drawPlot = key_->drawPlot();

  auto prect = drawPlot->windowToPixel(rect);

  bool swapped;
  auto prect1 = prect.adjusted(2, 2, -2, -2, swapped);
  if (swapped) return;

  //---

  CQChartsPenBrush penBrush;

  penBrush.pen   = strokePen();
  penBrush.brush = fillBrush();

  if (isInside())
    penBrush.brush.setColor(plot->insideColor(penBrush.brush.color()));

  auto bbox = drawPlot->pixelToWindow(prect1);

  CQChartsDrawUtil::drawRoundedRect(device, penBrush, bbox, cornerRadius());
}

QBrush
CQChartsKeyColorBox::
fillBrush() const
{
  auto *plot = key_->plot();

  auto ic = calcColorInd();

  QColor c;

  if (color_.isValid())
    c = plot_->interpColor(color_, ic);
  else
    c = plot->interpPaletteColor(ic);

  bool hidden = false;

  if (value_.isValid())
    hidden = (CQChartsVariant::cmp(value_, plot->hideValue()) == 0);
  else
    hidden = plot->isSetHidden(ic.i);

  if (hidden)
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

QPen
CQChartsKeyColorBox::
strokePen() const
{
  auto ic = calcColorInd();

  return interpStrokeColor(ic);
}

CQChartsKeyColorBox::ColorInd
CQChartsKeyColorBox::
calcColorInd() const
{
  return plot()->calcColorInd(nullptr, this, is_, ig_, iv_);
}

double
CQChartsKeyColorBox::
xColorValue(bool relative) const
{
  return (relative ? xv_.map() : xv_.v);
}

double
CQChartsKeyColorBox::
yColorValue(bool relative) const
{
  return (relative ? yv_.map() : yv_.v);
}

//------

CQChartsKeyLine::
CQChartsKeyLine(Plot *plot, const ColorInd &is, const ColorInd &ig) :
 CQChartsKeyItem(plot->key(), is.n > 1 ? is : ig), is_(is), ig_(ig)
{
  setClickable(true);
}

CQChartsKeyLine::
CQChartsKeyLine(PlotKey *key, const ColorInd &is, const ColorInd &ig) :
 CQChartsKeyItem(key, is.n > 1 ? is : ig), is_(is), ig_(ig)
{
  setClickable(true);
}

CQChartsGeom::Size
CQChartsKeyLine::
size() const
{
  auto *plot     = key_->plot();
  auto *drawPlot = key_->drawPlot();

  auto font = plot->view()->plotFont(plot, key_->textFont());

  QFontMetricsF fm(font);

  double w = fm.width("-X-");
  double h = fm.height();

  double ww = drawPlot->pixelToWindowWidth (w + 8);
  double wh = drawPlot->pixelToWindowHeight(h + 2);

  return Size(ww, wh);
}

bool
CQChartsKeyLine::
selectPress(const Point &w, SelMod selMod)
{
  if (! value_.isValid())
    return CQChartsKeyItem::selectPress(w, selMod);

  if (isClickable()) {
    if      (key_->pressBehavior() == CQChartsKeyPressBehavior::Type::SHOW) {
      auto *plot = key_->plot();

      if (CQChartsVariant::cmp(value_, plot->hideValue()) != 0)
        plot->setHideValue(value_);
      else
        plot->setHideValue(QVariant());

      plot->updateRangeAndObjs();
    }
    else if (key_->pressBehavior() == CQChartsKeyPressBehavior::Type::SELECT) {
    }
  }

  return true;
}

void
CQChartsKeyLine::
draw(PaintDevice *device, const BBox &rect) const
{
  auto *plot     = key_->plot();
  auto *drawPlot = key_->drawPlot();

  auto prect = drawPlot->windowToPixel(rect);

  bool swapped;
  auto pbbox1 = prect.adjusted(2, 2, -2, -2, swapped);
  if (swapped) return;

  double x1 = pbbox1.getXMin() + 4;
  double x2 = pbbox1.getXMax() - 4;
  double y  = pbbox1.getYMid();

  const auto &fillData   = symbolData_.fill();
  const auto &strokeData = symbolData_.stroke();

  auto lc = plot->interpColor(fillData  .color(), ig_);
  auto fc = plot->interpColor(strokeData.color(), ig_);

  CQChartsPenBrush penBrush;

  plot->setPenBrush(penBrush, PenData(true, lc), BrushData(true, fc));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawLine(drawPlot->pixelToWindow(Point(x1, y)), drawPlot->pixelToWindow(Point(x2, y)));

  Point ps(CMathUtil::avg(x1, x2), y);

  CQChartsDrawUtil::drawSymbol(device, penBrush, symbolData_.type(), drawPlot->pixelToWindow(ps),
                               symbolData_.size());
}

//------

CQChartsGradientKeyItem::
CQChartsGradientKeyItem(Plot *plot) :
 CQChartsKeyItem(plot->key(), ColorInd()), plot_(plot)
{
}

CQChartsGradientKeyItem::
CQChartsGradientKeyItem(PlotKey *key) :
 CQChartsKeyItem(key, ColorInd()), plot_(key->plot())
{
}

CQChartsGeom::Size
CQChartsGradientKeyItem::
size() const
{
  // get char width/height
  auto font = plot_->view()->plotFont(plot_, key_->textFont());

  QFontMetricsF fm(font);

  double fw = fm.width("X");
  double fh = fm.height();

  //---

  // calc max label width
  QStringList labels;

  calcLabels(labels);

  double tw = 0.0;

  for (const auto &label : labels)
    tw = std::max(tw, fm.width(label));

  //--

  double ww = plot_->pixelToWindowWidth (2*fw + tw + 6);
  double wh = plot_->pixelToWindowHeight(7*fh + fh + 4);

  return Size(ww, wh);
}

void
CQChartsGradientKeyItem::
draw(PaintDevice *device, const BBox &rect) const
{
  // get char height
  plot_->view()->setPlotPainterFont(plot_, device, key_->textFont());

  QFontMetricsF fm(device->font());

  double fh = fm.height();

  //---

  // calc max label width
  QStringList labels;

  calcLabels(labels);

  double tw = 0.0;

  for (const auto &label : labels)
    tw = std::max(tw, fm.width(label));

  double wtw = plot_->pixelToWindowWidth(tw);

  //---

  // calc margins
  double wxm = plot_->pixelToWindowWidth (2);
  double wym = plot_->pixelToWindowHeight(fh/2 + 2);

  //---

  // calc left/right boxes
  BBox lrect(rect.getXMin() + wxm, rect.getYMin() + wym,
             rect.getXMax() - wtw - 2*wxm, rect.getYMax() - wym);
  BBox rrect(rect.getXMax() - wtw - wxm, rect.getYMin() + wym,
             rect.getXMax() - wxm, rect.getYMax() - wym);

  auto lprect = plot_->windowToPixel(lrect);
  auto rprect = plot_->windowToPixel(rrect);

  //---

  // draw gradient in left box
  Point pg1(lprect.getXMin(), lprect.getYMax());
  Point pg2(lprect.getXMin(), lprect.getYMin());

  QLinearGradient lg(pg1.x, pg1.y, pg2.x, pg2.y);

  CQColorsPalette *colorsPalette = nullptr;

  if (palette().isValid())
    colorsPalette = palette().palette();
  else
    colorsPalette = plot_->view()->themePalette();

  colorsPalette->setLinearGradient(lg, 1.0);

  QBrush brush(lg);

  BBox fbbox(pg1.x                    , pg2.y,
             pg1.x + lprect.getWidth(), pg2.y + lprect.getHeight());

  device->setBrush(brush);

  device->fillRect(device->pixelToWindow(fbbox));
  device->drawRect(device->pixelToWindow(fbbox));

  //---

  // rect (pixel) positions of label positions
  double y1 = rprect.getYMax();
  double y5 = rprect.getYMin();

  double dy = (y1 - y5)/4.0; // delta for min, lower mid, mid, upper mid, max

  double y2 = y1 - dy;       // lower mid
  double y4 = y5 + dy;       // upper mid
  double y3 = (y5 + y1)/2.0; // mid

  //---

  // set text pen
  CQChartsPenBrush penBrush;

  auto tc = plot_->interpThemeColor(ColorInd(1.0));

  plot_->setPen(penBrush, PenData(true, tc, Alpha()));

  device->setPen(penBrush.pen);

  //---

  // draw labels
  auto drawTextLabel = [&](const Point &p, const QString &label) {
    auto p1 = plot_->pixelToWindow(p);

    CQChartsTextOptions options;

    options.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, label, options);
  };

  double x1 = rprect.getXMin();
  double df = (fm.ascent() - fm.descent())/2.0;

  drawTextLabel(Point(x1, y1 + df), labels[0]);
  drawTextLabel(Point(x1, y2 + df), labels[1]);
  drawTextLabel(Point(x1, y3 + df), labels[2]);
  drawTextLabel(Point(x1, y4 + df), labels[3]);
  drawTextLabel(Point(x1, y5 + df), labels[4]);
}

void
CQChartsGradientKeyItem::
calcLabels(QStringList &labels) const
{
  // calc label values
  double n1 = minValue_;
  double n5 = maxValue_;

  double dn = (n5 - n1)/4.0; // delta for min, lower mid, mid, upper mid, max

  double n2 = n1 + dn;       // lower mid
  double n4 = n5 - dn;       // upper mid
  double n3 = (n5 + n1)/2.0; // mid

  labels.push_back(QString("%1").arg(n1));
  labels.push_back(QString("%1").arg(n2));
  labels.push_back(QString("%1").arg(n3));
  labels.push_back(QString("%1").arg(n4));
  labels.push_back(QString("%1").arg(n5));
}

//------

CQChartsMapKey::
CQChartsMapKey(Plot *plot) :
 CQChartsObj(plot->charts()), plot_(plot)
{
  setEditable(true);
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
contains(const Point &p) const
{
  return bbox_.inside(p);
}

CQChartsEditHandles *
CQChartsMapKey::
editHandles() const
{
  if (! editHandles_) {
    auto *th = const_cast<CQChartsMapKey *>(this);

    th->editHandles_ = std::make_unique<EditHandles>(plot(), EditHandles::Mode::MOVE);
  }

  return editHandles_.get();
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

void
CQChartsMapKey::
drawEditHandles(QPainter *painter) const
{
  assert(plot_->view()->mode() == CQChartsView::Mode::EDIT && isSelected());

  if (! isVisible())
    return;

  setEditHandlesBBox();

  editHandles()->draw(painter);
}

void
CQChartsMapKey::
setEditHandlesBBox() const
{
  auto bbox = this->bbox();
  if (! bbox.isValid()) return;

  auto *th = const_cast<CQChartsMapKey *>(this);

  th->editHandles()->setBBox(bbox);
}

//---

CQChartsColorMapKey::
CQChartsColorMapKey(Plot *plot) :
 CQChartsMapKey(plot)
{
}

void
CQChartsColorMapKey::
addProperties(PropertyModel *model, const QString &path)
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

  addProp("margin", "Margin");

  addProp("paletteName", "Palette Name");
}

void
CQChartsColorMapKey::
draw(PaintDevice *device, bool /*usePenBrush*/)
{
  QFontMetricsF fm(device->font());

  double bm = this->margin();
  double bw = fm.width("X") + 16;

  auto dataMid = CMathUtil::avg(dataMin(), dataMax());

  auto dataMinStr = QString("%1").arg(dataMin());
  auto dataMidStr = QString("%1").arg(dataMid  );
  auto dataMaxStr = QString("%1").arg(dataMax());

  double tw = std::max(std::max(fm.width(dataMinStr), fm.width(dataMidStr)), fm.width(dataMaxStr));

  double kw = bw + tw + 4;
  double kh = fm.height()*5;

  // calc center
  Point pos;

  if (position().isValid()) {
    pos = plot_->positionToPixel(position());
  }
  else {
    auto pbbox = plot_->calcPlotPixelRect();

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

  // fill background
  auto fillColor   = plot_->interpThemeColor(ColorInd(0.0));
  auto strokeColor = plot_->interpThemeColor(ColorInd(1.0));

  PenBrush penBrush;

  plot_->setPenBrush(penBrush, PenData(true, strokeColor), BrushData(true, fillColor));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->fillRect(bbox());

  //---

  // draw border
  device->drawRect(bbox());

  //---

  // draw gradient
  BBox gbbox(pbbox.getXMin() + bm     , pbbox.getYMin() + bm,
             pbbox.getXMin() + bw + bm, pbbox.getYMax() - bm);

  QLinearGradient lg(gbbox.getXMid(), gbbox.getYMax(), gbbox.getXMid(), gbbox.getYMin());

  CQColorsPalette *colorsPalette = nullptr;

  if (paletteName().isValid())
    colorsPalette = paletteName().palette();
  else
    colorsPalette = plot_->view()->themePalette();

  colorsPalette->setLinearGradient(lg, 1.0, mapMin(), mapMax());

  QBrush brush(lg);

  device->setBrush(brush);

  device->fillRect(device->pixelToWindow(gbbox));

  //---

  // draw labels
  auto drawTextLabel = [&](const Point &p, const QString &label) {
    auto p1 = plot_->pixelToWindow(p);

    CQChartsTextOptions options;

    options.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, label, options);
  };

  double df = (fm.ascent() - fm.descent())/2.0;

  double tx  = gbbox.getXMax() + bm;
  double ty1 = pbbox.getYMin() + df;
  double ty2 = pbbox.getYMid() + df;
  double ty3 = pbbox.getYMax() + df;

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

//------

CQChartsSymbolSizeMapKey::
CQChartsSymbolSizeMapKey(Plot *plot) :
 CQChartsMapKey(plot)
{
}

void
CQChartsSymbolSizeMapKey::
addProperties(PropertyModel *model, const QString &path)
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

  addProp("margin", "Margin");

  addProp("scale"  , "Scale Factor");
  addProp("stacked", "Stacked Vertical instead of overlaid");
  addProp("rows"   , "Number of symbol rows");

  addProp("textAlign"  , "Text Align");
  addProp("paletteName", "Palette Name");

  addProp("border", "Border");

  addProp("alpha", "Alpha");
}

void
CQChartsSymbolSizeMapKey::
draw(PaintDevice *device, bool usePenBrush)
{
  drawCircles(device, usePenBrush);

  CQChartsTextOptions textOptions;

  drawText(device, textOptions, usePenBrush);

  drawBorder(device, usePenBrush);
}

void
CQChartsSymbolSizeMapKey::
drawCircles(PaintDevice *device, bool usePenBrush)
{
  auto drawEllipse = [&](const QColor &c, const BBox &pbbox) {
    PenBrush penBrush;

    if (! usePenBrush) {
      auto strokeColor = plot_->interpThemeColor(ColorInd(1.0));

      plot_->setPenBrush(penBrush,
        PenData(true, strokeColor), BrushData(true, c, this->alpha()));
    }
    else {
      penBrush.pen   = device->pen();
      penBrush.brush = device->brush();

      penBrush.brush.setColor(c);
    }

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    device->drawEllipse(plot_->pixelToWindow(pbbox));
  };

  //---

  BBoxes pbboxes;

  getSymbolBoxes(pbboxes);

  if (pbboxes.empty())
    return;

  //---

  // draw ellipse for min, mean, max radii
  auto *palette = (paletteName_.isValid() ? paletteName_.palette() : nullptr);

  double y  = 1.0;
  double dy = (pbboxes.size() > 1 ? 1.0/(pbboxes.size() - 1) : 0.0);

  for (const auto &pbbox : pbboxes) {
    auto fillColor = (palette ?
      palette->getColor(y, /*scale*/false, /*invert*/false) :
      plot_->interpPaletteColor(ColorInd(y)));

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

    auto strokeColor = plot_->interpThemeColor(ColorInd(1.0));

    plot_->setPenBrush(penBrush, PenData(true, strokeColor), BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, penBrush);
  }

  //---

  QFontMetricsF fm(device->font());

  auto drawText = [&](const Point &p, double value, double &tw, Qt::Alignment align) {
    QString text;

    if      (CMathUtil::realEq(value, 0.0))
      text = "0.0";
    else if (CMathUtil::realEq(value, 1.0))
      text = "1.0";
    else
      text.setNum(value);

    tw = fm.width(text);

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

    auto p2 = plot_->pixelToWindow(p1);

    CQChartsDrawUtil::drawTextAtPoint(device, p2, text, textOptions1);
  };

  //---

  BBoxes pbboxes;

  getSymbolBoxes(pbboxes);

  if (pbboxes.empty())
    return;

  auto &pbbox1 = pbboxes[0];

  //---

  double min = this->dataMin();
  double max = this->dataMax();

  double y  = 1.0;
  double dy = (pbboxes.size() > 1 ? 1.0/(pbboxes.size() - 1) : 0.0);

  // outer margin
  double pm = this->margin();

  double pxt1 = (! isStacked() ? pbbox1.getXMid() : pbbox1.getXMax() + pm);
  double pyt1 = (! isStacked() ? pbbox1.getYMid() : pbbox1.getYMin()     );
  double pxt2 = pxt1;
  double pyt2 = pyt1;

  double fh = fm.height();
  double fa = fm.ascent();

  for (const auto &pbbox : pbboxes) {
    double r = CMathUtil::map(y, 0.0, 1.0, min, max);

    double tw;

    if (! isStacked()) {
      drawText(Point(pbbox.getXMid(), pbbox.getYMin() - 2), r, tw, textAlign());

      pxt1 = std::min(pxt1, pbbox.getXMid() - tw/2.0 - pm);
      pxt2 = std::max(pxt2, pbbox.getXMid() + tw/2.0 + pm);

      pyt1 = std::min(pyt1, pbbox.getYMin() - 2 - fa - pm);
      pyt2 = std::max(pyt2, pbbox.getYMin() - 2      + pm);
    }
    else {
      if      (textAlign() & Qt::AlignLeft) {
        drawText(Point(pbbox1.getXMin() - pm, pbbox.getYMid()), r, tw, textAlign());

        pxt1 = std::min(pxt1, pbbox1.getXMin() - pm - tw - pm);
      }
      else if (textAlign() & Qt::AlignRight) {
        drawText(Point(pbbox1.getXMax() + pm, pbbox.getYMid()), r, tw, textAlign());

        pxt2 = std::max(pxt2, pbbox1.getXMax() + pm + tw + pm);
      }
      else {
        drawText(Point(pbbox1.getXMid(), pbbox.getYMid()), r, tw, textAlign());

        pxt1 = std::min(pxt2, pbbox1.getXMid() - tw/2.0 - pm);
        pxt2 = std::max(pxt2, pbbox1.getXMid() + tw/2.0 + pm);
      }

      pyt1 = std::min(pyt1, pbbox1.getYMid() - fh/2.0 - pm);
      pyt2 = std::max(pyt2, pbbox1.getYMid() - fh/2.0 + pm);
    }

    y -= dy;
  }

  //---

  auto *th = const_cast<CQChartsSymbolSizeMapKey *>(this);

  th->tbbox_ = plot_->pixelToWindow(BBox(pxt1, pyt1, pxt2, pyt2));
}

void
CQChartsSymbolSizeMapKey::
drawBorder(PaintDevice *device, bool usePenBrush)
{
  if (! border_)
    return;

  BBoxes pbboxes;

  getSymbolBoxes(pbboxes);

  if (pbboxes.empty())
    return;

  if (! usePenBrush) {
    PenBrush penBrush;

    auto strokeColor = plot_->interpThemeColor(ColorInd(1.0));

    plot_->setPenBrush(penBrush, PenData(true, strokeColor), BrushData(false));

    CQChartsDrawUtil::setPenBrush(device, penBrush);
  }

  auto bbox = this->bbox() + tbbox_;

  device->drawRect(bbox);
}

void
CQChartsSymbolSizeMapKey::
invalidate()
{
  emit dataChanged();
}

void
CQChartsSymbolSizeMapKey::
getSymbolBoxes(BBoxes &pbboxes) const
{
  // symbol sizes
  double prmin = scale()*this->mapMin();
  double prmax = scale()*this->mapMax();

  //---

  // calc center
  Point pos;

  if (position().isValid()) {
    pos = plot_->positionToPixel(position());
  }
  else {
    auto pbbox = plot_->calcPlotPixelRect();

    double px = pbbox.getXMax() - prmin;
    double py = pbbox.getYMax() - prmin;

    pos = Point(px, py);
  }

  double xm = pos.x;
  double ym = pos.y;

  //---

  if (rows() < 1)
    return;

  double y  = 1.0;
  double sy = (rows() > 1 ? 1.0/(rows() - 1) : 0.0);

  // outer margin
  double pm = this->margin();

  //---

  BBox bbox;

  auto *th = const_cast<CQChartsSymbolSizeMapKey *>(this);

  if (! isStacked()) {
    double yb = ym + prmax;

    for (int i = 0; i < rows(); ++i) {
      double pr = CMathUtil::map(y, 0.0, 1.0, prmin, prmax);

      auto pbbox = BBox(xm - pr, yb - 2*pr, xm + pr, yb);

      pbboxes.push_back(pbbox);

      y -= sy;
    }

    bbox = plot_->pixelToWindow(BBox(xm - prmax - pm, ym - prmax - pm,
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

      pbboxes.push_back(pbbox);

      yt += 2*pr + pm;

      y -= sy;
    }

    bbox = plot_->pixelToWindow(BBox(xm - prmax - pm, ym - h/2 - pm,
                                     xm + prmax + pm, ym + h/2 + pm));
  }

  //---

  double w = bbox.getWidth ();
  double h = bbox.getHeight();

  double dx = 0.0;
  double dy = 0.0;

  if      (align() & Qt::AlignLeft ) dx =  w/2.0;
  else if (align() & Qt::AlignRight) dx = -w/2.0;

  if      (align() & Qt::AlignBottom) dy =  h/2.0;
  else if (align() & Qt::AlignTop   ) dy = -h/2.0;

  bbox = bbox.translated(dx, dy);

  double pdx = plot_->windowToSignedPixelWidth (dx);
  double pdy = plot_->windowToSignedPixelHeight(dy);

  for (auto &pbbox : pbboxes)
    pbbox = pbbox.translated(pdx, pdy);

  //---

  th->setBBox(bbox);
}

//------

CQChartsSymbolTypeMapKey::
CQChartsSymbolTypeMapKey(Plot *plot) :
 CQChartsMapKey(plot)
{
}

void
CQChartsSymbolTypeMapKey::
addProperties(PropertyModel *model, const QString &path)
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

  addProp("margin", "Margin");
}

void
CQChartsSymbolTypeMapKey::
draw(PaintDevice *device, bool /*usePenBrush*/)
{
  auto ks = pixelSize();

  QFontMetricsF fm(device->font());

  double bm = this->margin();
  double bw = fm.width("X") + 4;

  double kw = ks.width ();
  double kh = ks.height();

  // calc center
  Point pos;

  if (position().isValid()) {
    pos = plot_->positionToPixel(position());
  }
  else {
    auto pbbox = plot_->calcPlotPixelRect();

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

  // fill background
  auto fillColor   = plot_->interpThemeColor(ColorInd(0.0));
  auto strokeColor = plot_->interpThemeColor(ColorInd(1.0));

  PenBrush penBrush;

  plot_->setPenBrush(penBrush, PenData(true, strokeColor), BrushData(true, fillColor));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->fillRect(bbox());

  //---

  // draw border
  device->drawRect(bbox());

  //---

  const auto *symbolSetMgr = plot_->charts()->symbolSetMgr();

  auto *symbolSet = symbolSetMgr->symbolSet(symbolSet_);

  // draw symbols and labels
  auto drawTextLabel = [&](const Point &p, const QString &label) {
    auto p1 = plot_->pixelToWindow(p);

    CQChartsTextOptions options;

    options.align = Qt::AlignLeft;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, label, options);
  };

  double df = (fm.ascent() - fm.descent())/2.0;

  auto symbolFillColor = plot_->interpPaletteColor(ColorInd());

  for (int i = mapMin(); i <= mapMax(); ++i) {
    auto dataStr = QString("%1").arg(CMathUtil::map(i, mapMin(), mapMax(), dataMin(), dataMax()));

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
      plot_->setPenBrush(symbolPenBrush,
        PenData(true, strokeColor), BrushData(true, symbolFillColor));
    else
      plot_->setPenBrush(symbolPenBrush,
        PenData(true, strokeColor), BrushData(false));

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
  auto font = plot_->view()->viewFont(plot_->font());

  QFontMetricsF fm(font);

  double bm = this->margin();
  double bw = fm.width("X") + 4;
  double kh = (fm.height() + 2)*(mapMax() - mapMin() + 1) + 2*bm;

  double tw = 0.0;

  for (int i = mapMin(); i <= mapMax(); ++i) {
    auto dataStr = QString("%1").arg(CMathUtil::map(i, mapMin(), mapMax(), dataMin(), dataMax()));

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
