#include <CQChartsPointObj.h>
#include <CQPropertyViewTree.h>
#include <QPainter>

CQChartsPointObj::
CQChartsPointObj()
{
}

QString
CQChartsPointObj::
symbolName() const
{
  return CSymbol2DMgr::typeToName(symbolType());
}

void
CQChartsPointObj::
setSymbolName(const QString &s)
{
  CSymbol2D::Type type = CSymbol2DMgr::nameToType(s);

  if (type != CSymbol2D::Type::NONE)
    setSymbolType(type);
}

void
CQChartsPointObj::
addProperties(CQPropertyViewTree *tree, const QString &path)
{
  tree->addProperty(path, this, "displayed" , "visible");
  tree->addProperty(path, this, "symbolName", "symbol" );
  tree->addProperty(path, this, "color"     , "color"  );
  tree->addProperty(path, this, "palette"   , "palette");
  tree->addProperty(path, this, "size"      , "size"   );
  tree->addProperty(path, this, "filled"    , "filled" );
}

void
CQChartsPointObj::
draw(QPainter *painter, const QPointF &p)
{
  draw(painter, p, symbolType(), size(), color(), isFilled());
}

void
CQChartsPointObj::
draw(QPainter *painter, const QPointF &p, const CSymbol2D::Type &symbol,
     double size, const QColor &color, bool filled)
{
  painter->setPen  (color);
  painter->setBrush(color);

  CQChartsSymbol2DRenderer renderer(painter, CPoint2D(p.x(), p.y()), size);

  if (! filled)
    CSymbol2DMgr::drawSymbol(symbol, &renderer);
  else
    CSymbol2DMgr::fillSymbol(symbol, &renderer);
}
