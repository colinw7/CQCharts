#include <CQChartsHtmlPaintDevice.h>

CQChartsHtmlPaintDevice::
CQChartsHtmlPaintDevice(CQChartsView *view, std::ostream &os) :
 CQChartsPaintDevice(view), os_(&os)
{
}

CQChartsHtmlPaintDevice::
CQChartsHtmlPaintDevice(CQChartsPlot *plot, std::ostream &os) :
 CQChartsPaintDevice(plot), os_(&os)
{
}

QPen
CQChartsHtmlPaintDevice::
pen() const
{
  return data_.pen;
}

QBrush
CQChartsHtmlPaintDevice::
brush() const
{
  return data_.brush;
}

const QFont &
CQChartsHtmlPaintDevice::
font() const
{
  return data_.font;
}

void
CQChartsHtmlPaintDevice::
resetData()
{
  data_.reset();
}

void
CQChartsHtmlPaintDevice::
setTransformRotate(const Point &p, double angle)
{
  QTransform t = data_.transform;

  t.translate(p.x, p.y);
  t.rotate(angle);

  data_.transform      = t;
  data_.transformPoint = p;
  data_.transformAngle = angle;
}

const QTransform &
CQChartsHtmlPaintDevice::
transform() const
{
  return data_.transform;
}

void
CQChartsHtmlPaintDevice::
setRenderHints(QPainter::RenderHints, bool)
{
}

void
CQChartsHtmlPaintDevice::
setColorNames()
{
  setColorNames("strokeColor", "fillColor");
}

void
CQChartsHtmlPaintDevice::
setColorNames(const QString &strokeName, const QString &fillName)
{
  setStrokeStyleName(strokeName);
  setFillStyleName  (fillName  );
}

void
CQChartsHtmlPaintDevice::
resetColorNames()
{
  setStrokeStyleName("");
  setFillStyleName  ("");
}

void
CQChartsHtmlPaintDevice::
createButton(const BBox &bbox, const QString &text, const QString &id, const QString &clickProc)
{
  if (! bbox.isValid()) return;

  auto pbbox = windowToPixel(bbox);

  *os_ << "<button id=\"" << id.toStdString() << "\" style=\"position:absolute;"
          " left:"  << pbbox.getXMin () << "px; top:"    << pbbox.getYMin  () << "px;"
          " width:" << pbbox.getWidth() << "px; height:" << pbbox.getHeight() << "px;";

  QString styleName = data_.font.styleName();

  if (! styleName.length())
    styleName = "normal";

  *os_ << " font-family:" << data_.font.family().toStdString() << ";" <<
          " font-size:" << data_.font.pointSizeF() << ";" <<
          " font-weight:" << data_.font.weight() << ";" <<
          " font-style:" << styleName.toStdString() << ";";

  *os_ << "\"";

  if (clickProc.length())
    *os_ << " onclick=\"" << clickProc.toStdString() << "('" << id.toStdString() << "')\"";

  *os_ << ">" << text.toStdString() << "</button>\n";
}
