#include <CQChartsTitle.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsView.h>
#include <CQChartsEditHandles.h>
#include <CQChartsUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>

CQChartsTitle::
CQChartsTitle(CQChartsPlot *plot) :
 CQChartsTextBoxObj(plot)
{
  setObjectName("title");

  editHandles_ = new CQChartsEditHandles(plot);

  setTextStr("Title");

  //---

  CQChartsFont font;

  font.incFontSize(4);

  textData_.setFont(font);

  //---

  setFilled (false);
  setStroked(false);
}

CQChartsTitle::
~CQChartsTitle()
{
  delete editHandles_;
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
  CQChartsUtil::testAndSet(selected_, b, [&]() { plot_->drawObjs(); } );
}

void
CQChartsTitle::
redraw(bool wait)
{
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
  CQChartsGeom::BBox bbox = plot_->calcGroupedDataRange();

  //---

  // calc title size
  CQChartsGeom::Size ts = calcSize();

  CQChartsTitleLocation location = this->location();

  CQChartsGeom::Size marginSize = plot_->pixelToWindowSize(CQChartsGeom::Size(8, 8));

  double kx = bbox.getXMid() - ts.width()/2;
  double ky = 0.0;

  CQChartsAxis *xAxis = plot_->xAxis();

  if      (location == CQChartsTitleLocation::Type::TOP) {
    if (! isInsidePlot()) {
      ky = bbox.getYMax() + marginSize.height();

      if (xAxis)
        ky += plot_->calcGroupedXAxisRange(CQChartsAxisSide::Type::TOP_RIGHT).getHeight();
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
        ky -= plot_->calcGroupedXAxisRange(CQChartsAxisSide::Type::BOTTOM_LEFT).getHeight();
    }
    else
      ky = bbox.getYMin() + marginSize.height();
  }
  else {
    ky = bbox.getYMid() - ts.height()/2;
  }

  CQChartsGeom::Point kp(kx, ky);

  if      (location == CQChartsTitleLocation::Type::ABSOLUTE_POSITION) {
    kp = absolutePlotPosition();
  }
  else if (location == CQChartsTitleLocation::Type::ABSOLUTE_RECTANGLE) {
    CQChartsGeom::BBox bbox = absolutePlotRectangle();

    if (bbox.isValid())
      kp = CQChartsGeom::Point(bbox.getUL());
  }

  setPosition(kp);
}

void
CQChartsTitle::
addProperties(CQPropertyViewModel *model, const QString &path, const QString &/*desc*/)
{
  model->addProperty(path, this, "visible"          )->setDesc("Title visible");
  model->addProperty(path, this, "location"         )->setDesc("Title location");
  model->addProperty(path, this, "absolutePosition" )->setDesc("Title absolute position");
  model->addProperty(path, this, "absoluteRectangle")->setDesc("Title absolute rectangle");
  model->addProperty(path, this, "insidePlot"       )->setDesc("Title is inside plot");

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
setAbsolutePlotPosition(const CQChartsGeom::Point &p)
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
setAbsolutePlotRectangle(const CQChartsGeom::BBox &bbox)
{
  setAbsoluteRectangle(CQChartsRect(plot_->windowToView(bbox), CQChartsUnits::VIEW));
}

CQChartsGeom::Size
CQChartsTitle::
calcSize()
{
  if (textStr().length()) {
    // get font
    QFont font = view()->plotFont(plot(), textFont());

    // get pixel size
    CQChartsTextOptions textOptions;

    textOptions.html = isTextHtml();

    CQChartsGeom::Size psize = CQChartsDrawUtil::calcTextSize(textStr(), font, textOptions);

    // convert to window size
    CQChartsGeom::Size wsize = plot_->pixelToWindowSize(psize);

    // add padding and margin
    double xlm = lengthParentWidth (margin().left  ());
    double xrm = lengthParentWidth (margin().right ());
    double ytm = lengthParentHeight(margin().top   ());
    double ybm = lengthParentHeight(margin().bottom());

    CQChartsGeom::Size paddingSize =
      plot_->pixelToWindowSize(CQChartsGeom::Size(padding(), padding()));

    size_ = CQChartsGeom::Size(wsize.width () + 2*paddingSize.width () + xlm + xrm,
                               wsize.height() + 2*paddingSize.height() + ybm + ytm);
  }
  else {
    size_ = CQChartsGeom::Size();
  }

  return size_;
}

bool
CQChartsTitle::
contains(const CQChartsGeom::Point &p) const
{
  if (! isVisible())
    return false;

  return bbox().inside(p);
}

//------

bool
CQChartsTitle::
editPress(const CQChartsGeom::Point &p)
{
  editHandles_->setDragPos(p);

  if (location() != CQChartsTitleLocation::Type::ABSOLUTE_POSITION &&
      location() != CQChartsTitleLocation::Type::ABSOLUTE_RECTANGLE) {
    setLocation(CQChartsTitleLocation::Type::ABSOLUTE_POSITION);

    setAbsolutePlotPosition(position_);
  }

  return true;
}

bool
CQChartsTitle::
editMove(const CQChartsGeom::Point &p)
{
  const CQChartsGeom::Point& dragPos  = editHandles_->dragPos();
  const CQChartsResizeSide&  dragSide = editHandles_->dragSide();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  if (location() == CQChartsTitleLocation::Type::ABSOLUTE_POSITION &&
      dragSide == CQChartsResizeSide::MOVE) {
    setLocation(CQChartsTitleLocation::Type::ABSOLUTE_POSITION);

    setAbsolutePlotPosition(absolutePlotPosition() + CQChartsGeom::Point(dx, dy));
  }
  else {
    setLocation(CQChartsTitleLocation::Type::ABSOLUTE_RECTANGLE);

    editHandles_->updateBBox(dx, dy);

    setAbsolutePlotRectangle(editHandles_->bbox());
  }

  editHandles_->setDragPos(p);

  redraw(/*wait*/false);

  return true;
}

bool
CQChartsTitle::
editMotion(const CQChartsGeom::Point &p)
{
  return editHandles_->selectInside(p);
}

bool
CQChartsTitle::
editRelease(const CQChartsGeom::Point &)
{
  return true;
}

void
CQChartsTitle::
editMoveBy(const CQChartsGeom::Point &d)
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
  CQChartsGeom::BBox clipRect = plot_->calcPlotRect();

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

    bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);
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

  CQChartsGeom::Size paddingSize =
    device->pixelToWindowSize(CQChartsGeom::Size(padding(), padding()));

  double xlm = device->lengthWindowWidth (margin().left  ());
  double xrm = device->lengthWindowWidth (margin().right ());
  double ytm = device->lengthWindowHeight(margin().top   ());
  double ybm = device->lengthWindowHeight(margin().bottom());

  CQChartsGeom::BBox ibbox(x     + paddingSize.width(), y     + paddingSize.height(),
                           x + w - paddingSize.width(), y + h - paddingSize.height());

  CQChartsGeom::BBox tbbox(x + paddingSize.width () + xlm,
                           y + paddingSize.height() + ybm,
                           x + w - paddingSize.width () - xlm - xrm,
                           y + h - paddingSize.height() - ybm - ytm);

  //---

  CQChartsBoxObj::draw(device, ibbox);

  //---

  // set text pen
  QPen pen;

  QColor tc = interpTextColor(ColorInd());

  plot()->setPen(pen, true, tc, textAlpha());

  device->setPen(pen);

  //---

  // set text options
  CQChartsTextOptions textOptions;

  textOptions.angle         = textAngle();
  textOptions.contrast      = isTextContrast();
  textOptions.contrastAlpha = textContrastAlpha();
  textOptions.formatted     = true;
  textOptions.html          = isTextHtml();
  textOptions.clipped       = false;
  textOptions.align         = textAlign();

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

  if (location() != CQChartsTitleLocation::Type::ABSOLUTE_RECTANGLE)
    const_cast<CQChartsTitle *>(this)->editHandles_->setBBox(this->bbox());

  editHandles_->draw(painter);
}
