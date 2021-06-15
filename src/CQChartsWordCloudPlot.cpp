#include <CQChartsWordCloudPlot.h>
#include <CQChartsWordCloud.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsModelDetails.h>
#include <CQChartsModelData.h>
#include <CQChartsAnalyzeModelData.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsTip.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsScriptPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <QMenu>
#include <QAction>

CQChartsWordCloudPlotType::
CQChartsWordCloudPlotType()
{
}

void
CQChartsWordCloudPlotType::
addParameters()
{
  startParameterGroup("Word Cloud");

  addColumnParameter("value", "Value", "valueColumn").
   setRequired().setPropPath("columns.value").setTip("Value column");
  addColumnParameter("count", "Count", "countColumn").
   setPropPath("columns.count").setTip("Count column");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsWordCloudPlotType::
description() const
{
  auto B   = [](const QString &str) { return CQChartsHtml::Str::bold(str); };
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Word Cloud Plot").
    h3("Summary").
     p("Draws words in column by frequency.").
    h3("Columns").
    p("The " + B("Value") + " column specifies the value (word) column.").
    p("The optional " + B("Count") + " column specifies the number of instances "
      "of the word. If not specified the number of occurences of each word in "
      "the value column is used.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/wordCloud.png"));
}

void
CQChartsWordCloudPlotType::
analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData)
{
  auto *details = modelData->details();
  if (! details) return;

  Column valueColumn;
//Column countColumn;
  int    numUnique = -1;

  for (int c = 0; c < details->numColumns(); ++c) {
    const auto *columnDetails = details->columnDetails(Column(c));
    if (! columnDetails) continue;

    int numUnique1 = columnDetails->numUnique();
    if (numUnique < 2) continue;

    if (numUnique < 0 || numUnique1 < numUnique) {
      valueColumn = Column(c);
      numUnique   = numUnique1;
    }
  }

  if (valueColumn.isValid())
    analyzeModelData.parameterNameColumn["value"] = valueColumn;
}

CQChartsPlot *
CQChartsWordCloudPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsWordCloudPlot(view, model);
}

//------

CQChartsWordCloudPlot::
CQChartsWordCloudPlot(View *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("wordCloud"), model),
 CQChartsObjTextData<CQChartsWordCloudPlot>(this)
{
}

CQChartsWordCloudPlot::
~CQChartsWordCloudPlot()
{
}

//---

void
CQChartsWordCloudPlot::
init()
{
  CQChartsPlot::init();

  //---

  NoUpdate noUpdate(this);

  setTextColor(Color(Color::Type::INTERFACE_VALUE, 1));

  //---

  addTitle();

  //---

  addColorMapKey();
}

void
CQChartsWordCloudPlot::
term()
{
}

//------

void
CQChartsWordCloudPlot::
setValueColumn(const Column &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsWordCloudPlot::
setCountColumn(const Column &c)
{
  CQChartsUtil::testAndSet(countColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//------

CQChartsColumn
CQChartsWordCloudPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "value") c = this->valueColumn();
  else if (name == "count") c = this->countColumn();
  else                      c = CQChartsPlot::getNamedColumn(name);

  return c;
}

void
CQChartsWordCloudPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "value") this->setValueColumn(c);
  else if (name == "count") this->setCountColumn(c);
  else                      CQChartsPlot::setNamedColumn(name, c);
}

//------

void
CQChartsWordCloudPlot::
addProperties()
{
  addBaseProperties();

  // columns
  addProp("columns", "valueColumn", "value", "Value column");
  addProp("columns", "countColumn", "count", "Count column");

  // text
//addProp("text", "textVisible", "visible", "Text visible");

  addTextProperties("text", "text", "", CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

CQChartsGeom::Range
CQChartsWordCloudPlot::
calcRange() const
{
  // base range always (0, 0) - (1, 1)
  Range dataRange;

  dataRange.updateRange(0.0, 0.0);
  dataRange.updateRange(1.0, 1.0);

  // adjust for equal scale
  if (isEqualScale()) {
    double aspect = this->aspect();

    dataRange.equalScale(aspect);
  }

  return dataRange;
}

//------

bool
CQChartsWordCloudPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsWordCloudPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsWordCloudPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  // value column required, count column optional
  if (! checkColumn(valueColumn(), "Value", /*required*/true))
    columnsValid = false;

  if (! checkColumn(countColumn(), "Count"))
    columnsValid = false;

  if (! columnsValid)
    return false;

  //---

  // get words from model
  struct WordData {
    int         count { 1 };
    QModelIndex ind;

    WordData() = default;

    WordData(int count, const QModelIndex &ind=QModelIndex()) :
     count(count), ind(ind) {
    }
  };

  using WordDatas = std::map<QString, WordData>;

  WordDatas wordDatas;

  if (! countColumn().isValid()) {
    auto *modelData = getModelData();

    auto *details = modelData->details();

    const auto *columnDetails = details->columnDetails(valueColumn());

    for (const auto &valueCount : columnDetails->uniqueValueCounts()) {
      wordDatas[valueCount.first.toString()] = WordData(valueCount.second);
    }
  }
  else {
    class WordCloudModelVisitor : public ModelVisitor {
     public:
      WordCloudModelVisitor(const CQChartsWordCloudPlot *plot, WordDatas &wordDatas) :
       plot_(plot), wordDatas_(wordDatas) {
      }

      State visit(const QAbstractItemModel *, const VisitData &data) override {
        ModelIndex valueModelInd(plot_, data.row, plot_->valueColumn(), data.parent);
        ModelIndex countModelInd(plot_, data.row, plot_->countColumn(), data.parent);

        bool ok1;
        auto name = plot_->modelString(valueModelInd, ok1);
        if (! ok1) return State::SKIP;

        bool ok2;
        int count = plot_->modelInteger(countModelInd, ok2);
        if (! ok2 || count < 1) return State::SKIP;

        auto ind = plot_->normalizeIndex(plot_->modelIndex(valueModelInd));

        auto p = wordDatas_.find(name);

        if (p != wordDatas_.end())
          (*p).second.count += count;
        else
          wordDatas_[name] = WordData(count, ind);

        return State::OK;
      }

     private:
      const CQChartsWordCloudPlot* plot_ { nullptr };
      WordDatas&                   wordDatas_;
    };

    WordCloudModelVisitor visitor(this, wordDatas);

    visitModel(visitor);
  }

  //---

  // place words
  CQChartsWordCloud wordCloud;

  for (const auto &pw : wordDatas)
    wordCloud.addWord(pw.first, pw.second.count);

  wordCloud.setMinFontSize(windowToPixelWidth(0.02));
  wordCloud.setMaxFontSize(windowToPixelWidth(0.15));

  wordCloud.place(this);

  //---

  // create objects
  int i = 0;
  int n = wordCloud.wordDatas().size();

  for (const auto &wordData : wordCloud.wordDatas()) {
    BBox rect(wordData->wordRect.xmin(), wordData->wordRect.ymin(),
              wordData->wordRect.xmax(), wordData->wordRect.ymax());

    auto p = wordDatas.find(wordData->word);
    assert(p != wordDatas.end());

    auto ind = (*p).second.ind;

    ColorInd iv(i, n);

    auto *obj = th->createObj(rect, wordData->word, ind, iv);

    objs.push_back(obj);

    ++i;
  }

  //---

  return true;
}

//---

bool
CQChartsWordCloudPlot::
addMenuItems(QMenu *menu)
{
  bool added = false;

  if (canDrawColorMapKey()) {
    addColorMapKeyItems(menu);

    added = true;
  }

  return added;
}

//---

CQChartsWordObj *
CQChartsWordCloudPlot::
createObj(const BBox &rect, const QString &name, const QModelIndex &ind, const ColorInd &iv)
{
  return new CQChartsWordObj(this, rect, name, ind, iv);
}

//---

bool
CQChartsWordCloudPlot::
hasForeground() const
{
  if (! isLayerActive(CQChartsLayer::Type::FOREGROUND))
    return false;

  return true;
}

void
CQChartsWordCloudPlot::
execDrawForeground(PaintDevice *device) const
{
  if (isColorMapKey())
    drawColorMapKey(device);
}

//---

CQChartsPlotCustomControls *
CQChartsWordCloudPlot::
createCustomControls()
{
  auto *controls = new CQChartsWordCloudPlotCustomControls(charts());

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsWordObj::
CQChartsWordObj(const CQChartsWordCloudPlot *plot, const BBox &rect, const QString &name,
                const QModelIndex &ind, const ColorInd &is) :
 CQChartsPlotObj(const_cast<CQChartsWordCloudPlot *>(plot), rect, is, ColorInd(), ColorInd()),
 plot_(plot), name_(name)
{
  setDetailHint(DetailHint::MAJOR);

  if (ind.isValid())
    setModelInd(ind);
}

QString
CQChartsWordObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(is_.i);
}

QString
CQChartsWordObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  tableTip.addTableRow("Name", name_);

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

void
CQChartsWordObj::
addProperties(CQPropertyViewModel *model, const QString &path)
{
  auto path1 = path + "/" + propertyId();

  model->setObjectRoot(path1, this);

  CQChartsPlotObj::addProperties(model, path1);

  model->addProperty(path1, this, "rect")->setDesc("Bounding box");
  model->addProperty(path1, this, "name")->setDesc("Name");
}

//---

void
CQChartsWordObj::
getObjSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->valueColumn());
  addColumnSelectIndex(inds, plot_->countColumn());
}

void
CQChartsWordObj::
draw(PaintDevice *device) const
{
  // calc stroke and brush
  PenBrush penBrush;

  bool updateState = device->isInteractive();

  calcPenBrush(penBrush, updateState);

  //---

  device->setColorNames();

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  CQChartsTextOptions textOptions;

  textOptions.contrast         = plot_->isTextContrast();
  textOptions.contrastAlpha    = plot_->textContrastAlpha();
  textOptions.scaled           = true;
  textOptions.minScaleFontSize = 4;
  textOptions.maxScaleFontSize = 1000;

  CQChartsDrawUtil::drawTextInBox(device, rect(), name(), textOptions);

  device->resetColorNames();
}

void
CQChartsWordObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  QColor textColor;

  if (plot_->colorColumn().isValid() && plot_->colorType() == CQChartsPlot::ColorType::AUTO) {
    auto ind1 = modelInd();

    ModelIndex ind2(plot_, ind1.row(), plot_->colorColumn(), ind1.parent());

    Color indColor;

    if (plot_->modelIndexColor(ind2, indColor))
      textColor = plot_->interpColor(indColor, colorInd);
    else
      textColor = plot_->interpTextColor(colorInd);
  }
  else {
    textColor = plot_->interpTextColor(colorInd);
  }

  plot_->setPen(penBrush, PenData(true, textColor, plot_->textAlpha()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsWordObj::
writeScriptData(ScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);
}

//------

CQChartsWordCloudPlotCustomControls::
CQChartsWordCloudPlotCustomControls(CQCharts *charts) :
 CQChartsPlotCustomControls(charts, "wordCloud")
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  //---

  // value and count columns
  addColumnWidgets(QStringList() << "value" << "count", columnsFrame);

  //---

  addColorColumnWidgets("Text Color");

  //---

  connectSlots(true);
}

void
CQChartsWordCloudPlotCustomControls::
connectSlots(bool b)
{
  CQChartsPlotCustomControls::connectSlots(b);
}

void
CQChartsWordCloudPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  plot_ = dynamic_cast<CQChartsWordCloudPlot *>(plot);

  CQChartsPlotCustomControls::setPlot(plot);
}

void
CQChartsWordCloudPlotCustomControls::
updateWidgets()
{
  connectSlots(false);

  //---

  CQChartsPlotCustomControls::updateWidgets();

  //---

  connectSlots(true);
}

CQChartsColor
CQChartsWordCloudPlotCustomControls::
getColorValue()
{
  return plot_->textColor();
}

void
CQChartsWordCloudPlotCustomControls::
setColorValue(const CQChartsColor &c)
{
  plot_->setTextColor(c);
}
