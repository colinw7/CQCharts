#include <CQChartsTextBoxObj.h>
#include <CQChartsUtil.h>
#include <CQPropertyViewTree.h>
#include <CQRotatedText.h>
#include <QPainter>

CQChartsTextBoxObj::
CQChartsTextBoxObj()
{
}

void
CQChartsTextBoxObj::
addProperties(CQPropertyViewTree *tree, const QString &path)
{
  CQChartsBoxObj::addProperties(tree, path);

  QString textPath = path + "/text";

  tree->addProperty(textPath, this, "text" );
  tree->addProperty(textPath, this, "font" );
  tree->addProperty(textPath, this, "color");
  tree->addProperty(textPath, this, "angle");
}

void
CQChartsTextBoxObj::
draw(QPainter *p, const QRectF &rect) const
{
  CQChartsBoxObj::draw(p, rect);

  //---

  drawText(p, rect, text());
}

void
CQChartsTextBoxObj::
draw(QPainter *p, const QPolygonF &poly) const
{
  CQChartsBoxObj::draw(p, poly);

  //---

  QRectF rect = poly.boundingRect();

  drawText(p, rect, text());
}

void
CQChartsTextBoxObj::
drawText(QPainter *p, const QRectF &rect, const QString &text) const
{
  QFontMetrics fm(font());

  p->setPen (color());
  p->setFont(font());

  p->drawText(rect.left() + margin(), rect.bottom() - margin() - fm.descent(), text);
}

//------

CQChartsRotatedTextBoxObj::
CQChartsRotatedTextBoxObj()
{
}

void
CQChartsRotatedTextBoxObj::
draw(QPainter *p, const QPointF &c, const QString &text, double angle,
     Qt::Alignment align) const
{
  p->save();

  QFontMetrics fm(font());

  int tw = fm.width(text);

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

  p->setFont(font());
  p->setPen(color());

  if (CQChartsUtil::isZero(angle)) {
    CQChartsBoxObj::draw(p, rect_);
  }
  else {
    QRectF                bbox;
    CQRotatedText::Points points;

    CQRotatedText::bboxData(c.x(), c.y(), text, font(), angle, margin(),
                            bbox, points, align, /*alignBBox*/ true);

    QPolygonF poly;

    for (std::size_t i = 0; i < points.size(); ++i)
      poly << points[i];

    CQChartsBoxObj::draw(p, poly);
  }

  p->setPen(color());

  CQRotatedText::drawRotatedText(p, c.x() + cd, c.y(), text, angle,
                                 align, /*alignBBox*/ true);

  p->restore();
}
