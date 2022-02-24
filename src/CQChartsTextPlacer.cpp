#include <CQChartsTextPlacer.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPenBrush.h>

CQChartsTextPlacer::
CQChartsTextPlacer()
{
}

CQChartsTextPlacer::
~CQChartsTextPlacer()
{
  clear();
}

void
CQChartsTextPlacer::
setDebug(bool b)
{
  placer_.setDebug(b);
}

void
CQChartsTextPlacer::
addDrawText(PaintDevice *device, const QString &str, const Point &point,
            const TextOptions &textOptions, const Point &targetPoint,
            int margin, bool centered)
{
  auto bbox = CQChartsDrawUtil::calcTextAtPointRect(device, point, str, textOptions);

  if (margin > 0) {
    auto dxt = margin*device->pixelToWindowWidth (1);
    auto dyt = margin*device->pixelToWindowHeight(1);

    bbox.expand(-dxt, -dyt, dxt, dyt);
  }

  auto color = device->pen().color();

  auto *drawText = new DrawText(str, point, textOptions, color, Alpha(), targetPoint,
                                device->font(), centered);

  drawText->setBBox(bbox);

  addDrawText(drawText);
}

void
CQChartsTextPlacer::
addDrawText(DrawText *drawText)
{
  drawTexts_.push_back(drawText);
}

void
CQChartsTextPlacer::
place(const BBox &bbox)
{
  placer_.setClipRect(CQChartsRectPlacer::Rect(bbox.getXMin(), bbox.getYMin(),
                                               bbox.getXMax(), bbox.getYMax()));

  for (const auto &drawText : drawTexts_)
    placer_.addRect(drawText);

  placer_.place();
}

void
CQChartsTextPlacer::
draw(PaintDevice *device)
{
  for (const auto *drawText : drawTexts_) {
    CQChartsPenBrush penBrush;

    CQChartsUtil::setPen(penBrush.pen, true, drawText->color, drawText->alpha);

    device->setFont(drawText->font);

    BBox bbox(drawText->rect().xmin(), drawText->rect().ymin(),
              drawText->rect().xmax(), drawText->rect().ymax());

    //device->drawRect(bbox);

    device->setPen(penBrush.pen);

    CQChartsDrawUtil::drawTextAtPoint(device, drawText->point, drawText->str, drawText->options);

    if (drawText->point != drawText->origPoint) {
      CQChartsUtil::setPen(penBrush.pen, true, drawText->color, Alpha(0.4));

      device->setPen(penBrush.pen);

      auto bbox = CQChartsDrawUtil::calcTextAtPointRect(device, drawText->point, drawText->str,
                                                        drawText->options);

      Qt::Orientation orient;
      auto p = CQChartsUtil::nearestRectPoint(bbox, drawText->targetPoint, orient);

      device->drawLine(p, drawText->targetPoint);
    }
  }
}

void
CQChartsTextPlacer::
clear()
{
  for (const auto &drawText : drawTexts_)
    delete drawText;

  drawTexts_.clear();

  placer_.clear();
}

//------

CQChartsTextPlacer::DrawText::
DrawText(const QString &str, const Point &point, const TextOptions &options,
         const QColor &color, const Alpha &alpha, const Point &targetPoint,
         const QFont &font, bool centered) :
 str(str), point(point), options(options), color(color), alpha(alpha), targetPoint(targetPoint),
 font(font), centered(centered)
{
  origPoint = point;
}

void
CQChartsTextPlacer::DrawText::
setBBox(const BBox &bbox)
{
  textRect = Rect(bbox.getXMin(), bbox.getYMin(), bbox.getXMax(), bbox.getYMax());
  origRect = textRect;
}

const CQChartsRectPlacer::Rect &
CQChartsTextPlacer::DrawText::
rect() const
{
  return textRect;
}

void
CQChartsTextPlacer::DrawText::
setRect(const Rect &r)
{
  textRect = r;

  double dx = textRect.xmin() - origRect.xmin();
  double dy = textRect.ymin() - origRect.ymin();

  point = Point(origPoint.x + dx, origPoint.y + dy);
}

std::string
CQChartsTextPlacer::DrawText::
name() const
{
  return str.toStdString();
}

//------

void
CQChartsAxisTextPlacer::
clear()
{
  drawTexts_.clear();
}

void
CQChartsAxisTextPlacer::
autoHide()
{
  auto n = drawTexts_.size();
  if (n <= 1) return;

  const auto &firstBBox = drawTexts_[0    ].bbox;
  const auto &lastBBox  = drawTexts_[n - 1].bbox;

  // if first and last labels overlap then only draw first
  if (lastBBox.overlaps(firstBBox)) {
    for (size_t i = 1; i < n; ++i)
      drawTexts_[i].visible = false;
  }
  // otherwise draw first and last and clip others
  else {
    auto prevBBox = firstBBox;

    for (size_t i = 1; i < n - 1; ++i) {
      auto &data = drawTexts_[i];

      if (data.bbox.overlaps(prevBBox) || data.bbox.overlaps(lastBBox))
        data.visible = false;

      if (data.visible)
        prevBBox = data.bbox;
    }
  }
}

void
CQChartsAxisTextPlacer::
draw(PaintDevice *device, const TextOptions &textOptions, bool showBoxes)
{
  for (const auto &data : drawTexts_) {
    if (! data.visible)
      continue;

    auto p1 = device->pixelToWindow(data.p);

    TextOptions options = textOptions;

    options.angle = data.angle;
    options.align = data.align;

    CQChartsDrawUtil::drawTextAtPoint(device, p1, data.text, options, /*centered*/true);
  }

  if (showBoxes) {
    for (const auto &data : drawTexts_) {
      if (! data.visible)
        continue;

      device->setPen(QColor(Qt::red));
      device->setBrush(Qt::NoBrush);

      device->drawRect(data.bbox);
    }
  }
}
