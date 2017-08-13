#include <CQChartsXYPlot.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>
#include <CMathGeom2D.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsXYPlot::
CQChartsXYPlot(QAbstractItemModel *model) :
 CQChartsPlot(nullptr, model)
{
  addAxes();

  addKey();

  xAxis_->setColumn(xColumn_);
  yAxis_->setColumn(yColumn_);

  addProperty("columns"  , this, "xColumn"       , "x"     );
  addProperty("columns"  , this, "yColumn"       , "y"     );
  addProperty("columns"  , this, "nameColumn"    , "name"  );
  addProperty("bivariate", this, "bivariate"               );
  addProperty("stacked"  , this, "stacked"                 );
  addProperty("points"   , this, "points"        , "shown" );
  addProperty("points"   , this, "pointsColor"   , "color" );
  addProperty("points"   , this, "symbolName"    , "symbol");
  addProperty("points"   , this, "symbolSize"    , "size"  );
  addProperty("lines"    , this, "lines"         , "shown" );
  addProperty("lines"    , this, "linesColor"    , "color" );
  addProperty("fillUnder", this, "fillUnder"     , "shown" );
  addProperty("fillUnder", this, "fillUnderColor", "color" );
}

QString
CQChartsXYPlot::
pointsColorStr() const
{
  if (pointData_.palette)
    return "palette";

  return pointData_.color.name();
}

void
CQChartsXYPlot::
setPointsColorStr(const QString &str)
{
  if (str == "palette") {
    pointData_.palette = true;
  }
  else {
    pointData_.palette = false;
    pointData_.color   = QColor(str);
  }
}

QString
CQChartsXYPlot::
linesColorStr() const
{
  if (lineData_.palette)
    return "palette";

  return lineData_.color.name();
}

void
CQChartsXYPlot::
setLinesColorStr(const QString &str)
{
  if (str == "palette") {
    lineData_.palette = true;
  }
  else {
    lineData_.palette = false;
    lineData_.color   = QColor(str);
  }
}

QString
CQChartsXYPlot::
fillUnderColorStr() const
{
  if (fillUnderData_.palette)
    return "palette";

  return fillUnderData_.color.name();
}

void
CQChartsXYPlot::
setFillUnderColorStr(const QString &str)
{
  if (str == "palette") {
    fillUnderData_.palette = true;
  }
  else {
    fillUnderData_.palette = false;
    fillUnderData_.color   = QColor(str);
  }
}

QString
CQChartsXYPlot::
symbolName() const
{
  return CSymbol2DMgr::typeToName(symbolType()).c_str();
}

void
CQChartsXYPlot::
setSymbolName(const QString &s)
{
  CSymbol2D::Type type = CSymbol2DMgr::nameToType(s.toStdString());

  if (type != CSymbol2D::Type::NONE)
    setSymbolType(type);
}

QColor
CQChartsXYPlot::
pointColor(int i, int n) const
{
  if (pointData_.palette)
    return paletteColor(i, n);

  return pointData_.color;
}

QColor
CQChartsXYPlot::
lineColor(int i, int n) const
{
  if (lineData_.palette)
    return paletteColor(i, n);

  return lineData_.color;
}

QColor
CQChartsXYPlot::
fillUnderColor(int i, int n) const
{
  if (fillUnderData_.palette)
    return paletteColor(i, n);

  return fillUnderData_.color;
}

void
CQChartsXYPlot::
updateRange()
{
  int n = model_->rowCount(QModelIndex());

  dataRange_.reset();

  for (int i = 0; i < n; ++i) {
    double x = CQChartsUtil::modelReal(model_, i, xColumn_);

    if      (isBivariate()) {
      for (int j = 0; j < numSets(); ++j) {
        int yColumn = getSetColumn(j);

        double y = CQChartsUtil::modelReal(model_, i, yColumn);

        dataRange_.updateRange(x, y);
      }
    }
    else if (isStacked()) {
      double sum = 0.0;

      for (int j = 0; j < numSets(); ++j) {
        int yColumn = getSetColumn(j);

        double y = CQChartsUtil::modelReal(model_, i, yColumn);

        sum += y;
      }

      dataRange_.updateRange(x, 0.0);
      dataRange_.updateRange(x, sum);
    }
    else {
      for (int j = 0; j < numSets(); ++j) {
        int yColumn = getSetColumn(j);

        double y = CQChartsUtil::modelReal(model_, i, yColumn);

        dataRange_.updateRange(x, y);
      }
    }
  }

  //---

  xAxis_->setColumn(xColumn_);

  QString xname = model_->headerData(xColumn_, Qt::Horizontal).toString();

  xAxis_->setLabel(xname);

  if      (isBivariate()) {
  }
  else if (isStacked()) {
  }
  else {
    if (yColumns_.empty()) {
      yAxis_->setColumn(yColumn_);

      QString yname = model_->headerData(yColumn_, Qt::Horizontal).toString();

      yAxis_->setLabel(yname);
    }
  }

  //---

  applyDataRange();
}

void
CQChartsXYPlot::
initObjs(bool force)
{
  if (force) {
    for (auto &plotObj : plotObjs_)
      delete plotObj;

    plotObjs_.clear();

    dataRange_.reset();

    plotObjTree_.reset();
  }

  //--

  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //--

  double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  int n = model_->rowCount(QModelIndex());

  if      (isBivariate()) {
    for (int i = 0; i < n; ++i) {
      double x = CQChartsUtil::modelReal(model_, i, xColumn_);

      std::vector<double> yVals;

      for (int j = 0; j < numSets(); ++j) {
        int yColumn = getSetColumn(j);

        double y = CQChartsUtil::modelReal(model_, i, yColumn);

        yVals.push_back(y);
      }

      if (yVals.size() < 2)
        continue;

      double y1 = yVals[0];

      for (std::size_t j = 1; j < yVals.size(); ++j) {
        double y2 = yVals[j];

        CBBox2D bbox(x - sw/2, y1 - sh/2, x + sw/2, y2 + sh/2);

        CQChartsXYBiLineObj *lineObj = new CQChartsXYBiLineObj(this, bbox, x, y1, y2, j);

        if (nameColumn_ >= 0) {
          QString name = CQChartsUtil::modelString(model_, i, nameColumn_);

          lineObj->setId(name);
        }
        else
          lineObj->setId(QString("%1:%2").arg(i).arg(j - 1));

        addPlotObject(lineObj);

        y1 = y2;
      }
    }
  }
  else if (isStacked()) {
    typedef std::vector<double> Reals;

    Reals sum, lastSum;

    sum.resize(n);

    for (int j = 0; j < numSets(); ++j) {
      int yColumn = getSetColumn(j);

      QString name = model_->headerData(yColumn, Qt::Horizontal).toString();

      //---

      QPolygonF poly, polyLine;

      lastSum = sum;

      for (int i = 0; i < n; ++i) {
        double x = CQChartsUtil::modelReal(model_, i, xColumn_);
        double y = CQChartsUtil::modelReal(model_, i, yColumn );

        double y1 = y + lastSum[i];

        QString name1;

        if (nameColumn_ >= 0)
          name1 = CQChartsUtil::modelString(model_, i, nameColumn_);
        else
          name1 = name;

        CBBox2D bbox(x - sw/2, y1 - sh/2, x + sw/2, y1 + sh/2);

        CQChartsXYPointObj *pointObj = new CQChartsXYPointObj(this, bbox, x, y1, j, i);

        if (name1.length())
          pointObj->setId(QString("%1:%2:%3").arg(name1).arg(x).arg(y));
        else
          pointObj->setId(QString("%1:%2:%3:%4").arg(i).arg(j).arg(x).arg(y));

        addPlotObject(pointObj);

        //---

        if (i == 0) {
          poly << QPointF(x, lastSum[i]);
        }

        poly     << QPointF(x, y1);
        polyLine << QPointF(x, y1);

        if (i == n - 1) {
          for (int k = i; k > 0; --k) {
            double x1 = CQChartsUtil::modelReal(model_, k, xColumn_);

            poly << QPointF(x1, lastSum[k]);
          }
        }

        //---

        sum[i] = y1;
      }

      //---

      CBBox2D bbox = CQUtil::fromQRect(polyLine.boundingRect());

      CQChartsXYPolylineObj *lineObj = new CQChartsXYPolylineObj(this, bbox, polyLine, j);

      lineObj->setId(QString("%1").arg(name));

      addPlotObject(lineObj);

      //---

      bbox = CQUtil::fromQRect(poly.boundingRect());

      CQChartsXYPolygonObj *polyObj = new CQChartsXYPolygonObj(this, bbox, poly, j);

      polyObj->setId(QString("%1").arg(name));

      addPlotObject(polyObj);
    }
  }
  else {
    for (int j = 0; j < numSets(); ++j) {
      int yColumn = getSetColumn(j);

      QString name = model_->headerData(yColumn, Qt::Horizontal).toString();

      //---

      QPolygonF poly, polyLine;

      for (int i = 0; i < n; ++i) {
        double x = CQChartsUtil::modelReal(model_, i, xColumn_);
        double y = CQChartsUtil::modelReal(model_, i, yColumn );

        QString name1;

        if (nameColumn_ >= 0)
          name1 = CQChartsUtil::modelString(model_, i, nameColumn_);
        else
          name1 = name;

        CBBox2D bbox(x - sw/2, y - sh/2, x + sw/2, y + sh/2);

        CQChartsXYPointObj *pointObj = new CQChartsXYPointObj(this, bbox, x, y, j, i);

        if (name1.length())
          pointObj->setId(QString("%1:%2:%3").arg(name1).arg(x).arg(y));
        else
          pointObj->setId(QString("%1:%2:%3:%4").arg(i).arg(j).arg(x).arg(y));

        addPlotObject(pointObj);

        //---

        if (i == 0)
          poly << QPointF(x, dataRange_.ymin());

        poly     << QPointF(x, y);
        polyLine << QPointF(x, y);

        if (i == n - 1)
          poly << QPointF(x, dataRange_.ymin());
      }

      //---

      CBBox2D bbox = CQUtil::fromQRect(polyLine.boundingRect());

      CQChartsXYPolylineObj *lineObj = new CQChartsXYPolylineObj(this, bbox, polyLine, j);

      lineObj->setId(QString("%1").arg(name));

      addPlotObject(lineObj);

      //---

      bbox = CQUtil::fromQRect(poly.boundingRect());

      CQChartsXYPolygonObj *polyObj = new CQChartsXYPolygonObj(this, bbox, poly, j);

      polyObj->setId(QString("%1").arg(name));

      addPlotObject(polyObj);
    }
  }

  //----

  key_->clearItems();

  if      (isBivariate()) {
  }
  else if (isStacked()) {
    int ns = numSets();

    for (int i = 0; i < ns; ++i) {
      int yColumn = getSetColumn(i);

      QString name = model_->headerData(yColumn, Qt::Horizontal).toString();

      CQChartsXYKeyColor *color = new CQChartsXYKeyColor(this, true, i);
      CQChartsKeyText    *text  = new CQChartsKeyText   (this, name);

      key_->addItem(color, i, 0);
      key_->addItem(text , i, 1);
    }
  }
  else {
    int ns = numSets();

    for (int i = 0; i < ns; ++i) {
      int yColumn = getSetColumn(i);

      QString name = model_->headerData(yColumn, Qt::Horizontal).toString();

      CQChartsXYKeyColor *color = new CQChartsXYKeyColor(this, true, i);
      CQChartsKeyText    *text  = new CQChartsKeyText   (this, name);

      key_->addItem(color, i, 0);
      key_->addItem(text , i, 1);
    }
  }
}

int
CQChartsXYPlot::
numSets() const
{
  if (yColumns_.empty())
    return 1;

  return yColumns_.size();
}

int
CQChartsXYPlot::
getSetColumn(int i) const
{
  if (! yColumns_.empty())
    return yColumns_[i];
  else
    return yColumn_;
}

void
CQChartsXYPlot::
paintEvent(QPaintEvent *)
{
  initObjs();

  //---

  QPainter p(this);

  p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  drawBackground(&p);

  //---

  for (const auto &plotObj : plotObjs_)
    plotObj->draw(&p);

  drawAxes(&p);

  //---

  drawKey(&p);
}

//------

CQChartsXYBiLineObj::
CQChartsXYBiLineObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x,
                    double y1, double y2, int ind) :
 CQChartsPlotObj(rect), plot_(plot), x_(x), y1_(y1), y2_(y2), ind_(ind)
{
}

bool
CQChartsXYBiLineObj::
visible() const
{
  if (! plot_->isFillUnder() && ! plot_->isPoints())
    return false;

  return isVisible();
}

bool
CQChartsXYBiLineObj::
inside(const CPoint2D &p) const
{
  double px, py1, py2;

  plot_->windowToPixel(x_, y1_, px, py1);
  plot_->windowToPixel(x_, y2_, px, py2);

  double s = plot_->symbolSize();

  CBBox2D pbbox(px - s, py1 - s, px + s, py2 + s);

  CPoint2D pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsXYBiLineObj::
draw(QPainter *p)
{
  int ns = plot_->numSets();

  if (plot_->isFillUnder()) {
    double px, py1, py2;

    plot_->windowToPixel(x_, y1_, px, py1);
    plot_->windowToPixel(x_, y2_, px, py2);

    QColor lineColor = plot_->objectStateColor(this, plot_->fillUnderColor(ind_, ns));

    p->setPen(lineColor);

    p->drawLine(px, py1, px, py2);
  }

  if (plot_->isPoints()) {
    QColor c = plot_->objectStateColor(this, plot_->pointColor(ind_, ns));

    p->setPen(c);

    double s = plot_->symbolSize();

    plot_->drawSymbol(p, CPoint2D(x_, y1_), plot_->symbolType(), s);
    plot_->drawSymbol(p, CPoint2D(x_, y2_), plot_->symbolType(), s);
  }
}

//------

CQChartsXYPointObj::
CQChartsXYPointObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x, double y,
                   int iset, int ind) :
 CQChartsPlotObj(rect), plot_(plot), x_(x), y_(y), iset_(iset), ind_(ind)
{
}

bool
CQChartsXYPointObj::
visible() const
{
  if (! plot_->isPoints())
    return false;

  return isVisible();
}

bool
CQChartsXYPointObj::
inside(const CPoint2D &p) const
{
  double px, py;

  plot_->windowToPixel(x_, y_, px, py);

  double s = plot_->symbolSize();

  CBBox2D pbbox(px - s, py - s, px + s, py + s);

  CPoint2D pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsXYPointObj::
draw(QPainter *p)
{
  if (plot_->isPoints()) {
    int ns = plot_->numSets();

    QColor c = plot_->objectStateColor(this, plot_->pointColor(iset_, ns));

    p->setPen(c);

    double s = plot_->symbolSize();

    plot_->drawSymbol(p, CPoint2D(x_, y_), plot_->symbolType(), s);
  }
}

//------

CQChartsXYPolylineObj::
CQChartsXYPolylineObj(CQChartsXYPlot *plot, const CBBox2D &rect, const QPolygonF &poly, int ind) :
 CQChartsPlotObj(rect), plot_(plot), poly_(poly), ind_(ind)
{
}

bool
CQChartsXYPolylineObj::
visible() const
{
  if (! plot_->isLines())
    return false;

  return isVisible();
}

bool
CQChartsXYPolylineObj::
inside(const CPoint2D &p) const
{
  if (! plot_->isLines())
    return false;

  double px, py;

  plot_->windowToPixel(p.x, p.y, px, py);

  for (int i = 1; i < poly_.length(); ++i) {
    double x1 = poly_[i - 1].x();
    double y1 = poly_[i - 1].y();
    double x2 = poly_[i    ].x();
    double y2 = poly_[i    ].y();

    double px1, py1, px2, py2;

    plot_->windowToPixel(x1, y1, px1, py1);
    plot_->windowToPixel(x2, y2, px2, py2);

    CLine2D line(CPoint2D(px1, py1), CPoint2D(px2, py2));

    double d;

    if (CMathGeom2D::PointLineDistance(CPoint2D(px, py), line, &d) && d < 1)
      return true;
  }

  return false;
}

void
CQChartsXYPolylineObj::
draw(QPainter *p)
{
  if (! plot_->isLines())
    return;

  QColor c;

  if      (plot_->isBivariate())
    c = Qt::black;
  else if (plot_->isStacked()) {
    int ns = plot_->numSets();

    c = plot_->objectStateColor(this, plot_->lineColor(ind_, ns));
  }
  else {
    int ns = plot_->numSets();

    c = plot_->objectStateColor(this, plot_->lineColor(ind_, ns));
  }

  QPen pen(c);

  if (isInside())
    pen.setWidth(3);

  p->setPen(pen);

  for (int i = 1; i < poly_.length(); ++i) {
    double px1, py1, px2, py2;

    plot_->windowToPixel(poly_[i - 1].x(), poly_[i - 1].y(), px1, py1);
    plot_->windowToPixel(poly_[i    ].x(), poly_[i    ].y(), px2, py2);

    p->drawLine(px1, py1, px2, py2);
  }
}

//------

CQChartsXYPolygonObj::
CQChartsXYPolygonObj(CQChartsXYPlot *plot, const CBBox2D &rect, const QPolygonF &poly, int ind) :
 CQChartsPlotObj(rect), plot_(plot), poly_(poly), ind_(ind)
{
}

bool
CQChartsXYPolygonObj::
visible() const
{
  if (! plot_->isFillUnder())
    return false;

  return isVisible();
}

bool
CQChartsXYPolygonObj::
inside(const CPoint2D &p) const
{
  return poly_.containsPoint(CQUtil::toQPoint(p), Qt::OddEvenFill);
}

void
CQChartsXYPolygonObj::
draw(QPainter *p)
{
  if (! plot_->isFillUnder())
    return;

  p->setPen(Qt::NoPen);

  int ns = plot_->numSets();

  QColor fillColor = plot_->objectStateColor(this, plot_->fillUnderColor(ind_, ns));

  fillColor.setAlpha(128);

  p->setBrush(fillColor);

  QPolygonF poly;

  for (int i = 0; i < poly_.length(); ++i) {
    double px, py;

    plot_->windowToPixel(poly_[i].x(), poly_[i].y(), px, py);

    poly.push_back(QPointF(px, py));
  }

  p->drawPolygon(poly);
}

//------

CQChartsXYKeyColor::
CQChartsXYKeyColor(CQChartsXYPlot *plot, bool valueColor, int ind) :
 CQChartsKeyItem(plot->key()), plot_(plot), valueColor_(valueColor), ind_(ind)
{
}

QSizeF
CQChartsXYKeyColor::
size() const
{
  QFontMetrics fm(plot_->font());

  double h = fm.height();

  double ww = plot_->pixelToWindowWidth (h + 2);
  double wh = plot_->pixelToWindowHeight(h + 2);

  return QSizeF(ww, wh);
}

void
CQChartsXYKeyColor::
draw(QPainter *p, const CBBox2D &rect)
{
  p->setPen(Qt::black);

  CBBox2D prect;

  plot_->windowToPixel(rect, prect);
  plot_->windowToPixel(rect, prect);

  QRectF prect1(QPointF(prect.getXMin() + 2, prect.getYMin() + 2),
                QPointF(prect.getXMax() - 2, prect.getYMax() - 2));

  int ns = plot_->numSets();

  QColor c = plot_->lineColor(ind_, ns);

  p->setPen  (Qt::black);
  p->setBrush(c);

  p->drawRect(prect1);
}
