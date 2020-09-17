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
   setRequired().setTip("Value column");
  addColumnParameter("count", "Count", "countColumn").
   setTip("Count column");

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
    p("The " + B("Value") + " column specifies the value column.").
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

  CQChartsColumn valueColumn;
//CQChartsColumn countColumn;
  int            numUnique = -1;

  for (int c = 0; c < details->numColumns(); ++c) {
    const auto *columnDetails = details->columnDetails(CQChartsColumn(c));
    if (! columnDetails) continue;

    int numUnique1 = columnDetails->numUnique();
    if (numUnique < 2) continue;

    if (numUnique < 0 || numUnique1 < numUnique) {
      valueColumn = CQChartsColumn(c);
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

  addTitle();
}

void
CQChartsWordCloudPlot::
term()
{
}

//------

void
CQChartsWordCloudPlot::
setValueColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(valueColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsWordCloudPlot::
setCountColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(countColumn_, c, [&]() { updateRangeAndObjs(); } );
}

//------

void
CQChartsWordCloudPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  addBaseProperties();

  // columns
  addProp("column", "valueColumn", "value", "Value column");
  addProp("column", "countColumn", "count", "Count column");

  // text
//addProp("text", "textVisible", "visible", "Text visible");

  addTextProperties("text", "text", "", CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);
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

  CQChartsWordCloud wordCloud;

  if (! countColumn().isValid()) {
    auto *modelData = getModelData();

    auto *details = modelData->details();

    const auto *columnDetails = details->columnDetails(valueColumn());

    for (const auto &valueCount : columnDetails->uniqueValueCounts()) {
      wordCloud.addWord(valueCount.first.toString(), valueCount.second);
    }
  }
  else {
    class WordCloudModelVisitor : public ModelVisitor {
     public:
      WordCloudModelVisitor(const CQChartsWordCloudPlot *plot, CQChartsWordCloud &wordCloud) :
       plot_(plot), wordCloud_(wordCloud) {
      }

      State visit(const QAbstractItemModel *, const VisitData &data) override {
        auto *plot = const_cast<CQChartsWordCloudPlot *>(plot_);

        ModelIndex valueModelInd(plot, data.row, plot_->valueColumn(), data.parent);
        ModelIndex countModelInd(plot, data.row, plot_->countColumn(), data.parent);

        bool ok1;
        QString name = plot_->modelString(valueModelInd, ok1);
        if (! ok1) return State::SKIP;

        bool ok2;
        int count = plot_->modelInteger(countModelInd, ok2);
        if (! ok2 || count < 1) return State::SKIP;

        wordCloud_.addWord(name, count);

        return State::OK;
      }

     private:
      const CQChartsWordCloudPlot* plot_ { nullptr };
      CQChartsWordCloud&           wordCloud_;
    };

    WordCloudModelVisitor visitor(this, wordCloud);

    visitModel(visitor);
  }

  wordCloud.setMinFontSize(windowToPixelWidth(0.02));
  wordCloud.setMaxFontSize(windowToPixelWidth(0.15));

  wordCloud.place(this);

  int i = 0;
  int n = wordCloud.wordDatas().size();

  for (const auto &wordData : wordCloud.wordDatas()) {
    BBox rect(wordData->wordRect.xmin(), wordData->wordRect.ymin(),
              wordData->wordRect.xmax(), wordData->wordRect.ymax());

    QModelIndex ind;
    ColorInd    iv(i, n);

    auto *obj = th->createObj(rect, wordData->word, ind, iv);

    objs.push_back(obj);

    ++i;
  }

  //---

  return true;
}

CQChartsWordObj *
CQChartsWordCloudPlot::
createObj(const BBox &rect, const QString &name, const QModelIndex &ind, const ColorInd &iv)
{
  return new CQChartsWordObj(this, rect, name, ind, iv);
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
  QString path1 = path + "/" + propertyId();

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
draw(CQChartsPaintDevice *device)
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
  ColorInd colorInd = calcColorInd();

  QColor textColor = plot_->interpTextColor(colorInd);

  plot_->setPen(penBrush, PenData(true, textColor, plot_->textAlpha()));

  if (updateState)
    plot_->updateObjPenBrushState(this, penBrush);
}

void
CQChartsWordObj::
writeScriptData(CQChartsScriptPaintDevice *device) const
{
  calcPenBrush(penBrush_, /*updateState*/ false);

  CQChartsPlotObj::writeScriptData(device);
}
