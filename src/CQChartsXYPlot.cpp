#include <CQChartsXYPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>
#include <CMathGeom2D.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsXYPlot::
CQChartsXYPlot(CQChartsView *view, QAbstractItemModel *model) :
 CQChartsPlot(view, model)
{
  addAxes();

  addKey();

  addTitle();

  setXValueColumn(xColumn_);
  setYValueColumn(yColumn_);
}

void
CQChartsXYPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns"  , this, "xColumn"       , "x"      );
  addProperty("columns"  , this, "yColumn"       , "y"      );
  addProperty("columns"  , this, "nameColumn"    , "name"   );
  addProperty("bivariate", this, "bivariate"     , "enabled");
  addProperty("bivariate", this, "bivariateWidth", "width"  );
  addProperty("stacked"  , this, "stacked"       , "enabled");
  addProperty("points"   , this, "points"        , "shown"  );
  addProperty("points"   , this, "pointsColor"   , "color"  );
  addProperty("points"   , this, "symbolName"    , "symbol" );
  addProperty("points"   , this, "symbolSize"    , "size"   );
  addProperty("points"   , this, "symbolFilled"  , "filled" );
  addProperty("lines"    , this, "lines"         , "shown"  );
  addProperty("lines"    , this, "linesColor"    , "color"  );
  addProperty("lines"    , this, "linesWidth"    , "width"  );
  addProperty("fillUnder", this, "fillUnder"     , "shown"  );
  addProperty("fillUnder", this, "fillUnderColor", "color"  );
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
  if (! pointData_.palette)
    return pointData_.color;

  return paletteColor(i, n);
}

QColor
CQChartsXYPlot::
lineColor(int i, int n) const
{
  if (! lineData_.palette)
    return lineData_.color;

  return paletteColor(i, n);
}

QColor
CQChartsXYPlot::
fillUnderColor(int i, int n) const
{
  if (! fillUnderData_.palette)
    return fillUnderData_.color;

  return paletteColor(i, n);
}

QColor
CQChartsXYPlot::
paletteColor(int i, int n, const QColor &def) const
{
  if (prevPlot() || nextPlot()) {
    CQChartsPlot *plot1 = prevPlot();
    CQChartsPlot *plot2 = nextPlot();

    while (plot1) { ++n; plot1 = plot1->prevPlot(); }
    while (plot2) { ++n; plot2 = plot2->nextPlot(); }
  }

  //---

  CQChartsPlot *plot1 = prevPlot();

  while (plot1) {
    ++i;

    plot1 = plot1->prevPlot();
  }

  return CQChartsPlot::paletteColor(i, n, def);
}

void
CQChartsXYPlot::
updateRange()
{
  int n = model_->rowCount(QModelIndex());

  dataRange_.reset();

  int ns = numSets();

  typedef std::vector<double> Reals;

  Reals sum, lastSum;

  sum.resize(ns);

  for (int i = 0; i < n; ++i) {
    lastSum = sum;

    //---

    bool ok1;

    double x = CQChartsUtil::modelReal(model_, i, xColumn_, ok1);

    if (! ok1) x = i;

    if (CQChartsUtil::isNaN(x))
      continue;

    if      (isBivariate()) {
      for (int j = 0; j < ns; ++j) {
        int yColumn = getSetColumn(j);

        bool ok2;

        double y = CQChartsUtil::modelReal(model_, i, yColumn, ok2);

        if (! ok2) y = i;

        if (CQChartsUtil::isNaN(y))
          continue;

        dataRange_.updateRange(x, y);
      }
    }
    else if (isStacked()) {
      double sum1 = 0.0;

      for (int j = 0; j < ns; ++j) {
        int yColumn = getSetColumn(j);

        bool ok2;

        double y = CQChartsUtil::modelReal(model_, i, yColumn, ok2);

        if (! ok2) y = i;

        if (CQChartsUtil::isNaN(y))
          continue;

        sum1 += y;
      }

      dataRange_.updateRange(x, 0.0);
      dataRange_.updateRange(x, sum1);
    }
    else {
      for (int j = 0; j < ns; ++j) {
        int yColumn = getSetColumn(j);

        bool ok2;

        double y = CQChartsUtil::modelReal(model_, i, yColumn, ok2);

        if (! ok2) y = i;

        if (CQChartsUtil::isNaN(y))
          continue;

        double y1 = y;

        if (isCumulative()) {
          y1 = y + lastSum[j];

          sum[j] += y;
        }

        dataRange_.updateRange(x, y1);
      }
    }
  }

  if (xmin_.isValid()) dataRange_.setLeft  (xmin_.getValue());
  if (ymin_.isValid()) dataRange_.setBottom(ymin_.getValue());
  if (xmax_.isValid()) dataRange_.setRight (xmax_.getValue());
  if (ymax_.isValid()) dataRange_.setTop   (ymax_.getValue());

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
    clearPlotObjects();

    dataRange_.reset();
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
      bool ok1;

      double x = CQChartsUtil::modelReal(model_, i, xColumn_, ok1);

      if (! ok1) x = i;

      if (CQChartsUtil::isNaN(x))
        continue;

      std::vector<double> yVals;

      for (int j = 0; j < numSets(); ++j) {
        int yColumn = getSetColumn(j);

        bool ok2;

        double y = CQChartsUtil::modelReal(model_, i, yColumn, ok2);

        if (! ok2) y = i;

        if (CQChartsUtil::isNaN(y))
          continue;

        yVals.push_back(y);
      }

      if (yVals.size() < 2)
        continue;

      double y1 = yVals[0];
      int    ny = yVals.size();

      for (std::size_t j = 1; j < yVals.size(); ++j) {
        double y2 = yVals[j];

        CBBox2D bbox(x - sw/2, y1 - sh/2, x + sw/2, y2 + sh/2);

        CQChartsXYBiLineObj *lineObj =
          new CQChartsXYBiLineObj(this, bbox, x, y1, y2, j - 1, ny - 1);

        QString xstr  = xStr(x);
        QString y1str = yStr(y1);
        QString y2str = yStr(y2);

        if (nameColumn_ >= 0) {
          bool ok;

          QString name = CQChartsUtil::modelString(model_, i, nameColumn_, ok);

          lineObj->setId(QString("%1:%2:%3").arg(name).arg(y1str).arg(y2str));
        }
        else
          lineObj->setId(QString("%1:%2:%3").arg(xstr).arg(y1str).arg(y2str));

        addPlotObject(lineObj);

        y1 = y2;
      }
    }
  }
  else if (isStacked()) {
    int ns = numSets();

    typedef std::vector<double> Reals;

    Reals sum, lastSum;

    sum.resize(n);

    for (int j = 0; j < ns; ++j) {
      bool hidden = isSetHidden(j);

      if (hidden)
        continue;

      //---

      int yColumn = getSetColumn(j);

      QString name = model_->headerData(yColumn, Qt::Horizontal).toString();

      //---

      QPolygonF poly, polyLine;

      lastSum = sum;

      for (int i = 0; i < n; ++i) {
        bool ok1, ok2;

        double x = CQChartsUtil::modelReal(model_, i, xColumn_, ok1);
        double y = CQChartsUtil::modelReal(model_, i, yColumn , ok2);

        if (! ok1) x = i;
        if (! ok2) y = i;

        if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y)) {
          if (polyLine.length()) {
            addPolyLine(polyLine, j, ns, name);

            polyLine = QPolygonF();
          }

          continue;
        }

        double y1 = y + lastSum[i];

        QString name1;

        if (nameColumn_ >= 0) {
          bool ok;

          name1 = CQChartsUtil::modelString(model_, i, nameColumn_, ok);
        }
        else
          name1 = name;

        CBBox2D bbox(x - sw/2, y1 - sh/2, x + sw/2, y1 + sh/2);

        CQChartsXYPointObj *pointObj =
          new CQChartsXYPointObj(this, bbox, x, y1, j, ns, i, n);

        QString xstr = xStr(x);
        QString ystr = yStr(y);

        if (name1.length())
          pointObj->setId(QString("%1:%2:%3").arg(name1).arg(xstr).arg(ystr));
        else
          pointObj->setId(QString("%1:%2:%3:%4").arg(i).arg(j).arg(xstr).arg(ystr));

        addPlotObject(pointObj);

        //---

        if (i == 0) {
          poly << QPointF(x, lastSum[i]);
        }

        poly     << QPointF(x, y1);
        polyLine << QPointF(x, y1);

        if (i == n - 1) {
          for (int k = i; k > 0; --k) {
            bool ok1;

            double x1 = CQChartsUtil::modelReal(model_, k, xColumn_, ok1);

            if (! ok1) x1 = k;

            if (CQChartsUtil::isNaN(x1))
              continue;

            poly << QPointF(x1, lastSum[k]);
          }
        }

        //---

        sum[i] = y1;
      }

      //---

      if (polyLine.length())
        addPolyLine(polyLine, j, ns, name);

      //---

      addPolygon(poly, j, ns, name);
    }
  }
  else {
    int ns = numSets();

    for (int j = 0; j < ns; ++j) {
      bool hidden = isSetHidden(j);

      if (hidden)
        continue;

      //---

      int yColumn = getSetColumn(j);

      QString name = model_->headerData(yColumn, Qt::Horizontal).toString();

      //---

      double sum     = 0.0;
      double lastSum = 0.0;

      QPolygonF poly, polyLine;

      for (int i = 0; i < n; ++i) {
        lastSum = sum;

        //---

        bool ok1, ok2;

        double x = CQChartsUtil::modelReal(model_, i, xColumn_, ok1);
        double y = CQChartsUtil::modelReal(model_, i, yColumn , ok2);

        if (! ok1) x = i;
        if (! ok2) y = i;

        if (CQChartsUtil::isNaN(x) || CQChartsUtil::isNaN(y)) {
          if (polyLine.length()) {
            addPolyLine(polyLine, j, ns, name);

            polyLine = QPolygonF();
          }

          continue;
        }

        double y1 = y;

        if (isCumulative()) {
          sum += y;

          y1 = y + lastSum;
        }

        QString name1;

        if (nameColumn_ >= 0) {
          bool ok;

          name1 = CQChartsUtil::modelString(model_, i, nameColumn_, ok);
        }
        else
          name1 = name;

        CBBox2D bbox(x - sw/2, y1 - sh/2, x + sw/2, y1 + sh/2);

        CQChartsXYPointObj *pointObj =
          new CQChartsXYPointObj(this, bbox, x, y1, j, ns, i, n);

        QString xstr = xStr(x);
        QString ystr = yStr(y1);

        if (name1.length())
          pointObj->setId(QString("%1:%2:%3").arg(name1).arg(xstr).arg(ystr));
        else
          pointObj->setId(QString("%1:%2:%3:%4").arg(i).arg(j).arg(xstr).arg(ystr));

        addPlotObject(pointObj);

        //---

        if (i == 0)
          poly << QPointF(x, dataRange_.ymin());

        poly     << QPointF(x, y1);
        polyLine << QPointF(x, y1);

        if (i == n - 1)
          poly << QPointF(x, dataRange_.ymin());
      }

      //---

      if (polyLine.length())
        addPolyLine(polyLine, j, ns, name);

      //---

      addPolygon(poly, j, ns, name);
    }
  }

  //----

  if (! prevPlot()) {
    CQChartsKey *key = this->key();

    key->clearItems();

    addKeyItems(key);

    CQChartsPlot *plot1 = nextPlot();

    while (plot1) {
      plot1->addKeyItems(key);

      plot1 = plot1->nextPlot();
    }
  }
}

void
CQChartsXYPlot::
addPolyLine(const QPolygonF &polyLine, int i, int n, const QString &name)
{
  CBBox2D bbox = CQUtil::fromQRect(polyLine.boundingRect());

  CQChartsXYPolylineObj *lineObj = new CQChartsXYPolylineObj(this, bbox, polyLine, i, n);

  lineObj->setId(QString("%1").arg(name));

  addPlotObject(lineObj);
}

void
CQChartsXYPlot::
addPolygon(const QPolygonF &poly, int i, int n, const QString &name)
{
  CBBox2D bbox = CQUtil::fromQRect(poly.boundingRect());

  CQChartsXYPolygonObj *polyObj = new CQChartsXYPolygonObj(this, bbox, poly, i, n);

  polyObj->setId(QString("%1").arg(name));

  addPlotObject(polyObj);
}

void
CQChartsXYPlot::
addKeyItems(CQChartsKey *key)
{
  int row = key->maxRow();

  if      (isBivariate()) {
    int yColumn = getSetColumn(0);

    QString name = model_->headerData(yColumn, Qt::Horizontal).toString();

    CQChartsXYKeyColor *color = new CQChartsXYKeyColor(this, 0, 1);
    CQChartsXYKeyText  *text  = new CQChartsXYKeyText (this, 0, name);

    key->addItem(color, row, 0);
    key->addItem(text , row, 1);
  }
  else if (isStacked()) {
    int ns = numSets();

    for (int i = 0; i < ns; ++i) {
      int yColumn = getSetColumn(i);

      QString name = model_->headerData(yColumn, Qt::Horizontal).toString();

      CQChartsXYKeyLine *line = new CQChartsXYKeyLine(this, i, ns);
      CQChartsXYKeyText *text = new CQChartsXYKeyText(this, i, name);

      key->addItem(line, row + i, 0);
      key->addItem(text, row + i, 1);
    }
  }
  else {
    int ns = numSets();

    for (int i = 0; i < ns; ++i) {
      int yColumn = getSetColumn(i);

      QString name = model_->headerData(yColumn, Qt::Horizontal).toString();

      CQChartsXYKeyLine *line = new CQChartsXYKeyLine(this, i, ns);
      CQChartsXYKeyText *text = new CQChartsXYKeyText(this, i, name);

      key->addItem(line, row + i, 0);
      key->addItem(text, row + i, 1);
    }
  }

  key->plot()->updateKeyPosition(/*force*/true);
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

bool
CQChartsXYPlot::
interpY(double x, std::vector<double> &yvals) const
{
  if (isBivariate()) {
    return false;
  }

  for (const auto &plotObj : plotObjs_) {
    CQChartsXYPolylineObj *polyObj = dynamic_cast<CQChartsXYPolylineObj *>(plotObj);

    if (! polyObj)
      continue;

    std::vector<double> yvals1;

    polyObj->interpY(x, yvals1);

    for (const auto &y1 : yvals1)
      yvals.push_back(y1);
  }

  return ! yvals.empty();
}

void
CQChartsXYPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawBackground(p);

  drawBgAxes(p);
  drawBgKey (p);

  drawObjs(p);

  drawFgAxes(p);
  drawFgKey (p);

  //---

  drawTitle(p);
}

//------

CQChartsXYBiLineObj::
CQChartsXYBiLineObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x,
                    double y1, double y2, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), x_(x), y1_(y1), y2_(y2), i_(i), n_(n)
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
  if (plot_->isFillUnder()) {
    QColor lineColor = plot_->objectStateColor(this, plot_->fillUnderColor(i_, n_));

    double px, py1, py2;

    plot_->windowToPixel(x_, y1_, px, py1);
    plot_->windowToPixel(x_, y2_, px, py2);

    QPen pen(lineColor);

    if (plot_->bivariateWidth() > 0.0)
      pen.setWidth(plot_->bivariateWidth());

    p->setPen(pen);

    p->drawLine(px, py1, px, py2);
  }

  if (plot_->isPoints()) {
    QColor c = plot_->objectStateColor(this, plot_->pointColor(i_, n_));
    double s = plot_->symbolSize();

    plot_->drawSymbol(p, CPoint2D(x_, y1_), plot_->symbolType(), s, c, plot_->isSymbolFilled());
    plot_->drawSymbol(p, CPoint2D(x_, y2_), plot_->symbolType(), s, c, plot_->isSymbolFilled());
  }
}

//------

CQChartsXYPointObj::
CQChartsXYPointObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x, double y,
                   int iset, int nset, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), x_(x), y_(y), iset_(iset), nset_(nset), i_(i), n_(n)
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
    QColor c = plot_->objectStateColor(this, plot_->pointColor(iset_, nset_));
    double s = plot_->symbolSize();

    plot_->drawSymbol(p, CPoint2D(x_, y_), plot_->symbolType(), s, c, plot_->isSymbolFilled());
  }
}

//------

CQChartsXYPolylineObj::
CQChartsXYPolylineObj(CQChartsXYPlot *plot, const CBBox2D &rect,
                      const QPolygonF &poly, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), poly_(poly), i_(i), n_(n)
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

  for (int i = 1; i < poly_.count(); ++i) {
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

bool
CQChartsXYPolylineObj::
interpY(double x, std::vector<double> &yvals) const
{
  if (! plot_->isLines())
    return false;

  for (int i = 1; i < poly_.count(); ++i) {
    double x1 = poly_[i - 1].x();
    double y1 = poly_[i - 1].y();
    double x2 = poly_[i    ].x();
    double y2 = poly_[i    ].y();

    if (x >= x1 && x <= x2) {
      double y = (y2 - y1)*(x - x1)/(x2 - x1) + y1;

      yvals.push_back(y);
    }
  }

  return ! yvals.empty();
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
  else if (plot_->isStacked())
    c = plot_->objectStateColor(this, plot_->lineColor(i_, n_));
  else
    c = plot_->objectStateColor(this, plot_->lineColor(i_, n_));

  QPen pen(c);

  if (isInside())
    pen.setWidth(3);
  else
    pen.setWidth(plot_->linesWidth());

  p->setPen(pen);

  for (int i = 1; i < poly_.count(); ++i) {
    double px1, py1, px2, py2;

    plot_->windowToPixel(poly_[i - 1].x(), poly_[i - 1].y(), px1, py1);
    plot_->windowToPixel(poly_[i    ].x(), poly_[i    ].y(), px2, py2);

    p->drawLine(px1, py1, px2, py2);
  }
}

//------

CQChartsXYPolygonObj::
CQChartsXYPolygonObj(CQChartsXYPlot *plot, const CBBox2D &rect,
                     const QPolygonF &poly, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), poly_(poly), i_(i), n_(n)
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

  QColor fillColor;

  if (plot_->isStacked())
    fillColor = plot_->objectStateColor(this, plot_->lineColor(i_, n_));
  else
    fillColor = plot_->objectStateColor(this, plot_->fillUnderColor(i_, n_));

  fillColor.setAlpha(128);

  p->setBrush(fillColor);

  QPolygonF poly;

  for (int i = 0; i < poly_.count(); ++i) {
    double px, py;

    plot_->windowToPixel(poly_[i].x(), poly_[i].y(), px, py);

    poly.push_back(QPointF(px, py));
  }

  p->drawPolygon(poly);
}

//------

CQChartsXYKeyColor::
CQChartsXYKeyColor(CQChartsXYPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsXYKeyColor::
mousePress(const CPoint2D &)
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->initObjs(/*force*/true);

  plot->update();

  return true;
}

QColor
CQChartsXYKeyColor::
fillColor() const
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  QColor c;

  if      (plot->isBivariate())
    c = plot->fillUnderColor(i_, n_);
  else if (plot->prevPlot() || plot->nextPlot())
    c = plot->lineColor(i_, n_);
  else
    c = CQChartsKeyColorBox::fillColor();

  if (plot->isSetHidden(i_))
    c = CQUtil::blendColors(c, key_->bgColor(), 0.5);

  return c;
}

//------

CQChartsXYKeyLine::
CQChartsXYKeyLine(CQChartsXYPlot *plot, int i, int n) :
 CQChartsKeyItem(plot->key()), plot_(plot), i_(i), n_(n)
{
}

QSizeF
CQChartsXYKeyLine::
size() const
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  CQChartsXYPlot *keyPlot = qobject_cast<CQChartsXYPlot *>(key_->plot());

  QFontMetrics fm(plot->view()->font());

  double w = fm.width("X-X");
  double h = fm.height();

  double ww = keyPlot->pixelToWindowWidth (w + 8);
  double wh = keyPlot->pixelToWindowHeight(h + 2);

  return QSizeF(ww, wh);
}

bool
CQChartsXYKeyLine::
mousePress(const CPoint2D &)
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->initObjs(/*force*/true);

  plot->update();

  return true;
}

void
CQChartsXYKeyLine::
draw(QPainter *p, const CBBox2D &rect)
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  CQChartsXYPlot *keyPlot = qobject_cast<CQChartsXYPlot *>(key_->plot());

  CBBox2D prect;

  keyPlot->windowToPixel(rect, prect);

  QRectF prect1(QPointF(prect.getXMin() + 2, prect.getYMin() + 2),
                QPointF(prect.getXMax() - 2, prect.getYMax() - 2));

  QColor c = plot->pointColor(i_, n_);

  if (plot->isSetHidden(i_))
    c = CQUtil::blendColors(c, key_->bgColor(), 0.5);

  p->setPen(c);

  double x1 = prect.getXMin() + 4;
  double x2 = prect.getXMax() - 4;
  double y  = prect.getYMid();

  p->drawLine(x1, y, x2, y);

  double dx = keyPlot->pixelToWindowWidth(4);

  x1 = rect.getXMin() + dx;
  x2 = rect.getXMax() - dx;
  y  = rect.getYMid();

  double s = plot->symbolSize();

  keyPlot->drawSymbol(p, CPoint2D(x1, y), plot->symbolType(), s, c, plot->isSymbolFilled());
  keyPlot->drawSymbol(p, CPoint2D(x2, y), plot->symbolType(), s, c, plot->isSymbolFilled());
}

//------

CQChartsXYKeyText::
CQChartsXYKeyText(CQChartsXYPlot *plot, int i, const QString &text) :
 CQChartsKeyText(plot, text), i_(i)
{
}

QColor
CQChartsXYKeyText::
textColor() const
{
  QColor c = CQChartsKeyText::textColor();

  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  if (plot->isSetHidden(i_))
    c = CQUtil::blendColors(c, Qt::white, 0.5);

  return c;
}
