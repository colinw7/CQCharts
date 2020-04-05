#include <CQChartsKey.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsView.h>
#include <CQChartsEditHandles.h>
#include <CQChartsVariant.h>
#include <CQChartsUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

#include <QScrollBar>
#include <QStylePainter>
#include <QStyleOptionSlider>

CQChartsKey::
CQChartsKey(CQChartsView *view) :
 CQChartsBoxObj(view),
 CQChartsObjTextData      <CQChartsKey>(this),
 CQChartsObjHeaderTextData<CQChartsKey>(this)
{
  init();

  setStateColoring(false);

  editHandles_ = new CQChartsEditHandles(view, CQChartsEditHandles::Mode::MOVE);
}

CQChartsKey::
CQChartsKey(CQChartsPlot *plot) :
 CQChartsBoxObj(plot),
 CQChartsObjTextData      <CQChartsKey>(this),
 CQChartsObjHeaderTextData<CQChartsKey>(this)
{
  init();

  setFilled(true);
  setFillAlpha(CQChartsAlpha(0.5));

  editHandles_ = new CQChartsEditHandles(plot, CQChartsEditHandles::Mode::MOVE);
}

void
CQChartsKey::
init()
{
  setObjectName("key");

  setHeaderTextAlign(Qt::AlignHCenter | Qt::AlignVCenter);

  //---

  CQChartsFont font;

  font.decFontSize(4);

  setTextFont(font);
}

CQChartsKey::
~CQChartsKey()
{
  delete editHandles_;
}

QString
CQChartsKey::
calcId() const
{
  if      (view())
    return view()->id() + "/key";
  else if (plot())
    return plot()->id() + "/key";
  else
    return "key";
}

void
CQChartsKey::
setSelected(bool b)
{
  CQChartsUtil::testAndSet(selected_, b, [&]() {
    if      (view()) {
      view()->invalidateObjects();
      view()->invalidateOverlay();

      view()->update();
    }
    else if (plot()) {
      plot()->drawObjs();
    }
  } );
}

void
CQChartsKey::
setLocation(const CQChartsKeyLocation &l)
{
  CQChartsUtil::testAndSet(location_, l, [&]() { updatePosition(); } );
}

//---

void
CQChartsKey::
draw(CQChartsPaintDevice *) const
{
}

//------

CQChartsViewKey::
CQChartsViewKey(CQChartsView *view) :
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
  if (location() == CQChartsKeyLocation::Type::ABSOLUTE_POSITION)
    return;

  //----

  QFont font = view()->viewFont(textFont());

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

  auto p = view()->windowToPixel(CQChartsGeom::Point(x, y));

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
      CQChartsPlot *plot = view()->plot(i);

      QString name = plot->keyText();

      double tw = fm.width(name) + bs + xlm + xrm;

      pw = std::max(pw, tw);

      ph += bs;
    }

    size_ = CQChartsGeom::Size(pw + xlm + xrm, ph + ybm + ytm + (numPlots_ - 1)*2);

    //---

    double pxr = 0.0, pyr = 0.0;

    if      (location().onLeft   ()) pxr = p.x                   + xlm;
    else if (location().onHCenter()) pxr = p.x - size_.width()/2;
    else if (location().onRight  ()) pxr = p.x - size_.width()   - xrm;

    if      (location().onTop    ()) pyr = p.y                    + ytm;
    else if (location().onVCenter()) pyr = p.y - size_.height()/2;
    else if (location().onBottom ()) pyr = p.y - size_.height()   - ybm;

    pposition_ = CQChartsGeom::Point(pxr, pyr);
  }
  else {
    size_      = CQChartsGeom::Size(0.0, 0.0);
    pposition_ = CQChartsGeom::Point(0.0, 0.0);
  }

  wposition_ = view()->pixelToWindow(pposition_);
}

void
CQChartsViewKey::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &name, const QString &desc, bool hidden=false) {
    model->addProperty(path, this, name)->setDesc(desc).setHidden(hidden);
  };

  //---

  addProp("visible"   , "Is visible");
  addProp("selected"  , "Is selected");
  addProp("horizontal", "Draw items horizontally");
  addProp("autoHide"  , "Auto hide key when too large");
  addProp("clipped"   , "Clip key to view");

  addProp("location", "Key placement location");

  addProp("interactive"  , "Key supports click", true);
  addProp("pressBehavior", "Key click behavior", true);

  addProp("hiddenAlpha", "Alpha for hidden items");

  //---

  // header text
  QString headerPath     = path + "/header";
  QString headerTextPath = headerPath + "/text";

  auto addHeaderProp = [&](const QString &name, const QString &alias, const QString &desc) {
    return &(model->addProperty(headerTextPath, this, name, alias)->
              setDesc("Key header text " + desc));
  };

  auto addHeaderStyleProp = [&](const QString &name, const QString &alias, const QString &desc) {
    CQPropertyViewItem *item = addHeaderProp(name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  addHeaderProp("header", "string", "string");

  addHeaderStyleProp("headerTextColor"    , "color"    , "color");
  addHeaderStyleProp("headerTextAlpha"    , "alpha"    , "alpha");
  addHeaderStyleProp("headerTextFont"     , "font"     , "font");
//addHeaderStyleProp("headerTextAngle"    , "angle"    , "angle");
  addHeaderStyleProp("headerTextContrast" , "contrast" , "contrast");
  addHeaderStyleProp("headerTextAlign"    , "align"    , "align");
  addHeaderStyleProp("headerTextFormatted", "formatted", "formatted to fit box");
  addHeaderStyleProp("headerTextScaled"   , "scaled"   , "scaled to box");
  addHeaderStyleProp("headerTextHtml"     , "html"     , "is html");

  //---

  // stroke, fill
  addBoxProperties(model, path, "");

  //---

  // key text
  QString textPath = path + "/text";

  auto addTextProp = [&](const QString &name, const QString &alias, const QString &desc) {
    return &(model->addProperty(textPath, this, name, alias)->setDesc("Key text " + desc));
  };

  auto addTextStyleProp = [&](const QString &name, const QString &alias, const QString &desc) {
    CQPropertyViewItem *item = addTextProp(name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  addTextStyleProp("textColor"    , "color"    , "color");
  addTextStyleProp("textAlpha"    , "alpha"    , "alpha");
  addTextStyleProp("textFont"     , "font"     , "font");
//addTextStyleProp("textAngle"    , "angle"    , "angle");
  addTextStyleProp("textContrast" , "contrast" , "contrast");
  addTextStyleProp("textAlign"    , "align"    , "align");
  addTextStyleProp("textFormatted", "formatted", "formatted to fit box");
  addTextStyleProp("textScaled"   , "scaled"   , "scaled to box");
  addTextStyleProp("textHtml"     , "html"     , "is html");
}

bool
CQChartsViewKey::
contains(const CQChartsGeom::Point &p) const
{
  return pbbox_.inside(p);
}

void
CQChartsViewKey::
draw(CQChartsPaintDevice *device) const
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

  wbbox_ = CQChartsGeom::BBox(px, py - ph, px + pw, py);

  //---

  auto p1 = view()->pixelToWindow(CQChartsGeom::Point(px     , py     ));
  auto p2 = view()->pixelToWindow(CQChartsGeom::Point(px + pw, py + ph));

  pbbox_ = CQChartsGeom::BBox(p1.x, p2.y, p2.x, p1.y);

  //---

  CQChartsGeom::BBox bbox(px, py, px + pw, py + ph);

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

    CQChartsPlot *plot = view()->plot(i);

    bool checked = plot->isVisible();

    //---

    drawCheckBox(device, px1, (py1 + py2)/2.0 - bs/2.0, int(bs), checked);

    //---

    device->setPen(interpTextColor(ColorInd()));

    QString name = plot->keyText();

    double px2 = px1 + bs + xrm;

    //double tw = fm.width(name);

    CQChartsGeom::BBox rect1(px2, py1, px2 + pw - bs - ybm - ytm, py2);

    CQChartsTextOptions textOptions;

    textOptions.align         = textAlign();
    textOptions.contrast      = isTextContrast();
    textOptions.contrastAlpha = textContrastAlpha();
    textOptions.formatted     = isTextFormatted();
    textOptions.scaled        = isTextScaled();
    textOptions.html          = isTextHtml();

    CQChartsDrawUtil::drawTextInBox(device, device->pixelToWindow(rect1), name, textOptions);

    //---

    // save view key item (plot) rect
    auto p1 = view()->pixelToWindow(CQChartsGeom::Point(px     , py1));
    auto p2 = view()->pixelToWindow(CQChartsGeom::Point(px + pw, py2));

    CQChartsGeom::BBox prect(p1.x, p2.y, p2.x, p1.y);

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

  auto *th = const_cast<CQChartsViewKey *>(this);

  th->editHandles_->setMode(CQChartsEditHandles::Mode::MOVE);

  th->editHandles_->setBBox(pbbox_);

  editHandles_->draw(painter);
}

void
CQChartsViewKey::
drawCheckBox(CQChartsPaintDevice *device, double x, double y, int bs, bool checked) const
{
  QImage cimage = CQChartsUtil::initImage(QSize(bs, bs));

  cimage.fill(QColor(0,0,0,0));

  CQChartsGeom::BBox bbox(0, 0, bs, bs);

  QStylePainter spainter(&cimage, view());

  spainter.setPen(interpTextColor(ColorInd()));

  QStyleOptionButton opt;

  opt.initFrom(view());

  opt.rect = bbox.qrect().toRect();

  opt.state |= (checked ? QStyle::State_On : QStyle::State_Off);

  spainter.drawControl(QStyle::CE_CheckBox, opt);

  device->drawImage(CQChartsGeom::Point(x, y), cimage);
}

//------

bool
CQChartsViewKey::
selectPress(const CQChartsGeom::Point &w, CQChartsSelMod selMod)
{
  int n = std::min(view()->numPlots(), int(prects_.size()));

  for (int i = 0; i < n; ++i) {
    if (! prects_[i].inside(w))
      continue;

    if      (pressBehavior() == CQChartsKeyPressBehavior::Type::SHOW)
      doShow(i, selMod);
    else if (pressBehavior() == CQChartsKeyPressBehavior::Type::SELECT)
      doSelect(i, selMod);

    break;
  }

  redraw();

  return true;
}

bool
CQChartsViewKey::
selectMove(const CQChartsGeom::Point &)
{
  return false;
}

bool
CQChartsViewKey::
selectRelease(const CQChartsGeom::Point &)
{
  return false;
}

//------

bool
CQChartsViewKey::
editPress(const CQChartsGeom::Point &w)
{
  editHandles_->setDragPos(w);

  return true;
}

bool
CQChartsViewKey::
editMove(const CQChartsGeom::Point &w)
{
  const auto &dragPos = editHandles_->dragPos();

  double dx = w.x - dragPos.x;
  double dy = w.y - dragPos.y;

  location_ = CQChartsKeyLocation::Type::ABSOLUTE_POSITION;

  wposition_ = wposition_ + CQChartsGeom::Point(dx, dy);
  pposition_ = view()->windowToPixel(wposition_);

  editHandles_->setDragPos(w);

  updatePosition(/*queued*/false);

  return true;
}

bool
CQChartsViewKey::
editMotion(const CQChartsGeom::Point &w)
{
  return editHandles_->selectInside(w);
}

bool
CQChartsViewKey::
editRelease(const CQChartsGeom::Point &)
{
  return true;
}

//------

void
CQChartsViewKey::
doShow(int i, CQChartsSelMod selMod)
{
  CQChartsPlot *plot = view()->plot(i);

  if      (selMod == CQChartsSelMod::REPLACE) {
    CQChartsView::Plots plots;

    view()->getPlots(plots);

    for (auto &plot1 : plots)
      plot1->setVisible(plot1 == plot);
  }
  else if (selMod == CQChartsSelMod::ADD)
    plot->setVisible(true);
  else if (selMod == CQChartsSelMod::REMOVE)
    plot->setVisible(false);
  else if (selMod == CQChartsSelMod::TOGGLE)
    plot->setVisible(! plot->isVisible());
}

void
CQChartsViewKey::
doSelect(int, CQChartsSelMod)
{
}

void
CQChartsViewKey::
redraw(bool /*queued*/)
{
  view()->invalidateObjects();
  view()->invalidateOverlay();

  view()->update();
}

//------

CQChartsPlotKey::
CQChartsPlotKey(CQChartsPlot *plot) :
 CQChartsKey(plot)
{
  autoHide_ = false;

  //---

  setStroked(true);

  clearItems();

  //---

  // create scroll bar
  scrollData_.hbar = new QScrollBar(Qt::Horizontal, plot->view());
  scrollData_.vbar = new QScrollBar(Qt::Vertical  , plot->view());

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
    const CQChartsPlotKey *key = plot()->getFirstPlotKey();

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
  invalidateLayout();

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
updateLocation(const CQChartsGeom::BBox &bbox)
{
  // get external margin
  double xlm = plot()->lengthPlotWidth (margin().left  ());
  double xrm = plot()->lengthPlotWidth (margin().right ());
  double ytm = plot()->lengthPlotHeight(margin().top   ());
  double ybm = plot()->lengthPlotHeight(margin().bottom());

  // get internal padding
  double xlp = plot()->lengthPlotWidth (padding().left  ());
  double xrp = plot()->lengthPlotWidth (padding().right ());
  double ytp = plot()->lengthPlotHeight(padding().top   ());
  double ybp = plot()->lengthPlotHeight(padding().bottom());

  //---

  CQChartsAxis *xAxis = plot()->xAxis();
  CQChartsAxis *yAxis = plot()->yAxis();

  // get key contents size
  auto ks = calcSize();

  // calc key contents position
  // (bbox is plot bbox)
  double kx { 0.0 }, ky { 0.0 };

  if (location().isAuto()) {
    auto fitBBox = plot()->findEmptyBBox(ks.width(), ks.height());

    if (fitBBox.isSet()) {
      kx = fitBBox.getXMid() - ks.width ()/2;
      ky = fitBBox.getYMid() + ks.height()/2;

      location_.setType(CQChartsKeyLocation::Type::ABSOLUTE_POSITION);

      setAbsolutePlotPosition(CQChartsGeom::Point(kx, ky));
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
        kx -= plot_->calcGroupedYAxisRange(CQChartsAxisSide::Type::BOTTOM_LEFT).getWidth() + xlm;
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
        kx += plot_->calcGroupedYAxisRange(CQChartsAxisSide::Type::TOP_RIGHT).getWidth() + xrm;
    }
  }

  if      (location().onTop()) {
    if (isInsideY())
      ky = bbox.getYMax() - ytm - ytp;
    else {
      ky = bbox.getYMax() + ks.height() + ytm + ytp;

      // offset by bottom x axis height
      if (xAxis)
        ky += plot_->calcGroupedXAxisRange(CQChartsAxisSide::Type::TOP_RIGHT).getHeight() + ytm;
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

      // offset by top x axis height
      if (xAxis)
        ky -= plot_->calcGroupedXAxisRange(CQChartsAxisSide::Type::BOTTOM_LEFT).getHeight() + ybm;
    }
  }

  CQChartsGeom::Point kp(kx, ky);

  //---

  // update location for absolute position/rectangle
  CQChartsKeyLocation::Type locationType = this->location().type();

  if      (locationType == CQChartsKeyLocation::Type::ABSOLUTE_POSITION) {
    kp = absolutePlotPosition();
  }
  else if (locationType == CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE) {
    auto bbox = absolutePlotRectangle();

    if (bbox.isValid())
      kp = CQChartsGeom::Point(bbox.getUL());
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
    CQPropertyViewItem *item = addProp(name, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  addProp("visible"   , "Is visible");
  addProp("selected"  , "Is selected");
  addProp("horizontal", "Draw items horizontally");
  addProp("flipped"   , "Draw name value flipped");
  addProp("autoHide"  , "Auto hide key when too large");
  addProp("clipped"   , "Clip key to plot");

  addProp("above"   , "Draw key above plot");
  addProp("insideX" , "Key placed inside plot in x direction");
  addProp("insideY" , "Key placed inside plot in y direction");
  addProp("location", "Key placement location");

  addProp("absolutePosition" , "Key placement absolute position in view coordinates");
  addProp("absoluteRectangle", "Key placement absolute rectangle in view coordinates");

  addProp("interactive"  , "Key supports click");
  addProp("pressBehavior", "Key click behavior");

  addStyleProp("hiddenAlpha", "Alpha for hidden items");

  addProp("maxRows", "Max rows for key");
  addProp("spacing", "Spacing between rows in pixels");

  //---

  QString scrollPath = path + "/scroll";

  auto addScrollProp = [&](const QString &name, const QString &alias, const QString &desc) {
    model->addProperty(scrollPath, this, name, alias)->setDesc(desc);
  };

  addScrollProp("scrollWidth" , "width" , "Key has fixed width and will scroll when larger");
  addScrollProp("scrollHeight", "height", "Key has fixed height and will scroll when larger");

  //---

  // header text
  QString headerPath     = path + "/header";
  QString headerTextPath = headerPath + "/text";

  auto addHeaderProp = [&](const QString &name, const QString &alias, const QString &desc) {
    return &(model->addProperty(headerTextPath, this, name, alias)->
              setDesc("Key header text " + desc));
  };

  auto addHeaderStyleProp = [&](const QString &name, const QString &alias, const QString &desc) {
    CQPropertyViewItem *item = addHeaderProp(name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  addHeaderProp("header", "string", "string");

  addHeaderStyleProp("headerTextColor"    , "color"    , "color");
  addHeaderStyleProp("headerTextAlpha"    , "alpha"    , "alpha");
  addHeaderStyleProp("headerTextFont"     , "font"     , "font");
//addHeaderStyleProp("headerTextAngle"    , "angle"    , "angle");
  addHeaderStyleProp("headerTextContrast" , "contrast" , "contrast");
  addHeaderStyleProp("headerTextAlign"    , "align"    , "align");
  addHeaderStyleProp("headerTextFormatted", "formatted", "formatted to fit box");
  addHeaderStyleProp("headerTextScaled"   , "scaled"   , "scaled to box");
  addHeaderStyleProp("headerTextHtml"     , "html"     , "is html");

  //---

  // stroke, fill
  addBoxProperties(model, path, "");

  //---

  // key text
  QString textPath = path + "/text";

  auto addTextProp = [&](const QString &name, const QString &alias, const QString &desc) {
    return &(model->addProperty(textPath, this, name, alias)->setDesc("Key text " + desc));
  };

  auto addTextStyleProp = [&](const QString &name, const QString &alias, const QString &desc) {
    CQPropertyViewItem *item = addTextProp(name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  addTextStyleProp("textColor"    , "color"    , "color");
  addTextStyleProp("textAlpha"    , "alpha"    , "alpha");
  addTextStyleProp("textFont"     , "font"     , "font");
//addTextStyleProp("textAngle"    , "angle"    , "angle");
  addTextStyleProp("textContrast" , "contrast" , "contrast");
  addTextStyleProp("textAlign"    , "align"    , "align");
  addTextStyleProp("textFormatted", "formatted", "formatted to fit box");
  addTextStyleProp("textScaled"   , "scaled"   , "scaled to box");
  addTextStyleProp("textHtml"     , "html"     , "is html");
}

void
CQChartsPlotKey::
invalidateLayout()
{
  needsLayout_ = true;
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
addItem(CQChartsKeyItem *item, int row, int col, int nrows, int ncols)
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
  using ColItems    = std::map<int,Items>;
  using RowColItems = std::map<int,ColItems>;

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
      const Items &items = rowColItems[r][c];

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

  // get spacing, margin and padding in plot coords
  xs_ = plot()->pixelToWindowWidth (spacing());
  ys_ = plot()->pixelToWindowHeight(spacing());

  pmargin_.xl = plot()->lengthPlotWidth (margin().left  ());
  pmargin_.xr = plot()->lengthPlotWidth (margin().right ());
  pmargin_.yt = plot()->lengthPlotHeight(margin().top   ());
  pmargin_.yb = plot()->lengthPlotHeight(margin().bottom());

  ppadding_.xl = plot()->lengthPlotWidth (padding().left  ());
  ppadding_.xr = plot()->lengthPlotWidth (padding().right ());
  ppadding_.yt = plot()->lengthPlotHeight(padding().top   ());
  ppadding_.yb = plot()->lengthPlotHeight(padding().bottom());

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
    QFont font = view()->plotFont(plot(), headerTextFont());

    // get text size
    auto tsize = CQChartsDrawUtil::calcTextSize(headerStr(), font, textOptions);

    layoutData_.headerWidth  = plot()->pixelToWindowWidth (tsize.width ()) + 2*xs_;
    layoutData_.headerHeight = plot()->pixelToWindowHeight(tsize.height()) + 2*ys_;
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

      Cell &cell = rowColCell_[r][c];

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
    Cell &cell = rowColCell_[0][c];

    w += cell.width;
  }

  w += pmargin_.xl + ppadding_.xl + pmargin_.xr + ppadding_.xr;

  for (int r = 0; r < numRows; ++r) {
    Cell &cell = rowColCell_[r][0];

    h += cell.height;
  }

  h += pmargin_.yb + ppadding_.yb + pmargin_.yt + ppadding_.yt + layoutData_.headerHeight;

  w = std::max(w, layoutData_.headerWidth);

  layoutData_.fullSize = CQChartsGeom::Size(w, h);

  //---

  double vbw = plot()->pixelToWindowWidth (scrollData_.pixelBarSize);
  double hbh = plot()->pixelToWindowHeight(scrollData_.pixelBarSize);

  //---

  // check if key size exceeds plot pixel size (auto hide if needed)
  auto plotPixelRect = plot()->calcPlotPixelRect();

  double maxPixelWidth  = plotPixelRect.getWidth ()*0.8;
  double maxPixelHeight = plotPixelRect.getHeight()*0.8;

  double pixelWidth  = plot()->windowToPixelWidth (layoutData_.fullSize.width ());
  double pixelHeight = plot()->windowToPixelHeight(layoutData_.fullSize.height());

  layoutData_.pixelWidthExceeded  = (pixelWidth  > maxPixelWidth );
  layoutData_.pixelHeightExceeded = (pixelHeight > maxPixelHeight);

  //---

  // calc if horizontally, vertically scrolled and scroll area sizes
  layoutData_.hscrolled       = false;
  layoutData_.scrollAreaWidth = w;

  if      (scrollData_.width.isSet()) {
    double sw = plot()->lengthPlotWidth(scrollData_.width.length());

    layoutData_.hscrolled       = (w > sw);
    layoutData_.scrollAreaWidth = sw;
  }
  else if (layoutData_.pixelWidthExceeded) {
    layoutData_.hscrolled       = true;
    layoutData_.scrollAreaWidth = plot()->pixelToWindowWidth(maxPixelWidth);
  }

  layoutData_.vscrolled        = false;
  layoutData_.scrollAreaHeight = h - layoutData_.headerHeight;

  if      (scrollData_.height.isSet()) {
    double sh = plot()->lengthPlotHeight(scrollData_.height.length());

    layoutData_.vscrolled        = (h > sh + layoutData_.headerHeight);
    layoutData_.scrollAreaHeight = sh;
  }
  else if (layoutData_.pixelHeightExceeded) {
    layoutData_.vscrolled        = true;
    layoutData_.scrollAreaHeight = plot()->pixelToWindowHeight(maxPixelHeight) -
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

  layoutData_.size = CQChartsGeom::Size(w, h);
}

CQChartsGeom::Point
CQChartsPlotKey::
absolutePlotPosition() const
{
  return plot()->viewToWindow(absolutePosition());
}

void
CQChartsPlotKey::
setAbsolutePlotPosition(const CQChartsGeom::Point &p)
{
  setAbsolutePosition(plot()->windowToView(p));
}

CQChartsGeom::BBox
CQChartsPlotKey::
absolutePlotRectangle() const
{
  auto bbox = absoluteRectangle();

  if (! bbox.isValid())
    return bbox;

  return plot()->viewToWindow(bbox);
}

void
CQChartsPlotKey::
setAbsolutePlotRectangle(const CQChartsGeom::BBox &bbox)
{
  setAbsoluteRectangle(plot()->windowToView(bbox));
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
contains(const CQChartsGeom::Point &p) const
{
  if (! isOverlayVisible() || isEmpty())
    return false;

  return bbox().inside(p);
}

void
CQChartsPlotKey::
boxDataInvalidate()
{
  redraw();
}

CQChartsKeyItem *
CQChartsPlotKey::
getItemAt(const CQChartsGeom::Point &p) const
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
selectPress(const CQChartsGeom::Point &, CQChartsSelMod)
{
  return false;
}

bool
CQChartsPlotKey::
selectMove(const CQChartsGeom::Point &w)
{
  bool changed = false;

  if (contains(w)) {
    CQChartsKeyItem *item = getItemAt(w);

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

bool
CQChartsPlotKey::
selectRelease(const CQChartsGeom::Point &)
{
  return false;
}

//------

bool
CQChartsPlotKey::
editPress(const CQChartsGeom::Point &p)
{
  editHandles_->setDragPos(p);

  CQChartsKeyLocation::Type locationType = this->location().type();

  if (locationType != CQChartsKeyLocation::Type::ABSOLUTE_POSITION &&
      locationType != CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE) {
    location_ = CQChartsKeyLocation::Type::ABSOLUTE_POSITION;

    setAbsolutePlotPosition(position_);
  }

  return true;
}

bool
CQChartsPlotKey::
editMove(const CQChartsGeom::Point &p)
{
  const auto &dragPos  = editHandles_->dragPos();
  const auto &dragSide = editHandles_->dragSide();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  if (dragSide == CQChartsResizeSide::MOVE) {
    location_ = CQChartsKeyLocation::Type::ABSOLUTE_POSITION;

    setAbsolutePlotPosition(absolutePlotPosition() + CQChartsGeom::Point(dx, dy));
  }
  else {
    location_ = CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE;

    editHandles_->updateBBox(dx, dy);

    wbbox_ = editHandles_->bbox();

    setAbsolutePlotRectangle(wbbox_);

    CQChartsLength width (wbbox_.getWidth ()                           , CQChartsUnits::PLOT);
    CQChartsLength height(wbbox_.getHeight() - layoutData_.headerHeight, CQChartsUnits::PLOT);

    setScrollWidth (CQChartsOptLength(width ));
    setScrollHeight(CQChartsOptLength(height));

    invalidateLayout();
  }

  editHandles_->setDragPos(p);

  updatePosition(/*queued*/false);

  return true;
}

bool
CQChartsPlotKey::
editMotion(const CQChartsGeom::Point &p)
{
  return editHandles_->selectInside(p);
}

bool
CQChartsPlotKey::
editRelease(const CQChartsGeom::Point &)
{
  return true;
}

void
CQChartsPlotKey::
editMoveBy(const CQChartsGeom::Point &f)
{
  CQChartsKeyLocation::Type locationType = this->location().type();

  if (locationType != CQChartsKeyLocation::Type::ABSOLUTE_POSITION &&
      locationType != CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE) {
    location_ = CQChartsKeyLocation::Type::ABSOLUTE_POSITION;

    setAbsolutePlotPosition(position_ + f);
  }

  updatePosition();
}

//------

bool
CQChartsPlotKey::
tipText(const CQChartsGeom::Point &p, QString &tip) const
{
  bool rc = false;

  CQChartsKeyItem *item = getItemAt(p);

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

  needsLayout_ = true;

  redraw();
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

  CQChartsKeyLocation::Type locationType = this->location().type();

  //---

  // calc plot bounding box (full size)
  double x = position_.x; // left
  double y = position_.y; // top
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

  CQChartsGeom::BBox sbbox(x, y - sh, x + sw, y);

  //---

  // set displayed bbox
  if (locationType != CQChartsKeyLocation::Type::ABSOLUTE_RECTANGLE) {
    wbbox_ = CQChartsGeom::BBox(x, y - h, x + w, y);
  }
  else {
    auto bbox = absolutePlotRectangle();

    if (bbox.isValid())
      wbbox_ = bbox;
  }

  //---

  // calc pixel bounding box
  auto p1 = plot()->windowToPixel(CQChartsGeom::Point(x     + pmargin_.xl, y     - pmargin_.yt));
  auto p2 = plot()->windowToPixel(CQChartsGeom::Point(x + w - pmargin_.xl, y - h + pmargin_.yb));

  CQChartsGeom::BBox pixelRect(p1, p2);

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
  auto plotPixelRect = plot()->calcPlotPixelRect();

  bool clipped  = false;
  auto clipRect = plotPixelRect;

  //---

  // get plot data rect
  auto dataPixelRect = plot()->calcDataPixelRect();

  auto dataRect = dataPixelRect;

  //---

  // get max number of rows
  int numRows = calcNumRows();

  //---

  device->save();

  //---

  // place scroll bar
  sx_ = 0.0;
  sy_ = 0.0;

  double phh = plot()->windowToPixelHeight(layoutData_.headerHeight);

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
    int pageStep  = (int) plot()->windowToPixelWidth(sw - layoutData_.vbarWidth);
    int fullWidth = (int) plot()->windowToPixelWidth(layoutData_.fullSize.width());

    if (scrollData_.hbar->pageStep() != pageStep)
      scrollData_.hbar->setPageStep(pageStep);

    int smax = std::max(fullWidth - pageStep, 1);

    if (scrollData_.hbar->maximum() != smax)
      scrollData_.hbar->setRange(0, smax);

    if (scrollData_.hbar->value() != scrollData_.hpos)
      scrollData_.hbar->setValue(scrollData_.hpos);

    sx_ = plot()->pixelToWindowWidth(scrollData_.hpos);
  }
  else {
    if (scrollData_.hbar)
      scrollData_.hbar->hide();
  }

  //---

  if (layoutData_.vscrolled || layoutData_.hscrolled) {
    clipped  = true;
    clipRect = CQChartsGeom::BBox(p1.x, p1.y + phh, p2.x - vspw, p2.y - hsph);
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
        clipRect = plot()->windowToPixel(bbox);
      }
    }

    clipped = isClipped();
  }

  //---

  // draw box (background)
  CQChartsBoxObj::draw(device, device->pixelToWindow(pixelRect));

  //---

  if (clipped) {
    auto cr = device->pixelToWindow(clipRect);

    device->setClipRect(cr);
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

    textOptions = plot()->adjustTextOptions(textOptions);

    //---

    // get font
    QFont font = view()->plotFont(plot(), headerTextFont());

    device->setFont(font);

    //---

    // get key top left, width (pixels), margins
    auto p = plot()->windowToPixel(CQChartsGeom::Point(x, y)); // top left

    double pw = plot()->windowToPixelWidth(sw);

    //---

    // get external margin
  //double xlm = plot()->lengthPlotWidth (margin().left  ());
  //double xrm = plot()->lengthPlotWidth (margin().right ());
    double ytm = plot()->lengthPlotHeight(margin().top   ());
    double ybm = plot()->lengthPlotHeight(margin().bottom());

    // get internal padding
  //double xlp = plot()->lengthPlotWidth (padding().left  ());
  //double xrp = plot()->lengthPlotWidth (padding().right ());
  //double ytp = plot()->lengthPlotHeight(padding().top   ());
  //double ybp = plot()->lengthPlotHeight(padding().bottom());

    //---

    // calc text rect
    auto tsize = CQChartsDrawUtil::calcTextSize(headerStr(), font, textOptions);

    double tw = pw;
    double th = tsize.height() + ybm + ytm;

    CQChartsGeom::BBox trect(p.x, p.y, p.x + tw, p.y + th);

    //---

    // set text pen
    CQChartsPenBrush tPenBrush;

    QColor tc = interpHeaderTextColor(ColorInd());

    plot()->setPen(tPenBrush.pen, true, tc, headerTextAlpha());

    plot()->updateObjPenBrushState(this, tPenBrush);

    //---

    // draw text
    device->setPen(tPenBrush.pen);

    CQChartsDrawUtil::drawTextInBox(device, device->pixelToWindow(trect), headerStr(), textOptions);
  }

  //---

  // draw items
  for (const auto &item : items_) {
    int col = item->col();

    if (isFlipped())
      col = numCols_ - 1 - col;

    Cell &cell = rowColCell_[item->row()][col];

    double x1 = cell.x - sx_;
    double y1 = cell.y + sy_;
    double w1 = cell.width;
    double h1 = cell.height;

    for (int c = 1; c < item->colSpan(); ++c) {
      Cell &cell1 = rowColCell_[item->row()][col + c];

      w1 += cell1.width;
    }

    for (int r = 1; r < item->rowSpan(); ++r) {
      Cell &cell1 = rowColCell_[item->row() + r][col];

      h1 += cell1.height;
    }

    CQChartsGeom::BBox bbox(x1 + x, y1 + y - h1, x1 + x + w1, y1 + y);

    item->setBBox(bbox);

    if (wbbox_.overlaps(bbox)) {
      item->draw(device, bbox);

      if (plot()->showBoxes())
        plot()->drawWindowColorBox(device, bbox);
    }
  }

  //---

  // draw box
  if (plot()->showBoxes()) {
    plot()->drawWindowColorBox(device, wbbox_);

    CQChartsGeom::BBox headerBox(x, y - layoutData_.headerHeight, x + sw, y);

    plot()->drawWindowColorBox(device, headerBox);
  }

  //---

  device->restore();
}

void
CQChartsPlotKey::
drawEditHandles(QPainter *painter) const
{
  assert(plot()->view()->mode() == CQChartsView::Mode::EDIT || isSelected());

  auto *th = const_cast<CQChartsPlotKey *>(this);

  if (scrollData_.height.isSet() || scrollData_.width.isSet() ||
      layoutData_.vscrolled || layoutData_.hscrolled)
    th->editHandles_->setMode(CQChartsEditHandles::Mode::RESIZE);
  else
    th->editHandles_->setMode(CQChartsEditHandles::Mode::MOVE);

  th->editHandles_->setBBox(this->bbox());

  editHandles_->draw(painter);
}

QColor
CQChartsPlotKey::
interpBgColor() const
{
  if (isFilled())
    return interpFillColor(ColorInd());

  CQChartsKeyLocation::Type locationType = this->location().type();

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

CQChartsKeyItem::
CQChartsKeyItem(CQChartsPlotKey *key, const ColorInd &ic) :
 key_(key), ic_(ic)
{
}

bool
CQChartsKeyItem::
selectPress(const CQChartsGeom::Point &, CQChartsSelMod selMod)
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
selectMove(const CQChartsGeom::Point &)
{
  return isClickable();
}

void
CQChartsKeyItem::
doShow(CQChartsSelMod selMod)
{
  CQChartsPlot *plot = key_->plot();

  const auto &ic = colorIndex();

  if      (selMod == CQChartsSelMod::REPLACE) {
    for (int i = 0; i < ic.n; ++i)
      plot->setSetHidden(i, i != ic.i);
  }
  else if (selMod == CQChartsSelMod::ADD)
    plot->setSetHidden(ic.i, false);
  else if (selMod == CQChartsSelMod::REMOVE)
    plot->setSetHidden(ic.i, true);
  else if (selMod == CQChartsSelMod::TOGGLE)
    plot->setSetHidden(ic.i, ! plot->isSetHidden(ic.i));

  plot->updateObjs();
}

void
CQChartsKeyItem::
doSelect(CQChartsSelMod)
{
}

bool
CQChartsKeyItem::
tipText(const CQChartsGeom::Point &, QString &) const
{
  return false;
}

//------

CQChartsKeyText::
CQChartsKeyText(CQChartsPlot *plot, const QString &text, const ColorInd &ic) :
 CQChartsKeyItem(plot->key(), ic), plot_(plot), text_(text)
{
}

CQChartsGeom::Size
CQChartsKeyText::
size() const
{
  CQChartsPlot *plot = key_->plot();

  QFont font = plot->view()->plotFont(plot, key_->textFont());

  QFontMetricsF fm(font);

  double w = fm.width(text_);
  double h = fm.height();

  double ww = plot->pixelToWindowWidth (w + 4);
  double wh = plot->pixelToWindowHeight(h + 4);

  return CQChartsGeom::Size(ww, wh);
}

QColor
CQChartsKeyText::
interpTextColor(const ColorInd &ind) const
{
  return key_->interpTextColor(ind);
}

void
CQChartsKeyText::
draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect) const
{
  CQChartsPlot *plot = key_->plot();

  plot->view()->setPlotPainterFont(plot, device, key_->textFont());

  QColor tc = interpTextColor(ColorInd());

  device->setPen(tc);

  CQChartsTextOptions textOptions;

  textOptions.align         = key_->textAlign();
  textOptions.contrast      = key_->isTextContrast();
  textOptions.contrastAlpha = key_->textContrastAlpha();
  textOptions.formatted     = key_->isTextFormatted();
  textOptions.scaled        = key_->isTextScaled();
  textOptions.html          = key_->isTextHtml();

  textOptions = plot->adjustTextOptions(textOptions);

  CQChartsDrawUtil::drawTextInBox(device, rect, text_, textOptions);
}

//------

CQChartsKeyColorBox::
CQChartsKeyColorBox(CQChartsPlot *plot, const ColorInd &is, const ColorInd &ig, const ColorInd &iv,
                    const RangeValue &xv, const RangeValue &yv) :
 CQChartsKeyItem(plot->key(), iv), plot_(plot), is_(is), ig_(ig), iv_(iv), xv_(xv), yv_(yv)
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
  CQChartsPlot *plot = key_->plot();

  QFont font = plot->view()->plotFont(plot, key_->textFont());

  QFontMetricsF fm(font);

  double h = fm.height();

  double ww = plot->pixelToWindowWidth (h + 2);
  double wh = plot->pixelToWindowHeight(h + 2);

  return CQChartsGeom::Size(ww, wh);
}

bool
CQChartsKeyColorBox::
selectPress(const Point &w, CQChartsSelMod selMod)
{
  if (! value_.isValid())
    return CQChartsKeyItem::selectPress(w, selMod);

  if (isClickable()) {
    if      (key_->pressBehavior() == CQChartsKeyPressBehavior::Type::SHOW) {
      CQChartsPlot *plot = key_->plot();

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
draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect) const
{
  CQChartsPlot *plot = key_->plot();

  auto prect = plot->windowToPixel(rect);

  bool swapped;
  auto prect1 = prect.adjusted(2, 2, -2, -2, swapped);
  if (swapped) return;

  ColorInd colorInd = calcColorInd();

  QColor bc    = interpStrokeColor(colorInd);
  QBrush brush = fillBrush();

  if (isInside())
    brush.setColor(plot->insideColor(brush.color()));

  device->setPen  (bc);
  device->setBrush(brush);

  CQChartsDrawUtil::drawRoundedPolygon(device, device->pixelToWindow(prect1), cornerRadius());
}

QBrush
CQChartsKeyColorBox::
fillBrush() const
{
  CQChartsPlot *plot = key_->plot();

  ColorInd ic = calcColorInd();

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
CQChartsKeyLine(CQChartsPlot *plot, const ColorInd &is, const ColorInd &ig) :
 CQChartsKeyItem(plot->key(), is.n > 1 ? is : ig), is_(is), ig_(ig)
{
  setClickable(true);
}

CQChartsGeom::Size
CQChartsKeyLine::
size() const
{
  CQChartsPlot *plot = key_->plot();

  QFont font = plot->view()->plotFont(plot, key_->textFont());

  QFontMetricsF fm(font);

  double w = fm.width("-X-");
  double h = fm.height();

  double ww = plot->pixelToWindowWidth (w + 8);
  double wh = plot->pixelToWindowHeight(h + 2);

  return CQChartsGeom::Size(ww, wh);
}

bool
CQChartsKeyLine::
selectPress(const Point &w, CQChartsSelMod selMod)
{
  if (! value_.isValid())
    return CQChartsKeyItem::selectPress(w, selMod);

  if (isClickable()) {
    if      (key_->pressBehavior() == CQChartsKeyPressBehavior::Type::SHOW) {
      CQChartsPlot *plot = key_->plot();

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
draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect) const
{
  CQChartsPlot *plot = key_->plot();

  auto prect = plot->windowToPixel(rect);

  bool swapped;
  auto pbbox1 = prect.adjusted(2, 2, -2, -2, swapped);
  if (swapped) return;

  double x1 = pbbox1.getXMin() + 4;
  double x2 = pbbox1.getXMax() - 4;
  double y  = pbbox1.getYMid();

  const CQChartsFillData   &fillData   = symbolData_.fill();
  const CQChartsStrokeData &strokeData = symbolData_.stroke();

  QColor lc = plot->interpColor(fillData  .color(), ig_);
  QColor fc = plot->interpColor(strokeData.color(), ig_);

  CQChartsPenBrush penBrush;

  plot->setPenBrush(penBrush,
    CQChartsPenData  (true, lc),
    CQChartsBrushData(true, fc));

  device->setPen  (penBrush.pen);
  device->setBrush(penBrush.brush);

  device->drawLine(device->pixelToWindow(CQChartsGeom::Point(x1, y)),
                   device->pixelToWindow(CQChartsGeom::Point(x2, y)));

  CQChartsGeom::Point ps(CMathUtil::avg(x1, x2), y);

  plot->drawSymbol(device, device->pixelToWindow(ps), symbolData_.type(),
                   symbolData_.size(), penBrush);
}
