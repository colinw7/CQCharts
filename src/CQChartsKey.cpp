#include <CQChartsKey.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsView.h>
#include <CQChartsEditHandles.h>
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
  setFillAlpha(0.5);

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
  if (location() == CQChartsKeyLocation::Type::ABS_POSITION)
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

  CQChartsGeom::Point p = view()->windowToPixel(CQChartsGeom::Point(x, y));

  //p.x += dx*bs;
  //p.y += dy*bs;

  //---

  double pw = 0.0;
  double ph = 0.0;

  int n = view()->numPlots();

  for (int i = 0; i < n; ++i) {
    CQChartsPlot *plot = view()->plot(i);

    QString name = plot->keyText();

    double tw = fm.width(name) + bs + margin();

    pw = std::max(pw, tw);

    ph += bs;
  }

  size_ = QSizeF(pw + 2*margin(), ph + 2*margin()  + (n - 1)*2);

  double pxr = 0.0, pyr = 0.0;

  if      (location().onLeft   ()) pxr = p.x                   + margin();
  else if (location().onHCenter()) pxr = p.x - size_.width()/2;
  else if (location().onRight  ()) pxr = p.x - size_.width()   - margin();

  if      (location().onTop    ()) pyr = p.y                    + margin();
  else if (location().onVCenter()) pyr = p.y - size_.height()/2;
  else if (location().onBottom ()) pyr = p.y - size_.height()   - margin();

  pposition_ = QPointF(pxr, pyr);
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
  CQChartsBoxObj::addProperties(model, path, "");

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

  CQChartsViewKey *th = const_cast<CQChartsViewKey *>(this);

  th->doLayout();

  //---

  // pixel position & size (TODO: using view/units)
  double px = pposition_.x(); // left
  double py = pposition_.y(); // top

  double pw = size_.width ();
  double ph = size_.height();

  wbbox_ = CQChartsGeom::BBox(px, py - ph, px + pw, py);

  //---

  CQChartsGeom::Point p1 = view()->pixelToWindow(CQChartsGeom::Point(px     , py     ));
  CQChartsGeom::Point p2 = view()->pixelToWindow(CQChartsGeom::Point(px + pw, py + ph));

  pbbox_ = CQChartsGeom::BBox(p1.x, p2.y, p2.x, p1.y);

  //---

  QRectF rect(px, py, pw, ph);

  //---

  CQChartsBoxObj::draw(device, rect);

  //---

  view()->setPainterFont(device, textFont());

  QFontMetricsF fm(device->font());

  double px1 = px + margin();
  double py1 = py + margin();

  int n = view()->numPlots();

  double bs = fm.height() + 4.0;

  //double dth = (bs - fm.height())/2;

  prects_.clear();

  for (int i = 0; i < n; ++i) {
    double py2 = py1 + bs + 2;

    CQChartsPlot *plot = view()->plot(i);

    bool checked = plot->isVisible();

    //---

    drawCheckBox(device, px1, (py1 + py2)/2.0 - bs/2.0, bs, checked);

    //---

    device->setPen(interpTextColor(ColorInd()));

    QString name = plot->keyText();

    double px2 = px1 + bs + margin();

    //double tw = fm.width(name);

    QRectF rect1(px2, py1, pw - bs - 2*margin(), py2 - py1);

    CQChartsTextOptions textOptions;

    textOptions.contrast  = isTextContrast();
    textOptions.formatted = isTextFormatted();
    textOptions.scaled    = isTextScaled();
    textOptions.html      = isTextHtml();
    textOptions.align     = textAlign();

    CQChartsDrawUtil::drawTextInBox(device, device->pixelToWindow(rect1), name, textOptions);

    //CQChartsDrawUtil::drawSimpleText(device, QPointF(px2, py1 + fm.ascent() + dth), name);

    //---

    // save view key item (plot) rect
    CQChartsGeom::Point p1 = view()->pixelToWindow(CQChartsGeom::Point(px     , py1));
    CQChartsGeom::Point p2 = view()->pixelToWindow(CQChartsGeom::Point(px + pw, py2));

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

  CQChartsViewKey *th = const_cast<CQChartsViewKey *>(this);

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

  //QRectF qrect(x, y, bs, bs);
  QRectF qrect(0, 0, bs, bs);

  QStylePainter spainter(&cimage, view());

  spainter.setPen(interpTextColor(ColorInd()));

  QStyleOptionButton opt;

  opt.initFrom(view());

  opt.rect = qrect.toRect();

  opt.state |= (checked ? QStyle::State_On : QStyle::State_Off);

  spainter.drawControl(QStyle::CE_CheckBox, opt);

  device->drawImage(QPointF(x, y), cimage);

  //device->drawRect(qrect);
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
  const CQChartsGeom::Point &dragPos = editHandles_->dragPos();

  double dx = w.x - dragPos.x;
  double dy = w.y - dragPos.y;

  location_ = CQChartsKeyLocation::Type::ABS_POSITION;

  wposition_ = wposition_ + QPointF(dx, dy);
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
updateLocation(const CQChartsGeom::BBox &bbox)
{
  // calc key size
  QSizeF ks = calcSize();

  double xm = plot()->pixelToWindowWidth (margin());
  double ym = plot()->pixelToWindowHeight(margin());

  double kx { 0.0 }, ky { 0.0 };

  if (location().isAuto()) {
    CQChartsGeom::BBox fitBBox = plot()->findEmptyBBox(ks.width(), ks.height());

    if (fitBBox.isSet()) {
      kx = fitBBox.getXMid() - ks.width ()/2;
      ky = fitBBox.getYMid() + ks.height()/2;

      location_.setType(CQChartsKeyLocation::Type::ABS_POSITION);

      setAbsolutePlotPosition(QPointF(kx, ky));
    }
    else
      location_.setType(CQChartsKeyLocation::Type::TOP_RIGHT);
  }

  if      (location().onLeft()) {
    if (isInsideX())
      kx = bbox.getXMin() + xm;
    else
      kx = bbox.getXMin() - ks.width() - xm;
  }
  else if (location().onHCenter()) {
    kx = bbox.getXMid() - ks.width()/2;
  }
  else if (location().onRight()) {
    if (isInsideX())
      kx = bbox.getXMax() - ks.width() - xm;
    else
      kx = bbox.getXMax() + xm;
  }

  if      (location().onTop()) {
    if (isInsideY())
      ky = bbox.getYMax() - ym;
    else
      ky = bbox.getYMax() + ks.height() + ym;
  }
  else if (location().onVCenter()) {
    ky = bbox.getYMid() + ks.height()/2;
  }
  else if (location().onBottom()) {
    if (isInsideY())
      ky = bbox.getYMin() + ks.height() + ym;
    else {
      ky = bbox.getYMin() - ym;

      CQChartsAxis *xAxis = plot()->xAxis();

      if (xAxis && xAxis->side() == CQChartsAxisSide::Type::BOTTOM_LEFT && xAxis->bbox().isSet())
        ky -= xAxis->bbox().getHeight();
    }
  }

  QPointF kp(kx, ky);

  CQChartsKeyLocation::Type locationType = this->location().type();

  if      (locationType == CQChartsKeyLocation::Type::ABS_POSITION) {
    kp = absolutePlotPosition();
  }
  else if (locationType == CQChartsKeyLocation::Type::ABS_RECT) {
    kp = absolutePlotRect().bottomLeft();
  }

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

  addProp("absolutePosition" , "Key placement absolute position");
  addProp("absoluteRectangle", "Key placement absolute rectangle");

  addProp("interactive"  , "Key supports click");
  addProp("pressBehavior", "Key click behavior");

  addStyleProp("hiddenAlpha", "Alpha for hidden items");

  addProp("maxRows", "Max rows for key");
  addProp("spacing", "Spacing between rows");

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
  CQChartsBoxObj::addProperties(model, path, "");

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
        QSizeF size = item->size();

        double width  = size.width ()/item->colSpan();
        double height = size.height()/item->rowSpan();

        rowColCell_[r][c].width  = std::max(rowColCell_[r][c].width , width );
        rowColCell_[r][c].height = std::max(rowColCell_[r][c].height, height);
      }
    }
  }

  //---

  // get spacing and margin in plot coords
  xs_ = plot()->pixelToWindowWidth (spacing());
  ys_ = plot()->pixelToWindowHeight(spacing());

  xm_ = plot()->pixelToWindowWidth (margin());
  ym_ = plot()->pixelToWindowHeight(margin());

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
    QSizeF tsize = CQChartsDrawUtil::calcTextSize(headerStr(), font, textOptions);

    layoutData_.headerWidth  = plot()->pixelToWindowWidth (tsize.width ()) + 2*xs_;
    layoutData_.headerHeight = plot()->pixelToWindowHeight(tsize.height()) + 2*ys_;
  }

  //---

  // update cell positions and sizes
  double y = -ym_;

  y -= layoutData_.headerHeight;

  for (int r = 0; r < numRows; ++r) {
    double x = xm_;

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

  w += 2*xm_;

  for (int r = 0; r < numRows; ++r) {
    Cell &cell = rowColCell_[r][0];

    h += cell.height;
  }

  h += 2*ym_ + layoutData_.headerHeight;

  w = std::max(w, layoutData_.headerWidth);

  layoutData_.fullSize = QSizeF(w, h);

  //---

  double vbw = plot()->pixelToWindowWidth (scrollData_.pixelBarSize);
  double hbh = plot()->pixelToWindowHeight(scrollData_.pixelBarSize);

  //---

  // check if key size exceeds plot pixel size (auto hide if needed)
  CQChartsGeom::BBox plotPixelRect = plot()->calcPlotPixelRect();

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

  layoutData_.size = QSizeF(w, h);
}

QPointF
CQChartsPlotKey::
absolutePlotPosition() const
{
  return plot()->viewToWindow(absolutePosition());
}

void
CQChartsPlotKey::
setAbsolutePlotPosition(const QPointF &p)
{
  setAbsolutePosition(plot()->windowToView(p));
}

QRectF
CQChartsPlotKey::
absolutePlotRect() const
{
  return plot()->viewToWindow(absoluteRectangle());
}

void
CQChartsPlotKey::
setAbsolutePlotRect(const QRectF &r)
{
  setAbsoluteRectangle(plot()->windowToView(r));
}

int
CQChartsPlotKey::
calcNumRows() const
{
  // get max number of rows
  int numRows = numRows_;

#if 0
  // limit rows if height (and this scrolled) not defined
  if (! scrollData_.height.isSet())
    numRows = std::min(numRows, maxRows());
#endif

  return numRows;
}

QSizeF
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

  if (locationType != CQChartsKeyLocation::Type::ABS_POSITION &&
      locationType != CQChartsKeyLocation::Type::ABS_RECT) {
    location_ = CQChartsKeyLocation::Type::ABS_POSITION;

    setAbsolutePlotPosition(position_);
  }

  return true;
}

bool
CQChartsPlotKey::
editMove(const CQChartsGeom::Point &p)
{
  const CQChartsGeom::Point& dragPos  = editHandles_->dragPos();
  const CQChartsResizeSide&  dragSide = editHandles_->dragSide();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  if (dragSide == CQChartsResizeSide::MOVE) {
    location_ = CQChartsKeyLocation::Type::ABS_POSITION;

    setAbsolutePlotPosition(absolutePlotPosition() + QPointF(dx, dy));
  }
  else {
    location_ = CQChartsKeyLocation::Type::ABS_RECT;

    editHandles_->updateBBox(dx, dy);

    wbbox_ = editHandles_->bbox();

    setAbsolutePlotRect(wbbox_.qrect());

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
editMoveBy(const QPointF &f)
{
  CQChartsKeyLocation::Type locationType = this->location().type();

  if (locationType != CQChartsKeyLocation::Type::ABS_POSITION &&
      locationType != CQChartsKeyLocation::Type::ABS_RECT) {
    location_ = CQChartsKeyLocation::Type::ABS_POSITION;

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

  CQChartsPlotKey *th = const_cast<CQChartsPlotKey *>(this);

  th->doLayout();

  //---

  CQChartsKeyLocation::Type locationType = this->location().type();

  //---

  // calc plot bounding box (full size)
  double x = position_.x(); // left
  double y = position_.y(); // top
  double w = layoutData_.size.width ();
  double h = layoutData_.size.height();

  if (locationType ==CQChartsKeyLocation::Type::ABS_RECT) {
    w = wbbox_.getWidth ();
    h = wbbox_.getHeight();
  }

  //---

  // calc key drawing area (not including scrollbars)
  double sw = layoutData_.scrollAreaWidth;
  double sh = layoutData_.scrollAreaHeight;

  CQChartsGeom::BBox sbbox(x, y - sh, x + sw, y);

  //---

  // set displayed bbox
  if (locationType != CQChartsKeyLocation::Type::ABS_RECT) {
    wbbox_ = CQChartsGeom::BBox(x, y - h, x + w, y);
  }

  //---

  // calc pixel bounding box
  CQChartsGeom::Point p1 = plot()->windowToPixel(CQChartsGeom::Point(x    , y    ));
  CQChartsGeom::Point p2 = plot()->windowToPixel(CQChartsGeom::Point(x + w, y - h));

  QRectF pixelRect(p1.x, p1.y, p2.x - p1.x, p2.y - p1.y);

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
  CQChartsGeom::BBox plotPixelRect = plot()->calcPlotPixelRect();

  bool   clipped  = false;
  QRectF clipRect = plotPixelRect.qrect();

  //---

  // get plot data rect
  CQChartsGeom::BBox dataPixelRect = plot()->calcDataPixelRect();

  QRectF dataRect = dataPixelRect.qrect();

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

    scrollData_.vbar->move(p2.x - scrollData_.pixelBarSize - 1, p1.y + phh);
    scrollData_.vbar->resize(scrollData_.pixelBarSize - 2, p2.y - p1.y - phh - hsph - 1);

    //---

    // count number of rows in height
    int    scrollRows   = 0;
    double scrollHeight = sh - 2*ym_ - layoutData_.hbarHeight;

    for (int i = 0; i < numRows; ++i) {
      if (scrollHeight <= 0)
        break;

      ++scrollRows;

      int rh = rowHeights_[i] + 2*ys_;

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

    scrollData_.hbar->move(p1.x + 1, p2.y - scrollData_.pixelBarSize - 1);
    scrollData_.hbar->resize(p2.x - p1.x - vspw, scrollData_.pixelBarSize - 2);

    //---

    // update scroll bar
    int pageStep  = plot()->windowToPixelWidth(sw - layoutData_.vbarWidth);
    int fullWidth = plot()->windowToPixelWidth(layoutData_.fullSize.width());

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
    clipRect = QRectF(p1.x, p1.y + phh, p2.x - p1.x - vspw, p2.y - p1.y - phh - hsph);
  }
  else {
    if (locationType != CQChartsKeyLocation::Type::ABS_POSITION &&
        locationType != CQChartsKeyLocation::Type::ABS_RECT) {
      if (isInsideX()) {
        clipRect.setLeft (dataRect.left ());
        clipRect.setRight(dataRect.right());
      }

      if (isInsideY()) {
        clipRect.setTop   (dataRect.top   ());
        clipRect.setBottom(dataRect.bottom());
      }
    }

    clipped = isClipped();
  }

  //---

  // draw box (background)
  CQChartsBoxObj::draw(device, device->pixelToWindow(pixelRect));

  //---

  // draw header
  if (headerStr().length()) {
    // set text options
    CQChartsTextOptions textOptions;

    textOptions.contrast  = isHeaderTextContrast();
    textOptions.formatted = isHeaderTextFormatted();
    textOptions.scaled    = isHeaderTextScaled();
    textOptions.html      = isHeaderTextHtml();
    textOptions.align     = headerTextAlign();

    textOptions = plot()->adjustTextOptions(textOptions);

    //---

    // get font
    QFont font = view()->plotFont(plot(), headerTextFont());

    device->setFont(font);

    //---

    // get key top left, width (pixels), margins
    QPointF p = plot()->windowToPixel(QPointF(x, y)); // top left

    double pw = plot()->windowToPixelWidth(sw);

  //double xm = margin();
    double ym = margin();

    //---

    // calc text rect
    QSizeF tsize = CQChartsDrawUtil::calcTextSize(headerStr(), font, textOptions);

    double tw = pw;
    double th = tsize.height() + 2*ym;

    QRectF trect(p.x(), p.y(), tw, th);

    //---

    // set text pen
    QPen   tpen;
    QBrush tbrush;

    QColor tc = interpHeaderTextColor(ColorInd());

    plot()->setPen(tpen, true, tc, headerTextAlpha());

    plot()->updateObjPenBrushState(this, tpen, tbrush);

    //---

    // draw text
    device->setPen(tpen);

    CQChartsDrawUtil::drawTextInBox(device, device->pixelToWindow(trect), headerStr(), textOptions);
  }

  //---

  if (clipped) {
    QRectF cr = device->pixelToWindow(clipRect);

    //device->setPen(Qt::red);
    //device->drawRect(cr);
    //device->setPen(Qt::black);

    device->setClipRect(cr);
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

  CQChartsPlotKey *th = const_cast<CQChartsPlotKey *>(this);

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

  if (locationType != CQChartsKeyLocation::Type::ABS_POSITION &&
      locationType != CQChartsKeyLocation::Type::ABS_RECT) {
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

  if      (selMod == CQChartsSelMod::REPLACE) {
    for (int i = 0; i < ic_.n; ++i)
      plot->setSetHidden(i, i != ic_.i);
  }
  else if (selMod == CQChartsSelMod::ADD)
    plot->setSetHidden(ic_.i, false);
  else if (selMod == CQChartsSelMod::REMOVE)
    plot->setSetHidden(ic_.i, true);
  else if (selMod == CQChartsSelMod::TOGGLE)
    plot->setSetHidden(ic_.i, ! plot->isSetHidden(ic_.i));

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

QSizeF
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

  return QSizeF(ww, wh);
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

#if 0
  CQChartsGeom::Point p1 =
    plot->windowToPixel(CQChartsGeom::Point(rect.getXMin(), rect.getYMin()));
  CQChartsGeom::Point p2 =
    plot->windowToPixel(CQChartsGeom::Point(rect.getXMax(), rect.getYMin()));

  if (p1.x > p2.x)
    std::swap(p1.x, p2.x);

  double px = p1.x + 2;

  QFontMetricsF fm(device->font());

  if (key_->textAlign() & Qt::AlignRight)
    px = p2.x - 2 - fm.width(text_);

  QPointF tp;

  if (! plot->isInvertY())
    tp = QPointF(px, p1.y - fm.descent() - 2);
  else
    tp = QPointF(px, p1.y + fm.ascent() + 2);
#endif

  CQChartsTextOptions textOptions;

  textOptions.contrast  = key_->isTextContrast();
  textOptions.formatted = key_->isTextFormatted();
  textOptions.scaled    = key_->isTextScaled();
  textOptions.html      = key_->isTextHtml();
  textOptions.align     = key_->textAlign();

  textOptions = plot->adjustTextOptions(textOptions);

  QRectF qrect = rect.qrect();

  CQChartsDrawUtil::drawTextInBox(device, qrect, text_, textOptions);

  //CQChartsDrawUtil::drawSimpleText(device, device->pixelToWindow(tp), text_);
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

QSizeF
CQChartsKeyColorBox::
size() const
{
  CQChartsPlot *plot = key_->plot();

  QFont font = plot->view()->plotFont(plot, key_->textFont());

  QFontMetricsF fm(font);

  double h = fm.height();

  double ww = plot->pixelToWindowWidth (h + 2);
  double wh = plot->pixelToWindowHeight(h + 2);

  return QSizeF(ww, wh);
}

void
CQChartsKeyColorBox::
draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect) const
{
  CQChartsPlot *plot = key_->plot();

  CQChartsGeom::BBox prect = plot->windowToPixel(rect);

  QRectF prect1(QPointF(prect.getXMin() + 2, prect.getYMin() + 2),
                QPointF(prect.getXMax() - 2, prect.getYMax() - 2));

  ColorInd colorInd = calcColorInd();

  QColor bc    = interpStrokeColor(colorInd);
  QBrush brush = fillBrush();

  if (isInside())
    brush.setColor(plot->insideColor(brush.color()));

  device->setPen  (bc);
  device->setBrush(brush);

  CQChartsDrawUtil::drawRoundedPolygon(device,
    device->pixelToWindow(prect1), cornerRadius(), cornerRadius());
}

QBrush
CQChartsKeyColorBox::
fillBrush() const
{
  CQChartsPlot *plot = key_->plot();

  ColorInd ic = calcColorInd();

  QColor c = plot->interpPaletteColor(ic);

  if (plot->isSetHidden(ic.i))
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
