#include <CQChartsTitle.h>
#include <CQChartsPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsView.h>
#include <CQChartsUtil.h>
#include <CQPropertyViewModel.h>
#include <QPainter>
#include <QRectF>

CQChartsTitle::
CQChartsTitle(CQChartsPlot *plot) :
 CQChartsTextBoxObj(plot), editHandles_(plot)
{
  setObjectName("title");

  setTextStr("Title");

  textData_.font.setPointSizeF(1.2*textFont().pointSizeF());

  boxData_.shape.background.visible = false;
  boxData_.shape.border    .visible = false;
}

void
CQChartsTitle::
redraw()
{
  plot_->update();
}

QString
CQChartsTitle::
locationStr() const
{
  switch (location_.location) {
    case LocationType::TOP:      return "top";
    case LocationType::CENTER:   return "center";
    case LocationType::BOTTOM:   return "bottom";
    case LocationType::ABS_POS:  return "abs_pos";
    case LocationType::ABS_RECT: return "abs_rect";
    default:                     return "none";
  }
}

void
CQChartsTitle::
setLocationStr(const QString &str)
{
  QString lstr = str.toLower();

  if      (lstr == "top"     ) location_.location = LocationType::TOP;
  else if (lstr == "center"  ) location_.location = LocationType::CENTER;
  else if (lstr == "bottom"  ) location_.location = LocationType::BOTTOM;
  else if (lstr == "abs_pos" ) location_.location = LocationType::ABS_POS;
  else if (lstr == "abs_rect") location_.location = LocationType::ABS_RECT;

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

  LocationType location = this->location();

//double xm = plot_->pixelToWindowWidth (8);
  double ym = plot_->pixelToWindowHeight(8);

  double kx = bbox.getXMid() - ts.width()/2;
  double ky = 0.0;

  CQChartsAxis *xAxis = plot_->xAxis();

  if      (location == CQChartsTitle::LocationType::TOP) {
    if (! isInside()) {
      ky = bbox.getYMax() + ym;

      if (xAxis && xAxis->side() == CQChartsAxis::Side::TOP_RIGHT)
        ky += xAxis->bbox().getHeight();
    }
    else
      ky = bbox.getYMax() - ts.height() - ym;
  }
  else if (location == CQChartsTitle::LocationType::CENTER) {
    ky = bbox.getYMid() - ts.height()/2;
  }
  else if (location == CQChartsTitle::LocationType::BOTTOM) {
    if (! isInside()) {
      ky = bbox.getYMin() - ts.height() - ym;

      if (xAxis && xAxis->side() == CQChartsAxis::Side::BOTTOM_LEFT)
        ky -= xAxis->bbox().getHeight();
    }
    else
      ky = bbox.getYMin() + ym;
  }
  else {
    ky = bbox.getYMid() - ts.height()/2;
  }

  QPointF kp(kx, ky);

  if      (location == LocationType::ABS_POS) {
    kp = absPlotPosition();
  }
  else if (location == LocationType::ABS_RECT) {
  }

  setPosition(kp);
}

void
CQChartsTitle::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "visible"    );
  model->addProperty(path, this, "location"   );
  model->addProperty(path, this, "absPosition");
  model->addProperty(path, this, "absRect"    );
  model->addProperty(path, this, "inside"     );

  CQChartsTextBoxObj::addProperties(model, path);
}

QPointF
CQChartsTitle::
absPlotPosition() const
{
  return plot_->positionToPlot(absPosition());
}

void
CQChartsTitle::
setAbsPlotPosition(const QPointF &p)
{
  double vx, vy;

  plot_->windowToView(p.x(), p.y(), vx, vy);

  setAbsPosition(CQChartsPosition(QPointF(vx, vy), CQChartsPosition::Units::VIEW));
}

QSizeF
CQChartsTitle::
calcSize()
{
  if (textStr().length()) {
    QFontMetricsF fm(textFont());

    double pw = fm.width(textStr());
    double ph = fm.height();

    double ww = plot_->pixelToWindowWidth (pw);
    double wh = plot_->pixelToWindowHeight(ph);

    double xp = plot_->pixelToWindowWidth (padding());
    double yp = plot_->pixelToWindowHeight(padding());

    double xm = plot_->pixelToWindowWidth (margin());
    double ym = plot_->pixelToWindowHeight(margin());

    size_ = QSizeF(ww + 2*xp + 2*xm, wh + 2*yp + 2*ym);
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
  editHandles_.setDragPos(p);

  if (location_.location != LocationType::ABS_POS &&
      location_.location != LocationType::ABS_RECT) {
    location_.location = LocationType::ABS_POS;

    setAbsPlotPosition(position_);
  }

  return true;
}

bool
CQChartsTitle::
editMove(const CQChartsGeom::Point &p)
{
  const CQChartsGeom::Point        &dragPos  = editHandles_.dragPos();
  const CQChartsResizeHandle::Side &dragSide = editHandles_.dragSide();

  double dx = p.x - dragPos.x;
  double dy = p.y - dragPos.y;

  if (location_.location == LocationType::ABS_POS &&
      dragSide == CQChartsResizeHandle::Side::MOVE) {
    location_.location = LocationType::ABS_POS;

    setAbsPlotPosition(absPlotPosition() + QPointF(dx, dy));
  }
  else {
    location_.location = LocationType::ABS_RECT;

    editHandles_.updateBBox(dx, dy);

    bbox_ = editHandles_.bbox();

    setAbsRect(CQChartsUtil::toQRect(bbox_));
  }

  editHandles_.setDragPos(p);

  redraw();

  return true;
}

bool
CQChartsTitle::
editMotion(const CQChartsGeom::Point &p)
{
  return editHandles_.selectInside(p);
}

bool
CQChartsTitle::
editRelease(const CQChartsGeom::Point &)
{
  return true;
}

//------

void
CQChartsTitle::
draw(QPainter *painter)
{
  if (! isVisible())
    return;

  if (! textStr().length())
     return;

  //---

  painter->save();

  //---

  // clip to plot
  QRectF clipRect = CQChartsUtil::toQRect(plot_->calcPixelRect());

  painter->setClipRect(clipRect);

  //---

  if (location_.location != LocationType::ABS_RECT)
    updateLocation();

  //---

  double x, y, w, h;

  if (location_.location != LocationType::ABS_RECT) {
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

  double xp = plot_->pixelToWindowWidth (padding());
  double yp = plot_->pixelToWindowHeight(padding());
  double xm = plot_->pixelToWindowWidth (margin ());
  double ym = plot_->pixelToWindowHeight(margin ());

  CQChartsGeom::BBox ibbox(x + xp     , y + yp     , x + w - xp     , y + h - yp     );
  CQChartsGeom::BBox tbbox(x + xp + xm, y + yp + ym, x + w - xp - xm, y + h - yp - ym);

  //---

  CQChartsGeom::BBox prect, pirect, ptrect;

  plot_->windowToPixel(bbox_, prect);
  plot_->windowToPixel(ibbox, pirect);
  plot_->windowToPixel(tbbox, ptrect);

  //---

  CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(pirect));

  //---

  QColor tc = interpTextColor(0, 1);

  tc.setAlphaF(textAlpha());

  QPen pen(tc);

  painter->setPen (pen);
  painter->setFont(textFont());

  CQChartsTextOptions textOptions;

  textOptions.angle     = textAngle();
  textOptions.contrast  = isTextContrast();
  textOptions.formatted = true;
  textOptions.clipped   = false;
  textOptions.align     = textAlign();

  plot_->drawTextInBox(painter, CQChartsUtil::toQRect(ptrect), textStr(), pen, textOptions);

  //---

  if (plot_->showBoxes())
    plot_->drawWindowColorBox(painter, bbox_, Qt::red);

  //---

  if (isSelected()) {
    if (location_.location != LocationType::ABS_RECT)
      editHandles_.setBBox(this->bbox());

    editHandles_.draw(painter);
  }

  //---

  painter->restore();
}
