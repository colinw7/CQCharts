#include <CQChartsTextBoxObj.h>
#include <CQChartsPlot.h>
#include <CQChartsUtil.h>
#include <CQPropertyViewModel.h>
#include <CQChartsRotatedText.h>
#include <QPainter>

CQChartsTextBoxObj::
CQChartsTextBoxObj(CQChartsPlot *plot) :
 CQChartsBoxObj(plot)
{
  CQChartsPaletteColor themeFg(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  setTextColor(themeFg);
}

QString
CQChartsTextBoxObj::
textColorStr() const
{
  return textColor_.colorStr();
}

void
CQChartsTextBoxObj::
setTextColorStr(const QString &s)
{
  textColor_.setColorStr(s);
}

QColor
CQChartsTextBoxObj::
interpTextColor(int i, int n) const
{
  return textColor_.interpColor(plot_, i, n);
}

void
CQChartsTextBoxObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  CQChartsBoxObj::addProperties(model, path);

  QString textPath = path + "/text";

  model->addProperty(textPath, this, "textStr"  , "text" );
  model->addProperty(textPath, this, "textFont" , "font" );
  model->addProperty(textPath, this, "textColor", "color");
  model->addProperty(textPath, this, "textAngle", "angle");
}

void
CQChartsTextBoxObj::
draw(QPainter *painter, const QRectF &rect) const
{
  if (! isTextVisible())
    return;

  CQChartsBoxObj::draw(painter, rect);

  //---

  drawText(painter, rect, textStr());
}

void
CQChartsTextBoxObj::
draw(QPainter *painter, const QPolygonF &poly) const
{
  CQChartsBoxObj::draw(painter, poly);

  //---

  QRectF rect = poly.boundingRect();

  drawText(painter, rect, textStr());
}

void
CQChartsTextBoxObj::
drawText(QPainter *painter, const QRectF &rect, const QString &text) const
{
  QFontMetricsF fm(textFont());

  QColor c = interpTextColor(0, 1);

  c.setAlphaF(textAlpha());

  QPen pen(c);

  painter->setPen (pen);
  painter->setFont(textFont());

  painter->drawText(QPointF(rect.left() + margin(),
                            rect.bottom() - margin() - fm.descent()), text);
}

//------

CQChartsRotatedTextBoxObj::
CQChartsRotatedTextBoxObj(CQChartsPlot *plot) :
 CQChartsTextBoxObj(plot)
{
}

void
CQChartsRotatedTextBoxObj::
draw(QPainter *painter, const QPointF &center, const QString &text, double angle,
     Qt::Alignment align) const
{
  painter->save();

  QFontMetricsF fm(textFont());

  double tw = fm.width(text);

  double tw1 = tw + 2*margin();
  double th1 = fm.height() + 2*margin();

  double cx = center.x();
  double cy = center.y() - th1/2;
  double cd = 0.0;

  if      (align & Qt::AlignHCenter) {
    cx -= tw1/2;
  }
  else if (align & Qt::AlignRight) {
    cx -= tw1;
    cd  = -margin();
  }
  else {
    cd  = margin();
  }

  rect_ = QRectF(cx, cy, tw1, th1);

  //---

  // draw box
  if (CQChartsUtil::isZero(angle)) {
    CQChartsBoxObj::draw(painter, rect_);
  }
  else {
    QRectF                      bbox;
    CQChartsRotatedText::Points points;

    CQChartsRotatedText::bboxData(center.x(), center.y(), text, textFont(), angle, margin(),
                                  bbox, points, align, /*alignBBox*/ true);

    QPolygonF poly;

    for (std::size_t i = 0; i < points.size(); ++i)
      poly << points[i];

    CQChartsBoxObj::draw(painter, poly);
  }

  //---

  // draw text
  painter->setFont(textFont());

  QColor c = interpTextColor(0, 1);

  c.setAlphaF(textAlpha());

  QPen pen(c);

  painter->setPen(pen);

  CQChartsRotatedText::drawRotatedText(painter, center.x() + cd, center.y(), text, angle,
                                       align, /*alignBBox*/ true);

  painter->restore();
}

CQChartsGeom::BBox
CQChartsRotatedTextBoxObj::
bbox(const QPointF &center, const QString &text, double angle, Qt::Alignment align) const
{
  QFontMetricsF fm(textFont());

  double tw = fm.width(text);

  double tw1 = tw + 2*margin();
  double th1 = fm.height() + 2*margin();

  double cx = center.x();
  double cy = center.y() - th1/2;

  if      (align & Qt::AlignHCenter) {
    cx -= tw1/2;
  }
  else if (align & Qt::AlignRight) {
    cx -= tw1;
  }

  QRectF qrect;

  if (angle) {
    qrect = QRectF(cx, cy, tw1, th1);
  }
  else {
    CQChartsRotatedText::Points points;

    CQChartsRotatedText::bboxData(center.x(), center.y(), text, textFont(), angle, margin(),
                                  qrect, points, align, /*alignBBox*/ true);
  }

  CQChartsGeom::BBox bbox;

  plot_->pixelToWindow(CQChartsUtil::fromQRect(qrect), bbox);

  return bbox;
}
