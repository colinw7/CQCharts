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
#include <CMathRound.h>

#include <QScrollBar>

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
}

void
CQChartsKey::
init()
{
  setObjectName("key");

  setEditable(true);

  //---

  setLocation(Location(Location::Type::TOP_RIGHT));

  setFilled(true);
  setFillAlpha(Alpha(0.5));

  //---

  setHeaderTextAlign(Qt::AlignHCenter | Qt::AlignVCenter);

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
addScrollBars(View *view) const
{
  if (! scrollData_.hbar) {
    auto *th = const_cast<CQChartsKey *>(this);

    // create scroll bar
    th->scrollData_.hbar = new QScrollBar(Qt::Horizontal, view);
    th->scrollData_.hbar->setObjectName("keyHBar");

    th->scrollData_.vbar = new QScrollBar(Qt::Vertical  , view);
    th->scrollData_.vbar->setObjectName("keyVBar");

    hideScrollBars();

    connect(scrollData_.hbar, SIGNAL(valueChanged(int)), this, SLOT(hscrollSlot(int)));
    connect(scrollData_.vbar, SIGNAL(valueChanged(int)), this, SLOT(vscrollSlot(int)));
  }
}

void
CQChartsKey::
removeScrollBars()
{
  if (scrollData_.hbar) {
    delete scrollData_.hbar;
    delete scrollData_.vbar;

    scrollData_.hbar = nullptr;
    scrollData_.vbar = nullptr;
  }
}

void
CQChartsKey::
hideScrollBars() const
{
  if (scrollData_.hbar) {
    scrollData_.hbar->hide();
    scrollData_.vbar->hide();
  }
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
setLocation(const Location &l)
{
  CQChartsUtil::testAndSet(location_, l, [&]() { updatePosition(); } );
}

//---

void
CQChartsKey::
setHeaderStr(const QString &s)
{
  CQChartsUtil::testAndSet(headerStr_, s, [&]() { updateLayout(); } );
}

void
CQChartsKey::
setDefHeaderStr(const QString &s, bool update)
{
  CQChartsUtil::testAndSet(defHeaderStr_, s, [&]() {
    if (update)
     updateLayout();
  } );
}

QString
CQChartsKey::
calcHeaderStr() const
{
  if (headerStr_.length())
    return headerStr_;

  return defHeaderStr_;
}

//---

void
CQChartsKey::
setHiddenColor(const Color &c)
{
  CQChartsUtil::testAndSet(hiddenColor_, c, [&]() { redraw(); } );
}

void
CQChartsKey::
setHiddenAlpha(const Alpha &a)
{
  CQChartsUtil::testAndSet(hiddenAlpha_, a, [&]() { redraw(); } );
}

//---

QColor
CQChartsKey::
calcHiddenColor(const QColor &c) const
{
  QColor c1;

  if (hiddenColor().isValid()) {
    charts()->setContrastColor(c);

    c1 = view()->interpColor(hiddenColor(), ColorInd());

    charts()->resetContrastColor();
  }
  else
    c1 = CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), hiddenAlpha().value());

  return c1;
}

//---

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
  setPressBehavior(view->keyBehavior());

  setMargin (Margin::pixel(8, 8, 8, 8));
  setPadding(Margin::pixel(4, 4, 4, 4));
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
  updatePosition();
}

void
CQChartsViewKey::
doLayout()
{
  if (location().type() == Location::Type::ABSOLUTE_POSITION)
    return;

  //----

  auto font = view()->viewFont(textFont());

  QFontMetricsF fm(font);

  double bs = fm.height() + 4.0;

  //---

  // get external margin (view)
  double xlm = view()->lengthViewWidth (margin().left  ());
  double xrm = view()->lengthViewWidth (margin().right ());
  double ytm = view()->lengthViewHeight(margin().top   ());
  double ybm = view()->lengthViewHeight(margin().bottom());

  // get internal padding (pixels)
  double xlp = view()->lengthPixelWidth (padding().left  ());
  double xrp = view()->lengthPixelWidth (padding().right ());
  double ytp = view()->lengthPixelHeight(padding().top   ());
  double ybp = view()->lengthPixelHeight(padding().bottom());

  //----

  double x = 0.0, y = 0.0;

  if      (location().onLeft   ()) { x =         xlm;  }
  else if (location().onHCenter()) { x =        50.0;  }
  else if (location().onRight  ()) { x = 100.0 - xrm;  }

  if      (location().onTop    ()) { y = 100.0 - ytm;  }
  else if (location().onVCenter()) { y =        50.0;  }
  else if (location().onBottom ()) { y =         ybm;  }

  //----

  auto p = view()->windowToPixel(Point(x, y));

  //---

  numPlots_ = view()->numPlots();

  if (numPlots_ > 0) {
    double pw = 0.0;
    double ph = 0.0;

    for (int i = 0; i < numPlots_; ++i) {
      auto *ploti = view()->plot(i);

      auto name = ploti->keyText();

      double tw = fm.horizontalAdvance(name) + bs + 4;

      pw = std::max(pw, tw);

      ph += bs;
    }

    psize_ = Size(pw + xlp + xrp, ph + ybp + ytp + (numPlots_ - 1)*2);

    //---

    double pxr = 0.0, pyr = 0.0;

    if      (location().onLeft   ()) pxr = p.x;
    else if (location().onHCenter()) pxr = p.x - psize_.width ()/2;
    else if (location().onRight  ()) pxr = p.x - psize_.width ();

    if      (location().onTop    ()) pyr = p.y;
    else if (location().onVCenter()) pyr = p.y - psize_.height()/2;
    else if (location().onBottom ()) pyr = p.y - psize_.height();

    pposition_ = Point(pxr, pyr); // top left
  }
  else {
    psize_     = Size (0.0, 0.0);
    pposition_ = Point(0.0, 0.0);
  }

  wposition_ = view()->pixelToWindow(pposition_);
}

void
CQChartsViewKey::
addProperties(PropertyModel *model, const QString &path, const QString &/*desc*/)
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

  addStyleProp("hiddenColor", "Color for hidden items");
  addStyleProp("hiddenAlpha", "Alpha for hidden items");

  addProp("columns", "Number of item columns");

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

  addHeaderStyleProp("headerTextColor"        , "color"        , "color");
  addHeaderStyleProp("headerTextAlpha"        , "alpha"        , "alpha");
  addHeaderStyleProp("headerTextFont"         , "font"         , "font");
//addHeaderStyleProp("headerTextAngle"        , "angle"        , "angle");
  addHeaderStyleProp("headerTextContrast"     , "contrast"     , "contrast");
  addHeaderStyleProp("headerTextContrastAlpha", "contrastAlpha", "contrast alpha");
  addHeaderStyleProp("headerTextAlign"        , "align"        , "align");
  addHeaderStyleProp("headerTextFormatted"    , "formatted"    , "formatted to fit box");
  addHeaderStyleProp("headerTextScaled"       , "scaled"       , "scaled to box");
  addHeaderStyleProp("headerTextHtml"         , "html"         , "is html");
  addHeaderStyleProp("headerTextClipLength"   , "clipLength"   , "clipped to length");
  addHeaderStyleProp("headerTextClipElide"    , "clipElide"    , "clip elide");

  //---

  // stroke, fill
  addBoxProperties(model, path, "");

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

  addTextStyleProp("textColor"        , "color"        , "color");
  addTextStyleProp("textAlpha"        , "alpha"        , "alpha");
  addTextStyleProp("textFont"         , "font"         , "font");
//addTextStyleProp("textAngle"        , "angle"        , "angle");
  addTextStyleProp("textContrast"     , "contrast"     , "contrast");
  addTextStyleProp("textContrastAlpha", "contrastAlpha", "contrast alpha");
  addTextStyleProp("textAlign"        , "align"        , "align");
  addTextStyleProp("textFormatted"    , "formatted"    , "formatted to fit box");
  addTextStyleProp("textScaled"       , "scaled"       , "scaled to box");
  addTextStyleProp("textHtml"         , "html"         , "is html");
  addTextStyleProp("textClipLength"   , "clipLength"   , "clipped to length");
  addTextStyleProp("textClipElide"    , "clipElide"    , "clip elide");
}

int
CQChartsViewKey::
numItems() const
{
  return std::min(view()->numPlots(), int(prects_.size()));
}

CQChartsGeom::BBox
CQChartsViewKey::
itemBBox(int i) const
{
  int n = numItems();
  if (i < 0 || i >= n) return BBox();

  return prects_[size_t(i)];
}

bool
CQChartsViewKey::
contains(const Point &p) const
{
  return bbox().inside(p);
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

  // pixel position & size
  double px = pposition_.x; // left
  double py = pposition_.y; // top

  double pw = psize_.width ();
  double ph = psize_.height();

  pbbox_ = BBox(px, py, px + pw, py + ph);

  th->setBBox(view()->pixelToWindow(pbbox_));

  //---

  CQChartsBoxObj::draw(device, bbox());

  //---

  // get external margin (view)
//double xlm = view()->lengthViewWidth (margin().left  ());
//double xrm = view()->lengthViewWidth (margin().right ());
//double ytm = view()->lengthViewHeight(margin().top   ());
//double ybm = view()->lengthViewHeight(margin().bottom());

  // get internal padding (pixels)
  double xlp = view()->lengthPixelWidth (padding().left  ());
  double xrp = view()->lengthPixelWidth (padding().right ());
  double ytp = view()->lengthPixelHeight(padding().top   ());
//double ybp = view()->lengthPixelHeight(padding().bottom());

  //---

  view()->setPainterFont(device, textFont());

  QFontMetricsF fm(device->font());

  double px1 = px + xlp;
  double py1 = py + ytp;

  double bs = fm.height() + 4.0;

  //double dth = (bs - fm.height())/2;

  prects_.clear();

  for (int i = 0; i < numPlots_; ++i) {
    double py2 = py1 + bs + 2;

    auto *ploti = view()->plot(i);

    bool checked = ploti->isVisible();

    //---

    device->setPen(interpTextColor(ColorInd()));

    CQChartsDrawUtil::drawCheckBox(device, px1, (py1 + py2)/2.0 - bs/2.0, int(bs), checked);

    //---

    device->setPen(interpTextColor(ColorInd()));

    auto name = ploti->keyText();

    double px2 = px1 + bs + 2;

    //double tw = fm.horizontalAdvance(name);

    BBox rect1(px2, py1, px + pw - xrp, py2);

    auto textOptions = this->textOptions();

    CQChartsDrawUtil::drawTextInBox(device, view()->pixelToWindow(rect1), name, textOptions);

    //---

    // save view key item (plot) rect
    auto p1 = view()->pixelToWindow(Point(px     , py1));
    auto p2 = view()->pixelToWindow(Point(px + pw, py2));

    BBox prect(p1.x, p2.y, p2.x, p1.y);

    prects_.push_back(std::move(prect));

    //---

    py1 = py2;
  }
}

void
CQChartsViewKey::
drawEditHandles(PaintDevice *device) const
{
  assert(view()->mode() == CQChartsView::Mode::EDIT || isSelected());

  setEditHandlesBBox();

  editHandles()->draw(device);
}

void
CQChartsViewKey::
setEditHandlesBBox() const
{
  editHandles()->setMode(EditHandles::Mode::MOVE);

  editHandles()->setBBox(pbbox_);
}

//------

bool
CQChartsViewKey::
selectPress(const Point &w, SelData &selData)
{
  int n = numItems();

  for (int i = 0; i < n; ++i) {
    if (! prects_[size_t(i)].inside(w))
      continue;

    if      (pressBehavior().type() == KeyBehavior::Type::SHOW)
      doShow(i, selData.selMod);
    else if (pressBehavior().type() == KeyBehavior::Type::SELECT)
      doSelect(i, selData.selMod);

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

  location_ = Location(Location::Type::ABSOLUTE_POSITION);

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
  auto *ploti = view()->plot(i);

  View::Plots plots;

  view()->getPlots(plots);

  // no modifiers : toggle clicked
  if      (selMod == SelMod::REPLACE) {
    ploti->setVisible(! ploti->isVisible());
  }
  // control: clicked plot only
  else if (selMod == SelMod::ADD) {
    for (auto &plot1 : plots)
      plot1->setVisible(plot1 == ploti);
  }
  // shift: unclicked plots
  else if (selMod == SelMod::REMOVE) {
    for (auto &plot1 : plots)
      plot1->setVisible(plot1 != ploti);
  }
  // control+shift: toggle all on/off
  else if (selMod == SelMod::TOGGLE) {
    // make all visible if some not visible
    int num_visible = 0;

    for (auto &plot1 : plots)
      num_visible += plot1->isVisible();

    bool state = (num_visible != int(plots.size()) ? true : false);

    for (auto &plot1 : plots)
      plot1->setVisible(state);
  }
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

  setPressBehavior(plot->view()->keyBehavior());

  clearItems();

  //---

  scrollData_.pixelBarSize = view()->style()->pixelMetric(QStyle::PM_ScrollBarExtent) + 2;

  //---

  setMargin (Margin::pixel(4, 4, 4, 4));
  setPadding(Margin::pixel(2, 2, 2, 2));

  //---

  addScrollBars(view());
}

CQChartsPlotKey::
~CQChartsPlotKey()
{
  removeScrollBars();

  for (auto &item : items_)
    delete item;
}

//---

CQChartsPlot *
CQChartsPlotKey::
drawPlot() const
{
  auto *drawPlot = plot();

  while (drawPlot && drawPlot->isComposite())
    drawPlot = dynamic_cast<CQChartsCompositePlot *>(plot())->currentPlot();

  return drawPlot;
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

  updatePosition();
}

void
CQChartsPlotKey::
updatePosition(bool queued)
{
  resetBBox();

  plot()->updateKeyPosition();

  if (plot()->isAutoFit())
    plot()->setNeedsAutoFit(true);

  redraw(queued);
}

void
CQChartsPlotKey::
updatePositionAndLayout(bool queued)
{
  invalidateLayout();

  plot()->updateKeyPosition();

  if (plot()->isAutoFit())
    plot()->setNeedsAutoFit(true);

  redraw(queued);
}

void
CQChartsPlotKey::
updatePlotLocation()
{
  auto bbox = plot()->calcDataRange();
  if (! bbox.isSet()) return;

  if (! isInsideX())
    bbox += plot()->calcGroupedYAxisRange(CQChartsAxisSide::Type::NONE);

  if (! isInsideY())
    bbox += plot()->calcGroupedXAxisRange(CQChartsAxisSide::Type::NONE);

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

  //auto *xAxis = drawPlot->xAxis();
  //auto *yAxis = drawPlot->yAxis();

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

      location_.setType(Location::Type::ABSOLUTE_POSITION);

      setAbsolutePlotPosition(Point(kx, ky));
    }
    else
      location_.setType(Location::Type::TOP_RIGHT);
  }

  if      (location().onLeft()) {
    if (isInsideX())
      kx = bbox.getXMin() + xlm + xlp;
    else {
      kx = bbox.getXMin() - ks.width() - xrm - xrp;

      // offset by left y axis width
      //if (yAxis)
      //  kx -= plot()->calcGroupedYAxisRange(CQChartsAxisSide::Type::BOTTOM_LEFT).
      //          getOptWidth() + xlm;
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
      //if (yAxis)
      //  kx += plot()->calcGroupedYAxisRange(CQChartsAxisSide::Type::TOP_RIGHT).
      //          getOptWidth() + xrm;
    }
  }

  if      (location().onTop()) {
    if (isInsideY())
      ky = bbox.getYMax() - ytm - ytp;
    else {
      ky = bbox.getYMax() + ks.height() + ytm + ytp;

      // offset by top x axis height
      //if (xAxis)
      //  ky += plot()->calcGroupedXAxisRange(CQChartsAxisSide::Type::TOP_RIGHT).
      //          getOptHeight() + ytm;
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
      //if (xAxis)
      //  ky -= plot()->calcGroupedXAxisRange(CQChartsAxisSide::Type::BOTTOM_LEFT).
      //          getOptHeight() + ybm;
    }
  }

  Point kp(kx, ky);

  //---

  // update location for absolute position/rectangle
  auto locationType = this->location().type();

  if      (locationType == Location::Type::ABSOLUTE_POSITION) {
    kp = absolutePlotPosition();
  }
  else if (locationType == Location::Type::ABSOLUTE_RECTANGLE) {
    auto bbox = absolutePlotRectangle();

    if (bbox.isValid())
      kp = Point(bbox.getUL());
  }

  //---

  setPosition(kp);
}

void
CQChartsPlotKey::
addProperties(PropertyModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &name, const QString &desc, bool hidden=false) {
    auto *item = model->addProperty(path, this, name);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
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

  addStyleProp("hiddenColor", "Color for hidden items");
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

  addHeaderStyleProp("headerTextColor"        , "color"        , "color");
  addHeaderStyleProp("headerTextAlpha"        , "alpha"        , "alpha");
  addHeaderStyleProp("headerTextFont"         , "font"         , "font");
//addHeaderStyleProp("headerTextAngle"        , "angle"        , "angle");
  addHeaderStyleProp("headerTextContrast"     , "contrast"     , "contrast");
  addHeaderStyleProp("headerTextContrastAlpha", "contrastAlpha", "contrast alpha");
  addHeaderStyleProp("headerTextAlign"        , "align"        , "align");
  addHeaderStyleProp("headerTextFormatted"    , "formatted"    , "formatted to fit box");
  addHeaderStyleProp("headerTextScaled"       , "scaled"       , "scaled to box");
  addHeaderStyleProp("headerTextHtml"         , "html"         , "is html");
  addHeaderStyleProp("headerTextClipLength"   , "clipLength"   , "clipped to length");
  addHeaderStyleProp("headerTextClipElide"    , "clipElide"    , "clip elide");

  //---

  // stroke, fill
  addBoxProperties(model, path, "");

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

  addTextStyleProp("textColor"        , "color"        , "color");
  addTextStyleProp("textAlpha"        , "alpha"        , "alpha");
  addTextStyleProp("textFont"         , "font"         , "font");
//addTextStyleProp("textAngle"        , "angle"        , "angle");
  addTextStyleProp("textContrast"     , "contrast"     , "contrast");
  addTextStyleProp("textContrastAlpha", "contrastAlpha", "contrast alpha");
  addTextStyleProp("textAlign"        , "align"        , "align");
  addTextStyleProp("textFormatted"    , "formatted"    , "formatted to fit box");
  addTextStyleProp("textScaled"       , "scaled"       , "scaled to box");
  addTextStyleProp("textHtml"         , "html"         , "is html");
  addTextStyleProp("textClipLength"   , "clipLength"   , "clipped to length");
  addTextStyleProp("textClipElide"    , "clipElide"    , "clip elide");
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

  auto headerStr = calcHeaderStr();

  if (headerStr.length()) {
    // set text options
    CQChartsTextOptions textOptions;

    textOptions.html = isHeaderTextHtml();

    // get font
    auto font = view()->plotFont(plot(), headerTextFont());

    // get text size
    auto ptsize = CQChartsDrawUtil::calcTextSize(headerStr, font, textOptions);

    layoutData_.headerWidth  = drawPlot->pixelToWindowWidth (ptsize.width ()) + 2*xs_;
    layoutData_.headerHeight = drawPlot->pixelToWindowHeight(ptsize.height()) + 2*ys_;
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

  w = std::max(w, layoutData_.headerWidth);

  w += pmargin_.xl + ppadding_.xl + pmargin_.xr + ppadding_.xr;

  for (int r = 0; r < numRows; ++r) {
    auto &cell = rowColCell_[r][0];

    h += cell.height;
  }

  h += pmargin_.yb + ppadding_.yb + pmargin_.yt + ppadding_.yt + layoutData_.headerHeight;

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

  if (! hasBBox())
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

  for (auto *item : items_) {
    auto *group = dynamic_cast<CQChartsGroupKeyItem *>(item);

    if (group) {
      auto *item1 = group->getItemAt(p);

      if (item1)
        return item1;
    }
    else {
      if (item->bbox().inside(p))
        return item;
    }
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

  if (locationType != Location::Type::ABSOLUTE_POSITION &&
      locationType != Location::Type::ABSOLUTE_RECTANGLE) {
    location_ = Location(Location::Type::ABSOLUTE_POSITION);

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

  if (dragSide == CQChartsResizeSide::NONE)
    return false;

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  editHandles()->updateBBox(dx, dy);

  if (dragSide == CQChartsResizeSide::MOVE)
    editDragMove(Point(dx, dy));
  else
    editDragResize(editHandles()->bbox());

  editHandles()->setDragPos(p);

  return true;
}

void
CQChartsPlotKey::
editDragMove(const Point &d)
{
  auto locationType = this->location().type();

  if (locationType != Location::Type::ABSOLUTE_POSITION &&
      locationType != Location::Type::ABSOLUTE_RECTANGLE) {
    location_ = Location(Location::Type::ABSOLUTE_POSITION);

    setAbsolutePlotPosition(position() + d);
  }
  else
    setAbsolutePlotPosition(absolutePlotPosition() + d);

  updatePosition(/*queued*/false);
}

void
CQChartsPlotKey::
editDragResize(const BBox &bbox)
{
  setBBox(bbox);

  location_ = Location(Location::Type::ABSOLUTE_RECTANGLE);

  setAbsolutePlotRectangle(this->bbox());

  auto width  = CQChartsLength::plot(this->bbox().getWidth ());
  auto height = CQChartsLength::plot(this->bbox().getHeight() - layoutData_.headerHeight);

  setScrollWidth (CQChartsOptLength(width ));
  setScrollHeight(CQChartsOptLength(height));

  invalidateLayout();

  updatePosition(/*queued*/false);
}

bool
CQChartsPlotKey::
editMotion(const Point &p)
{
  return editHandles()->selectInside(p);
}

bool
CQChartsPlotKey::
editMoveBy(const Point &f)
{
  auto locationType = this->location().type();

  if (locationType != Location::Type::ABSOLUTE_POSITION &&
      locationType != Location::Type::ABSOLUTE_RECTANGLE) {
    location_ = Location(Location::Type::ABSOLUTE_POSITION);

    setAbsolutePlotPosition(position() + f);
  }

  updatePosition();

  return true;
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

    auto *group = dynamic_cast<CQChartsGroupKeyItem *>(item1);

    if (group) {
      if (group->setInsideItem(item))
        changed = true;

      group->updateInside();
    }
    else {
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
draw(PaintDevice *device) const
{
  auto *th = const_cast<CQChartsPlotKey *>(this);

  if (! plot()->isVisible() || ! isOverlayVisible() || isEmpty()) {
    th->hideScrollBars();
    return;
  }

  if (location().isAuto())
    return;

  //---

  th->doLayout();

  //---

  auto locationType = this->location().type();

  //---

  // calc plot bounding box (full size)
  double x = position().x; // left
  double y = position().y; // top
  double w = layoutData_.size.width ();
  double h = layoutData_.size.height();

  if (locationType == Location::Type::ABSOLUTE_RECTANGLE) {
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
  if (locationType != Location::Type::ABSOLUTE_RECTANGLE) {
    th->setBBox(BBox(x, y - h, x + w, y));
  }
  else {
    auto bbox = absolutePlotRectangle();

    if (bbox.isValid())
      th->setBBox(bbox);
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
      th->hideScrollBars();
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

  auto *pdevice = dynamic_cast<CQChartsPlotPaintDevice *>(device);
  auto *painter = (pdevice ? pdevice->painter() : nullptr);
  if (! painter) return;

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
    addScrollBars(plot()->view());

    //---

    scrollData_.vbar->show();

    scrollData_.vbar->move(int(p2.x - scrollData_.pixelBarSize - 1), int(p1.y + phh));
    scrollData_.vbar->resize(int(scrollData_.pixelBarSize - 2), int(p2.y - p1.y - phh - hsph - 1));

    //---

    // count number of rows in height
    scrollData_.vrows = 0;
    scrollData_.vsum  = 0;

    double scrollHeight = sh - pmargin_.yb - ppadding_.yb -
                               pmargin_.yt - ppadding_.yt - layoutData_.hbarHeight;

    for (int i = 0; i < numRows; ++i) {
      double rh = rowHeights_[i] + 2*ys_;

      if (rh <= scrollHeight) {
        ++scrollData_.vrows;

        scrollHeight -= rh;
      }

      scrollData_.vsum += rh;
    }

    //---

    // update scroll bar
    if (scrollData_.vbar->pageStep() != scrollData_.vrows)
      scrollData_.vbar->setPageStep(scrollData_.vrows);

    scrollData_.vmax = std::max(numRows - scrollData_.vrows, 1);

    if (scrollData_.vbar->maximum() != scrollData_.vmax)
      scrollData_.vbar->setRange(0, scrollData_.vmax);

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
    addScrollBars(plot()->view());

    //---

    scrollData_.hbar->show();

    scrollData_.hbar->move(int(p1.x + 1), int(p2.y - scrollData_.pixelBarSize - 1));
    scrollData_.hbar->resize(int(p2.x - p1.x - vspw), int(scrollData_.pixelBarSize - 2));

    //---

    // update scroll bar
    auto pageStep  = CMathRound::Round(drawPlot->windowToPixelWidth(sw - layoutData_.vbarWidth));
    auto fullWidth = CMathRound::Round(drawPlot->windowToPixelWidth(layoutData_.fullSize.width()));

    if (scrollData_.hbar->pageStep() != pageStep)
      scrollData_.hbar->setPageStep(pageStep);

    scrollData_.hmax = std::max(fullWidth - pageStep, 1);

    if (scrollData_.hbar->maximum() != scrollData_.hmax)
      scrollData_.hbar->setRange(0, scrollData_.hmax);

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
    if      (locationType != Location::Type::ABSOLUTE_POSITION &&
             locationType != Location::Type::ABSOLUTE_RECTANGLE) {
      if (isInsideX()) {
        clipRect.setXMin(dataRect.getXMin());
        clipRect.setXMax(dataRect.getXMax());
      }

      if (isInsideY()) {
        clipRect.setYMin(dataRect.getYMin());
        clipRect.setYMax(dataRect.getYMax());
      }
    }
    else if (locationType == Location::Type::ABSOLUTE_RECTANGLE) {
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
  auto headerStr = calcHeaderStr();

  if (headerStr.length()) {
    // set text options
    auto textOptions = this->headerTextOptions();

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
  //double ybm = drawPlot->lengthPlotHeight(margin().bottom());

    double pytm = drawPlot->windowToPixelHeight(ytm);

    // get internal padding
  //double xlp = drawPlot->lengthPlotWidth (padding().left  ());
  //double xrp = drawPlot->lengthPlotWidth (padding().right ());
  //double ytp = drawPlot->lengthPlotHeight(padding().top   ());
  //double ybp = drawPlot->lengthPlotHeight(padding().bottom());

    //---

    // calc text rect
    auto ptsize = CQChartsDrawUtil::calcTextSize(headerStr, font, textOptions);

    double tw = pw;
    double th = ptsize.height() + 2*spacing();

    BBox trect(p.x, p.y + pytm, p.x + tw, p.y + pytm + th);

    //---

    // set text pen
    CQChartsPenBrush tPenBrush;

    auto tc = interpHeaderTextColor(ColorInd());

    plot()->setPen(tPenBrush, PenData(true, tc, headerTextAlpha()));

    bool updateState = device->isInteractive();

    if (updateState)
      plot()->updateObjPenBrushState(this, tPenBrush);

    //---

    // draw text
    device1.setPen(tPenBrush.pen);

    CQChartsDrawUtil::drawTextInBox(&device1, drawPlot->pixelToWindow(trect),
                                    headerStr, textOptions);
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

    if (this->bbox().overlaps(bbox)) {
      item->draw(&device1, bbox);

      if (plot()->showBoxes())
        drawPlot->drawWindowColorBox(&device1, bbox);
    }
  }

  //---

  // draw box
  if (plot()->showBoxes()) {
    drawPlot->drawWindowColorBox(&device1, this->bbox());

    BBox headerBox(x, y - layoutData_.headerHeight, x + sw, y);

    drawPlot->drawWindowColorBox(&device1, headerBox);
  }

  //---

  device1.restore();
}

void
CQChartsPlotKey::
drawEditHandles(PaintDevice *device) const
{
  assert(plot()->view()->mode() == CQChartsView::Mode::EDIT || isSelected());

  setEditHandlesBBox();

  editHandles()->draw(device);
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
  return (scrollData_.height.isSet() || scrollData_.width.isSet() ||
          layoutData_.vscrolled || layoutData_.hscrolled);
}

QColor
CQChartsPlotKey::
calcHiddenColor(const QColor &c) const
{
  QColor c1;

  if (hiddenColor().isValid()) {
    charts()->setContrastColor(c);

    c1 = view()->interpColor(hiddenColor(), ColorInd());

    charts()->resetContrastColor();
  }
  else
    c1 = CQChartsUtil::blendColors(c, interpBgColor(), hiddenAlpha().value());

  return c1;
}

QColor
CQChartsPlotKey::
interpBgColor() const
{
  if (isFilled())
    return interpFillColor(ColorInd());

  auto locationType = this->location().type();

  if (locationType != Location::Type::ABSOLUTE_POSITION &&
      locationType != Location::Type::ABSOLUTE_RECTANGLE) {
    if      (isInsideX() && isInsideY()) {
      if (plot()->isDataFilled())
        return plot()->interpDataFillColor(ColorInd());
    }
    else if (isInsideX()) {
      if (locationType == Location::Type::CENTER_LEFT ||
          locationType == Location::Type::CENTER_CENTER ||
          locationType == Location::Type::CENTER_RIGHT) {
        if (plot()->isDataFilled())
          return plot()->interpDataFillColor(ColorInd());
      }
    }
    else if (isInsideY()) {
      if (locationType == Location::Type::TOP_CENTER ||
          locationType == Location::Type::CENTER_CENTER ||
          locationType == Location::Type::BOTTOM_CENTER) {
        if (plot()->isDataFilled())
          return plot()->interpDataFillColor(ColorInd());
      }
    }
  }

  if (plot()->isPlotFilled())
    return plot()->interpPlotFillColor(ColorInd());

  return plot()->interpThemeColor(ColorInd());
}

//---

void
CQChartsPlotKey::
write(const CQPropertyViewModel *propertyModel, const QString &plotName, std::ostream &os)
{
  CQPropertyViewModel::NameValues nameValues;

  propertyModel->getChangedNameValues(this, nameValues, /*tcl*/true);

  for (const auto &nv : nameValues) {
    QString str;

    if (! CQChartsVariant::toString(nv.second, str))
      str.clear();

    os << "set_charts_property -plot $" << plotName.toStdString();

    os << " -name " << nv.first.toStdString() << " -value {" << str.toStdString() << "}\n";
  }
}

//------

CQChartsColumnKey::
CQChartsColumnKey(CQChartsPlot *plot, const Column &column) :
 CQChartsPlotKey(plot), column_(column)
{
  plot->addFilterColumn(column_);

  updateItems();
}

CQChartsColumnKey::
~CQChartsColumnKey()
{
  plot()->removeFilterColumn(column_);
}

void
CQChartsColumnKey::
setColumn(const Column &c)
{
  if (c != column_) {
    plot()->removeFilterColumn(column_);

    column_ = c;

    plot()->addFilterColumn(column_);

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

  auto addKeyRow = [&](const QString &name, const Column &column, int i, int n) {
    auto *checkItem = new CQChartsColumnCheckKeyItem(this, column, i, n);
    auto *textItem  = new CQChartsTextKeyItem       (this, name, ColorInd());

    auto *groupItem = new CQChartsGroupKeyItem(this);

    groupItem->addRowItems(checkItem, textItem);

    addItem(groupItem, row, col);

    nextRowCol(row, col);

    return std::pair<CQChartsCheckKeyItem *, CQChartsTextKeyItem *>(checkItem, textItem);
  };

  clearItems();

  auto *details = plot()->columnDetails(column_);
  if (! details) return;

  int ig = 0;
  int ng = details->numUnique();

  for (const auto &value : details->uniqueValues())
    addKeyRow(value.toString(), column_, ig++, ng);
}

void
CQChartsColumnKey::
addProperties(PropertyModel *model, const QString &path, const QString &desc)
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
CQChartsKeyItem(PlotKey *key, const ColorInd &ic, Plot *plot) :
 key_(key), ic_(ic), plot_(plot)
{
  assert(key_);
}

CQChartsPlot *
CQChartsKeyItem::
plot() const
{
  return plot_.data();
}

bool
CQChartsKeyItem::
selectPress(const Point &, SelData &selData)
{
  if (! isClickable())
    return false;

  if      (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SHOW)
    doShow(selData.selMod);
  else if (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SELECT)
    doSelect(selData.selMod);

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
  auto *keyPlot = key_->plot();

  auto ic = setIndex();

  // no modifiers : toggle clicked
  if      (selMod == SelMod::REPLACE) {
    setSetHidden(! isSetHidden());
  }
  // control: clicked item only
  else if (selMod == SelMod::ADD) {
    for (int i = 0; i < ic.n; ++i)
      keyPlot->setSetHidden(i, i != ic.i);
  }
  // shift: unclicked items
  else if (selMod == SelMod::REMOVE) {
    for (int i = 0; i < ic.n; ++i)
      keyPlot->setSetHidden(i, i == ic.i);
  }
  // control+shift: toggle all on/off
  else if (selMod == SelMod::TOGGLE) {
    // unhide all if some hidden
    int num_hidden = 0;

    for (int i = 0; i < ic.n; ++i)
      num_hidden += keyPlot->isSetHidden(i);

    bool state = (num_hidden > 0 ? false : true);

    for (int i = 0; i < ic.n; ++i)
      keyPlot->setSetHidden(i, state);
  }
}

void
CQChartsKeyItem::
adjustFillColor(QColor &c) const
{
  bool hidden      = calcHidden();
  bool groupHidden = (group_ && group_->calcHidden());

  if (hidden || groupHidden)
    c = key_->calcHiddenColor(c);
}

bool
CQChartsKeyItem::
calcHidden() const
{
  return isSetHidden();
}

void
CQChartsKeyItem::
setHidden(bool hidden)
{
  setSetHidden(hidden);
}

bool
CQChartsKeyItem::
isClicked() const
{
  if      (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SHOW)
    return ! isSetHidden();
  else if (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SELECT)
    return false;
  else
    return false;
}

bool
CQChartsKeyItem::
isSetHidden() const
{
  auto *keyPlot = key_->plot();

  return keyPlot->isSetHidden(setIndex().i);
}

void
CQChartsKeyItem::
setSetHidden(bool b)
{
  auto *keyPlot = key_->plot();

  keyPlot->CQChartsPlot::setSetHidden(setIndex().i, b);
}

void
CQChartsKeyItem::
doSelect(SelMod)
{
  // TODO:
}

bool
CQChartsKeyItem::
tipText(const Point &, QString &) const
{
  return false;
}

//------

CQChartsGroupKeyItem::
CQChartsGroupKeyItem(Plot *plot) :
 CQChartsKeyItem(plot->key(), ColorInd(), plot)
{
}

CQChartsGroupKeyItem::
CQChartsGroupKeyItem(PlotKey *key) :
 CQChartsKeyItem(key, ColorInd(), key->plot())
{
}

CQChartsGroupKeyItem::
~CQChartsGroupKeyItem()
{
  for (auto &item : items_)
    delete item;
}

void
CQChartsGroupKeyItem::
setKey(PlotKey *key)
{
  CQChartsKeyItem::setKey(key);

  for (auto &item : items_)
    item->setKey(key);
}

void
CQChartsGroupKeyItem::
addRowItems(KeyItem *litem, KeyItem *ritem)
{
  if (key()->isFlipped())
    std::swap(litem, ritem);

  addItem(litem);
  addItem(ritem);

  litem->setCol(0);
  ritem->setCol(1);
}

void
CQChartsGroupKeyItem::
addItem(KeyItem *item)
{
  if (item->group())
    const_cast<CQChartsGroupKeyItem *>(item->group())->removeItem(item, /*keep*/true);

  items_.push_back(item);

  item->setGroup(this);

  item->setKey(const_cast<PlotKey *>(this->key()));
}

void
CQChartsGroupKeyItem::
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
CQChartsGroupKeyItem::
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
CQChartsGroupKeyItem::
tipText(const Point &p, QString &tip) const
{
  for (auto &item : items_) {
    if (item->tipText(p, tip))
      return true;
  }

  return false;
}

bool
CQChartsGroupKeyItem::
calcHidden() const
{
  for (const auto &item : items_) {
    if (item->calcHidden())
      return true;
  }

  return false;
}

//---

CQChartsKeyItem *
CQChartsGroupKeyItem::
getItemAt(const Point &p) const
{
  for (auto *item : items_) {
    auto *group = dynamic_cast<CQChartsGroupKeyItem *>(item);

    if (group) {
      auto *item1 = group->getItemAt(p);

      if (item1)
        return item1;
    }
    else {
      if (item->bbox().inside(p))
        return item;
    }
  }

  return nullptr;
}

bool
CQChartsGroupKeyItem::
setInsideItem(CQChartsKeyItem *item)
{
  bool changed = false;

  for (auto &item1 : items_) {
    if (! item1) continue;

    auto *group = dynamic_cast<CQChartsGroupKeyItem *>(item1);

    if (group) {
      if (group->setInsideItem(item))
        changed = true;

      group->updateInside();
    }
    else {
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
  }

  return changed;
}

void
CQChartsGroupKeyItem::
updateInside()
{
  bool inside = false;

  for (auto &item1 : items_) {
    if (! item1) continue;

    auto *group = dynamic_cast<CQChartsGroupKeyItem *>(item1);

    if (group)
      updateInside();

    if (item1->isInside())
      inside = true;
  }

  setInside(inside);
}

//---

bool
CQChartsGroupKeyItem::
selectPress(const Point &p, SelData &selData)
{
  for (auto &item : items_) {
    if (item->selectPress(p, selData))
      return true;
  }

  return false;
}

bool
CQChartsGroupKeyItem::
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
CQChartsGroupKeyItem::
doShow(SelMod selMod)
{
  for (auto &item : items_)
    item->doShow(selMod);
}

void
CQChartsGroupKeyItem::
doSelect(SelMod selMod)
{
  for (auto &item : items_)
    item->doSelect(selMod);
}

//---

void
CQChartsGroupKeyItem::
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

    item->setBBox(rect1);

    x += s.width();
  }
}

//------

CQChartsTextKeyItem::
CQChartsTextKeyItem(Plot *plot, const QString &text, const ColorInd &ic) :
 CQChartsKeyItem(plot->key(), ic, plot), text_(text)
{
}

CQChartsTextKeyItem::
CQChartsTextKeyItem(PlotKey *key, const QString &text, const ColorInd &ic) :
 CQChartsKeyItem(key, ic, key->plot()), text_(text)
{
}

CQChartsGeom::Size
CQChartsTextKeyItem::
size() const
{
  auto *keyPlot  = key_->plot();
  auto *drawPlot = key_->drawPlot();

  auto font = keyPlot->view()->plotFont(keyPlot, key_->textFont());

  QFontMetricsF fm(font);

  double clipLength = drawPlot->lengthPixelWidth(key_->textClipLength());
  auto   clipElide  = key_->textClipElide();

  auto text = CQChartsDrawUtil::clipTextToLength(text_, font, clipLength, clipElide);

  double w = fm.horizontalAdvance(text);
  double h = fm.height();

  double ww = drawPlot->pixelToWindowWidth (w + 4);
  double wh = drawPlot->pixelToWindowHeight(h + 4);

  return Size(ww, wh);
}

QColor
CQChartsTextKeyItem::
interpTextColor(const ColorInd &ind) const
{
  return key_->interpTextColor(ind);
}

void
CQChartsTextKeyItem::
draw(PaintDevice *device, const BBox &rect) const
{
  auto *keyPlot  = key_->plot();
//auto *drawPlot = key_->drawPlot();

  //---

  keyPlot->setPainterFont(device, key_->textFont());

  auto tc = interpTextColor(ColorInd());

  bool inside      = (! calcHidden() && isInside());
  bool groupInside = (group_ && ! group_->calcHidden() && group_->isInside());

  if (inside || groupInside)
    tc = keyPlot->insideColor(tc);

  adjustFillColor(tc);

  device->setPen(tc);

  auto textOptions = key_->textOptions();

  textOptions = keyPlot->adjustTextOptions(textOptions);

  CQChartsDrawUtil::drawTextInBox(device, rect, text_, textOptions);
}

//------

CQChartsColorBoxKeyItem::
CQChartsColorBoxKeyItem(Plot *plot, const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
                        const RangeValue &xv, const RangeValue &yv) :
 CQChartsKeyItem(plot->key(), iv, plot), is_(is), ig_(ig), iv_(iv), xv_(xv), yv_(yv)
{
  assert(is_.isValid());
  assert(ig_.isValid());
  assert(iv_.isValid());

  setClickable(true);
}

CQChartsColorBoxKeyItem::
CQChartsColorBoxKeyItem(PlotKey *key, const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
                        const RangeValue &xv, const RangeValue &yv) :
 CQChartsKeyItem(key, iv, key->plot()), is_(is), ig_(ig), iv_(iv), xv_(xv), yv_(yv)
{
  assert(is_.isValid());
  assert(ig_.isValid());
  assert(iv_.isValid());

  setClickable(true);
}

QColor
CQChartsColorBoxKeyItem::
interpStrokeColor(const ColorInd &ic) const
{
  return plot()->interpColor(strokeColor(), ic);
}

CQChartsGeom::Size
CQChartsColorBoxKeyItem::
size() const
{
  auto *keyPlot  = key_->plot();
  auto *drawPlot = key_->drawPlot();

  auto font = keyPlot->view()->plotFont(keyPlot, key_->textFont());

  QFontMetricsF fm(font);

  double h = fm.height();

  double ww = drawPlot->pixelToWindowWidth (h + 2);
  double wh = drawPlot->pixelToWindowHeight(h + 2);

  return Size(ww, wh);
}

bool
CQChartsColorBoxKeyItem::
selectPress(const Point &w, SelData &selData)
{
  if (! value_.isValid())
    return CQChartsKeyItem::selectPress(w, selData);

  if (! isClickable())
    return false;

  if      (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SHOW) {
    auto *keyPlot = key_->plot();

    if (! plot_->isHideValue(value_))
      keyPlot->setHideValue(value_);
    else
      keyPlot->setHideValue(QVariant());

    keyPlot->updateRangeAndObjs();
  }
  else if (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SELECT) {
  }

  return true;
}

QVariant
CQChartsColorBoxKeyItem::
drawValue() const
{
  auto brush = fillBrush();

  return QVariant(brush.color());
}

void
CQChartsColorBoxKeyItem::
draw(PaintDevice *device, const BBox &rect) const
{
  auto *keyPlot  = key_->plot();
  auto *drawPlot = key_->drawPlot();

  auto prect = drawPlot->windowToPixel(rect);

  bool swapped;
  auto prect1 = prect.adjusted(2, 2, -2, -2, swapped);
  if (swapped) return;

  //---

  CQChartsPenBrush penBrush;

  penBrush.pen   = strokePen();
  penBrush.brush = fillBrush();

  //bool inside = (! calcHidden() && isInside());
  bool inside = isInside();

  if (inside)
    penBrush.brush.setColor(keyPlot->insideColor(penBrush.brush.color()));

  auto bbox = drawPlot->pixelToWindow(prect1);

  CQChartsDrawUtil::drawRoundedRect(device, penBrush, bbox, cornerRadius());
}

QBrush
CQChartsColorBoxKeyItem::
fillBrush() const
{
  auto *keyPlot = key_->plot();

  auto ic = calcColorInd();

  QColor c;

  if (color_.isValid())
    c = keyPlot->interpColor(color_, ic);
  else
    c = keyPlot->interpPaletteColor(ic);

  adjustFillColor(c);

  return c;
}

bool
CQChartsColorBoxKeyItem::
calcHidden() const
{
  bool hidden = false;

  if (value_.isValid())
    hidden = plot()->isHideValue(value_);
  else {
    auto ic = calcColorInd();

    hidden = plot()->isSetHidden(ic.i);
  }

  return hidden;
}

QPen
CQChartsColorBoxKeyItem::
strokePen() const
{
  auto ic = calcColorInd();

  return interpStrokeColor(ic);
}

CQChartsColorBoxKeyItem::ColorInd
CQChartsColorBoxKeyItem::
calcColorInd() const
{
  return plot()->calcColorInd(nullptr, this, is_, ig_, iv_);
}

double
CQChartsColorBoxKeyItem::
xColorValue(bool relative) const
{
  return (relative ? xv_.map() : xv_.v);
}

double
CQChartsColorBoxKeyItem::
yColorValue(bool relative) const
{
  return (relative ? yv_.map() : yv_.v);
}

//------

CQChartsLineKeyItem::
CQChartsLineKeyItem(Plot *plot, const ColorInd &is, const ColorInd &ig) :
 CQChartsKeyItem(plot->key(), is.n > 1 ? is : ig), is_(is), ig_(ig)
{
  setClickable(true);
}

CQChartsLineKeyItem::
CQChartsLineKeyItem(PlotKey *key, const ColorInd &is, const ColorInd &ig) :
 CQChartsKeyItem(key, is.n > 1 ? is : ig), is_(is), ig_(ig)
{
  setClickable(true);
}

CQChartsGeom::Size
CQChartsLineKeyItem::
size() const
{
  auto *keyPlot  = key_->plot();
  auto *drawPlot = key_->drawPlot();

  auto font = keyPlot->view()->plotFont(keyPlot, key_->textFont());

  QFontMetricsF fm(font);

  double w = fm.horizontalAdvance("-X-");
  double h = fm.height();

  double ww = drawPlot->pixelToWindowWidth (w + 8);
  double wh = drawPlot->pixelToWindowHeight(h + 2);

  return Size(ww, wh);
}

bool
CQChartsLineKeyItem::
selectPress(const Point &w, SelData &selData)
{
  if (! value_.isValid())
    return CQChartsKeyItem::selectPress(w, selData);

  if (! isClickable())
    return false;

  if      (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SHOW) {
    auto *keyPlot = key_->plot();

    if (! keyPlot->isHideValue(value_))
      keyPlot->setHideValue(value_);
    else
      keyPlot->setHideValue(QVariant());

    keyPlot->updateRangeAndObjs();
  }
  else if (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SELECT) {
  }

  return true;
}

void
CQChartsLineKeyItem::
draw(PaintDevice *device, const BBox &rect) const
{
  auto *keyPlot  = key_->plot();
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

  auto lc = keyPlot->interpColor(fillData  .color(), ig_);
  auto fc = keyPlot->interpColor(strokeData.color(), ig_);

  CQChartsPenBrush penBrush;

  keyPlot->setPenBrush(penBrush, PenData(true, lc), BrushData(true, fc));

  bool inside      = (! calcHidden() && isInside());
  bool groupInside = (group_ && ! group_->calcHidden() && group_->isInside());

  if (inside || groupInside)
    penBrush.brush.setColor(keyPlot->insideColor(penBrush.brush.color()));

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  device->drawLine(drawPlot->pixelToWindow(Point(x1, y)), drawPlot->pixelToWindow(Point(x2, y)));

  Point ps(CMathUtil::avg(x1, x2), y);

  if (symbolData_.symbol().isValid())
    CQChartsDrawUtil::drawSymbol(device, penBrush, symbolData_.symbol(),
                                 drawPlot->pixelToWindow(ps), symbolData_.size());
}

//------

CQChartsGradientKeyItem::
CQChartsGradientKeyItem(Plot *plot) :
 CQChartsKeyItem(plot->key(), ColorInd(), plot)
{
}

CQChartsGradientKeyItem::
CQChartsGradientKeyItem(PlotKey *key) :
 CQChartsKeyItem(key, ColorInd(), key->plot())
{
}

CQChartsGeom::Size
CQChartsGradientKeyItem::
size() const
{
  // get char width/height
  auto font = plot()->view()->plotFont(plot(), key_->textFont());

  QFontMetricsF fm(font);

  double fw = fm.horizontalAdvance("X");
  double fh = fm.height();

  //---

  // calc max label width
  QStringList labels;

  calcLabels(labels);

  double tw = 0.0;

  for (const auto &label : labels)
    tw = std::max(tw, fm.horizontalAdvance(label));

  //--

  double ww = plot()->pixelToWindowWidth (2*fw + tw + 6);
  double wh = plot()->pixelToWindowHeight(7*fh + fh + 4);

  return Size(ww, wh);
}

void
CQChartsGradientKeyItem::
draw(PaintDevice *device, const BBox &rect) const
{
  // get char height
  plot()->setPainterFont(device, key_->textFont());

  QFontMetricsF fm(device->font());

  double fh = fm.height();

  //---

  // calc max label width
  QStringList labels;

  calcLabels(labels);

  double tw = 0.0;

  for (const auto &label : labels)
    tw = std::max(tw, fm.horizontalAdvance(label));

  double wtw = plot()->pixelToWindowWidth(tw);

  //---

  // calc margins
  double wxm = plot()->pixelToWindowWidth (2);
  double wym = plot()->pixelToWindowHeight(fh/2 + 2);

  //---

  // calc left/right boxes
  BBox lrect(rect.getXMin() + wxm, rect.getYMin() + wym,
             rect.getXMax() - wtw - 2*wxm, rect.getYMax() - wym);
  BBox rrect(rect.getXMax() - wtw - wxm, rect.getYMin() + wym,
             rect.getXMax() - wxm, rect.getYMax() - wym);

  auto lprect = plot()->windowToPixel(lrect);
  auto rprect = plot()->windowToPixel(rrect);

  //---

  // draw gradient in left box
  Point pg1(lprect.getXMin(), lprect.getYMax());
  Point pg2(lprect.getXMin(), lprect.getYMin());

  QLinearGradient lg(pg1.x, pg1.y, pg2.x, pg2.y);

  CQColorsPalette *colorsPalette = nullptr;

  if (palette().isValid())
    colorsPalette = palette().palette();
  else
    colorsPalette = plot()->view()->themePalette();

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

  auto tc = plot()->interpThemeColor(ColorInd(1.0));

  plot()->setPen(penBrush, PenData(true, tc, Alpha()));

  device->setPen(penBrush.pen);

  //---

  // draw labels
  auto drawTextLabel = [&](const Point &p, const QString &label) {
    auto p1 = plot()->pixelToWindow(p);

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

  labels.push_back(QString::number(n1));
  labels.push_back(QString::number(n2));
  labels.push_back(QString::number(n3));
  labels.push_back(QString::number(n4));
  labels.push_back(QString::number(n5));
}

//------

CQChartsCheckKeyItem::
CQChartsCheckKeyItem(Plot *plot, const ColorInd &is, const ColorInd &ig, const ColorInd &iv) :
 CQChartsKeyItem(plot->key(), iv, plot), is_(is), ig_(ig), iv_(iv)
{
  assert(is_.isValid());
  assert(ig_.isValid());
  assert(iv_.isValid());

  setClickable(true);
}

CQChartsCheckKeyItem::
CQChartsCheckKeyItem(PlotKey *key, const ColorInd &is, const ColorInd &ig, const ColorInd &iv) :
 CQChartsKeyItem(key, iv, key->plot()), is_(is), ig_(ig), iv_(iv)
{
  assert(is_.isValid());
  assert(ig_.isValid());
  assert(iv_.isValid());

  setClickable(true);
}

CQChartsGeom::Size
CQChartsCheckKeyItem::
size() const
{
  // get char width/height
  auto font = plot()->view()->plotFont(plot(), key_->textFont());

  QFontMetricsF fm(font);

  double fw = fm.horizontalAdvance("X") + 4;
  double fh = fm.height() + 4;

  auto ps = std::max(fw, fh);

  double ww = plot()->pixelToWindowWidth (ps + 4);
  double wh = plot()->pixelToWindowHeight(ps + 4);

  return Size(ww, wh);
}

bool
CQChartsCheckKeyItem::
selectPress(const Point &w, SelData &selData)
{
  if (! isClickable())
    return false;

  if      (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SHOW) {
    auto *keyPlot = key_->plot();

    setHidden(! calcHidden());

    keyPlot->updateRangeAndObjs();
  }
  else if (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SELECT) {
    if (! value_.isValid())
      return CQChartsKeyItem::selectPress(w, selData);
  }

  return true;
}

void
CQChartsCheckKeyItem::
setHidden(bool hidden)
{
  if (value_.isValid()) {
    if (hidden)
      plot()->setHideValue(value_);
    else
      plot()->setHideValue(QVariant());
  }
  else {
    auto ic = calcColorInd();

    plot()->setSetHidden(ic.i, hidden);
  }
}

void
CQChartsCheckKeyItem::
draw(PaintDevice *device, const BBox &rect) const
{
  // get char height
  plot()->setPainterFont(device, key_->textFont());

  QFontMetricsF fm(device->font());

  double fw = fm.horizontalAdvance("X") + 4;
  double fh = fm.height() + 4;

  //---

  auto prect = device->windowToPixel(rect);

  auto ps = std::max(fw, fh);
  auto px = prect.getXMid() - ps/2;
  auto py = prect.getYMid() - ps/2;

  //device->setPen(interpTextColor(ColorInd()));

  bool checked = ! calcHidden();

  CQChartsDrawUtil::drawCheckBox(device, int(px), int(py), int(ps), checked);
}

bool
CQChartsCheckKeyItem::
calcHidden() const
{
  bool hidden = false;

  if (value_.isValid())
    hidden = plot()->isHideValue(value_);
  else {
    auto ic = calcColorInd();

    hidden = plot()->isSetHidden(ic.i);
  }

  return hidden;
}

//------

CQChartsColumnCheckKeyItem::
CQChartsColumnCheckKeyItem(PlotKey *key, const Column &column, int i, int n) :
 CQChartsCheckKeyItem(key, ColorInd(), ColorInd(), ColorInd()), column_(column), i_(i), n_(n)
{
}

bool
CQChartsColumnCheckKeyItem::
calcHidden() const
{
  auto *details = plot()->columnDetails(column_);
  if (! details) return false;

  auto value = details->uniqueValue(i_);

  return ! plot()->isColumnValueVisible(column_, value);
}

void
CQChartsColumnCheckKeyItem::
setHidden(bool hidden)
{
  auto *details = plot()->columnDetails(column_);
  if (! details) return;

  auto value = details->uniqueValue(i_);

  plot()->setColumnValueVisible(column_, value, ! hidden);

  QString str;
  CQChartsVariant::toString(value, str);
  std::cerr << str.toStdString() << " = " << hidden << "\n";
}
