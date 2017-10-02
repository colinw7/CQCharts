#include <CQChartsTextBoxObj.h>
#include <CQPropertyViewTree.h>
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
