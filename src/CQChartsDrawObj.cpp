#include <CQChartsDrawObj.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsUtil.h>

CQChartsDrawObj::
CQChartsDrawObj()
{
}

CQChartsDrawObj::
~CQChartsDrawObj()
{
  for (auto &c : children_)
    delete c;
}

void
CQChartsDrawObj::
addChild(DrawObj *obj)
{
  obj->parent_ = this;

  children_.push_back(obj);
}

void
CQChartsDrawObj::
place(PaintDevice *device, const BBox &placeBBox)
{
  auto bbox = calcBBox(device);

  double xs = placeBBox.getWidth ()/bbox.getWidth ();
  double ys = placeBBox.getHeight()/bbox.getHeight();

  scale_ = std::min(xs, ys);

  if (maxScale_ > 0.0)
    scale_ = std::min(scale_, maxScale_);

  double dx = (placeBBox.getWidth () - scale_*bbox.getWidth ())/2.0;
  double dy = (placeBBox.getHeight() - scale_*bbox.getHeight())/2.0;

  for (const auto &c : children_)
    c->moveScale(placeBBox.getXMin() + dx, placeBBox.getYMin() + dy, scale_);
}

CQChartsGeom::BBox
CQChartsDrawObj::
calcBBox(PaintDevice *device) const
{
  auto bbox = BBox();

  for (const auto &c : children_)
    bbox += c->calcBBox(device);

  return bbox;
}

void
CQChartsDrawObj::
moveScale(double dx, double dy, double s)
{
  scale_ = s;

  double x1 = bbox_.getXMin()*s + dx;
  double y1 = bbox_.getYMin()*s + dy;
  double x2 = bbox_.getXMax()*s + dx;
  double y2 = bbox_.getYMax()*s + dy;

  sbbox_ = BBox(x1, y1, x2, y2);
}

void
CQChartsDrawObj::
draw(PaintDevice *device)
{
  for (const auto &c : children_)
    c->draw(device);

  drawContents(device);
}

//---

CQChartsDrawRect::
CQChartsDrawRect(const BBox &bbox, const PenBrush &penBrush) :
 CQChartsDrawObj()
{
  bbox_     = bbox;
  penBrush_ = penBrush;
}

CQChartsGeom::BBox
CQChartsDrawRect::
calcBBox(PaintDevice *) const
{
  return bbox_;
}

void
CQChartsDrawRect::
drawContents(PaintDevice *device)
{
  CQChartsDrawUtil::setPenBrush(device, penBrush_);

  device->drawRect(sbbox_);
}

//---

CQChartsDrawText::
CQChartsDrawText(const Point &p, const QString &str, const QFont &font, const PenBrush &penBrush) :
 CQChartsDrawObj(), p_(p), str_(str), font_(font)
{
  penBrush_ = penBrush;
}

CQChartsGeom::BBox
CQChartsDrawText::
calcBBox(PaintDevice *device) const
{
  auto psize = CQChartsDrawUtil::calcTextSize(str_, font_, CQChartsTextOptions());

  double tw = device->pixelToWindowWidth (psize.width ());
  double th = device->pixelToWindowHeight(psize.height());

  const_cast<CQChartsDrawText *>(this)->bbox_ = BBox(p_.x, p_.y - th/2.0, p_.x + tw, p_.y + th/2.0);

  return bbox_;
}

void
CQChartsDrawText::
drawContents(PaintDevice *device)
{
  auto font = CQChartsUtil::scaleFontSize(font_, scale_, 0.1, 1000.0);

  device->save();

  device->setFont(font);

  QFontMetrics fm(device->font());

  //double fa = device->pixelToWindowHeight(fm.ascent ());
  //double fd = device->pixelToWindowHeight(fm.descent());

  auto p = Point(sbbox_.getXMid(), sbbox_.getYMid());

  CQChartsTextOptions options;

  options.align = Qt::AlignHCenter | Qt::AlignVCenter;

  CQChartsDrawUtil::setPenBrush(device, penBrush_);

  CQChartsDrawUtil::drawTextAtPoint(device, p, str_, options);

  device->restore();
}
