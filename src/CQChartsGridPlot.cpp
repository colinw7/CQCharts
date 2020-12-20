#include <CQChartsGridPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsPlotDrawUtil.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsHtml.h>
#include <CQChartsTip.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CQTclUtil.h>
#include <CMathRound.h>

#include <QMenu>
#include <QScrollBar>
#include <QMetaMethod>

CQChartsGridPlotType::
CQChartsGridPlotType()
{
}

void
CQChartsGridPlotType::
addParameters()
{
  startParameterGroup("Data");

  addColumnParameter ("name"  , "Name"  , "nameColumn"  ).setRequired().setTip("Name");
  addColumnParameter ("label" , "Label" , "labelColumn" ).setOptional().setTip("Label");
  addColumnParameter ("row"   , "Row"   , "rowColumn"   ).setOptional().setTip("Row");
  addColumnParameter ("column", "Column", "columnColumn").setOptional().setTip("Column");
  addColumnsParameter("values", "Values", "valueColumns").setRequired().setTip("Values");

  endParameterGroup();

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
     p("Draws values in grid.").
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
 CQChartsPlot(view, view->charts()->plotType("grid"), model)
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
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGridPlot::
setLabelColumn(const Column &c)
{
  CQChartsUtil::testAndSet(labelColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGridPlot::
setRowColumn(const Column &c)
{
  CQChartsUtil::testAndSet(rowColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGridPlot::
setColumnColumn(const Column &c)
{
  CQChartsUtil::testAndSet(columnColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsGridPlot::
setValueColumns(const Columns &c)
{
  CQChartsUtil::testAndSet(valueColumns_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsGridPlot::
setDrawType(const DrawType &t)
{
  CQChartsUtil::testAndSet(drawType_, t, [&]() { drawObjs(); } );
}

void
CQChartsGridPlot::
setCellPalette(const PaletteName &n)
{
  CQChartsUtil::testAndSet(cellPalette_, n, [&]() { drawObjs(); } );
}

//---

void
CQChartsGridPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc, bool hidden=false) {
    auto *item = this->addProperty(path, this, name, alias);
    item->setDesc(desc);
    if (hidden) CQCharts::setItemIsHidden(item);
    return item;
  };

  //---

  addBaseProperties();

  // columns
  addProp("columns", "nameColumn"  , "name"  , "Name column");
  addProp("columns", "labelColumn" , "label" , "Label column");
  addProp("columns", "rowColumn"   , "row"   , "Row column");
  addProp("columns", "columnColumn", "column", "Column column");
  addProp("columns", "valueColumns", "values", "Value columns");

  addProp("options", "drawType"   , "", "Draw type");
  addProp("options", "cellPalette", "", "Cell Palette");
}

CQChartsGeom::Range
CQChartsGridPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsGridPlot::calcRange");

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

      ny_ = sqrt(nr);
      nx_ = (nr + ny_ - 1)/std::max(ny_, 1);
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      auto *plot = const_cast<CQChartsGridPlot *>(plot_);

      int row = data.row/std::max(nx_, 1);

      if (plot_->rowColumn().isValid()) {
        ModelIndex rowModelInd(plot, data.row, plot_->rowColumn(), data.parent);

        bool ok1;
        row = plot_->modelInteger(rowModelInd, ok1);
        if (! ok1) return State::SKIP;

        if (row < 0)
          return State::SKIP;
      }

      int column = data.row % std::max(nx_, 1);

      if (plot_->columnColumn().isValid()) {
        ModelIndex columnModelInd(plot, data.row, plot_->columnColumn(), data.parent);

        bool ok2;
        column = plot_->modelInteger(columnModelInd, ok2);
        if (! ok2) return State::SKIP;

        if (column < 0)
          return State::SKIP;
      }

      for (int i = 0; i < plot_->valueColumns().count(); ++i) {
        ModelIndex valueModelInd(plot, data.row, plot_->valueColumns().getColumn(i), data.parent);

        bool ok4;
        auto value = plot_->modelReal(valueModelInd, ok4);
        if (! ok4) continue;

        values_.addValue(CQChartsRValues::OptReal(value));
      }

      int row1 = ny_ - 1 - row;

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

  GridModelVisitor visitor(this);

  visitModel(visitor);

  auto *th = const_cast<CQChartsGridPlot *>(this);

  th->minValue_ = visitor.minValue();
  th->maxValue_ = visitor.maxValue();

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

      ny_ = sqrt(nr);
      nx_ = (nr + ny_ - 1)/std::max(ny_, 1);
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      auto *plot = const_cast<CQChartsGridPlot *>(plot_);

      //---

      ModelIndex nameModelInd(plot, data.row, plot_->nameColumn(), data.parent);

      bool ok1;
      auto name = plot_->modelString(nameModelInd, ok1);
      if (! ok1) return State::SKIP;

      //---

      QString label;

      if (plot_->labelColumn().isValid()) {
        ModelIndex labelModelInd(plot, data.row, plot_->labelColumn(), data.parent);

        bool ok1;
        label = plot_->modelString(labelModelInd, ok1);
     }

      //---

      int row = data.row/std::max(nx_, 1);

      if (plot_->rowColumn().isValid()) {
        ModelIndex rowModelInd(plot, data.row, plot_->rowColumn(), data.parent);

        bool ok2;
        row = plot_->modelInteger(rowModelInd, ok2);
        if (! ok2) return State::SKIP;

        if (row < 0)
          return State::SKIP;
      }

      //---

      int column = data.row % std::max(nx_, 1);

      if (plot_->columnColumn().isValid()) {
        ModelIndex columnModelInd(plot, data.row, plot_->columnColumn(), data.parent);

        bool ok3;
        column = plot_->modelInteger(columnModelInd, ok3);
        if (! ok3) return State::SKIP;

        if (column < 0)
          return State::SKIP;
      }

      //---

      using Values = CQChartsRValues;

      Values values;

      for (int i = 0; i < plot_->valueColumns().count(); ++i) {
        ModelIndex valueModelInd(plot, data.row, plot_->valueColumns().getColumn(i), data.parent);

        bool ok4;
        auto value = plot_->modelReal(valueModelInd, ok4);
        if (! ok4) continue;

        values.addValue(CQChartsRValues::OptReal(value));
      }

      int row1 = ny_ - 1 - row;

      BBox bbox(column - 0.5, row1 - 0.5, column + 0.5, row1 + 0.5);

      auto *obj = plot_->createCellObj(bbox, name, label, row, column, values);

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

  GridModelVisitor visitor(this);

  visitModel(visitor);

  objs = visitor.objs();

  return true;
}

//------

bool
CQChartsGridPlot::
addMenuItems(QMenu *)
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

  tableTip.addTableRow("Name", name_);

  if (label_.length())
    tableTip.addTableRow("Label", label_);

//tableTip.addTableRow("Row"   , row_);
//tableTip.addTableRow("Column", column_);

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

void
CQChartsGridCellObj::
draw(PaintDevice *device) const
{
  double cw = rect().getWidth ()*0.475;
  double ch = rect().getHeight()*0.475;

  BBox cellRect(rect().getXMid() - cw, rect().getYMid() - ch,
                rect().getXMid() + cw, rect().getYMid() + ch);

  //---

  int nv = values_.size();
  if (! nv) return;

  // draw background ellipse
  double f = CMathUtil::map(values_.max(0.0), plot_->minValue(), plot_->maxValue(), 0.0, 1.0);

  Color color(Color::Type::PALETTE);

  PenBrush penBrush;

  plot_->setPenBrush(penBrush,
    PenData(false), BrushData(true, plot_->interpColor(color, ColorInd(f))));

  if (device->isInteractive())
    plot_->updateObjPenBrushState(this, penBrush);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  if      (plot_->drawType() == CQChartsGridPlot::DrawType::PIE)
    device->drawEllipse(cellRect);
  else if (plot_->drawType() == CQChartsGridPlot::DrawType::TREEMAP)
    device->drawRect(cellRect);
  else
    device->drawEllipse(cellRect);

  //---

  auto tc = plot_->interpColor(Color(Color::Type::INTERFACE_VALUE, 1.0), ColorInd());

  //---

  // draw label and short value in center of cell
  CQChartsTextOptions textOptions;

  double tx = rect().getXMid();
  double ty = rect().getYMid();

  QString label = label_;

  if (! label.length())
    label = name_;

  if (label.length()) {
    auto psize = CQChartsDrawUtil::calcTextSize(label, device->font(), textOptions);

    ty += plot()->pixelToWindowHeight(psize.height())/2.0;

    plot_->setPen(penBrush, PenData(true, tc));

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    CQChartsDrawUtil::drawTextAtPoint(device, Point(tx, ty), label,
                                      textOptions, /*centered*/false);

    ty -= plot()->pixelToWindowHeight(psize.height());
  }

  auto sv = CMathUtil::scaledNumberString(values_.sum(), 0);

  CQChartsDrawUtil::drawTextAtPoint(device, Point(tx, ty), sv.c_str(),
                                    textOptions, /*centered*/false);

  //---

  // draw pie slice or treemap if inside
  if (isInside() && nv > 1) {
    if      (plot_->drawType() == CQChartsGridPlot::DrawType::PIE) {
      CQChartsPlotDrawUtil::drawPie(const_cast<CQChartsGridPlot *>(plot_), device,
                                    values_, cellRect, plot_->cellPalette());
    }
    else if (plot_->drawType() == CQChartsGridPlot::DrawType::TREEMAP) {
      CQChartsPlotDrawUtil::drawTreeMap(const_cast<CQChartsGridPlot *>(plot_), device,
                                        values_, cellRect, plot_->cellPalette());
    }

    //---

    // set palette color
    Color color(Color::Type::PALETTE);

    color.setPaletteName(plot_->cellPalette().name());

    // text text color
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
                                        CQChartsLength(4, CQChartsUnits::PIXEL));

      //---

      QString name = plot_->columnHeaderName(plot_->valueColumns().getColumn(i));

      QString text = QString("%1 %2").arg(name).arg(v.value_or(0.0));

      CQChartsTextOptions textOptions;

      textOptions.align = Qt::AlignLeft;

      auto psize = CQChartsDrawUtil::calcTextSize(text, device->font(), textOptions);

      plot_->setPen(penBrush, PenData(true, tc));

      CQChartsDrawUtil::setPenBrush(device, penBrush);

      CQChartsDrawUtil::drawTextAtPoint(device, Point(tx + bw + xm, ty), text,
                                        textOptions, /*centered*/false);

      ty -= plot()->pixelToWindowHeight(psize.height());

      ++i;
    }

    //---

    tx = rect().getXMid();

    CQChartsTextOptions textOptions;

    auto psize = CQChartsDrawUtil::calcTextSize(name_, device->font(), textOptions);

    ty = rect().getYMin() - plot()->pixelToWindowHeight(psize.height());

    CQChartsDrawUtil::drawTextAtPoint(device, Point(tx, ty), name_,
                                      textOptions, /*centered*/false);

    ty -= plot()->pixelToWindowHeight(psize.height());

    QString text = QString("%1").arg(values_.sum());

    CQChartsDrawUtil::drawTextAtPoint(device, Point(tx, ty), text,
                                      textOptions, /*centered*/false);
  }
}

void
CQChartsGridCellObj::
getObjSelectIndices(Indices &) const
{
}
