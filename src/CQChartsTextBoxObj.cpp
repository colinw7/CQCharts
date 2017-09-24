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
}

void
CQChartsTextBoxObj::
draw(QPainter *p, const QRectF &rect)
{
  CQChartsBoxObj::draw(p, rect);

  //---

  drawText(p, rect);
}

void
CQChartsTextBoxObj::
draw(QPainter *p, const QPolygonF &poly)
{
  CQChartsBoxObj::draw(p, poly);

  //---

  QRectF rect = poly.boundingRect();

  drawText(p, rect);
}

void
CQChartsTextBoxObj::
drawText(QPainter *p, const QRectF &rect)
{
  QFontMetrics fm(font());

  p->setPen (color());
  p->setFont(font());

  p->drawText(rect.left() + margin(), rect.right() - margin() - fm.descent(), text());
}
