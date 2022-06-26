#include <CQChartsGridPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsPlotDrawUtil.h>
#include <CQChartsUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsTip.h>
#include <CQChartsHtml.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CQTclUtil.h>
#include <CMathRound.h>

#include <QMenu>
#include <QMetaMethod>

CQChartsGridPlotType::
CQChartsGridPlotType()
{
}

void
CQChartsGridPlotType::
addParameters()
{
  using GridPlot = CQChartsGridPlot;

  startParameterGroup("Data");

  addColumnParameter ("name"  , "Name"  , "nameColumn"  ).
    setRequired().setPropPath("columns.name").setTip("Name");
  addColumnParameter ("label" , "Label" , "labelColumn" ).
    setOptional().setPropPath("columns.label").setTip("Label");
  addColumnParameter ("row"   , "Row"   , "rowColumn"   ).
    setOptional().setPropPath("columns.row").setTip("Row");
  addColumnParameter ("column", "Column", "columnColumn").
    setOptional().setPropPath("columns.column").setTip("Column");
  addColumnsParameter("values", "Values", "valueColumns").
    setRequired().setPropPath("columns.values").setTip("Values");

  endParameterGroup();

  // options
  addEnumParameter("drawType", "Draw Type", "drawType").
    addNameValue("PIE"    , static_cast<int>(GridPlot::DrawType::PIE    )).
    addNameValue("TREEMAP", static_cast<int>(GridPlot::DrawType::TREEMAP)).
    setTip("Draw type");

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsGridPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Grid Plot").
    h3("Summary").
     p("Draws set of values in a grid.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/grid.png"));
}

void
CQChartsGridPlotType::
analyzeModel(ModelData *, AnalyzeModelData &)
{
}

CQChartsPlot *
CQChartsGridPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsGridPlot(view, model);
}

//------

CQChartsGridPlot::
CQChartsGridPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("grid"), model),
 CQChartsObjShapeData<CQChartsGridPlot>(this),
 CQChartsObjTextData <CQChartsGridPlot>(this)
{
}

CQChartsGridPlot::
~CQChartsGridPlot()
{
  term();
}

//---

void
CQChartsGridPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  setFilled(true);
  setFillColor(Color::makePalette());

  setStroked(true);
  setStrokeAlpha(Alpha(0.5));

  setTextColor(Color::makeInterfaceValue(1.0));
  setTextScaled(true);

  //---

  setEqualScale(true);

  //---

  addTitle();
}

void
CQChartsGridPlot::
term()
{
}

//---

void
CQChartsGridPlot::
setNameColumn(const Column &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsGridPlot::
setLabelColumn(const Column &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsGridPlot::
setRowColumn(const Column &c)
{
  CQChartsUtil::testAndSet(rowColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsGridPlot::
setColumnColumn(const Column &c)
{
  CQChartsUtil::testAndSet(columnColumn_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

void
CQChartsGridPlot::
setValueColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(valueColumns_, c, [&]() {
    updateRangeAndObjs(); emit customDataChanged();
  } );
}

//---

CQChartsColumn
CQChartsGridPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "name"  ) c = this->nameColumn();
  else if (name == "label" ) c = this->labelColumn();
  else if (name == "row"   ) c = this->rowColumn();
  else if (name == "column") c = this->columnColumn();
  else                       c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsGridPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "name"  ) this->setNameColumn(c);
  else if (name == "label" ) this->setLabelColumn(c);
  else if (name == "row"   ) this->setRowColumn(c);
  else if (name == "column") this->setColumnColumn(c);
  else                       CQChartsPlot::setNamedColumn(name, c);
}

CQChartsColumns
CQChartsGridPlot::
getNamedColumns(const QString &name) const
{
  Columns c;
  if (name == "values") c = this->valueColumns();
  else                  c = CQChartsPlot::getNamedColumns(name);

  return c;
}

void
CQChartsGridPlot::
setNamedColumns(const QString &name, const Columns &c)
{
  if (name == "values") this->setValueColumns(c);
  else                  CQChartsPlot::setNamedColumns(name, c);
}

//---

void
CQChartsGridPlot::
setDrawType(const DrawType &t)
{
  CQChartsUtil::testAndSet(drawType_, t, [&]() {
    drawObjs(); emit customDataChanged();
  } );
}

void
CQChartsGridPlot::
setCellMargin(double m)
{
  CQChartsUtil::testAndSet(cellMargin_, m, [&]() { drawObjs(); } );
}

void
CQChartsGridPlot::
setCellPalette(const PaletteName &n)
{
  CQChartsUtil::testAndSet(cellPalette_, n, [&]() { drawObjs(); } );
}

void
CQChartsGridPlot::
setShowValue(bool b)
{
  CQChartsUtil::testAndSet(showValue_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsGridPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "nameColumn"  , "name"  , "Name column");
  addProp("columns", "labelColumn" , "label" , "Label column");
  addProp("columns", "rowColumn"   , "row"   , "Row column");
  addProp("columns", "columnColumn", "column", "Column column");
  addProp("columns", "valueColumns", "values", "Value columns");

  // options
  addProp("options", "drawType"   , "", "Draw type");
  addProp("options", "cellMargin" , "", "Cell margin");
  addProp("options", "cellPalette", "", "Cell palette");
  addProp("options", "showValue"  , "", "Show value");

  // fill
  addProp("fill", "filled", "visible", "Fill visible");

  addFillProperties("fill", "fill", "");

  // stroke
  addProp("stroke", "stroked", "visible", "Stroke visible");

  addLineProperties("stroke", "stroke", "");

  // text
//addProp("text", "textVisible", "visible", "Text visible");

  addTextProperties("text", "text", "",
    CQChartsTextOptions::ValueType::CONTRAST | CQChartsTextOptions::ValueType::SCALED |
    CQChartsTextOptions::ValueType::CLIP_LENGTH |
    CQChartsTextOptions::ValueType::CLIP_ELIDE);
}

CQChartsGeom::Range
CQChartsGridPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsGridPlot::calcRange");

  //---

  // check columns (TODO: all columns)
  bool columnsValid = true;

  if (! checkNumericColumns(valueColumns(), "Values", /*required*/true))
    columnsValid = false;

  if (! columnsValid)
    return Range();

  //---

  class GridModelVisitor : public ModelVisitor {
   public:
    using Values = CQChartsRValues;

   public:
    GridModelVisitor(const CQChartsGridPlot *plot) :
     plot_(plot) {
    }

    void initVisit() override {
      int nr = model_->rowCount();

      // calc square grid from row count
      CQChartsUtil::countToSquareGrid(nr, nx_, ny_);
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // get row number (from row column or from model row)
      int row = data.row/std::max(nx_, 1);

      if (plot_->rowColumn().isValid()) {
        ModelIndex rowModelInd(plot_, data.row, plot_->rowColumn(), data.parent);

        bool ok1;
        row = int(plot_->modelInteger(rowModelInd, ok1));
        if (! ok1) return State::SKIP;

        if (row < 0)
          return State::SKIP;
      }

      //---

      // get column number (from column column or from model row)
      int column = data.row % std::max(nx_, 1);

      if (plot_->columnColumn().isValid()) {
        ModelIndex columnModelInd(plot_, data.row, plot_->columnColumn(), data.parent);

        bool ok2;
        column = int(plot_->modelInteger(columnModelInd, ok2));
        if (! ok2) return State::SKIP;

        if (column < 0)
          return State::SKIP;
      }

      //---

      // get values from value columns
      for (int i = 0; i < plot_->valueColumns().count(); ++i) {
        ModelIndex valueModelInd(plot_, data.row, plot_->valueColumns().getColumn(i), data.parent);

        bool ok4;
        auto value = plot_->modelReal(valueModelInd, ok4);
        if (! ok4) continue;

        values_.addValue(CQChartsRValues::OptReal(value));
      }

      //---

      int row1 = ny_ - 1 - row; // flip y

      BBox bbox(column - 0.5, row1 - 0.5, column + 0.5, row1 + 0.5);

      bbox_ += bbox;

      return State::OK;
    }

    double minValue() const { return values_.min(0.0); }
    double maxValue() const { return values_.max(0.0); }

    const BBox &bbox() const { return bbox_; }

   private:
    const CQChartsGridPlot* plot_ { nullptr };
    int                     nx_   { 1 };
    int                     ny_   { 1 };
    Values                  values_;
    BBox                    bbox_;
  };

  //---

  // process model
  GridModelVisitor visitor(this);

  visitModel(visitor);

  //---

  // save min/max or all values
  auto *th = const_cast<CQChartsGridPlot *>(this);

  th->minValue_ = visitor.minValue();
  th->maxValue_ = visitor.maxValue();

  //---

  if (! visitor.bbox().isSet())
    return Range(-1, -1, 1, 1);

  return Range(visitor.bbox());
}

bool
CQChartsGridPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsGridPlot::createObjs");

  class GridModelVisitor : public ModelVisitor {
   public:
    GridModelVisitor(const CQChartsGridPlot *plot) :
     plot_(plot) {
    }

    void initVisit() override {
      int nr = model_->rowCount();

      // calc square grid from row count
      CQChartsUtil::countToSquareGrid(nr, nx_, ny_);
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // get name
      ModelIndex nameModelInd(plot_, data.row, plot_->nameColumn(), data.parent);

      bool ok1;
      auto name = plot_->modelString(nameModelInd, ok1);
      if (! ok1) return State::SKIP;

      //---

      // get label (if label column specified)
      QString label;

      if (plot_->labelColumn().isValid()) {
        ModelIndex labelModelInd(plot_, data.row, plot_->labelColumn(), data.parent);

        bool ok1;
        label = plot_->modelString(labelModelInd, ok1);
     }

      //---

      // get row number (from row column or from model row)
      int row = data.row/std::max(nx_, 1);

      if (plot_->rowColumn().isValid()) {
        ModelIndex rowModelInd(plot_, data.row, plot_->rowColumn(), data.parent);

        bool ok2;
        row = int(plot_->modelInteger(rowModelInd, ok2));
        if (! ok2) return State::SKIP;

        if (row < 0)
          return State::SKIP;
      }

      //---

      // get column number (from column column or from model row)
      int column = data.row % std::max(nx_, 1);

      if (plot_->columnColumn().isValid()) {
        ModelIndex columnModelInd(plot_, data.row, plot_->columnColumn(), data.parent);

        bool ok3;
        column = int(plot_->modelInteger(columnModelInd, ok3));
        if (! ok3) return State::SKIP;

        if (column < 0)
          return State::SKIP;
      }

      //---

      // get values from value columns
      using Values = CQChartsRValues;

      Values values;

      for (int i = 0; i < plot_->valueColumns().count(); ++i) {
        ModelIndex valueModelInd(plot_, data.row, plot_->valueColumns().getColumn(i), data.parent);

        bool ok4;
        auto value = plot_->modelReal(valueModelInd, ok4);
        if (! ok4) continue;

        values.addValue(CQChartsRValues::OptReal(value));
      }

      //---

      int row1 = ny_ - 1 - row; // flip y

      BBox bbox(column - 0.5, row1 - 0.5, column + 0.5, row1 + 0.5);

      //---

      // create cell object
      auto *obj = plot_->createCellObj(bbox, name, label, row, column, values);

      obj->setModelInd(plot_->normalizedModelIndex(nameModelInd));

      objs_.push_back(obj);

      return State::OK;
    }

    const PlotObjs &objs() const { return objs_; }

   private:
    const CQChartsGridPlot* plot_ { nullptr };
    PlotObjs                objs_;
    int                     nx_   { 1 };
    int                     ny_   { 1 };
  };

  //---

  // process nodel
  GridModelVisitor visitor(this);

  visitModel(visitor);

  //---

  objs = visitor.objs();

  return true;
}

//------

bool
CQChartsGridPlot::
addMenuItems(QMenu *, const Point &)
{
  return true;
}

//------

CQChartsGridCellObj *
CQChartsGridPlot::
createCellObj(const BBox &bbox, const QString &name, const QString &label,
              int row, int column, const RValues &values) const
{
  return new CQChartsGridCellObj(this, bbox, name, label, row, column, values);
}

//---

CQChartsPlotCustomControls *
CQChartsGridPlot::
createCustomControls()
{
  auto *controls = new CQChartsGridPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsGridCellObj::
CQChartsGridCellObj(const Plot *plot, const BBox &bbox, const QString &name,
                    const QString &label, int row, int column, const RValues &values) :
 CQChartsPlotObj(const_cast<Plot *>(plot), bbox, ColorInd(), ColorInd(), ColorInd()),
 plot_(plot), name_(name), label_(label), row_(row), column_(column), values_(values)
{
}

QString
CQChartsGridCellObj::
calcId() const
{
  return QString("%1:%2:%3").arg(typeName()).arg(row_).arg(column_);
}

QString
CQChartsGridCellObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  //---

  auto addColumnRowValue = [&](const Column &column) {
    plot_->addTipColumn(tableTip, column, modelInd());
  };

  //---

  tableTip.addTableRow("Name", name_);

  if (label_.length())
    tableTip.addTableRow("Label", label_);

  addColumnRowValue(plot_->rowColumn());
  addColumnRowValue(plot_->columnColumn());

  if (plot_->colorColumn().isValid())
    addColumnRowValue(plot_->colorColumn());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsGridCellObj::
draw(PaintDevice *device) const
{
  // set cell rect
  double m = plot_->cellMargin();

  double cw = (rect().getWidth ()/2.0 - m);
  double ch = (rect().getHeight()/2.0 - m);

  double xm = rect().getXMid();
  double ym = rect().getYMid();

  BBox cellRect(xm - cw, ym - ch, xm + cw, ym + ch);

  //---

  int nv = values_.size();
  if (! nv) return; // optional values ?

  //---

  // draw background shape (dependent on mouse over draw type)
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  double adjustScale = 1.0;

  if      (plot_->drawType() == CQChartsGridPlot::DrawType::PIE) {
    device->drawEllipse(cellRect); adjustScale = 1.0/sqrt(2.0);
  }
  else if (plot_->drawType() == CQChartsGridPlot::DrawType::TREEMAP) {
    device->drawRect(cellRect); adjustScale = 1.0;
  }
  else {
    device->drawEllipse(cellRect); adjustScale = 1.0/sqrt(2.0);
  }

  //---

  // calc label
  auto label = label_;

  if (! label.length())
    label = name_;

  //---

  // calc text color
  plot_->charts()->setContrastColor(penBrush.brush.color());

  auto tc = plot_->interpTextColor(ColorInd());

  plot_->charts()->resetContrastColor();

  //---

  // draw label and optional short value

  QStringList labels;

  if (label.length())
    labels.push_back(label);

  if (plot_->isShowValue()) {
    auto sv = CMathUtil::scaledNumberString(values_.sum(), 0);

    labels.push_back(QString::fromStdString(sv));
  }

  auto textOptions = plot_->textOptions();

  textOptions.angle = Angle();
  textOptions.align = (Qt::AlignHCenter | Qt::AlignVCenter);

  if (labels.length()) {
    plot_->setPen(penBrush, PenData(true, tc, plot_->textAlpha()));

    plot_->setPainterFont(device, plot_->textFont());

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    CQChartsDrawUtil::drawStringsInBox(device, cellRect, labels, textOptions, adjustScale);
  }

  //---

  // draw pie slice or treemap if inside
  auto &cellPalette = plot_->cellPalette();

  if (isInside() && nv > 1) {
    if      (plot_->drawType() == CQChartsGridPlot::DrawType::PIE) {
      CQChartsPlotDrawUtil::drawPie(const_cast<CQChartsGridPlot *>(plot_), device,
                                    values_, cellRect, cellPalette, penBrush.pen);
    }
    else if (plot_->drawType() == CQChartsGridPlot::DrawType::TREEMAP) {
      CQChartsPlotDrawUtil::drawTreeMap(const_cast<CQChartsGridPlot *>(plot_), device,
                                        values_, cellRect, cellPalette, penBrush.pen);
    }

    //---

    // set palette color
    auto color = Color::makePalette();

    color.setPaletteName(cellPalette.name());

    //---

    // draw values
    double xm = plot()->pixelToWindowWidth(4);

    double tx = rect().getXMax() + xm;
    double ty = rect().getYMax();

    int i  = 0;
    int nv = values_.size();

    double bw = plot()->pixelToWindowWidth (16);
    double bh = plot()->pixelToWindowHeight(16);

    for (auto &v : values_.values()) {
      plot_->setBrush(penBrush, BrushData(true, plot_->interpColor(color, ColorInd(i, nv))));

      CQChartsDrawUtil::setPenBrush(device, penBrush);

      CQChartsDrawUtil::drawRoundedRect(device, BBox(tx, ty, tx + bw, ty + bh),
                                        CQChartsLength::pixel(4));

      //---

      auto name = plot_->columnHeaderName(plot_->valueColumns().getColumn(i));

      auto text = QString("%1 %2").arg(name).arg(v.value_or(0.0));

      auto textOptions = plot_->textOptions();

      textOptions.angle = Angle();
      textOptions.align = Qt::AlignLeft;

      auto psize = CQChartsDrawUtil::calcTextSize(text, device->font(), textOptions);

      plot_->setPen(penBrush, PenData(true, tc, plot_->textAlpha()));

      CQChartsDrawUtil::setPenBrush(device, penBrush);

      CQChartsDrawUtil::drawTextAtPoint(device, Point(tx + bw + xm, ty), text,
                                        textOptions, /*centered*/false);

      ty -= plot()->pixelToWindowHeight(psize.height());

      ++i;
    }

    //---

    // draw sum
    tx = rect().getXMid();

    auto textOptions = plot_->textOptions();

    textOptions.angle = Angle();
    textOptions.align = (Qt::AlignHCenter | Qt::AlignVCenter);

    auto psize = CQChartsDrawUtil::calcTextSize(name_, device->font(), textOptions);

    ty = rect().getYMin() - plot()->pixelToWindowHeight(psize.height());

    CQChartsDrawUtil::drawTextAtPoint(device, Point(tx, ty), name_,
                                      textOptions, /*centered*/false);

    ty -= plot()->pixelToWindowHeight(psize.height());

    auto text = QString::number(values_.sum());

    CQChartsDrawUtil::drawTextAtPoint(device, Point(tx, ty), text,
                                      textOptions, /*centered*/false);
  }
}

void
CQChartsGridCellObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  ColorInd colorInd;

  //---

  bool useValueColor = false;

  int nv = values_.size();

  if (nv > 1) {
    double f = CMathUtil::map(values_.max(0.0), plot_->minValue(), plot_->maxValue(), 0.0, 1.0);

    colorInd = ColorInd(f);

    useValueColor = true;
  }

  //---

  // calc brush color
  QColor bc;

  Color indColor;

  auto ind1 = plot_->unnormalizeIndex(modelInd());

  if      (plot_->colorColumnColor(ind1.row(), ind1.parent(), indColor))
    bc = plot_->interpColor(indColor, colorInd);
  else if (useValueColor)
    bc = plot_->interpColor(Color::makePalette(), colorInd);
  else
    bc = plot_->interpFillColor(colorInd);

  // calc stroke color
  auto sc = plot_->interpStrokeColor(colorInd);

  //---

  plot_->setPenBrush(penBrush, plot_->penData(sc), plot_->brushData(bc));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsGridCellObj::
getObjSelectIndices(Indices &) const
{
}

//------

CQChartsGridPlotCustomControls::
CQChartsGridPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "grid")
{
}

void
CQChartsGridPlotCustomControls::
init()
{
  addWidgets();

  addLayoutStretch();

  connectSlots(true);
}

void
CQChartsGridPlotCustomControls::
addWidgets()
{
  addColumnWidgets();

  addOptionsWidgets();
}

void
CQChartsGridPlotCustomControls::
addColumnWidgets()
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  addNamedColumnWidgets(QStringList() <<
    "name" << "label" << "row" << "column" << "values" << "color", columnsFrame);
}

void
CQChartsGridPlotCustomControls::
addOptionsWidgets()
{
  // options group
  optionsFrame_ = createGroupFrame("Options", "optionsFrame", "groupBox", /*stretch*/false);

  drawTypeCombo_ = createEnumEdit("drawType");

  addFrameWidget(optionsFrame_, "Draw Type", drawTypeCombo_);
}

void
CQChartsGridPlotCustomControls::
connectSlots(bool b)
{
  CQChartsWidgetUtil::optConnectDisconnect(b,
    drawTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(drawTypeSlot()));

  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsGridPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsGridPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);
}

void
CQChartsGridPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  drawTypeCombo_->setCurrentValue(static_cast<int>(plot_->drawType()));

  //---

  CQChartsPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

//---

void
CQChartsGridPlotCustomControls::
drawTypeSlot()
{
  plot_->setDrawType(static_cast<CQChartsGridPlot::DrawType>(drawTypeCombo_->currentValue()));
}
