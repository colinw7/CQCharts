#include <CQChartsDendrogramPlot.h>
#include <CQChartsView.h>
#include <CQChartsModelUtil.h>
#include <CQChartsDrawUtil.h>
#include <CQCharts.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQChartsPlotParameterEdit.h>
#include <CQChartsHtml.h>
#include <CQChartsTip.h>
#include <CQChartsNamePair.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>

#include <CBuchHeim.h>

namespace {

class Tree : public CBuchHeim::Tree {
 public:
  using Node = CQChartsDendrogram::Node;

 public:
  Tree(Node *node) :
   CBuchHeim::Tree(node->name().toStdString()), node_(node) {
  }

  Node *node() const { return node_; }

 private:
  Node *node_ { nullptr };
};

//---

class PlotDendrogram : public CQChartsDendrogram {
 public:
  using ModelIndex = CQChartsModelIndex;
  using Color      = CQChartsColor;
  using OptReal    = CQChartsOptReal;

 public:
  class PlotNode : public CQChartsDendrogram::Node {
   public:
    PlotNode(CQChartsDendrogram::Node *parent, const QString &name="",
             const OptReal &size=OptReal()) :
     CQChartsDendrogram::Node(parent, name,
       size.isSet() ? size.real() : CQChartsDendrogram::OptReal()) {
    }

    const ModelIndex &modelInd() const { return modelInd_; }
    void setModelInd(const ModelIndex &v) { modelInd_ = v; }

    const QModelIndex &ind() const { return ind_; }
    void setInd(const QModelIndex &v) { ind_ = v; }

    const Color &color() const { return color_; }
    void setColor(const Color &c) { color_ = c; }

    const OptReal &colorValue() const { return colorValue_; }
    void setColorValue(const OptReal &r) { colorValue_ = r; }

    const OptReal &sizeValue() const { return sizeValue_; }
    void setSizeValue(const OptReal &v) { sizeValue_ = v; }

    const Color &edgeColor(const PlotNode *toNode) const {
      return getIdColor(toNode, edgeColor_); }
    void setEdgeColor(PlotNode *toNode, const Color &c) {
      edgeColor_[toNode->id()] = c; }

    const OptReal &edgeColorValue(const PlotNode *toNode) const {
      return getIdOptReal(toNode, edgeColorValue_); }
    void setEdgeColorValue(PlotNode *toNode, const OptReal &r) {
      edgeColorValue_[toNode->id()] = r; }

    const OptReal &edgeSizeValue(const PlotNode *toNode) const {
      return getIdOptReal(toNode, edgeSizeValue_); }
    void setEdgeSizeValue(PlotNode *toNode, const OptReal &v) {
      edgeSizeValue_[toNode->id()] = v; }

   private:
    using IdColor   = std::map<uint, Color>;
    using IdOptReal = std::map<uint, OptReal>;

    const Color &getIdColor(const PlotNode *toNode, const IdColor &idColor) const {
      static Color c;
      auto p = idColor.find(toNode->id());
      return (p != idColor.end() ? (*p).second : c);
    }

    const OptReal &getIdOptReal(const PlotNode *toNode, const IdOptReal &idOptReal) const {
      static OptReal r;
      auto p = idOptReal.find(toNode->id());
      return (p != idOptReal.end() ? (*p).second : r);
    }

   private:
    ModelIndex  modelInd_;
    QModelIndex ind_;
    Color       color_;
    OptReal     colorValue_;
    OptReal     sizeValue_;
    IdColor     edgeColor_;
    IdOptReal   edgeColorValue_;
    IdOptReal   edgeSizeValue_;
  };

 public:
  PlotDendrogram() :
   CQChartsDendrogram() {
  }

  Node *createNode(CQChartsDendrogram::Node *hier, const QString &name,
                   const CQChartsDendrogram::OptReal &size) const override {
    return new PlotNode(hier, name, size);
  }
};

}

//---

CQChartsDendrogramPlotType::
CQChartsDendrogramPlotType()
{
}

void
CQChartsDendrogramPlotType::
addParameters()
{
  CQChartsHierPlotType::addHierParameters("Dendrogram");

  //---

  // options
  addEnumParameter("orientation", "Orientation", "orientation").
    addNameValue("HORIZONTAL", static_cast<int>(Qt::Horizontal)).
    addNameValue("VERTICAL"  , static_cast<int>(Qt::Vertical  )).
    setTip("Draw orientation");

  //---

  CQChartsPlotType::addParameters();
}

void
CQChartsDendrogramPlotType::
addExtraHierParameters()
{
  addColumnParameter("link", "Link", "linkColumn").
   setStringColumn ().setRequired().setPropPath("columns.link").setTip("Link column");
  addColumnParameter("size", "Size", "sizeColumn").
   setNumericColumn().setPropPath("columns.size").setTip("Size column");
}

QString
CQChartsDendrogramPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Dendrogram Plot").
    h3("Summary").
     p("Draw hierarchical data using collapsible tree.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/dendrogram.png"));
}

CQChartsPlot *
CQChartsDendrogramPlotType::
create(View *view, const ModelP &model) const
{
  return new CQChartsDendrogramPlot(view, model);
}

//------

CQChartsDendrogramPlot::
CQChartsDendrogramPlot(View *view, const ModelP &model) :
 CQChartsHierPlot(view, view->charts()->plotType("dendrogram"), model),
 CQChartsObjRootShapeData<CQChartsDendrogramPlot>(this),
 CQChartsObjHierShapeData<CQChartsDendrogramPlot>(this),
 CQChartsObjLeafShapeData<CQChartsDendrogramPlot>(this),
 CQChartsObjEdgeShapeData<CQChartsDendrogramPlot>(this),
 CQChartsObjRootTextData <CQChartsDendrogramPlot>(this),
 CQChartsObjHierTextData <CQChartsDendrogramPlot>(this),
 CQChartsObjLeafTextData <CQChartsDendrogramPlot>(this)
{
}

CQChartsDendrogramPlot::
~CQChartsDendrogramPlot()
{
  CQChartsDendrogramPlot::term();
}

//---

void
CQChartsDendrogramPlot::
init()
{
  CQChartsHierPlot::init();

  //---

  NoUpdate noUpdate(this);

  setRootFillColor(Color::makePalette());
  setHierFillColor(Color::makePalette());
  setLeafFillColor(Color::makePalette());

  setEdgeFillColor(Color::makeInterfaceValue(0.2));

  setRootTextPosition(TextPosition::CENTER);
  setHierTextPosition(TextPosition::LEFT);
  setLeafTextPosition(TextPosition::RIGHT);

  setRootShape(CQChartsShapeType(CQChartsShapeType::Type::DIAMOND));
  setHierShape(CQChartsShapeType(CQChartsShapeType::Type::DIAMOND));
  setLeafShape(CQChartsShapeType(CQChartsShapeType::Type::CIRCLE));

  //---

//addKey();

  addTitle();

  //---

  addColorMapKey();

  //---

  registerSlot("expand"      , QStringList());
  registerSlot("expand_all"  , QStringList());
  registerSlot("collapse_all", QStringList());
}

void
CQChartsDendrogramPlot::
term()
{
}

//---

void
CQChartsDendrogramPlot::
setLinkColumn(const Column &c)
{
  CQChartsUtil::testAndSet(linkColumn_, c, [&]() {
    cacheData_.needsReload = true; updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

void
CQChartsDendrogramPlot::
setColorColumn(const Column &c)
{
  if (c != colorColumnData_.column()) {
    colorColumnData_.setColumn(c);
    cacheData_.needsReload = true; updateRangeAndObjs(); Q_EMIT customDataChanged();
  }
}

void
CQChartsDendrogramPlot::
setSizeColumn(const Column &c)
{
  CQChartsUtil::testAndSet(sizeColumn_, c, [&]() {
    cacheData_.needsReload = true; updateRangeAndObjs(); Q_EMIT customDataChanged();
  } );
}

//---

void
CQChartsDendrogramPlot::
setRootVisible(bool b)
{
  CQChartsUtil::testAndSet(rootVisible_, b, [&]() {
    updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setRootSize(const Length &s)
{
  CQChartsUtil::testAndSet(rootNodeData_.size, s, [&]() {
    cacheData_.needsPlace = true; updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setRootMinSize(const Length &s)
{
  CQChartsUtil::testAndSet(rootNodeData_.minSize, s, [&]() {
    cacheData_.needsPlace = true; updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setRootShape(const CQChartsShapeType &s)
{
  CQChartsUtil::testAndSet(rootNodeData_.shape, s, [&]() {
    drawObjs();
  } );
}

void
CQChartsDendrogramPlot::
setRootAspect(double a)
{
  CQChartsUtil::testAndSet(rootNodeData_.aspect, a, [&]() {
    cacheData_.needsPlace = true; updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setRootTextMargin(double r)
{
  CQChartsUtil::testAndSet(rootNodeData_.textMargin, r, [&]() {
    drawObjs();
  } );
}

void
CQChartsDendrogramPlot::
setRootValueLabel(bool b)
{
  CQChartsUtil::testAndSet(rootNodeData_.valueLabel, b, [&]() {
    drawObjs();
  } );
}

double
CQChartsDendrogramPlot::
calcRootSize() const
{
  // get specified shape size
  auto ss = lengthPixelWidth(rootSize());

  // if negative set shape size based on font size
  if (ss <= 0) {
    auto font = view()->plotFont(this, rootTextFont());

    QFontMetricsF fm(font);

    ss = fm.height() + 2.0;
  }

  return ss;
}

//---

void
CQChartsDendrogramPlot::
setHierSize(const Length &s)
{
  CQChartsUtil::testAndSet(hierNodeData_.size, s, [&]() {
    cacheData_.needsPlace = true; updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setHierMinSize(const Length &s)
{
  CQChartsUtil::testAndSet(hierNodeData_.minSize, s, [&]() {
    cacheData_.needsPlace = true; updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setHierShape(const CQChartsShapeType &s)
{
  CQChartsUtil::testAndSet(hierNodeData_.shape, s, [&]() {
    drawObjs();
  } );
}

void
CQChartsDendrogramPlot::
setHierAspect(double a)
{
  CQChartsUtil::testAndSet(hierNodeData_.aspect, a, [&]() {
    cacheData_.needsPlace = true; updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setHierTextMargin(double r)
{
  CQChartsUtil::testAndSet(hierNodeData_.textMargin, r, [&]() {
    drawObjs();
  } );
}

void
CQChartsDendrogramPlot::
setHierValueLabel(bool b)
{
  CQChartsUtil::testAndSet(hierNodeData_.valueLabel, b, [&]() {
    drawObjs();
  } );
}

double
CQChartsDendrogramPlot::
calcHierSize() const
{
  // get specified shape size
  auto ss = lengthPixelWidth(hierSize());

  // if negative set shape size based on font size
  if (ss <= 0) {
    auto font = view()->plotFont(this, hierTextFont());

    QFontMetricsF fm(font);

    ss = fm.height() + 2.0;
  }

  return ss;
}

//---

void
CQChartsDendrogramPlot::
setLeafSize(const Length &s)
{
  CQChartsUtil::testAndSet(leafNodeData_.size, s, [&]() {
    cacheData_.needsPlace = true; updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setLeafMinSize(const Length &s)
{
  CQChartsUtil::testAndSet(leafNodeData_.minSize, s, [&]() {
    cacheData_.needsPlace = true; updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setLeafShape(const CQChartsShapeType &s)
{
  CQChartsUtil::testAndSet(leafNodeData_.shape, s, [&]() {
    drawObjs();
  } );
}

void
CQChartsDendrogramPlot::
setLeafAspect(double a)
{
  CQChartsUtil::testAndSet(leafNodeData_.aspect, a, [&]() {
    cacheData_.needsPlace = true; updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setLeafTextMargin(double r)
{
  CQChartsUtil::testAndSet(leafNodeData_.textMargin, r, [&]() {
    drawObjs();
  } );
}

void
CQChartsDendrogramPlot::
setLeafValueLabel(bool b)
{
  CQChartsUtil::testAndSet(leafNodeData_.valueLabel, b, [&]() {
    drawObjs();
  } );
}

double
CQChartsDendrogramPlot::
calcLeafSize() const
{
  // get specified shape size
  auto ss = lengthPixelWidth(leafSize());

  // if negative set shape size based on font size
  if (ss <= 0) {
    auto font = view()->plotFont(this, leafTextFont());

    QFontMetricsF fm(font);

    ss = fm.height() + 2.0;
  }

  return ss;
}

void
CQChartsDendrogramPlot::
setNodeColorByValue(const ValueType &v)
{
  CQChartsUtil::testAndSet(leafNodeData_.colorByValue, v, [&]() { drawObjs(); } );
}

void
CQChartsDendrogramPlot::
setNodeSizeByValue(const ValueType &v)
{
  CQChartsUtil::testAndSet(leafNodeData_.sizeByValue, v, [&]() { drawObjs(); } );
}

void
CQChartsDendrogramPlot::
setColorClosed(bool b)
{
  CQChartsUtil::testAndSet(colorClosed_, b, [&]() { drawObjs(); } );
}

void
CQChartsDendrogramPlot::
setSwatchColor(bool b)
{
  CQChartsUtil::testAndSet(swatchColor_, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsDendrogramPlot::
setEdgeWidth(const Length &l)
{
  CQChartsUtil::testAndSet(edgeData_.width, l, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDendrogramPlot::
setMinEdgeWidth(const Length &l)
{
  CQChartsUtil::testAndSet(edgeData_.minWidth, l, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsDendrogramPlot::
setEdgeColorByValue(bool b)
{
  CQChartsUtil::testAndSet(edgeData_.colorByValue, b, [&]() { drawObjs(); } );
}

void
CQChartsDendrogramPlot::
setEdgeSizeByValue(bool b)
{
  CQChartsUtil::testAndSet(edgeData_.sizeByValue, b, [&]() { drawObjs(); } );
}

void
CQChartsDendrogramPlot::
setEdgeSelectable(bool b)
{
  CQChartsUtil::testAndSet(edgeData_.selectable, b, [&]() {
    for (const auto &plotObj : plotObjects()) {
      auto *edgeObj = dynamic_cast<EdgeObj *>(plotObj);
      if (! edgeObj) continue;

      edgeObj->setSelectable(edgeData_.selectable);
    }
  } );
}

//---

double
CQChartsDendrogramPlot::
mapHierValue(double value) const
{
  auto *root = rootNodeObj();
  if (! root) return 0.0;

  if (! root->hierValue().isSet())
    return 0.0;

  RMinMax valueRange(0.0, root->hierValue().real());

  return valueRange.map(value, 0.0, 1.0);
}

double
CQChartsDendrogramPlot::
mapValue(double value) const
{
  return valueRange_.map(value, 0.0, 1.0);
}

double
CQChartsDendrogramPlot::
mapColor(double value) const
{
  return colorRange_.map(value, 0.0, 1.0);
}

double
CQChartsDendrogramPlot::
mapSize(double value) const
{
  return sizeRange_.map(value, 0.0, 1.0);
}

double
CQChartsDendrogramPlot::
mapNodeValue(double value) const
{
  return nodeValueRange_.map(value, 0.0, 1.0);
}

double
CQChartsDendrogramPlot::
mapEdgeValue(double value) const
{
  return edgeValueRange_.map(value, 0.0, 1.0);
}

//---

void
CQChartsDendrogramPlot::
setRootTextPosition(const TextPosition &p)
{
  CQChartsUtil::testAndSet(rootNodeData_.textPosition, p, [&]() {
    drawObjs();
  } );
}

void
CQChartsDendrogramPlot::
setRootRotatedText(bool b)
{
  CQChartsUtil::testAndSet(rootNodeData_.rotatedText, b, [&]() { drawObjs(); } );
}

void
CQChartsDendrogramPlot::
setHierTextPosition(const TextPosition &p)
{
  CQChartsUtil::testAndSet(hierNodeData_.textPosition, p, [&]() {
    drawObjs();
  } );
}

void
CQChartsDendrogramPlot::
setHierRotatedText(bool b)
{
  CQChartsUtil::testAndSet(hierNodeData_.rotatedText, b, [&]() { drawObjs(); } );
}

void
CQChartsDendrogramPlot::
setLeafTextPosition(const TextPosition &p)
{
  CQChartsUtil::testAndSet(leafNodeData_.textPosition, p, [&]() {
    drawObjs();
  } );
}

void
CQChartsDendrogramPlot::
setLeafRotatedText(bool b)
{
  CQChartsUtil::testAndSet(leafNodeData_.rotatedText, b, [&]() { drawObjs(); } );
}

//---

void
CQChartsDendrogramPlot::
setOrientation(const Qt::Orientation &orient)
{
  CQChartsUtil::testAndSet(orientation_, orient, [&]() {
    cacheData_.needsPlace = true; updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setPlaceType(const PlaceType &t)
{
  CQChartsUtil::testAndSet(placeType_, t, [&]() {
    cacheData_.needsPlace = true; updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setHideDepth(int d)
{
  CQChartsUtil::testAndSet(hideDepth_, d, [&]() {
    cacheData_.needsPlace = true; updateRangeAndObjs();
  } );
}

void
CQChartsDendrogramPlot::
setSizeScale(double r)
{
  CQChartsUtil::testAndSet(sizeScale_, r, [&]() {
    drawObjs();
  } );
}

//---

void
CQChartsDendrogramPlot::
setRemoveNodeOverlaps(bool b)
{
  CQChartsUtil::testAndSet(removeNodeOverlaps_, b, [&]() {
    // if enabling overlap removal save current (non overlap) positions
    if (isRemoveNodeOverlaps()) {
      saveOverlapRects();

      //updateRangeAndObjs();
      execRemoveOverlaps();
    }
    // if disabling overlap removal restore saved (non overlap) positions
    else {
      overlapScale_ = 1.0;

      restoreOverlapRects();
    }

    drawObjs();
  } );
}

void
CQChartsDendrogramPlot::
setRemoveGroupOverlaps(bool b)
{
  CQChartsUtil::testAndSet(removeGroupOverlaps_, b, [&]() {
    if (isRemoveNodeOverlaps()) {
      //updateRangeAndObjs();
      overlapScale_ = 1.0;

      restoreOverlapRects();

      execRemoveOverlaps(); drawObjs();
    }
  } );
}

void
CQChartsDendrogramPlot::
setAdjustOverlaps(bool b)
{
  CQChartsUtil::testAndSet(adjustOverlaps_, b, [&]() {
    if (isRemoveNodeOverlaps()) {
      //updateRangeAndObjs();
      overlapScale_ = 1.0;

      restoreOverlapRects();

      execRemoveOverlaps(); drawObjs();
    }
  } );
}

void
CQChartsDendrogramPlot::
setOverlapMargin(const Length &l)
{
  CQChartsUtil::testAndSet(overlapMargin_, l, [&]() {
    if (isRemoveNodeOverlaps()) {
      //updateRangeAndObjs();
      overlapScale_ = 1.0;

      restoreOverlapRects();

      execRemoveOverlaps(); drawObjs();
    }
  } );
}

//---

CQChartsColumn
CQChartsDendrogramPlot::
getNamedColumn(const QString &name) const
{
  Column c;
  if      (name == "link") c = this->linkColumn();
  else if (name == "size") c = this->sizeColumn();
  else                     c = CQChartsHierPlot::getNamedColumn(name);

  return c;
}

void
CQChartsDendrogramPlot::
setNamedColumn(const QString &name, const Column &c)
{
  if      (name == "link") this->setLinkColumn(c);
  else if (name == "size") this->setSizeColumn(c);
  else                     CQChartsHierPlot::setNamedColumn(name, c);
}

//---

void
CQChartsDendrogramPlot::
addProperties()
{
  addHierProperties();

  // columns
  addProp("columns", "linkColumn", "link" , "Link column");
  addProp("columns", "sizeColumn", "size" , "Size column");

  // options
  addProp("options", "followViewExpand", "", "Follow view expand");

  addProp("options", "orientation", "orientation", "Draw orientation");
  addProp("options", "placeType"  , "placeType"  , "Place type");
  addProp("options", "hideDepth"  , "hideDepth"  , "Hide depth");
  addProp("options", "sizeScale"  , "sizeScale"  , "Size scale");

  addProp("options", "removeNodeOverlaps" , "removeNodeOverlaps" , "Remove node overlaps");
  addProp("options", "removeGroupOverlaps", "removeGroupOverlaps", "Remove group overlaps");
  addProp("options", "adjustOverlaps"     , "adjustOverlaps"     , "Adjust overlap");
  addProp("options", "overlapMargin"      , "overlapMargin"      , "Overlap margin");

  addProp("options", "hierValueTip", "hierValueTip", "Show hier value in tip");

  // root
  addProp("root"      , "rootVisible"     , "visible"   , "Root is visible");
  addProp("root"      , "rootSize"        , "size"      , "Node shape size");
  addProp("root"      , "rootMinSize"     , "minSize"   , "Node shape min size");
  addProp("root"      , "rootAspect"      , "aspect"    , "Node shape aspect");
  addProp("root"      , "rootShape"       , "shape"     , "Root shape");
  addProp("root/label", "rootTextPosition", "position"  , "Root label position");
  addProp("root/label", "rootRotatedText" , "rotated"   , "Root is rotated text");
  addProp("root/label", "rootTextMargin"  , "margin"    , "Root text margin in pixels")->
    setMinValue(1.0);
  addProp("root/label", "rootValueLabel"  , "valueLabel", "Root value label");

  addProp("root/stroke", "rootStroked", "visible", "Root stroke visible");
  addLineProperties("root/stroke", "rootStroke", "Root");

  addProp("root/fill", "rootFilled", "visible", "Root fill visible");
  addFillProperties("root/fill", "rootFill", "Root");

  addTextProperties("root/label", "rootText", "Root Label",
                    CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::SCALED |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  // hier
  addProp("hier"      , "hierSize"        , "size"      , "Hier shape size");
  addProp("hier"      , "hierMinSize"     , "minSize"   , "Hier shape min size");
  addProp("hier"      , "hierAspect"      , "aspect"    , "Hier shape aspect");
  addProp("hier"      , "hierShape"       , "shape"     , "Hier shape");
  addProp("hier/label", "hierTextVisible" , "visible"   , "Hier labels visible");
  addProp("hier/label", "hierTextPosition", "position"  , "Hier labels position");
  addProp("hier/label", "hierRotatedText" , "rotated"   , "Hier is rotated text");
  addProp("hier/label", "hierTextMargin"  , "margin"    , "Hier text margin in pixels")->
    setMinValue(1.0);
  addProp("hier/label", "hierValueLabel"  , "valueLabel", "Hier value label");

  addProp("hier/stroke", "hierStroked", "visible", "Hier stroke visible");
  addLineProperties("hier/stroke", "hierStroke", "Hier");

  addProp("hier/fill", "hierFilled", "visible", "Hier fill visible");
  addFillProperties("hier/fill", "hierFill", "Hier");

  addTextProperties("hier/label", "hierText", "Hier Label",
                    CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::SCALED |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  // leaf
  addProp("leaf"      , "leafSize"        , "size"      , "Leaf shape size");
  addProp("leaf"      , "leafMinSize"     , "minSize"   , "Leaf shape min size");
  addProp("leaf"      , "leafAspect"      , "aspect"    , "Leaf shape aspect");
  addProp("leaf"      , "leafShape"       , "shape"     , "Leaf shape");
  addProp("leaf/label", "leafTextVisible" , "visible"   , "Leaf labels visible");
  addProp("leaf/label", "leafTextPosition", "position"  , "Leaf labels position");
  addProp("leaf/label", "leafRotatedText" , "rotated"   , "Leaf is rotated text");
  addProp("leaf/label", "leafTextMargin"  , "margin"    , "Leaf text margin in pixels")->
    setMinValue(1.0);
  addProp("leaf/label", "leafValueLabel"  , "valueLabel", "Leaf value label");

  addTextProperties("leaf/label", "leafText", "Leaf Label",
                    CQChartsTextOptions::ValueType::CONTRAST |
                    CQChartsTextOptions::ValueType::SCALED |
                    CQChartsTextOptions::ValueType::CLIP_LENGTH |
                    CQChartsTextOptions::ValueType::CLIP_ELIDE);

  addProp("leaf/stroke", "leafStroked", "visible", "Leaf stroke visible");
  addLineProperties("leaf/stroke", "leafStroke", "Leaf");

  addProp("leaf/fill", "leafFilled", "visible", "Leaf fill visible");
  addFillProperties("leaf/fill", "leafFill", "Leaf");

  addProp("node", "nodeColorByValue", "colorByValue", "Node is color by value");
  addProp("node", "nodeSizeByValue" , "sizeByValue" , "Node is size by value");
  addProp("node", "colorClosed"     , "colorClosed" , "Node color closed");
  addProp("node", "swatchColor"     , "swatchColor" , "Node swatch color");

  // general edge
  addProp("edge", "edgeWidth"       , "width"       , "Edge width");
  addProp("edge", "minEdgeWidth"    , "minWidth"    , "Min Edge width");
  addProp("edge", "edgeColorByValue", "colorByValue", "Edge is color by value");
  addProp("edge", "edgeSizeByValue" , "sizeByValue" , "Edge is size by value");
  addProp("edge", "edgeSelectable"  , "selectable"  , "Edge is selectable");

  addProp("edge/stroke", "edgeStroked", "visible", "Edge stroke visible");
  addLineProperties("edge/stroke", "edgeStroke", "Edge");

  addProp("edge/fill", "edgeFilled", "visible", "Edge fill visible");
  addFillProperties("edge/fill", "edgeFill", "Edge");

  //---

  // color map
  addColorMapProperties();

  // color map key
  addColorMapKeyProperties();
}

//---

CQChartsGeom::Range
CQChartsDendrogramPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsDendrogramPlot::calcRange");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsDendrogramPlot *>(this);

  th->clearErrors();

  //---

  // check columns
  bool columnsValid = true;

  if (! checkColumns      (nameColumns(), "Name" )) columnsValid = false;
  if (! checkColumn       (linkColumn (), "Link" )) columnsValid = false;
  if (! checkNumericColumn(valueColumn(), "Value")) columnsValid = false;
  if (! checkNumericColumn(sizeColumn (), "Size" )) columnsValid = false;

  // attributes optional
  if (! checkColumn(attributesColumn(), "Attributes")) columnsValid = false;

  if (! columnsValid)
    return Range(0.0, 0.0, 1.0, 1.0);

  //---

  // fixed range (data scaled to fit)
  Range dataRange;

  if (placeType() == PlaceType::CIRCULAR) {
    dataRange.updateRange(-1, -1);
    dataRange.updateRange( 1,  1);
  }
  else {
    dataRange.updateRange(0, 0);
    dataRange.updateRange(1, 1);
  }

  //---

  return dataRange;
}

//------

void
CQChartsDendrogramPlot::
clearPlotObjects()
{
  CQChartsHierPlot::clearPlotObjects();

  cacheData_.nodeObjs.clear();
}

bool
CQChartsDendrogramPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsDendrogramPlot::createObjs");

  NoUpdate noUpdate(this);

  auto *th = const_cast<CQChartsDendrogramPlot *>(this);

  th->clearErrors();

  //---

  th->overlapScale_ = 1.0;

  //---

  if (cacheData_.needsReload) {
    placeModel();

    cacheData_.needsReload = false;
  }
  else if (cacheData_.needsPlace) {
    place();

    cacheData_.needsPlace = false;
  }

  //---

  cacheData_.nodeObjs.clear();

  auto *root = rootNode();

  //---

  PlotObjs nodeObjs;

  cacheData_.numNodes = 1;
  cacheData_.numEdges = 1;
  cacheData_.depth    = 1;
  cacheData_.nodeInd  = 1;
  cacheData_.edgeInd  = 1;

  if (root) {
    cacheData_.numNodes = root->maxNodes();
    cacheData_.numEdges = root->maxEdges();
    cacheData_.depth    = root->calcDepth();

    cacheData_.rootNodeObj = addNodeObj(root, nodeObjs, /*hier*/true);

    cacheData_.rootNodeObj->setRoot(true);

    addNodeObjs(root, 0, cacheData_.rootNodeObj, nodeObjs);
  }
  else
    cacheData_.rootNodeObj = nullptr;

  //---

  th->nodeValueRange_.reset();
  th->edgeValueRange_.reset();

  // init sum of child values for all hier nodes (including root)
  for (auto *plotObj : nodeObjs) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);
    if (! nodeObj) continue;

    auto *hierNode = dynamic_cast<const Node *>(nodeObj->node());

    if (hierNode) {
      nodeObj->setHierColor(calcHierColor(hierNode));
      nodeObj->setHierSize (calcHierSize (hierNode));
    }

    cacheData_.nodeObjs[const_cast<Node *>(hierNode)] = nodeObj;

    if (hierNode->size())
      th->nodeValueRange_.add(*hierNode->size());
  }

  //---

  // create edge objs
  using EdgeObjs = std::vector<EdgeObj *>;

  EdgeObjs edgeObjs;

  for (auto *plotObj : nodeObjs) {
    auto *fromObj = dynamic_cast<NodeObj *>(plotObj);
    if (! fromObj) continue;

    for (const auto &child : fromObj->children()) {
      auto *toObj = child.node;

      auto *edgeObj = addEdgeObj(fromObj, toObj);

      edgeObjs.push_back(edgeObj);

      if (edgeObj->value().isSet())
        th->edgeValueRange_.add(edgeObj->value().real());
    }
  }

  //---

  // ensure root is open
  ModelIndex rootModelInd(this, 0, nameColumns().column(), QModelIndex());

  auto rootModelInd1 = modelIndex(rootModelInd);

  if (rootNodeObj())
    rootNodeObj()->setModelIndex(rootModelInd);

  if (rootNodeObj() && rootModelInd1.isValid())
    rootNodeObj()->setModelInd(normalizeIndex(rootModelInd1));

  if (rootNodeObj())
    th->setOpen(rootNodeObj(), true);

  //---

  // move nodes if root not visible
  if (! isRootVisible()) {
    th->execMoveNonRoot(nodeObjs);
  }

  // remove overlaps
  if (placeType() == PlaceType::BUCHHEIM) {
    if (isRemoveNodeOverlaps()) {
      th->saveOverlapRects(nodeObjs);

      th->execRemoveOverlaps(nodeObjs);
    }
    else
      th->saveOverlapRects(nodeObjs);
  }

  //th->calcNodeSize(nodeObjs);

  //---

  objs.reserve(edgeObjs.size() + nodeObjs.size());

  for (auto *edgeObj : edgeObjs)
    objs.push_back(edgeObj);

  for (auto *nodeObj : nodeObjs)
    objs.push_back(nodeObj);

  return true;
}

//---

bool
CQChartsDendrogramPlot::
isEdgeValue() const
{
  return (linkColumn().isValid());
}

//---

void
CQChartsDendrogramPlot::
placeModel() const
{
  auto *th = const_cast<CQChartsDendrogramPlot *>(this);

  for (const auto &plotObj : plotObjects()) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);

    if (nodeObj)
      nodeObj->resetNode();
  }

  th->dendrogram_ = std::make_unique<PlotDendrogram>();

  //---

  th->tempRoot_ = th->dendrogram_->createNode(nullptr, "tempRoot");
  th->tempRoot_->setTemp(true);

  //---

  // add name values
  class HierRowVisitor : public ModelVisitor {
   public:
    using DendrogramPlot = CQChartsDendrogramPlot;

   public:
    HierRowVisitor(const DendrogramPlot *dendrogramPlot, Edges &edges) :
     dendrogramPlot_(dendrogramPlot), edges_(edges) {
    }

    State hierVisit(const QAbstractItemModel *model, const VisitData &data) override {
      if (dendrogramPlot_->nameColumns().isValid()) {
        // get name and associated model index for row
        QString    name;
        ModelIndex modelNameInd;

        auto ok = getName(data, modelNameInd, name);

        // add parent path
        auto path = CQChartsModelUtil::parentPath(model, data.parent);

        if (ok && path.length())
          name = path + "/" + name;

        dendrogramPlot_->addHierName(name, modelNameInd);
      }

      return State::OK;
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      // get name and associated model index for row
      QString          name;
      CQChartsNamePair namePair;
      ModelIndex       modelNameInd;

      // get name
      if      (dendrogramPlot_->nameColumns().isValid()) {
        // get node name
        auto ok = getName(data, modelNameInd, name);

        // add parent path
        auto path = CQChartsModelUtil::parentPath(model, data.parent);

        if (ok && path.length())
          name = path + "/" + name;
      }
      // get link
      else if (dendrogramPlot_->linkColumn().isValid()) {
        // get link (from/to)
        QString linkStr;

        (void) getLink(data, linkStr);

        namePair = CQChartsNamePair(linkStr, "/");
      }
      else
        return State::SKIP;

      //--

      // get node or edge value
      OptReal value;

      ModelIndex valueInd;
      double     rvalue = 0.0;

      if (getValue(data, valueInd, rvalue)) {
        if (! CMathUtil::isNaN(rvalue))
          value = OptReal(rvalue);
        else
          return State::SKIP;
      }
      else {
        if (! dendrogramPlot_->isSkipBad())
          return addDataError(valueInd, "Invalid Value");
      }

      //---

      // get node or edge color
      Color   color;
      OptReal colorValue;

      if (dendrogramPlot_->colorColumn().isValid()) {
        ModelIndex colorInd;

        if (! getColor(data, colorInd, color, colorValue)) {
          if (! dendrogramPlot_->isSkipBad())
            return addDataError(colorInd, "Invalid Color");
        }
      }

      //---

      // get node or edge size
      OptReal sizeValue;

      if (dendrogramPlot_->sizeColumn().isValid()) {
        double     size = 0.0;
        ModelIndex sizeInd;

        if (getSize(data, sizeInd, size)) {
          if (! CMathUtil::isNaN(size))
            sizeValue = OptReal(size);
        }
        else {
          if (! dendrogramPlot_->isSkipBad())
            return addDataError(sizeInd, "Invalid Size");
        }
      }

      //---

      // get attributes from optional column
      CQChartsNameValues nameValues;

      if (dendrogramPlot_->attributesColumn().isValid()) {
        ModelIndex attributesModelInd(dendrogramPlot_, data.row,
                                      dendrogramPlot_->attributesColumn(), data.parent);

        bool ok;
        auto attributesStr = dendrogramPlot_->modelString(attributesModelInd, ok);
        if (! ok) return State::SKIP;

        nameValues = CQChartsNameValues(attributesStr);
      }

      //---

      // add to nodes/edges
      dendrogramPlot_->addNameValue(name, namePair, QStringList(),
                                    value, modelNameInd, color, colorValue, sizeValue,
                                    nameValues, edges_);

      //---

      return State::OK;
    }

    bool getName(const VisitData &data, ModelIndex &nameModelInd, QString &name) const {
      // get node name
      nameModelInd = ModelIndex(dendrogramPlot_, data.row, dendrogramPlot_->nameColumns().column(),
                                data.parent);

      bool ok;
      name = dendrogramPlot_->modelString(nameModelInd, ok);
      return ok;
    }

    bool getLink(const VisitData &data, QString &link) const {
      // get link (from/to)
      auto linkModelInd = ModelIndex(dendrogramPlot_, data.row, dendrogramPlot_->linkColumn(),
                                     data.parent);

      bool ok;
      link = dendrogramPlot_->modelString(linkModelInd, ok);
      return ok;
    }

    bool getValue(const VisitData &data, ModelIndex &valueInd, double &value) {
      // get node or edge value
      valueInd = ModelIndex(dendrogramPlot_, data.row, dendrogramPlot_->valueColumn(),
                            data.parent);

      bool ok;
      value = dendrogramPlot_->modelReal(valueInd, ok);

      if (! CMathUtil::isNaN(value))
        valueRange_.add(value);

      return ok;
    }

    bool getColor(const VisitData &data, ModelIndex &colorInd, Color &color, OptReal &colorValue) {
      colorInd = ModelIndex(dendrogramPlot_, data.row, dendrogramPlot_->colorColumn(),
                            data.parent);

      bool ok;
      auto var = dendrogramPlot_->modelValue(colorInd, ok);
      if (! ok) return false;

      auto r = CQChartsVariant::toReal(var, ok);

      if (ok) {
        if (CMathUtil::isNaN(r)) return false;

        color      = Color::makePaletteValue(r);
        colorValue = OptReal(r);

        colorRange_.add(r);
      }
      else {
        ok = dendrogramPlot_->columnValueColor(var, color);
      }

      return ok;
    }

    bool getSize(const VisitData &data, ModelIndex &sizeInd, double &size) {
      sizeInd = ModelIndex(dendrogramPlot_, data.row, dendrogramPlot_->sizeColumn(),
                           data.parent);

      bool ok;
      size = dendrogramPlot_->modelReal(sizeInd, ok);
      if (! ok) return false;

      sizeRange_.add(size);

      return true;
    }

    const RMinMax &valueRange() const { return valueRange_; }
    const RMinMax &colorRange() const { return colorRange_; }
    const RMinMax &sizeRange () const { return sizeRange_; }

   private:
    State addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<DendrogramPlot *>(dendrogramPlot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const DendrogramPlot*  dendrogramPlot_ { nullptr };
    DendrogramPlot::Edges& edges_;
    RMinMax                valueRange_;
    RMinMax                colorRange_;
    RMinMax                sizeRange_;
  };

  //---

  class FlatRowVisitor : public ModelVisitor {
   public:
    using DendrogramPlot = CQChartsDendrogramPlot;

   public:
    FlatRowVisitor(const DendrogramPlot *dendrogramPlot, Edges &edges) :
     dendrogramPlot_(dendrogramPlot), edges_(edges) {
      groupColumn_ = dendrogramPlot_->groupColumn();

      if (groupColumn_.isValid()) {
        for (const auto &c : dendrogramPlot->nameColumns())
          if (c != groupColumn_)
            nameColumns_.addColumn(c);

        if (nameColumns_.count() == 0)
          nameColumns_ = dendrogramPlot->nameColumns();
      }
      else
        nameColumns_ = dendrogramPlot_->nameColumns();
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      // get hier names and associated model indices from name columns
      QStringList   nameStrs;
      QModelIndices nameInds;

      auto sep = dendrogramPlot_->calcSeparator();

      if (! dendrogramPlot_->getHierColumnNames(data.parent, data.row, nameColumns_,
                                                sep, nameStrs, nameInds))
        return State::SKIP;

      //---

      // add group name at top of hier if specified
      QString groupName;

      if (groupColumn_.isValid()) {
        ModelIndex groupModelInd(dendrogramPlot_, data.row, groupColumn_, data.parent);

        bool ok;

        groupName = dendrogramPlot_->modelString(groupModelInd, ok); // hier ?

        if (groupName == "")
          groupName = "<none>";
      }

      //---

      // get value name (last name columns name or from id column)
      QString     name;
      QModelIndex nameInd;
      ModelIndex  modelNameInd;

      if (dendrogramPlot_->idColumn().isValid()) {
        ModelIndex idModelInd(dendrogramPlot_, data.row, dendrogramPlot_->idColumn(), data.parent);

        bool ok;

        name = dendrogramPlot_->modelString(idModelInd, ok);

        if (! ok)
          name = nameStrs.back();

        nameInd = dendrogramPlot_->modelIndex(idModelInd);

        modelNameInd = ModelIndex(dendrogramPlot_, data.row,
                                  dendrogramPlot_->idColumn(), data.parent);
      }
      else {
        name    = nameStrs.back();
        nameInd = nameInds[0];

        modelNameInd = ModelIndex(dendrogramPlot_, data.row, nameColumns_.column(), data.parent);
      }

//    auto nameInd1 = dendrogramPlot_->normalizeIndex(nameInd);

      //---

      // get node value
      OptReal value;

      ModelIndex valueInd;
      double     rvalue = 0.0;

      if (getValue(data, valueInd, rvalue)) {
        if (! CMathUtil::isNaN(rvalue))
          value = OptReal(rvalue);
        else
          return State::SKIP;
      }
      else {
        if (! dendrogramPlot_->isSkipBad())
          return addDataError(valueInd, "Invalid Value");
      }

      //---

      // get node color
      Color   color;
      OptReal colorValue;

      if (dendrogramPlot_->colorColumn().isValid()) {
        ModelIndex colorInd;

        if (! getColor(data, colorInd, color, colorValue)) {
          if (! dendrogramPlot_->isSkipBad())
            return addDataError(colorInd, "Invalid Color");
        }
      }

      //---

      // get node size
      OptReal sizeValue;

      if (dendrogramPlot_->sizeColumn().isValid()) {
        double     size = 0.0;
        ModelIndex sizeInd;

        if (getSize(data, sizeInd, size)) {
          if (! CMathUtil::isNaN(size))
            sizeValue = OptReal(size);
        }
        else {
          if (! dendrogramPlot_->isSkipBad())
            return addDataError(sizeInd, "Invalid Size");
        }
      }

      //---

      // get attributes from optional column
      CQChartsNameValues nameValues;

      if (dendrogramPlot_->attributesColumn().isValid()) {
        ModelIndex attributesModelInd(dendrogramPlot_, data.row,
                                      dendrogramPlot_->attributesColumn(), data.parent);

        bool ok;
        auto attributesStr = dendrogramPlot_->modelString(attributesModelInd, ok);
        if (! ok) return State::SKIP;

        nameValues = CQChartsNameValues(attributesStr);
      }

      //---

      // add to nodes/edges
      dendrogramPlot_->addNameValue(groupName, CQChartsNamePair(), nameStrs,
                                    value, modelNameInd, color, colorValue, sizeValue,
                                    nameValues, edges_);

      //---

      return State::OK;
    }

    bool getValue(const VisitData &data, ModelIndex &valueInd, double &value) {
      // get node or edge value
      valueInd = ModelIndex(dendrogramPlot_, data.row, dendrogramPlot_->valueColumn(),
                            data.parent);

      bool ok;
      value = dendrogramPlot_->modelReal(valueInd, ok);

      if (! CMathUtil::isNaN(value))
        valueRange_.add(value);

      return ok;
    }

    bool getColor(const VisitData &data, ModelIndex &colorInd, Color &color, OptReal &colorValue) {
      colorInd = ModelIndex(dendrogramPlot_, data.row, dendrogramPlot_->colorColumn(),
                            data.parent);

      bool ok;
      auto var = dendrogramPlot_->modelValue(colorInd, ok);
      if (! ok) return false;

      auto r = CQChartsVariant::toReal(var, ok);

      if (ok) {
        if (CMathUtil::isNaN(r)) return false;

        color      = Color::makePaletteValue(r);
        colorValue = OptReal(r);

        colorRange_.add(r);
      }
      else {
        ok = dendrogramPlot_->columnValueColor(var, color);
      }

      return ok;
    }

    bool getSize(const VisitData &data, ModelIndex &sizeInd, double &size) {
      sizeInd = ModelIndex(dendrogramPlot_, data.row, dendrogramPlot_->sizeColumn(),
                           data.parent);

      bool ok;
      size = dendrogramPlot_->modelReal(sizeInd, ok);
      if (! ok) return false;

      sizeRange_.add(size);

      return true;
    }

    const RMinMax &valueRange() const { return valueRange_; }
    const RMinMax &colorRange() const { return colorRange_; }
    const RMinMax &sizeRange () const { return sizeRange_; }

   private:
    State addDataError(const ModelIndex &ind, const QString &msg) const {
      const_cast<DendrogramPlot *>(dendrogramPlot_)->addDataError(ind , msg);
      return State::SKIP;
    }

   private:
    const DendrogramPlot*  dendrogramPlot_ { nullptr };
    DendrogramPlot::Edges& edges_;
    Column                 groupColumn_;
    Columns                nameColumns_;
    RMinMax                valueRange_;
    RMinMax                colorRange_;
    RMinMax                sizeRange_;
  };

  //---

  Edges edges;
  bool  isEdgeRows = false;

  if (isHierarchical() || linkColumn().isValid()) {
    HierRowVisitor visitor(this, edges);

    visitModel(visitor);

    th->valueRange_ = visitor.valueRange();
    th->colorRange_ = visitor.colorRange();
    th->sizeRange_  = visitor.sizeRange ();

    if (linkColumn().isValid())
      isEdgeRows = true;
  }
  else {
    FlatRowVisitor visitor(this, edges);

    visitModel(visitor);

    th->valueRange_ = visitor.valueRange();
    th->colorRange_ = visitor.colorRange();
    th->sizeRange_  = visitor.sizeRange ();
  }

  //---

  auto *root = rootNode();

  // process from/to edges to build tree
  if (! edges.empty()) {
    // remove tempRoot_ children
    th->tempRoot_->clear();

    for (auto *edge : edges) {
      // link to -> from if to has bad parent (tempRoot_)
      if (edge->to->parent() == tempRoot_) {
        //std::cerr << "Connect " << edge->from->name().toStdString() <<
        //             " -> " << edge->to->name().toStdString() << "\n";
        edge->from->addChild(edge->to, edge->value.value());
      }
    }

    for (auto *edge : edges) {
      // link root -> from if bad parent (tempRoot_)
      if (root && edge->from->parent() == tempRoot_) {
        //std::cerr << "Connect root -> " << edge->from->name().toStdString() << "\n";
        root->addChild(edge->from, CQChartsDendrogram::OptReal());
      }
    }

    for (auto *edge : edges) {
      // link from and to if bad parent (tempRoot_)
      if (! edge->from->hasChild(edge->to)) {
        //std::cerr << "Bad edge " << edge->from->name().toStdString() <<
        //             " -> " << edge->to->name().toStdString() << "\n";
        // edge->from->addChild(edge->to, edge->value.value());
      }
    }

    for (auto *edge : edges)
      delete edge;
  }

  delete th->tempRoot_;

  //---

  // remove extra roots
  while (root && root->getChildren().size() == 1 && root->isTemp()) {
    auto &child = root->getChildren()[0];

    auto *childNode  = child.node;
  //auto  childValue = child.value;

    root->removeChild(childNode);

  //root->setValue(childValue);

    for (auto &child1 : childNode->getChildren()) {
      auto *plotNode1 = dynamic_cast<PlotDendrogram::PlotNode *>(childNode);
      auto *plotNode2 = dynamic_cast<PlotDendrogram::PlotNode *>(child1.node);

      auto edgeColor = plotNode1->edgeColor(plotNode2);

      root->addChild(child1.node, child1.value);

      auto *rootNode = dynamic_cast<PlotDendrogram::PlotNode *>(root);

      rootNode->setEdgeColor(plotNode2, edgeColor);
    }

    childNode->removeAll();

    root->setName(childNode->name());

    if (childNode->size())
      root->setSize(childNode->size());

    root->setTemp(childNode->isTemp());

    delete childNode;
  }

  //---

  if (root)
    root->setOpen(true);

  //---

  processMetaData(isEdgeRows);

  //---

  place();
}

//---

bool
CQChartsDendrogramPlot::
processMetaNodeValue(const QString &name, const QString &key, const QVariant &value)
{
  auto *root = rootNode();
  if (! root) return false;

  auto *node = root->findHierChild(name);
  if (! node) return false;

  return processNodeNameVar(node, key, value);
}

bool
CQChartsDendrogramPlot::
processMetaEdgeValue(const QString &, const QString &, const QVariant &)
{
  return false;
}

//---

void
CQChartsDendrogramPlot::
place() const
{
  if (! dendrogram_)
    return;

  if      (placeType() == PlaceType::BUCHHEIM) {
    placeBuchheim();
  }
  else if (placeType() == PlaceType::CIRCULAR) {
    placeCircular();
  }
  else {
    dendrogram_->placeNodes();
  }
}

void
CQChartsDendrogramPlot::
placeBuchheim() const
{
  auto *root = rootNode();
  if (! root) return;

  cacheData_.buchheimTree = std::make_unique<Tree>(root);

  addBuchheimHierNode(cacheData_.buchheimTree.get(), root, 0);

  cacheData_.buchheimDrawTree =
    std::make_unique<CBuchHeim::DrawTree>(cacheData_.buchheimTree.get());

  cacheData_.buchheimDrawTree->place();

  cacheData_.buchheimDrawTree->fixOverlaps();

  auto ss = lengthPlotWidth(leafSize());

  cacheData_.buchheimDrawTree->normalize(ss, /*equalScale*/false);
//cacheData_.buchheimDrawTree->normalize(/*equalScale*/false);

  //---

  root->resetPlaced();

  moveBuchheimHierNode(cacheData_.buchheimDrawTree.get());
}

void
CQChartsDendrogramPlot::
addBuchheimHierNode(CBuchHeim::Tree *tree, Node *hierNode, int depth) const
{
  auto isHideDepth = [&](Node *node, int depth1) {
    if (hideDepth() <= 0) return false;

    int depth2 = depth1 + node->calcDepth(/*ignoreOpen*/true);
    return (depth2 < hideDepth());
  };

  //---

  hierNode->setDepth(depth);

  if (! hierNode->isOpen())
    return;

  for (auto &hierNode1 : hierNode->getChildren()) {
    if (isHideDepth(hierNode1.node, depth + 1))
      continue;

    auto *childTree = new Tree(hierNode1.node);

    tree->addChild(CBuchHeim::TreeP(childTree));

    addBuchheimHierNode(childTree, hierNode1.node, depth + 1);
  }
}

void
CQChartsDendrogramPlot::
moveBuchheimHierNode(CBuchHeim::DrawTree *drawTree) const
{
  auto *tree = static_cast<Tree *>(drawTree->tree());

  auto *node1 = tree->node();

  double x1 = drawTree->x1();
  double y1 = drawTree->y1();
  double x2 = drawTree->x2();
  double y2 = drawTree->y2();

  double xc = CMathUtil::avg(x1, x2);
  double yc = CMathUtil::avg(y1, y2);
  double r  = 0.4*std::min(x2 - x1, y2 - y1);

  if (orientation() == Qt::Horizontal)
    node1->setBBox(BBox(yc - r, xc - r, yc + r, xc + r));
  else
    node1->setBBox(BBox(xc - r, 1.0 - (yc - r), xc + r, 1.0 - (yc + r)));

  node1->setPlaced(true);

  for (const auto &node : drawTree->children())
    moveBuchheimHierNode(node.get());
}

void
CQChartsDendrogramPlot::
placeCircular() const
{
  auto *root = rootNode();
  if (! root) return;

  root->resetPlaced();

  //---

  CircularDepth circularDepth;
  NodeAngles    nodeAngles;

  int maxDepth = 0;

  initCircularDepth(root, circularDepth, 0, maxDepth);

  auto r  = 0.0;
  auto dr = (maxDepth > 0 ? 1.0/maxDepth : 0.0);

  // get specified shape size
  double cs = calcLeafSize();

  double cw = pixelToWindowWidth (cs);
  double ch = pixelToWindowHeight(cs);

  for (const auto &pc : circularDepth) {
    //auto depth = pc.first;

    const auto &cnode = pc.second;

    Node *pnode { nullptr };

    int ind = 0;

    auto a1  = 0.0;
    auto da1 = 2.0*M_PI;

    for (auto &node : cnode.nodes) {
      auto *pnode1 = node->parent();

      if (pnode1 != pnode) {
        pnode = pnode1;
        ind   = 0;

        auto a  = 0.0;
        auto da = 2.0*M_PI;

        auto pa = nodeAngles.find(pnode);

        if (pa != nodeAngles.end()) {
          a  = (*pa).second.a;
          da = (*pa).second.da;
        }

        auto n = pnode->getChildren().size();

        if (n > 1) {
          a1  = a;
          da1 = da/double(n);
        }
        else {
          a1  = a + da/2.0;
          da1 = 0.0;
        }
      }
      else
        ++ind;

      auto a2 = a1 + ind*da1;

      AnglePair ap;

      ap.a  = a2 - da1/2.0;
      ap.da = da1;

      nodeAngles[node] = ap;

      auto x = std::cos(a2)*r;
      auto y = std::sin(a2)*r;

      auto bbox = BBox(x - cw/2.0, y - ch/2.0, x + cw/2.0, y + ch/2.0);

      node->setBBox(bbox);

      node->setPlaced(true);
    }

    r += dr;
  }
}

void
CQChartsDendrogramPlot::
initCircularDepth(Node *hierNode, CircularDepth &circularDepth, int depth, int &maxDepth) const
{
  maxDepth = std::max(maxDepth, depth);

  circularDepth[depth].nodes.push_back(hierNode);

  if (hierNode->isOpen()) {
    maxDepth = std::max(maxDepth, depth + 1);

    for (const auto &child : hierNode->getChildren())
      initCircularDepth(child.node, circularDepth, depth + 1, maxDepth);
  }
}

void
CQChartsDendrogramPlot::
addHierName(const QString &nameStr, const ModelIndex &modelInd) const
{
  auto *root = rootNode();

  if (! root) {
    auto *hierNode = dendrogram_->addRootNode("root");
    hierNode->setTemp(true);

    hierNode->setOpen(false);
  }

  //---

  QStringList names;

  // split name into hier path elements
  auto name1 = nameStr;

  int pos = name1.indexOf('/');

  while (pos != -1) {
    auto lhs = name1.mid(0, pos);
    auto rhs = name1.mid(pos + 1);

    names.push_back(lhs);

    name1 = rhs;

    pos = name1.indexOf('/');
  }

  names.push_back(name1);

  //---

  // create nodes for hierarchy
  Node *hierNode = nullptr;

  for (const auto &n : names) {
    if (! hierNode) {
      hierNode = rootNode();

      if (hierNode)
        hierNode->setName(n);
    }
    else {
      auto *hierNode1 = hierNode->findChild(n);

      if (! hierNode1) {
        hierNode = dendrogram_->addEdge(hierNode, n);

        hierNode->setOpen(false);
      }
      else
        hierNode = hierNode1;
    }
  }

  auto *hierNode1 = dynamic_cast<PlotDendrogram::PlotNode *>(hierNode);

  if (hierNode1) {
    auto modelInd1 = modelIndex(modelInd);

    hierNode1->setModelInd(modelInd);
    hierNode1->setInd     (normalizeIndex(modelInd1));
  }
}

void
CQChartsDendrogramPlot::
addNameValue(const QString &nameStr, const CQChartsNamePair &namePair, const QStringList &nameList,
             const OptReal &value, const ModelIndex &modelInd, const Color &color,
             const OptReal &colorValue, OptReal &sizeValue,
             const CQChartsNameValues &nameValues, Edges &edges) const
{
  auto *root = rootNode();

  if (! root) {
    auto *hierNode = dendrogram_->addRootNode("root");
    hierNode->setTemp(true);

    hierNode->setOpen(false);
  }

  //---

  QStringList names;

  // edge for namePair names
  if      (namePair.isValid()) {
    // get from/to node and create edge
    auto *fromNode = tempRoot_->findChild(namePair.name1());
    auto *toNode   = tempRoot_->findChild(namePair.name2());

    if (! fromNode)
      fromNode = dendrogram_->addEdge(tempRoot_, namePair.name1());

    if (! toNode)
      toNode = dendrogram_->addEdge(tempRoot_, namePair.name2());

    auto *edge = new Edge(fromNode, toNode, value);

    auto *fromNode1 = dynamic_cast<PlotDendrogram::PlotNode *>(fromNode); assert(fromNode1);
    auto *toNode1   = dynamic_cast<PlotDendrogram::PlotNode *>(toNode); assert(toNode1);

    if (value.isSet())
      fromNode->setChildValue(toNode, value.real());

    if (color.isValid() || colorValue.isSet()) {
      edge->color      = color;
      edge->colorValue = colorValue;

      fromNode1->setEdgeColor     (toNode1, color);
      fromNode1->setEdgeColorValue(toNode1, colorValue);
    }

    if (sizeValue.isSet()) {
      edge->sizeValue = sizeValue;

      fromNode1->setEdgeSizeValue(toNode1, sizeValue);
    }

    edges.push_back(edge);

    processEdgeNameValues(edge, nameValues);
  }
  // edges for hier names
  else if (nameList.count() > 0) {
    auto nameList1 = nameList;

    if (nameStr != "")
      nameList1.push_front(nameStr);

    // create nodes for hierarchy
    int n = nameList1.count();

    Node *hierNode = rootNode();

    for (int i = 0; i < n - 1; ++i) {
      const auto &name = nameList1[i];

      auto *hierNode1 = hierNode->findChild(name);

      if (! hierNode1) {
        hierNode = dendrogram_->addEdge(hierNode, name);

        hierNode->setOpen(false);
      }
      else
        hierNode = hierNode1;
    }

    const auto &nname = nameList1[n - 1];

    auto *node = dendrogram_->addNode(hierNode, nname, value.realOr(0.0));
    assert(node);

    auto *node1 = dynamic_cast<PlotDendrogram::PlotNode *>(node); assert(node1);

    auto modelInd1 = modelIndex(modelInd);

    node1->setModelInd(modelInd);
    node1->setInd     (normalizeIndex(modelInd1));

    if (color.isValid() || colorValue.isSet()) {
      node1->setColor     (color);
      node1->setColorValue(colorValue);
    }

    if (sizeValue.isSet())
      node1->setSizeValue(sizeValue);

    processNodeNameValues(node1, nameValues);
  }
  // edges for separated names
  else {
    // split name into hier path elements
    auto name1 = nameStr;

    int pos = name1.indexOf('/');

    if (pos != -1) {
      while (pos != -1) {
        auto lhs = name1.mid(0, pos);
        auto rhs = name1.mid(pos + 1);

        names.push_back(lhs);

        name1 = rhs;

        pos = name1.indexOf('/');
      }
    }
    else {
      names.push_back(name1);
    }

    //---

    // create nodes for hierarchy
    Node *hierNode = nullptr;

    for (const auto &n : names) {
      if (! hierNode) {
        hierNode = rootNode();

        if (hierNode)
          hierNode->setName(n);
      }
      else {
        auto *hierNode1 = hierNode->findChild(n);

        if (! hierNode1) {
          hierNode = dendrogram_->addEdge(hierNode, n);

          hierNode->setOpen(false);
        }
        else
          hierNode = hierNode1;
      }
    }

    auto *node = dendrogram_->addNode(hierNode, name1, value.realOr(0.0));
    assert(node);

    auto *node1 = dynamic_cast<PlotDendrogram::PlotNode *>(node); assert(node1);

    auto modelInd1 = modelIndex(modelInd);

    node1->setModelInd(modelInd);
    node1->setInd     (normalizeIndex(modelInd1));

    if (color.isValid() || colorValue.isSet()) {
      node1->setColor     (color);
      node1->setColorValue(colorValue);
    }

    if (sizeValue.isSet())
      node1->setSizeValue(sizeValue);

    processNodeNameValues(node1, nameValues);
  }
}

//---

void
CQChartsDendrogramPlot::
processEdgeNameValues(Edge *edge, const CQChartsNameValues &nameValues) const
{
  for (const auto &nv : nameValues.nameValues()) {
    const auto &name     = nv.first;
    auto        valueStr = nv.second.toString();

    // custom value
    if      (name == "label") {
      edge->value = OptReal(valueStr.toDouble());
    }
    // handle custom value for source node
    else if (name.left(4) == "src_") {
      processNodeNameValue(edge->from, name.mid(4), valueStr);
    }
    // handle custom value for destination node
    else if (name.left(5) == "dest_") {
      processNodeNameValue(edge->to, name.mid(5), valueStr);
    }
  }
}

void
CQChartsDendrogramPlot::
processNodeNameValues(Node *node, const CQChartsNameValues &nameValues) const
{
  for (const auto &nv : nameValues.nameValues()) {
    const auto &name  = nv.first;
    auto        value = nv.second.toString();

    processNodeNameValue(node, name, value);
  }
}

void
CQChartsDendrogramPlot::
processNodeNameValue(Node *node, const QString &name, const QString &valueStr) const
{
  if (! processNodeNameVar(node, name, valueStr)) {
    auto *th = const_cast<CQChartsDendrogramPlot *>(this);

    th->addDataError(ModelIndex(), QString("Unhandled name '%1'").arg(name));
  }
}

bool
CQChartsDendrogramPlot::
processNodeNameVar(Node *node, const QString &name, const QVariant &var) const
{
  bool ok;

  // custom value
  if      (name == "value") {
    auto v = CQChartsVariant::toReal(var, ok);
    if (! ok) return false;

    node->setSize(v);
  }
  else if (name == "color") {
    Color   color;
    OptReal colorValue;

    bool ok;
    auto r = CQChartsVariant::toReal(var, ok);

    if (ok) {
      color      = Color::makePaletteValue(r);
      colorValue = OptReal(r);
    }
    else {
      color = CQChartsVariant::toColor(var, ok);
    }

    auto *node1 = dynamic_cast<PlotDendrogram::PlotNode *>(node); assert(node1);

    if (node1) {
      node1->setColor     (color);
      node1->setColorValue(colorValue);
    }
  }
  else
    return false;

  return true;
}

//---

CQChartsGeom::BBox
CQChartsDendrogramPlot::
calcExtraFitBBox() const
{
  CQPerfTrace trace("CQChartsDendrogramPlot::calcExtraFitBBox");

  BBox bbox;

  for (const auto &plotObj : plotObjects()) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);
    if (! nodeObj) continue;

    if (nodeObj->isRoot() && ! isRootVisible())
      continue;

    bbox += nodeObj->displayRect();

    bool isHier = nodeObj->isHier();

    bool textVisible = (isHier ? isHierTextVisible() : isLeafTextVisible());

    if (textVisible)
      bbox += nodeObj->textRect();
  }

  return bbox;
}

//---

void
CQChartsDendrogramPlot::
execMoveNonRoot(const PlotObjs &objs)
{
  using NodeObjs  = std::vector<NodeObj *>;
  using DepthObjs = std::map<int, NodeObjs>;

  DepthObjs depthObjs;

  // get nodes for each parent at each depth
  for (auto *plotObj : objs) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);
    if (! nodeObj) continue;

    auto depth = nodeObj->node()->depth();

    depthObjs[depth].push_back(nodeObj);
  }

  if (depthObjs.size() <= 1)
    return;

  double dd  = 1.0/(depthObjs.size() - 1);
  double dd1 = dd/2.0;

  for (const auto &pd : depthObjs) {
    if (pd.first == 0) continue; // skip root

    for (auto *nodeObj : pd.second) {
      auto rect = nodeObj->rect();

      double dp = 0.0;

      if (orientation() == Qt::Horizontal)
        dp = dd1 - rect.getXMid();
      else {
        auto dd2 = 1.0 - dd1; // vertical is 1.0 -> 0.0 so flip

        dp = dd2 - rect.getYMid();
      }

      nodeObj->moveParBy(dp);
    }

    dd1 += dd;
  }
}

//---

void
CQChartsDendrogramPlot::
saveOverlapRects()
{
  saveOverlapRects(plotObjects());
}

void
CQChartsDendrogramPlot::
saveOverlapRects(const PlotObjs &objs)
{
  for (auto *plotObj : objs) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);
    if (! nodeObj) continue;

    auto rect = nodeObj->rect();
    assert(rect.isSet());

    nodeObj->setSaveRect(rect);
  }
}

void
CQChartsDendrogramPlot::
restoreOverlapRects()
{
  for (auto *plotObj : plotObjects()) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);
    if (! nodeObj) continue;

    auto saveRect = nodeObj->saveRect();

    if (saveRect.isSet())
      nodeObj->setRect(saveRect);
  }
}

void
CQChartsDendrogramPlot::
execRemoveOverlaps()
{
  execRemoveOverlaps(plotObjects());
}

void
CQChartsDendrogramPlot::
execRemoveOverlaps(const PlotObjs &objs)
{
  double sizeScale = 1.0;

  std::swap(sizeScale, sizeScale_);

  overlapScale_ = 1.0;

  //---

  auto getPerpMin = [&](const BBox &r) {
    return (orientation() == Qt::Horizontal ? r.getYMin() : r.getXMin());
  };

//auto getPerpMax = [&](const BBox &r) {
//  return (orientation() == Qt::Horizontal ? r.getYMax() : r.getXMax());
//};

  auto getPerpMid = [&](const BBox &r) {
    return (orientation() == Qt::Horizontal ? r.getYMid() : r.getXMid());
  };

  auto getPerpSize = [&](const BBox &r) {
    return (orientation() == Qt::Horizontal ? r.getHeight() : r.getWidth());
  };

  auto getParSize = [&](const BBox &r) {
    return (orientation() == Qt::Horizontal ? r.getWidth() : r.getHeight());
  };

  //--

  using NodeObjs            = std::vector<NodeObj *>;
  using RectNodeObjs        = std::pair<BBox, NodeObjs>;
  using PosParent           = std::pair<double, const NodeObj *>;
  using ParentNodeObjs      = std::map<PosParent, RectNodeObjs>;
  using DepthParentNodeObjs = std::map<int, ParentNodeObjs>;

  auto makePosParent = [&](const NodeObj *nodeObj) {
    auto *parent = nodeObj->parent();
    if (! parent) return PosParent(0.5, parent);

    return PosParent(getPerpMid(parent->displayRect()), parent);
  };

  DepthParentNodeObjs depthParentNodeObjs;

  // get nodes for each parent at each depth
  for (auto *plotObj : objs) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);
    if (! nodeObj) continue;

    auto depth = nodeObj->node()->depth();
    auto ppos  = makePosParent(nodeObj);

    depthParentNodeObjs[depth][ppos].second.push_back(nodeObj);
  }

  //---

  // calc perp margin
  double perpMargin = 0.0;

  if (overlapMargin().value() > 0.0) {
    if (orientation() == Qt::Horizontal)
      setRefLength(OptReal(lengthPlotWidth(leafSize())));
    else
      setRefLength(OptReal(lengthPlotHeight(leafSize())));

    if (orientation() == Qt::Horizontal)
      perpMargin = std::max(lengthPlotWidth(overlapMargin()), 0.0);
    else
      perpMargin = std::max(lengthPlotHeight(overlapMargin()), 0.0);

    resetRefLength();
  }

  //---

  // calc scale such that all nodes can stack vertically in range (0.0 -> 1.0)
  auto calcPerpScale = [&]() {
    double maxPerp = 0.0;

    for (auto &dpn : depthParentNodeObjs) {
      double perp = -perpMargin;

      for (auto &pn : dpn.second) {
        for (auto *nodeObj : pn.second.second) {
          auto bbox = nodeObj->displayRect();

          perp += getPerpSize(bbox) + perpMargin;
        }
      }

      maxPerp = std::max(maxPerp, perp);
    }

    return (maxPerp > 0.0 ? 1.0/maxPerp : 1.0);
  };

  overlapScale_ = calcPerpScale();
//overlapScale_ = std::min(calcPerpScale(), 1.0);

  // adjust scale so not too wide/tall
  auto calcSizeScale = [&]() {
    double maxSize  = 0.0;
    int    maxDepth = 0;

    for (auto &dpn : depthParentNodeObjs) {
      maxDepth = std::max(maxDepth, dpn.first);

      //---

      double size = 0.0;

      for (auto &pn : dpn.second) {
        for (auto *nodeObj : pn.second.second) {
          auto bbox = nodeObj->displayRect();

          size = std::max(size, getParSize(bbox));
        }
      }

      maxSize = std::max(maxSize, size);
    }

    auto depthSize = 1.0/(maxDepth + 1);

    if (maxSize > 0.0 && maxSize > depthSize)
      return depthSize/maxSize;
    else
      return 1.0;
  };

  overlapScale_ *= calcSizeScale();

  //---

  // process depth nodes
  std::set<int> adjustDepths;

  for (auto &dpn : depthParentNodeObjs) {
    auto depth = dpn.first;

    // process each set of parent nodes
    for (auto &pn : dpn.second) {
      std::vector<BBox> rects;

      // get node bounding box and non-overlap height
      double perp = -perpMargin;

      for (auto *nodeObj : pn.second.second) {
        auto bbox1 = nodeObj->displayRect();

        perp += getPerpSize(bbox1) + perpMargin;

        pn.second.first += bbox1;

        rects.push_back(bbox1);
      }

      // remove parent overlaps
      bool overlaps1    = false;
      bool needsSpread1 = false;

      if (rects.size() > 1) {
        if (CQChartsUtil::checkOverlaps(rects))
          overlaps1 = true;

        if (perp > getPerpSize(pn.second.first)) {
          overlaps1    = true;
          needsSpread1 = true;
        }
      }

#if 0
      if (overlaps1) {
        const auto &posParent = pn.first;
        auto name = (posParent.second ? posParent.second->name() : "<null>");
        std::cerr << "Overlap for " << name.toStdString() << "\n";
      }
#endif

      if (overlaps1) {
        // place from bottom to top
        auto pos = (needsSpread1 ? getPerpMid(pn.second.first) - perp/2.0 :
                                   getPerpMin(pn.second.first));

        auto numObjs1 = pn.second.second.size();

        auto perpMargin1 = (numObjs1 > 0 ? (getPerpSize(pn.second.first) - perp)/numObjs1 : 0.0);

        perpMargin1 = std::max(perpMargin, perpMargin1);

        pn.second.first = BBox();

      //double perp1 = -perpMargin1;

        for (auto *nodeObj : pn.second.second) {
          auto bbox1 = nodeObj->displayRect();

          auto rect = nodeObj->rect();

          auto dp = pos + getPerpSize(bbox1)/2.0 - getPerpMid(rect);

          nodeObj->movePerpBy(dp);

          pos   += getPerpSize(bbox1) + perpMargin1;
        //perp1 += getPerpSize(bbox1) + perpMargin1;

          pn.second.first += nodeObj->displayRect();
        }

        if (! needsSpread1)
          adjustDepths.insert(depth);
      }
    }
  }

  //---

  if (isRemoveGroupOverlaps()) {
    // process node sets at depth
    for (auto &dpn : depthParentNodeObjs) {
      BBox              dbbox;
      BBox              pbbox;
      std::vector<BBox> drects;
      double            dperp = 0.0;

      // calc bbox for each set of parent nodes
      for (auto &pn : dpn.second) {
        if (pn.first.second)
          pbbox += pn.first.second->displayRect();
        else
          pbbox += BBox(0, 0, 1, 1);

        // calc group bbox (common parent)
        BBox dbbox1;

        for (auto *nodeObj : pn.second.second) {
          auto bbox1 = nodeObj->displayRect();

          dbbox1 += bbox1;
        }

        drects.push_back(dbbox1);

        dbbox += dbbox1;

        dperp += getPerpSize(dbbox1);
      }

      // remove depth overlaps if not enough space
      bool overlaps = false;

      if (drects.size() > 1) {
        if (CQChartsUtil::checkOverlaps(drects))
          overlaps = true;

        if (dperp > getPerpSize(dbbox))
          overlaps = true;
      }

  #if 0
      if (overlaps) {
        std::cerr << "Overlap for depth " << dpn.first << "\n";
      }
  #endif

      if (overlaps) {
        // place from bottom to top
        auto pos = getPerpMid(pbbox) - dperp/2.0;

        // move all parent nodes to new center
        int ip = 0;

        for (auto &pn : dpn.second) {
          const auto &prect = drects[ip];

          auto dp = pos - getPerpMin(prect);

          for (auto *nodeObj : pn.second.second)
            nodeObj->movePerpBy(dp);

          pos += getPerpSize(prect);

          ++ip;
        }
      }
    }
  }

  //---

  if (isAdjustOverlaps()) {
    //std::cerr << "Adjust overlaps\n";

    // move back to closest to original rect
    for (auto &dpn : depthParentNodeObjs) {
      auto depth = dpn.first;

      //if (depth != 1) continue;

      if (adjustDepths.find(depth) == adjustDepths.end())
        continue;

      //std::cerr << "Depth changed : " << depth << "\n";

      std::vector<BBox> oldRects, newRects;

      for (auto &pn : dpn.second) {
        for (auto *nodeObj : pn.second.second) {
          assert(nodeObj->saveRect().isSet());

          oldRects.push_back(nodeObj->saveRect());
          newRects.push_back(nodeObj->displayRect());
        }
      }

      CQChartsUtil::adjustRectsToOriginal(oldRects, newRects, orientation());

#if 0
      uint i = 0;

      for (auto &pn : dpn.second) {
        for (auto *nodeObj : pn.second.second) {
          auto pos1 = getPerpMid(nodeObj->displayRect());
          auto pos2 = getPerpMid(newRects[i]);

          std::cerr << "Move " << nodeObj->name().toStdString() << " " << pos2 - pos1 << "\n";

          ++i;
        }
      }
#endif
    }
  }

  std::swap(sizeScale, sizeScale_);
}

#if 0
void
CQChartsDendrogramPlot::
calcNodeSize(PlotObjs &objs)
{
  using NodeObjs            = std::vector<NodeObj *>;
  using ParentNodeObjs      = std::map<const NodeObj *, NodeObjs>;
  using DepthParentNodeObjs = std::map<int, ParentNodeObjs>;

  DepthParentNodeObjs depthParentNodeObjs;

  // get nodes for each parent at each depth
  for (auto *plotObj : objs) {
    auto *nodeObj = dynamic_cast<NodeObj *>(plotObj);
    if (! nodeObj) continue;

    depthParentNodeObjs[nodeObj->node()->depth()][nodeObj->parent()].push_back(nodeObj);
  }

  for (auto &dpn : depthParentNodeObjs) {
    std::cerr << "Depth: " << dpn.first << "\n";

    for (auto &pn : dpn.second) {
      if (pn.first)
        std::cerr << "  Parent: " << pn.first->name().toStdString() << "\n";

      for (auto *nodeObj : pn.second) {
        auto bbox = nodeObj->displayRect();

        std::cerr << "    Node: " << nodeObj->name().toStdString() <<
                     " Y: " << bbox.getYMid() << "\n";
      }
    }
  }
}
#endif

double
CQChartsDendrogramPlot::
calcHierColor(const Node *hierNode) const
{
  // get hier color value sum for hier node
  double color = 0.0;

  if (hierNode->isHier()) {
    for (const auto &child : hierNode->getChildren())
      color += calcHierColor(child.node);
  }
  else {
    auto *node1 = dynamic_cast<const PlotDendrogram::PlotNode *>(hierNode); assert(node1);

    color += node1->colorValue().realOr(0.0);
  }

  return color;
}

double
CQChartsDendrogramPlot::
calcHierSize(const Node *hierNode) const
{
  // get hier value sum for hier node
  // TODO: same as hierNode->size() !?
  double size = 0.0;

  if (hierNode->isHier()) {
    for (const auto &child : hierNode->getChildren())
      size += calcHierSize(child.node);
  }
  else {
    auto *node1 = dynamic_cast<const PlotDendrogram::PlotNode *>(hierNode); assert(node1);

    size += node1->sizeValue().realOr(0.0);
  }

  return size;
}

void
CQChartsDendrogramPlot::
addNodeObjs(Node *hier, int depth, NodeObj *parentObj, PlotObjs &objs) const
{
  for (auto &hierData : hier->getChildren()) {
    auto       *hierNode = hierData.node;
    const auto &value    = hierData.value;

    bool hier = hierNode->isHier();

    auto *hierNodeObj = addNodeObj(hierNode, objs, hier);
    if (! hierNodeObj) continue;

    if (parentObj) {
      hierNodeObj->setParent(parentObj);

      parentObj->addChild(hierNodeObj, value);
    }

    addNodeObjs(hierNode, depth + 1, hierNodeObj, objs);
  }
}

CQChartsDendrogramNodeObj *
CQChartsDendrogramPlot::
addNodeObj(Node *node, PlotObjs &objs, bool isHier) const
{
  if (! node->isPlaced())
    return nullptr;

  auto rect = getBBox(node);

  ColorInd ig(node->depth()     , cacheData_.depth + 1);
  ColorInd iv(cacheData_.nodeInd, cacheData_.numNodes + 1);

  auto *obj = createNodeObj(node, rect, ig, iv);

  obj->setInd(cacheData_.nodeInd++);

  obj->connectDataChanged(this, SLOT(updateSlot()));

  obj->setHier(isHier);

  objs.push_back(obj);

  return obj;
}

CQChartsDendrogramEdgeObj *
CQChartsDendrogramPlot::
addEdgeObj(NodeObj *fromNode, NodeObj *toNode) const
{
  auto rect = fromNode->rect() + toNode->rect();

  ColorInd ig(fromNode->node()->depth(), cacheData_.depth);
  ColorInd iv(cacheData_.edgeInd       , cacheData_.numEdges + 1);

  auto *obj = createEdgeObj(fromNode, toNode, rect, ig, iv);

  obj->setInd(cacheData_.edgeInd++);

  obj->setSelectable(isEdgeSelectable());

  obj->connectDataChanged(this, SLOT(updateSlot()));

  return obj;
}

//---

void
CQChartsDendrogramPlot::
modelViewExpansionChanged()
{
  if (! dendrogram_)
    return;

  if (! isFollowViewExpand())
    return;

  resetNodeExpansion(false);

  std::set<QModelIndex> indSet;

  expandedModelIndices(indSet);

  for (const auto &hierData : rootNode()->getChildren()) {
    auto *hierNode = hierData.node;

    setNodeExpansion(hierNode, indSet);
  }

  cacheData_.needsPlace = true;

  updateObjs();
}

void
CQChartsDendrogramPlot::
resetNodeExpansion(bool expanded)
{
  for (const auto &hierData : rootNode()->getChildren()) {
    auto *hierNode = hierData.node;

    resetNodeExpansion(hierNode, expanded);
  }
}

void
CQChartsDendrogramPlot::
resetNodeExpansion(Node *hierNode, bool expanded)
{
  if (hierNode->isOpen() != expanded)
    hierNode->setOpen(expanded);

  for (auto &hierData1 : hierNode->getChildren()) {
    auto *hierNode1 = hierData1.node;

    resetNodeExpansion(hierNode1, expanded);
  }
}

void
CQChartsDendrogramPlot::
setNodeExpansion(Node *hierNode, const std::set<QModelIndex> &indSet)
{
  auto pn = cacheData_.nodeObjs.find(hierNode);
  if (pn == cacheData_.nodeObjs.end()) return;

  auto *nodeObj = (*pn).second;

  if (nodeObj->modelIndex().isValid()) {
    auto ind = nodeObj->modelInd();

    bool expanded = (indSet.find(ind) != indSet.end());

    if (hierNode->isOpen() != expanded)
      hierNode->setOpen(expanded);
  }

  for (auto &hierData1 : hierNode->getChildren()) {
    auto *hierNode1 = hierData1.node;

    setNodeExpansion(hierNode1, indSet);
  }
}

//---

CQChartsGeom::BBox
CQChartsDendrogramPlot::
getBBox(Node *node) const
{
  if (placeType() == PlaceType::BUCHHEIM)
    return node->bbox();

  if (placeType() == PlaceType::CIRCULAR)
    return node->bbox();

  //---

  // get specified shape size
  auto calcSize = [&]() {
    if (node->isRoot()) return calcRootSize();
    if (node->isHier()) return calcHierSize();
    return calcLeafSize();
  };

  double cs = calcSize();

  double cw = pixelToWindowWidth (cs);
  double ch = pixelToWindowHeight(cs);

//double mw = pixelToWindowWidth(tm);

//double xc = dendrogram_->nodeX(node) + mw;
  double xc = dendrogram_->nodeX(node);
  double yc = dendrogram_->nodeYC(node);

  if (orientation() == Qt::Horizontal)
    return BBox(xc - cw/2.0, yc - ch/2.0, xc + cw/2.0, yc + ch/2.0);
  else
    return BBox(yc - cw/2.0, 1.0 - (xc - ch/2.0), yc + cw/2.0, 1.0 - (xc + ch/2.0));
}

//------

bool
CQChartsDendrogramPlot::
handleSelectDoubleClick(const Point &p, SelMod /*selMod*/)
{
  PlotObjs plotObjs;

  plotObjsAtPoint(p, plotObjs, Constraints::SELECTABLE);

  NodeObj *nodeObj = nullptr;

  for (auto *plotObj : plotObjs) {
    nodeObj = dynamic_cast<NodeObj *>(plotObj);
    if (nodeObj) break;
  }

  auto *node = (nodeObj ? const_cast<Node *>(nodeObj->node()) : nullptr);
  if (! node) return false;

  if (! node->isRoot()) {
    setOpen(nodeObj, ! node->isOpen());

    cacheData_.needsPlace = true;

    updateObjs();
  }

  return true;
}

//------

CQChartsDendrogramNodeObj *
CQChartsDendrogramPlot::
createNodeObj(Node *node, const BBox &rect, const ColorInd &ig, const ColorInd &iv) const
{
  return new CQChartsDendrogramNodeObj(this, node, rect, ig, iv);
}

CQChartsDendrogramEdgeObj *
CQChartsDendrogramPlot::
createEdgeObj(NodeObj *fromNode, NodeObj *toNode, const BBox &rect,
              const ColorInd &ig, const ColorInd &iv) const
{
  return new CQChartsDendrogramEdgeObj(this, fromNode, toNode, rect, ig, iv);
}

//---

bool
CQChartsDendrogramPlot::
isOpen(NodeObj *nodeObj) const
{
  auto *node = (nodeObj ? const_cast<Node *>(nodeObj->node()) : nullptr);
  if (! node) return false;

  return node->isOpen();
}

void
CQChartsDendrogramPlot::
setOpen(NodeObj *nodeObj, bool open)
{
  auto *node = (nodeObj ? const_cast<Node *>(nodeObj->node()) : nullptr);
  if (! node) return;

  node->setOpen(open);

  if (isFollowViewExpand()) {
    if (nodeObj->modelInd().isValid())
      expandModelIndex(nodeObj->modelInd(), node->isOpen());
  }
}

//---

bool
CQChartsDendrogramPlot::
addMenuItems(QMenu *menu, const Point &)
{
  menu->addSeparator();

  //---

  addMenuAction(menu, "Expand", SLOT(expandSlot()));
  addMenuAction(menu, "Expand All", SLOT(expandAllSlot()));
  addMenuAction(menu, "Collapse All", SLOT(collapseAllSlot()));

  //---

  if (canDrawColorMapKey())
    addColorMapKeyItems(menu);

  return true;
}

void
CQChartsDendrogramPlot::
expandSlot()
{
  auto *root = rootNode();
  if (! root) return;

  expandNode(root, /*all*/false);

  cacheData_.needsPlace = true;

  updateObjs();
}

void
CQChartsDendrogramPlot::
expandAllSlot()
{
  auto *root = rootNode();
  if (! root) return;

  expandNode(root, /*all*/true);

  cacheData_.needsPlace = true;

  updateObjs();
}

void
CQChartsDendrogramPlot::
collapseAllSlot()
{
  auto *root = rootNode();
  if (! root) return;

  for (const auto &child : root->getChildren())
    collapseNode(child.node, /*all*/true);

  cacheData_.needsPlace = true;

  updateObjs();
}

void
CQChartsDendrogramPlot::
expandNode(Node *hierNode, bool all)
{
  bool changed = false;

  auto pn = cacheData_.nodeObjs.find(hierNode);

  if (pn != cacheData_.nodeObjs.end()) {
    if (! isOpen((*pn).second)) {
      setOpen((*pn).second, true);

      changed = true;
    }
  }
  else {
    if (! hierNode->isOpen()) {
      hierNode->setOpen(true);

      changed = true;
    }
  }

  if (! all && changed)
    return;

  for (const auto &child : hierNode->getChildren())
    expandNode(child.node, all);
}

void
CQChartsDendrogramPlot::
collapseNode(Node *hierNode, bool all)
{
  bool collapseChildren = (all || ! hierNode->isOpen());

  if (hierNode->isOpen()) {
    auto pn = cacheData_.nodeObjs.find(hierNode);

    if (pn != cacheData_.nodeObjs.end())
      setOpen((*pn).second, false);
    else
      hierNode->setOpen(false);
  }

  if (collapseChildren) {
    for (const auto &child : hierNode->getChildren())
      collapseNode(child.node, all);
  }
}

//---

CQChartsPlotCustomControls *
CQChartsDendrogramPlot::
createCustomControls()
{
  auto *controls = new CQChartsDendrogramPlotCustomControls(charts());

  controls->init();

  controls->setPlot(this);

  controls->updateWidgets();

  return controls;
}

//------

CQChartsDendrogramNodeObj::
CQChartsDendrogramNodeObj(const DendrogramPlot *dendrogramPlot, Node *node, const BBox &rect,
                          const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<DendrogramPlot *>(dendrogramPlot), rect, ColorInd(), ig, iv),
 dendrogramPlot_(dendrogramPlot), node_(node), name_(node->name())
{
  double s;

  if (node->size())
    setValue(OptReal(*node->size()));

  if (node->calcHierSize(s))
    setHierValue(OptReal(s));

  setOpen(node->isOpen());

  auto *node1 = dynamic_cast<PlotDendrogram::PlotNode *>(node);

  if (node1) {
    setModelIndex(node1->modelInd());

    if (node1->ind().isValid())
      setModelInd(node1->ind());

    setColor     (node1->color());
    setColorValue(node1->colorValue());
    setSize      (node1->sizeValue());
  }
}

void
CQChartsDendrogramNodeObj::
addChild(NodeObj *node, const OptReal &value)
{
  children_.push_back(Edge(node, value));

  if (value.isSet())
    childTotal_ += value.real();
}

//---

QString
CQChartsDendrogramNodeObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(ind());
}

QString
CQChartsDendrogramNodeObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  plot()->addNoTipColumns(tableTip);

  //---

  tableTip.addTableRow("Name", name());

  if (value().isSet()) {
    QString valueName;

    if (! plot()->isEdgeValue() && plot()->valueColumn().isValid())
      valueName = plot()->columnHeaderName(plot()->valueColumn(), /*tip*/true);

    if (valueName == "")
      valueName = "Value";

    tableTip.addTableRow(valueName, value().real());
  }

  if (hierValue().isSet() && plot()->isHierValueTip())
    tableTip.addTableRow("Hier Value", hierValue().real());

  if      (colorValue().isSet())
    tableTip.addTableRow("Color", colorValue().real());
  else if (color().isValid())
    tableTip.addTableRow("Color", color().toString());

  if (size().isSet())
    tableTip.addTableRow("Size", size().real());

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

  return tableTip.str();
}

//---

bool
CQChartsDendrogramNodeObj::
inside(const Point &p) const
{
  auto rect = displayRect();

  auto pbbox = dendrogramPlot_->windowToPixel(rect);

  pbbox.expand(2.0);

  auto pp = dendrogramPlot_->windowToPixel(p);

  return pbbox.inside(pp);
}

//---

CQChartsGeom::BBox
CQChartsDendrogramNodeObj::
textRect() const
{
  // get font
  auto calcFont = [&]() {
    if (isRoot()) return plot()->rootTextFont();
    if (isHier()) return plot()->hierTextFont();
    return plot()->leafTextFont();
  };

  auto font  = calcFont();
  auto qfont = plot()->view()->plotFont(plot(), font);

  //---

  // calc position
  Point         p;
  Angle         angle;
  uint          position;
  Qt::Alignment align;
  bool          centered;

  calcTextPos(p, qfont, angle, position, align, centered);

  //---

  CQChartsPlotPaintDevice device(const_cast<CQChartsDendrogramPlot *>(plot()), nullptr);

  device.setFont(qfont);

  auto calcTextOptions = [&]() {
    if (isRoot()) return plot()->rootTextOptions(&device);
    if (isHier()) return plot()->hierTextOptions(&device);
    return plot()->leafTextOptions(&device);
  };

  auto textOptions = calcTextOptions();

  textOptions.angle     = angle;
  textOptions.align     = align;
  textOptions.formatted = false;
  textOptions.html      = false;

  const auto &name = this->name();

  using TextPosition = CQChartsDendrogramPlot::TextPosition;

  BBox tbbox;

  if (textOptions.scaled && TextPosition(position) == TextPosition::CENTER) {
    tbbox = displayRect();
  }
  else {
    tbbox = CQChartsDrawUtil::calcTextAtPointRect(&device, p, name, textOptions,
                                                  centered, 0.0, 0.0);
  }

#if 0
  QFontMetricsF fm(qfont);

  double dy = (fm.ascent() - fm.descent())/2.0;
  double dx = fm.horizontalAdvance(name);

  auto pbbox = plot()->windowToPixel(rect());

  auto pc = pbbox.getCenter();
  auto pr = pbbox.getWidth()/2.0;

  Point p;

  if (isHier())
    p = Point(pc.x - pr - dx, pc.y + dy);
  else
    p = Point(pc.x + pr, pc.y + dy);

  Point p1(p.x     , p.y - fm.ascent());
  Point p2(p.x + dx, p.y + fm.ascent());

  BBox ptbbox(p1, p2);

  auto tbbox = plot()->pixelToWindow(ptbbox);
#endif

  return tbbox;
}

void
CQChartsDendrogramNodeObj::
draw(PaintDevice *device) const
{
  if (isRoot() && ! plot()->isRootVisible())
    return;

  //---

  bool updateState = device->isInteractive();

  // calc pen and brush
  PenBrush penBrush;

  calcPenBrush(penBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, penBrush);

  auto shapeColor = penBrush.brush.color();

  //---

  // draw node
  auto rect1 = displayRect();

  // get shape
  auto calcShape = [&]() {
    if (isRoot()) return plot()->rootShape();
    if (isHier()) return plot()->hierShape();
    return plot()->leafShape();
  };

  auto shape = calcShape();

  auto shapeData = CQChartsShapeTypeData(shape.type());

  CQChartsDrawUtil::drawShape(device, shapeData, rect1);

  if (plot()->isSwatchColor() && color().isValid()) {
    auto colorInd = calcColorInd();

    auto fillColor = plot()->interpColor(color(), colorInd);

    penBrush.brush = QBrush(fillColor);

    CQChartsDrawUtil::setPenBrush(device, penBrush);

    CQChartsDrawUtil::drawShapeSwatch(device, shapeData, rect1);
  }

  //---

  drawText(device, shapeColor);

  //---

  if (plot()->showBoxes()) {
    auto bbox = textRect();

    drawDebugRect(device, bbox);
  }
}

void
CQChartsDendrogramNodeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  //---

  auto calcStrokeColor = [&]() {
    if (isRoot()) return plot()->interpRootStrokeColor(colorInd);
    if (isHier()) return plot()->interpHierStrokeColor(colorInd);
    return plot()->interpLeafStrokeColor(colorInd);
  };

  auto calcFillColor = [&]() {
    if (isRoot()) return plot()->interpRootFillColor(colorInd);
    if (isHier()) return plot()->interpHierFillColor(colorInd);
    return plot()->interpLeafFillColor(colorInd);
  };

  auto strokeColor = calcStrokeColor();

  //---

  QColor fillColor;
  bool   filled = true;

  if (! plot()->isColorClosed() && isHier() && ! isOpen())
    filled = false;

  if      (dendrogramPlot_->nodeColorByValue() == CQChartsDendrogramPlot::ValueType::HIER) {
    auto value = this->hierValue().realOr(0.0);
    auto color = dendrogramPlot_->mapHierValue(value);

    fillColor = plot()->interpPaletteColor(ColorInd(color));
  }
  else if (dendrogramPlot_->nodeColorByValue() == CQChartsDendrogramPlot::ValueType::NODE) {
    auto value = this->value().realOr(0.0);
    auto color = dendrogramPlot_->mapValue(value);

    fillColor = plot()->interpPaletteColor(ColorInd(color));
  }
  else if (color().isValid()) {
    fillColor = plot()->interpColor(color(), colorInd);
  }
  else if (plot()->colorColumn().isValid()) {
    Color c;

    double color = 0.0;

    if (isHier() && ! this->color().isValid())
      color = hierColor();
    else
      color = this->colorValue().realOr(0.0);

    if (plot()->isColorMapped()) {
      auto color1 = plot()->mapColor(color);

      c = plot()->colorFromColorMapPaletteValue(color1);
    }
    else {
      auto maxColor = (parent() ? parent()->hierColor() : this->colorValue().realOr(0.0));

      c = plot()->colorFromColorMapPaletteValue(maxColor > 0 ? color/maxColor : color);
    }

    fillColor = plot()->interpColor(c, colorInd);
  }
  else {
    fillColor = calcFillColor();
  }

  //---

  auto calcPenData = [&]() {
    if (isRoot()) return plot()->rootPenData(strokeColor);
    if (isHier()) return plot()->hierPenData(strokeColor);
    return plot()->leafPenData(strokeColor);
  };

  auto calcBrushData = [&]() {
    if (isRoot()) return plot()->rootBrushData(fillColor);
    if (isHier()) return plot()->hierBrushData(fillColor);
    return plot()->leafBrushData(fillColor);
  };

  if (filled)
    plot()->setPenBrush(penBrush, calcPenData(), calcBrushData());
  else
    plot()->setPenBrush(penBrush, calcPenData(), BrushData(false));

  //---

  if (updateState)
    plot()->updateObjPenBrushState(this, penBrush);
}

void
CQChartsDendrogramNodeObj::
drawText(PaintDevice *device, const QColor &shapeColor) const
{
  auto calcTextVisible = [&]() {
    if (isRoot()) return dendrogramPlot_->isRootTextVisible();
    if (isHier()) return dendrogramPlot_->isHierTextVisible();
    return dendrogramPlot_->isLeafTextVisible();
  };

  auto textVisible = calcTextVisible();
  if (! textVisible) return;

  //---

  // calc position
  Point         p;
  Angle         angle;
  uint          position;
  Qt::Alignment align;
  bool          centered;

  calcTextPos(p, device->font(), angle, position, align, centered);

  //---

  using TextPosition = CQChartsDendrogramPlot::TextPosition;

  bool textCenter = (TextPosition(position) == TextPosition::CENTER);

  if (textCenter)
    charts()->setContrastColor(shapeColor);

  //---

  // set pen
  ColorInd colorInd;
  PenBrush tpenBrush;

  auto calcTextColor = [&]() {
    if (isRoot()) return dendrogramPlot_->interpRootTextColor(colorInd);
    if (isHier()) return dendrogramPlot_->interpHierTextColor(colorInd);
    return dendrogramPlot_->interpLeafTextColor(colorInd);
  };

  auto calcTextAlpha = [&]() {
    if (isRoot()) return dendrogramPlot_->rootTextAlpha();
    if (isHier()) return dendrogramPlot_->hierTextAlpha();
    return dendrogramPlot_->leafTextAlpha();
  };

  auto tc = calcTextColor();
  auto ta = calcTextAlpha();

  plot()->setPen(tpenBrush, PenData(/*stroked*/true, tc, ta));

  device->setPen(tpenBrush.pen);

  //---

  // set font
  auto calcTextFont = [&]() {
    if (isRoot()) return dendrogramPlot_->rootTextFont();
    if (isHier()) return dendrogramPlot_->hierTextFont();
    return dendrogramPlot_->leafTextFont();
  };

  auto font = calcTextFont();

  plot()->setPainterFont(device, font);

  //---

  auto isValueLabel = [&]() {
    if (isRoot()) return dendrogramPlot_->isRootValueLabel();
    if (isHier()) return dendrogramPlot_->isHierValueLabel();
    return dendrogramPlot_->isLeafValueLabel();
  };

  // draw node text
  // only support contrast and clip
  const auto &name = this->name();

  QStringList strs;

  strs << name;

  if (isValueLabel()) {
    if      (value().isSet())
      strs << QString::number(value().real());
    else if (hierValue().isSet())
      strs << QString::number(hierValue().real());
  }

  auto calcTextOptions = [&]() {
    if (isRoot()) return dendrogramPlot_->rootTextOptions(device);
    if (isHier()) return dendrogramPlot_->hierTextOptions(device);
    return dendrogramPlot_->leafTextOptions(device);
  };

  auto textOptions = calcTextOptions();

  textOptions.angle     = angle;
  textOptions.align     = align;
  textOptions.formatted = false;
  textOptions.html      = false;

  if (textOptions.scaled && textCenter) {
    auto rect1 = displayRect();
    if (! rect1.isValid()) return;

    if (strs.size() == 1)
      CQChartsDrawUtil::drawTextInBox(device, rect1, name, textOptions);
    else
      CQChartsDrawUtil::drawTextsInBox(device, rect1, strs, textOptions);
  }
  else {
    if (strs.size() == 1)
      CQChartsDrawUtil::drawTextAtPoint(device, p, name, textOptions, centered);
    else
      CQChartsDrawUtil::drawTextsAtPoint(device, p, strs, textOptions);
  }

  //---

  if (textCenter)
    charts()->resetContrastColor();
}

void
CQChartsDendrogramNodeObj::
calcTextPos(Point &p, const QFont &font, Angle &angle, uint &iposition,
            Qt::Alignment &align, bool &centered) const
{
  auto rect1 = displayRect();

  const auto &name = this->name();

  QFontMetricsF fm(font);

  double ta = fm.ascent();
  double td = fm.descent();
  double tw = fm.horizontalAdvance(name);

//double dy = (ta - td)/2.0;
//double dy = 0.0;

  BBox pbbox = plot()->windowToPixel(rect1);

  auto calcTextMargin = [&]() {
    if (isRoot()) return dendrogramPlot_->rootTextMargin();
    if (isHier()) return dendrogramPlot_->hierTextMargin();
    return dendrogramPlot_->leafTextMargin();
  };

  auto textMargin = calcTextMargin();

  double tm = std::max(textMargin, 1.0);

  align    = Qt::AlignLeft | Qt::AlignVCenter;
  centered = false;

  Point pp;

  using TextPosition = CQChartsDendrogramPlot::TextPosition;

  auto position      = TextPosition::CENTER;
  bool isRotatedText = false;

  if (plot()->placeType() != DendrogramPlot::PlaceType::CIRCULAR) {
    auto calcTextPosition = [&]() {
      if (isRoot()) return dendrogramPlot_->rootTextPosition();
      if (isHier()) return dendrogramPlot_->hierTextPosition();
      return dendrogramPlot_->leafTextPosition();
    };

    auto calcIsRotatedText = [&]() {
      if (isRoot()) return dendrogramPlot_->isRootRotatedText();
      if (isHier()) return dendrogramPlot_->isHierRotatedText();
      return dendrogramPlot_->isLeafRotatedText();
    };

    position      = calcTextPosition();
    isRotatedText = calcIsRotatedText();

    //---

    if (plot()->orientation() == Qt::Horizontal) {
      if      (position == TextPosition::LEFT)
        pp = Point(pbbox.getXMin() - tw - tm, pbbox.getYMid()); // align right
      else if (position == TextPosition::RIGHT)
        pp = Point(pbbox.getXMax()      + tm, pbbox.getYMid()); // align left
      else {
        pp    = pbbox.getCenter();
        align = Qt::AlignCenter;
      }
    }
    else {
      if (isRotatedText) {
        angle = Angle(90);
        align = Qt::AlignHCenter | Qt::AlignVCenter;

        if      (position == TextPosition::LEFT)
          pp = Point(pbbox.getXMid(), pbbox.getYMin() - tw/2.0 - tm); // align right
        else if (position == TextPosition::RIGHT)
          pp = Point(pbbox.getXMid(), pbbox.getYMax() + tw/2.0 + tm); // align left
        else {
          pp    = pbbox.getCenter();
          align = Qt::AlignCenter;
        }

        centered = true;
      }
      else {
        if      (position == TextPosition::LEFT)
          pp = Point(pbbox.getXMid() - tw/2.0, pbbox.getYMin() - tm - td); // align top
        else if (position == TextPosition::RIGHT)
          pp = Point(pbbox.getXMid() - tw/2.0, pbbox.getYMax() + tm + ta); // align bottom
        else {
          pp    = pbbox.getCenter();
          align = Qt::AlignCenter;
        }
      }
    }
  }
  else {
    pp    = pbbox.getCenter();
    align = Qt::AlignCenter;

    if (! isRoot()) {
      auto p1 = plot()->pixelToWindow(pp);

      auto angle1 = Angle::radians(std::atan2(p1.y, p1.x));

      //---

      double dx1 = 0.0;

      if (isRotatedText) {
        dx1 = plot()->pixelToWindowWidth (tw/2.0);
      //dy1 = plot()->pixelToWindowHeight(dy);
      }
      else {
        dx1 = plot()->pixelToWindowWidth(tm);
      }

      auto x1 = p1.x + dx1*angle1.cos();
      auto y1 = p1.y + dx1*angle1.sin();

      pp = plot()->windowToPixel(Point(x1, y1));

      //---

      if (isRotatedText) {
        if (angle1.cos() < 0.0)
          angle1.flipX();

        angle = angle1;

        centered = true;
      }
      else {
        align = Qt::Alignment();

        if      (pp.x < 0.0) align |= Qt::AlignRight;
        else if (pp.x > 0.0) align |= Qt::AlignLeft;

        if      (pp.y < 0.0) align |= Qt::AlignTop;
        else if (pp.y < 0.0) align |= Qt::AlignBottom;
      }
    }
  }

  p = plot()->pixelToWindow(pp);

  iposition = uint(position);
}

CQChartsGeom::BBox
CQChartsDendrogramNodeObj::
displayRect() const
{
  auto rect1 = rect();

  // get shape size (scaled to value)
  auto ss = calcScaledShapePixelSize();

  // aspect w/h (> 1.0 wider, < 1.0 taller)
   // get specified shape size
  auto calcAspect = [&]() {
    if (isRoot()) return dendrogramPlot_->rootAspect();
    if (isHier()) return dendrogramPlot_->hierAspect();
    return dendrogramPlot_->leafAspect();
  };

  auto aspect = calcAspect();
  if (aspect <= 0.0) aspect = 1.0;

  auto shapeWidth  = aspect*dendrogramPlot_->pixelToWindowWidth (ss);
  auto shapeHeight = dendrogramPlot_->pixelToWindowHeight(ss)/aspect;

  auto xf = shapeWidth /rect1.getWidth ();
  auto yf = shapeHeight/rect1.getHeight();

  auto scale = dendrogramPlot_->overlapScale()*dendrogramPlot_->sizeScale();

  xf *= scale;
  yf *= scale;

  rect1 = rect1.centerScaled(xf, yf);

  return rect1;
}

double
CQChartsDendrogramNodeObj::
calcScaledShapePixelSize() const
{
  // get specified shape size
  auto calcSize = [&]() {
    if (isRoot()) return plot()->calcRootSize();
    if (isHier()) return plot()->calcHierSize();
    return plot()->calcLeafSize();
  };

  auto minSize = [&]() {
    if (isRoot()) return plot()->rootMinSize();
    if (isHier()) return plot()->hierMinSize();
    return plot()->leafMinSize();
  };

  auto leafSize    = calcSize();
  auto leafMinSize = plot()->lengthPixelWidth(minSize());

  if      (plot()->nodeSizeByValue() == CQChartsDendrogramPlot::ValueType::HIER) {
    auto value     = this->hierValue().realOr(0.0);
    auto sizeScale = dendrogramPlot_->mapHierValue(value);

    leafSize = CMathUtil::map(sizeScale, 0.0, 1.0, leafMinSize, leafSize);
  }
  else if (plot()->nodeSizeByValue() == CQChartsDendrogramPlot::ValueType::NODE) {
    auto value     = this->value().realOr(0.0);
    auto sizeScale = dendrogramPlot_->mapNodeValue(value);

    leafSize = CMathUtil::map(sizeScale, 0.0, 1.0, leafMinSize, leafSize);
  }
  // scale to size column value
  else if (plot()->sizeColumn().isValid()) {
    // normalize to parent hierarchical size
    double maxSize = 0.0;

    if (parent())
      maxSize = parent()->hierSize();
    else
      maxSize = plot()->rootNodeObj()->hierSize();

    double size = 0.0;

    bool isHier = this->isHier();

    if (isHier)
      size = hierSize();
    else
      size = this->size().realOr(0.0);

    leafSize = CMathUtil::mapSqr(size, 0.0, maxSize, 0.0, leafSize);
  }

  return leafSize;
}

#if 0
void
CQChartsDendrogramNodeObj::
movePerpCenter(double pos)
{
  auto rect = this->rect();

  if (dendrogramPlot_->orientation() == Qt::Horizontal) {
    auto d = pos - rect.getYMid();

    rect.moveBy(Point(0.0, d));
  }
  else {
    auto d = pos - rect.getXMid();

    rect.moveBy(Point(d, 0.0));
  }

  this->setRect(rect);
}
#endif

void
CQChartsDendrogramNodeObj::
movePerpBy(double d)
{
  auto rect = this->rect();

  if (dendrogramPlot_->orientation() == Qt::Horizontal)
    rect.moveBy(Point(0.0, d));
  else
    rect.moveBy(Point(d, 0.0));

  this->setRect(rect);
}

void
CQChartsDendrogramNodeObj::
moveParBy(double d)
{
  auto rect = this->rect();

  if (dendrogramPlot_->orientation() == Qt::Horizontal)
    rect.moveBy(Point(d, 0.0));
  else
    rect.moveBy(Point(0.0, d));

  this->setRect(rect);
}

//---

CQChartsDendrogramEdgeObj::
CQChartsDendrogramEdgeObj(const DendrogramPlot *dendrogramPlot, NodeObj *fromNode,
                          NodeObj *toNode, const BBox &rect,
                          const ColorInd &ig, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<DendrogramPlot *>(dendrogramPlot), rect, ColorInd(), ig, iv),
 dendrogramPlot_(dendrogramPlot), fromNode_(fromNode), toNode_(toNode)
{
  auto *fromNode1 = dynamic_cast<const PlotDendrogram::PlotNode *>(fromNode->node());
  auto *toNode1   = dynamic_cast<const PlotDendrogram::PlotNode *>(toNode->node());

  if (fromNode1 && toNode1) {
    auto value = fromNode1->childValue(toNode1);

    if (value)
      setValue(OptReal(*value));

    if (fromNode1->edgeColor(toNode1).isValid())
      setColor(fromNode1->edgeColor(toNode1));

    if (fromNode1->edgeColorValue(toNode1).isSet())
      setColorValue(fromNode1->edgeColorValue(toNode1));

    if (fromNode1->edgeSizeValue(toNode1).isSet())
      setSize(fromNode1->edgeSizeValue(toNode1));
  }
}

QString
CQChartsDendrogramEdgeObj::
calcId() const
{
  return QString("%1:%2").arg(typeName()).arg(ind());
}

QString
CQChartsDendrogramEdgeObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  plot()->addNoTipColumns(tableTip);

  //---

  if (value().isSet())
    tableTip.addTableRow("Value", value().real());

  if      (colorValue().isSet())
    tableTip.addTableRow("Color", colorValue().real());
  else if (color().isValid())
    tableTip.addTableRow("Color", color().toString());

  if (size().isSet())
    tableTip.addTableRow("Size", size().real());

  //---

  return tableTip.str();
}

//---

bool
CQChartsDendrogramEdgeObj::
inside(const Point &p) const
{
  if (! path_.isEmpty())
    return path_.contains(p.qpoint());
  else
    return CQChartsPlotObj::inside(p);
}

//---

void
CQChartsDendrogramEdgeObj::
draw(PaintDevice *device) const
{
  if (fromNode_->isRoot() && ! plot()->isRootVisible())
    return;

  //---

  auto rect1 = fromNode_->displayRect();
  auto rect2 = toNode_  ->displayRect();

  auto pbbox1 = plot()->windowToPixel(rect1); // from
  auto pbbox2 = plot()->windowToPixel(rect2); // to

  //---

  bool updateState = device->isInteractive();

  // calc pen and brush
  PenBrush lpenBrush;

  calcPenBrush(lpenBrush, updateState);

  CQChartsDrawUtil::setPenBrush(device, lpenBrush);

  //---

  // draw edge
  double x1, y1, x4, y4;

  if (plot()->placeType() != DendrogramPlot::PlaceType::CIRCULAR) {
    if (plot()->orientation() == Qt::Horizontal) {
      x1 = pbbox1.getXMax(); y1 = pbbox1.getYMid();
      x4 = pbbox2.getXMin(); y4 = pbbox2.getYMid();
    }
    else {
      x1 = pbbox1.getXMid(); y1 = pbbox1.getYMax();
      x4 = pbbox2.getXMid(); y4 = pbbox2.getYMin();
    }
  }
  else {
    x1 = pbbox1.getXMid(); y1 = pbbox1.getYMid();
    x4 = pbbox2.getXMid(); y4 = pbbox2.getYMid();
  }

  auto p1 = plot()->pixelToWindow(Point(x1, y1));
  auto p4 = plot()->pixelToWindow(Point(x4, y4));

  //---

  // calc edge width
  auto lw    = dendrogramPlot_->lengthPlotWidth(dendrogramPlot_->edgeWidth());
  auto minLw = dendrogramPlot_->lengthPlotWidth(dendrogramPlot_->minEdgeWidth());

  if (dendrogramPlot_->isEdgeSizeByValue()) {
    if (value().isSet()) {
      auto widthScale = dendrogramPlot_->mapValue(value().real());

      lw = CMathUtil::map(widthScale, 0.0, 1.0, minLw, lw);
    }
  }

  lw = std::max(lw, minLw);

  //---

  bool edgeFilled = plot()->isEdgeFilled();

  path_ = QPainterPath();

  if (plot()->placeType() != DendrogramPlot::PlaceType::CIRCULAR) {
    auto angle1 = Angle::fromOrientation(plot()->orientation());
    auto angle2 = angle1.flippedX();

    if (edgeFilled)
      CQChartsDrawUtil::edgePath(path_, p1, p4, lw, CQChartsDrawUtil::EdgeType::ARC,
                                 angle1, angle2);
    else
      CQChartsDrawUtil::curvePath(path_, p1, p4, CQChartsDrawUtil::EdgeType::ARC,
                                  angle1, angle2);

    device->drawPath(path_);
  }
  else {
    if (edgeFilled)
      CQChartsDrawUtil::drawRoundedLine(device, p1, p4, lw);
    else
      device->drawLine(p1, p4);
  }
}

void
CQChartsDendrogramEdgeObj::
calcPenBrush(PenBrush &penBrush, bool updateState) const
{
  auto colorInd = calcColorInd();

  auto strokeColor = plot()->interpEdgeStrokeColor(colorInd);

  QColor fillColor;

  if      (dendrogramPlot_->isEdgeColorByValue()) {
    auto value = this->value().realOr(0.0);
    auto color = dendrogramPlot_->mapValue(value);

    fillColor = plot()->interpPaletteColor(ColorInd(color));
  }
  else if (color().isValid()) {
    fillColor = plot()->interpColor(color(), colorInd);
  }
  else
    fillColor = plot()->interpEdgeFillColor(colorInd);

  plot()->setPenBrush(penBrush, plot()->edgePenData(strokeColor),
                      plot()->edgeBrushData(fillColor));

  if (updateState)
    plot()->updateObjPenBrushState(this, penBrush);
}

//---

bool
CQChartsDendrogramPlot::
executeSlotFn(const QString &name, const QVariantList &args, QVariant &res)
{
  if      (name == "expand")
    expandSlot();
  else if (name == "expand_all")
    expandAllSlot();
  else if (name == "collapse_all")
    collapseAllSlot();
  else
    return CQChartsHierPlot::executeSlotFn(name, args, res);

  return true;
}

//------

CQChartsDendrogramPlotCustomControls::
CQChartsDendrogramPlotCustomControls(CQCharts *charts) :
 CQChartsHierPlotCustomControls(charts, "dendrogram")
{
}

void
CQChartsDendrogramPlotCustomControls::
init()
{
  addWidgets();

  addOverview();

  addLayoutStretch();

  addButtonWidgets();

  connectSlots(true);
}

void
CQChartsDendrogramPlotCustomControls::
addWidgets()
{
  addColumnWidgets();

  addOptionsWidgets();

  addColorColumnWidgets("Node/Edge Color");

//addKeyList();
}

void
CQChartsDendrogramPlotCustomControls::
addColumnWidgets()
{
  // columns group
  auto columnsFrame = createGroupFrame("Columns", "columnsFrame");

  addNamedColumnWidgets(QStringList() << "name" << "link" << "value" << "color" << "size",
                        columnsFrame);
}

void
CQChartsDendrogramPlotCustomControls::
addOptionsWidgets()
{
  // options group
  optionsFrame_ = createGroupFrame("Options", "optionsFrame");

  orientationCombo_ = createEnumEdit("orientation");

  addFrameWidget(optionsFrame_, "Orientation", orientationCombo_);
}

void
CQChartsDendrogramPlotCustomControls::
addButtonWidgets()
{
  // buttons group
  buttonsFrame_ = createGroupFrame("Functions", "buttonsFrame", FrameOpts::makeHBox());

  expand_      = CQUtil::makeLabelWidget<QPushButton>("Expand", "expandButton");
  expandAll_   = CQUtil::makeLabelWidget<QPushButton>("Expand All", "expandAllButton");
  collapseAll_ = CQUtil::makeLabelWidget<QPushButton>("Collapse All", "collapseAllButton");

  buttonsFrame_.box->addWidget(expand_);
  buttonsFrame_.box->addWidget(expandAll_);
  buttonsFrame_.box->addWidget(collapseAll_);

  buttonsFrame_.box->addStretch(1);
}

void
CQChartsDendrogramPlotCustomControls::
setPlot(CQChartsPlot *plot)
{
  if (plot_ && dendrogramPlot_)
    disconnect(dendrogramPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));

  dendrogramPlot_ = dynamic_cast<CQChartsDendrogramPlot *>(plot);

  CQChartsHierPlotCustomControls::setPlot(plot);

  if (dendrogramPlot_)
    connect(dendrogramPlot_, SIGNAL(customDataChanged()), this, SLOT(updateWidgets()));
}

void
CQChartsDendrogramPlotCustomControls::
updateWidgets()
{
  CQChartsHierPlotCustomControls::updateWidgets();

  //---

  connectSlots(false);

  if (orientationCombo_)
    orientationCombo_->setCurrentValue(static_cast<int>(dendrogramPlot_->orientation()));

  connectSlots(true);
}

void
CQChartsDendrogramPlotCustomControls::
connectSlots(bool b)
{
  CQChartsHierPlotCustomControls::connectSlots(b);

  CQUtil::optConnectDisconnect(b,
    orientationCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(orientationSlot()));

  CQUtil::optConnectDisconnect(b,
    expand_     , SIGNAL(clicked()), dendrogramPlot_, SLOT(expandSlot()));
  CQUtil::optConnectDisconnect(b,
    expandAll_  , SIGNAL(clicked()), dendrogramPlot_, SLOT(expandAllSlot()));
  CQUtil::optConnectDisconnect(b,
    collapseAll_, SIGNAL(clicked()), dendrogramPlot_, SLOT(collapseAllSlot()));
}

void
CQChartsDendrogramPlotCustomControls::
orientationSlot()
{
  dendrogramPlot_->setOrientation(
    static_cast<Qt::Orientation>(orientationCombo_->currentValue()));
}
