#include <CQChartsXYPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQRotatedText.h>

#include <QAbstractItemModel>
#include <QItemSelectionModel>
#include <QPainter>

CQChartsXYPlotType::
CQChartsXYPlotType()
{
  // columns
  addColumnParameter ("x", "X", "xColumn" , "", 0);
  addColumnsParameter("y", "Y", "yColumns", "", "1");

  addColumnParameter("name", "Name", "nameColumn", "optional");
  addColumnParameter("size", "Size", "sizeColumn", "optional");

  addColumnParameter("pointLabel" , "PointLabel" , "pointLabelColumn" , "optional");
  addColumnParameter("pointColor" , "PointColor" , "pointColorColumn" , "optional");
  addColumnParameter("pointSymbol", "PointSymbol", "pointSymbolColumn", "optional");

  // bool parameters
  addBoolParameter("bivariate" , "Bivariate" , "bivariate" , "optional");
  addBoolParameter("stacked"   , "Stacked"   , "stacked"   , "optional");
  addBoolParameter("cumulative", "Cumulative", "cumulative", "optional");
  addBoolParameter("fillUnder" , "FillUnder" , "fillUnder" , "optional");
  addBoolParameter("impulse"   , "Impulse"   , "impulse"   , "optional");
}

CQChartsPlot *
CQChartsXYPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsXYPlot(view, model);
}

//---

CQChartsXYPlot::
CQChartsXYPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("xy"), model)
{
  addAxes();

  addKey();

  addTitle();
}

QString
CQChartsXYPlot::
yColumnsStr() const
{
  return CQChartsUtil::toString(yColumns_);
}

bool
CQChartsXYPlot::
setYColumnsStr(const QString &s)
{
  std::vector<int> yColumns;

  if (! CQChartsUtil::fromString(s, yColumns))
    return false;

  setYColumns(yColumns);

  return true;
}

void
CQChartsXYPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  // columns
  addProperty("columns", this, "xColumn"           , "x"          );
  addProperty("columns", this, "yColumn"           , "y"          );
  addProperty("columns", this, "yColumns"          , "yset"       );
  addProperty("columns", this, "nameColumn"        , "name"       );
  addProperty("columns", this, "sizeColumn"        , "size"       );
  addProperty("columns", this, "pointLabelColumn"  , "pointLabel" );
  addProperty("columns", this, "pointColorColumn"  , "pointColor" );
  addProperty("columns", this, "pointSymbolColumn" , "pointSymbol");

  // bivariate
  addProperty("bivariate", this, "bivariate"         , "enabled");
  addProperty("bivariate", this, "bivariateLineWidth", "width"  );

  // stacked
  addProperty("stacked", this, "stacked", "enabled");

  // points
  addProperty("points", this, "points"      , "visible");
  addProperty("points", this, "pointsColor" , "color"  );
  addProperty("points", this, "symbolName"  , "symbol" );
  addProperty("points", this, "symbolSize"  , "size"   );
  addProperty("points", this, "symbolFilled", "filled" );

  // lines
  addProperty("lines", this, "lines"          , "visible"   );
  addProperty("lines", this, "linesSelectable", "selectable");
  addProperty("lines", this, "linesColor"     , "color"     );
  addProperty("lines", this, "linesWidth"     , "width"     );

  // fill under
  addProperty("fillUnder", this, "fillUnder"       , "visible" );
  addProperty("fillUnder", this, "fillUnderColor"  , "color"   );
  addProperty("fillUnder", this, "fillUnderAlpha"  , "alpha"   );
  addProperty("fillUnder", this, "fillUnderPattern", "pattern" );
  addProperty("fillUnder", this, "fillUnderPos"    , "position");
  addProperty("fillUnder", this, "fillUnderSide"   , "side"    );

  // impulse
  addProperty("impulse", this, "impulse", "visible");

  // data label
  addProperty("dataLabel", this, "dataLabelColor", "color");
  addProperty("dataLabel", this, "dataLabelAngle", "angle");
}

QString
CQChartsXYPlot::
pointsColorStr() const
{
  return pointObj_.colorStr();
}

void
CQChartsXYPlot::
setPointsColorStr(const QString &str)
{
  pointObj_.setColorStr(str);

  update();
}

QString
CQChartsXYPlot::
linesColorStr() const
{
  return lineObj_.colorStr();
}

void
CQChartsXYPlot::
setLinesColorStr(const QString &str)
{
  lineObj_.setColorStr(str);

  update();
}

QString
CQChartsXYPlot::
fillUnderColorStr() const
{
  return fillUnderData_.fillObj.colorStr();
}

void
CQChartsXYPlot::
setFillUnderColorStr(const QString &str)
{
  fillUnderData_.fillObj.setColorStr(str);

  update();
}

void
CQChartsXYPlot::
drawBivariateLine(QPainter *painter, const QPointF &p1, const QPointF &p2, const QColor &c)
{
  bivariateLineObj_.setColor(c);

  bivariateLineObj_.draw(painter, p1, p2);
}

QColor
CQChartsXYPlot::
pointColor(int i, int n) const
{
  if (! pointObj_.isPalette())
    return pointObj_.color();

  return paletteColor(i, n);
}

QColor
CQChartsXYPlot::
lineColor(int i, int n) const
{
  if (! lineObj_.isPalette())
    return lineObj_.color();

  return paletteColor(i, n);
}

QColor
CQChartsXYPlot::
fillUnderColor(int i, int n) const
{
  if (! fillUnderData_.fillObj.isPalette())
    return fillUnderData_.fillObj.color();

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
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int n = model->rowCount(QModelIndex());

  dataRange_.reset();

  int ns = numSets();

  using Reals = std::vector<double>;

  Reals sum, lastSum;

  sum.resize(ns);

  for (int i = 0; i < n; ++i) {
    lastSum = sum;

    //---

    QModelIndex xind = model->index(i, xColumn());

    //---

    bool ok1;

    double x = CQChartsUtil::modelReal(model, xind, ok1);

    if (! ok1) x = i;

    if (CQChartsUtil::isNaN(x) || CQChartsUtil::isInf(x))
      continue;

    if      (isBivariate() && ns > 1) {
      for (int j = 0; j < ns; ++j) {
        int yColumn = getSetColumn(j);

        QModelIndex yind = model->index(i, yColumn);

        //---

        bool ok2;

        double y = CQChartsUtil::modelReal(model, yind, ok2);

        if (! ok2) y = i;

        if (CQChartsUtil::isNaN(y) || CQChartsUtil::isInf(y))
          continue;

        dataRange_.updateRange(x, y);
      }
    }
    else if (isStacked()) {
      double sum1 = 0.0;

      for (int j = 0; j < ns; ++j) {
        int yColumn = getSetColumn(j);

        QModelIndex yind = model->index(i, yColumn);

        //---

        bool ok2;

        double y = CQChartsUtil::modelReal(model, yind, ok2);

        if (! ok2) y = i;

        if (CQChartsUtil::isNaN(y) || CQChartsUtil::isInf(y))
          continue;

        sum1 += y;
      }

      dataRange_.updateRange(x, 0.0);
      dataRange_.updateRange(x, sum1);
    }
    else {
      for (int j = 0; j < ns; ++j) {
        int yColumn = getSetColumn(j);

        QModelIndex yind = model->index(i, yColumn);

        //---

        bool ok2;

        double y = CQChartsUtil::modelReal(model, yind, ok2);

        if (! ok2) y = i;

        if (CQChartsUtil::isNaN(y) || CQChartsUtil::isInf(y))
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

  if (xmin_) dataRange_.setLeft  (*xmin_);
  if (ymin_) dataRange_.setBottom(*ymin_);
  if (xmax_) dataRange_.setRight (*xmax_);
  if (ymax_) dataRange_.setTop   (*ymax_);

  //---

  setXValueColumn(xColumn());
  setYValueColumn(yColumn());

  //---

  if (isFirstPlot())
    xAxis()->setColumn(xColumn());

  QString xname = model->headerData(xColumn(), Qt::Horizontal).toString();

  if (isFirstPlot())
    xAxis()->setLabel(xname);

  if      (isBivariate() && ns > 1) {
    QString name = title();

    if (! name.length()) {
      int yColumn1 = getSetColumn(0);
      int yColumn2 = getSetColumn(1);

      QString yname1 = model->headerData(yColumn1, Qt::Horizontal).toString();
      QString yname2 = model->headerData(yColumn2, Qt::Horizontal).toString();

      name = QString("%1-%2").arg(yname1).arg(yname2);
    }

    if      (isFirstPlot())
      yAxis()->setLabel(name);
    else if (isOverlayOtherPlot())
      firstPlot()->yAxis()->setLabel(firstPlot()->yAxis()->label() + ", " + name);
  }
  else if (isStacked()) {
  }
  else {
    int yColumn = getSetColumn(0);

    if (isFirstPlot())
      yAxis()->setColumn(yColumn);

    QString yname;

    for (int j = 0; j < numSets(); ++j) {
      QString yname1 = model->headerData(getSetColumn(j), Qt::Horizontal).toString();

      if (yname.length())
        yname += ", ";

      yname += yname1;
    }

    if      (isFirstPlot())
      yAxis()->setLabel(yname);
    else if (isOverlayOtherPlot())
      firstPlot()->yAxis()->setLabel(firstPlot()->yAxis()->label() + ", " + yname);
  }

  //---

  if (apply)
    applyDataRange();
}

void
CQChartsXYPlot::
postInit()
{
  int ns = numSets();

  if      (isBivariate() && ns > 1) {
    setLines (true);
    setPoints(false);
  }
  else if (isStacked()) {
    setFillUnder(true);
    setPoints   (false);
  }
}

void
CQChartsXYPlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  // TODO: use actual symbol size
  double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  int n = model->rowCount(QModelIndex());

  int ns = numSets();

  if      (isBivariate() && ns > 1) {
    using Polygons = std::vector<QPolygonF>;

    Polygons polygons1, polygons2;

    polygons1.resize(ns - 1);
    polygons2.resize(ns - 1);

    for (int i = 0; i < n; ++i) {
      QModelIndex xind = model->index(i, xColumn());

      //---

      bool ok1;

      double x = CQChartsUtil::modelReal(model, xind, ok1);

      if (! ok1) x = i;

      if (CQChartsUtil::isNaN(x) || CQChartsUtil::isInf(x))
        continue;

      std::vector<double> yVals;

      for (int j = 0; j < ns; ++j) {
        bool hidden = isSetHidden(j);

        if (hidden)
          continue;

        //---

        int yColumn = getSetColumn(j);

        QModelIndex yind = model->index(i, yColumn);

        //---

        bool ok2;

        double y = CQChartsUtil::modelReal(model, yind, ok2);

        if (! ok2) y = i;

        if (CQChartsUtil::isNaN(y) || CQChartsUtil::isInf(y))
          continue;

        yVals.push_back(y);
      }

      // need at least 2 values
      if (yVals.size() < 2)
        continue;

      // connect each y value to next y value
      double y1 = yVals[0];
      int    ny = yVals.size();

      for (int j = 1; j < ny; ++j) {
        double y2 = yVals[j];

        CQChartsGeom::BBox bbox(x - sw/2, y1 - sh/2, x + sw/2, y2 + sh/2);

        if (! isFillUnder()) {
          // use vertical line object for each point pair if not fill under
          CQChartsXYBiLineObj *lineObj =
            new CQChartsXYBiLineObj(this, bbox, x, y1, y2, j - 1, ny - 1);

          QString xstr  = xStr(x);
          QString y1str = yStr(y1);
          QString y2str = yStr(y2);

          if (nameColumn() >= 0) {
            bool ok;

            QString name = CQChartsUtil::modelString(model, i, nameColumn(), ok);

            lineObj->setId(QString("%1:%2:%3").arg(name).arg(y1str).arg(y2str));
          }
          else
            lineObj->setId(QString("%1:%2:%3").arg(xstr).arg(y1str).arg(y2str));

          addPlotObject(lineObj);
        }
        else {
          QPolygonF &poly1 = polygons1[j - 1];
          QPolygonF &poly2 = polygons2[j - 1];

          // build lower and upper poly line for fill under polygon
          poly1 << QPointF(x, y1);
          poly2 << QPointF(x, y2);
        }

        y1 = y2;
      }
    }

    // add lower, upper and polygon objects for fill under
    if (isFillUnder()) {
      QString name = title();

      if (! name.length()) {
        int yColumn1 = getSetColumn(0);
        int yColumn2 = getSetColumn(1);

        QString yname1 = model->headerData(yColumn1, Qt::Horizontal).toString();
        QString yname2 = model->headerData(yColumn2, Qt::Horizontal).toString();

        name = QString("%1-%2").arg(yname1).arg(yname2);
      }

      for (int j = 1; j < ns; ++j) {
        bool hidden = isSetHidden(j);

        if (hidden)
          continue;

        //---

        QString name1;

        if (ns > 2)
          name1 = QString("%1:%2").arg(name).arg(j);
        else
          name1 = name;

        //---

        QPolygonF &poly1 = polygons1[j - 1];
        QPolygonF &poly2 = polygons2[j - 1];

        addPolyLine(poly1, j - 1, ns - 1, name1);
        addPolyLine(poly2, j - 1, ns - 1, name1);

        int len = poly1.length();

        if      (fillUnderSide() == "both") {
          // add upper poly line to lower one (points reversed) to build fill polygon
          for (int k = len - 1; k >= 0; --k)
            poly1 << poly2[k];
        }
        else if (fillUnderSide() == "above") {
          QPolygonF poly3, poly4;

          QPointF pa1, pb1; bool above1 = true;

          for (int k = 0; k < len; ++k) {
            const QPointF &pa2 = poly1[k];
            const QPointF &pb2 = poly2[k];

            bool above2 = (pa2.y() > pb2.y());

            if (k > 0 && above1 != above2) {
              QPointF pi;

              CQChartsUtil::intersectLines(pa1, pa2, pb1, pb2, pi);

              poly3 << pi;
              poly4 << pi;
            }

            if (above2) {
              poly3 << pa2;
              poly4 << pb2;
            }

            pa1 = pa2; pb1 = pb2; above1 = above2;
          }

          len = poly4.length();

          for (int k = len - 1; k >= 0; --k)
            poly3 << poly4[k];

          poly1 = poly3;
        }
        else if (fillUnderSide() == "below") {
          QPolygonF poly3, poly4;

          QPointF pa1, pb1; bool below1 = true;

          for (int k = 0; k < len; ++k) {
            const QPointF &pa2 = poly1[k];
            const QPointF &pb2 = poly2[k];

            bool below2 = (pa2.y() < pb2.y());

            if (k > 0 && below1 != below2) {
              QPointF pi;

              CQChartsUtil::intersectLines(pa1, pa2, pb1, pb2, pi);

              poly3 << pi;
              poly4 << pi;
            }

            if (below2) {
              poly3 << pa2;
              poly4 << pb2;
            }

            pa1 = pa2; pb1 = pb2; below1 = below2;
          }

          len = poly4.length();

          for (int k = len - 1; k >= 0; --k)
            poly3 << poly4[k];

          poly1 = poly3;
        }

        addPolygon(poly1, j - 1, ns - 1, name1);
      }
    }
  }
  else if (isStacked()) {
    using Reals = std::vector<double>;

    Reals sum, lastSum;

    sum.resize(n);

    for (int j = 0; j < ns; ++j) {
      bool hidden = isSetHidden(j);

      if (hidden)
        continue;

      //---

      int yColumn = getSetColumn(j);

      QString name = model->headerData(yColumn, Qt::Horizontal).toString();

      //---

      QPolygonF poly, polyLine;

      lastSum = sum;

      for (int i = 0; i < n; ++i) {
        QModelIndex xind = model->index(i, xColumn());
        QModelIndex yind = model->index(i, yColumn  );

        //---

        bool ok1, ok2;

        double x = CQChartsUtil::modelReal(model, xind, ok1);
        double y = CQChartsUtil::modelReal(model, yind, ok2);

        if (! ok1) x = i;
        if (! ok2) y = i;

        if (CQChartsUtil::isNaN(x) || CQChartsUtil::isInf(x) ||
            CQChartsUtil::isNaN(y) || CQChartsUtil::isInf(y)) {
          if (polyLine.count()) {
            addPolyLine(polyLine, j, ns, name);

            polyLine = QPolygonF();
          }

          continue;
        }

        double y1 = y + lastSum[i];

        //---

        QString name1;

        if (nameColumn() >= 0) {
          bool ok;

          name1 = CQChartsUtil::modelString(model, i, nameColumn(), ok);
        }
        else
          name1 = name;

        //---

        double size = -1;

        if (sizeColumn() >= 0) {
          bool ok;

          size = CQChartsUtil::modelReal(model, i, sizeColumn(), ok);

          if (! ok)
            size = -1;
        }

        //---

        CQChartsGeom::BBox bbox(x - sw/2, y1 - sh/2, x + sw/2, y1 + sh/2);

        CQChartsXYPointObj *pointObj =
          new CQChartsXYPointObj(this, bbox, x, y1, size, xind, j, ns, i, n);

        QString xstr = xStr(x);
        QString ystr = yStr(y1);

        if (name1.length())
          pointObj->setId(QString("%1:%2:%3").arg(name1).arg(xstr).arg(ystr));
        else
          pointObj->setId(QString("%1:%2:%3:%4").arg(i).arg(j).arg(xstr).arg(ystr));

        addPlotObject(pointObj);

        //---

        if (i == 0)
          poly << QPointF(x, lastSum[i]);

        poly     << QPointF(x, y1);
        polyLine << QPointF(x, y1);

        if (i == n - 1) {
          for (int k = i; k > 0; --k) {
            QModelIndex xind = model->index(k, xColumn());

            //---

            bool ok1;

            double x1 = CQChartsUtil::modelReal(model, xind, ok1);

            if (! ok1) x1 = k;

            if (CQChartsUtil::isNaN(x1) || CQChartsUtil::isInf(x1))
              continue;

            poly << QPointF(x1, lastSum[k]);
          }
        }

        //---

        sum[i] = y1;
      }

      //---

      if (polyLine.count())
        addPolyLine(polyLine, j, ns, name);

      //---

      addPolygon(poly, j, ns, name);
    }
  }
  else {
    for (int j = 0; j < ns; ++j) {
      bool hidden = isSetHidden(j);

      if (hidden)
        continue;

      //---

      int yColumn = getSetColumn(j);

      QString name = model->headerData(yColumn, Qt::Horizontal).toString();

      //---

      double sum     = 0.0;
      double lastSum = 0.0;

      QPolygonF poly, polyLine;

      for (int i = 0; i < n; ++i) {
        lastSum = sum;

        //---

        QModelIndex xind = model->index(i, xColumn());
        QModelIndex yind = model->index(i, yColumn  );

        //---

        bool ok1, ok2;

        double x = CQChartsUtil::modelReal(model, xind, ok1);
        double y = CQChartsUtil::modelReal(model, yind, ok2);

        if (! ok1) x = i;
        if (! ok2) y = i;

        if (CQChartsUtil::isNaN(x) || CQChartsUtil::isInf(x) ||
            CQChartsUtil::isNaN(y) || CQChartsUtil::isInf(y)) {
          if (polyLine.count()) {
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

        //---

        QString name1 = name;

        if (nameColumn() >= 0) {
          bool ok;

          name1 = CQChartsUtil::modelString(model, i, nameColumn(), ok);
        }

        //---

        double size = -1;

        if (sizeColumn() >= 0) {
          bool ok;

          size = CQChartsUtil::modelReal(model, i, sizeColumn(), ok);

          if (! ok)
            size = -1;
        }

        //---

        CQChartsGeom::BBox bbox(x - sw/2, y1 - sh/2, x + sw/2, y1 + sh/2);

        CQChartsXYPointObj *pointObj =
          new CQChartsXYPointObj(this, bbox, x, y1, size, xind, j, ns, i, n);

        QString xstr = xStr(x);
        QString ystr = yStr(y1);

        if (name1.length())
          pointObj->setId(QString("%1:%2:%3").arg(name1).arg(xstr).arg(ystr));
        else
          pointObj->setId(QString("%1:%2:%3:%4").arg(i).arg(j).arg(xstr).arg(ystr));

        if (pointLabelColumn() >= 0) {
          bool ok;

          QString pointLabelStr = CQChartsUtil::modelString(model, i, pointLabelColumn(), ok);

          if (ok && pointLabelStr.length())
            pointObj->setLabel(pointLabelStr);
        }

        if (pointColorColumn() >= 0) {
          bool ok;

          QString pointColorStr = CQChartsUtil::modelString(model, i, pointColorColumn(), ok);

          if (ok && pointColorStr.length())
            pointObj->setColor(QColor(pointColorStr));
        }

        if (pointSymbolColumn() >= 0) {
          bool ok;

          QString pointSymbolStr = CQChartsUtil::modelString(model, i, pointSymbolColumn(), ok);

          if (ok && pointSymbolStr.length())
            pointObj->setSymbol(CSymbol2DMgr::nameToType(pointSymbolStr));
        }

        addPlotObject(pointObj);

        //---

        if (isImpulse()) {
          double ys = std::min(y1, 0.0);
          double ye = std::max(y1, 0.0);

          CQChartsGeom::BBox bbox(x, ys, x, ye);

          CQChartsXYImpulseLineObj *lineObj =
            new CQChartsXYImpulseLineObj(this, bbox, x, ys, x, ye, j, ns);

          QString xstr = xStr(x);
          QString ystr = yStr(y1);

          if (name1.length())
            lineObj->setId(QString("%1:%2:%3").arg(name1).arg(xstr).arg(ystr));
          else
            lineObj->setId(QString("%1:%2:%3:%4").arg(i).arg(j).arg(xstr).arg(ystr));

          addPlotObject(lineObj);
        }

        //---

        if (i == 0)
          poly << fillUnderPos(x, dataRange_.ymin());

        poly     << QPointF(x, y1);
        polyLine << QPointF(x, y1);

        if (i == n - 1)
          poly << fillUnderPos(x, dataRange_.ymin());
      }

      //---

      if (polyLine.count())
        addPolyLine(polyLine, j, ns, name);

      //---

      addPolygon(poly, j, ns, name);
    }
  }

  //----

  resetKeyItems();
}

QPointF
CQChartsXYPlot::
fillUnderPos(double x, double y) const
{
  const QString &str = fillUnderPosStr();

  QStringList strs = str.split(" ", QString::KeepEmptyParts);

  if (! strs.length())
    return QPointF(x, y);

  double x1 = x;
  double y1 = y;

  if (strs.length() > 1) {
    const QString &xstr = strs[0];
    const QString &ystr = strs[1];

    if      (xstr == "min" || xstr == "xmin")
      x1 = dataRange_.xmin();
    else if (xstr == "max" || xstr == "xmax")
      x1 = dataRange_.xmax();
    else {
      bool ok;

      x1 = CQChartsUtil::toReal(xstr, ok);

      if (! ok)
        x1 = x;
    }

    if      (ystr == "min" || ystr == "ymin")
      y1 = dataRange_.ymin();
    else if (ystr == "max" || ystr == "ymax")
      y1 = dataRange_.ymax();
    else {
      bool ok;

      y1 = CQChartsUtil::toReal(ystr, ok);

      if (! ok)
        y1 = y;
    }
  }
  else {
    const QString &str = strs[0];

    if      (str == "xmin")
      x1 = dataRange_.xmin();
    else if (str == "xmax")
      x1 = dataRange_.xmax();
    else if (str == "min" || str == "ymin")
      y1 = dataRange_.ymin();
    else if (str == "max" || str == "ymax")
      y1 = dataRange_.ymax();
    else {
      bool ok;

      y1 = CQChartsUtil::toReal(str, ok);

      if (! ok)
        y1 = y;
    }
  }

  return QPointF(x1, y1);
}

void
CQChartsXYPlot::
addPolyLine(const QPolygonF &polyLine, int i, int n, const QString &name)
{
  CQChartsGeom::BBox bbox = CQChartsUtil::fromQRect(polyLine.boundingRect());

  CQChartsXYPolylineObj *lineObj = new CQChartsXYPolylineObj(this, bbox, polyLine, i, n);

  lineObj->setId(QString("%1").arg(name));

  addPlotObject(lineObj);
}

void
CQChartsXYPlot::
addPolygon(const QPolygonF &poly, int i, int n, const QString &name)
{
  CQChartsGeom::BBox bbox = CQChartsUtil::fromQRect(poly.boundingRect());

  CQChartsXYPolygonObj *polyObj = new CQChartsXYPolygonObj(this, bbox, poly, i, n);

  polyObj->setId(QString("%1").arg(name));

  addPlotObject(polyObj);
}

void
CQChartsXYPlot::
addKeyItems(CQChartsKey *key)
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int row, col;

  if (! key->isHorizontal()) {
    row = key->maxRow();
    col = 0;
  }
  else {
    row = 0;
    col = key->maxCol();
  }

  int ns = numSets();

  if      (isBivariate() && ns > 1) {
    QString name = title();

    if (! name.length()) {
      int yColumn1 = getSetColumn(0);
      int yColumn2 = getSetColumn(1);

      QString yname1 = model->headerData(yColumn1, Qt::Horizontal).toString();
      QString yname2 = model->headerData(yColumn2, Qt::Horizontal).toString();

      name = QString("%1-%2").arg(yname1).arg(yname2);
    }

    CQChartsXYKeyColor *color = new CQChartsXYKeyColor(this, 0, 1);
    CQChartsXYKeyText  *text  = new CQChartsXYKeyText (this, 0, name);

    key->addItem(color, row, col    );
    key->addItem(text , row, col + 1);
  }
  else if (isStacked()) {
    for (int i = 0; i < ns; ++i) {
      int yColumn = getSetColumn(i);

      QString name = model->headerData(yColumn, Qt::Horizontal).toString();

      CQChartsXYKeyLine *line = new CQChartsXYKeyLine(this, i, ns);
      CQChartsXYKeyText *text = new CQChartsXYKeyText(this, i, name);

      key->addItem(line, row + i, col    );
      key->addItem(text, row + i, col + 1);
    }
  }
  else {
    for (int i = 0; i < ns; ++i) {
      int yColumn = getSetColumn(i);

      QString name = model->headerData(yColumn, Qt::Horizontal).toString();

      if (ns == 1 && (name == "" || name == QString("%1").arg(yColumn + 1))) {
        if      (title().length())
          name = title();
        else if (fileName().length())
          name = fileName();
      }

      CQChartsXYKeyLine *line = new CQChartsXYKeyLine(this, i, ns);
      CQChartsXYKeyText *text = new CQChartsXYKeyText(this, i, name);

      if (! key->isHorizontal()) {
        key->addItem(line, row + i, col    );
        key->addItem(text, row + i, col + 1);
      }
      else {
        key->addItem(line, row, col + 2*i    );
        key->addItem(text, row, col + 2*i + 1);
      }
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
  if (i < 0 || (i > 1 && i >= int(yColumns_.size())))
    return -1;

  if (! yColumns_.empty())
    return yColumns_[i];

  return yColumn();
}

bool
CQChartsXYPlot::
interpY(double x, std::vector<double> &yvals) const
{
  int ns = numSets();

  if (isBivariate() && ns > 1) {
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

  drawParts(p);
}

//------

CQChartsXYBiLineObj::
CQChartsXYBiLineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect, double x,
                    double y1, double y2, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), x_(x), y1_(y1), y2_(y2), i_(i), n_(n)
{
}

bool
CQChartsXYBiLineObj::
visible() const
{
  if (! plot_->isLines() && ! plot_->isPoints())
    return false;

  return isVisible();
}

bool
CQChartsXYBiLineObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  double px, py1, py2;

  plot_->windowToPixel(x_, y1_, px, py1);
  plot_->windowToPixel(x_, y2_, px, py2);

  double s = plot_->symbolSize();

  CQChartsGeom::BBox pbbox(px - s, py1 - s, px + s, py2 + s);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsXYBiLineObj::
mousePress(const CQChartsGeom::Point &)
{
  if (! visible())
    return;
}

bool
CQChartsXYBiLineObj::
isIndex(const QModelIndex &) const
{
  return false;
}

void
CQChartsXYBiLineObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  if (! visible())
    return;

  double px, py1, py2;

  plot_->windowToPixel(x_, y1_, px, py1);
  plot_->windowToPixel(x_, y2_, px, py2);

  if (plot_->isLines()) {
    QPen   pen  (plot_->fillUnderColor(i_, n_));
    QBrush brush(Qt::NoBrush);

    plot_->updateObjPenBrushState(this, pen, brush);

    plot_->drawBivariateLine(p, QPointF(px, py1), QPointF(px, py2), pen.color());
  }

  if (plot_->isPoints()) {
    QColor          c      = plot_->pointColor(i_, n_);
    CSymbol2D::Type symbol = plot_->symbolType();
    double          s      = plot_->symbolSize();
    bool            filled = plot_->isSymbolFilled();

    QPen   pen  (c);
    QBrush brush(Qt::NoBrush);

    plot_->updateObjPenBrushState(this, pen, brush);

    CQChartsPointObj::draw(p, QPointF(px, py1), symbol, s, pen.color(), filled);
    CQChartsPointObj::draw(p, QPointF(px, py2), symbol, s, pen.color(), filled);
  }
}

//------

CQChartsXYImpulseLineObj::
CQChartsXYImpulseLineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect, double x1, double y1,
                         double x2, double y2, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), x1_(x1), y1_(y1), x2_(x2), y2_(y2), i_(i), n_(n)
{
}

bool
CQChartsXYImpulseLineObj::
visible() const
{
  return isVisible();
}

bool
CQChartsXYImpulseLineObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  double px1, py1, px2, py2;

  plot_->windowToPixel(x1_, y1_, px1, py1);
  plot_->windowToPixel(x2_, y2_, px2, py2);

  double b = 2;

  CQChartsGeom::BBox pbbox(px1 - b, py1 - b, px2 + b, py2 + b);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsXYImpulseLineObj::
mousePress(const CQChartsGeom::Point &)
{
  if (! visible())
    return;
}

bool
CQChartsXYImpulseLineObj::
isIndex(const QModelIndex &) const
{
  return false;
}

void
CQChartsXYImpulseLineObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  if (! visible())
    return;

  double px1, py1, px2, py2;

  plot_->windowToPixel(x1_, y1_, px1, py1);
  plot_->windowToPixel(x2_, y2_, px2, py2);

  QColor c = plot_->pointColor(i_, n_);

  QBrush brush(Qt::NoBrush);
  QPen   pen  (c);

  plot_->updateObjPenBrushState(this, pen, brush);

  p->setPen(pen);

  p->drawLine(px1, py1, px2, py2);
}

//------

CQChartsXYPointObj::
CQChartsXYPointObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect, double x, double y,
                   double size, const QModelIndex &ind, int iset, int nset, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), x_(x), y_(y), size_(size), ind_(ind),
 iset_(iset), nset_(nset), i_(i), n_(n)
{
}

CQChartsXYPointObj::
~CQChartsXYPointObj()
{
  delete edata_;
}

void
CQChartsXYPointObj::
setLabel(const QString &label)
{
  if (! edata_)
    edata_ = new ExtraData;

  edata_->label = label;
}

void
CQChartsXYPointObj::
setColor(const QColor &c)
{
  if (! edata_)
    edata_ = new ExtraData;

  edata_->c = c;
}

void
CQChartsXYPointObj::
setSymbol(CSymbol2D::Type symbol)
{
  if (! edata_)
    edata_ = new ExtraData;

  edata_->symbol = symbol;
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
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  double px, py;

  plot_->windowToPixel(x_, y_, px, py);

  double s = (size_ <= 0 ? plot_->symbolSize() : size_);

  CQChartsGeom::BBox pbbox(px - s, py - s, px + s, py + s);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsXYPointObj::
mousePress(const CQChartsGeom::Point &)
{
  if (! visible())
    return;

  plot_->beginSelect();

  int yColumn = plot_->getSetColumn(iset_);

  plot_->addSelectIndex(ind_.row(), plot_->xColumn());
  plot_->addSelectIndex(ind_.row(), yColumn         );

  plot_->endSelect();
}

bool
CQChartsXYPointObj::
isIndex(const QModelIndex &ind) const
{
  return (ind == ind_);
}

void
CQChartsXYPointObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  if (! visible())
    return;

  CSymbol2D::Type symbol = plot_->symbolType();
  QColor          c      = plot_->pointColor(iset_, nset_);
  bool            filled = plot_->isSymbolFilled();

  if (edata_ && edata_->symbol != CSymbol2D::Type::NONE)
    symbol = edata_->symbol;

  if (edata_ && edata_->c.isValid())
    c = edata_->c;

  double s = (size_ <= 0 ? plot_->symbolSize() : size_);

  QBrush brush(Qt::NoBrush);
  QPen   pen  (c);

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  CQChartsGeom::Point pp(x_, y_);

  double px, py;

  plot_->windowToPixel(pp.x, pp.y, px, py);

  CQChartsPointObj::draw(p, QPointF(px, py), symbol, s, pen.color(), filled);

  if (edata_ && edata_->label != "") {
    p->setPen(plot_->dataLabelColor());

    CQRotatedText::drawRotatedText(p, px, py, edata_->label, plot_->dataLabelAngle(),
                                   Qt::AlignHCenter | Qt::AlignBottom);
  }
}

//------

CQChartsXYPolylineObj::
CQChartsXYPolylineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect,
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
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  if (! plot_->isLinesSelectable())
    return false;

  double px, py;

  plot_->windowToPixel(p.x, p.y, px, py);

  CQChartsGeom::Point pp(px, py);

  for (int i = 1; i < poly_.count(); ++i) {
    double x1 = poly_[i - 1].x();
    double y1 = poly_[i - 1].y();
    double x2 = poly_[i    ].x();
    double y2 = poly_[i    ].y();

    double px1, py1, px2, py2;

    plot_->windowToPixel(x1, y1, px1, py1);
    plot_->windowToPixel(x2, y2, px2, py2);

    CQChartsGeom::Point pl1(px1, py1);
    CQChartsGeom::Point pl2(px2, py2);

    double d;

    if (CQChartsUtil::PointLineDistance(pp, pl1, pl2, &d) && d < 1)
      return true;
  }

  return false;
}

bool
CQChartsXYPolylineObj::
interpY(double x, std::vector<double> &yvals) const
{
  if (! visible())
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
mousePress(const CQChartsGeom::Point &)
{
  if (! visible())
    return;
}

bool
CQChartsXYPolylineObj::
isIndex(const QModelIndex &) const
{
  return false;
}

void
CQChartsXYPolylineObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  if (! visible())
    return;

  int ns = plot_->numSets();

  QColor c;

  if      (plot_->isBivariate() && ns > 1)
    c = Qt::black;
  else if (plot_->isStacked())
    c = plot_->lineColor(i_, n_);
  else
    c = plot_->lineColor(i_, n_);

  QPen   pen  (c);
  QBrush brush(Qt::NoBrush);

  if (isInside())
    pen.setWidth(3);
  else
    pen.setWidth(plot_->linesWidth());

  plot_->updateObjPenBrushState(this, pen, brush);

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
CQChartsXYPolygonObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect,
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
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  return poly_.containsPoint(CQChartsUtil::toQPoint(p), Qt::OddEvenFill);
}

void
CQChartsXYPolygonObj::
mousePress(const CQChartsGeom::Point &)
{
  if (! visible())
    return;
}

bool
CQChartsXYPolygonObj::
isIndex(const QModelIndex &) const
{
  return false;
}

void
CQChartsXYPolygonObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  if (! visible())
    return;

  QBrush brush;

  QColor fillColor;

  if (plot_->isStacked())
    fillColor = plot_->fillUnderColor(i_, n_);
  else
    fillColor = plot_->fillUnderColor(i_, n_);

  fillColor.setAlpha(plot_->fillUnderAlpha()*255);

  brush.setColor(fillColor);

  brush.setStyle(CQChartsFillObj::patternToStyle(
    (CQChartsFillObj::Pattern) plot_->fillUnderPattern()));

  QPen pen(Qt::NoPen);

  plot_->updateObjPenBrushState(this, pen, brush);

  p->setPen  (pen);
  p->setBrush(brush);

  QPolygonF poly;

  for (int i = 0; i < poly_.count(); ++i) {
    double px, py;

    plot_->windowToPixel(poly_[i].x(), poly_[i].y(), px, py);

    poly << QPointF(px, py);
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
mousePress(const CQChartsGeom::Point &)
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->updateObjs();

  return true;
}

QBrush
CQChartsXYKeyColor::
fillBrush() const
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  int ns = plot->numSets();

  QBrush brush;

  QColor c;

  if      (plot->isBivariate() && ns > 1) {
    c = plot->fillUnderColor(i_, n_);

    c.setAlpha(plot->fillUnderAlpha()*255);

    brush.setStyle(CQChartsFillObj::patternToStyle(
      (CQChartsFillObj::Pattern) plot->fillUnderPattern()));
  }
  else if (plot->prevPlot() || plot->nextPlot())
    c = plot->lineColor(i_, n_);
  else
    c = CQChartsKeyColorBox::fillBrush().color();

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->bgColor(), 0.5);

  brush.setColor(c);

  return brush;
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

  QFontMetricsF fm(plot->view()->font());

  double w = fm.width("X-X");
  double h = fm.height();

  double ww = keyPlot->pixelToWindowWidth (w + 8);
  double wh = keyPlot->pixelToWindowHeight(h + 2);

  return QSizeF(ww, wh);
}

bool
CQChartsXYKeyLine::
mousePress(const CQChartsGeom::Point &)
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->updateObjs();

  return true;
}

void
CQChartsXYKeyLine::
draw(QPainter *p, const CQChartsGeom::BBox &rect)
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  CQChartsXYPlot *keyPlot = qobject_cast<CQChartsXYPlot *>(key_->plot());

  CQChartsGeom::BBox prect;

  keyPlot->windowToPixel(rect, prect);

  QRectF prect1(QPointF(prect.getXMin() + 2, prect.getYMin() + 2),
                QPointF(prect.getXMax() - 2, prect.getYMax() - 2));

  QColor pointColor = plot->pointColor(i_, n_);
  QColor lineColor  = plot->lineColor (i_, n_);

  if (plot->isSetHidden(i_)) {
    pointColor = CQChartsUtil::blendColors(pointColor, key_->bgColor(), 0.5);
    lineColor  = CQChartsUtil::blendColors(lineColor , key_->bgColor(), 0.5);
  }

  if (plot->isFillUnder()) {
    QColor fillColor = keyPlot->fillUnderColor(i_, n_);

    fillColor.setAlpha(keyPlot->fillUnderAlpha()*255);

    double x1 = prect.getXMin() + 4;
    double x2 = prect.getXMax() - 4;
    double y1 = prect.getYMin() + 4;
    double y2 = prect.getYMax() - 4;

    p->fillRect(CQChartsUtil::toQRect(CQChartsGeom::BBox(x1, y1, x2, y2)), fillColor);
  }

  if (plot->isLines() || plot->isImpulse()) {
    double x1 = prect.getXMin() + 4;
    double x2 = prect.getXMax() - 4;
    double y  = prect.getYMid();

    p->setPen(lineColor);

    p->drawLine(x1, y, x2, y);
  }

  if (plot->isPoints()) {
    double dx = keyPlot->pixelToWindowWidth(4);

    double x1 = rect.getXMin() + dx;
    double x2 = rect.getXMax() - dx;
    double y  = rect.getYMid();

    double px1, px2, py;

    keyPlot->windowToPixel(x1, y, px1, py);
    keyPlot->windowToPixel(x2, y, px2, py);

    CSymbol2D::Type symbol = plot->symbolType();
    double          s      = plot->symbolSize();
    bool            filled = plot->isSymbolFilled();

    if (plot->isLines() || plot->isImpulse()) {
      CQChartsPointObj::draw(p, QPointF(px1, py), symbol, s, pointColor, filled);
      CQChartsPointObj::draw(p, QPointF(px2, py), symbol, s, pointColor, filled);
    }
    else {
      double px = CQChartsUtil::avg(px1, px2);

      CQChartsPointObj::draw(p, QPointF(px, py), symbol, s, pointColor, filled);
    }
  }
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
    c = CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), 0.5);

  return c;
}
