#include <CQChartsTextBoxObj.h>
#include <CQChartsUtil.h>
#include <CQPropertyViewModel.h>
#include <CQChartsRotatedText.h>
#include <QPainter>

CQChartsTextBoxObj::
CQChartsTextBoxObj(CQChartsPlot *plot) :
 CQChartsBoxObj(plot)
{
  CQChartsPaletteColor themeFg(CQChartsPaletteColor::Type::THEME_VALUE, 1);

  setColor(themeFg);
}

QString
CQChartsTextBoxObj::
colorStr() const
{
  return color_.colorStr();
}

void
CQChartsTextBoxObj::
setColorStr(const QString &s)
{
  color_.setColorStr(s);
}

QColor
CQChartsTextBoxObj::
interpColor(int i, int n) const
{
  return color_.interpColor(plot_, i, n);
}

void
CQChartsTextBoxObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  CQChartsBoxObj::addProperties(model, path);

  QString textPath = path + "/text";

  model->addProperty(textPath, this, "text" );
  model->addProperty(textPath, this, "font" );
  model->addProperty(textPath, this, "color");
  model->addProperty(textPath, this, "angle");
}

void
CQChartsTextBoxObj::
draw(QPainter *painter, const QRectF &rect) const
{
  if (! isVisible())
    return;

  CQChartsBoxObj::draw(painter, rect);

  //---

  drawText(painter, rect, text());
}

void
CQChartsTextBoxObj::
draw(QPainter *painter, const QPolygonF &poly) const
{
  CQChartsBoxObj::draw(painter, poly);

  //---

  QRectF rect = poly.boundingRect();

  drawText(painter, rect, text());
}

void
CQChartsTextBoxObj::
drawText(QPainter *painter, const QRectF &rect, const QString &text) const
{
  QFontMetricsF fm(font());

  painter->setPen (interpColor(0, 1));
  painter->setFont(font());

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
draw(QPainter *painter, const QPointF &c, const QString &text, double angle,
     Qt::Alignment align) const
{
  painter->save();

  QFontMetricsF fm(font());

  double tw = fm.width(text);

  double tw1 = tw + 2*margin();
  double th1 = fm.height() + 2*margin();

  double cx = c.x();
  double cy = c.y() - th1/2;
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

  painter->setFont(font());

  painter->setPen(interpColor(0, 1));

  if (CQChartsUtil::isZero(angle)) {
    CQChartsBoxObj::draw(painter, rect_);
  }
  else {
    QRectF                      bbox;
    CQChartsRotatedText::Points points;

    CQChartsRotatedText::bboxData(c.x(), c.y(), text, font(), angle, margin(),
                                  bbox, points, align, /*alignBBox*/ true);

    QPolygonF poly;

    for (std::size_t i = 0; i < points.size(); ++i)
      poly << points[i];

    CQChartsBoxObj::draw(painter, poly);
  }

  painter->setPen(interpColor(0, 1));

  CQChartsRotatedText::drawRotatedText(painter, c.x() + cd, c.y(), text, angle,
                                       align, /*alignBBox*/ true);

  painter->restore();
}
