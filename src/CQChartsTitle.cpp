#include <CQChartsTitle.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsEditHandles.h>
#include <CQChartsPaintDevice.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

CQChartsTitle::
CQChartsTitle(CQChartsPlot *plot) :
 CQChartsTextBoxObj(plot)
{
  setObjectName("title");

  setTextStr("Title");

  //---

  CQChartsFont font;

  font.incFontSize(4);

  textData_.setFont(font);

  //---

  setTextColor(CQChartsColor(CQChartsColor::Type::INTERFACE_VALUE, 1.0));

  setFilled (false);
  setStroked(false);
}

CQChartsTitle::
~CQChartsTitle()
{
}

QString
CQChartsTitle::
calcId() const
{
  return plot_->id() + "/title";
}

void
CQChartsTitle::
setSelected(bool b)
{
  CQChartsUtil::testAndSet(selected_, b, [&]() { redraw(); } );
}

void
CQChartsTitle::
setLocation(const CQChartsTitleLocation &l)
{
  CQChartsUtil::testAndSet(location_, l, [&]() { redraw(); } );
}

void
CQChartsTitle::
setAbsolutePosition(const CQChartsPosition &p)
{
  CQChartsUtil::testAndSet(absolutePosition_, p, [&]() { redraw(); } );
}

void
CQChartsTitle::
setAbsoluteRectangle(const CQChartsRect &r)
{
  CQChartsUtil::testAndSet(absoluteRectangle_, r, [&]() { redraw(); } );
}

void
CQChartsTitle::
setInsidePlot(bool b)
{
  CQChartsUtil::testAndSet(insidePlot_, b, [&]() { updateLocation(); redraw(); } );
}

void
CQChartsTitle::
setExpandWidth(bool b)
{
  CQChartsUtil::testAndSet(expandWidth_, b, [&]() { updateLocation(); redraw(); } );
}

void
CQChartsTitle::
redraw(bool wait)
{
  plot_->emitTitleChanged();

  if (wait)
    plot_->drawForeground();
  else
    plot_->invalidateLayer(CQChartsBuffer::Type::FOREGROUND);
}

QString
CQChartsTitle::
locationStr() const
{
  return location().toString();
}

void
CQChartsTitle::
setLocationStr(const QString &str)
{
  setLocation(CQChartsTitleLocation(str));

  redraw();
}

void
CQChartsTitle::
updateLocation()
{
  bbox_ = BBox();

  // all overlay plots and annotation boxes
  auto bbox = plot_->calcGroupedDataRange(
                CQChartsPlot::RangeTypes().setAnnotation().setAxes().setKey());

  //---

  // calc title size
  auto ts = calcSize();

  auto location = this->location();

//auto marginSize = plot_->pixelToWindowSize(Size(8, 8));
  auto marginSize = plot_->pixelToWindowSize(Size(0, 0));

  double kx = bbox.getXMid() - ts.width()/2;
  double ky = 0.0;

  auto *xAxis = plot_->xAxis();

  if      (location == CQChartsTitleLocation::Type::TOP) {
    if (! isInsidePlot()) {
      ky = bbox.getYMax() + marginSize.height();

      if (xAxis)
        ky += plot_->calcGroupedXAxisRange(CQChartsAxisSide::Type::TOP_RIGHT).getOptHeight();
    }
    else
      ky = bbox.getYMax() - ts.height() - marginSize.height();
  }
  else if (location == CQChartsTitleLocation::Type::CENTER) {
    ky = bbox.getYMid() - ts.height()/2;
  }
  else if (location == CQChartsTitleLocation::Type::BOTTOM) {
    if (! isInsidePlot()) {
      ky = bbox.getYMin() - ts.height() - marginSize.height();

      if (xAxis)
        ky -= plot_->calcGroupedXAxisRange(CQChartsAxisSide::Type::BOTTOM_LEFT).getOptHeight();
    }
    else
      ky = bbox.getYMin() + marginSize.height();
  }
  else {
    ky = bbox.getYMid() - ts.height()/2;
  }

  Point kp(kx, ky);

  if      (location == CQChartsTitleLocation::Type::ABSOLUTE_POSITION) {
    kp = absolutePlotPosition();
  }
  else if (location == CQChartsTitleLocation::Type::ABSOLUTE_RECTANGLE) {
    auto bbox = absolutePlotRectangle();

    if (bbox.isValid())
      kp = Point(bbox.getUL());
  }

  setPosition(kp);
}

void
CQChartsTitle::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  auto addProp = [&](const QString &name, const QString &desc, bool hidden=false) {
    auto *item = model->addProperty(path, this, name);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

  addProp("visible"          , "Title visible");
  addProp("editable"         , "Title editable");
  addProp("location"         , "Title location");
  addProp("absolutePosition" , "Title absolute position");
  addProp("absoluteRectangle", "Title absolute rectangle");
  addProp("insidePlot"       , "Title is inside plot");
  addProp("expandWidth"      , "Title is sized to plot width");

  auto fitPath = path + "/fit";

  model->addProperty(fitPath, this, "fitHorizontal", "horizontal")->
    setDesc("Fit title horizontally");
  model->addProperty(fitPath, this, "fitVertical"  , "vertical"  )->
    setDesc("Fit title vertically");

  CQChartsTextBoxObj::addProperties(model, path, "");
}

CQChartsGeom::Point
CQChartsTitle::
absolutePlotPosition() const
{
  return plot_->positionToPlot(absolutePosition());
}

void
CQChartsTitle::
setAbsolutePlotPosition(const Point &p)
{
  setAbsolutePosition(CQChartsPosition(plot_->windowToView(p), CQChartsUnits::VIEW));
}

CQChartsGeom::BBox
CQChartsTitle::
absolutePlotRectangle() const
{
  return plot_->rectToPlot(absoluteRectangle());
}

void
CQChartsTitle::
setAbsolutePlotRectangle(const BBox &bbox)
{
  setAbsoluteRectangle(CQChartsRect(plot_->windowToView(bbox), CQChartsUnits::VIEW));
}

CQChartsGeom::Size
CQChartsTitle::
calcSize()
{
  if (textStr().length()) {
    // get font
    auto font = view()->plotFont(plot(), textFont());

    // get pixel size
    CQChartsTextOptions textOptions;

    textOptions.html = isTextHtml();

    auto psize = CQChartsDrawUtil::calcTextSize(textStr(), font, textOptions);

    // convert to window size
    auto wsize = plot_->pixelToWindowSize(psize);

    // add outer margin and inner padding
    double xlm = lengthParentWidth (margin().left  ());
    double xrm = lengthParentWidth (margin().right ());
    double ytm = lengthParentHeight(margin().top   ());
    double ybm = lengthParentHeight(margin().bottom());

    double xlp = lengthParentWidth (padding().left  ());
    double xrp = lengthParentWidth (padding().right ());
    double ytp = lengthParentHeight(padding().top   ());
    double ybp = lengthParentHeight(padding().bottom());

    size_ = Size(wsize.width() + xlp + xrp + xlm + xrm, wsize.height() + ybp + ytp + + ybm + ytm);
  }
  else {
    size_ = Size();
  }

  if (isExpandWidth()) {
    auto bbox = plot_->calcGroupedDataRange();

    size_.setWidth(bbox.getWidth());
  }

  return size_;
}

CQChartsGeom::BBox
CQChartsTitle::
fitBBox() const
{
  auto bbox = this->bbox();

  if (! bbox.isValid())
    return bbox;

  if (isFitHorizontal() && isFitVertical())
    return bbox;

  if (isFitHorizontal())
    return BBox(bbox.getXMin(), bbox.getYMid(), bbox.getXMax(), bbox.getYMid());

  if (isFitVertical())
    return BBox(bbox.getXMid(), bbox.getYMin(), bbox.getXMid(), bbox.getYMax());

  return BBox();
}

bool
CQChartsTitle::
contains(const Point &p) const
{
  if (! isVisible())
    return false;

  return bbox().inside(p);
}

//------

bool
CQChartsTitle::
editPress(const Point &p)
{
  editHandles()->setDragPos(p);

  if (location() != CQChartsTitleLocation::Type::ABSOLUTE_POSITION &&
      location() != CQChartsTitleLocation::Type::ABSOLUTE_RECTANGLE) {
    setLocation(CQChartsTitleLocation::Type::ABSOLUTE_POSITION);

    setAbsolutePlotPosition(position_);
  }

  return true;
}

bool
CQChartsTitle::
editMove(const Point &p)
{
  const auto &dragPos  = editHandles()->dragPos();
  const auto &dragSide = editHandles()->dragSide();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  if (location() == CQChartsTitleLocation::Type::ABSOLUTE_POSITION &&
      dragSide == CQChartsResizeSide::MOVE) {
    setLocation(CQChartsTitleLocation::Type::ABSOLUTE_POSITION);

    setAbsolutePlotPosition(absolutePlotPosition() + Point(dx, dy));
  }
  else {
    setLocation(CQChartsTitleLocation::Type::ABSOLUTE_RECTANGLE);

    editHandles()->updateBBox(dx, dy);

    setAbsolutePlotRectangle(editHandles()->bbox());
  }

  editHandles()->setDragPos(p);

  redraw(/*wait*/false);

  return true;
}

bool
CQChartsTitle::
editMotion(const Point &p)
{
  return editHandles()->selectInside(p);
}

void
CQChartsTitle::
editMoveBy(const Point &d)
{
  setLocation(CQChartsTitleLocation::Type::ABSOLUTE_POSITION);

  setAbsolutePlotPosition(position_ + d);

  redraw(/*wait*/false);
}

//------

bool
CQChartsTitle::
isDrawn() const
{
  if (! isVisible())
    return false;

  if (! textStr().length())
    return false;

  return true;
}

void
CQChartsTitle::
draw(CQChartsPaintDevice *device)
{
  if (! isDrawn())
    return;

  //---

  device->save();

  //---

  // clip to plot
  auto clipRect = plot_->calcPlotRect();

  device->setClipRect(clipRect);

  //---

  if (location() != CQChartsTitleLocation::Type::ABSOLUTE_RECTANGLE)
    updateLocation();

  //---

  double x { 0 }, y { 0 }, w { 1 }, h { 1 };

  if (location() != CQChartsTitleLocation::Type::ABSOLUTE_RECTANGLE) {
    x = position_.x; // bottom
    y = position_.y; // top
    w = size_.width ();
    h = size_.height();

    bbox_ = BBox(x, y, x + w, y + h);
  }
  else {
    bbox_ = absolutePlotRectangle();

    if (bbox_.isValid()) {
      x = bbox_.getXMin  ();
      y = bbox_.getYMin  ();
      w = bbox_.getWidth ();
      h = bbox_.getHeight();
    }
  }

  // add outer margin and inner padding
  double xlm = device->lengthWindowWidth (margin().left  ());
  double xrm = device->lengthWindowWidth (margin().right ());
  double ytm = device->lengthWindowHeight(margin().top   ());
  double ybm = device->lengthWindowHeight(margin().bottom());

  double xlp = device->lengthWindowWidth (padding().left  ());
  double xrp = device->lengthWindowWidth (padding().right ());
  double ytp = device->lengthWindowHeight(padding().top   ());
  double ybp = device->lengthWindowHeight(padding().bottom());

  BBox ibbox(x     + xlp      , y + ybp      , x + w - xrp      , y + h - ytp      );
  BBox tbbox(x     + xlp + xlm, y + ybp + ybm, x + w - xrp - xrm, y + h - ytp - ytm);

  //---

  CQChartsBoxObj::draw(device, ibbox);

  //---

  // set text pen
  CQChartsPenBrush penBrush;

  auto tc = interpTextColor(ColorInd());

  plot()->setPen(penBrush, CQChartsPenData(true, tc, textAlpha()));

  device->setPen(penBrush.pen);

  //---

  // set text options
  CQChartsTextOptions textOptions;

  textOptions.angle         = textAngle();
  textOptions.align         = textAlign();
  textOptions.contrast      = isTextContrast();
  textOptions.contrastAlpha = textContrastAlpha();
  textOptions.formatted     = true;
  textOptions.scaled        = false;
  textOptions.html          = isTextHtml();
  textOptions.clipLength    = lengthPixelWidth(textClipLength());
  textOptions.clipElide     = textClipElide();
  textOptions.clipped       = false;

  textOptions = plot_->adjustTextOptions(textOptions);

  //---

  // set font
  view()->setPlotPainterFont(plot(), device, textFont());

  //---

  // draw text
  device->setRenderHints(QPainter::Antialiasing);

  CQChartsDrawUtil::drawTextInBox(device, tbbox, textStr(), textOptions);

  //---

  if (plot_->showBoxes())
    plot_->drawWindowColorBox(device, bbox_, Qt::red);

  //---

  device->restore();
}

void
CQChartsTitle::
drawEditHandles(QPainter *painter) const
{
  assert(plot_->view()->mode() == CQChartsView::Mode::EDIT && isSelected());

  auto *th = const_cast<CQChartsTitle *>(this);

  if (location() != CQChartsTitleLocation::Type::ABSOLUTE_RECTANGLE)
    th->editHandles()->setBBox(this->bbox());

  editHandles()->draw(painter);
}

void
CQChartsTitle::
textBoxDataInvalidate()
{
  if (! isDrawn())
    bbox_ = BBox();

  plot_->drawObjs();
}
