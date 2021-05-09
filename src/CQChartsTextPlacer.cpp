#include <CQChartsTextPlacer.h>

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
  for (const auto &drawText : drawTexts_) {
    CQChartsPenBrush penBrush;

    CQChartsUtil::setPen(penBrush.pen, true, drawText->color, drawText->alpha);

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

      auto p = CQChartsUtil::nearestRectPoint(bbox, drawText->targetPoint);

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
         const QColor &color, const Alpha &alpha, const Point &targetPoint) :
  str(str), point(point), options(options), color(color), alpha(alpha), targetPoint(targetPoint)
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
