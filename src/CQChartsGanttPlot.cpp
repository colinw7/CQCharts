#include <CQChartsGanttPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

CQChartsGanttPlotType::
CQChartsGanttPlotType()
{
}

void
CQChartsGanttPlotType::
addParameters()
{
  startParameterGroup("Gantt");

  // required start/end times
  addColumnParameter("start", "Start", "startColumn").
    setRequired().setPropPath("columns.start");
  addColumnParameter("end"  , "End"  , "endColumn"  ).
    setRequired().setPropPath("columns.end"  );

  // optional name, value
  addColumnParameter("name" , "Name" , "nameColumn" ).setPropPath("columns.name").setBasic();
  addColumnParameter("value", "Value", "valueColumn").setPropPath("columns.value").setBasic();

  endParameterGroup();

  //---

  // parent type data
  CQChartsGroupPlotType::addParameters();
}

QString
CQChartsGanttPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Gantt Plot").
    h3("Summary").
     p("Draws Gantt from start, end time data.").
    h3("Columns").
     p("The task time is specified with the " + B("Start") + " and " + B("End") + " columns.").
     p("The optional task name can be specified in the " + B("Name") + " column.").
     p("The optional task value can be specified in the " + B("Value") + " column.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/gantt.png"));
}

CQChartsPlot *
CQChartsGanttPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsGanttPlot(view, model);
}

//------

CQChartsGanttPlot::
CQChartsGanttPlot(View *view, const ModelP &model) :
 CQChartsGroupPlot(view, view->charts()->plotType("gantt"), model),
 CQChartsObjShapeData     <CQChartsGanttPlot>(this),
 CQChartsObjTextData      <CQChartsGanttPlot>(this),
 CQChartsObjGroupShapeData<CQChartsGanttPlot>(this),
 CQChartsObjGroupTextData <CQChartsGanttPlot>(this)
{
}

CQChartsGanttPlot::
~CQChartsGanttPlot()
{
  CQChartsGanttPlot::term();
}

//---

void
CQChartsGanttPlot::
init()
{
  CQChartsGroupPlot::init();

  //---

  NoUpdate noUpdate(this);

  setFillColor(Color::makePalette());

  setGroupFillColor(Color::makePalette());
  setGroupFillAlpha(Alpha(0.2));

  //---

  addAxes();

  addTitle();
}

void
CQChartsGanttPlot::
term()
{
}

//---

void
CQChartsGanttPlot::
setStartColumn(const Column &c)
{
  CQChartsUtil::testAndSet(startColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsGanttPlot::
setEndColumn(const Column &c)
{
  CQChartsUtil::testAndSet(endColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsGanttPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGanttPlot::
setValueColumn(const Column &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsGanttPlot::
setGroupStretch(bool b)
{
  CQChartsUtil::testAndSet(groupStretch_, b, [&]() { updateRangeAndObjs(); } );
}

//---

CQChartsColumn
CQChartsGanttPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "start") c = this->startColumn();
  else if (name == "end"  ) c = this->endColumn();
  else if (name == "name" ) c = this->nameColumn();
  else if (name == "value") c = this->valueColumn();
  else                      c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsGanttPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "start") this->setStartColumn(c);
  else if (name == "end"  ) this->setEndColumn(c);
  else if (name == "name" ) this->setNameColumn(c);
  else if (name == "value") this->setValueColumn(c);
  else                      CQChartsPlot::setNamedColumn(name, c);
}

//---

void
CQChartsGanttPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "startColumn", "start", "Start time column");
  addProp("columns", "endColumn"  , "end"  , "End time column");
  addProp("columns", "nameColumn" , "name" , "Name column");
  addProp("columns", "valueColumn", "value", "Value column");

  addGroupingProperties();

  //---

  // bar fill
  addProp("bar/fill", "filled", "visible", "Bar Fill visible");

  addFillProperties("bar/fill", "fill", "Bar");

  // bar stroke
  addProp("bar/stroke", "stroked", "visible", "Bar Stroke visible");

  addLineProperties("bar/stroke", "stroke", "Bar");

  addStyleProp("bar" , "cornerSize" , "cornerSize", "Bar corner size");

  // bar value labels
  addProp("bar/labels", "textVisible", "visible", "Value labels visible");

  addTextProperties("bar/labels/text", "text", "Value", CQChartsTextOptions::ValueType::ALL);

  //---

  addProp("group", "groupStretch", "stretch", "Stretch group");

  // group fill
  addProp("group/fill", "groupFilled", "visible", "Group Fill visible");

  addFillProperties("group/fill", "groupFill", "Group");

  // group stroke
  addProp("group/stroke", "groupStroked", "visible", "Group Stroke visible");

  addLineProperties("group/stroke", "groupStroke", "Group");

  addStyleProp("group" , "cornerSize" , "groupCornerSize", "Group corner size");

  // group value labels (none ?)
  addProp("group/labels", "groupTextVisible", "visible", "Group Value labels visible");

  addTextProperties("group/labels/text", "groupText", "Group Value",
                    CQChartsTextOptions::ValueType::ALL);
}

CQChartsGeom::Range
CQChartsGanttPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsGanttPlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsGanttPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  // start/end columns required
  // name, value columns optional

  if (! checkColumn(startColumn(), "Start", /*required*/true) ||
      ! checkColumn(startColumn(), "End"  , /*required*/true))
    columnsValid = false;

  if (! checkColumn(nameColumn(), "Name")) columnsValid = false;

  if (! checkColumn(valueColumn(), "Value")) columnsValid = false;

  if (! columnsValid)
    return Range();

  //---

  Range dataRange;

  //---

  // init grouping
  initGroupData(Columns(), nameColumn());

  //---

  // process model data
  // calc data range using start, end values
  class RowVisitor : public ModelVisitor {
   public:
    struct RowData {
      double startTime { 0.0 };
      double endTime   { 0.0 };
    };

    using RowDatas = std::map<int, RowData>;

   public:
    RowVisitor(const CQChartsGanttPlot *ganttPlot) :
     ganttPlot_(ganttPlot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ModelIndex startModelInd(ganttPlot_, data.row, ganttPlot_->startColumn(), data.parent);
      ModelIndex endModelInd  (ganttPlot_, data.row, ganttPlot_->endColumn  (), data.parent);

      RowData rowData;

      bool ok;
      rowData.startTime = ganttPlot_->modelReal(startModelInd, ok); if (! ok) return State::SKIP;
      rowData.endTime   = ganttPlot_->modelReal(endModelInd  , ok); if (! ok) return State::SKIP;

      rowDatas_[data.row] = rowData;

      if (! timeSet_) {
        timeMin_ = rowData.startTime;
        timeMax_ = rowData.endTime;

        timeSet_ = true;
      }
      else {
        timeMin_ = std::min(timeMin_, rowData.startTime);
        timeMax_ = std::max(timeMax_, rowData.endTime);
      }

      if (ganttPlot_->nameColumn().isValid()) {
        ModelIndex nameModelInd(ganttPlot_, data.row, ganttPlot_->nameColumn(), data.parent);

        int ig = ganttPlot_->rowGroupInd(nameModelInd);
        if (ig < 0) ig = 0;

        groupInds_[ig].push_back(rowDatas_.size());
      }

      return State::OK;
    }

    void setGroupLabels() {
      ganttPlot_->yAxis()->clearTickLabels();

      for (const auto &ig : groupInds_) {
        int i1 = *ig.second.begin();
        int i2 = *ig.second.rbegin();

        int im = (i1 + i2)/2;

        for (int i = i1; i <= i2; ++i) {
          auto label = (i == im ? ganttPlot_->groupIndName(ig.first) : "");

          ganttPlot_->yAxis()->setTickLabel(i, label);
        }
      }
    }

    const RowDatas &rowDatas() const { return rowDatas_; }

    double timeMin() const { return timeMin_; }
    double timeMax() const { return timeMax_; }

   private:
    using Inds      = std::vector<int>;
    using GroupInds = std::map<int, Inds>;

    const CQChartsGanttPlot* ganttPlot_ { nullptr };
    RowDatas                 rowDatas_;
    GroupInds                groupInds_;
    bool                     timeSet_   { false };
    double                   timeMin_   { 0.0 };
    double                   timeMax_   { 0.0 };
  };

  //---

  RowVisitor visitor(this);

  visitModel(visitor);

  visitor.setGroupLabels();

  //---

  // set value range
  const auto &rowDatas = visitor.rowDatas();

  double xmin { 0.0 }, ymin { 0.0 }, xmax { 1.0 }, ymax { 1.0 };

  if (! rowDatas.empty()) {
    ymin = rowDatas.begin ()->first;
    ymax = rowDatas.rbegin()->first + 1;

    xmin = visitor.timeMin();
    xmax = visitor.timeMax();
  }

  dataRange = Range(xmin, ymin, xmax, ymax);

  //---

  return dataRange;
}

void
CQChartsGanttPlot::
postCalcRange()
{
  xAxis()->setColumn(startColumn());

  xAxis()->setValueType(CQChartsAxisValueType(CQChartsAxisValueType::Type::DATE));
  yAxis()->setValueType(CQChartsAxisValueType(CQChartsAxisValueType::Type::INTEGER));

  yAxis()->setMajorIncrement(1);
}

//------

bool
CQChartsGanttPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsGanttPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  using OptReal  = std::optional<double>;

  struct BarData {
    double      startTime { 0.0 };
    double      endTime   { 0.0 };
    QString     name;
    OptReal     value;
    BBox        rect;
    QModelIndex ind;
    ColorInd    ig;
    ColorInd    iv;
  };

  using BarDatas = std::vector<BarData>;

  // create boxes for date ranges
  // calc data range (start, end values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsGanttPlot *ganttPlot, PlotObjs &objs) :
     ganttPlot_(ganttPlot), objs_(objs) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      BarData barData;

      ModelIndex startModelInd(ganttPlot_, data.row, ganttPlot_->startColumn(), data.parent);
      ModelIndex endModelInd  (ganttPlot_, data.row, ganttPlot_->endColumn  (), data.parent);

      bool ok;
      barData.startTime = ganttPlot_->modelReal(startModelInd, ok); if (! ok) return State::SKIP;
      barData.endTime   = ganttPlot_->modelReal(endModelInd  , ok); if (! ok) return State::SKIP;

      if (! timeSet_) {
        timeMin_ = barData.startTime;
        timeMax_ = barData.endTime;

        timeSet_ = true;
      }
      else {
        timeMin_ = std::min(timeMin_, barData.startTime);
        timeMax_ = std::max(timeMax_, barData.endTime);
      }

      //---

      int ig = 0;
      int ng = ganttPlot_->numGroups();

      // get optional name
      if (ganttPlot_->nameColumn().isValid()) {
        ModelIndex nameModelInd(ganttPlot_, data.row, ganttPlot_->nameColumn(), data.parent);

        barData.name = ganttPlot_->modelString(nameModelInd, ok);
        if (! ok) barData.name.clear();

        ig = ganttPlot_->rowGroupInd(nameModelInd);
        if (ig < 0) ig = 0;

        groupInds_[ig].push_back(barDatas_.size());
      }

      //---

      // get optional value
      if (ganttPlot_->valueColumn().isValid()) {
        ModelIndex valueModelInd(ganttPlot_, data.row, ganttPlot_->valueColumn(), data.parent);

        barData.value = ganttPlot_->modelReal(valueModelInd, ok);
        if (! ok) barData.value = OptReal();
      }

      //---

      barData.ind = ganttPlot_->modelIndex(ganttPlot_->normalizeIndex(startModelInd));

      int iv = data.row;
      int nv = numRows();

      barData.rect = BBox(barData.startTime, iv, barData.endTime, iv + 1);
      barData.ig   = ColorInd(ig, ng);
      barData.iv   = ColorInd(iv, nv);

      barDatas_.push_back(barData);

      return State::OK;
    }

    void createObjs() {
      int ng = ganttPlot_->numGroups();

      for (const auto &pg : groupInds_) {
        auto ig = ColorInd(pg.first, ng);

        BBox rect;

        for (const auto &ind : pg.second) {
          const auto &barData = barDatas_[ind];

          rect += barData.rect;
        }

        if (ganttPlot_->isGroupStretch()) {
          rect.setXMin(timeMin_);
          rect.setXMax(timeMax_);
        }

        auto *obj = ganttPlot_->createGroupObj(rect, ig);

        objs_.push_back(obj);
      }

      for (const auto &barData : barDatas_)
        createObj(barData);
    }

    void createObj(const BarData &barData) {
      auto *obj = ganttPlot_->createBarObj(barData.rect, barData.ind, barData.ig, barData.iv);

      obj->setName(barData.name);

      if (barData.value)
        obj->setValue(barData.value);

      objs_.push_back(obj);
    }

   private:
    using Inds      = std::vector<int>;
    using GroupInds = std::map<int, Inds>;

    const CQChartsGanttPlot* ganttPlot_ { nullptr };
    PlotObjs&                objs_;
    BarDatas                 barDatas_;
    GroupInds                groupInds_;
    bool                     timeSet_   { false };
    double                   timeMin_   { 0.0 };
    double                   timeMax_   { 0.0 };
  };

  //---

  RowVisitor visitor(this, objs);

  visitModel(visitor);

  visitor.createObjs();

  return true;
}

//------

CQChartsGanttGroupObj *
CQChartsGanttPlot::
createGroupObj(const BBox &rect, const ColorInd &ig) const
{
  return new CQChartsGanttGroupObj(this, rect, ig);
}

CQChartsGanttBarObj *
CQChartsGanttPlot::
createBarObj(const BBox &rect, const QModelIndex &ind,
             const ColorInd &ig, const ColorInd &iv) const
{
  return new CQChartsGanttBarObj(this, rect, ind, ig, iv);
}

//---

CQChartsPlotCustomControls *
CQChartsGanttPlot::
createCustomControls()
{
  auto *controls = new CQChartsGanttPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsGanttGroupObj::
CQChartsGanttGroupObj(const GanttPlot *ganttPlot, const BBox &rect, const ColorInd &ig) :
 CQChartsPlotObj(const_cast<GanttPlot *>(ganttPlot), rect, ColorInd(), ig, ColorInd()),
 ganttPlot_(ganttPlot)
{
  setDetailHint(DetailHint::MAJOR);
}

QString
CQChartsGanttGroupObj::
calcId() const
{
  return QString("%1:%2:%3:%4").arg(typeName()).arg(is_.i).arg(ig_.i).arg(iv_.i);
}

QString
CQChartsGanttGroupObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  plot()->addNoTipColumns(tableTip);

  ganttPlot_->addTipColumn(tableTip, ganttPlot_->startColumn(), modelInd());
  ganttPlot_->addTipColumn(tableTip, ganttPlot_->endColumn(), modelInd());

  tableTip.addTableRow("Name", name());

  if (value())
    tableTip.addTableRow("Value", value().value());

  plot()->addTipColumns(tableTip, modelInd());

  return tableTip.str();
}

void
CQChartsGanttGroupObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, ganttPlot_->startColumn());
  addColumnSelectIndex(inds, ganttPlot_->endColumn());
}

void
CQChartsGanttGroupObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  plot_->setRefRect(CQChartsOptRect(CQChartsRect::plot(rect())));

  CQChartsDrawUtil::drawRoundedRect(device, penBrush, rect(), ganttPlot_->groupCornerSize());

  plot_->resetRefRect();

  if (name_.length()) {
    ColorInd colorInd;
    PenBrush tpenBrush;

    auto tc = ganttPlot_->interpGroupTextColor(colorInd);
    auto ta = ganttPlot_->groupTextAlpha();

    plot()->setPen(tpenBrush, PenData(/*stroked*/true, tc, ta));

    device->setPen(tpenBrush.pen);

    auto font = ganttPlot_->groupTextFont();

    plot()->setPainterFont(device, font);

    auto textOptions = ganttPlot_->groupTextOptions(device);

    CQChartsDrawUtil::drawTextInBox(device, rect(), name_, textOptions);
  }
}

void
CQChartsGanttGroupObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  auto sc = ganttPlot_->interpGroupStrokeColor(colorInd);
  auto fc = ganttPlot_->interpGroupFillColor(colorInd);

  ganttPlot_->setPenBrush(penBrush, ganttPlot_->groupPenData(sc), ganttPlot_->groupBrushData(fc));

  if (updateState)
    ganttPlot_->updateObjPenBrushState(this, penBrush);
}

//------

CQChartsGanttBarObj::
CQChartsGanttBarObj(const GanttPlot *ganttPlot, const BBox &rect, const QModelIndex &ind,
                    const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<GanttPlot *>(ganttPlot), rect, ColorInd(), ig, iv),
 ganttPlot_(ganttPlot)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInd(ind);
}

QString
CQChartsGanttBarObj::
calcId() const
{
  return QString("%1:%2:%3:%4").arg(typeName()).arg(is_.i).arg(ig_.i).arg(iv_.i);
}

QString
CQChartsGanttBarObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  plot()->addNoTipColumns(tableTip);

  ganttPlot_->addTipColumn(tableTip, ganttPlot_->startColumn(), modelInd());
  ganttPlot_->addTipColumn(tableTip, ganttPlot_->endColumn(), modelInd());

  tableTip.addTableRow("Name", name());

  if (value())
    tableTip.addTableRow("Value", value().value());

  plot()->addTipColumns(tableTip, modelInd());

  return tableTip.str();
}

void
CQChartsGanttBarObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, ganttPlot_->startColumn());
  addColumnSelectIndex(inds, ganttPlot_->endColumn());
}

void
CQChartsGanttBarObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  plot_->setRefRect(CQChartsOptRect(CQChartsRect::plot(rect())));

  CQChartsDrawUtil::drawRoundedRect(device, penBrush, rect(), ganttPlot_->cornerSize());

  plot_->resetRefRect();

  if (name_.length()) {
    ColorInd colorInd;
    PenBrush tpenBrush;

    auto tc = ganttPlot_->interpTextColor(colorInd);
    auto ta = ganttPlot_->textAlpha();

    plot()->setPen(tpenBrush, PenData(/*stroked*/true, tc, ta));

    device->setPen(tpenBrush.pen);

    auto font = ganttPlot_->textFont();

    plot()->setPainterFont(device, font);

    auto textOptions = ganttPlot_->textOptions(device);

    CQChartsDrawUtil::drawTextInBox(device, rect(), name_, textOptions);
  }
}

void
CQChartsGanttBarObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  auto sc = ganttPlot_->interpStrokeColor(colorInd);
  auto fc = ganttPlot_->interpFillColor(colorInd);

  ganttPlot_->setPenBrush(penBrush, ganttPlot_->penData(sc), ganttPlot_->brushData(fc));

  if (updateState)
    ganttPlot_->updateObjPenBrushState(this, penBrush);
}

//------

CQChartsGanttPlotCustomControls::
CQChartsGanttPlotCustomControls(CQCharts *charts) :
 CQChartsGroupPlotCustomControls(charts, "gantt")
{
}

void
CQChartsGanttPlotCustomControls::
init()
{
  addWidgets();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsGanttPlotCustomControls::
addWidgets()
{
  addOptionsWidgets();
}

void
CQChartsGanttPlotCustomControls::
addOptionsWidgets()
{
}

void
CQChartsGanttPlotCustomControls::
connectSlots(bool b)
{
  CQChartsGroupPlotCustomControls::connectSlots(b);
}

void
CQChartsGanttPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_ && ganttPlot_)
    disconnect(ganttPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  ganttPlot_ = dynamic_cast<CQChartsGanttPlot *>(plot);

  CQChartsGroupPlotCustomControls::setPlot(plot);

  if (ganttPlot_)
    connect(ganttPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsGanttPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  CQChartsGroupPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}
