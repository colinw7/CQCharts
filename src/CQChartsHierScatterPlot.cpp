#include <CQChartsHierScatterPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsKey.h>
#include <CQChartsValueSet.h>
#include <CQChartsUtil.h>
#include <CQChartsTip.h>
#include <CQChartsDataLabel.h>
#include <CQCharts.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>

CQChartsHierScatterPlotType::
CQChartsHierScatterPlotType()
{
}

void
CQChartsHierScatterPlotType::
addParameters()
{
  startParameterGroup("Hier Scatter");

  addColumnParameter("x", "X", "xColumn").
    setPropPath("columns.x").setTip("X Value").setRequired().setNumeric();
  addColumnParameter("y", "Y", "yColumn").
    setPropPath("columns.y").setTip("Y Value").setRequired().setNumeric();

  addColumnParameter("name", "Name", "nameColumn").
    setPropPath("columns.name").setTip("Value Name").setString();

  addColumnsParameter("group", "Group", "groupColumns").
    setPropPath("columns.group").setTip("Group Name(s)");

  addBoolParameter("textLabels", "Text Labels", "textLabels").
   setTip("Show Text Label at Point");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsHierScatterPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Hierarchical Scatter Plot").
    h3("Summary").
     p("Draws scatter plot x, y points with support for customization of point symbol type, "
       "symbol size and symbol color.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/hierscatter.png"));
}

CQChartsPlot *
CQChartsHierScatterPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsHierScatterPlot(view, model);
}

//---

CQChartsHierScatterPlot::
CQChartsHierScatterPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("hierscatter"), model),
 CQChartsObjPointData<CQChartsHierScatterPlot>(this)
{
}

CQChartsHierScatterPlot::
~CQChartsHierScatterPlot()
{
  term();
}

//---

void
CQChartsHierScatterPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  //---

  dataLabel_ = new CQChartsDataLabel(this);

  //---

  setSymbolSize(Length("4px"));
  setSymbolType(Symbol(Symbol::Type::CIRCLE));
  setSymbolStroked(true);
  setSymbolFilled (true);
  setSymbolFillColor(Color(Color::Type::PALETTE));

  //---

  addAxes();

  addKey();

  addTitle();
}

void
CQChartsHierScatterPlot::
term()
{
  for (const auto &groupValueSet : groupValueSets_)
    delete groupValueSet.second;

  delete rootGroup_;

  delete dataLabel_;
}

//---

void
CQChartsHierScatterPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsHierScatterPlot::
setXColumn(const Column &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { resetAxes(); updateRangeAndObjs(); } );
}

void
CQChartsHierScatterPlot::
setYColumn(const Column &c)
{
  CQChartsUtil::testAndSet(yColumn_, c, [&]() { resetAxes(); updateRangeAndObjs(); } );
}

void
CQChartsHierScatterPlot::
setGroupColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(groupColumns_, c, [&]() {
    initGroupValueSets();

    updateRangeAndObjs();
  } );
}

//---

CQChartsColumn
CQChartsHierScatterPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "name") c = this->nameColumn();
  else if (name == "x"   ) c = this->xColumn();
  else if (name == "y"   ) c = this->yColumn();
  else                     c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsHierScatterPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "name") this->setNameColumn(c);
  else if (name == "x"   ) this->setXColumn(c);
  else if (name == "y"   ) this->setYColumn(c);
  else                     CQChartsPlot::setNamedColumn(name, c);
}

CQChartsColumns
CQChartsHierScatterPlot::
getNamedColumns(const QString &name) const
{
  Columns c;
  if (name == "group") c = this->groupColumns();
  else                 c = CQChartsPlot::getNamedColumns(name);

  return c;
}

void
CQChartsHierScatterPlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if (name == "group") this->setGroupColumns(c);
  else                 CQChartsPlot::setNamedColumns(name, c);
}

//---

bool
CQChartsHierScatterPlot::
isTextLabels() const
{
  return dataLabel_->isVisible();
}

void
CQChartsHierScatterPlot::
setTextLabels(bool b)
{
  if (b != isTextLabels()) { dataLabel_->setVisible(b); drawObjs(); }
}

//---

void
CQChartsHierScatterPlot::
popCurrentGroup()
{
  if (! currentGroup())
    return;

  auto *group = currentGroup()->parent();

  if (group)
    setCurrentGroup(group);
}

void
CQChartsHierScatterPlot::
resetCurrentGroup()
{
  setCurrentGroup(rootGroup_);
}

void
CQChartsHierScatterPlot::
setCurrentGroup(CQChartsHierScatterPointGroup *group)
{
  filterNames_.clear();

  auto *group1 = group;

  while (group1 && group1 != rootGroup_) {
    filterNames_.push_front(group1->name());

    group1 = group1->parent();
  }

  currentGroup_ = group;

  updateRangeAndObjs();

  //updateObjs();
}

//------

void
CQChartsHierScatterPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "xColumn", "x", "X column");
  addProp("columns", "yColumn", "y", "Y column");

  addProp("columns", "nameColumn"  , "name"  , "Name column");
  addProp("columns", "groupColumns", "groups", "Group columns");

  // symbl
  addSymbolProperties("symbol", "", "");

  // point data labels
  dataLabel_->addPathProperties("labels", "Labels");
}

void
CQChartsHierScatterPlot::
getPropertyNames(QStringList &names, bool hidden) const
{
  CQChartsPlot::getPropertyNames(names, hidden);

  propertyModel()->objectNames(dataLabel_, names, hidden);
}

//------

CQChartsGeom::Range
CQChartsHierScatterPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsHierScatterPlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsHierScatterPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  if (! checkColumn(xColumn(), "X", /*required*/true))
    columnsValid = false;
  if (! checkColumn(yColumn(), "Y", /*required*/true))
    columnsValid = false;

  if (! checkColumn (nameColumn  (), "Name" )) columnsValid = false;
  if (! checkColumns(groupColumns(), "Group")) columnsValid = false;

  if (! columnsValid)
    return Range(0.0, 0.0, 1.0, 1.0);

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsHierScatterPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      if (plot_->isInterrupt())
        return State::TERMINATE;

      //---

      // check filter
      if (! plot_->acceptsRow(data.row, data.parent))
        return State::SKIP;

      //---

      // get x, y value
      ModelIndex xModelInd(plot_, data.row, plot_->xColumn(), data.parent);
      ModelIndex yModelInd(plot_, data.row, plot_->yColumn(), data.parent);

      double x, y;

      bool ok1 = plot_->modelMappedReal(xModelInd, x, plot_->isLogX(), data.row);
      bool ok2 = plot_->modelMappedReal(yModelInd, y, plot_->isLogY(), data.row);

      if (! ok1) x = data.row;
      if (! ok2) y = data.row;

      if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
        return State::SKIP;

      //---

      range_.updateRange(x, y);

      return State::OK;
    }

    const Range &range() const { return range_; }

   private:
    const CQChartsHierScatterPlot* plot_ { nullptr };
    Range                          range_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  auto dataRange = visitor.range();

  if (isInterrupt())
    return dataRange;

  //---

  // update data range if unset
  dataRange.makeNonZero();

  //---

  return dataRange;
}

void
CQChartsHierScatterPlot::
postCalcRange()
{
  initAxes();
}

int
CQChartsHierScatterPlot::
acceptsRow(int row, const QModelIndex &parent) const
{
  auto *th = const_cast<CQChartsHierScatterPlot *>(this);

  int depth = filterNames_.size();

  for (int i = 0; i < depth; ++i) {
    const auto &column = groupValues_.getColumn(i);

    ModelIndex modelInd(th, row, column, parent);

    bool ok;

    auto name = modelString(modelInd, ok);

    if (! ok)
      return false;

    if (name != filterNames_[i])
      return false;
  }

  return true;
}

void
CQChartsHierScatterPlot::
resetAxes()
{
  xAxis_->setDefLabel("");
  yAxis_->setDefLabel("");
}

void
CQChartsHierScatterPlot::
initAxes()
{
  setXValueColumn(xColumn());
  setYValueColumn(yColumn());

  //---

  xAxis_->setColumn(xColumn());
  yAxis_->setColumn(yColumn());

  if (xAxis_->label().string() == "") {
    bool ok;

    auto xname = modelHHeaderString(xColumn(), ok);

    xAxis_->setDefLabel(xname);
  }

  if (yAxis_->label().string() == "") {
    bool ok;

    auto yname = modelHHeaderString(yColumn(), ok);

    yAxis_->setDefLabel(yname);
  }
}

//------

void
CQChartsHierScatterPlot::
initGroupValueSets()
{
  // init and populate group value sets
  groupValues_ = groupColumns();

  for (const auto &groupValueSet : groupValueSets_)
    delete groupValueSet.second;

  groupValueSets_.clear();

  if (groupValues_.count() == 0)
    return;

  //---

  for (const auto &groupColumn : groupValues_)
    groupValueSets_[groupColumn] = new CQChartsValueSet(this);

  //---

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsHierScatterPlot *plot) :
     plot_(plot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      plot_->addRowGroupValueSets(data);

      return State::OK;
    }

   private:
    const CQChartsHierScatterPlot* plot_ { nullptr };
  };

  RowVisitor visitor(this);

  visitModel(visitor);
}

void
CQChartsHierScatterPlot::
addRowGroupValueSets(const ModelVisitor::VisitData &data) const
{
  auto *th = const_cast<CQChartsHierScatterPlot *>(this);

  for (const auto &groupValueSet : groupValueSets_) {
    auto  groupColumn = groupValueSet.first;
    auto *valueSet    = groupValueSet.second;

    ModelIndex groupModelInd(th, data.row, groupColumn, data.parent);

    bool ok;

    auto value = modelValue(groupModelInd, ok);

    if (! ok)
      continue;

    valueSet->addValue(value);
  }
}

//---

void
CQChartsHierScatterPlot::
clearPlotObjects()
{
  delete rootGroup_;

  rootGroup_    = nullptr;
  currentGroup_ = nullptr;

  CQChartsPlot::clearPlotObjects();
}

bool
CQChartsHierScatterPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsHierScatterPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsHierScatterPlot *>(this);

  //---

  // init name values
  if (! rootGroup_) {
    th->rootGroup_    = new CQChartsHierScatterPointGroup(nullptr, 0);
    th->currentGroup_ = rootGroup_;

    //---

    class RowVisitor : public ModelVisitor {
     public:
      RowVisitor(const CQChartsHierScatterPlot *plot) :
       plot_(plot) {
      }

      State visit(const QAbstractItemModel *, const VisitData &data) override {
        if (! plot_->acceptsRow(data.row, data.parent))
          return State::SKIP;

        //---

        // get x, y value
        ModelIndex xModelInd(plot_, data.row, plot_->xColumn(), data.parent);
        ModelIndex yModelInd(plot_, data.row, plot_->yColumn(), data.parent);

        double x, y;

        bool ok1 = plot_->modelMappedReal(xModelInd, x, plot_->isLogX(), data.row);
        bool ok2 = plot_->modelMappedReal(yModelInd, y, plot_->isLogY(), data.row);

        if (! ok1) x = data.row;
        if (! ok2) y = data.row;

        if (CMathUtil::isNaN(x) || CMathUtil::isNaN(y))
          return State::SKIP;

        //---

        // get optional name
        bool ok;

        ModelIndex nameModelInd(plot_, data.row, plot_->nameColumn(), data.parent);

        auto name = plot_->modelString(nameModelInd, ok);

        //---

        plot_->addGroupPoint(data, x, y, name);

        return State::OK;
      }

     private:
      const CQChartsHierScatterPlot* plot_ { nullptr };
    };

    RowVisitor visitor(this);

    visitModel(visitor);
  }

  //---

  bool ok1, ok2;

  th->xname_ = modelHHeaderString(xColumn(), ok1);
  th->yname_ = modelHHeaderString(yColumn(), ok2);

  if (! xname_.length()) th->xname_ = "x";
  if (! yname_.length()) th->yname_ = "y";

  //---

  for (const auto &name : filterNames_) {
    auto *group1 = currentGroup()->lookupGroup(name);

    if (! group1)
      break;

    th->currentGroup_ = group1;
  }

  //---

  if (! currentGroup()->groups().empty()) {
    for (auto &igroup : currentGroup()->groups()) {
      auto *group = igroup.second;

      addGroupPoints(group, group, objs);
    }
  }
  else {
    addGroupPoints(currentGroup(), currentGroup(), objs);
  }

  //---

  return true;
}

void
CQChartsHierScatterPlot::
addGroupPoint(const ModelVisitor::VisitData &data, double x, double y, const QString &name) const
{
  // get point groups
  struct GroupData {
    Column            column;
    CQChartsValueSet* valueSet { nullptr };
    QString           str;
    int               ind      { -1 };
  };

  //---

  auto *th = const_cast<CQChartsHierScatterPlot *>(this);

  std::vector<GroupData> groupDatas;

  for (const auto &groupValue : groupValues_) {
    GroupData groupData;

    groupData.column = groupValue;

    auto pv = groupValueSets_.find(groupData.column);

    if (pv != groupValueSets_.end())
      groupData.valueSet = (*pv).second;

    ModelIndex groupModelInd(th, data.row, groupData.column, data.parent);

    bool ok3;

    groupData.str = modelString(groupModelInd, ok3);

    groupData.ind = groupData.valueSet->sind(groupData.str);

    groupDatas.push_back(groupData);
  }

  //---

  // get parent group
  auto *group = currentGroup();

  for (const auto &groupData : groupDatas) {
    auto *group1 = group->lookupGroup(groupData.ind);

    if (! group1) {
      group1 = group->addGroup(groupData.ind, groupData.str);
    }

    group = group1;
  }

  //---

  ModelIndex xModelInd(th, data.row, xColumn(), data.parent);

  auto xInd  = modelIndex(xModelInd);
  auto xInd1 = normalizeIndex(xInd);

  CQChartsHierScatterPoint point(group, x, y, name, data.row, xInd1);

  group->addPoint(point);
}

void
CQChartsHierScatterPlot::
addGroupPoints(CQChartsHierScatterPointGroup *baseGroup,
               CQChartsHierScatterPointGroup *group, PlotObjs &objs) const
{
  for (auto &igroup : group->groups()) {
    auto *group1 = igroup.second;

    addGroupPoints(baseGroup, group1, objs);
  }

  //---

  double sx, sy;

  plotSymbolSize(symbolSize(), sx, sy);

  //---

  int n = baseGroup->parentCount();
  int i = baseGroup->i();

  for (const auto &point : group->points()) {
    const auto &p = point.p;

    BBox bbox(p.x - sx, p.y - sy, p.x + sx, p.y + sy);

    auto iv = (i > 0 ? ColorInd(i, n) : ColorInd());

    auto *pointObj = createPointObj(bbox, p, iv);

    //---

    pointObj->setName    (point.name);
    pointObj->setModelInd(point.ind);
    pointObj->setGroup   (point.group);

    objs.push_back(pointObj);
  }
}

void
CQChartsHierScatterPlot::
addKeyItems(PlotKey *key)
{
  auto *group = currentGroup();

  int n = group->numGroups();

  for (const auto &igroup : group->groups()) {
    auto *group = igroup.second;

    int i = group->i();

    const auto &name = group->name();

    auto *colorItem = new CQChartsHierScatterKeyColor(this, group, ColorInd(i, n));
    auto *textItem  = new CQChartsKeyText(this, name, ColorInd(i, n));

    auto *groupItem = new CQChartsKeyItemGroup(this);

    groupItem->addItem(colorItem);
    groupItem->addItem(textItem );

    //key->addItem(colorItem, i, 0);
    //key->addItem(textItem , i, 1);

    key->addItem(groupItem, i, 0);
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

//---

bool
CQChartsHierScatterPlot::
probe(ProbeData &probeData) const
{
  CQChartsPlotObj *obj;

  if (! objNearestPoint(probeData.p, obj))
    return false;

  auto c = obj->rect().getCenter();

  probeData.p    = c;
  probeData.both = true;

  probeData.xvals.emplace_back(c.x, "", "");
  probeData.yvals.emplace_back(c.y, "", "");

  return true;
}

//---

bool
CQChartsHierScatterPlot::
addMenuItems(QMenu *menu)
{
  auto *popAction   = new QAction("Pop Filter"  , menu);
  auto *resetAction = new QAction("Reset Filter", menu);

  connect(popAction  , SIGNAL(triggered()), this, SLOT(popCurrentGroup()));
  connect(resetAction, SIGNAL(triggered()), this, SLOT(resetCurrentGroup()));

  popAction  ->setEnabled(currentGroup() != rootGroup_);
  resetAction->setEnabled(currentGroup() != rootGroup_);

  menu->addSeparator();

  menu->addAction(popAction  );
  menu->addAction(resetAction);

  return true;
}

//---

void
CQChartsHierScatterPlot::
write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
      const QString &viewVarName) const
{
  CQChartsPlot::write(os, plotVarName, modelVarName, viewVarName);

  dataLabel_->write(os, plotVarName);
}

//---

CQChartsHierScatterPointObj *
CQChartsHierScatterPlot::
createPointObj(const BBox &rect, const Point &p, const ColorInd &iv) const
{
  return new CQChartsHierScatterPointObj(this, rect, p, iv);
}

//---

CQChartsPlotCustomControls *
CQChartsHierScatterPlot::
createCustomControls(CQCharts *charts)
{
  auto *controls = new CQChartsHierScatterPlotCustomControls(charts);

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsHierScatterPointObj::
CQChartsHierScatterPointObj(const CQChartsHierScatterPlot *plot, const BBox &rect,
                            const Point &p, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsHierScatterPlot *>(plot), rect, ColorInd(), ColorInd(), iv),
 plot_(plot), p_(p)
{
}

QString
CQChartsHierScatterPointObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(iv_.i);
}

QString
CQChartsHierScatterPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Group", group()->name());
  tableTip.addTableRow("Name" , name());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

bool
CQChartsHierScatterPointObj::
inside(const Point &p) const
{
  double sx, sy;

  plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

  auto p1 = plot_->windowToPixel(p_);

  BBox pbbox(p1.x - sx, p1.y - sy, p1.x + sx, p1.y + sy);

  auto pp = plot_->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsHierScatterPointObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->xColumn());
  addColumnSelectIndex(inds, plot_->yColumn());
}

void
CQChartsHierScatterPointObj::
draw(PaintDevice *device) const
{
  auto ic = calcColorInd();

  //---

  // calc pen and brush
  PenBrush penBrush;

  auto fillColor   = plot_->interpColor(plot_->symbolFillColor  (), ic);
  auto strokeColor = plot_->interpColor(plot_->symbolStrokeColor(), ic);

  plot_->setPenBrush(penBrush,
    PenData  (true, strokeColor, plot_->symbolStrokeAlpha()),
    BrushData(true, fillColor  , plot_->symbolFillAlpha()));

  plot_->updateObjPenBrushState(this, penBrush, CQChartsPlot::DrawType::SYMBOL);

  //---

  // get symbol type and size
  auto symbolType = plot_->symbolType();
  auto symbolSize = plot_->symbolSize();

  // draw symbol
  CQChartsDrawUtil::drawSymbol(device, penBrush, symbolType, p_, symbolSize);

  //---

  // draw label
  if (plot_->isTextLabels()) {
    const auto *dataLabel = plot_->dataLabel();

    auto ps = plot_->windowToPixel(p_);

    double sx, sy;

    plot_->pixelSymbolSize(symbolSize, sx, sy);

    BBox ebbox(ps.x - sx, ps.y - sy, ps.x + sx, ps.y + sy);

    dataLabel->draw(device, plot_->pixelToWindow(ebbox), name_);
  }
}

//------

CQChartsHierScatterKeyColor::
CQChartsHierScatterKeyColor(CQChartsHierScatterPlot *plot, CQChartsHierScatterPointGroup *group,
                            const ColorInd &ic) :
 CQChartsKeyColorBox(plot, ColorInd(), ColorInd(), ic), group_(group)
{
}

bool
CQChartsHierScatterKeyColor::
selectPress(const Point &, CQChartsSelMod)
{
  auto *plot = qobject_cast<CQChartsHierScatterPlot *>(plot_);

  //plot->setSetHidden(ic_.i, ! plot->isSetHidden(ic_.i));

  //plot->updateObjs();

  if (group_->numGroups())
    plot->setCurrentGroup(group_);

  return true;
}

QBrush
CQChartsHierScatterKeyColor::
fillBrush() const
{
  auto c = CQChartsKeyColorBox::fillBrush().color();

  //auto *plot = qobject_cast<CQChartsHierScatterPlot *>(plot_);

  //if (plot->isSetHidden(ic_.i))
  //  c = CQChartsUtil::blendColors(c, key_->interpBgColor(), key_->hiddenAlpha());

  return c;
}

//------

CQChartsHierScatterPlotCustomControls::
CQChartsHierScatterPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "hierscatter")
{
  // options group
  auto optionsFrame = createGroupFrame("Options");

  //---

  addColumnWidgets(QStringList() << "x" << "y" << "name" << "group", optionsFrame);

  //---

  connectSlots(true);
}

void
CQChartsHierScatterPlotCustomControls::
connectSlots(bool b)
{
  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsHierScatterPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsHierScatterPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);
}

void
CQChartsHierScatterPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  CQChartsPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}
