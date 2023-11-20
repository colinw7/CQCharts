#ifndef CQChartsHierParallelPlot_H
#define CQChartsHierParallelPlot_H

#include <CQChartsHierPlot.h>
#include <CQChartsHierPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CSafeIndex.h>

class CQChartsHierParallelPlot;

class CQChartsHierParallelNode {
 public:
  using HierParallelPlot = CQChartsHierParallelPlot;
  using Node             = CQChartsHierParallelNode;

  struct NodeValue {
    Node*  node  { nullptr };
    double value { 0.0 };

    NodeValue(Node *node1, double &value1) :
     node(node1), value(value1) {
    }
  };

  using Children    = std::vector<Node *>;
  using Point       = CQChartsGeom::Point;
  using Values      = std::vector<NodeValue>;
  using ValuesArray = std::vector<Values>;

  CQChartsHierParallelNode(const HierParallelPlot *plot, Node *parent, const QString &name) :
   parallelPlot_(plot), parent_(parent), name_(name) {
    if (parent)
      parent->children_.push_back(this);
  }

 ~CQChartsHierParallelNode() {
    for (auto *child : children_)
      delete child;
  }

  Node *parent() const { return parent_; }

  const QString &name() const { return name_; }

  QString hierName() {
    auto parentName = (parent_ ? parent_->hierName() : "");

    return (parentName.length() ? parentName + "/" + name_ : name_);
  }

  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

  const QModelIndex &modelInd() const { return modelInd_; }
  void setModelInd(const QModelIndex &v) { modelInd_ = v; }

  int parentDepth() const {
    return (parent_ ? parent_->parentDepth() + 1 : 0);
  }

  int childDepth() const {
    int d = 0;

    for (auto *child : children_)
      d = std::max(d, child->childDepth() + 1);

    return d;
  }

  Node *getChild(const QString &name) const {
    for (auto *child : children_)
      if (child->name_ == name)
        return child;

    return nullptr;
  }

  void valueArrays(ValuesArray &valuesArray) {
    Values values;
    valueArrays1(valuesArray, values, 0);
  }

 private:
  void valueArrays1(ValuesArray &valuesArray, Values &values, uint depth) {
    values.push_back(NodeValue(this, value_));

    if (children_.empty()) {
      valuesArray.push_back(values);
    }
    else {
      for (auto *child : children_) {
        auto values1 = values;

        child->valueArrays1(valuesArray, values1, depth + 1);
      }
    }
  }

 private:
  const HierParallelPlot* parallelPlot_ { nullptr }; //!< parent plot
  Node*                   parent_       { nullptr }; //!< parent node
  Children                children_;                 //!< child nodes
  QString                 name_;                     //!< node name
  double                  value_        { 0.0 };     //!< node value
  QModelIndex             modelInd_;                 //!< model index
};

//---

/*!
 * \brief Parallel plot type
 * \ingroup Charts
 */
class CQChartsHierParallelPlotType : public CQChartsHierPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsHierParallelPlotType();

  QString name() const override { return "hierparallel"; }
  QString desc() const override { return "HierParallel"; }

  Category category() const override { return Category::HIER; }

  void addParameters() override;

  bool allowXLog() const override { return false; }

  bool canProbe() const override { return true; }

  QString description() const override;

  //---

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsHierParallelPlot;

/*!
 * \brief Parallel Plot Line object
 * \ingroup Charts
 */
class CQChartsHierParallelLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using HierParallelPlot = CQChartsHierParallelPlot;

 public:
  CQChartsHierParallelLineObj(const HierParallelPlot *plot, const BBox &rect, int ind,
                              const Polygon &poly, const ColorInd &is);

  //---

  QString typeName() const override { return "line"; }

  //---

  bool isPolygon() const override { return true; }
  Polygon polygon() const override { return polyLine_; }

  bool isSolid() const override { return false; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isVisible() const override;

  bool inside(const Point &p) const override;

  //---

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawOverlay(PaintDevice *device) const;

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const override;

  bool hasIndex(const QModelIndex &ind) const;

 private:
  void getPolyLine(Polygon &poly) const;

 private:
  const HierParallelPlot* parallelPlot_ { nullptr }; //!< plot
  int                     ind_          { 0 };       //!< line index
  Polygon                 poly_;                     //!< polygon
  mutable Polygon         polyLine_;                 //!< polyline
};

//---

/*!
 * \brief Parallel Plot Point object
 * \ingroup Charts
 */
class CQChartsHierParallelPointObj : public CQChartsPlotPointObj {
  Q_OBJECT

 public:
  using HierParallelPlot = CQChartsHierParallelPlot;
  using Column           = CQChartsColumn;
  using Length           = CQChartsLength;
  using Symbol           = CQChartsSymbol;

 public:
  CQChartsHierParallelPointObj(const HierParallelPlot *plot, const BBox &rect, const QString &name,
                               const QString &hierName, int depth, double yval, const Point &p,
                               const QModelIndex &ind, const ColorInd &is, const ColorInd &iv);

  //---

  const HierParallelPlot *parallelPlot() const { return parallelPlot_; }

  const QString &hierName() const { return hierName_; }

  const QString &name() const { return name_; }

  int depth() const { return depth_; }

  double value() const { return value_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isVisible() const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) const override;

  void drawOverlay(PaintDevice *device) const;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  Length calcSymbolSize() const override;

  Point calcPoint() const;

 private:
  const HierParallelPlot* parallelPlot_ { nullptr }; //!< plot
  QString                 name_;                     //!< name
  QString                 hierName_;                 //!< hierarical name
  double                  depth_        { 0 };       //!< depth value
  double                  value_        { 0.0 };     //!< value
  Point                   point_;
};

//---

/*!
 * \brief Parallel Plot
 * \ingroup Charts
 */
class CQChartsHierParallelPlot : public CQChartsHierPlot,
 public CQChartsObjLineData <CQChartsHierParallelPlot>,
 public CQChartsObjPointData<CQChartsHierParallelPlot> {
  Q_OBJECT

  // options
  Q_PROPERTY(Qt::Orientation orientation READ orientation  WRITE setOrientation)
  Q_PROPERTY(bool            normalized  READ isNormalized WRITE setNormalized)
  Q_PROPERTY(QString         labels      READ labels       WRITE setLabels)

  // root
  Q_PROPERTY(bool rootVisible READ isRootVisible WRITE setRootVisible)

  // axis
  Q_PROPERTY(AxisLabelPos axisLabelPos READ axisLabelPos WRITE setAxisLabelPos)
  Q_PROPERTY(bool         axisLocal    READ isAxisLocal  WRITE setAxisLocal)
  Q_PROPERTY(bool         axisSpread   READ isAxisSpread WRITE setAxisSpread)

  // lines (display, stroke)
  CQCHARTS_LINE_DATA_PROPERTIES

  Q_PROPERTY(bool linesSelectable  READ isLinesSelectable  WRITE setLinesSelectable)
  Q_PROPERTY(bool linesNodeColored READ isLinesNodeColored WRITE setLinesNodeColored)

  // points (display, symbol)
  CQCHARTS_POINT_DATA_PROPERTIES

  Q_ENUMS(AxisLabelPos)

 public:
  using Node     = CQChartsHierParallelNode;
  using Color    = CQChartsColor;
  using Alpha    = CQChartsAlpha;
  using Symbol   = CQChartsSymbol;
  using PenBrush = CQChartsPenBrush;
  using PenData  = CQChartsPenData;
  using Length   = CQChartsLength;
  using ColorInd = CQChartsUtil::ColorInd;
  using RMinMax  = CQChartsGeom::RMinMax;

  using LineObj  = CQChartsHierParallelLineObj;
  using PointObj = CQChartsHierParallelPointObj;

  enum class AxisLabelPos {
    AXIS,
    TOP,
    BOTTOM,
    ALTERNATE
  };

 public:
  CQChartsHierParallelPlot(View *view, const ModelP &model);
 ~CQChartsHierParallelPlot();

  //---

  void init() override;
  void term() override;

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  Columns getNamedColumns(const QString &name) const override;
  void setNamedColumns(const QString &name, const Columns &c) override;

  //---

  //! get/set orientation
  const Qt::Orientation &orientation() const { return orientation_; }
  void setOrientation(const Qt::Orientation &orient);

  bool isHorizontal() const { return orientation() == Qt::Horizontal; }
  bool isVertical  () const { return orientation() == Qt::Vertical  ; }

  //---

  bool isNormalized() const { return normalized_; }
  void setNormalized(bool b);

  QString labels() const;
  void setLabels(const QString &s);

  //---

  //! get/set lines selectable
  bool isLinesSelectable() const { return linesSelectable_; }
  void setLinesSelectable(bool b);

  //! get/set lines colored from nodes
  bool isLinesNodeColored() const { return linesNodeColored_; }
  void setLinesNodeColored(bool b);

  //---

  //! get/set root visible
  bool isRootVisible() const { return rootVisible_; }
  void setRootVisible(bool b);

  //---

  //! get/set axis label pos
  const AxisLabelPos &axisLabelPos() const { return axisLabelPos_; }
  void setAxisLabelPos(const AxisLabelPos &p);

  //! get/set local axis for each range
  bool isAxisLocal() const { return axisLocal_; }
  void setAxisLocal(bool b);

  //! get/set spread axis to global range
  bool isAxisSpread() const { return axisSpread_; }
  void setAxisSpread(bool b);

  //---

  RMinMax depthRange(int i) const {
    auto pr = depthRanges_.find(i);
    if (pr != depthRanges_.end())
      return (*pr).second;

    return RMinMax();
  }

  Axis *axis(int i) const { return CUtil::safeIndex(axes_, i).get(); }

  //---

  void addProperties() override;

  //---

  void initRange() override;

  Range calcRange() const override;

  Node *addNode(Node *parent, const QString &name, const QModelIndex &nameInd) const;

  void updateAxes();

  //---

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool probe(ProbeData &probeData) const override;

  bool addMenuItems(QMenu *menu, const Point &p) override;

  //---

  void redrawAxis(CQChartsAxis *axis, bool wait) override;

  BBox axesFitBBox() const override;

  BBox calcExtraFitBBox() const override;

  bool hasFgAxes() const override;

  void drawFgAxes(PaintDevice *device) const override;

  //---

  bool hasForeground() const override;
  void execDrawForeground(PaintDevice *) const override;

  bool hasOverlay() const override;
  void execDrawOverlay(PaintDevice *device) const override;

  //---

  const PointObj *getModelPointObj(const QModelIndex &ind) const;

 protected:
  virtual LineObj *createLineObj(const BBox &rect, int ind, const Polygon &poly,
                                 const ColorInd &is) const;

  virtual PointObj *createPointObj(const BBox &rect, const QString &name, const QString &hierName,
                                   int depth, double yval, const Point &p,
                                   const QModelIndex &modelInd, const ColorInd &is,
                                   const ColorInd &iv) const;

 public Q_SLOTS:
  // set horizontal
  void setHorizontal(bool b);

 protected:
  CQChartsHierPlotCustomControls *createCustomControls() override;

 private:
  using DepthRanges   = std::map<int, RMinMax>;
  using AxisP         = std::unique_ptr<CQChartsAxis>;
  using YAxes         = std::vector<AxisP>;
  using ColumnVisible = std::map<int, bool>;

  Qt::Orientation orientation_ { Qt::Vertical }; //!< axis orientation
  bool            normalized_  { true };         //!< is normalized
  QStringList     labels_;

  Range dataRange_;           //!< global data range
  Range normalizedDataRange_; //!< normalized data range

  // lines
  bool linesSelectable_  { false }; //!< are lines selectable
  bool linesNodeColored_ { false }; //!< are lines colored from nodes

  DepthRanges depthRanges_; //!< range at depth

  // axes
  Qt::Orientation adir_      { Qt::Horizontal }; //!< axis direction
  AxisP           mainXAxis_;                    //!< main x axis
  AxisP           mainYAxis_;                    //!< main y axis
  YAxes           axes_;                         //!< value axes
  BBox            axesBBox_;                     //!< axes bbox

  double max_tw_ { 0.0 }; //!< max text width

  bool rootVisible_ { false }; //!< root is visible

  AxisLabelPos axisLabelPos_ { AxisLabelPos::TOP };
  bool         axisLocal_    { true };
  bool         axisSpread_   { true };

  mutable Node *root_  { nullptr };
  mutable int   depth_ { 1 };
};

//---

/*!
 * \brief Parallel Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsHierParallelPlotCustomControls : public CQChartsHierPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsHierParallelPlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(Plot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void addWidgets() override;

  void addColumnWidgets() override;

  void addOptionsWidgets() override;

  void connectSlots(bool b) override;

  //---

  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 protected Q_SLOTS:
  void orientationSlot();
  void normalizedSlot();

 protected:
  CQChartsHierParallelPlot* parallelPlot_ { nullptr };

  FrameData optionsFrame_;

  CQChartsEnumParameterEdit* orientationCombo_ { nullptr };
  CQChartsBoolParameterEdit* normalizedCheck_  { nullptr };
};

#endif
