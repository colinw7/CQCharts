#include <CQChartsPivotPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsTip.h>
#include <CQChartsDataLabel.h>
#include <CQChartsUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsHtml.h>
#include <CQChartsModelDetails.h>
#include <CQChartsPaintDevice.h>
#include <CQCharts.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColors.h>
#include <CQColorsPalette.h>
#include <CQPerfMonitor.h>
#include <CQPivotModel.h>

#include <QMenu>

CQChartsPivotPlotType::
CQChartsPivotPlotType()
{
}

void
CQChartsPivotPlotType::
addParameters()
{
  startParameterGroup("Pivot");

  // name, desc, propName, attributes, default
  addColumnsParameter("x", "X", "xColumns").
    setRequired().setTip("X Key Column(s)");
  addColumnsParameter("y", "Y", "yColumns").
    setRequired().setTip("Y Key Column(s)");

  addColumnParameter("value", "Value", "valueColumn").
    setRequired().setNumeric().setTip("Value Column");

  // options
  addBoolParameter("horizontal", "Horizontal", "horizontal").setTip("Draw bars horizontal");

  endParameterGroup();
}

QString
CQChartsPivotPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Pivot Plot").
    h3("Summary").
     p("A pivot plot displays the values of a column grouped in one or two "
       "dimensions using keys generated from the x and/or y columns.").
     p("The values can be summed, averaged or counted to produced the "
       "displayed valued.").
     p("The plot can be a barchart (side by side to stacked), a line chart "
       "with optional fill under, or a grid plot").
    h3("Limitations").
     p("Logaritmic axes are not supported.").
    h3("Example").
     p(IMG("images/pivot_charts.png"));
}

CQChartsPlot *
CQChartsPivotPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsPivotPlot(view, model);
}

//------

CQChartsPivotPlot::
CQChartsPivotPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("pivot"), model),
 CQChartsObjBarShapeData<CQChartsPivotPlot>(this)
{
  NoUpdate noUpdate(this);

  //---

  dataLabel_ = new CQChartsDataLabel(this);

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true); // for data label

  //---

  setBarFilled   (true);
  setBarFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setBarStroked(true);

  //---

  addAxes();

  addKey();

  addTitle();

  key()->setLocation(CQChartsKeyLocation::Type::AUTO);

  //---

  pivotModel_ = new CQPivotModel(model.data());

  pivotModel_->setIncludeTotals(false);

  updatePivot();
}

CQChartsPivotPlot::
~CQChartsPivotPlot()
{
  delete dataLabel_;
  delete pivotModel_;
}

//---

void
CQChartsPivotPlot::
setXColumns(const CQChartsColumns &c)
{
  CQChartsUtil::testAndSet(xColumns_, c, [&]() { updatePivot(); updateRangeAndObjs(); } );
}

void
CQChartsPivotPlot::
setYColumns(const CQChartsColumns &c)
{
  CQChartsUtil::testAndSet(yColumns_, c, [&]() { updatePivot(); updateRangeAndObjs(); } );
}

void
CQChartsPivotPlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updatePivot(); updateRangeAndObjs(); } );
}

//---

void
CQChartsPivotPlot::
setPlotType(const PlotType &v)
{
  CQChartsUtil::testAndSet(plotType_, v, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPivotPlot::
setValueType(const ValueType &v)
{
  CQChartsUtil::testAndSet(valueType_, v, [&]() { updatePivot(); updateRangeAndObjs(); } );
}

void
CQChartsPivotPlot::
setHorizontal(bool b)
{
  CQChartsUtil::testAndSet(horizontal_, b, [&]() {
    dataLabel_->setDirection(horizontal_ ? Qt::Horizontal : Qt::Vertical);

    CQChartsAxis::swap(xAxis(), yAxis());

    updateRangeAndObjs();
  } );
}

void
CQChartsPivotPlot::
setXSorted(bool b)
{
  CQChartsUtil::testAndSet(xsorted_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPivotPlot::
setYSorted(bool b)
{
  CQChartsUtil::testAndSet(ysorted_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsPivotPlot::
setGridBars(bool b)
{
  CQChartsUtil::testAndSet(gridBars_, b, [&]() { invalidateLayers(); } );
}

//---

void
CQChartsPivotPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsPlot::addProperties();

  // columns
  addProp("columns", "xColumns"   , "x"    , "X key column(s)");
  addProp("columns", "yColumns"   , "y"    , "Y key column(s)");
  addProp("columns", "valueColumn", "value", "Value column");

  // options
  addProp("options", "plotType" , "plotType" , "Plot type" );
  addProp("options", "valueType", "valueType", "Value type");

  addProp("options", "horizontal", "horizontal", "Draw horizontal");
  addProp("options", "xSorted"   , "xSorted"   , "X keys are sorted");
  addProp("options", "ySorted"   , "ySorted"   , "Y keys are sorted");
  addProp("options", "gridBars"  , "gridBars"  , "Draw bars in grid cells");

  // fill
  addProp("fill", "barFilled", "visible", "Bar fill visible");

  addFillProperties("fill", "barFill", "Bar");

  // stroke
  addProp("stroke", "barStroked", "visible", "Bar stroke visible");

  addLineProperties("stroke", "barStroke", "Bar");

  addProp("stroke", "barCornerSize", "cornerSize", "Bar corner size");

  //---

  dataLabel_->addPathProperties("labels", "Labels");
}

void
CQChartsPivotPlot::
getPropertyNames(QStringList &names, bool hidden) const
{
  CQChartsPlot::getPropertyNames(names, hidden);

  propertyModel()->objectNames(dataLabel_, names, hidden);
}

//------

void
CQChartsPivotPlot::
updatePivot()
{
  using Column  = CQPivotModel::Column;
  using Columns = CQPivotModel::Columns;

  Columns xColumns, yColumns;

  for (const auto &c : this->xColumns())
    xColumns.push_back(c.column());

  for (const auto &c : this->yColumns())
    yColumns.push_back(c.column());

  Column valueColumn = this->valueColumn().column();

  pivotModel()->setHColumns(xColumns);
  pivotModel()->setVColumns(yColumns);

  pivotModel()->setValueColumn(valueColumn);

  if      (valueType() == ValueType::COUNT)
    pivotModel()->setValueType(CQPivotModel::ValueType::COUNT);
  else if (valueType() == ValueType::COUNT_UNIQUE)
    pivotModel()->setValueType(CQPivotModel::ValueType::COUNT_UNIQUE);
  else if (valueType() == ValueType::SUM)
    pivotModel()->setValueType(CQPivotModel::ValueType::SUM);
  else if (valueType() == ValueType::AVERAGE)
    pivotModel()->setValueType(CQPivotModel::ValueType::MEAN);
  else if (valueType() == ValueType::MIN)
    pivotModel()->setValueType(CQPivotModel::ValueType::MIN);
  else if (valueType() == ValueType::MAX)
    pivotModel()->setValueType(CQPivotModel::ValueType::MAX);
}

//---

CQChartsGeom::Range
CQChartsPivotPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsPivotPlot::calcRange");

  //---

  CQChartsGeom::Range dataRange;

  auto updateRange = [&](double x, double y) {
    if (! isHorizontal())
      dataRange.updateRange(x, y);
    else
      dataRange.updateRange(y, x);
  };

  //---

  updateRange(0.0, 0.0);

  //---

  QStringList hkeys = pivotModel()->hkeys(isXSorted());
  QStringList vkeys = pivotModel()->vkeys(isYSorted());

  int nh = hkeys.length();
  int nv = vkeys.length();

  bool hasYValues = yColumns().isValid(); // has grouping y columns key

  //---

  // row for vertical keys, (totals if included)
  // column for vertical header, horizontal key, totals (if include)
  if (plotType() == PlotType::STACKED_BAR) {
    for (int iv = 0; iv < nv; ++iv) {
      int r = pivotModel()->vkeyRow(vkeys[iv]);

      double sum = 0.0;

      for (int ih = 0; ih < nh; ++ih) {
        int c = pivotModel()->hkeyCol(hkeys[ih]);

        int c1 = c + 1;

        QModelIndex ind = pivotModel()->index(r, c1, QModelIndex());

        QVariant var = pivotModel()->data(ind, Qt::EditRole);

        bool ok;

        double value = var.toDouble(&ok);
        if (! ok) continue;

        sum += value;
      }

      if (hasYValues) {
        updateRange(iv - 0.5, 0.0);
        updateRange(iv + 0.5, sum);
      }
      else {
        updateRange(-0.5, 0.0);
        updateRange(+0.5, sum);
      }
    }
  }
  else {
    for (int iv = 0; iv < nv; ++iv) {
      int r = pivotModel()->vkeyRow(vkeys[iv]);

      for (int ih = 0; ih < nh; ++ih) {
        int c = pivotModel()->hkeyCol(hkeys[ih]);

        int c1 = c + 1;

        QModelIndex ind = pivotModel()->index(r, c1, QModelIndex());

        QVariant var = pivotModel()->data(ind, Qt::EditRole);

        bool ok;

        double value = var.toDouble(&ok);
        if (! ok) continue;

        if      (plotType() == PlotType::GRID) {
          updateRange(iv - 0.5, ih - 0.5);
          updateRange(iv + 0.5, ih + 0.5);
        }
        else if (plotType() == PlotType::LINES || plotType() == PlotType::AREA) {
          if (hasYValues) {
            updateRange(iv, 0.0);
            updateRange(iv, value);
          }
          else {
            updateRange(ih, 0.0);
            updateRange(ih, value);
          }
        }
        else if (plotType() == PlotType::POINTS) {
          double ss = 5.0;

          double sx = pixelToWindowWidth (ss);
          double sy = pixelToWindowHeight(ss);

          if (hasYValues) {
            updateRange(iv - sx, value - sy);
            updateRange(iv + sx, value + sy);
          }
          else {
            updateRange(ih - sx, value - sy);
            updateRange(ih + sx, value + sy);
          }
        }
        else {
          if (hasYValues) {
            updateRange(iv - 0.5, 0.0  );
            updateRange(iv + 0.5, value);
          }
          else {
            updateRange(ih - 0.5, 0.0  );
            updateRange(ih + 0.5, value);
          }
        }
      }
    }
  }

  //---

  dataRange.makeNonZero();

  return dataRange;
}

//------

CQChartsGeom::BBox
CQChartsPivotPlot::
annotationBBox() const
{
  CQChartsGeom::BBox bbox;

  CQChartsDataLabel::Position position = dataLabel()->position();

  if (position != CQChartsDataLabel::TOP_OUTSIDE && position != CQChartsDataLabel::BOTTOM_OUTSIDE)
    return bbox;

  if (dataLabel()->isVisible()) {
    for (const auto &plotObj : plotObjs_) {
      CQChartsPivotBarObj *barObj = dynamic_cast<CQChartsPivotBarObj *>(plotObj);

      if (barObj)
        bbox += barObj->dataLabelRect();
    }
  }

  return bbox;
}

//------

bool
CQChartsPivotPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsPivotPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  CQChartsAxis *xAxis = mappedXAxis();
  CQChartsAxis *yAxis = mappedYAxis();

  bool hasYValues = yColumns().isValid(); // has grouping y columns key

  //---

  xAxis->clearTickLabels();

  xAxis->setValueType     (CQChartsAxisValueType::Type::INTEGER, /*notify*/false);
  xAxis->setGridMid       (true);
  xAxis->setTicksDisplayed(CQChartsAxis::TicksDisplayed::MAJOR);

  if (hasYValues)
    xAxis->setLabel(pivotModel()->vheader());
  else
    xAxis->setLabel(pivotModel()->hheader());

  if (plotType() != PlotType::GRID) {
    if (valueType() != ValueType::COUNT) {
      yAxis->setValueType     (CQChartsAxisValueType::Type::REAL, /*notify*/false);
      yAxis->setTicksDisplayed(CQChartsAxis::TicksDisplayed::MAJOR_AND_MINOR);

      if      (valueType() == ValueType::SUM)
        yAxis->setLabel("Sum");
      else if (valueType() == ValueType::MIN)
        yAxis->setLabel("Minimum");
      else if (valueType() == ValueType::MAX)
        yAxis->setLabel("Maximum");
      else if (valueType() == ValueType::AVERAGE)
        yAxis->setLabel("Average");
    }
    else {
      yAxis->setValueType     (CQChartsAxisValueType::Type::INTEGER, /*notify*/false);
      yAxis->setTicksDisplayed(CQChartsAxis::TicksDisplayed::MAJOR);

      yAxis->setLabel("Count");
    }
  }
  else {
    yAxis->clearTickLabels();

    yAxis->setValueType     (CQChartsAxisValueType::Type::INTEGER, /*notify*/false);
    yAxis->setGridMid       (true);
    xAxis->setTicksDisplayed(CQChartsAxis::TicksDisplayed::MAJOR);

    yAxis->setLabel(pivotModel()->hheader());
  }

  //---

  QStringList hkeys = pivotModel()->hkeys(isXSorted());
  QStringList vkeys = pivotModel()->vkeys(isYSorted());

  int nh = hkeys.length();
  int nv = vkeys.length();

  //---

  // row for vertical keys, (totals if included)
  // column for vertical header, horizontal key, totals (if include)

  using ColHeights    = std::map<int,double>;
  using RowColHeights = std::map<int,ColHeights>;

  RowColHeights rowColHeights; // cumulative heights

  if (plotType() == PlotType::STACKED_BAR) {
    for (int iv = 0; iv < nv; ++iv) {
      int r = pivotModel()->vkeyRow(vkeys[iv]);

      for (int ih = 0; ih < nh; ++ih) {
        int c = pivotModel()->hkeyCol(hkeys[ih]);

        int c1 = c + 1;

        QModelIndex ind = pivotModel()->index(r, c1, QModelIndex());

        QVariant var = pivotModel()->data(ind, Qt::EditRole);

        bool ok;

        double value = var.toDouble(&ok);
        if (! ok) continue;

        rowColHeights[iv][ih] = value;
      }

      for (int ih = 1; ih < nh; ++ih) {
        rowColHeights[iv][ih] += rowColHeights[iv][ih - 1];
      }
    }
  }

  //---

  if      (plotType() == PlotType::BAR || plotType() == PlotType::STACKED_BAR) {
    for (int iv = 0; iv < nv; ++iv) {
      const QString &vkey = vkeys[iv];

      int r = pivotModel()->vkeyRow(vkey);

      ColorInd ir(iv, nv);

      for (int ih = 0; ih < nh; ++ih) {
        const QString &hkey = hkeys[ih];

        int c = pivotModel()->hkeyCol(hkey);

        int c1 = c + 1;

        QModelIndex ind = pivotModel()->index(r, c1, QModelIndex());

        QVariant var = pivotModel()->data(ind, Qt::EditRole);

        bool ok;

        double value = var.toDouble(&ok);
        if (! ok) continue;

        CQPivotModel::Inds inds;

        (void) pivotModel()->modelInds(hkey, vkey, inds);

        ColorInd ic(ih, nh);

        CQChartsPlotObj *obj = nullptr;

        // bar side by side
        if      (plotType() == PlotType::BAR) {
          double x1 = (hasYValues ? iv - 0.5 : ih - 0.5);
          double x2 = (hasYValues ? iv + 0.5 : ih + 0.5);
          double dx = (hasYValues ? (x2 - x1)/nh : 1.0);

          CQChartsGeom::BBox rect;

          if (hasYValues)
            rect = CQChartsGeom::makeDirBBox(isHorizontal(),
                     x1 + ih*dx, 0.0, x1 + (ih + 1)*dx, value);
          else
            rect = CQChartsGeom::makeDirBBox(isHorizontal(),
                     x1, 0.0, x1 + dx, value);

          obj = new CQChartsPivotBarObj(this, rect, ind, inds, ir, ic, value);
        }
        // bar stacked
        else if (plotType() == PlotType::STACKED_BAR) {
          double oldValue = rowColHeights[iv][ih - 1];
          double newValue = oldValue + value;

          assert(CMathUtil::realEq(newValue, rowColHeights[iv][ih]));

          CQChartsGeom::BBox rect;

          if (hasYValues)
            rect = CQChartsGeom::makeDirBBox(isHorizontal(),
                     iv - 0.5, oldValue, iv + 0.5, newValue);
          else
            rect = CQChartsGeom::makeDirBBox(isHorizontal(),
                     ih - 0.5, oldValue, ih + 0.5, newValue);

          obj = new CQChartsPivotBarObj(this, rect, ind, inds, ir, ic, value);
        }

        if (obj)
          objs.push_back(obj);
      }
    }
  }
  else if (plotType() == PlotType::LINES || plotType() == PlotType::AREA) {
    bool isFilled = (plotType() == CQChartsPivotPlot::PlotType::AREA);

    using ModelIndices = std::vector<QModelIndex>;

    for (int ih = 0; ih < nh; ++ih) {
      int c = pivotModel()->hkeyCol(hkeys[ih]);

      int c1 = c + 1;

      QPolygonF    polygon;
      ModelIndices inds;
      double       minValue { 0.0 };
      double       maxValue { 0.0 };
      int          lastR    { -1 };

      for (int iv = 0; iv < nv; ++iv) {
        int r = pivotModel()->vkeyRow(vkeys[iv]);

        QModelIndex ind = pivotModel()->index(r, c1, QModelIndex());

        QVariant var = pivotModel()->data(ind, Qt::EditRole);

        bool ok;

        double value = var.toDouble(&ok);
        if (! ok) continue;

        minValue = (! inds.empty() ? std::min(minValue, value) : value);
        maxValue = (! inds.empty() ? std::max(maxValue, value) : value);

        if (isFilled && polygon.size() == 0) {
          QPointF p;

          if (hasYValues)
            p = (! isHorizontal() ? QPointF(iv, 0.0) : QPointF(0.0, iv));
          else
            p = (! isHorizontal() ? QPointF(ih, 0.0) : QPointF(0.0, ih));

          polygon.push_back(p);
        }

        QPointF p;

        if (hasYValues)
          p = (! isHorizontal() ? QPointF(iv, value) : QPointF(value, iv));
        else
          p = (! isHorizontal() ? QPointF(ih, value) : QPointF(value, ih));

        polygon.push_back(p);

        inds.push_back(ind);

        lastR = (hasYValues ? iv : ih);
      }

      if (isFilled && lastR >= 0) {
        QPointF p = (! isHorizontal() ? QPointF(lastR, 0.0) : QPointF(0.0, lastR));

        polygon.push_back(p);
      }

      //---

      QString name = pivotModel()->headerData(c1, Qt::Horizontal).toString();

      ColorInd ic(ih, nh);

      CQChartsGeom::BBox rect;

      if (hasYValues)
        rect = CQChartsGeom::makeDirBBox(isHorizontal(), 0.0, minValue, nv, maxValue);
      else
        rect = CQChartsGeom::makeDirBBox(isHorizontal(), 0.0, minValue, nh, maxValue);

      CQChartsPivotLineObj *obj = new CQChartsPivotLineObj(this, rect, inds, ic, polygon, name);

      objs.push_back(obj);
    }
  }
  else if (plotType() == PlotType::POINTS) {
    double ss = 5.0;

    double sx = pixelToWindowWidth (ss);
    double sy = pixelToWindowHeight(ss);

    for (int iv = 0; iv < nv; ++iv) {
      int r = pivotModel()->vkeyRow(vkeys[iv]);

      ColorInd ir(iv, nv);

      for (int ih = 0; ih < nh; ++ih) {
        int c = pivotModel()->hkeyCol(hkeys[ih]);

        int c1 = c + 1;

        QModelIndex ind = pivotModel()->index(r, c1, QModelIndex());

        QVariant var = pivotModel()->data(ind, Qt::EditRole);

        bool ok;

        double value = var.toDouble(&ok);
        if (! ok) continue;

        QPointF p;

        if (hasYValues)
          p = (! isHorizontal() ? QPointF(iv, value) : QPointF(value, iv));
        else
          p = (! isHorizontal() ? QPointF(ih, value) : QPointF(value, ih));

        //---

        ColorInd ic(ih, nh);

        CQChartsGeom::BBox rect(p.x() - sx, p.y() - sy, p.x() + sx, p.y() + sy);

        CQChartsPivotPointObj *obj =
          new CQChartsPivotPointObj(this, rect, ind, ir, ic, p, value);

        objs.push_back(obj);
      }
    }
  }
  else if (plotType() == PlotType::GRID) {
    for (int ih = 0; ih < nh; ++ih) {
      int c = pivotModel()->hkeyCol(hkeys[ih]);

      int c1 = c + 1;

      double hmin = 0.0; // pivotModel()->hmin(c);
      double hmax = pivotModel()->hmax(c);

      for (int iv = 0; iv < nv; ++iv) {
        int r = pivotModel()->vkeyRow(vkeys[iv]);

        QModelIndex ind = pivotModel()->index(r, c1, QModelIndex());

        QVariant var = pivotModel()->data(ind, Qt::EditRole);

        bool ok;

        double value = var.toDouble(&ok);
        if (! ok) value = 0.0;

        //---

        double vmin = 0.0; // pivotModel()->vmin(r);
        double vmax = pivotModel()->vmax(r);

        double hnorm = (hmax > hmin ? (value - hmin)/(hmax - hmin) : 0.0);
        double vnorm = (vmax > hmin ? (value - vmin)/(vmax - vmin) : 0.0);

        //---

        QString name = pivotModel()->headerData(c1, Qt::Horizontal).toString();

        ColorInd ir(iv, nv);
        ColorInd ic(ih, nh);

        CQChartsGeom::BBox rect = CQChartsGeom::makeDirBBox(isHorizontal(),
          iv - 0.5, ih - 0.5, iv + 0.5, ih + 0.5);

        CQChartsPivotCellObj *obj =
          new CQChartsPivotCellObj(this, rect, ind, ir, ic, name, value, hnorm, vnorm, ok);

        objs.push_back(obj);
      }
    }
  }
  else {
    assert(false);
  }

  //---

  if (plotType() != PlotType::GRID) {
    if (hasYValues) {
      for (int iv = 0; iv < nv; ++iv) {
        int r = pivotModel()->vkeyRow(vkeys[iv]);

        QModelIndex ind = pivotModel()->index(r, 0, QModelIndex());

        QString name = pivotModel()->data(ind, Qt::EditRole).toString();

        xAxis->setTickLabel(iv, name);
      }
    }
    else {
      for (int ih = 0; ih < nh; ++ih) {
        int c = pivotModel()->hkeyCol(hkeys[ih]);

        QString name = pivotModel()->headerData(c + 1, Qt::Horizontal).toString();

        xAxis->setTickLabel(ih, name);
      }
    }
  }
  else {
    for (int iv = 0; iv < nv; ++iv) {
      int r = pivotModel()->vkeyRow(vkeys[iv]);

      QModelIndex ind = pivotModel()->index(r, 0, QModelIndex());

      QString name = pivotModel()->data(ind, Qt::EditRole).toString();

      xAxis->setTickLabel(iv, name);
    }

    for (int ih = 0; ih < nh; ++ih) {
      int c = pivotModel()->hkeyCol(hkeys[ih]);

      QString name = pivotModel()->headerData(c + 1, Qt::Horizontal).toString();

      yAxis->setTickLabel(ih, name);
    }
  }

  //---

  return true;
}

CQChartsAxis *
CQChartsPivotPlot::
mappedXAxis() const
{
  return (! isHorizontal() ? xAxis() : yAxis());
}

CQChartsAxis *
CQChartsPivotPlot::
mappedYAxis() const
{
  return (! isHorizontal() ? yAxis() : xAxis());
}

void
CQChartsPivotPlot::
addKeyItems(CQChartsPlotKey *key)
{
  bool hasYValues = yColumns().isValid(); // has grouping y columns key

  if (hasYValues) {
    key->setHeaderStr(pivotModel()->hheader());

    //---

    int row = 0;

    auto addKeyRow = [&](const ColorInd &ic, const QString &name) {
      CQChartsPivotKeyColor *keyColor = new CQChartsPivotKeyColor(this, ic);
      CQChartsPivotKeyText  *keyText  = new CQChartsPivotKeyText (this, name);

      key->addItem(keyColor, row, 0);
      key->addItem(keyText , row, 1);

      ++row;
    };

    //---

    QStringList hkeys = pivotModel()->hkeys(isXSorted());
  //QStringList vkeys = pivotModel()->vkeys(isYSorted());

    int nh = hkeys.length();
  //int nv = vkeys.length();

    for (int ih = 0; ih < nh; ++ih) {
      int c = pivotModel()->hkeyCol(hkeys[ih]);

      int c1 = c + 1;

      ColorInd ic(ih, nh);

      QString name = pivotModel()->headerData(c1, Qt::Horizontal).toString();

      addKeyRow(ic, name);
    }
  }

  //---

  //key->plot()->updateKeyPosition(/*force*/true);
}

//------

bool
CQChartsPivotPlot::
addMenuItems(QMenu *menu)
{
  auto addMenuCheckedAction = [&](QMenu *menu, const QString &name,
                                  bool isSet, const char *slot) -> QAction *{
    QAction *action = new QAction(name, menu);

    action->setCheckable(true);
    action->setChecked(isSet);

    connect(action, SIGNAL(triggered(bool)), this, slot);

    menu->addAction(action);

    return action;
  };

  auto addCheckedAction = [&](const QString &name, bool isSet, const char *slot) -> QAction *{
    return addMenuCheckedAction(menu, name, isSet, slot);
  };

  //---

  menu->addSeparator();

  (void) addCheckedAction("Horizontal", isHorizontal(), SLOT(setHorizontal(bool)));

  QMenu *typeMenu = new QMenu("Plot Type");

  for (const auto &plotType : plotTypes())
    (void) addMenuCheckedAction(typeMenu, plotTypeName(plotType), this->plotType() == plotType,
                                SLOT(setPlotTypeSlot(bool)));

  menu->addMenu(typeMenu);

  QMenu *valueMenu = new QMenu("Value Type");

  for (const auto &valueType : valueTypes())
    (void) addMenuCheckedAction(valueMenu, valueTypeName(valueType), this->valueType() == valueType,
                                SLOT(setValueTypeSlot(bool)));

  menu->addMenu(valueMenu);

  return true;
}

//------

void
CQChartsPivotPlot::
postObjTree()
{
  updateKeyPosition(/*force*/true);

  invalidateLayers();
}

//------

void
CQChartsPivotPlot::
setPlotTypeSlot(bool b)
{
  if (! b) return;

  QAction *action = qobject_cast<QAction *>(sender());
  if (! action) return;

  QString name = action->text();

  for (const auto &plotType : plotTypes()) {
    if (plotTypeName(plotType) == name) {
      setPlotType(plotType);
      return;
    }
  }
}

void
CQChartsPivotPlot::
setValueTypeSlot(bool b)
{
  if (! b) return;

  QAction *action = qobject_cast<QAction *>(sender());
  if (! action) return;

  QString name = action->text();

  for (const auto &valueType : valueTypes()) {
    if (valueTypeName(valueType) == name) {
      setValueType(valueType);
      return;
    }
  }
}

QString
CQChartsPivotPlot::
plotTypeName(const PlotType &plotType) const
{
  switch (plotType) {
    case PlotType::BAR        : return "Bar";
    case PlotType::STACKED_BAR: return "Stacked Bar";
    case PlotType::LINES      : return "Lines";
    case PlotType::AREA       : return "Area";
    case PlotType::POINTS     : return "Points";
    case PlotType::GRID       : return "Grid";
    default                   : assert(false); return "";
  };
}

QString
CQChartsPivotPlot::
valueTypeName(const ValueType &valueType) const
{
  switch (valueType) {
    case ValueType::COUNT       : return "Count";
    case ValueType::COUNT_UNIQUE: return "Count Unique";
    case ValueType::SUM         : return "Sum";
    case ValueType::AVERAGE     : return "Average";
    case ValueType::MIN         : return "Min";
    case ValueType::MAX         : return "Max";
    default                     : assert(false); return "";
  };
}

//---

void
CQChartsPivotPlot::
write(std::ostream &os, const QString &varName, const QString &modelName) const
{
  CQChartsPlot::write(os, varName, modelName);

  dataLabel_->write(os, varName);
}

//------

CQChartsPivotBarObj::
CQChartsPivotBarObj(const CQChartsPivotPlot *plot, const CQChartsGeom::BBox &rect,
                    const QModelIndex &ind, const ModelIndices &inds, const ColorInd &ir,
                    const ColorInd &ic, double value) :
 CQChartsPlotObj(const_cast<CQChartsPivotPlot *>(plot), rect, ColorInd(), ir, ic),
 plot_(plot), value_(value), ind_(ind)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInds(inds);
}

//---

QString
CQChartsPivotBarObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(modelInd().row()).arg(modelInd().column());
}

QString
CQChartsPivotBarObj::
calcTipId() const
{
  int ic = ind_.column();
  int ir = ind_.row();

  CQChartsTableTip tableTip;

  QString valueName = plot()->columnHeaderName(plot_->valueColumn());
  QString vkeyValue = plot_->pivotModel()->headerData(ic, Qt::Horizontal).toString();
  QString hkeyValue = plot_->pivotModel()->data(plot_->pivotModel()->index(ir, 0)).toString();

  if (plot_->valueColumn().isValid())
    tableTip.addTableRow(valueName, QString("%1").arg(value()));

  if (plot_->xColumns().isValid())
    tableTip.addTableRow(plot_->pivotModel()->hheader(), vkeyValue);

  if (plot_->yColumns().isValid())
    tableTip.addTableRow(plot_->pivotModel()->vheader(), hkeyValue);

  plot()->addTipColumns(tableTip, modelInd());

  return tableTip.str();
}

//---

CQChartsGeom::BBox
CQChartsPivotBarObj::
dataLabelRect() const
{
  if (! plot_->dataLabel()->isVisible())
    return CQChartsGeom::BBox();

  CQChartsGeom::BBox prect = plot_->windowToPixel(rect());

  QString label = QString("%1").arg(value());

  return plot_->dataLabel()->calcRect(prect.qrect(), label);
}

//---

void
CQChartsPivotBarObj::
getSelectIndices(Indices &inds) const
{
  for (const auto &c : plot_->xColumns())
    addColumnSelectIndex(inds, c);

  for (const auto &c : plot_->yColumns())
    addColumnSelectIndex(inds, c);
}

//---

void
CQChartsPivotBarObj::
draw(CQChartsPaintDevice *device)
{
  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = (device->type() != CQChartsPaintDevice::Type::SCRIPT);

  calcPenBrush(penBrush, updateState);

  //---

  // draw bar
  drawRoundedPolygon(device, penBrush, rect(), plot_->barCornerSize());
}

void
CQChartsPivotBarObj::
drawFg(CQChartsPaintDevice *device) const
{
  // draw data label on foreground layers
  if (! plot_->dataLabel()->isVisible())
    return;

  QString label = QString("%1").arg(value());

  if (label != "") {
    CQChartsDataLabel::Position pos = plot_->dataLabel()->position();

    plot_->dataLabel()->draw(device, rect().qrect(), label, pos);
  }
}

void
CQChartsPivotBarObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // calc pen and brush
  ColorInd colorInd = calcColorInd();

  QColor bc = plot_->interpBarStrokeColor(colorInd);
  QColor fc = plot_->interpBarFillColor  (colorInd);

  plot_->setPenBrush(penBrush.pen, penBrush.brush,
    plot_->isBarStroked(), bc, plot_->barStrokeAlpha(), plot_->barStrokeWidth(),
    plot_->barStrokeDash(),
    plot_->isBarFilled(), fc, plot_->barFillAlpha(), plot_->barFillPattern());

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush.pen, penBrush.brush);
}

void
CQChartsPivotBarObj::
writeScriptData(std::ostream &os) const
{
  CQChartsPlotObj::writeScriptData(os);

  CQChartsPenBrush penBrush;

  calcPenBrush(penBrush, /*updateState*/ false);

  os << "  this.strokeColor = \"";

  if (penBrush.pen.style() == Qt::NoPen)
    os <<  "#00000000";
  else
    os << CQChartsUtil::encodeScriptColor(penBrush.pen.color()).toStdString();

  os << "\";\n";

  os << "  this.fillColor = \"";

  if (penBrush.brush.style() == Qt::NoBrush)
    os <<  "#00000000";
  else
    os << CQChartsUtil::encodeScriptColor(penBrush.brush.color()).toStdString();

  os << "\";\n";

  os << "  this.value = " << value() << "\n";
}

//------

CQChartsPivotLineObj::
CQChartsPivotLineObj(const CQChartsPivotPlot *plot, const CQChartsGeom::BBox &rect,
                     const ModelIndices &inds, const ColorInd &ic, const QPolygonF &polygon,
                     const QString &name) :
 CQChartsPlotObj(const_cast<CQChartsPivotPlot *>(plot), rect, ColorInd(), ic, ColorInd()),
 plot_(plot), polygon_(polygon), name_(name)
{
  setModelInds(inds);
}

QString
CQChartsPivotLineObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(ig_.i);
}

QString
CQChartsPivotLineObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", name_);

  //plot()->addTipColumns(tableTip, modelInd());

  return tableTip.str();
}

bool
CQChartsPivotLineObj::
inside(const CQChartsGeom::Point &p) const
{
  bool isFilled = (plot_->plotType() == CQChartsPivotPlot::PlotType::AREA);
  bool isLines  = (plot_->plotType() == CQChartsPivotPlot::PlotType::LINES ||
                   plot_->plotType() == CQChartsPivotPlot::PlotType::AREA);
  bool isPoints = (plot_->plotType() == CQChartsPivotPlot::PlotType::LINES);

  CQChartsGeom::Point pp = plot()->windowToPixel(p);

  if      (isFilled) {
    return polygon_.containsPoint(p.qpoint(), Qt::OddEvenFill);
  }
  else if (isLines) {
    int np = polygon_.count();

    for (int i = 1; i < np; ++i) {
      CQChartsGeom::Point p1(polygon_[i - 1]);
      CQChartsGeom::Point p2(polygon_[i    ]);

      CQChartsGeom::Point pl1 = plot()->windowToPixel(p1);
      CQChartsGeom::Point pl2 = plot()->windowToPixel(p2);

      double d;

      if (CQChartsUtil::PointLineDistance(pp, pl1, pl2, &d) && d <= 2)
        return true;
    }
  }
  else if (isPoints) {
    int np = polygon_.count();

    for (int i = 0; i < np; ++i) {
      CQChartsGeom::Point p1(polygon_[i]);

      CQChartsGeom::Point pl1 = plot()->windowToPixel(p1);

      if (CQChartsUtil::PointPointDistance(p, pl1) < 4)
        return true;
    }
  }
  else {
    assert(false);
  }

  return false;
}

void
CQChartsPivotLineObj::
getSelectIndices(Indices &inds) const
{
  for (const auto &c : plot_->xColumns())
    addColumnSelectIndex(inds, c);

  for (const auto &c : plot_->yColumns())
    addColumnSelectIndex(inds, c);
}

void
CQChartsPivotLineObj::
draw(CQChartsPaintDevice *device)
{
  // calc bar color
  ColorInd colorInd = calcColorInd();

  //---

  bool isFilled = (plot_->plotType() == CQChartsPivotPlot::PlotType::AREA);
  bool isLines  = (plot_->plotType() == CQChartsPivotPlot::PlotType::LINES ||
                   plot_->plotType() == CQChartsPivotPlot::PlotType::AREA);
  bool isPoints = (plot_->plotType() == CQChartsPivotPlot::PlotType::LINES);

  int np = polygon_.count();

  //---

  // draw line
  if (isLines) {
    // calc pen and brush
    QPen   pen;
    QBrush brush;

    QColor lc = plot_->interpBarFillColor(colorInd);

    plot_->setPenBrush(pen, brush,
      true , lc, 1.0, 0.0, CQChartsLineDash(),
      false, lc, 1.0, CQChartsFillPattern());

    plot_->updateObjPenBrushState(this, pen, brush);

    device->setPen(pen);
    device->setBrush(brush);

    //---

    // draw line
    if (! isFilled) {
      for (int i = 1; i < np; ++i)
        device->drawLine(polygon_[i - 1], polygon_[i]);
    }
    else {
      for (int i = 2; i < np - 1; ++i)
        device->drawLine(polygon_[i - 1], polygon_[i]);
    }
  }

  // draw points (symbols)
  if (isPoints) {
    // calc pen and brush
    QPen   pen;
    QBrush brush;

    QColor bc = plot_->interpBarStrokeColor(colorInd);
    QColor fc = plot_->interpBarFillColor  (colorInd);

    plot_->setPenBrush(pen, brush,
      plot_->isBarStroked(), bc, plot_->barStrokeAlpha(), plot_->barStrokeWidth(),
      plot_->barStrokeDash(),
      plot_->isBarFilled(), fc, plot_->barFillAlpha(), plot_->barFillPattern());

    plot_->updateObjPenBrushState(this, pen, brush);

    device->setPen(pen);
    device->setBrush(brush);

    CQChartsSymbol symbol(CQChartsSymbol::Type::CIRCLE);

    double ss = 5.0;

    for (int i = 0; i < np; ++i)
      plot()->drawSymbol(device, polygon_[i], symbol, ss, pen, brush);
  }

  // fill area
  if (isFilled) {
    // calc pen and brush
    QPen   pen;
    QBrush brush;

    QColor fc = plot_->interpBarFillColor(colorInd);

    plot_->setPenBrush(pen, brush,
      false, fc, 1.0, 0.0, CQChartsLineDash(),
      true , fc, 0.5, CQChartsFillPattern());

    plot_->updateObjPenBrushState(this, pen, brush);

    device->setPen(pen);
    device->setBrush(brush);

    QPainterPath path;

    for (int i = 0; i < np; ++i) {
      if (i == 0)
        path.moveTo(polygon_[i]);
      else
        path.lineTo(polygon_[i]);
    }

    device->drawPath(path);
  }
}

//------

CQChartsPivotPointObj::
CQChartsPivotPointObj(const CQChartsPivotPlot *plot, const CQChartsGeom::BBox &rect,
                     const QModelIndex &ind, const ColorInd &ir, const ColorInd &ic,
                     const QPointF &p, double value) :
 CQChartsPlotObj(const_cast<CQChartsPivotPlot *>(plot), rect, ColorInd(), ic, ir),
 plot_(plot), p_(p), value_(value)
{
  setModelInd(ind);
}

QString
CQChartsPivotPointObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(ig_.i);
}

QString
CQChartsPivotPointObj::
calcTipId() const
{
  int ic = modelInd().column();
  int ir = modelInd().row();

  CQChartsTableTip tableTip;

  QString valueName = plot()->columnHeaderName(plot_->valueColumn());
  QString vkeyValue = plot_->pivotModel()->headerData(ic, Qt::Horizontal).toString();
  QString hkeyValue = plot_->pivotModel()->data(plot_->pivotModel()->index(ir, 0)).toString();

  if (plot_->valueColumn().isValid())
    tableTip.addTableRow(valueName, QString("%1").arg(value()));

  if (plot_->xColumns().isValid())
    tableTip.addTableRow(plot_->pivotModel()->hheader(), vkeyValue);

  if (plot_->yColumns().isValid())
    tableTip.addTableRow(plot_->pivotModel()->vheader(), hkeyValue);

  plot()->addTipColumns(tableTip, modelInd());

  return tableTip.str();
}

bool
CQChartsPivotPointObj::
inside(const CQChartsGeom::Point &p) const
{
  CQChartsGeom::Point p1 = plot()->windowToPixel(p);
  CQChartsGeom::Point p2 = plot()->windowToPixel(CQChartsGeom::Point(p_));

  return (CQChartsUtil::PointPointDistance(p1, p2) < 4);
}

void
CQChartsPivotPointObj::
getSelectIndices(Indices &inds) const
{
  for (const auto &c : plot_->xColumns())
    addColumnSelectIndex(inds, c);

  for (const auto &c : plot_->yColumns())
    addColumnSelectIndex(inds, c);
}

void
CQChartsPivotPointObj::
draw(CQChartsPaintDevice *device)
{
  // calc bar color
  ColorInd colorInd = calcColorInd();

  //---

  // draw points (symbols)

  // calc pen and brush
  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpBarStrokeColor(colorInd);
  QColor fc = plot_->interpBarFillColor  (colorInd);

  plot_->setPenBrush(pen, brush,
    plot_->isBarStroked(), bc, plot_->barStrokeAlpha(), plot_->barStrokeWidth(),
    plot_->barStrokeDash(),
    plot_->isBarFilled(), fc, plot_->barFillAlpha(), plot_->barFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  device->setPen(pen);
  device->setBrush(brush);

  CQChartsSymbol symbol(CQChartsSymbol::Type::CIRCLE);

  double ss = 5.0;

  plot()->drawSymbol(device, p_, symbol, ss, pen, brush);
}

//------

CQChartsPivotCellObj::
CQChartsPivotCellObj(const CQChartsPivotPlot *plot, const CQChartsGeom::BBox &rect,
                     const QModelIndex &ind, const ColorInd &ir, const ColorInd &ic,
                     const QString &name, double value, double hnorm, double vnorm,
                     bool valid) :
 CQChartsPlotObj(const_cast<CQChartsPivotPlot *>(plot), rect, ColorInd(), ic, ir),
 plot_(plot), name_(name), value_(value), hnorm_(hnorm), vnorm_(vnorm), valid_(valid)
{
  setModelInd(ind);

  // get column palette and bg color
  CQChartsModelColumnDetails *columnDetails = plot_->columnDetails(modelInd().column());

  color_ = columnDetails->tableDrawColor();

  if (! color_.isValid())
    color_ = CQChartsColor(CQChartsColor::Type::PALETTE);
}

QString
CQChartsPivotCellObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(ig_.i).arg(iv_.i);
}

QString
CQChartsPivotCellObj::
calcTipId() const
{
  int ic = modelInd().column();
  int ir = modelInd().row();

  CQChartsTableTip tableTip;

  QString valueName = plot()->columnHeaderName(plot_->valueColumn());
  QString vkeyValue = plot_->pivotModel()->headerData(ic, Qt::Horizontal).toString();
  QString hkeyValue = plot_->pivotModel()->data(plot_->pivotModel()->index(ir, 0)).toString();

  if (plot_->valueColumn().isValid())
    tableTip.addTableRow(valueName, QString("%1").arg(value()));

  if (plot_->xColumns().isValid())
    tableTip.addTableRow(plot_->pivotModel()->hheader(), vkeyValue);

  if (plot_->yColumns().isValid())
    tableTip.addTableRow(plot_->pivotModel()->vheader(), hkeyValue);

  tableTip.addTableRow("Row %"   , 100*hnorm_);
  tableTip.addTableRow("Column %", 100*vnorm_);

  plot()->addTipColumns(tableTip, modelInd());

  return tableTip.str();
}

void
CQChartsPivotCellObj::
getSelectIndices(Indices &inds) const
{
  for (const auto &c : plot_->xColumns())
    addColumnSelectIndex(inds, c);

  for (const auto &c : plot_->yColumns())
    addColumnSelectIndex(inds, c);
}

void
CQChartsPivotCellObj::
draw(CQChartsPaintDevice *device)
{
  // get column palette and bg color
  QColor hbg, vbg;

  if (color_.isValid()) {
    QColor bg1 = plot_->charts()->interpColor(color_, 0, 1);
    QColor bg2 = plot_->interpPlotFillColor(ColorInd());

    hbg = CQChartsUtil::blendColors(bg1, bg2, hnorm_);
    vbg = CQChartsUtil::blendColors(bg1, bg2, vnorm_);
  }

  //---

  // get bar color
  ColorInd colorInd = calcColorInd();

  //---

  // calc bar box
  CQChartsGeom::BBox prect = plot_->windowToPixel(rect());

  QRectF qrect = prect.qrect();

  double m  = 4;
  double bs = std::min(std::min(qrect.width ()/2 - 2*m, qrect.height()/2 - 2*m), 32.0);
  double tw = qrect.width () - bs - 3*m;
  double th = qrect.height() - bs - 3*m;

  double bw = qrect.width () - bs - 3*m;
  double bh = qrect.height() - bs - 3*m;

  QRectF qrecth1(qrect.left () + m, qrect.bottom() - bs - m, bw*hnorm_, bs);
  QRectF qrecth2(qrect.left () + m, qrect.bottom() - bs - m, bw       , bs);

  QRectF qrectv1(qrect.right() - m - bs, qrect.top() + m, bs, bh*vnorm_);
  QRectF qrectv2(qrect.right() - m - bs, qrect.top() + m, bs, bh       );

  // calc text box
  QRectF qrectt(qrect.left() + m, qrect.top() + m, tw, th);

  //---

  // calc background pen and brush and draw
  QPen   bgpen;
  QBrush bgbrush;

  QColor bgsc = plot_->interpBarStrokeColor(colorInd);
  QColor bgfc = plot_->interpBarFillColor(colorInd);

  plot_->setPenBrush(bgpen, bgbrush,
    plot_->isBarStroked(), bgsc, plot_->barStrokeAlpha(), plot_->barStrokeWidth(),
    plot_->barStrokeDash(),
    plot_->isBarFilled(), bgfc, plot_->barFillAlpha(), plot_->barFillPattern());

  plot_->updateObjPenBrushState(this, bgpen, bgbrush);

  if (! valid_)
    bgbrush = Qt::NoBrush;

  device->setPen(bgpen);
  device->setBrush(bgbrush);

  device->drawRect(device->pixelToWindow(qrect));

  //---

  // draw value
  // TODO: honor label visible ?
  if (valid_) {
    QString valueStr = CQChartsUtil::formatReal(value());

    //---

    CQChartsTextOptions textOptions;

    textOptions.angle     = 0.0;
    textOptions.contrast  = plot_->dataLabel()->isTextContrast();
    textOptions.formatted = false;
    textOptions.scaled    = plot_->dataLabel()->isTextScaled();
    textOptions.html      = false;
    textOptions.clipped   = false;
    textOptions.margin    = 0;
    textOptions.align     = Qt::AlignHCenter | Qt::AlignVCenter;

    textOptions = plot_->adjustTextOptions(textOptions);

    device->setPen(CQChartsUtil::bwColor(vbg));

    plot_->view()->setPlotPainterFont(plot_, device, plot_->dataLabel()->textFont());

    QRectF tr;

    if (plot_->isGridBars())
      tr = device->pixelToWindow(qrectt);
    else
      tr = device->pixelToWindow(qrect);

    CQChartsDrawUtil::drawTextInBox(device, tr, valueStr, textOptions);
  }

  //---

  // calc bar pen and brush and draw
  if (valid_ && plot_->isGridBars()) {
    QPen   fgpen;
    QBrush fgbrush;

    QColor fgsc = plot_->interpBarStrokeColor(colorInd);

    plot_->setPen(fgpen,
      plot_->isBarStroked(), fgsc, plot_->barStrokeAlpha(), plot_->barStrokeWidth(),
      plot_->barStrokeDash());

    device->setPen(fgpen);
    device->setBrush(plot_->interpPlotFillColor(ColorInd()));

    device->drawRect(device->pixelToWindow(qrecth2));

    device->setPen(Qt::NoPen);
    device->setBrush(hbg);

    device->drawRect(device->pixelToWindow(qrecth1));

    //---

    device->setPen(fgpen);
    device->setBrush(plot_->interpPlotFillColor(ColorInd()));

    device->drawRect(device->pixelToWindow(qrectv2));

    device->setPen(Qt::NoPen);
    device->setBrush(vbg);

    device->drawRect(device->pixelToWindow(qrectv1));
  }
}

//------

CQChartsPivotKeyColor::
CQChartsPivotKeyColor(CQChartsPivotPlot *plot, const ColorInd &ic) :
 CQChartsKeyColorBox(plot, ColorInd(), ic, ColorInd())
{
}

QBrush
CQChartsPivotKeyColor::
fillBrush() const
{
  CQChartsPivotPlot *plot = qobject_cast<CQChartsPivotPlot *>(this->plot());

  QColor fc = plot->interpBarFillColor(ig_);

  if (plot_->isSetHidden(ig_.i))
    fc = CQChartsUtil::blendColors(fc, key_->interpBgColor(), key_->hiddenAlpha());

  QBrush brush;

  plot->setBrush(brush, plot->isBarFilled(), fc, plot->barFillAlpha(), plot->barFillPattern());

  return brush;
}

//------

CQChartsPivotKeyText::
CQChartsPivotKeyText(CQChartsPivotPlot *plot, const QString &name) :
 CQChartsKeyText(plot, name, ColorInd())
{
}
