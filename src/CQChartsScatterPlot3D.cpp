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
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsScatterPlot3D(view, model);
}

//---

CQChartsScatterPlot3D::
CQChartsScatterPlot3D(CQChartsView *view, const ModelP &model) :
 CQChartsPlot3D(view, view->charts()->plotType("scatter3d"), model),
 CQChartsObjPointData<CQChartsScatterPlot3D>(this)
{
  NoUpdate noUpdate(this);

  //---

  setSymbolType(CQChartsSymbol::Type::CIRCLE);
  setSymbolStroked(true);
  setSymbolFilled (true);
  setSymbolFillColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setDataClip(false);

  //---

  addKey();

  addTitle();
}

CQChartsScatterPlot3D::
~CQChartsScatterPlot3D()
{
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

  //---

  // symbol
  addSymbolProperties("symbol", "", "");

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

  auto *th = const_cast<CQChartsScatterPlot3D *>(this);

  //---

  // check columns
  bool columnsValid = true;

  th->clearErrors();

  if (! checkColumn(xColumn(), "X", th->xColumnType_, /*required*/true))
    columnsValid = false;
  if (! checkColumn(yColumn(), "Y", th->yColumnType_, /*required*/true))
    columnsValid = false;
  if (! checkColumn(zColumn(), "Z", th->zColumnType_, /*required*/true))
    columnsValid = false;

  if (! checkColumn(nameColumn (), "Name" )) columnsValid = false;
  if (! checkColumn(labelColumn(), "Label")) columnsValid = false;

  if (! columnsValid)
    return CQChartsGeom::Range(-1.0, -1.0, 1.0, 1.0);

  //---

  initGroupData(CQChartsColumns(), CQChartsColumn());

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsScatterPlot3D *plot) :
     plot_(plot) {
      hasGroups_ = (plot_->numGroups() > 1);
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      if (plot_->isInterrupt())
        return State::TERMINATE;

      CQChartsModelIndex xModelInd(data.row, plot_->xColumn(), data.parent);
      CQChartsModelIndex yModelInd(data.row, plot_->yColumn(), data.parent);
      CQChartsModelIndex zModelInd(data.row, plot_->zColumn(), data.parent);

      // init group
      int groupInd = plot_->rowGroupInd(xModelInd);

      bool hidden = (hasGroups_ && plot_->isSetHidden(groupInd));

      if (! hidden) {
        double x   { 0.0  }, y   { 0.0  }, z   { 0.0  };
        bool   okx { true }, oky { true }, okz { true };

        //---

        if      (plot_->xColumnType() == ColumnType::REAL ||
                 plot_->xColumnType() == ColumnType::INTEGER) {
          okx = plot_->modelMappedReal(xModelInd, x, plot_->isLogX(), data.row);
        }
        else if (plot_->xColumnType() == ColumnType::TIME) {
          x = plot_->modelReal(xModelInd, okx);
        }
        else {
          x = uniqueId(data, plot_->xColumn()); ++uniqueX_;
        }

        //---

        if      (plot_->yColumnType() == ColumnType::REAL ||
                 plot_->yColumnType() == ColumnType::INTEGER) {
          oky = plot_->modelMappedReal(yModelInd, y, plot_->isLogY(), data.row);
        }
        else if (plot_->yColumnType() == ColumnType::TIME) {
          y = plot_->modelReal(yModelInd, oky);
        }
        else {
          y = uniqueId(data, plot_->yColumn()); ++uniqueY_;
        }

        //---

        if      (plot_->zColumnType() == ColumnType::REAL ||
                 plot_->zColumnType() == ColumnType::INTEGER) {
          okz = plot_->modelMappedReal(zModelInd, z, /*log*/false, data.row);
        }
        else if (plot_->zColumnType() == ColumnType::TIME) {
          z = plot_->modelReal(zModelInd, okz);
        }
        else {
          z = uniqueId(data, plot_->zColumn()); ++uniqueZ_;
        }

        //---

        if (plot_->isSkipBad() && (! okx || ! oky))
          return State::SKIP;

        if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
          return State::SKIP;

        range3D_.updateRange(x, y, z);
      }

      return State::OK;
    }

    int uniqueId(const VisitData &data, const CQChartsColumn &column) {
      CQChartsModelIndex columnInd(data.row, column, data.parent);

      bool ok;

      QVariant var = plot_->modelValue(columnInd, ok);
      if (! var.isValid()) return -1;

      auto *columnDetails = this->columnDetails(column);

      return (columnDetails ? columnDetails->uniqueId(var) : -1);
    }

    CQChartsModelColumnDetails *columnDetails(const CQChartsColumn &column) {
      if (! details_) {
        auto *modelData = plot_->getModelData();

        details_ = (modelData ? modelData->details() : nullptr);
      }

      return (details_ ? details_->columnDetails(column) : nullptr);
    }

    const CQChartsGeom::Range3D &range3D() const { return range3D_; }

    bool isUniqueX() const { return uniqueX_ == numRows(); }
    bool isUniqueY() const { return uniqueY_ == numRows(); }
    bool isUniqueZ() const { return uniqueZ_ == numRows(); }

   private:
    const CQChartsScatterPlot3D* plot_      { nullptr };
    int                          hasGroups_ { false };
    CQChartsGeom::Range3D        range3D_;
    CQChartsModelDetails*        details_   { nullptr };
    int                          uniqueX_   { 0 };
    int                          uniqueY_   { 0 };
    int                          uniqueZ_   { 0 };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  auto dataRange3D = visitor.range3D();

  bool uniqueX = visitor.isUniqueX();
  bool uniqueY = visitor.isUniqueY();
  bool uniqueZ = visitor.isUniqueZ();

  if (isInterrupt())
    return CQChartsGeom::Range(-1, -1, 1, 1);

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

  return CQChartsGeom::Range(-1, -1, 1, 1);
}

//------

void
CQChartsScatterPlot3D::
clearPlotObjects()
{
  groupNameValues_.clear();

  CQChartsPlot::clearPlotObjects();
}

bool
CQChartsScatterPlot3D::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsScatterPlot3D::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsScatterPlot3D *>(this);

  //---

  // init value set
  //initValueSets();

  //---

  // init name values
  if (groupNameValues_.empty())
    addNameValues();

  th->groupPoints_.clear();

  //---

  th->updateColumnNames();

  //---

  addPointObjects(objs);

  //---

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

  setColumnHeaderName(nameColumn (), "Name" );
  setColumnHeaderName(labelColumn(), "Label");
}

void
CQChartsScatterPlot3D::
addPointObjects(PlotObjs &objs) const
{
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

    int               groupInd   = groupNameValue.first;
    const NameValues &nameValues = groupNameValue.second;

    //---

    bool hidden = (hasGroups && isSetHidden(groupInd));

    if (hidden) { ++ig; continue; }

    //---

    // get group points
    auto *th = const_cast<CQChartsScatterPlot3D *>(this);

    auto pg = th->groupPoints_.find(groupInd);

    if (pg == th->groupPoints_.end())
      pg = th->groupPoints_.insert(pg, GroupPoints::value_type(groupInd, Points()));

    Points &points = const_cast<Points &>((*pg).second);

    //---

    int is = 0;
    int ns = nameValues.size();

    for (const auto &nameValue : nameValues) {
      if (isInterrupt())
        break;

      bool hidden = (! hasGroups && isSetHidden(is));

      if (hidden) { ++is; continue; }

      //---

    //const QString &name   = nameValue.first;
      const Values  &values = nameValue.second.values;

      int nv = values.size();

      for (int iv = 0; iv < nv; ++iv) {
        if (isInterrupt())
          break;

        //---

        // get point position
        const ValueData &valuePoint = values[iv];

        const auto &p = valuePoint.p;

        //---

        // get symbol size (needed for bounding box)
        CQChartsLength symbolSize(CQChartsUnits::NONE, 0.0);

        double sx, sy;

        plotSymbolSize(symbolSize.isValid() ? symbolSize : this->symbolSize(), sx, sy);

        //---

        // create point object
        ColorInd is1(is, ns);
        ColorInd ig1(ig, ng);
        ColorInd iv1(iv, nv);

        CQChartsGeom::BBox bbox(p.x - sx, p.y - sy, p.x + sx, p.y + sy);

        auto *pointObj = new CQChartsScatterPoint3DObj(this, groupInd, bbox, p, is1, ig1, iv1);

        pointObj->setModelInd(valuePoint.ind);

        if (symbolSize.isValid())
          pointObj->setSymbolSize(symbolSize);

        objs.push_back(pointObj);

        points.push_back(p);

        //---

        // set optional symbol type
        CQChartsSymbol symbolType(CQChartsSymbol::Type::NONE);

        if (symbolType.isValid())
          pointObj->setSymbolType(symbolType);

        //---

        // set optional font size
        CQChartsLength fontSize(CQChartsUnits::NONE, 0.0);

        if (fontSize.isValid())
          pointObj->setFontSize(fontSize);

        //---

        // set optional symbol fill color
        CQChartsColor symbolColor(CQChartsColor::Type::NONE);

        if (colorColumn().isValid()) {
          if (! colorColumnColor(valuePoint.row, valuePoint.ind.parent(), symbolColor))
            symbolColor = CQChartsColor(CQChartsColor::Type::NONE);
        }

        if (symbolColor.isValid())
          pointObj->setColor(symbolColor);

        //---

        // set optional point label
        QString pointName;

        if (labelColumn().isValid() || nameColumn().isValid()) {
          bool ok;

          if (labelColumn().isValid()) {
            CQChartsModelIndex labelInd(valuePoint.row, labelColumn(), valuePoint.ind.parent());

            pointName = modelString(labelInd, ok);
          }
          else {
            CQChartsModelIndex nameInd(valuePoint.row, nameColumn(), valuePoint.ind.parent());

            pointName = modelString(nameInd, ok);
          }

          if (! ok)
            pointName = "";
        }

        if (pointName.length())
          pointObj->setName(pointName);
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
      CQChartsModelIndex xModelInd(data.row, plot_->xColumn(), data.parent);
      CQChartsModelIndex yModelInd(data.row, plot_->yColumn(), data.parent);
      CQChartsModelIndex zModelInd(data.row, plot_->zColumn(), data.parent);

      // get group
      int groupInd = plot_->rowGroupInd(xModelInd);

      //---

      // get x, y value
      QModelIndex xInd  = plot_->modelIndex(xModelInd);
      QModelIndex xInd1 = plot_->normalizeIndex(xInd);

      double x   { 0.0  }, y   { 0.0  }, z   { 0.0  };
      bool   okx { true }, oky { true }, okz { true };

      //---

      if      (plot_->xColumnType() == ColumnType::REAL ||
               plot_->xColumnType() == ColumnType::INTEGER) {
        okx = plot_->modelMappedReal(xModelInd, x, plot_->isLogX(), data.row);
      }
      else if (plot_->xColumnType() == ColumnType::TIME) {
        x = plot_->modelReal(xModelInd, okx);
      }
      else {
        x = uniqueId(data, plot_->xColumn());
      }

      //---

      if      (plot_->yColumnType() == ColumnType::REAL ||
               plot_->yColumnType() == ColumnType::INTEGER) {
        oky = plot_->modelMappedReal(yModelInd, y, plot_->isLogY(), data.row);
      }
      else if (plot_->yColumnType() == ColumnType::TIME) {
        y = plot_->modelReal(yModelInd, oky);
      }
      else {
        y = uniqueId(data, plot_->yColumn());
      }

      //---

      if      (plot_->zColumnType() == ColumnType::REAL ||
               plot_->zColumnType() == ColumnType::INTEGER) {
        okz = plot_->modelMappedReal(zModelInd, z, /*log*/false, data.row);
      }
      else if (plot_->zColumnType() == ColumnType::TIME) {
        z = plot_->modelReal(zModelInd, okz);
      }
      else {
        z = uniqueId(data, plot_->zColumn());
      }

      //---

      if (plot_->isSkipBad() && (! okx || ! oky))
        return State::SKIP;

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      //---

      // get optional grouping name (name column, title)
      QString name;

      if (plot_->nameColumn().isValid()) {
        CQChartsModelIndex nameColumnInd(data.row, plot_->nameColumn(), data.parent);

        bool ok;

        name = plot_->modelString(nameColumnInd, ok);
      }

      if (! name.length() && plot_->title())
        name = plot_->title()->textStr();

      //---

      // get symbol type, size, font size and color
      CQChartsColor color;

      // get color label (needed if not string ?)
      if (plot_->colorColumn().isValid()) {
        (void) plot_->colorColumnColor(data.row, data.parent, color);
      }

      //---

      auto *plot = const_cast<CQChartsScatterPlot3D *>(plot_);

      Point3D p(x, y, z);

      plot->addNameValue(groupInd, name, p, data.row, xInd1, color);

      return State::OK;
    }

    int uniqueId(const VisitData &data, const CQChartsColumn &column) {
      CQChartsModelIndex columnInd(data.row, column, data.parent);

      bool ok;

      QVariant var = plot_->modelValue(columnInd, ok);
      if (! var.isValid()) return -1;

      auto *columnDetails = this->columnDetails(column);

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
             const QModelIndex &xind, const CQChartsColor &color)
{
  ValuesData &valuesData = groupNameValues_[groupInd][name];

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

    key->addItem(colorItem, i, 0);
    key->addItem(textItem , i, 1);

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

          auto color = CQChartsColor(CQChartsColor::Type::PALETTE_VALUE, r);

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
    const NameValues &nameValues = (*groupNameValues_.begin()).second;

    int ns = nameValues.size();

    if (ns > 1) {
      int is = 0;

      for (const auto &nameValue : nameValues) {
        const QString &name = nameValue.first;

        auto *colorItem = addKeyItem(-1, name, is, ns);

        //--

        if (colorColumn().isValid()) {
          const Values &values = nameValue.second.values;

          int nv = values.size();

          if (nv > 0) {
            const ValueData &valuePoint = values[0];

            CQChartsColor color;

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
        const QString &name = nameValues.begin()->first;

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

  CQChartsGeom::BBox bbox;

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
  drawAxis(device);

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
postDrawObjs(CQChartsPaintDevice *) const
{
}

//------

CQChartsScatterPoint3DObj::
CQChartsScatterPoint3DObj(const CQChartsScatterPlot3D *plot, int groupInd,
                          const CQChartsGeom::BBox &rect, const Point3D &pos,
                          const ColorInd &is, const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsScatterPlot3D *>(plot), rect, is, ig, iv),
 plot_(plot), groupInd_(groupInd), pos_(pos)
{
  setDetailHint(DetailHint::MAJOR);
}

//---

CQChartsSymbol
CQChartsScatterPoint3DObj::
symbolType() const
{
  auto symbolType = extraData().symbolType;

  if (! symbolType.isValid())
    symbolType = plot_->symbolType();

  return symbolType;
}

CQChartsLength
CQChartsScatterPoint3DObj::
symbolSize() const
{
  auto symbolSize = extraData().symbolSize;

  if (! symbolSize.isValid())
    symbolSize = plot()->symbolSize();

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
    QString groupName = plot_->groupIndName(groupInd_);

    tableTip.addTableRow("Group", groupName);
  }

  //---

  // add x, y columns
  QString xstr = plot()->xStr(pos_.x);
  QString ystr = plot()->yStr(pos_.y);
  QString zstr = plot()->yStr(pos_.z);

  tableTip.addTableRow(plot_->xHeaderName(), xstr);
  tableTip.addTableRow(plot_->yHeaderName(), ystr);
  tableTip.addTableRow(plot_->zHeaderName(), zstr);

  //---

  // get values for name (grouped id identical names)
  CQChartsScatterPlot3D::ValueData valuePoint;

  auto pg = plot_->groupNameValues().find(groupInd_);
  assert(pg != plot_->groupNameValues().end());

  auto p = (*pg).second.find(name_);

  if (p != (*pg).second.end()) {
    const auto &values = (*p).second.values;

    valuePoint = values[iv_.i];
  }

  //---

  auto addColumnRowValue = [&](const CQChartsColumn &column) {
    if (! column.isValid()) return;

    CQChartsModelIndex columnInd(modelInd().row(), column, modelInd().parent());

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
inside(const CQChartsGeom::Point &p) const
{
  double sx, sy;

  plot_->pixelSymbolSize(this->symbolSize(), sx, sy);

  CQChartsCamera *camera = plot_->camera();

  auto pt = camera->transform(point());

  auto p1 = plot_->windowToPixel(CQChartsGeom::Point(pt.x, pt.y));

  CQChartsGeom::BBox pbbox(p1.x - sx, p1.y - sy, p1.x + sx, p1.y + sy);

  auto pp = plot_->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsScatterPoint3DObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->xColumn());
  addColumnSelectIndex(inds, plot_->yColumn());
  addColumnSelectIndex(inds, plot_->zColumn());

  addColumnSelectIndex(inds, plot_->colorColumn());
}

//---

void
CQChartsScatterPoint3DObj::
draw(CQChartsPaintDevice *device)
{
  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // get symbol type and size
  CQChartsSymbol symbolType = this->symbolType();
  CQChartsLength symbolSize = this->symbolSize();

  double sx, sy;

  plot_->pixelSymbolSize(symbolSize, sx, sy);

  //---

  // get point
  CQChartsCamera *camera = plot_->camera();

  auto pt = camera->transform(point());

  auto ps = plot_->windowToPixel(CQChartsGeom::Point(pt.x, pt.y));

  //---

  // draw symbol
  auto ps1 = plot_->pixelToWindow(ps);

  plot_->drawSymbol(device, ps1, symbolType, symbolSize, penBrush);

  device->resetColorNames();
}

void
CQChartsScatterPoint3DObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  ColorInd ic = calcColorInd();

  plot_->setSymbolPenBrush(penBrush, ic);

  // override symbol fill color for custom color
  auto color = this->color();

  if (color.isValid()) {
    QColor c = plot_->interpColor(color, ic);

    c.setAlphaF(plot_->symbolFillAlpha().value());

    penBrush.brush.setColor(c);
  }

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::SYMBOL);
}

//------

CQChartsScatterKeyColor3D::
CQChartsScatterKeyColor3D(CQChartsScatterPlot3D *plot, int groupInd, const ColorInd &ic) :
 CQChartsKeyColorBox(plot, ColorInd(), ColorInd(), ic), groupInd_(groupInd)
{
}

bool
CQChartsScatterKeyColor3D::
selectPress(const CQChartsGeom::Point &, CQChartsSelMod selMod)
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
