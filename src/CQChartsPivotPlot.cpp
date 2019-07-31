#include <CQChartsPivotPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsTip.h>
#include <CQChartsDataLabel.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsHtml.h>
#include <CQChartsRoundedPolygon.h>
#include <CQChartsModelDetails.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColors.h>
#include <CQColorsPalette.h>
#include <CQPerfMonitor.h>
#include <CQPivotModel.h>

#include <QPainter>
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
     p("The values can be summed, averages or counted to produced the "
       "displayed valued.").
     p("The plot can be a barchart (side by side ot stacked), a line chart "
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
  addProp("options", "plotType"  , "plotType"  , "Plot type"      );
  addProp("options", "valueType" , "valueType" , "Value type"     );
  addProp("options", "horizontal", "horizontal", "Draw horizontal");

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

  // row for vertical keys, (totals if included)
  // column for vertical header, horizontal key, totals (if include)
  int nr = pivotModel()->rowCount   ();
  int nc = pivotModel()->columnCount();

  if (plotType() == PlotType::STACKED_BAR) {
    for (int r = 0; r < nr; ++r) {
      double sum = 0.0;

      for (int c = 1; c < nc; ++c) {
        QModelIndex ind = pivotModel()->index(r, c, QModelIndex());

        QVariant var = pivotModel()->data(ind, Qt::EditRole);

        bool ok;

        double value = var.toDouble(&ok);
        if (! ok) continue;

        sum += value;
      }

      updateRange(r - 0.5, 0.0);
      updateRange(r + 0.5, sum);
    }
  }
  else {
    for (int r = 0; r < nr; ++r) {
      for (int c = 1; c < nc; ++c) {
        QModelIndex ind = pivotModel()->index(r, c, QModelIndex());

        QVariant var = pivotModel()->data(ind, Qt::EditRole);

        bool ok;

        double value = var.toDouble(&ok);
        if (! ok) continue;

        int c1 = c - 1;

        if      (plotType() == PlotType::GRID) {
          updateRange(r - 0.5, c1 - 0.5);
          updateRange(r + 0.5, c1 + 0.5);
        }
        else if (plotType() == PlotType::LINES || plotType() == PlotType::AREA) {
          updateRange(r, 0.0);
          updateRange(r, value);
        }
        else if (plotType() == PlotType::POINTS) {
          double ss = 5.0;

          double sx = pixelToWindowWidth (ss);
          double sy = pixelToWindowHeight(ss);

          updateRange(r - sx, value - sy);
          updateRange(r + sx, value + sy);
        }
        else {
          updateRange(r - 0.5, 0.0  );
          updateRange(r + 0.5, value);
        }
      }
    }
  }

  //---

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

  CQChartsPivotPlot *th = const_cast<CQChartsPivotPlot *>(this);

  NoUpdate noUpdate(th);

  //---

  CQChartsAxis *xAxis = mappedXAxis();
  CQChartsAxis *yAxis = mappedYAxis();

  //---

  xAxis->clearTickLabels();

  xAxis->setValueType     (CQChartsAxisValueType::Type::INTEGER, /*notify*/false);
  xAxis->setGridMid       (true);
  xAxis->setTicksDisplayed(CQChartsAxis::TicksDisplayed::MAJOR);

  xAxis->setLabel(pivotModel()->vheader());

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

  // row for vertical keys, (totals if included)
  // column for vertical header, horizontal key, totals (if include)
  int nr = pivotModel()->rowCount   ();
  int nc = pivotModel()->columnCount();

  using ColHeights    = std::map<int,double>;
  using RowColHeights = std::map<int,ColHeights>;

  RowColHeights rowColHeights;

  if (plotType() == PlotType::STACKED_BAR) {
    for (int r = 0; r < nr; ++r) {
      for (int c = 1; c < nc; ++c) {
        QModelIndex ind = pivotModel()->index(r, c, QModelIndex());

        QVariant var = pivotModel()->data(ind, Qt::EditRole);

        bool ok;

        double value = var.toDouble(&ok);
        if (! ok) continue;

        double prevValue = rowColHeights[r][c - 1];

        rowColHeights[r][c] = prevValue + value;
      }
    }
  }

  //---

  if      (plotType() == PlotType::BAR || plotType() == PlotType::STACKED_BAR) {
    for (int r = 0; r < nr; ++r) {
      ColorInd ir(r, nr);

      for (int c = 1; c < nc; ++c) {
        QModelIndex ind = pivotModel()->index(r, c, QModelIndex());

        QVariant var = pivotModel()->data(ind, Qt::EditRole);

        bool ok;

        double value = var.toDouble(&ok);
        if (! ok) continue;

        ColorInd ic(c - 1, nc - 1);

        CQChartsPlotObj *obj = nullptr;

        // bar side by side
        if      (plotType() == PlotType::BAR) {
          double x1 = r - 0.5;
          double x2 = r + 0.5;
          double dx = (x2 - x1)/(nc - 1);

          CQChartsGeom::BBox rect = CQChartsGeom::makeDirBBox(isHorizontal(),
            x1 + (c - 1)*dx, 0.0, x1 + c*dx, value);

          obj = new CQChartsPivotBarObj(this, rect, ind, ir, ic, value);
        }
        // bar stacked
        else if (plotType() == PlotType::STACKED_BAR) {
          double oldValue = rowColHeights[r][c - 1];

          CQChartsGeom::BBox rect = CQChartsGeom::makeDirBBox(isHorizontal(),
            r - 0.5, oldValue, r + 0.5, oldValue + value);

          obj = new CQChartsPivotBarObj(this, rect, ind, ir, ic, value);
        }

        if (obj)
          objs.push_back(obj);
      }
    }
  }
  else if (plotType() == PlotType::LINES || plotType() == PlotType::AREA) {
    bool isFilled = (plotType() == CQChartsPivotPlot::PlotType::AREA);

    using ModelIndices = std::vector<QModelIndex>;

    for (int c = 1; c < nc; ++c) {
      QPolygonF    polygon;
      ModelIndices inds;
      double       minValue { 0.0 };
      double       maxValue { 0.0 };
      int          lastR    { -1 };

      for (int r = 0; r < nr; ++r) {
        QModelIndex ind = pivotModel()->index(r, c, QModelIndex());

        QVariant var = pivotModel()->data(ind, Qt::EditRole);

        bool ok;

        double value = var.toDouble(&ok);
        if (! ok) continue;

        minValue = (! inds.empty() ? std::min(minValue, value) : value);
        maxValue = (! inds.empty() ? std::max(maxValue, value) : value);

        if (isFilled && polygon.length() == 0) {
          if (! isHorizontal())
            polygon.push_back(QPointF(r, 0.0));
          else
            polygon.push_back(QPointF(0.0, r));
        }

        if (! isHorizontal())
          polygon.push_back(QPointF(r, value));
        else
          polygon.push_back(QPointF(value, r));

        inds.push_back(ind);

        lastR = r;
      }

      if (isFilled && lastR >= 0) {
        if (! isHorizontal())
          polygon.push_back(QPointF(lastR, 0.0));
        else
          polygon.push_back(QPointF(0.0, lastR));
      }

      //---

      QString name = pivotModel()->headerData(c, Qt::Horizontal).toString();

      ColorInd ic(c - 1, nc - 1);

      CQChartsGeom::BBox rect = CQChartsGeom::makeDirBBox(isHorizontal(),
        0.0, minValue, nr, maxValue);

      CQChartsPivotLineObj *obj = new CQChartsPivotLineObj(this, rect, inds, ic, polygon, name);

      objs.push_back(obj);
    }
  }
  else if (plotType() == PlotType::POINTS) {
    double ss = 5.0;

    double sx = pixelToWindowWidth (ss);
    double sy = pixelToWindowHeight(ss);

    for (int r = 0; r < nr; ++r) {
      ColorInd ir(r, nr);

      for (int c = 1; c < nc; ++c) {
        QModelIndex ind = pivotModel()->index(r, c, QModelIndex());

        QVariant var = pivotModel()->data(ind, Qt::EditRole);

        bool ok;

        double value = var.toDouble(&ok);
        if (! ok) continue;

        QPointF p;

        if (! isHorizontal())
          p = QPointF(c - 1, value);
        else
          p = QPointF(value, c - 1);

        //---

        ColorInd ic(c - 1, nc - 1);

        CQChartsGeom::BBox rect(p.x() - sx, p.y() - sy, p.x() + sx, p.y() + sy);

        CQChartsPivotPointObj *obj =
          new CQChartsPivotPointObj(this, rect, ind, ir, ic, p, value);

        objs.push_back(obj);
      }
    }
  }
  else if (plotType() == PlotType::GRID) {
    for (int c = 1; c < nc; ++c) {
      int c1 = c - 1;

      double hmin = 0.0; // pivotModel()->hmin(c1);
      double hmax = pivotModel()->hmax(c1);

      for (int r = 0; r < nr; ++r) {
        QModelIndex ind = pivotModel()->index(r, c, QModelIndex());

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

        QString name = pivotModel()->headerData(c, Qt::Horizontal).toString();

        ColorInd ir(r , nr);
        ColorInd ic(c1, nc - 1);

        CQChartsGeom::BBox rect = CQChartsGeom::makeDirBBox(isHorizontal(),
          r - 0.5, c1 - 0.5, r + 0.5, c1 + 0.5);

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

  for (int r = 0; r < nr; ++r) {
    QModelIndex ind = pivotModel()->index(r, 0, QModelIndex());

    QString name = pivotModel()->data(ind, Qt::EditRole).toString();

    xAxis->setTickLabel(r, name);
  }

  if (plotType() == PlotType::GRID) {
    for (int c = 1; c < nc; ++c) {
      int c1 = c - 1;

      QString name = pivotModel()->headerData(c, Qt::Horizontal).toString();

      yAxis->setTickLabel(c1, name);
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

  int nc = pivotModel()->columnCount();

  for (int c = 1; c < nc; ++c) {
    ColorInd ic(c - 1, nc - 1);

    QString name = pivotModel()->headerData(c, Qt::Horizontal).toString();

    addKeyRow(ic, name);
  }

  //---

  key->plot()->updateKeyPosition(/*force*/true);
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
                    const QModelIndex &ind, const ColorInd &ir, const ColorInd &ic,
                    double value) :
 CQChartsPlotObj(const_cast<CQChartsPivotPlot *>(plot), rect, ColorInd(), ic, ir),
 plot_(plot), value_(value)
{
  setModelInd(ind);
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
  CQChartsTableTip tableTip;

  QString valueName = plot()->columnHeaderName(plot_->valueColumn());
  QString hkeyValue = plot_->pivotModel()->data(plot_->pivotModel()->index(iv_.i, 0)).toString();
  QString vkeyValue = plot_->pivotModel()->headerData(ig_.i + 1, Qt::Horizontal).toString();

  tableTip.addTableRow(valueName, QString("%1").arg(value_));
  tableTip.addTableRow(plot_->pivotModel()->hheader(), vkeyValue);
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

  QRectF qrect = CQChartsUtil::toQRect(prect);

  QString label = QString("%1").arg(value_);

  return plot_->dataLabel()->calcRect(qrect, label);
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
draw(QPainter *painter)
{
  if (! visible())
    return;

  //---

  CQChartsGeom::BBox prect = plot_->windowToPixel(rect());

  QRectF qrect = CQChartsUtil::toQRect(prect);

  //---

  // calc bar color
  ColorInd colorInd(ig_);

  //---

  // calc pen and brush
  QPen   pen;
  QBrush brush;

  QColor bc = plot_->interpBarStrokeColor(colorInd);
  QColor fc = plot_->interpBarFillColor(colorInd);

  plot_->setPenBrush(pen, brush,
    plot_->isBarStroked(), bc, plot_->barStrokeAlpha(), plot_->barStrokeWidth(),
    plot_->barStrokeDash(),
    plot_->isBarFilled(), fc, plot_->barFillAlpha(), plot_->barFillPattern());

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen(pen);
  painter->setBrush(brush);

  //---

  double cxs = plot_->lengthPixelWidth (plot_->barCornerSize());
  double cys = plot_->lengthPixelHeight(plot_->barCornerSize());

  CQChartsRoundedPolygon::draw(painter, qrect, cxs, cys);
}

void
CQChartsPivotBarObj::
drawFg(QPainter *painter) const
{
  // draw data label on foreground layers
  if (! plot_->dataLabel()->isVisible())
    return;

  CQChartsGeom::BBox prect = plot_->windowToPixel(rect());

  QRectF qrect = CQChartsUtil::toQRect(prect);

  //---

  QString label = QString("%1").arg(value_);

  CQChartsDataLabel::Position pos = plot_->dataLabel()->position();

  if (label != "")
    plot_->dataLabel()->draw(painter, qrect, label, pos);
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
  if (! visible())
    return false;

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
draw(QPainter *painter)
{
  if (! visible())
    return;

  //---

  // calc bar color
  ColorInd colorInd(ig_);

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

    painter->setPen(pen);
    painter->setBrush(brush);

    //---

    // draw line
    if (! isFilled) {
      for (int i = 1; i < np; ++i) {
        QPointF p1 = plot()->windowToPixel(polygon_[i - 1]);
        QPointF p2 = plot()->windowToPixel(polygon_[i    ]);

        painter->drawLine(p1, p2);
      }
    }
    else {
      for (int i = 2; i < np - 1; ++i) {
        QPointF p1 = plot()->windowToPixel(polygon_[i - 1]);
        QPointF p2 = plot()->windowToPixel(polygon_[i    ]);

        painter->drawLine(p1, p2);
      }
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

    painter->setPen(pen);
    painter->setBrush(brush);

    CQChartsSymbol symbol(CQChartsSymbol::Type::CIRCLE);

    double ss = 5.0;

    for (int i = 0; i < np; ++i) {
      QPointF p = plot()->windowToPixel(polygon_[i]);

      plot()->drawSymbol(painter, p, symbol, ss, pen, brush);
    }
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

    painter->setPen(pen);
    painter->setBrush(brush);

    QPainterPath path;

    for (int i = 0; i < np; ++i) {
      QPointF p = plot()->windowToPixel(polygon_[i]);

      if (i == 0)
        path.moveTo(p);
      else
        path.lineTo(p);
    }

    painter->drawPath(path);
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
  CQChartsTableTip tableTip;

  QString valueName = plot()->columnHeaderName(plot_->valueColumn());
  QString hkeyValue = plot_->pivotModel()->data(plot_->pivotModel()->index(iv_.i, 0)).toString();
  QString vkeyValue = plot_->pivotModel()->headerData(ig_.i + 1, Qt::Horizontal).toString();

  tableTip.addTableRow(valueName, QString("%1").arg(value_));
  tableTip.addTableRow(plot_->pivotModel()->hheader(), vkeyValue);
  tableTip.addTableRow(plot_->pivotModel()->vheader(), hkeyValue);

  plot()->addTipColumns(tableTip, modelInd());

  return tableTip.str();
}

bool
CQChartsPivotPointObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

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
draw(QPainter *painter)
{
  if (! visible())
    return;

  //---

  // calc bar color
  ColorInd colorInd(ig_);

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

  painter->setPen(pen);
  painter->setBrush(brush);

  CQChartsSymbol symbol(CQChartsSymbol::Type::CIRCLE);

  double ss = 5.0;

  QPointF p = plot()->windowToPixel(p_);

  plot()->drawSymbol(painter, p, symbol, ss, pen, brush);
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

  const CQChartsColumnType::ColorPalette &colorPalette = columnDetails->tableDrawPalette();

  palette_ = colorPalette.palette;
  color_   = colorPalette.color;

  if (! palette_ && ! color_.isValid())
    palette_ = CQColorsMgrInst->getNamedPalette("greens");
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
  CQChartsTableTip tableTip;

  QString valueName = plot()->columnHeaderName(plot_->valueColumn());
  QString hkeyValue = plot_->pivotModel()->data(plot_->pivotModel()->index(iv_.i, 0)).toString();
  QString vkeyValue = plot_->pivotModel()->headerData(ig_.i + 1, Qt::Horizontal).toString();

  tableTip.addTableRow(valueName, QString("%1").arg(value_));
  tableTip.addTableRow(plot_->pivotModel()->hheader(), vkeyValue);
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
draw(QPainter *painter)
{
  if (! visible())
    return;

  //---

  CQChartsGeom::BBox prect = plot_->windowToPixel(rect());

  QRectF qrect = CQChartsUtil::toQRect(prect);

  //---

  // get column palette and bg color
  QColor hbg, vbg;

  if      (palette_) {
    hbg = palette_->getColor(hnorm_);
    vbg = palette_->getColor(vnorm_);
  }
  else if (color_.isValid()) {
    QColor bg1 = plot_->charts()->interpColor(color_, 0, 1);
    QColor bg2 = plot_->interpPlotFillColor(ColorInd());

    hbg = CQChartsUtil::blendColors(bg1, bg2, hnorm_);
    vbg = CQChartsUtil::blendColors(bg1, bg2, vnorm_);
  }

  // get bar color
  ColorInd colorInd(ig_);

  //---

  // calc bar box
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

  painter->setPen(bgpen);
  painter->setBrush(bgbrush);

  painter->drawRect(qrect);

  //---

  // draw value
  if (valid_) {
    QString valueStr = CQChartsUtil::formatReal(value_);

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

    painter->setPen(CQChartsUtil::bwColor(vbg));

    plot_->view()->setPlotPainterFont(plot_, painter, plot_->dataLabel()->textFont());

    CQChartsDrawUtil::drawTextInBox(painter, qrectt, valueStr, textOptions);
  }

  //---

  // calc bar pen and brush and draw
  if (valid_) {
    QPen   fgpen;
    QBrush fgbrush;

    QColor fgsc = plot_->interpBarStrokeColor(colorInd);

    plot_->setPen(fgpen,
      plot_->isBarStroked(), fgsc, plot_->barStrokeAlpha(), plot_->barStrokeWidth(),
      plot_->barStrokeDash());

    painter->setPen(fgpen);
    painter->setBrush(plot_->interpPlotFillColor(ColorInd()));

    painter->drawRect(qrecth2);

    painter->setPen(Qt::NoPen);
    painter->setBrush(hbg);

    painter->drawRect(qrecth1);

    //---

    painter->setPen(fgpen);
    painter->setBrush(plot_->interpPlotFillColor(ColorInd()));

    painter->drawRect(qrectv2);

    painter->setPen(Qt::NoPen);
    painter->setBrush(vbg);

    painter->drawRect(qrectv1);
  }
}

//------

CQChartsPivotKeyColor::
CQChartsPivotKeyColor(CQChartsPivotPlot *plot, const ColorInd &ic) :
 CQChartsKeyColorBox(plot, ColorInd(), ic, ColorInd())
{
}

//------

CQChartsPivotKeyText::
CQChartsPivotKeyText(CQChartsPivotPlot *plot, const QString &name) :
 CQChartsKeyText(plot, name, ColorInd())
{
}
