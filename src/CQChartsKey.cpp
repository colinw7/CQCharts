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

void
CQChartsKey::
setHeaderStr(const QString &s)
{
  CQChartsUtil::testAndSet(header_, s, [&]() { updateLayout(); } );
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
  if (location().type() == Location::Type::ABSOLUTE_POSITION)
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
  addTextStyleProp("textAlign"     , "align"     , "text align");
  addTextStyleProp("textFormatted" , "formatted" , "text formatted to fit box");
  addTextStyleProp("textScaled"    , "scaled"    , "text scaled to box");
  addTextStyleProp("textHtml"      , "html"      , "text is html");
  addTextStyleProp("textClipLength", "clipLength", "text clipped to length");
  addTextStyleProp("textClipElide" , "clipElide" , "text clip elide");
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

  cimage.fill(Qt::transparent);

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

    if      (pressBehavior().type() == KeyBehavior::Type::SHOW)
      doShow(i, selMod);
    else if (pressBehavior().type() == KeyBehavior::Type::SELECT)
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
  auto *plot = view()->plot(i);

  View::Plots plots;

  view()->getPlots(plots);

  // no modifiers : toggle clicked
  if      (selMod == SelMod::REPLACE) {
    plot->setVisible(! plot->isVisible());
  }
  // control: clicked plot only
  else if (selMod == SelMod::ADD) {
    for (auto &plot1 : plots)
      plot1->setVisible(plot1 == plot);
  }
  // shift: unclicked plots
  else if (selMod == SelMod::REMOVE) {
    for (auto &plot1 : plots)
      plot1->setVisible(plot1 != plot);
  }
  // control+shift: toggle all on/off
  else if (selMod == SelMod::TOGGLE) {
    // make all visible if some not visible
    int num_visible = 0;

    for (auto &plot1 : plots)
      num_visible += plot1->isVisible();

    bool state = (num_visible != int(plots.size()) ? false : true);

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
  addTextStyleProp("textAlign"     , "align"     , "text align");
  addTextStyleProp("textFormatted" , "formatted" , "text formatted to fit box");
  addTextStyleProp("textScaled"    , "scaled"    , "text scaled to box");
  addTextStyleProp("textHtml"      , "html"      , "text is html");
  addTextStyleProp("textClipLength", "clipLength", "text clipped to length");
  addTextStyleProp("textClipElide" , "clipElide" , "text clip elide");
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

  for (auto *item : items_) {
    auto *group = dynamic_cast<CQChartsKeyItemGroup *>(item);

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

  if (dragSide == CQChartsResizeSide::MOVE) {
    location_ = Location(Location::Type::ABSOLUTE_POSITION);

    setAbsolutePlotPosition(absolutePlotPosition() + Point(dx, dy));
  }
  else {
    location_ = Location(Location::Type::ABSOLUTE_RECTANGLE);

    editHandles()->updateBBox(dx, dy);

    wbbox_ = editHandles()->bbox();

    setAbsolutePlotRectangle(wbbox_);

    auto width  = CQChartsLength::plot(wbbox_.getWidth ());
    auto height = CQChartsLength::plot(wbbox_.getHeight() - layoutData_.headerHeight);

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

  if (locationType != Location::Type::ABSOLUTE_POSITION &&
      locationType != Location::Type::ABSOLUTE_RECTANGLE) {
    location_ = Location(Location::Type::ABSOLUTE_POSITION);

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

    auto *group = dynamic_cast<CQChartsKeyItemGroup *>(item1);

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
    auto pageStep  = CMathRound::Round(drawPlot->windowToPixelWidth(sw - layoutData_.vbarWidth));
    auto fullWidth = CMathRound::Round(drawPlot->windowToPixelWidth(layoutData_.fullSize.width()));

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
  if (headerStr().length()) {
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
    auto *colorItem = new CQChartsColorBoxKeyItem(this, ColorInd(), ColorInd(), ic);
    auto *textItem  = new CQChartsTextKeyItem    (this, name, ic);

    auto *groupItem = new CQChartsKeyItemGroup(this);

    groupItem->addItem(colorItem);
    groupItem->addItem(textItem );

    addItem(groupItem, row, col);

    nextRowCol(row, col);

    return std::pair<CQChartsColorBoxKeyItem *, CQChartsTextKeyItem *>(colorItem, textItem);
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
CQChartsKeyItem(PlotKey *key, const ColorInd &ic) :
 key_(key), ic_(ic)
{
  assert(key_);
}

bool
CQChartsKeyItem::
selectPress(const Point &, SelMod selMod)
{
  if (! isClickable())
    return false;

  if      (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SHOW)
    doShow(selMod);
  else if (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SELECT)
    doSelect(selMod);

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

  auto ic = setIndex();

  // no modifiers : toggle clicked
  if      (selMod == SelMod::REPLACE) {
    setSetHidden(! isSetHidden());
  }
  // control: clicked item only
  else if (selMod == SelMod::ADD) {
    for (int i = 0; i < ic.n; ++i)
      plot->setSetHidden(i, i != ic.i);
  }
  // shift: unclicked items
  else if (selMod == SelMod::REMOVE) {
    for (int i = 0; i < ic.n; ++i)
      plot->setSetHidden(i, i == ic.i);
  }
  // control+shift: toggle all on/off
  else if (selMod == SelMod::TOGGLE) {
    // unhide all if some hidden
    int num_hidden = 0;

    for (int i = 0; i < ic.n; ++i)
      num_hidden += plot->isSetHidden(i);

    bool state = (num_hidden > 0 ? false : true);

    for (int i = 0; i < ic.n; ++i)
      plot->setSetHidden(i, state);
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
  auto *plot = key_->plot();

  return plot->isSetHidden(setIndex().i);
}

void
CQChartsKeyItem::
setSetHidden(bool b)
{
  auto *plot = key_->plot();

  plot->CQChartsPlot::setSetHidden(setIndex().i, b);
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
CQChartsKeyItemGroup::
getItemAt(const Point &p) const
{
  for (auto *item : items_) {
    auto *group = dynamic_cast<CQChartsKeyItemGroup *>(item);

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
CQChartsKeyItemGroup::
setInsideItem(CQChartsKeyItem *item)
{
  bool changed = false;

  for (auto &item1 : items_) {
    if (! item1) continue;

    auto *group = dynamic_cast<CQChartsKeyItemGroup *>(item1);

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
CQChartsKeyItemGroup::
updateInside()
{
  bool inside = false;

  for (auto &item1 : items_) {
    if (! item1) continue;

    auto *group = dynamic_cast<CQChartsKeyItemGroup *>(item1);

    if (group)
      updateInside();

    if (item1->isInside())
      inside = true;
  }

  setInside(inside);
}

//---

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

    item->setBBox(rect1);

    x += s.width();
  }
}

//------

CQChartsTextKeyItem::
CQChartsTextKeyItem(Plot *plot, const QString &text, const ColorInd &ic) :
 CQChartsKeyItem(plot->key(), ic), plot_(plot), text_(text)
{
}

CQChartsTextKeyItem::
CQChartsTextKeyItem(PlotKey *key, const QString &text, const ColorInd &ic) :
 CQChartsKeyItem(key, ic), plot_(key->plot()), text_(text)
{
}

CQChartsGeom::Size
CQChartsTextKeyItem::
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
CQChartsTextKeyItem::
interpTextColor(const ColorInd &ind) const
{
  return key_->interpTextColor(ind);
}

void
CQChartsTextKeyItem::
draw(PaintDevice *device, const BBox &rect) const
{
  auto *plot     = key_->plot();
//auto *drawPlot = key_->drawPlot();

  //---

  plot->setPainterFont(device, key_->textFont());

  auto tc = interpTextColor(ColorInd());

  bool inside      = (! calcHidden() && isInside());
  bool groupInside = (group_ && ! group_->calcHidden() && group_->isInside());

  if (inside || groupInside)
    tc = plot->insideColor(tc);

  adjustFillColor(tc);

  device->setPen(tc);

  auto textOptions = key_->textOptions();

  textOptions = plot->adjustTextOptions(textOptions);

  CQChartsDrawUtil::drawTextInBox(device, rect, text_, textOptions);
}

//------

CQChartsColorBoxKeyItem::
CQChartsColorBoxKeyItem(Plot *plot, const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
                        const RangeValue &xv, const RangeValue &yv) :
 CQChartsKeyItem(plot->key(), iv), plot_(plot), is_(is), ig_(ig), iv_(iv), xv_(xv), yv_(yv)
{
  assert(is_.isValid());
  assert(ig_.isValid());
  assert(iv_.isValid());

  setClickable(true);
}

CQChartsColorBoxKeyItem::
CQChartsColorBoxKeyItem(PlotKey *key, const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
                        const RangeValue &xv, const RangeValue &yv) :
 CQChartsKeyItem(key, iv), plot_(key->plot()), is_(is), ig_(ig), iv_(iv), xv_(xv), yv_(yv)
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
CQChartsColorBoxKeyItem::
selectPress(const Point &w, SelMod selMod)
{
  if (! value_.isValid())
    return CQChartsKeyItem::selectPress(w, selMod);

  if (! isClickable())
    return false;

  if      (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SHOW) {
    auto *plot = key_->plot();

    if (CQChartsVariant::cmp(value_, plot->hideValue()) != 0)
      plot->setHideValue(value_);
    else
      plot->setHideValue(QVariant());

    plot->updateRangeAndObjs();
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

  //bool inside = (! calcHidden() && isInside());
  bool inside = isInside();

  if (inside)
    penBrush.brush.setColor(plot->insideColor(penBrush.brush.color()));

  auto bbox = drawPlot->pixelToWindow(prect1);

  CQChartsDrawUtil::drawRoundedRect(device, penBrush, bbox, cornerRadius());
}

QBrush
CQChartsColorBoxKeyItem::
fillBrush() const
{
  auto *plot = key_->plot();

  auto ic = calcColorInd();

  QColor c;

  if (color_.isValid())
    c = plot_->interpColor(color_, ic);
  else
    c = plot->interpPaletteColor(ic);

  adjustFillColor(c);

  return c;
}

bool
CQChartsColorBoxKeyItem::
calcHidden() const
{
  bool hidden = false;

  if (value_.isValid())
    hidden = (CQChartsVariant::cmp(value_, plot_->hideValue()) == 0);
  else {
    auto ic = calcColorInd();

    hidden = plot_->isSetHidden(ic.i);
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
CQChartsLineKeyItem::
selectPress(const Point &w, SelMod selMod)
{
  if (! value_.isValid())
    return CQChartsKeyItem::selectPress(w, selMod);

  if (! isClickable())
    return false;

  if      (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SHOW) {
    auto *plot = key_->plot();

    if (CQChartsVariant::cmp(value_, plot->hideValue()) != 0)
      plot->setHideValue(value_);
    else
      plot->setHideValue(QVariant());

    plot->updateRangeAndObjs();
  }
  else if (key_->pressBehavior().type() == CQChartsKeyPressBehavior::Type::SELECT) {
  }

  return true;
}

void
CQChartsLineKeyItem::
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

  bool inside      = (! calcHidden() && isInside());
  bool groupInside = (group_ && ! group_->calcHidden() && group_->isInside());

  if (inside || groupInside)
    penBrush.brush.setColor(plot->insideColor(penBrush.brush.color()));

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
  plot_->setPainterFont(device, key_->textFont());

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

  labels.push_back(QString::number(n1));
  labels.push_back(QString::number(n2));
  labels.push_back(QString::number(n3));
  labels.push_back(QString::number(n4));
  labels.push_back(QString::number(n5));
}
