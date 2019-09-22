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

#include <QRectF>

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
  CQChartsGeom::BBox bbox = plot_->calcDataRange();

  if (bbox.isSet())
    bbox += plot_->annotationBBox();
  else
    bbox = CQChartsGeom::BBox(0, 0, 1, 1);

  //---

  // calc title size
  QSizeF ts = calcSize();

  CQChartsTitleLocation location = this->location();

  QSizeF marginSize = plot_->pixelToWindowSize(QSizeF(8, 8));

  double kx = bbox.getXMid() - ts.width()/2;
  double ky = 0.0;

  CQChartsAxis *xAxis = plot_->xAxis();

  if      (location == CQChartsTitleLocation::Type::TOP) {
    if (! isInsidePlot()) {
      ky = bbox.getYMax() + marginSize.height();

      if (xAxis && xAxis->side() == CQChartsAxisSide::Type::TOP_RIGHT)
        ky += xAxis->bbox().getHeight();
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

      if (xAxis && xAxis->side() == CQChartsAxisSide::Type::BOTTOM_LEFT)
        ky -= xAxis->bbox().getHeight();
    }
    else
      ky = bbox.getYMin() + marginSize.height();
  }
  else {
    ky = bbox.getYMid() - ts.height()/2;
  }

  QPointF kp(kx, ky);

  if      (location == CQChartsTitleLocation::Type::ABS_POS) {
    kp = absolutePlotPosition();
  }
  else if (location == CQChartsTitleLocation::Type::ABS_RECT) {
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

QPointF
CQChartsTitle::
absolutePlotPosition() const
{
  return plot_->positionToPlot(absolutePosition());
}

void
CQChartsTitle::
setAbsolutePlotPosition(const QPointF &p)
{
  setAbsolutePosition(CQChartsPosition(plot_->windowToView(p), CQChartsUnits::VIEW));
}

QSizeF
CQChartsTitle::
calcSize()
{
  if (textStr().length()) {
    // get font
    QFont font = view()->plotFont(plot(), textFont());

    // get pixel size
    CQChartsTextOptions textOptions;

    textOptions.html = isTextHtml();

    QSizeF psize = CQChartsDrawUtil::calcTextSize(textStr(), font, textOptions);

    // convert to window size
    QSizeF wsize = plot_->pixelToWindowSize(psize);

    // add padding and margin
    QSizeF paddingSize = plot_->pixelToWindowSize(QSizeF(padding(), padding()));
    QSizeF marginSize  = plot_->pixelToWindowSize(QSizeF(margin (), margin ()));

    size_ = QSizeF(wsize.width () + 2*paddingSize.width () + 2*marginSize.width (),
                   wsize.height() + 2*paddingSize.height() + 2*marginSize.height());
  }
  else {
    size_ = QSizeF();
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

  if (location() != CQChartsTitleLocation::Type::ABS_POS &&
      location() != CQChartsTitleLocation::Type::ABS_RECT) {
    setLocation(CQChartsTitleLocation::Type::ABS_POS);

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

  if (location() == CQChartsTitleLocation::Type::ABS_POS &&
      dragSide == CQChartsResizeSide::MOVE) {
    setLocation(CQChartsTitleLocation::Type::ABS_POS);

    setAbsolutePlotPosition(absolutePlotPosition() + QPointF(dx, dy));
  }
  else {
    setLocation(CQChartsTitleLocation::Type::ABS_RECT);

    editHandles_->updateBBox(dx, dy);

    bbox_ = editHandles_->bbox();

    setAbsoluteRectangle(bbox_.qrect());
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
editMoveBy(const QPointF &d)
{
  setLocation(CQChartsTitleLocation::Type::ABS_POS);

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
  QRectF clipRect = plot_->calcPlotRect().qrect();

  device->setClipRect(clipRect);

  //---

  if (location() != CQChartsTitleLocation::Type::ABS_RECT)
    updateLocation();

  //---

  double x, y, w, h;

  if (location() != CQChartsTitleLocation::Type::ABS_RECT) {
    x = position_.x(); // bottom
    y = position_.y(); // top
    w = size_.width ();
    h = size_.height();

    bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);
  }
  else {
    x = bbox_.getXMin  ();
    y = bbox_.getYMin  ();
    w = bbox_.getWidth ();
    h = bbox_.getHeight();
  }

  QSizeF paddingSize = device->pixelToWindowSize(QSizeF(padding(), padding()));
  QSizeF marginSize  = device->pixelToWindowSize(QSizeF(margin (), margin ()));

  CQChartsGeom::BBox ibbox(x     + paddingSize.width(), y     + paddingSize.height(),
                           x + w - paddingSize.width(), y + h - paddingSize.height());

  CQChartsGeom::BBox tbbox(x + paddingSize.width () + marginSize.width (),
                           y + paddingSize.height() + marginSize.height(),
                           x + w - paddingSize.width () - marginSize.width (),
                           y + h - paddingSize.height() - marginSize.height());

  //---

  CQChartsBoxObj::draw(device, ibbox.qrect());

  //---

  // set text pen
  QPen pen;

  QColor tc = interpTextColor(ColorInd());

  plot()->setPen(pen, true, tc, textAlpha());

  device->setPen(pen);

  //---

  // set text options
  CQChartsTextOptions textOptions;

  textOptions.angle     = textAngle();
  textOptions.contrast  = isTextContrast();
  textOptions.formatted = true;
  textOptions.html      = isTextHtml();
  textOptions.clipped   = false;
  textOptions.align     = textAlign();

  textOptions = plot_->adjustTextOptions(textOptions);

  //---

  // set font
  view()->setPlotPainterFont(plot(), device, textFont());

  //---

  // draw text
  device->setRenderHints(QPainter::Antialiasing);

  CQChartsDrawUtil::drawTextInBox(device, tbbox.qrect(), textStr(), textOptions);

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

  if (location() != CQChartsTitleLocation::Type::ABS_RECT)
    const_cast<CQChartsTitle *>(this)->editHandles_->setBBox(this->bbox());

  editHandles_->draw(painter);
}
