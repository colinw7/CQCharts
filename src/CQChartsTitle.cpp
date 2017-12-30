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
 CQChartsTextBoxObj(plot)
{
  setTextStr("Title");

  textFont_.setPointSizeF(1.5*textFont().pointSizeF());
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
    case LocationType::ABSOLUTE: return "abs";
    default:                     return "none";
  }
}

void
CQChartsTitle::
setLocationStr(const QString &str)
{
  QString lstr = str.toLower();

  if      (lstr == "top"   ) location_.location = LocationType::TOP;
  else if (lstr == "center") location_.location = LocationType::CENTER;
  else if (lstr == "bottom") location_.location = LocationType::BOTTOM;
  else if (lstr == "abs"   ) location_.location = LocationType::ABSOLUTE;

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

  if (location == LocationType::ABSOLUTE) {
    kp = absPlotPosition();
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
  model->addProperty(path, this, "inside"     );

  CQChartsTextBoxObj::addProperties(model, path);
}

QPointF
CQChartsTitle::
absPlotPosition() const
{
  double wx, wy;

  plot_->viewToWindow(absPosition().x(), absPosition().y(), wx, wy);

  return QPointF(wx, wy);
}

void
CQChartsTitle::
setAbsPlotPosition(const QPointF &p)
{
  double vx, vy;

  plot_->windowToView(p.x(), p.y(), vx, vy);

  setAbsPosition(QPointF(vx, vy));
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
mouseDragPress(const CQChartsGeom::Point &p)
{
  dragPos_ = p;

  location_.location = LocationType::ABSOLUTE;

  setAbsPlotPosition(position_);

  return true;
}

bool
CQChartsTitle::
mouseDragMove(const CQChartsGeom::Point &p)
{
  double dx = p.x - dragPos_.x;
  double dy = p.y - dragPos_.y;

  location_.location = LocationType::ABSOLUTE;

  setAbsPlotPosition(absPlotPosition() + QPointF(dx, dy));

  dragPos_ = p;

  redraw();

  return true;
}

void
CQChartsTitle::
mouseDragRelease(const CQChartsGeom::Point &)
{
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

  QRectF clipRect = CQChartsUtil::toQRect(plot_->calcPixelRect());

  painter->setClipRect(clipRect);

  //---

  updateLocation();

  //---

  double x = position_.x(); // bottom
  double y = position_.y(); // top
  double w = size_.width ();
  double h = size_.height();

  bbox_ = CQChartsGeom::BBox(x, y, x + w, y + h);

  double xp = plot_->pixelToWindowWidth (padding());
  double yp = plot_->pixelToWindowHeight(padding());

  CQChartsGeom::BBox ibbox = CQChartsGeom::BBox(x + xp, y + yp, x + w - xp, y + h - yp);

  //---

  CQChartsGeom::BBox prect, pirect;

  plot_->windowToPixel(bbox_, prect);
  plot_->windowToPixel(ibbox, pirect);

  //---

  CQChartsBoxObj::draw(painter, CQChartsUtil::toQRect(pirect));

  //---

  QFontMetricsF fm(textFont());

  painter->setFont(textFont());

  painter->setPen(interpTextColor(0, 1));

  painter->drawText(QPointF(pirect.getXMin() + margin(),
                             pirect.getYMax() - margin() - fm.descent()), textStr());

  //---

  if (plot_->showBoxes())
    plot_->drawWindowRedBox(painter, bbox_);

  //---

  painter->restore();
}
