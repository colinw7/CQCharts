#include <CQChartsPointObj.h>
#include <CQPropertyViewModel.h>
#include <QPainter>

CQChartsPointObj::
CQChartsPointObj()
{
}

QString
CQChartsPointObj::
symbolName() const
{
  return CQChartsPlotSymbolMgr::typeToName(symbolType());
}

void
CQChartsPointObj::
setSymbolName(const QString &s)
{
  CQChartsPlotSymbol::Type type = CQChartsPlotSymbolMgr::nameToType(s);

  if (type != CQChartsPlotSymbol::Type::NONE)
    setSymbolType(type);
}

void
CQChartsPointObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  model->addProperty(path, this, "displayed" , "visible");
  model->addProperty(path, this, "symbolName", "symbol" );
  model->addProperty(path, this, "color"     , "color"  );
  model->addProperty(path, this, "palette"   , "palette");
  model->addProperty(path, this, "size"      , "size"   );
  model->addProperty(path, this, "filled"    , "filled" );
}

void
CQChartsPointObj::
draw(QPainter *painter, const QPointF &p)
{
  draw(painter, p, symbolType(), size(), color(), isFilled());
}

void
CQChartsPointObj::
draw(QPainter *painter, const QPointF &p, const CQChartsPlotSymbol::Type &symbol,
     double size, const QColor &color, bool filled)
{
  painter->setPen  (color);
  painter->setBrush(color);

  CQChartsSymbol2DRenderer renderer(painter, CQChartsGeom::Point(p.x(), p.y()), size);

  if (! filled)
    CQChartsPlotSymbolMgr::drawSymbol(symbol, &renderer);
  else
    CQChartsPlotSymbolMgr::fillSymbol(symbol, &renderer);
}
