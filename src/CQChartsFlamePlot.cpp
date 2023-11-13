#include <CQChartsFlamePlot.h>
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

CQChartsFlamePlotType::
CQChartsFlamePlotType()
{
}

void
CQChartsFlamePlotType::
addParameters()
{
  startParameterGroup("Flame");

  // required path/start/end times
  addColumnParameter("ind"  , "Ind"  , "indColumn"  ).
    setRequired().setPropPath("columns.ind");
  addColumnParameter("path" , "Path" , "pathColumn" ).
    setRequired().setPropPath("columns.path");
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
CQChartsFlamePlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
    h2("Flame Plot").
     h3("Summary").
      p("Draws Flame from ind, path, start, end time data.").
     h3("Columns").
      p("The time is specified with the " + B("Start") + " and " + B("End") + " columns.").
      p("The optional name can be specified in the " + B("Name") + " column.").
      p("The optional value can be specified in the " + B("Value") + " column.").
     h3("Limitations").
      p("None.").
     h3("Example").
      p(IMG("images/flame.png"));
}

CQChartsPlot *
CQChartsFlamePlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsFlamePlot(view, model);
}

//------

CQChartsFlamePlot::
CQChartsFlamePlot(View *view, const ModelP &model) :
 CQChartsGroupPlot(view, view->charts()->plotType("flame"), model),
 CQChartsObjShapeData<CQChartsFlamePlot>(this),
 CQChartsObjTextData <CQChartsFlamePlot>(this)
{
}

CQChartsFlamePlot::
~CQChartsFlamePlot()
{
  CQChartsFlamePlot::term();
}

//---

void
CQChartsFlamePlot::
init()
{
  CQChartsGroupPlot::init();

  //---

  NoUpdate noUpdate(this);

  setFillColor(Color::makePalette());

  //---

  addAxes();

  addTitle();
}

void
CQChartsFlamePlot::
term()
{
}

//---

void
CQChartsFlamePlot::
setIndColumn(const Column &c)
{
  CQChartsUtil::testAndSet(indColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsFlamePlot::
setPathColumn(const Column &c)
{
  CQChartsUtil::testAndSet(pathColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsFlamePlot::
setStartColumn(const Column &c)
{
  CQChartsUtil::testAndSet(startColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsFlamePlot::
setEndColumn(const Column &c)
{
  CQChartsUtil::testAndSet(endColumn_, c, [&]() {
    updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsFlamePlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsFlamePlot::
setValueColumn(const Column &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//---

CQChartsColumn
CQChartsFlamePlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "ind"  ) c = this->indColumn();
  else if (name == "path" ) c = this->pathColumn();
  else if (name == "start") c = this->startColumn();
  else if (name == "end"  ) c = this->endColumn();
  else if (name == "name" ) c = this->nameColumn();
  else if (name == "value") c = this->valueColumn();
  else                      c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsFlamePlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "ind"  ) this->setIndColumn(c);
  else if (name == "path" ) this->setPathColumn(c);
  else if (name == "start") this->setStartColumn(c);
  else if (name == "end"  ) this->setEndColumn(c);
  else if (name == "name" ) this->setNameColumn(c);
  else if (name == "value") this->setValueColumn(c);
  else                      CQChartsPlot::setNamedColumn(name, c);
}

//---

void
CQChartsFlamePlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "indColumn", "ind", "Ind column");
  addProp("columns", "pathColumn", "path", "Path column");
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
}

CQChartsGeom::Range
CQChartsFlamePlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsFlamePlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsFlamePlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  // ind/path/start/end columns required
  // name, value columns optional

  if (! checkColumn(indColumn  (), "Ind"  , /*required*/true) ||
      ! checkColumn(pathColumn (), "Path" , /*required*/true) ||
      ! checkColumn(startColumn(), "Start", /*required*/true) ||
      ! checkColumn(endColumn  (), "End"  , /*required*/true))
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
  // calc data range using ind, path, start, end values
  class RowVisitor : public ModelVisitor {
   public:
    struct RowData {
      QString     ind;
      QStringList path;
      double      startTime { 0.0 };
      double      endTime   { 0.0 };
    };

    using RowDatas = std::map<int, RowData>;

   public:
    RowVisitor(const CQChartsFlamePlot *flamePlot) :
     flamePlot_(flamePlot) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ModelIndex indModelInd  (flamePlot_, data.row, flamePlot_->indColumn  (), data.parent);
      ModelIndex pathModelInd (flamePlot_, data.row, flamePlot_->pathColumn (), data.parent);
      ModelIndex startModelInd(flamePlot_, data.row, flamePlot_->startColumn(), data.parent);
      ModelIndex endModelInd  (flamePlot_, data.row, flamePlot_->endColumn  (), data.parent);

      auto modelString = [&](QString &str, ModelIndex &ind) {
        bool ok;
        str = flamePlot_->modelString(ind, ok);
        return ok;
      };

      auto modelReal = [&](double &r, ModelIndex &ind) {
        bool ok;
        r = flamePlot_->modelReal(ind, ok);
        return ok;
      };

      auto modelStringList = [&](QStringList &strs, ModelIndex &ind) {
        bool ok;
        auto str = flamePlot_->modelString(ind, ok);
        if (str.length())
          strs = str.split("/");
        else
          strs = QStringList();
        return ok;
      };

      RowData rowData;

      if (! modelString    (rowData.ind      , indModelInd  )) return State::SKIP;
      if (! modelStringList(rowData.path     , pathModelInd )) return State::SKIP;
      if (! modelReal      (rowData.startTime, startModelInd)) return State::SKIP;
      if (! modelReal      (rowData.endTime  , endModelInd  )) return State::SKIP;

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

      int depth = rowData.path.length();

      maxDepth_ = std::max(maxDepth_, depth + 1);

      return State::OK;
    }

    const RowDatas &rowDatas() const { return rowDatas_; }

    double timeMin() const { return timeMin_; }
    double timeMax() const { return timeMax_; }

    int maxDepth() const { return maxDepth_; }

   private:
    const CQChartsFlamePlot* flamePlot_ { nullptr };
    RowDatas                 rowDatas_;
    bool                     timeSet_   { false };
    double                   timeMin_   { 0.0 };
    double                   timeMax_   { 0.0 };
    int                      maxDepth_  { 1 };
  };

  //---

  RowVisitor visitor(this);

  visitModel(visitor);

  maxDepth_ = visitor.maxDepth();

  //---

  // set value range
  const auto &rowDatas = visitor.rowDatas();

  double xmin { 0.0 }, ymin { 0.0 }, xmax { 1.0 }, ymax { 1.0 };

  if (! rowDatas.empty()) {
    ymin = 0;
    ymax = maxDepth_;

    xmin = visitor.timeMin();
    xmax = visitor.timeMax();
  }

  dataRange = Range(xmin, ymin, xmax, ymax);

  //---

  return dataRange;
}

void
CQChartsFlamePlot::
postCalcRange()
{
  xAxis()->setColumn(startColumn());

  yAxis()->setValueType(CQChartsAxisValueType::integer());
  yAxis()->setMajorIncrement(1);
}

//------

bool
CQChartsFlamePlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsFlamePlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  using OptReal  = std::optional<double>;

  struct BarData {
    QString     ind;
    QStringList path;
    double      startTime { 0.0 };
    double      endTime   { 0.0 };
    QString     name;
    OptReal     value;
    BBox        rect;
    QModelIndex modelInd;
    ColorInd    ig;
    ColorInd    iv;
  };

  using BarDatas = std::vector<BarData>;

  // create boxes for date ranges
  // calc data range (start, end values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsFlamePlot *flamePlot, PlotObjs &objs) :
     flamePlot_(flamePlot), objs_(objs) {
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      ModelIndex indModelInd  (flamePlot_, data.row, flamePlot_->indColumn  (), data.parent);
      ModelIndex pathModelInd (flamePlot_, data.row, flamePlot_->pathColumn (), data.parent);
      ModelIndex startModelInd(flamePlot_, data.row, flamePlot_->startColumn(), data.parent);
      ModelIndex endModelInd  (flamePlot_, data.row, flamePlot_->endColumn  (), data.parent);

      auto modelString = [&](QString &str, ModelIndex &ind) {
        bool ok;
        str = flamePlot_->modelString(ind, ok);
        return ok;
      };

      auto modelReal = [&](double &r, ModelIndex &ind) {
        bool ok;
        r = flamePlot_->modelReal(ind, ok);
        return ok;
      };

      auto modelStringList = [&](QStringList &strs, ModelIndex &ind) {
        bool ok;
        auto str = flamePlot_->modelString(ind, ok);
        if (str.length())
          strs = str.split("/");
        else
          strs = QStringList();
        return ok;
      };

      BarData barData;

      if (! modelString    (barData.ind      , indModelInd  )) return State::SKIP;
      if (! modelStringList(barData.path     , pathModelInd )) return State::SKIP;
      if (! modelReal      (barData.startTime, startModelInd)) return State::SKIP;
      if (! modelReal      (barData.endTime  , endModelInd  )) return State::SKIP;

      if (! timeSet_) {
        timeMin_ = barData.startTime;
        timeMax_ = barData.endTime;

        timeSet_ = true;
      }
      else {
        timeMin_ = std::min(timeMin_, barData.startTime);
        timeMax_ = std::max(timeMax_, barData.endTime);
      }

      int depth = barData.path.length();

      maxDepth_ = std::max(maxDepth_, depth + 1);

      //---

      // get optional name
      if (flamePlot_->nameColumn().isValid()) {
        ModelIndex nameModelInd(flamePlot_, data.row, flamePlot_->nameColumn(), data.parent);

        bool ok;
        barData.name = flamePlot_->modelString(nameModelInd, ok);
        if (! ok) barData.name.clear();
      }

      //---

      // get optional value
      if (flamePlot_->valueColumn().isValid()) {
        ModelIndex valueModelInd(flamePlot_, data.row, flamePlot_->valueColumn(), data.parent);

        bool ok;
        barData.value = flamePlot_->modelReal(valueModelInd, ok);
        if (! ok) barData.value = OptReal();
      }

      //---

      barData.modelInd = flamePlot_->modelIndex(flamePlot_->normalizeIndex(startModelInd));

      int iv = data.row;
      int nv = numRows();

      barData.rect = BBox(barData.startTime, depth, barData.endTime, depth + 1);
      barData.ig   = ColorInd(depth, maxDepth_);
      barData.iv   = ColorInd(iv, nv);

      barDatas_.push_back(barData);

      return State::OK;
    }

    void createObjs() {
      for (const auto &barData : barDatas_)
        createObj(barData);
    }

    void createObj(const BarData &barData) {
      auto *obj = flamePlot_->createBarObj(barData.rect, barData.modelInd, barData.ig, barData.iv);

      obj->setName(barData.name);

      if (barData.value)
        obj->setValue(barData.value);

      objs_.push_back(obj);
    }

   private:
    const CQChartsFlamePlot* flamePlot_ { nullptr };
    PlotObjs&                objs_;
    BarDatas                 barDatas_;
    bool                     timeSet_   { false };
    double                   timeMin_   { 0.0 };
    double                   timeMax_   { 0.0 };
    int                      maxDepth_  { 1 };
  };

  //---

  RowVisitor visitor(this, objs);

  visitModel(visitor);

  visitor.createObjs();

  return true;
}

//------

CQChartsFlameBarObj *
CQChartsFlamePlot::
createBarObj(const BBox &rect, const QModelIndex &ind,
             const ColorInd &ig, const ColorInd &iv) const
{
  return new CQChartsFlameBarObj(this, rect, ind, ig, iv);
}

//---

CQChartsPlotCustomControls *
CQChartsFlamePlot::
createCustomControls()
{
  auto *controls = new CQChartsFlamePlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsFlameBarObj::
CQChartsFlameBarObj(const FlamePlot *flamePlot, const BBox &rect, const QModelIndex &ind,
                    const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<FlamePlot *>(flamePlot), rect, ColorInd(), ig, iv),
 flamePlot_(flamePlot)
{
  setDetailHint(DetailHint::MAJOR);

  setModelInd(ind);
}

QString
CQChartsFlameBarObj::
calcId() const
{
  return QString("%1:%2:%3:%4").arg(typeName()).arg(is_.i).arg(ig_.i).arg(iv_.i);
}

QString
CQChartsFlameBarObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  plot()->addNoTipColumns(tableTip);

  flamePlot_->addTipColumn(tableTip, flamePlot_->indColumn  (), modelInd());
  flamePlot_->addTipColumn(tableTip, flamePlot_->pathColumn (), modelInd());
  flamePlot_->addTipColumn(tableTip, flamePlot_->startColumn(), modelInd());
  flamePlot_->addTipColumn(tableTip, flamePlot_->endColumn  (), modelInd());

  tableTip.addTableRow("Name", name());

  if (value())
    tableTip.addTableRow("Value", value().value());

  plot()->addTipColumns(tableTip, modelInd());

  return tableTip.str();
}

void
CQChartsFlameBarObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, flamePlot_->indColumn  ());
  addColumnSelectIndex(inds, flamePlot_->pathColumn ());
  addColumnSelectIndex(inds, flamePlot_->startColumn());
  addColumnSelectIndex(inds, flamePlot_->endColumn  ());
}

void
CQChartsFlameBarObj::
draw(PaintDevice *device) const
{
  // calc pen and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  plot_->setRefRect(CQChartsOptRect(CQChartsRect::plot(rect())));

  CQChartsDrawUtil::drawRoundedRect(device, penBrush, rect(), flamePlot_->cornerSize());

  plot_->resetRefRect();

  if (name_.length()) {
    ColorInd colorInd;
    PenBrush tpenBrush;

    auto tc = flamePlot_->interpTextColor(colorInd);
    auto ta = flamePlot_->textAlpha();

    plot()->setPen(tpenBrush, PenData(/*stroked*/true, tc, ta));

    device->setPen(tpenBrush.pen);

    auto font = flamePlot_->textFont();

    plot()->setPainterFont(device, font);

    auto textOptions = flamePlot_->textOptions(device);

    CQChartsDrawUtil::drawTextInBox(device, rect(), name_, textOptions);
  }
}

void
CQChartsFlameBarObj::
calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  auto sc = flamePlot_->interpStrokeColor(colorInd);
  auto fc = flamePlot_->interpFillColor(colorInd);

  flamePlot_->setPenBrush(penBrush, flamePlot_->penData(sc), flamePlot_->brushData(fc));

  if (updateState)
    flamePlot_->updateObjPenBrushState(this, penBrush);
}

//------

CQChartsFlamePlotCustomControls::
CQChartsFlamePlotCustomControls(CQCharts *charts) :
 CQChartsGroupPlotCustomControls(charts, "flame")
{
}

void
CQChartsFlamePlotCustomControls::
init()
{
  addWidgets();

  addOverview();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsFlamePlotCustomControls::
addWidgets()
{
  addOptionsWidgets();
}

void
CQChartsFlamePlotCustomControls::
addOptionsWidgets()
{
}

void
CQChartsFlamePlotCustomControls::
connectSlots(bool b)
{
  CQChartsGroupPlotCustomControls::connectSlots(b);
}

void
CQChartsFlamePlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_ && flamePlot_)
    disconnect(flamePlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  flamePlot_ = dynamic_cast<CQChartsFlamePlot *>(plot);

  CQChartsGroupPlotCustomControls::setPlot(plot);

  if (flamePlot_)
    connect(flamePlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsFlamePlotCustomControls::
updateWidgets()
{
  CQChartsGroupPlotCustomControls::updateWidgets();
}
