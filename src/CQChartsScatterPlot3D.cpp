#include <CQChartsScatterPlot3D.h>
#include <CQChartsView.h>
#include <CQChartsKey.h>
#include <CQChartsTitle.h>
#include <CQChartsValueSet.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumnType.h>
#include <CQChartsDataLabel.h>
#include <CQChartsVariant.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQChartsDrawUtil.h>
#include <CQCharts.h>
#include <CQChartsCamera.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQColorsPalette.h>
#include <CQPerfMonitor.h>

#include <QMenu>

CQChartsScatterPlot3DType::
CQChartsScatterPlot3DType()
{
}

void
CQChartsScatterPlot3DType::
addParameters()
{
  startParameterGroup("Scatter");

  // columns
  addColumnParameter("x", "X", "xColumn").
    setRequired().setNumeric().setTip("X Value Column");
  addColumnParameter("y", "Y", "yColumn").
    setRequired().setNumeric().setTip("Y Value Column");
  addColumnParameter("z", "Z", "zColumn").
    setRequired().setNumeric().setTip("Z Value Column");

  addColumnParameter("name", "Name", "nameColumn").
    setString().setTip("Optional Name Column").setString();
  addColumnParameter("label", "Label", "labelColumn").
    setTip("Custom Label").setString();

  addColumnParameter("symbolType", "Symbol Type", "symbolTypeColumn").
    setTip("Custom Symbol Type").setMapped().
    setMapMinMax(CQChartsSymbol::minFillValue(), CQChartsSymbol::maxFillValue());

  addColumnParameter("symbolSize", "Symbol Size", "symbolSizeColumn").
    setTip("Custom Symbol Size").setMapped().
    setMapMinMax(CQChartsSymbolSize::minValue(), CQChartsSymbolSize::maxValue());

  addColumnParameter("fontSize", "Font Size", "fontSizeColumn").
    setTip("Custom Font Size for Label").setMapped().
    setMapMinMax(CQChartsFontSize::minValue(), CQChartsFontSize::maxValue());

  //--

  // options
  addBoolParameter("pointLabels", "Point Labels", "pointLabels").
    setTip("Show Label at Point").setPropPath("labels.visible");

  endParameterGroup();

  //---

  CQChartsPlot3DType::addParameters();
}

QString
CQChartsScatterPlot3DType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Scatter Plot").
    h3("Summary").
     p("Draws scatter plot of x, y, z points with support for grouping and customization of "
       "point symbol type, symbol size and symbol color.").
     p("The points can have individual labels in which case the label font size can "
       "also be customized.").
    h3("Grouping").
     p("The points can be grouped by specifying a " + B("Name") + " column, all values "
       "with the same name are placed in that group and will be default colored by the "
       "group index.").
    h3("Columns").
     p("The points are specified by the " + B("X") + ", " + B("Y") + " and " + B("Z") +
       " columns.").
     p("An optional " + B("SymbolType") + " column can be specified to supply the type of the "
       "symbol drawn at the point. An optional " + B("SymbolSize") + " column can be specified "
       "to supply the size of the symbol drawn at the point. An optional " + B("Color") + " "
       "column can be specified to supply the fill color of the symbol drawn at the point.").
     p("An optional point label can be specified using the " + B("Label") + " column or the " +
       B("Name") + " column. The font size of the label can be specified using the " +
       B("FontSize") + " column.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/scatterplot3d.png"));
}

CQChartsPlot *
CQChartsScatterPlot3DType::
create(View *view, const ModelP &model) const
{
  return new CQChartsScatterPlot3D(view, model);
}

//---

CQChartsScatterPlot3D::
CQChartsScatterPlot3D(View *view, const ModelP &model) :
 CQChartsPlot3D(view, view->charts()->plotType("scatter3d"), model),
 CQChartsObjPointData<CQChartsScatterPlot3D>(this)
{
}

CQChartsScatterPlot3D::
~CQChartsScatterPlot3D()
{
  term();
}

//---

void
CQChartsScatterPlot3D::
init()
{
  CQChartsPlot3D::init();

  //---

  NoUpdate noUpdate(this);

  //---

  // create a data label (shared state for all data labels)
  dataLabel_ = new CQChartsDataLabel(this);

  dataLabel_->setSendSignal(true);

  connect(dataLabel_, SIGNAL(dataChanged()), this, SLOT(dataLabelChanged()));

  //---

  setSymbolType(CQChartsSymbol::Type::CIRCLE);
  setSymbolStroked(true);
  setSymbolFilled (true);
  setSymbolFillColor(Color(Color::Type::PALETTE));

  setDataClip(false);

  //---

  addKey();

  addTitle();
}

void
CQChartsScatterPlot3D::
term()
{
  delete dataLabel_;
}

//------

void
CQChartsScatterPlot3D::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot3D::
setLabelColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot3D::
setXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot3D::
setYColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(yColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot3D::
setZColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(zColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

const CQChartsColumn &
CQChartsScatterPlot3D::
symbolTypeColumn() const
{
  return symbolTypeData_.column;
}

void
CQChartsScatterPlot3D::
setSymbolTypeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(symbolTypeData_.column, c, [&]() { updateObjs(); } );
}

const CQChartsColumn &
CQChartsScatterPlot3D::
symbolSizeColumn() const
{
  return symbolSizeData_.column;
}

void
CQChartsScatterPlot3D::
setSymbolSizeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(symbolSizeData_.column, c, [&]() { updateRangeAndObjs(); } );
}

const CQChartsColumn &
CQChartsScatterPlot3D::
fontSizeColumn() const
{
  return fontSizeData_.column;
}

void
CQChartsScatterPlot3D::
setFontSizeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(fontSizeData_.column, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot3D::
setDrawSymbols(bool b)
{
  CQChartsUtil::testAndSet(drawSymbols_, b, [&]() { drawObjs(); } );
}

void
CQChartsScatterPlot3D::
setDrawLines(bool b)
{
  CQChartsUtil::testAndSet(drawLines_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot3D::
setDrawBars(bool b)
{
  CQChartsUtil::testAndSet(drawBars_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot3D::
setFillUnder(bool b)
{
  CQChartsUtil::testAndSet(fillUnder_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsScatterPlot3D::
setBarSize(double s)
{
  CQChartsUtil::testAndSet(barSize_, s, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsScatterPlot3D::
dataLabelChanged()
{
  // TODO: not enough info to optimize behavior so reload all objects
  updateRangeAndObjs();
}

//---

void
CQChartsScatterPlot3D::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  addBaseProperties();

  // columns
  addProp("columns", "xColumn", "x", "X column");
  addProp("columns", "yColumn", "y", "Y column");
  addProp("columns", "zColumn", "z", "Z column");

  addProp("columns", "nameColumn" , "name" , "Name column");
  addProp("columns", "labelColumn", "label", "Label column");

  addProp("columns", "symbolTypeColumn", "symbolType", "Symbol type column");
  addProp("columns", "symbolSizeColumn", "symbolSize", "Symbol size column");
  addProp("columns", "fontSizeColumn"  , "fontSize"  , "Font size column");

  //---

  // options
  addProp("options", "drawSymbols", "drawSymbols", "Draw symbols at points");
  addProp("options", "drawLines"  , "drawLines"  , "Draw lines between points");
  addProp("options", "drawBars"   , "drawBars"   , "Draw bars between points");
  addProp("options", "fillUnder"  , "fillUnder"  , "Fill under lines between points");

  addProp("options", "barSize", "barSize", "Bar size factor");

  //---

  // symbol
  addSymbolProperties("symbol", "", "");

  // data labels
  dataLabel()->addBasicProperties("labels", "Labels");
  dataLabel()->addTextProperties ("labels", "Labels");

  //---

  CQChartsPlot3D::addCameraProperties();

  CQChartsPlot3D::addProperties();

  //---

  // color map
  addColorMapProperties();
}

//---

CQChartsGeom::Range
CQChartsScatterPlot3D::
calcRange() const
{
  CQPerfTrace trace("CQChartsScatterPlot3D::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsScatterPlot3D *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  if (! checkColumn(xColumn(), "X", th->xColumnType_, /*required*/true))
    columnsValid = false;
  if (! checkColumn(yColumn(), "Y", th->yColumnType_, /*required*/true))
    columnsValid = false;
  if (! checkColumn(zColumn(), "Z", th->zColumnType_, /*required*/true))
    columnsValid = false;

  if (! checkColumn(nameColumn (), "Name" )) columnsValid = false;
  if (! checkColumn(labelColumn(), "Label")) columnsValid = false;

  if (! columnsValid)
    return Range(-1.0, -1.0, 1.0, 1.0);

  //---

  initGroupData(CQChartsColumns(), CQChartsColumn());

  //---

  // calc data range (x, y, z values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsScatterPlot3D *plot) :
     plot_(plot) {
      hasGroups_ = (plot_->numGroups() > 1);
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      if (plot_->isInterrupt())
        return State::TERMINATE;

      //---

      auto *plot = const_cast<CQChartsScatterPlot3D *>(plot_);

      // init group
      ModelIndex xModelInd(plot, data.row, plot_->xColumn(), data.parent);

      int groupInd = plot_->rowGroupInd(xModelInd);

      bool hidden = (hasGroups_ && plot_->isSetHidden(groupInd));
      if (hidden) return State::OK;

      //---

      bool isCell = (plot_->xColumn().isCell() ||
                     plot_->yColumn().isCell() ||
                     plot_->zColumn().isCell());

      if (isCell) {
        for (int col = 0; col < numCols(); ++col) {
          visitCell(col, data);
        }

        return State::OK;
      }
      else {
        return visitCell(-1, data);
      }
    }

    State visitCell(int col, const VisitData &data) {
      auto *plot = const_cast<CQChartsScatterPlot3D *>(plot_);

      ModelIndex xModelInd(plot, data.row, plot_->xColumn(), data.parent);
      ModelIndex yModelInd(plot, data.row, plot_->yColumn(), data.parent);
      ModelIndex zModelInd(plot, data.row, plot_->zColumn(), data.parent);

      xModelInd.setCellCol(col);
      yModelInd.setCellCol(col);
      zModelInd.setCellCol(col);

      //---

      double x   { 0.0  }, y   { 0.0  }, z   { 0.0  };
      bool   okx { true }, oky { true }, okz { true };

      //---

      auto modelValue = [&](const ColumnType &columnType, const ModelIndex &modelInd,
                            double &value, bool &ok, bool isLog, int &numUnique) {
        if      (columnType == ColumnType::REAL || columnType == ColumnType::INTEGER) {
          ok = plot_->modelMappedReal(modelInd, value, isLog, data.row);
        }
        else if (columnType == ColumnType::TIME) {
          value = plot_->modelReal(modelInd, ok);
        }
        else if (modelInd.column().isRow() ||
                 modelInd.column().isColumn() ||
                 modelInd.column().isCell())
          value = plot_->modelReal(modelInd, ok);
        else {
          value = uniqueId(modelInd); ++numUnique;
        }
      };

      modelValue(plot_->xColumnType(), xModelInd, x, okx, plot_->isLogX(), uniqueX_);
      modelValue(plot_->yColumnType(), yModelInd, y, oky, plot_->isLogY(), uniqueY_);
      modelValue(plot_->zColumnType(), zModelInd, z, okz, /*log*/false   , uniqueZ_);

      //---

      if (plot_->isSkipBad() && (! okx || ! oky || ! okz))
        return State::SKIP;

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y) || CMathUtil::isNaN(z))
        return State::SKIP;

      range3D_.updateRange(x, y, z);

      return State::OK;
    }

    int uniqueId(const ModelIndex &columnInd) {
      bool ok;

      QVariant var = plot_->modelValue(columnInd, ok);
      if (! var.isValid()) return -1;

      auto *columnDetails = this->columnDetails(columnInd.column());

      return (columnDetails ? columnDetails->uniqueId(var) : -1);
    }

    CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) {
      if (! details_) {
        auto *modelData = plot_->getModelData();

        details_ = (modelData ? modelData->details() : nullptr);
      }

      return (details_ ? details_->columnDetails(column) : nullptr);
    }

    const Range3D &range3D() const { return range3D_; }

    bool isUniqueX() const { return uniqueX_ == numRows(); }
    bool isUniqueY() const { return uniqueY_ == numRows(); }
    bool isUniqueZ() const { return uniqueZ_ == numRows(); }

   private:
    const CQChartsScatterPlot3D* plot_      { nullptr };
    int                          hasGroups_ { false };
    Range3D                      range3D_;
    CQChartsModelDetails*        details_   { nullptr };
    int                          uniqueX_   { 0 };
    int                          uniqueY_   { 0 };
    int                          uniqueZ_   { 0 };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  auto dataRange3D = visitor.range3D();

  if (xmin().isSet()) dataRange3D.setXMin(xmin().real());
  if (xmax().isSet()) dataRange3D.setXMax(xmax().real());
  if (ymin().isSet()) dataRange3D.setYMin(ymin().real());
  if (ymax().isSet()) dataRange3D.setYMax(ymax().real());
  if (zmin().isSet()) dataRange3D.setZMin(zmin().real());
  if (zmax().isSet()) dataRange3D.setZMax(zmax().real());

  bool uniqueX = visitor.isUniqueX();
  bool uniqueY = visitor.isUniqueY();
  bool uniqueZ = visitor.isUniqueZ();

  if (isInterrupt())
    return Range(-1, -1, 1, 1);

  //---

  if (dataRange3D.isSet()) {
    if (uniqueX || uniqueY || uniqueZ) {
      if (uniqueX) {
        dataRange3D.updateRange(dataRange3D.xmin() - 0.5, dataRange3D.ymin(), dataRange3D.zmin());
        dataRange3D.updateRange(dataRange3D.xmax() + 0.5, dataRange3D.ymax(), dataRange3D.zmax());
      }

      if (uniqueY) {
        dataRange3D.updateRange(dataRange3D.xmin(), dataRange3D.ymin() - 0.5, dataRange3D.zmin());
        dataRange3D.updateRange(dataRange3D.xmax(), dataRange3D.ymax() + 0.5, dataRange3D.zmax());
      }

      if (uniqueZ) {
        dataRange3D.updateRange(dataRange3D.xmin(), dataRange3D.ymin(), dataRange3D.zmin() - 0.5);
        dataRange3D.updateRange(dataRange3D.xmax(), dataRange3D.ymax(), dataRange3D.zmin() + 0.5);
      }
    }
  }

  //---

  th->range3D_ = dataRange3D;

  th->camera_->init();

  th->groupNameValues_.clear();

  return Range(-1, -1, 1, 1);
}

void
CQChartsScatterPlot3D::
postUpdateRange()
{
  CQChartsPlot3D::postUpdateRange();

  groupObj_.clear();

  addAxis(xColumn(), yColumn(), zColumn());

  initGroups();

  addPointObjects();

  addObjs();
}

//------

bool
CQChartsScatterPlot3D::
createObjs(PlotObjs &) const
{
  return true;
}

void
CQChartsScatterPlot3D::
updateColumnNames()
{
  // set column header names
  CQChartsPlot::updateColumnNames();

  QString xname, yname;

  columnNames_[xColumn()] = xname;
  columnNames_[yColumn()] = yname;

  setColumnHeaderName(xColumn    (), "X"    );
  setColumnHeaderName(yColumn    (), "Y"    );
  setColumnHeaderName(zColumn    (), "Z"    );
  setColumnHeaderName(nameColumn (), "Name" );
  setColumnHeaderName(labelColumn(), "Label");
}

void
CQChartsScatterPlot3D::
initGroups()
{
  auto *th = const_cast<CQChartsScatterPlot3D *>(this);

  // init name values
  if (groupNameValues_.empty())
    addNameValues();

  th->groupPoints_.clear();

  //---

  th->updateColumnNames();
}

void
CQChartsScatterPlot3D::
addPointObjects() const
{
  auto *th = const_cast<CQChartsScatterPlot3D *>(this);

  //---

  auto *columnTypeMgr = charts()->columnTypeMgr();

  columnTypeMgr->startCache(model().data());

  //---

  int hasGroups = (numGroups() > 1);

  int ig = 0;
  int ng = groupNameValues_.size();

  if (! hasGroups) {
    if (ng <= 1 && parentPlot()) {
      ig = parentPlot()->childPlotIndex(this);
      ng = parentPlot()->numChildPlots();
    }
  }

  for (const auto &groupNameValue : groupNameValues_) {
    if (isInterrupt())
      break;

    int         groupInd   = groupNameValue.first;
    const auto &nameValues = groupNameValue.second;

    //---

    bool hidden = (hasGroups && isSetHidden(groupInd));

    if (hidden) { ++ig; continue; }

    //---

    // get group points
    auto pg = th->groupPoints_.find(groupInd);

    if (pg == th->groupPoints_.end())
      pg = th->groupPoints_.insert(pg, GroupPoints::value_type(groupInd, Points()));

    auto &points = const_cast<Points &>((*pg).second);

    //---

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      if (isInterrupt())
        break;

      bool hidden = (! hasGroups && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

    //const auto &name   = nameValue.first;
      const auto &values = nameValue.second.values;

      int nv = values.size();

      for (int iv = 0; iv < nv; ++iv) {
        if (isInterrupt())
          break;

        //---

        // get point position
        const auto &valuePoint = values[iv];

        const auto &p = valuePoint.p;

        //---

        // get symbol size (needed for bounding box)
        Length symbolSize(CQChartsUnits::NONE, 0.0);

        double sx, sy;

        plotSymbolSize(symbolSize.isValid() ? symbolSize : this->symbolSize(), sx, sy);

        //---

        // create point object
        ColorInd is1(is, ns);
        ColorInd ig1(ig, ng);
        ColorInd iv1(iv, nv);

        BBox bbox(p.x - sx, p.y - sy, p.x + sx, p.y + sy);

        auto *pointObj = createPointObj(groupInd, bbox, p, is1, ig1, iv1);

        pointObj->setModelInd(valuePoint.ind);

        if (symbolSize.isValid())
          pointObj->setSymbolSize(symbolSize);

      //objs.push_back(pointObj);

        points.push_back(p);

        //---

        // get point
        th->addPointObj(p, pointObj);

        //---

        // set optional symbol type
        CQChartsSymbol symbolType(CQChartsSymbol::Type::NONE);

        if (symbolType.isValid())
          pointObj->setSymbolType(symbolType);

        //---

        // set optional font size
        Length fontSize(CQChartsUnits::NONE, 0.0);

        if (fontSize.isValid())
          pointObj->setFontSize(fontSize);

        //---

        // set optional symbol fill color
        Color symbolColor(Color::Type::NONE);

        if (colorColumn().isValid()) {
          if (! colorColumnColor(valuePoint.row, valuePoint.ind.parent(), symbolColor))
            symbolColor = Color(Color::Type::NONE);
        }

        if (symbolColor.isValid())
          pointObj->setColor(symbolColor);

        //---

        // set optional point label
        QString pointName;

        if (labelColumn().isValid() || nameColumn().isValid()) {
          bool ok;

          if (labelColumn().isValid()) {
            ModelIndex labelInd(th, valuePoint.row, labelColumn(), valuePoint.ind.parent());

            pointName = modelString(labelInd, ok);
          }
          else {
            ModelIndex nameInd(th, valuePoint.row, nameColumn(), valuePoint.ind.parent());

            pointName = modelString(nameInd, ok);
          }

          if (! ok)
            pointName = "";
        }

        if (pointName.length())
          pointObj->setName(pointName);

        //---

        if (dataLabel()->isVisible()) {
          CQChartsPenBrush penBrush;

          QColor tc = dataLabel()->interpTextColor(ColorInd());

          setPenBrush(penBrush,
            PenData(true, tc, dataLabel()->textAlpha()), BrushData(false));

          Length fontSize(CQChartsUnits::NONE, 0.0);

          if (fontSizeColumn().isValid()) {
            if (! columnFontSize(valuePoint.row, valuePoint.ind.parent(), fontSizeData_, fontSize))
              fontSize = Length(CQChartsUnits::NONE, 0.0);
          }

          auto font = this->font();

          if (! font.isValid()) {
            font = dataLabel()->textFont();

            if (fontSize.isValid()) {
              double fontPixelSize = lengthPixelHeight(fontSize);

              // scale to font size
              fontPixelSize = limitFontSize(fontPixelSize);

              font.setPointSizeF(fontPixelSize);
            }
          }

          CQChartsTextOptions textOptions;

          textOptions.angle         = dataLabel()->textAngle();
          textOptions.align         = Qt::AlignHCenter|Qt::AlignBottom;
          textOptions.contrast      = dataLabel()->isTextContrast();
          textOptions.contrastAlpha = dataLabel()->textContrastAlpha();
          textOptions.clipLength    = dataLabel()->textClipLength();

          auto *textObj = createTextObj(p, p, pointName);

          textObj->setPenBrush   (penBrush);
          textObj->setFont       (font);
          textObj->setTextOptions(textOptions);

          th->addPointObj(p, textObj);
        }
      }

      ++is;
    }

    ++ig;
  }

  //---

  columnTypeMgr->endCache(model().data());
}

void
CQChartsScatterPlot3D::
addNameValues() const
{
  CQPerfTrace trace("CQChartsScatterPlot3D::addNameValues");

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsScatterPlot3D *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      bool isCell = (plot_->xColumn().isCell() ||
                     plot_->yColumn().isCell() ||
                     plot_->zColumn().isCell());

      if (isCell) {
        for (int col = 0; col < numCols(); ++col) {
          (void) visitCell(col, data);
        }

        return State::OK;
      }
      else {
        return visitCell(-1, data);
      }
    }

    State visitCell(int col, const VisitData &data) {
      auto *plot = const_cast<CQChartsScatterPlot3D *>(plot_);

      ModelIndex xModelInd(plot, data.row, plot_->xColumn(), data.parent);
      ModelIndex yModelInd(plot, data.row, plot_->yColumn(), data.parent);
      ModelIndex zModelInd(plot, data.row, plot_->zColumn(), data.parent);

      xModelInd.setCellCol(col);
      yModelInd.setCellCol(col);
      zModelInd.setCellCol(col);

      //---

      // get group
      int groupInd = plot_->rowGroupInd(xModelInd);

      //---

      // get x, y, z value
      QModelIndex xInd  = plot_->modelIndex(xModelInd);
      QModelIndex xInd1 = plot_->normalizeIndex(xInd);

      double x   { 0.0  }, y   { 0.0  }, z   { 0.0  };
      bool   okx { true }, oky { true }, okz { true };

      //---

      auto modelValue = [&](const ColumnType &columnType, const ModelIndex &modelInd,
                            double &value, bool &ok, bool isLog) {
        if      (columnType == ColumnType::REAL || columnType == ColumnType::INTEGER) {
          ok = plot_->modelMappedReal(modelInd, value, isLog, data.row);
        }
        else if (columnType == ColumnType::TIME) {
          value = plot_->modelReal(modelInd, ok);
        }
        else if (modelInd.column().isRow() ||
                 modelInd.column().isColumn() ||
                 modelInd.column().isCell())
          value = plot_->modelReal(modelInd, ok);
        else {
          value = uniqueId(modelInd);
        }
      };

      modelValue(plot_->xColumnType(), xModelInd, x, okx, plot_->isLogX());
      modelValue(plot_->yColumnType(), yModelInd, y, oky, plot_->isLogY());
      modelValue(plot_->zColumnType(), zModelInd, z, okz, /*log*/false);

      //---

      if (plot_->isSkipBad() && (! okx || ! oky || ! okz))
        return State::SKIP;

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      //---

      // get optional grouping name (name column, title)
      QString name;

      if (plot_->nameColumn().isValid()) {
        ModelIndex nameColumnInd(plot, data.row, plot_->nameColumn(), data.parent);

        bool ok;

        name = plot_->modelString(nameColumnInd, ok);
      }

      if (! name.length() && plot_->title())
        name = plot_->title()->textStr();

      //---

      // get symbol type, size, font size and color
      Color color;

      // get color label (needed if not string ?)
      if (plot_->colorColumn().isValid()) {
        (void) plot_->colorColumnColor(data.row, data.parent, color);
      }

      //---

      Point3D p(x, y, z);

      plot->addNameValue(groupInd, name, p, data.row, xInd1, color);

      return State::OK;
    }

    int uniqueId(const ModelIndex &columnInd) {
      bool ok;

      QVariant var = plot_->modelValue(columnInd, ok);
      if (! var.isValid()) return -1;

      auto *columnDetails = this->columnDetails(columnInd.column());

      return (columnDetails ? columnDetails->uniqueId(var) : -1);
    }

    CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) {
      if (! details_) {
        auto *modelData = plot_->getModelData();

        details_ = (modelData ? modelData->details() : nullptr);
      }

      return (details_ ? details_->columnDetails(column) : nullptr);
    }

   private:
    const CQChartsScatterPlot3D* plot_    { nullptr };
    CQChartsModelDetails*      details_ { nullptr };
  };

  RowVisitor visitor(this);

  visitModel(visitor);
}

void
CQChartsScatterPlot3D::
addNameValue(int groupInd, const QString &name, const Point3D &p, int row,
             const QModelIndex &xind, const Color &color)
{
  auto &valuesData = groupNameValues_[groupInd][name];

  valuesData.xrange.add(p.x);
  valuesData.yrange.add(p.y);
  valuesData.zrange.add(p.z);

  valuesData.values.emplace_back(p, row, xind, color);
}

void
CQChartsScatterPlot3D::
addKeyItems(CQChartsPlotKey *key)
{
  if (isOverlay() && ! isFirstPlot())
    return;

  addPointKeyItems(key);

  key->plot()->updateKeyPosition(/*force*/true);
}

void
CQChartsScatterPlot3D::
addPointKeyItems(CQChartsPlotKey *key)
{
  auto addKeyItem = [&](int ind, const QString &name, int i, int n) {
    ColorInd ic(i, n);

    auto *colorItem = new CQChartsScatterKeyColor3D(this, ind , ic);
    auto *textItem  = new CQChartsKeyText          (this, name, ic);

    auto *groupItem = new CQChartsKeyItemGroup(this);

    groupItem->addItem(colorItem);
    groupItem->addItem(textItem );

    //key->addItem(colorItem, i, 0);
    //key->addItem(textItem , i, 1);

    key->addItem(groupItem, i, 0);

    return colorItem;
  };

  int ng = groupNameValues_.size();

  // multiple group - key item per group
  if      (ng > 1) {
    int ig = 0;

    for (const auto &groupNameValue : groupNameValues_) {
      int     groupInd  = groupNameValue.first;
      QString groupName = groupIndName(groupInd);

      auto *colorItem = addKeyItem(groupInd, groupName, ig, ng);

      //--

      if (colorColumn().isValid() && colorColumn().isGroup()) {
        if (isColorMapped()) {
          double r = CMathUtil::map(groupInd, colorColumnData_.data_min, colorColumnData_.data_max,
                                    colorMapMin(), colorMapMax());

          auto color = Color(Color::Type::PALETTE_VALUE, r);

          if (color.isValid())
            colorItem->setColor(color);
        }
      }

      //--

      ++ig;
    }
  }
  // single group - key item per value set
  else if (ng > 0) {
    const auto &nameValues = (*groupNameValues_.begin()).second;

    int ns = nameValues.size();

    if (ns > 1) {
      int is = 0;

      for (const auto &nameValue : nameValues) {
        const auto &name = nameValue.first;

        auto *colorItem = addKeyItem(-1, name, is, ns);

        //--

        if (colorColumn().isValid()) {
          const auto &values = nameValue.second.values;

          int nv = values.size();

          if (nv > 0) {
            const auto &valuePoint = values[0];

            Color color;

            if (colorColumnColor(valuePoint.row, valuePoint.ind.parent(), color))
              colorItem->setColor(color);
          }
        }

        //--

        ++is;
      }
    }
    else {
      if (parentPlot() && ! nameValues.empty()) {
        const auto &name = nameValues.begin()->first;

        int ig = parentPlot()->childPlotIndex(this);
        int ng = parentPlot()->numChildPlots();

        (void) addKeyItem(-1, name, ig, ng);
      }
    }
  }
}

//---

bool
CQChartsScatterPlot3D::
probe(ProbeData &probeData) const
{
  CQChartsPlotObj *obj;

  if (! objNearestPoint(probeData.p, obj))
    return false;

  auto c = obj->rect().getCenter();

  probeData.p    = c;
  probeData.both = true;

  probeData.xvals.push_back(c.x);
  probeData.yvals.push_back(c.y);

  return true;
}

//---

bool
CQChartsScatterPlot3D::
addMenuItems(QMenu *)
{
  return true;
}

//------

CQChartsGeom::BBox
CQChartsScatterPlot3D::
calcAnnotationBBox() const
{
  CQPerfTrace trace("CQChartsScatterPlot3D::calcAnnotationBBox");

  BBox bbox;

  return bbox;
}

//------

bool
CQChartsScatterPlot3D::
hasBackground() const
{
  return true;
}

void
CQChartsScatterPlot3D::
execDrawBackground(CQChartsPaintDevice *device) const
{
  CQChartsPlot::execDrawBackground(device);
}

bool
CQChartsScatterPlot3D::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsScatterPlot3D::
execDrawForeground(CQChartsPaintDevice *) const
{
}

void
CQChartsScatterPlot3D::
preDrawObjs(CQChartsPaintDevice *) const
{
}

void
CQChartsScatterPlot3D::
addObjs() const
{
  auto *th = const_cast<CQChartsScatterPlot3D *>(this);

  int ng = groupPoints_.size();

  if (isDrawBars()) {
    for (auto &gp : groupPoints_) {
      for (auto &p : gp.second) {
        ColorInd ig(gp.first, ng);

        th->addBarPolygons(p, ig);
      }
    }
  }

  if (isFillUnder()) {
    for (auto &gp : groupPoints_) {
      if (gp.second.empty())
        continue;

      auto &groupData = th->getGroupData(gp.first);

      if (! groupData.polygon) {
        auto p = gp.second[0];

        //---

        groupData.polygon = createPolygonObj();

        groupData.polygon->setIg(ColorInd(gp.first, ng));

        th->addPointObj(p, groupData.polygon);
      }

      //---

      using Points = std::set<Point3D>;

      Points points;

      for (auto &p : gp.second)
        points.insert(p);

      //---

      bool first = true;

      Point3D p2;

      for (const auto &p : points) {
        p2 = p;

        if (first) {
          Point3D p1(p2.x, p2.y, range3D_.zmin());

          groupData.polygon->addPoint(p1);

          first = false;
        }

        groupData.polygon->addPoint(p2);
      }

      if (! first) {
        Point3D p1(p2.x, p2.y, range3D_.zmin());

        groupData.polygon->addPoint(p1);
      }
    }
  }

  //---

  if (isDrawLines()) {
    for (auto &gp : groupPoints_) {
      if (gp.second.empty())
        continue;

      auto &groupData = th->getGroupData(gp.first);

      if (! groupData.polyline) {
        auto p = gp.second[0];

        //---

        groupData.polyline = createPolylineObj();

        groupData.polyline->setIg(ColorInd(gp.first, ng));

        th->addPointObj(p, groupData.polyline);
      }

      //---

      using Points = std::set<Point3D>;

      Points points;

      for (auto &p : gp.second)
        points.insert(p);

      //---

      for (auto &p : points)
        groupData.polyline->addPoint(p);
    }
  }
}

//---

void
CQChartsScatterPlot3D::
addBarPolygons(const Point3D &p, const ColorInd &ig)
{
  const auto &range3D = this->range3D();

  double dt = this->barSize();

  double dx = dt*range3D.xsize();
  double dy = dt*range3D.ysize();
//double dz = dt*range3D.zsize();

  Point3D p1(p.x - dx, p.y - dy, p.z);
  Point3D p2(p.x + dx, p.y - dy, p.z);
  Point3D p3(p.x + dx, p.y + dy, p.z);
  Point3D p4(p.x - dx, p.y + dy, p.z);

  Point3D p5(p.x - dx, p.y - dy, range3D.zmin());
  Point3D p6(p.x + dx, p.y - dy, range3D.zmin());
  Point3D p7(p.x + dx, p.y + dy, range3D.zmin());
  Point3D p8(p.x - dx, p.y + dy, range3D.zmin());

  auto createPoly = [&](const Point3D &p1, const Point3D &p2, const Point3D &p3, const Point3D &p4,
                        const Point3D &n) {
    Polygon3D poly;

    poly.addPoint(p1);
    poly.addPoint(p2);
    poly.addPoint(p3);
    poly.addPoint(p4);

    auto *polyObj = createPolygonObj(poly);

    polyObj->setIg(ig);
    polyObj->setNormal(n);

    Point3D pm((p1 + p2 + p3 + p4)/4.0);

    Point3D pm1(pm.x, pm.y, range3D.zmin());

    addPointObj(pm1, polyObj);
  };

  createPoly(p1, p2, p6, p5, Point3D( 0, -1,  0));
  createPoly(p2, p3, p7, p6, Point3D( 1,  0,  0));
  createPoly(p3, p4, p8, p7, Point3D( 0,  1,  0));
  createPoly(p4, p1, p5, p8, Point3D(-1,  0,  0));
  createPoly(p1, p2, p3, p4, Point3D( 0,  0,  1));
  createPoly(p5, p6, p7, p8, Point3D( 0,  0, -1));
}

//---

void
CQChartsScatterPlot3D::
postDrawObjs(CQChartsPaintDevice *device) const
{
  drawPointObjs(device);
}

CQChartsScatterPlot3D::GroupData &
CQChartsScatterPlot3D::
getGroupData(int groupId)
{
  auto pg = groupObj_.find(groupId);

  if (pg == groupObj_.end())
    pg = groupObj_.insert(pg, GroupObj::value_type(groupId, GroupData()));

  return (*pg).second;
}

//---

CQChartsScatterPoint3DObj *
CQChartsScatterPlot3D::
createPointObj(int groupInd, const BBox &rect, const Point3D &pos,
               const ColorInd &is, const ColorInd &ig, const ColorInd &iv) const
{
  return new CQChartsScatterPoint3DObj(this, groupInd, rect, pos, is, ig, iv);
}

//------

CQChartsScatterPoint3DObj::
CQChartsScatterPoint3DObj(const CQChartsScatterPlot3D *plot, int groupInd,
                          const BBox &rect, const Point3D &pos,
                          const ColorInd &is, const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlot3DObj(plot), groupInd_(groupInd), pos_(pos)
{
  setRect(rect);

  setIs(is);
  setIg(ig);
  setIv(iv);

  setDetailHint(DetailHint::MAJOR);
}

const CQChartsScatterPlot3D *
CQChartsScatterPoint3DObj::
scatterPlot() const
{
  return dynamic_cast<const CQChartsScatterPlot3D *>(plot3D());
}

//---

CQChartsSymbol
CQChartsScatterPoint3DObj::
symbolType() const
{
  auto symbolType = extraData().symbolType;

  if (! symbolType.isValid())
    symbolType = scatterPlot()->symbolType();

  return symbolType;
}

CQChartsLength
CQChartsScatterPoint3DObj::
symbolSize() const
{
  auto symbolSize = extraData().symbolSize;

  if (! symbolSize.isValid())
    symbolSize = scatterPlot()->symbolSize();

  return symbolSize;
}

CQChartsLength
CQChartsScatterPoint3DObj::
fontSize() const
{
  auto fontSize = extraData().fontSize;

  return fontSize;
}

CQChartsColor
CQChartsScatterPoint3DObj::
color() const
{
  auto color = extraData().color;

  return color;
}

//---

QString
CQChartsScatterPoint3DObj::
calcId() const
{
  QModelIndex ind1 = plot_->unnormalizeIndex(modelInd());

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  return QString("%1:%2:%3:%4").arg(typeName()).arg(is_.i).arg(ig_.i).arg(iv_.i);
}

QString
CQChartsScatterPoint3DObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  // add name (label or name column) as header
  if (name_.length())
    tableTip.addBoldLine(name_);

  //---

  // TODO: id column

  //---

  // add group column
  if (ig_.n > 1) {
    QString groupName = scatterPlot()->groupIndName(groupInd_);

    tableTip.addTableRow("Group", groupName);
  }

  //---

  // add x, y columns
  QString xstr = plot()->xStr(pos_.x);
  QString ystr = plot()->yStr(pos_.y);
  QString zstr = plot()->yStr(pos_.z);

  tableTip.addTableRow(scatterPlot()->xHeaderName(), xstr);
  tableTip.addTableRow(scatterPlot()->yHeaderName(), ystr);
  tableTip.addTableRow(scatterPlot()->zHeaderName(), zstr);

  //---

  // get values for name (grouped id identical names)
  CQChartsScatterPlot3D::ValueData valuePoint;

  auto pg = scatterPlot()->groupNameValues().find(groupInd_);
  assert(pg != scatterPlot()->groupNameValues().end());

  auto p = (*pg).second.find(name_);

  if (p != (*pg).second.end()) {
    const auto &values = (*p).second.values;

    valuePoint = values[iv_.i];
  }

  //---

  auto addColumnRowValue = [&](const CQChartsColumn &column) {
    if (! column.isValid()) return;

    auto *plot = const_cast<CQChartsScatterPlot3D *>(scatterPlot());

    ModelIndex columnInd(plot, modelInd().row(), column, modelInd().parent());

    bool ok;

    QString str = plot_->modelString(columnInd, ok);
    if (! ok) return;

    tableTip.addTableRow(plot_->columnHeaderName(column), str);
  };

  //---

  // add color column
  if (valuePoint.color.isValid())
    tableTip.addTableRow(plot_->colorHeaderName(), valuePoint.color.colorStr());
  else
    addColumnRowValue(plot_->colorColumn());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

bool
CQChartsScatterPoint3DObj::
inside(const Point &p) const
{
  double sx, sy;

  plot_->pixelSymbolSize(this->symbolSize(), sx, sy);

  auto *camera = plot3D()->camera();

  auto pt = camera->transform(point());

  auto p1 = plot_->windowToPixel(Point(pt.x, pt.y));

  BBox pbbox(p1.x - sx, p1.y - sy, p1.x + sx, p1.y + sy);

  auto pp = plot_->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsScatterPoint3DObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, scatterPlot()->xColumn());
  addColumnSelectIndex(inds, scatterPlot()->yColumn());
  addColumnSelectIndex(inds, scatterPlot()->zColumn());

  addColumnSelectIndex(inds, plot_->colorColumn());
}

//---

void
CQChartsScatterPoint3DObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  ColorInd ic = calcColorInd();

  scatterPlot()->setSymbolPenBrush(penBrush, ic);

  // override symbol fill color for custom color
  auto color = this->color();

  if (color.isValid()) {
    QColor c = plot_->interpColor(color, ic);

    c.setAlphaF(scatterPlot()->symbolFillAlpha().value());

    penBrush.brush.setColor(c);
  }

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::SYMBOL);
}

void
CQChartsScatterPoint3DObj::
postDraw(CQChartsPaintDevice *device)
{
  if (! scatterPlot()->isDrawSymbols())
    return;

  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // get symbol type and size
  Symbol symbolType = this->symbolType();
  Length symbolSize = this->symbolSize();

//double sx, sy;
//plot_->pixelSymbolSize(symbolSize, sx, sy);

  //---

  auto *camera = plot3D()->camera();

  auto pt = camera->transform(point());

  auto pt2 = pt.point2D();

  plot_->drawSymbol(device, pt2, symbolType, symbolSize, penBrush);

  //---

  double sx = plot_->lengthPlotWidth (symbolSize);
  double sy = plot_->lengthPlotHeight(symbolSize);

  setDrawBBox(BBox(pt2.x - sx, pt2.y - sy, pt2.x + sx, pt2.y + sy));
}

//------

CQChartsScatterKeyColor3D::
CQChartsScatterKeyColor3D(CQChartsScatterPlot3D *plot, int groupInd, const ColorInd &ic) :
 CQChartsKeyColorBox(plot, ColorInd(), ColorInd(), ic), groupInd_(groupInd)
{
}

bool
CQChartsScatterKeyColor3D::
selectPress(const Point &, CQChartsSelMod selMod)
{
  auto *plot = qobject_cast<CQChartsScatterPlot3D *>(plot_);

  int ih = hideIndex();

  if (selMod == CQChartsSelMod::ADD) {
    for (int i = 0; i < ic_.n; ++i) {
      plot_->CQChartsPlot::setSetHidden(i, i != ih);
    }
  }
  else {
    plot->setSetHidden(ih, ! plot->isSetHidden(ih));
  }

  plot->updateRangeAndObjs();

  return true;
}

QBrush
CQChartsScatterKeyColor3D::
fillBrush() const
{
  auto *plot = qobject_cast<CQChartsScatterPlot3D *>(plot_);

  QColor c;

  if (color_.isValid())
    c = plot_->interpColor(color_, ColorInd());
  else {
    c = plot->interpSymbolFillColor(ic_);

    //c = CQChartsKeyColorBox::fillBrush().color();
  }

  c.setAlphaF(plot->symbolFillAlpha().value());

  int ih = hideIndex();

  if (plot->isSetHidden(ih))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

int
CQChartsScatterKeyColor3D::
hideIndex() const
{
  return (groupInd_ >= 0 ? groupInd_ : ic_.i);
}
