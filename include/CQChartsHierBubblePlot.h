#ifndef CQChartsHierBubblePlot_H
#define CQChartsHierBubblePlot_H

#include <CQChartsHierPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsCirclePack.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsData.h>
#include <QModelIndex>

//---

/*!
 * \brief Hierarchical Bubble Plot Type
 * \ingroup Charts
 */
class CQChartsHierBubblePlotType : public CQChartsHierPlotType {
 public:
  CQChartsHierBubblePlotType();

  QString name() const override { return "hierbubble"; }
  QString desc() const override { return "HierBubble"; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  bool customXRange() const override { return false; }
  bool customYRange() const override { return false; }

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  bool canEqualScale() const override { return true; }

  bool supportsIdColumn() const override { return true; }

  QString description() const override;

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsHierBubblePlot;
class CQChartsHierBubbleHierNode;

/*!
 * \brief Hierarchical Bubble Plot Node
 * \ingroup Charts
 */
class CQChartsHierBubbleNode : public CQChartsCircleNode {
 protected:
  static uint nextId() {
    static int lastId = 0;

    return ++lastId;
  }

 public:
  using Plot     = CQChartsHierBubblePlot;
  using Node     = CQChartsHierBubbleNode;
  using HierNode = CQChartsHierBubbleHierNode;
  using Color    = CQChartsColor;
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsHierBubbleNode(const Plot *plot, HierNode *parent, const QString &name,
                         double size, const QModelIndex &ind);

  virtual ~CQChartsHierBubbleNode();

  void initRadius();

  const Plot *plot() const { return plot_; }

  HierNode *parent() const { return parent_; }

  virtual uint id() const { return id_; }

  virtual const QString &name() const { return name_; }

  //! get/set siz
  virtual double size() const { return size_; }
  virtual void setSize(double s) { size_ = s; }

  //! get/set radius
  double radius() const override { return r_; }
  virtual void setRadius(double r) { r_ = r; }

  //! get/set x
  double x() const override { return CQChartsCircleNode::x(); }
  void setX(double x) override { CQChartsCircleNode::setX(x); }

  //! get/set y
  double y() const override { return CQChartsCircleNode::y(); }
  void setY(double y) override { CQChartsCircleNode::setY(y); }

  //! get/set colorId
  virtual int colorId() const { return colorId_; }
  virtual void setColorId(int id) { colorId_ = id; }

  //! get/set color
  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &v) { color_ = v; }

  //! get/set model index
  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  //! get/set depth
  virtual int depth() const { return depth_; }
  virtual void setDepth(int i) { depth_ = i; }

  //! get/set is filler
  bool isFiller() const { return filler_; }
  void setFiller(bool b) { filler_ = b; }

  //! get hier size
  virtual double hierSize() const { return size(); }

  //! get hier name
  virtual QString hierName() const;

  //! reset placement position
  virtual void resetPosition() {
    //CQChartsCircleNode::setPosition(0.0, 0.0);

    //placed_ = false;
  }

  //! set placement position
  void setPosition(double x, double y) override;

  //! get is placed
  virtual bool placed() const { return placed_; }

  //! sort by radius
  friend bool operator<(const Node &n1, const Node &n2) {
    return n1.r_ < n2.r_;
  }

  //! interp color
  virtual QColor interpColor(const Plot *plot, const CQChartsColor &c,
                             const ColorInd &colorInd, int n) const;

 protected:
  const Plot*   plot_    { nullptr }; //!< parent plot
  HierNode*     parent_  { nullptr }; //!< parent hier node
  uint          id_      { 0 };       //!< node id
  QString       name_;                //!< node name
  double        size_    { 0.0 };     //!< node size
  int           colorId_ { -1 };      //!< node color index
  CQChartsColor color_;               //!< node explicit color
  QModelIndex   ind_;                 //!< node model index
  int           depth_   { 0 };       //!< node depth
  bool          filler_  { false };   //!< is filler
  bool          placed_  { false };   //!< is placed
};

//---

/*!
 * \brief hier bubble node compare functor
 * \ingroup Charts
 */
struct CQChartsHierBubbleNodeCmp {
  using Node = CQChartsHierBubbleNode;

  bool operator()(const Node *n1, const Node *n2) {
    return (*n1) < (*n2);
  }
};

//---

/*!
 * \brief Hierarchical Bubble Plot Hierarchical Node
 * \ingroup Charts
 */
class CQChartsHierBubbleHierNode : public CQChartsHierBubbleNode {
 public:
  using Plot     = CQChartsHierBubblePlot;
  using HierNode = CQChartsHierBubbleHierNode;
  using Node     = CQChartsHierBubbleNode;
  using Nodes    = std::vector<Node*>;
  using Children = std::vector<HierNode*>;
  using Pack     = CQChartsCirclePack<Node>;

 public:
  CQChartsHierBubbleHierNode(const Plot *plot, HierNode *parent, const QString &name,
                             const QModelIndex &ind=QModelIndex());

 ~CQChartsHierBubbleHierNode();

  int hierInd() const { return hierInd_; }
  void setHierInd(int i) { hierInd_ = i; }

  //---

  //! get/set is expanded
  bool isExpanded() const { return expanded_; }
  void setExpanded(bool b) { expanded_ = b; }

  bool isHierExpanded() const;

  //---

  double hierSize() const override;

  //---

  bool hasNodes() const { return ! nodes_.empty(); }

  const Nodes &getNodes() const { return nodes_; }
  Nodes &getNodes() { return nodes_; }

  //---

  const Pack &pack() const { return pack_; }
  Pack &pack() { return pack_; }

  //---

  bool hasChildren() const { return ! children_.empty(); }

  const Children &getChildren() const { return children_; }

  //---

  void packNodes();

  void addNode(Node *node);

  void removeNode(Node *node);

  void setPosition(double x, double y) override;

  QColor interpColor(const Plot *plot, const CQChartsColor &c, const ColorInd &colorInd,
                     int n) const override;

 protected:
  Nodes    nodes_;             //!< child nodes
  Pack     pack_;              //!< circle pack
  Children children_;          //!< child hier nodes
  int      hierInd_  { -1 };   //!< hier index
  bool     expanded_ { true }; //!< is expanded
};

//---

class CQChartsHierBubbleHierObj;

/*!
 * \brief Hierarchical Bubble Plot object
 * \ingroup Charts
 */
class CQChartsHierBubbleNodeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot    = CQChartsHierBubblePlot;
  using Node    = CQChartsHierBubbleNode;
  using HierObj = CQChartsHierBubbleHierObj;

 public:
  CQChartsHierBubbleNodeObj(const Plot *plot, Node *node, HierObj *hierObj,
                            const BBox &rect, const ColorInd &is);

  QString typeName() const override { return "bubble"; }

  //---

  bool isCircle() const override { return true; }

  double radius() const override { return node_->radius(); }

  //---

  Node *node() const { return node_; }

  HierObj *parent() const { return hierObj_; }

  int ind() const { return ind_; }
  void setInd(int ind) { ind_ = ind; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawText(PaintDevice *device, const BBox &bbox) const;

  //---

  bool isCirclePoint() const;

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  void writeScriptData(ScriptPaintDevice *device) const override;

 protected:
  const Plot* plot_    { nullptr }; //!< parent plot
  Node*       node_    { nullptr }; //!< associated node
  HierObj*    hierObj_ { nullptr }; //!< parent hier obj
  int         ind_     { 0 };       //!< ind
};

//---

/*!
 * \brief Hierarchical Bubble Plot Hierarchical object
 * \ingroup Charts
 */
class CQChartsHierBubbleHierObj : public CQChartsHierBubbleNodeObj {
 public:
  using Plot     = CQChartsHierBubblePlot;
  using HierNode = CQChartsHierBubbleHierNode;
  using HierObj  = CQChartsHierBubbleHierObj;

 public:
  CQChartsHierBubbleHierObj(const Plot *plot, HierNode *hier, HierObj *hierObj,
                            const BBox &rect, const ColorInd &is);

  HierNode *hierNode() const { return hier_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isCircle() const override { return true; }

  double radius() const override { return hierNode()->radius(); }

  //---

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  //---

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  void writeScriptData(ScriptPaintDevice *device) const override;

 protected:
  HierNode* hier_ { nullptr }; //!< associated hier node
};

//---

/*!
 * \brief Hierarchical Bubble Plot
 * \ingroup Charts
 */
class CQChartsHierBubblePlot : public CQChartsHierPlot,
 public CQChartsObjShapeData<CQChartsHierBubblePlot>,
 public CQChartsObjTextData <CQChartsHierBubblePlot> {
  Q_OBJECT

  // options
  Q_PROPERTY(bool valueLabel READ isValueLabel WRITE setValueLabel)
  Q_PROPERTY(bool sorted     READ isSorted     WRITE setSorted    )

  // color
  Q_PROPERTY(bool            colorById READ isColorById WRITE setColorById)
  Q_PROPERTY(CQChartsOptReal minSize   READ minSize     WRITE setMinSize  )

  // shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // text
  CQCHARTS_TEXT_DATA_PROPERTIES

 public:
  using Node     = CQChartsHierBubbleNode;
  using Pack     = CQChartsCirclePack<Node>;
  using Nodes    = std::vector<Node*>;
  using HierNode = CQChartsHierBubbleHierNode;
  using HierObj  = CQChartsHierBubbleHierObj;
  using NodeObj  = CQChartsHierBubbleNodeObj;
  using Length   = CQChartsLength;
  using Color    = CQChartsColor;
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsHierBubblePlot(View *view, const ModelP &model);
 ~CQChartsHierBubblePlot();

  //---

  void init() override;
  void term() override;

  //---

  //! get/set is value label
  bool isValueLabel() const { return valueLabel_; }
  void setValueLabel(bool b);

  //---

  //! get/set is sorted
  bool isSorted() const { return sorted_; }
  void setSorted(bool b) { sorted_ = b; }

  //---

  //! get/set min size
  const OptReal &minSize() const { return minSize_; }
  void setMinSize(const OptReal &r);

  //---

  void setTextFontSize(double s);

  //---

  HierNode *root() const { return nodeData_.root; }

  HierNode *firstHier() const { return nodeData_.firstHier; }

  //! get/set current root
  HierNode *currentRoot() const;
  void setCurrentRoot(HierNode *r, bool update=false);

  //---

  //! get/set offset
  const Point &offset() const { return placeData_.offset; }
  void setOffset(const Point &o) { placeData_.offset = o; }

  //! get/set scale
  double scale() const { return placeData_.scale; }
  void setScale(double r) { placeData_.scale = r; }

  //---

  int colorId() const { return colorData_.colorId; }

  int numColorIds() const { return colorData_.numColorIds; }

  void initColorIds() {
    colorData_.colorId     = -1;
    colorData_.numColorIds = 0;
  }

  int nextColorId() {
    ++colorData_.colorId;

    if (colorData_.colorId >= colorData_.numColorIds)
      colorData_.numColorIds = colorData_.colorId + 1;

    return colorData_.colorId;
  }

  //---

  //! get/set is color by id
  bool isColorById() const { return colorById_; }
  void setColorById(bool b);

  //---

  int maxDepth() const { return nodeData_.maxDepth; }

  //---

  void addProperties() override;

  Range calcRange() const override;

  void clearPlotObjects() override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void postResize() override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *) const override;

  //---

  bool addMenuItems(QMenu *menu) override;

 protected:
  void initNodeObjs(HierNode *hier, HierObj *parentObj, int depth, PlotObjs &objs) const;

  void resetNodes();

  void initNodes() const;

  void replaceNodes() const;

  void placeNodes(HierNode *hier) const;

  void colorNodes(HierNode *hier) const;

  void colorNode(CQChartsHierBubbleNode *node) const;

  //---

  void loadHier() const;

  HierNode *addHierNode(HierNode *parent, const QString &name, const QModelIndex &nameInd) const;

  Node *addNode(HierNode *parent, const QString &name, double size,
                const QModelIndex &nameInd) const;

  void loadFlat() const;

  Node *addNode(const QStringList &nameStrs, double size, const QModelIndex &nameInd) const;

  void addExtraNodes(HierNode *hier) const;

  //---

  HierNode *childHierNode(HierNode *parent, const QString &name) const;
  Node *childNode(HierNode *parent, const QString &name) const;

  //---

  void initNodes(HierNode *hier) const;

  void transformNodes(HierNode *hier) const;

  void drawBounds(PaintDevice *device, HierNode *hier) const;

  //---

  void followViewExpandChanged() override;

  void modelViewExpansionChanged() override;
  void setNodeExpansion(HierNode *hierNode, const std::set<QModelIndex> &indSet);

  void resetNodeExpansion();
  void resetNodeExpansion(HierNode *hierNode);

  //---

  bool getValueSize(const ModelIndex &ind, double &size) const;

  //---

  virtual HierObj *createHierObj(HierNode *hier, HierObj *hierObj,
                                 const BBox &rect, const ColorInd &is) const;
  virtual NodeObj *createNodeObj(Node *node, HierObj *hierObj,
                                 const BBox &rect, const ColorInd &is) const;

 public slots:
  void pushSlot();
  void popSlot();
  void popTopSlot();

  void updateCurrentRoot();

 protected:
  CQChartsPlotCustomControls *createCustomControls(CQCharts *charts) override;

 private:
  struct PlaceData {
    Point  offset { 0, 0 }; //!< draw offset
    double scale  { 1.0 };  //!< draw scale
  };

  struct ColorData {
    int colorId     { -1 }; //!< current color id
    int numColorIds { 0 };  //!< num used color ids
  };

  struct NodeData {
    HierNode* root      { nullptr }; //!< root node
    HierNode* firstHier { nullptr }; //!< first hier node
    int       maxDepth  { 1 };       //!< max hier depth
    int       hierInd   { 0 };       //!< current hier ind
  };

 private:
  bool      valueLabel_       { false }; //!< draw value with name
  bool      sorted_           { false }; //!< sort nodes by value
  OptReal   minSize_;                    //!< min size
  QString   currentRootName_;            //!< current root name
  NodeData  nodeData_;                   //!< node data
  PlaceData placeData_;                  //!< place data
  ColorData colorData_;                  //!< color data
  bool      colorById_       { true };   //!< color by id
};

//---

class CQChartsHierBubblePlotCustomControls : public CQChartsHierPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsHierBubblePlotCustomControls(CQCharts *charts);

  void setPlot(CQChartsPlot *plot) override;

 private:
  void connectSlots(bool b);

 public slots:
  void updateWidgets() override;

 private:
  CQChartsHierBubblePlot* plot_ { nullptr };
};

#endif
