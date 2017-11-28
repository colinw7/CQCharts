#include <CQChartsPointObj.h>
#include <CQPropertyViewModel.h>
#include <QPainter>

CQChartsPointObj::
CQChartsPointObj(CQChartsPlot *plot) :
 plot_(plot)
{
  strokeColor_ = CQChartsPaletteColor(CQChartsPaletteColor::Type::THEME_VALUE, 1);
  fillColor_   = CQChartsPaletteColor(CQChartsPaletteColor::Type::PALETTE);
}

QColor
CQChartsPointObj::
interpStrokeColor(int i, int n) const
{
  return strokeColor_.interpColor(plot_, i, n);
}

QColor
CQChartsPointObj::
interpFillColor(int i, int n) const
{
  return fillColor_.interpColor(plot_, i, n);
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
  QString strokePath = path + "/stroke";
  QString fillPath   = path + "/fill";

  model->addProperty(path      , this, "displayed"     , "visible");
  model->addProperty(path      , this, "symbolName"    , "symbol" );
  model->addProperty(path      , this, "size"          , "size"   );
  model->addProperty(strokePath, this, "stroked"       , "visible");
  model->addProperty(strokePath, this, "strokeColor"   , "color"  );
  model->addProperty(strokePath, this, "strokePalette" , "palette");
  model->addProperty(strokePath, this, "lineWidth"     , "width"  );
  model->addProperty(fillPath  , this, "filled"        , "visible");
  model->addProperty(fillPath  , this, "fillColor"     , "color"  );
  model->addProperty(fillPath  , this, "fillPalette"   , "palette");
}

void
CQChartsPointObj::
draw(QPainter *painter, const QPointF &p)
{
  QColor strokeColor = interpStrokeColor(0, 1);
  QColor fillColor   = interpFillColor  (0, 1);

  draw(painter, p, symbolType(), size(), isStroked(), strokeColor, lineWidth(),
       isFilled(), fillColor);
}

void
CQChartsPointObj::
draw(QPainter *painter, const QPointF &p, const CQChartsPlotSymbol::Type &symbol,
     double size, bool stroked, const QColor &strokeColor, double lineWidth,
     bool filled, const QColor &fillColor)
{
  QPen   pen(strokeColor);
  QBrush brush(fillColor);

  pen.setWidthF(lineWidth);

  painter->setPen  (pen);
  painter->setBrush(brush);

  CQChartsSymbol2DRenderer renderer(painter, CQChartsGeom::Point(p.x(), p.y()), size);

  if (filled)
    CQChartsPlotSymbolMgr::fillSymbol(symbol, &renderer);

  if (stroked)
    CQChartsPlotSymbolMgr::drawSymbol(symbol, &renderer);
}
