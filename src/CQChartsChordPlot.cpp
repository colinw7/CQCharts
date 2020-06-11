#include <CQChartsChordPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsRotatedTextBoxObj.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>
#include <CQChartsNamePair.h>
#include <CQChartsConnectionList.h>
#include <CQChartsTip.h>
#include <CQChartsValueSet.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

//---

CQChartsChordPlotType::
CQChartsChordPlotType()
{
}

void
CQChartsChordPlotType::
addParameters()
{
  CQChartsConnectionPlotType::addParameters();
}

QString
CQChartsChordPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Chord Plot").
    h3("Summary").
     p("Draw connections using radial plot with sized path arcs.").
     p("The size of each arc is equivalent to the number of connections from that section.").
    h3("Columns").
     p("The link column specifies the node name and index (row) and the group column "
       "specifies the parent group. The remaining columns contain the connection value for "
       "each connected node i.e. there should be N rows and N + 1 or N + 2 columns depending "
       "on whether a group is specified").
    h3("Options").
     p("The inner radius (0-1) can be specified to adjust the width of the ring and connection "
       "area. The radius for the label can be specified ((0-1) inside, >1 outside) and the "
       "nodes can be sorted by value or use the original model order").
    h3("Customization").
     p("The stroke style, segment fill alpha, and arc fill alpha can be specified. The start angle "
       "and gap between nodes (in degress) can be specified. The label text style can be "
       "specified.").
    h3("Limitations").
     p("A user defined range cannot be specified, no axes or key is supported, logarithmic "
       "values are not allowed and probing is not available.").
    h3("Example").
     p(IMG("images/chord_plot.png"));
}

bool
CQChartsChordPlotType::
isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                     CQChartsPlotParameter *parameter) const
{
  return CQChartsConnectionPlotType::isColumnForParameter(columnDetails, parameter);
}

void
CQChartsChordPlotType::
analyzeModel(CQChartsModelData *modelData, CQChartsAnalyzeModelData &analyzeModelData)
{
  CQChartsConnectionPlotType::analyzeModel(modelData, analyzeModelData);
}

CQChartsPlot *
CQChartsChordPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsChordPlot(view, model);
}

//------

CQChartsChordPlot::
CQChartsChordPlot(CQChartsView *view, const ModelP &model) :
 CQChartsConnectionPlot(view, view->charts()->plotType("chord"), model),
 CQChartsObjStrokeData<CQChartsChordPlot>(this)
{
  NoUpdate noUpdate(this);

  textBox_ = new CQChartsRotatedTextBoxObj(this);

  setLayerActive(CQChartsLayer::Type::FG_PLOT, true);

  setStrokeAlpha(CQChartsAlpha(0.3));

  addTitle();
}

CQChartsChordPlot::
~CQChartsChordPlot()
{
  delete textBox_;
}

//---

void
CQChartsChordPlot::
setInnerRadius(double r)
{
  CQChartsUtil::testAndSet(innerRadius_, r, [&]() { drawObjs(); } );
}

void
CQChartsChordPlot::
setLabelRadius(double r)
{
  CQChartsUtil::testAndSet(labelRadius_, r, [&]() { drawObjs(); } );
}

void
CQChartsChordPlot::
setRotatedText(bool b)
{
  CQChartsUtil::testAndSet(rotatedText_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsChordPlot::
setSegmentAlpha(const CQChartsAlpha &a)
{
  CQChartsUtil::testAndSet(segmentAlpha_, a, [&]() { drawObjs(); } );
}

void
CQChartsChordPlot::
setArcAlpha(const CQChartsAlpha &a)
{
  CQChartsUtil::testAndSet(arcAlpha_, a, [&]() { drawObjs(); } );
}

void
CQChartsChordPlot::
setGapAngle(const CQChartsAngle &a)
{
  CQChartsUtil::testAndSet(gapAngle_, a, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsChordPlot::
setStartAngle(const CQChartsAngle &a)
{
  CQChartsUtil::testAndSet(startAngle_, a, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsChordPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  auto addStyleProp = [&](const QString &path, const QString &name, const QString &alias,
                          const QString &desc) {
    auto *item = addProp(path, name, alias, desc);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  //---

  CQChartsConnectionPlot::addProperties();

  //---

  // options
  addProp("options", "innerRadius", "", "Radius of inside of outer strip (0-1)")->
    setMinValue(0.0).setMaxValue(1.0);

  // stroke
  addLineProperties("stroke", "stroke", "");

  // segment
  addStyleProp("segment/fill", "segmentAlpha", "alpha", "Alpha of segment fill");

  // arc
  addProp("arc", "gapAngle"  , "gapAngle"  , "Angle for gap between strip segements");
  addProp("arc", "startAngle", "startAngle", "Angle for first strip segment");

  addStyleProp("arc/fill", "arcAlpha", "alpha", "Alpha for arc fill");

  // labels
  textBox_->addTextDataProperties(propertyModel(), "labels", "Labels");

  addProp("labels", "labelRadius", "radius", "Radius for segment label (>= 1.0)")->
    setMinValue(1.0);

  addProp("labels", "rotatedText", "rotated", "Rotate labels to segment angle");

  QString labelBoxPath = "labels/box";

  textBox_->addBoxProperties(propertyModel(), labelBoxPath, "Labels");
}

CQChartsGeom::Range
CQChartsChordPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsChordPlot::calcRange");

  double lr = std::max(labelRadius(), 1.0);

  CQChartsGeom::Range dataRange;

  dataRange.updateRange(-lr, -lr);
  dataRange.updateRange( lr,  lr);

  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  return dataRange;
}

CQChartsGeom::BBox
CQChartsChordPlot::
calcAnnotationBBox() const
{
  CQPerfTrace trace("CQChartsChordPlot::calcAnnotationBBox");

  CQChartsGeom::BBox bbox;

  for (const auto &plotObj : plotObjs_) {
    auto *obj = dynamic_cast<CQChartsChordArcObj *>(plotObj);
    if (! obj) continue;

    bbox += obj->textBBox();
  }

  return bbox;
}

//------

bool
CQChartsChordPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsChordPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  auto *th = const_cast<CQChartsChordPlot *>(this);

  //---

  // check columns
  if (! checkColumns())
    return false;

  //---

  th->groupValues_.setPlot(this);

  th->groupValues_.clear();

  if (groupColumn().isValid())
    addColumnValues(groupColumn(), th->groupValues_);

  //---

  // create objects
  bool rc = true;

  if (isHierarchical())
    rc = initHierObjs(objs);
  else {
    if      (linkColumn().isValid() && valueColumn().isValid())
      rc = initLinkObjs(objs);
    else if (connectionsColumn().isValid())
      rc = initConnectionObjs(objs);
    else if (pathColumn().isValid())
      rc = initPathObjs(objs);
    else
      rc = initTableObjs(objs);
  }

  if (! rc)
    return false;

  //---

  return true;
}

bool
CQChartsChordPlot::
initHierObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsChordPlot::initHierObjs");

  //---

  auto *th = const_cast<CQChartsChordPlot *>(this);

  th->nameDataMap_.clear();

  //---

  CQChartsConnectionPlot::initHierObjs();

  //---

  th->addNameDataMap(nameDataMap_, objs);

  return true;
}

void
CQChartsChordPlot::
initHierObjsAddHierConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  initHierObjsConnection(srcHierData .parentStr, srcHierData .parentLinkInd, srcHierData .total,
                         destHierData.parentStr, destHierData.parentLinkInd, destHierData.total);
}

void
CQChartsChordPlot::
initHierObjsAddLeafConnection(const HierConnectionData &srcHierData,
                              const HierConnectionData &destHierData) const
{
  initHierObjsConnection(srcHierData .parentStr, srcHierData .parentLinkInd, srcHierData .total,
                         destHierData.parentStr, destHierData.parentLinkInd, destHierData.total);
}

void
CQChartsChordPlot::
initHierObjsConnection(const QString &srcStr, const CQChartsModelIndex &srcLinkInd,
                       double /*srcValue*/,
                       const QString &destStr, const CQChartsModelIndex &destLinkInd,
                       double destValue) const
{
  // find src (create if doesn't exist)
  QModelIndex srcModelIndex  = modelIndex(srcLinkInd);
  QModelIndex srcModelIndex1 = normalizeIndex(srcModelIndex);

  auto &srcData = findNameData(srcStr, srcModelIndex1);

  // find dest (create if doesn't exist)
  QModelIndex destModelIndex  = modelIndex(destLinkInd);
  QModelIndex destModelIndex1 = normalizeIndex(destModelIndex);

  auto &destData = findNameData(destStr, destModelIndex1);

  // create link from src to dest for values
  // (hier always symmetric)
  srcData .addValue(destData.from(), destValue, /*primary*/true );
  destData.addValue(srcData .from(), destValue, /*primary*/false);
}

//---

bool
CQChartsChordPlot::
initPathObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsChordPlot::initPathObjs");

  //---

  auto *th = const_cast<CQChartsChordPlot *>(this);

  th->nameDataMap_.clear();

  th->maxNodeDepth_ = 0;

  //--

  CQChartsConnectionPlot::initPathObjs();

  //---

  if (isPropagate())
    th->propagatePathValues();

  //---

  th->filterPathObjs();

  //---

  th->addNameDataMap(nameDataMap_, objs);

  return true;
}

void
CQChartsChordPlot::
addPathValue(const QStringList &pathStrs, double value) const
{
  int n = pathStrs.length();
  assert(n > 0);

  auto *th = const_cast<CQChartsChordPlot *>(this);

  th->maxNodeDepth_ = std::max(maxNodeDepth_, n - 1);

  QChar separator = (this->separator().length() ? this->separator()[0] : '/');

  QString path1 = pathStrs[0];

  for (int i = 1; i < n; ++i) {
    QString path2 = path1 + separator + pathStrs[i];

    auto &srcData  = findNameData(path1, QModelIndex());
    auto &destData = findNameData(path2, QModelIndex());

    srcData .setLabel(pathStrs[i - 1]);
    destData.setLabel(pathStrs[i    ]);

    srcData .setDepth(i - 1);
    destData.setDepth(i);

    if (i < n - 1) {
      if (! srcData.hasTo(destData.from())) {
        srcData .addValue(destData.from(), value, /*primary*/true );
        destData.addValue(srcData .from(), value, /*primary*/false);

        destData.setParent(srcData.from());
      }
    }
    else {
      srcData .addValue(destData.from(), value, /*primary*/true );
      destData.addValue(srcData .from(), value, /*primary*/false);

      destData.setParent(srcData.from());
      destData.setValue (OptReal(value));
    }

    path1 = path2;
  }
}

void
CQChartsChordPlot::
propagatePathValues()
{
  // propagate node value up through edges and parent nodes
  for (int depth = maxNodeDepth_; depth >= 0; --depth) {
    for (auto &p : nameDataMap_) {
      auto &chordData = p.second;
      if (chordData.depth() != depth) continue;

      ChordData::Group group("", depth, maxNodeDepth_ + 1);

      chordData.setGroup(group);

      // set node value from sum of dest values
      if (! chordData.hasValue()) {
        if (! chordData.values().empty()) {
          OptReal sum;

          for (const auto &value : chordData.values()) {
            if (value.value.isSet()) {
              double v = value.value.real();

              if (sum.isSet())
                sum = OptReal(sum.real() + v);
              else
                sum = OptReal(v);
            }
          }

          if (sum.isSet())
            chordData.setValue(sum);
        }
      }

      // propagate set node value up to source nodes
      if (chordData.hasValue()) {
        int parent = chordData.parent();

        if (parent > 0) {
          auto &parentChordData = getIndData(parent);

          parentChordData.setToValue(chordData      .from(), chordData.value().real());
          chordData      .setToValue(parentChordData.from(), chordData.value().real());
        }
      }
    }
  }
}

void
CQChartsChordPlot::
filterPathObjs()
{
  // hide nodes below depth
  if (maxDepth() > 0) {
    for (auto &p : nameDataMap_) {
      auto &chordData = p.second;

      if (chordData.depth() > maxDepth())
        chordData.setVisible(false);
    }
  }

  // hide nodes less than min value
  if (minValue() > 0) {
    for (auto &p : nameDataMap_) {
      auto &chordData = p.second;

      if (! chordData.value().isSet() || chordData.value().real() < minValue())
        chordData.setVisible(false);
    }
  }
}

//---

bool
CQChartsChordPlot::
initLinkObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsChordPlot::initLinkObjs");

  //---

  auto *th = const_cast<CQChartsChordPlot *>(this);

  th->nameDataMap_.clear();

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsChordPlot *plot) :
     plot_(plot) {
      separator_ = (plot_->separator().length() ? plot_->separator()[0] : '/');
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // Get group value
      ChordData::Group group;

      if (plot_->groupColumn().isValid()) {
        CQChartsModelIndex groupModelInd(data.row, plot_->groupColumn(), data.parent);

        bool ok1;
        QVariant groupVar = plot_->modelValue(groupModelInd, ok1);

        if (! ok1)
          return addDataError(groupModelInd, "Invalid group value");

        group = plot_->getChordGroup(groupVar);
      }

      //---

      // Get link value
      CQChartsModelIndex linkModelInd(data.row, plot_->linkColumn(), data.parent);

      CQChartsNamePair namePair;

      if (plot_->linkColumnType() == ColumnType::NAME_PAIR) {
        bool ok;
        QVariant linkVar = plot_->modelValue(linkModelInd, ok);
        if (! ok) return addDataError(linkModelInd, "Invalid Link");

        namePair = linkVar.value<CQChartsNamePair>();
      }
      else {
        bool ok;
        QString linkStr = plot_->modelString(linkModelInd , ok);
        if (! ok) return addDataError(linkModelInd, "Invalid Link");

        namePair = CQChartsNamePair(linkStr, separator_);
      }

      if (! namePair.isValid())
        return addDataError(linkModelInd, "Invalid Link");

      QModelIndex linkInd  = plot_->modelIndex(linkModelInd);
      QModelIndex linkInd1 = plot_->normalizeIndex(linkInd);

      //---

      // Get value value
      CQChartsModelIndex valueModelInd(data.row, plot_->valueColumn(), data.parent);

      bool ok1;
      double value = plot_->modelReal(valueModelInd, ok1);
      if (! ok1) return addDataError(valueModelInd, "Invalid Value");

      //---

      QString srcStr  = namePair.name1();
      QString destStr = namePair.name2();

      addConnection(srcStr, destStr, linkInd1, group, value);

      return State::OK;
    }

   private:
    void addConnection(const QString &srcStr, const QString &destStr,
                       const QModelIndex &linkInd, const ChordData::Group &group,
                       double value) {
      // find src (create if doesn't exist)
      auto &srcData = plot_->findNameData(srcStr, linkInd);

      // find dest (create if doesn't exist)
      auto &destData = plot_->findNameData(destStr, linkInd);

      // create link from src to dest for value
      srcData .addValue(destData.from(), value, /*primary*/true );
      destData.addValue(srcData .from(), value, /*primary*/false);

      //---

      // set group if specified
      if (group.isValid())
        srcData.setGroup(group);
    }

    State addDataError(const CQChartsModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsChordPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsChordPlot* plot_ { nullptr };
    QChar                    separator_ { '/' };
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  th->addNameDataMap(nameDataMap_, objs);

  return true;
}

bool
CQChartsChordPlot::
initConnectionObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsChordPlot::initConnectionObjs");

  //---

  auto *th = const_cast<CQChartsChordPlot *>(this);

  //---

  using Connections = CQChartsConnectionList::Connections;

  struct ConnectionsData {
    QModelIndex      ind;
    int              node  { 0 };
    QString          name;
    ChordData::Group group;
    double           total { 0.0 };
    Connections      connections;
  };

  using IdConnectionsData = std::map<int,ConnectionsData>;

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsChordPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // get group value
      ChordData::Group group;

      if (plot_->groupColumn().isValid()) {
        CQChartsModelIndex groupModelInd(data.row, plot_->groupColumn(), data.parent);

        bool ok1;
        QVariant groupVar = plot_->modelValue(groupModelInd, ok1);

        if (! ok1)
          return addDataError(groupModelInd, "Invalid group value");

        group = plot_->getChordGroup(groupVar);
      }

      //---

      // get optional node id (default to row)
      CQChartsModelIndex nodeModelInd;

      int id = data.row;

      if (plot_->nodeColumn().isValid()) {
        nodeModelInd = CQChartsModelIndex(data.row, plot_->nodeColumn(), data.parent);

        bool ok2;
        id = (int) plot_->modelInteger(nodeModelInd, ok2);
        if (! ok2) return addDataError(nodeModelInd, "Non-integer node value");
      }

      //---

      // get connections
      ConnectionsData connectionsData;

      CQChartsModelIndex connectionsModelInd(data.row, plot_->connectionsColumn(), data.parent);

      if (plot_->connectionsColumnType() == ColumnType::CONNECTION_LIST) {
        bool ok3;
        QVariant connectionsVar = plot_->modelValue(connectionsModelInd, ok3);

        connectionsData.connections = connectionsVar.value<CQChartsConnectionList>().connections();
      }
      else {
        bool ok3;
        QString connectionsStr = plot_->modelString(connectionsModelInd, ok3);
        if (! ok3) return addDataError(connectionsModelInd, "Invalid connection string");

        CQChartsConnectionList::stringToConnections(connectionsStr, connectionsData.connections);
      }

      //----

      // get name
      QString name = QString("%1").arg(id);

      if (plot_->nameColumn().isValid()) {
        CQChartsModelIndex nameModelInd(data.row, plot_->nameColumn(), data.parent);

        bool ok4;
        name = plot_->modelString(nameModelInd, ok4);
        if (! ok4) return addDataError(nameModelInd, "Invalid name string");
      }

      //---

      // calc total
      double total = 0.0;

      for (const auto &connection : connectionsData.connections)
        total += connection.value;

      //---

      // return connections data
      if (nodeModelInd.isValid()) {
        auto nodeInd  = plot_->modelIndex(nodeModelInd);
        auto nodeInd1 = plot_->normalizeIndex(nodeInd);

        connectionsData.ind = nodeInd1;
      }

      connectionsData.node  = id;
      connectionsData.name  = name;
      connectionsData.group = group;
      connectionsData.total = total;

      idConnectionsData_[connectionsData.node] = connectionsData;

      return State::OK;
    }

    const IdConnectionsData &idConnectionsData() const { return idConnectionsData_; }

   private:
    State addDataError(const CQChartsModelIndex &ind, const QString &msg) const {
      const_cast<CQChartsChordPlot *>(plot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const CQChartsChordPlot* plot_ { nullptr };
    IdConnectionsData        idConnectionsData_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  //---

  NameDataMap nameDataMap;

  const IdConnectionsData &idConnectionsData = visitor.idConnectionsData();

  for (const auto &idConnections : idConnectionsData) {
    int         id              = idConnections.first;
    const auto &connectionsData = idConnections.second;

    QString srcStr = QString("%1").arg(id);

    // find src (create if doesn't exist)
    auto &srcData = findNameData(nameDataMap, srcStr, connectionsData.ind);

    srcData.setName (connectionsData.name );
    srcData.setGroup(connectionsData.group);

    for (const auto &connection : connectionsData.connections) {
      QString destStr = QString("%1").arg(connection.node);

      auto &destData = findNameData(nameDataMap, destStr, connectionsData.ind);

      // create link from src to dest for value
      srcData.addValue(destData.from(), connection.value, connection.value);
    }
  }

  //---

  th->addNameDataMap(nameDataMap, objs);

  return true;
}

bool
CQChartsChordPlot::
initTableObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsChordPlot::initTableObjs");

  //---

  TableConnectionDatas tableConnectionDatas;
  TableConnectionInfo  tableConnectionInfo;

  if (! processTableModel(tableConnectionDatas, tableConnectionInfo))
    return false;

  int nv  = tableConnectionDatas.size();
  int nv1 = tableConnectionInfo.numNonZero;

  double total = tableConnectionInfo.total;

  //---

  using ChordDatas = std::vector<ChordData>;

  ChordDatas datas;

  datas.resize(nv);

  for (int row = 0; row < nv; ++row) {
    auto &tableConnectionData = const_cast<TableConnectionData &>(tableConnectionDatas[row]);

    datas[row].setData(tableConnectionData);
  }

  //---

  // 360 degree circle, minus gap angle degrees per set
  double gap = std::max(this->gapAngle().value(), 0.0);

  double drange = 360 - nv1*gap;

  while (drange <= 180) {
    gap /= 2.0;

    drange = 360 - nv1*gap;
  }

  // divide remaining degrees by total to get value->degrees factor
  auto *th = const_cast<CQChartsChordPlot *>(this);

  th->valueToDegrees_ = drange/total;

  //---

  double angle1 = this->startAngle().value();

  for (int row = 0; row < nv; ++row) {
    ChordData &data = datas[row];

    double total1;

    if (! isSymmetric())
      total1 = data.maxTotal();
    else
      total1 = data.total();

    if (CMathUtil::isZero(total1))
      continue;

    double dangle = valueToDegrees(total1);

    double angle2 = angle1 + dangle;

    data.setAngles(CQChartsAngle(angle1), CQChartsAngle(dangle));

    angle1 = angle2 + gap;
  }

  //---

  th->arcObjs_.clear();

  for (int row = 0; row < nv; ++row) {
    const ChordData &data = datas[row];

    CQChartsGeom::BBox rect(-1, -1, 1, 1);

    ColorInd ig;

    if (data.group().isValid())
      ig = ColorInd(data.group().i, data.group().n);

    ColorInd iv(row, nv);

    auto *arcObj = createArcObj(rect, data, ig, iv);

    objs.push_back(arcObj);

    th->arcObjs_.push_back(arcObj);

    //---

    int ind = 0;

    for (const auto &value : data.values()) {
      if (! value.primary)
        continue;

      auto *edgeObj = createEdgeObj(rect, data, value.to, value.value);

      objs.push_back(edgeObj);

      th->edgeObjs_.push_back(edgeObj);

      ++ind;
    }
  }

  return true;
}

CQChartsChordPlot::ChordData &
CQChartsChordPlot::
findNameData(const QString &name, const QModelIndex &linkInd) const
{
  auto *th = const_cast<CQChartsChordPlot *>(this);

  return findNameData(th->nameDataMap_, name, linkInd, /*global*/true);
}

CQChartsChordPlot::ChordData &
CQChartsChordPlot::
findNameData(NameDataMap &nameDataMap, const QString &name,
             const QModelIndex &linkInd, bool global) const
{
  auto p = nameDataMap.find(name);

  if (p == nameDataMap.end()) {
    p = nameDataMap.insert(p, NameDataMap::value_type(name, ChordData()));

    int ind = nameDataMap.size();

    (*p).second.setFrom(ind);
    (*p).second.setName(name);
    (*p).second.setLinkInd(linkInd);

    if (global) {
      auto *th = const_cast<CQChartsChordPlot *>(this);

      th->indName_[ind] = name;
    }
  }

  return (*p).second;
}

CQChartsChordPlot::ChordData &
CQChartsChordPlot::
getIndData(int ind)
{
  auto pi = indName_.find(ind);
  assert(pi != indName_.end());

  auto pn = nameDataMap_.find((*pi).second);
  assert(pn != nameDataMap_.end());

  return (*pn).second;
}

void
CQChartsChordPlot::
addNameDataMap(const NameDataMap &nameDataMap, PlotObjs &objs)
{
  using Datas = std::vector<ChordData>;

  Datas datas;

  double total = 0.0;

  for (const auto &nameData : nameDataMap) {
    const ChordData &data = nameData.second;
    if (! data.isVisible()) continue;

    double total1;

    if (! isSymmetric())
      total1 = data.maxTotal();
    else
      total1 = data.total();

    if (CMathUtil::isZero(total1))
      continue;

    datas.push_back(data);

    total += total1;
  }

  int nv = datas.size();

  //---

  if (isSorted()) {
    // sort by value/total
    std::sort(datas.begin(), datas.end(),
      [&](const ChordData &lhs, const ChordData &rhs) {
        if (lhs.group().value() != rhs.group().value())
          return lhs.group().value() < rhs.group().value();

        return lhs.total(/*primaryOnly*/ !isSymmetric()) <
               rhs.total(/*primaryOnly*/ !isSymmetric());
      });

    for (auto &data : datas)
      data.sort();
  }
  else {
    // sort by value/from
    std::sort(datas.begin(), datas.end(),
      [&](const ChordData &lhs, const ChordData &rhs) {
        if (lhs.group().value() != rhs.group().value())
          return lhs.group().value() < rhs.group().value();

        return lhs.from() < rhs.from();
      });

    for (auto &data : datas)
      data.sort();
  }

  //---

  // 360 degree circle, minus gap angle degrees per set
  double gap = std::max(this->gapAngle().value(), 0.0);

  double drange = 360 - nv*gap;

  while (drange <= 180) {
    gap /= 2.0;

    drange = 360 - nv*gap;
  }

  // divide remaining degrees by total to get value->degrees factor
  auto *th = const_cast<CQChartsChordPlot *>(this);

  th->valueToDegrees_ = drange/total;

  //---

  double angle1 = this->startAngle().value();

  for (int row = 0; row < nv; ++row) {
    ChordData &data = datas[row];

    double total1;

    if (! isSymmetric())
      total1 = data.maxTotal();
    else
      total1 = data.total();

    double dangle = valueToDegrees(total1);

    double angle2 = angle1 + dangle;

    data.setAngles(CQChartsAngle(angle1), CQChartsAngle(dangle));

    angle1 = angle2 + gap;
  }

  //---

  th->arcObjs_.clear();

  for (int row = 0; row < nv; ++row) {
    const ChordData &data = datas[row];

    CQChartsGeom::BBox rect(-1, -1, 1, 1);

    ColorInd ig;

    if (data.group().isValid())
      ig = ColorInd(data.group().i, data.group().n);

    ColorInd iv(row, nv);

    auto *arcObj = createArcObj(rect, data, ig, iv);

    objs.push_back(arcObj);

    th->arcObjs_.push_back(arcObj);

    //---

    int ind = 0;

    for (const auto &value : data.values()) {
      if (! value.primary)
        continue;

      auto *edgeObj = createEdgeObj(rect, data, value.to, value.value);

      objs.push_back(edgeObj);

      th->edgeObjs_.push_back(edgeObj);

      ++ind;
    }
  }
}

CQChartsChordPlot::ChordData::Group
CQChartsChordPlot::
getChordGroup(const QVariant &groupVar) const
{
  QString groupStr;

  CQChartsVariant::toString(groupVar, groupStr);

  int ig = groupValues_.iset(groupVar);
  int ng = groupValues_.numUnique();

  return ChordData::Group(groupStr, ig, ng);
}

//---

CQChartsChordArcObj *
CQChartsChordPlot::
arcObject(int ind) const
{
  for (const auto &obj : arcObjs_) {
    if (obj->from() == ind)
      return obj;
  }

  return nullptr;
}

CQChartsChordEdgeObj *
CQChartsChordPlot::
edgeObject(int from, int to) const
{
  for (const auto &obj : edgeObjs_) {
    if (obj->from() == from && obj->to() == to)
      return obj;
  }

  return nullptr;
}

//---

void
CQChartsChordPlot::
postResize()
{
  CQChartsPlot::postResize();

  resetDataRange(/*updateRange*/true, /*updateObjs*/false);
}

//---

void
CQChartsChordPlot::
write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
      const QString &viewVarName) const
{
  CQChartsPlot::write(os, plotVarName, modelVarName, viewVarName);

  textBox_->write(os, plotVarName);
}

//---

CQChartsChordArcObj *
CQChartsChordPlot::
createArcObj(const CQChartsGeom::BBox &rect, const ChordData &data,
             const ColorInd &ig, const ColorInd &iv) const
{
  return new CQChartsChordArcObj(this, rect, data, ig, iv);
}

CQChartsChordEdgeObj *
CQChartsChordPlot::
createEdgeObj(const CQChartsGeom::BBox &rect, const ChordData &data,
              int to, const OptReal &value) const
{
  return new CQChartsChordEdgeObj(this, rect, data, to, value);
}

//------

CQChartsChordArcObj::
CQChartsChordArcObj(const CQChartsChordPlot *plot, const CQChartsGeom::BBox &rect,
                    const ChordData &data, const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsChordPlot *>(plot), rect, ColorInd(), ig, iv),
 plot_(plot), data_(data)
{
  setDetailHint(DetailHint::MAJOR);

  if (data.linkInd().isValid())
    setModelInd(data.linkInd());
}

QString
CQChartsChordArcObj::
calcId() const
{
  if (data_.group().str != "")
    return QString("%1:%2:%3:%4").arg(typeName()).arg(dataName()).
             arg(data_.group().str).arg(iv_.i);
  else
    return QString("%1:%2:%3").arg(typeName()).arg(dataName()).arg(iv_.i);
}

QString
CQChartsChordArcObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", dataName());

  if (data_.group().str != "")
    tableTip.addTableRow("Group", data_.group().str);

  tableTip.addTableRow("Total", data_.total(/*primaryOnly*/! plot_->isSymmetric()));

  tableTip.addTableRow("Count", data_.values().size());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

bool
CQChartsChordArcObj::
inside(const CQChartsGeom::Point &p) const
{
  return arcData().inside(p);
}

CQChartsArcData
CQChartsChordArcObj::
arcData() const
{
  CQChartsArcData arcData;

  arcData.setInnerRadius(innerRadius());
  arcData.setOuterRadius(outerRadius());

  double a1 = data_.angle().value();
  double a2 = a1 + data_.dangle().value();

  arcData.setAngle1(CQChartsAngle(a1));
  arcData.setAngle2(CQChartsAngle(a2));

  return arcData;
}

//---

void
CQChartsChordArcObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->linkColumn ());
  addColumnSelectIndex(inds, plot_->groupColumn());
}

CQChartsChordArcObj::PlotObjs
CQChartsChordArcObj::
getConnected() const
{
  PlotObjs plotObjs;

  for (const auto &value : data_.values()) {
    if (! value.primary)
      continue;

    auto *toObj = plot_->arcObject(value.to);
    if (! toObj) continue;

    plotObjs.push_back(toObj);
  }

  return plotObjs;
}

//---

void
CQChartsChordArcObj::
draw(CQChartsPaintDevice *device)
{
  // calc inner outer arc rectangles
  double ri = innerRadius();
  double ro = outerRadius();

  CQChartsGeom::Point o1(-ro, -ro);
  CQChartsGeom::Point o2( ro,  ro);
  CQChartsGeom::Point i1(-ri, -ri);
  CQChartsGeom::Point i2( ri,  ri);

  CQChartsGeom::BBox obbox(o1, o2);
  CQChartsGeom::BBox ibbox(i1, i2);

  //---

  double angle1 = data_.angle().value();
  double dangle = data_.dangle().value();

  //---

  // calc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  // draw value set segment arc path
  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  CQChartsDrawUtil::drawArcSegment(device, ibbox, obbox, CQChartsAngle(angle1),
                                   CQChartsAngle(dangle));

  device->resetColorNames();

  //---

  if (plot_->view()->drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
    plot_->view()->setDrawLayerType(CQChartsLayer::Type::NONE);

    for (const auto &value : data_.values()) {
      if (! value.primary)
        continue;

      auto *toObj = plot_->edgeObject(data_.from(), value.to);
      if (! toObj) continue;

      toObj->setInside(true); toObj->draw(device); toObj->setInside(false);
    }

    plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER);
  }
}

void
CQChartsChordArcObj::
drawFg(CQChartsPaintDevice *device) const
{
  if (dataName() == "")
    return;

  if (! plot_->textBox()->isTextVisible())
    return;

  // get total (skip if zero)
  double total;

  if (! plot_->isSymmetric())
    total = data_.maxTotal();
  else
    total = data_.total();

  if (CMathUtil::isZero(total))
    return;

  // calc label radius
  double ri = innerRadius();
  double ro = outerRadius();

  double lr = std::max(plot_->labelRadius(), 1.0);

  double lr1 = ri + lr*(ro - ri);

  lr1 = std::max(lr1, 0.01);

  // calc label angle
  double angle1 = data_.angle().value();
  double dangle = data_.dangle().value();
  double angle2 = angle1 + dangle;

  double ta = CMathUtil::avg(angle1, angle2);

  //---

  // set connection line pen
  // TODO: separate text and line pen control
  ColorInd colorInd = calcColorInd();

  QPen lpen;

  QColor bg = plot_->interpPaletteColor(colorInd);

  plot_->setPen(lpen, true, bg, CQChartsAlpha());

  //---

  // draw text using line pen
  CQChartsGeom::Point center(0, 0);

  plot_->textBox()->drawConnectedRadialText(device, center, ro, lr1, ta, dataName(),
                                            lpen, plot_->isRotatedText());
}

void
CQChartsChordArcObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  // set fill and stroke
  // TODO: separate segment stroke/fill control
  QColor segmentStrokeColor = plot_->interpStrokeColor(ColorInd());

  QColor        fromColor = calcFromColor();
  CQChartsAlpha fromAlpha;

  if (! isInside() && ! isSelected())
    fromAlpha = plot_->segmentAlpha();

  plot_->setPenBrush(penBrush,
    CQChartsPenData  (true, segmentStrokeColor, plot_->strokeAlpha(),
                      plot_->strokeWidth(), plot_->strokeDash()),
    CQChartsBrushData(true, fromColor, fromAlpha));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

QColor
CQChartsChordArcObj::
calcFromColor() const
{
  ColorInd colorInd = calcColorInd();

  if (plot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    double gval = data_.group().value();

    if (gval >= 0.0)
      return plot_->blendGroupPaletteColor(gval, iv_.value(), 0.1);
    else
      return plot_->interpPaletteColor(colorInd);
  }
  else
    return plot_->interpPaletteColor(colorInd);
}

void
CQChartsChordArcObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);

  std::ostream &os = device->os();

  os << "\n";
  os << "  this.total = " << data_.total(/*primaryOnly*/! plot_->isSymmetric()) << ";\n";
}

CQChartsGeom::BBox
CQChartsChordArcObj::
textBBox() const
{
  if (! dataName().length())
    return CQChartsGeom::BBox();

  //---

  double ri = innerRadius();
  double ro = outerRadius();

  //---

  double angle1 = data_.angle().value();
  double dangle = data_.dangle().value();
  double angle2 = angle1 + dangle;

  //---

  if (! plot_->textBox()->isTextVisible())
    return CQChartsGeom::BBox();

  double total = data_.total(/*primaryOnly*/! plot_->isSymmetric());

  if (CMathUtil::isZero(total))
    return CQChartsGeom::BBox();

  //---

  // calc box of text on arc center line
  double lr = std::max(plot_->labelRadius(), 1.0);

  double ta = CMathUtil::avg(angle1, angle2);

  CQChartsGeom::Point center(0, 0);

  double lr1 = ri + lr*(ro - ri);

  lr1 = std::max(lr1, 0.01);

  CQChartsGeom::BBox tbbox;

  plot_->textBox()->calcConnectedRadialTextBBox(center, ro, lr1, ta, dataName(),
                                                plot_->isRotatedText(), tbbox);

  return tbbox;
}

double
CQChartsChordArcObj::
innerRadius() const
{
  return std::min(std::max(plot_->innerRadius(), 0.01), 1.0);
}

double
CQChartsChordArcObj::
outerRadius() const
{
  return 1.0;
}

void
CQChartsChordArcObj::
dataAngles(double &a, double &da) const
{
  a  = data_.angle ().value();
  da = data_.dangle().value();
}

void
CQChartsChordArcObj::
valueAngles(int ind, double &a, double &da, ChordData::PrimaryType primaryType) const
{
  a = data_.angle().value();

  for (const auto &value : data_.values()) {
    if (primaryType == ChordData::PrimaryType::PRIMARY && ! value.primary)
      continue;

    if (primaryType == ChordData::PrimaryType::NON_PRIMARY && value.primary)
      continue;

    da = plot_->valueToDegrees(value.value.realOr(0.0));

    if (ind == value.to)
      return;

    a += da;
  }

  da = 0.0;
}

//------

CQChartsChordEdgeObj::
CQChartsChordEdgeObj(const CQChartsChordPlot *plot, const CQChartsGeom::BBox &rect,
                     const ChordData &data, int to, const OptReal &value) :
 CQChartsPlotObj(const_cast<CQChartsChordPlot *>(plot), rect, ColorInd(), ColorInd(), ColorInd()),
 plot_(plot), data_(data), to_(to), value_(value)
{
  setDetailHint(DetailHint::MAJOR);
}

QString
CQChartsChordEdgeObj::
calcId() const
{
  auto *fromObj = this->fromObj();
  auto *toObj   = this->toObj  ();

  QString fromName = (fromObj ? fromObj->dataName() : QString("%1").arg(from()));
  QString toName   = (toObj   ? toObj  ->dataName() : QString("%1").arg(to  ()));

  return QString("%1:%2").arg(fromName).arg(toName);
}

QString
CQChartsChordEdgeObj::
calcTipId() const
{
  auto *fromObj = this->fromObj();
  auto *toObj   = this->toObj  ();

  CQChartsTableTip tableTip;

  if (fromObj)
    tableTip.addTableRow("From", fromObj->dataName());

  if (toObj)
    tableTip.addTableRow("To", toObj->dataName());

  if (value_.isSet())
    tableTip.addTableRow("Value", value_.real());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

bool
CQChartsChordEdgeObj::
inside(const CQChartsGeom::Point &p) const
{
  return path_.contains(p.qpoint());
}

//---

void
CQChartsChordEdgeObj::
getObjSelectIndices(Indices &) const
{
}

CQChartsChordEdgeObj::PlotObjs
CQChartsChordEdgeObj::
getConnected() const
{
  PlotObjs plotObjs;

  auto *fromObj = this->fromObj();
  auto *toObj   = this->toObj  ();

  if (fromObj)
    plotObjs.push_back(fromObj);

  if (toObj)
    plotObjs.push_back(toObj);

  return plotObjs;
}

//---

void
CQChartsChordEdgeObj::
draw(CQChartsPaintDevice *device)
{
  auto *fromObj = this->fromObj();
  auto *toObj   = this->toObj  ();

  if (! fromObj || ! toObj)
    return;

  //---

  // calc inner outer arc rectangles
  double ri = fromObj->innerRadius();

  CQChartsGeom::Point i1(-ri, -ri);
  CQChartsGeom::Point i2( ri,  ri);

  CQChartsGeom::BBox ibbox(i1, i2);

  //---

  // get from/to angles
  double a1, da1, a2, da2;

  if (! plot_->isSymmetric()) {
    fromObj->valueAngles(to  (), a1, da1, ChordData::PrimaryType::PRIMARY    );
    toObj  ->valueAngles(from(), a2, da2, ChordData::PrimaryType::NON_PRIMARY);
  }
  else {
    fromObj->valueAngles(to  (), a1, da1);
    toObj  ->valueAngles(from(), a2, da2);
  }

  if (CMathUtil::isZero(da1))
    return;
//if (CMathUtil::isZero(da2))
//  return;

  //---

  // set arc pen and brush
  CQChartsPenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  //---

  // draw arcs between value sets
  bool isSelf = (from() == to());

  path_ = QPainterPath();

  CQChartsDrawUtil::arcsConnectorPath(path_, ibbox,
    CQChartsAngle(a1), CQChartsAngle(da1), CQChartsAngle(a2), CQChartsAngle(da2), isSelf);

  device->drawPath(path_);

  //---

  if (plot_->view()->drawLayerType() == CQChartsLayer::Type::MOUSE_OVER) {
    plot_->view()->setDrawLayerType(CQChartsLayer::Type::NONE);

    if (fromObj) { fromObj->setInside(true); fromObj->draw(device); fromObj->setInside(false); }
    if (toObj  ) { toObj  ->setInside(true); toObj  ->draw(device); toObj  ->setInside(false); }

    plot_->view()->setDrawLayerType(CQChartsLayer::Type::MOUSE_OVER);
  }
}

void
CQChartsChordEdgeObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  auto *fromObj = this->fromObj();
  auto *toObj   = this->toObj  ();

  if (! fromObj || ! toObj) return;

  //---

  // set pen and brush
  // TODO: separate arc stroke/fill control
  QColor arcStrokeColor = plot_->interpStrokeColor(ColorInd());

  QColor fromColor = fromObj->calcFromColor();
  QColor toColor   = toObj  ->calcFromColor();

  QColor fillColor = CQChartsUtil::blendColors(fromColor, toColor, 0.5);

  CQChartsAlpha fillAlpha;

  if (! isInside() && ! isSelected())
    fillAlpha = plot_->arcAlpha();

  plot_->setPenBrush(penBrush,
    CQChartsPenData  (true, arcStrokeColor, plot_->strokeAlpha(),
                      plot_->strokeWidth(), plot_->strokeDash()),
    CQChartsBrushData(true, fillColor, fillAlpha));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

CQChartsChordArcObj *
CQChartsChordEdgeObj::
fromObj() const
{
  return plot_->arcObject(from());
}

CQChartsChordArcObj *
CQChartsChordEdgeObj::
toObj() const
{
  return plot_->arcObject(to());
}
