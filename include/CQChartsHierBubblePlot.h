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

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
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
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsHierBubbleNode(const CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *parent,
                         const QString &name, double size, const QModelIndex &ind);

  virtual ~CQChartsHierBubbleNode();

  void initRadius();

  const CQChartsHierBubblePlot *plot() const { return plot_; }

  CQChartsHierBubbleHierNode *parent() const { return parent_; }

  virtual uint id() const { return id_; }

  virtual const QString &name() const { return name_; }

  virtual double size() const { return size_; }
  virtual void setSize(double s) { size_ = s; }

  virtual double radius() const override { return r_; }
  virtual void setRadius(double r) { r_ = r; }

  double x() const override { return CQChartsCircleNode::x(); }
  void setX(double x) override { CQChartsCircleNode::setX(x); }

  double y() const override { return CQChartsCircleNode::y(); }
  void setY(double y) override { CQChartsCircleNode::setY(y); }

  virtual int colorId() const { return colorId_; }
  virtual void setColorId(int id) { colorId_ = id; }

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &v) { color_ = v; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  virtual int depth() const { return depth_; }
  virtual void setDepth(int i) { depth_ = i; }

  bool isFiller() const { return filler_; }
  void setFiller(bool b) { filler_ = b; }

  virtual double hierSize() const { return size(); }

  virtual QString hierName() const;

  virtual void resetPosition() {
    //CQChartsCircleNode::setPosition(0.0, 0.0);

    //placed_ = false;
  }

  void setPosition(double x, double y) override;

  virtual bool placed() const { return placed_; }

  friend bool operator<(const CQChartsHierBubbleNode &n1, const CQChartsHierBubbleNode &n2) {
    return n1.r_ < n2.r_;
  }

  virtual QColor interpColor(const CQChartsHierBubblePlot *plot, const CQChartsColor &c,
                             const ColorInd &colorInd, int n) const;

 protected:
  const CQChartsHierBubblePlot* plot_    { nullptr }; //!< parent plot
  CQChartsHierBubbleHierNode*   parent_  { nullptr }; //!< parent hier node
  uint                          id_      { 0 };       //!< node id
  QString                       name_;                //!< node name
  double                        size_    { 0.0 };     //!< node size
  int                           colorId_ { -1 };      //!< node color index
  CQChartsColor                 color_;               //!< node explicit color
  QModelIndex                   ind_;                 //!< node model index
  int                           depth_   { 0 };       //!< node depth
  bool                          filler_  { false };   //!< is filler
  bool                          placed_  { false };   //!< is placed
};

//---

struct CQChartsHierBubbleNodeCmp {
  bool operator()(const CQChartsHierBubbleNode *n1, const CQChartsHierBubbleNode *n2) {
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
  using Nodes    = std::vector<CQChartsHierBubbleNode*>;
  using Children = std::vector<CQChartsHierBubbleHierNode*>;
  using Pack     = CQChartsCirclePack<CQChartsHierBubbleNode>;

 public:
  CQChartsHierBubbleHierNode(const CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *parent,
                             const QString &name, const QModelIndex &ind=QModelIndex());

 ~CQChartsHierBubbleHierNode();

  int hierInd() const { return hierInd_; }
  void setHierInd(int i) { hierInd_ = i; }

  //---

  double hierSize() const override;

  //---

  bool hasNodes() const { return ! nodes_.empty(); }

  const Nodes &getNodes() const { return nodes_; }

  bool hasChildren() const { return ! children_.empty(); }

  const Children &getChildren() const { return children_; }

  //---

  void packNodes();

  void addNode(CQChartsHierBubbleNode *node);

  void removeNode(CQChartsHierBubbleNode *node);

  void setPosition(double x, double y) override;

  QColor interpColor(const CQChartsHierBubblePlot *plot, const CQChartsColor &c,
                     const ColorInd &colorInd, int n) const override;

 protected:
  Nodes    nodes_;          //!< child nodes
  Pack     pack_;           //!< circle pack
  Children children_;       //!< child hier nodes
  int      hierInd_ { -1 }; //!< hier index
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
  CQChartsHierBubbleNodeObj(const CQChartsHierBubblePlot *plot, CQChartsHierBubbleNode *node,
                            CQChartsHierBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                            const ColorInd &is);

  QString typeName() const override { return "bubble"; }

  CQChartsHierBubbleNode *node() const { return node_; }

  CQChartsHierBubbleHierObj *parent() const { return hierObj_; }

  int ind() const { return ind_; }
  void setInd(int ind) { ind_ = ind; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 protected:
  const CQChartsHierBubblePlot* plot_    { nullptr }; //!< parent plot
  CQChartsHierBubbleNode*       node_    { nullptr }; //!< associated node
  CQChartsHierBubbleHierObj*    hierObj_ { nullptr }; //!< parent hier obj
  int                           ind_     { 0 };       //!< ind
};

//---

/*!
 * \brief Hierarchical Bubble Plot Hierarchical object
 * \ingroup Charts
 */
class CQChartsHierBubbleHierObj : public CQChartsHierBubbleNodeObj {
 public:
  CQChartsHierBubbleHierObj(const CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *hier,
                            CQChartsHierBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                            const ColorInd &is);

  CQChartsHierBubbleHierNode *hierNode() const { return hier_; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 protected:
  CQChartsHierBubbleHierNode* hier_ { nullptr }; //!< associated hier node
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
  Q_PROPERTY(bool colorById READ isColorById WRITE setColorById)

  // shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // text
  CQCHARTS_TEXT_DATA_PROPERTIES

 public:
  using Nodes = std::vector<CQChartsHierBubbleNode*>;

 public:
  CQChartsHierBubblePlot(CQChartsView *view, const ModelP &model);

 ~CQChartsHierBubblePlot();

  //---

  bool isValueLabel() const { return valueLabel_; }
  void setValueLabel(bool b);

  //---

  bool isSorted() const { return sorted_; }
  void setSorted(bool b) { sorted_ = b; }

  //---

  void setTextFontSize(double s);

  //---

  CQChartsHierBubbleHierNode *root() const { return nodeData_.root; }

  CQChartsHierBubbleHierNode *firstHier() const { return nodeData_.firstHier; }

  CQChartsHierBubbleHierNode *currentRoot() const;
  void setCurrentRoot(CQChartsHierBubbleHierNode *r, bool update=false);

  //---

  const CQChartsGeom::Point &offset() const { return placeData_.offset; }
  void setOffset(const CQChartsGeom::Point &o) { placeData_.offset = o; }

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

  bool isColorById() const { return colorById_; }
  void setColorById(bool b);

  //---

  int maxDepth() const { return nodeData_.maxDepth; }

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  void clearPlotObjects() override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void postResize() override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(CQChartsPaintDevice *) const override;

  //---

  bool addMenuItems(QMenu *menu) override;

 private:
  void initNodeObjs(CQChartsHierBubbleHierNode *hier, CQChartsHierBubbleHierObj *parentObj,
                    int depth, PlotObjs &objs) const;

  void resetNodes();

  void initNodes() const;

  void replaceNodes() const;

  void placeNodes(CQChartsHierBubbleHierNode *hier) const;

  void colorNodes(CQChartsHierBubbleHierNode *hier) const;

  void colorNode(CQChartsHierBubbleNode *node) const;

  //---

  void loadHier() const;

  CQChartsHierBubbleHierNode *addHierNode(CQChartsHierBubbleHierNode *parent, const QString &name,
                                          const QModelIndex &nameInd) const;

  CQChartsHierBubbleNode *addNode(CQChartsHierBubbleHierNode *parent, const QString &name,
                                  double size, const QModelIndex &nameInd) const;

  void loadFlat() const;

  CQChartsHierBubbleNode *addNode(const QStringList &nameStrs, double size,
                                  const QModelIndex &nameInd) const;

  void addExtraNodes(CQChartsHierBubbleHierNode *hier) const;

  //---

  CQChartsHierBubbleHierNode *childHierNode(CQChartsHierBubbleHierNode *parent,
                                            const QString &name) const;
  CQChartsHierBubbleNode *childNode(CQChartsHierBubbleHierNode *parent,
                                    const QString &name) const;

  //---

  void initNodes(CQChartsHierBubbleHierNode *hier) const;

  void transformNodes(CQChartsHierBubbleHierNode *hier) const;

  void drawBounds(CQChartsPaintDevice *device, CQChartsHierBubbleHierNode *hier) const;

 public slots:
  void pushSlot();
  void popSlot();
  void popTopSlot();

  void updateCurrentRoot();

 private:
  using HierNode = CQChartsHierBubbleHierNode;

  struct PlaceData {
    CQChartsGeom::Point offset { 0, 0 }; //!< draw offset
    double              scale  { 1.0 };  //!< draw scale
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
  bool      valueLabel_      { false }; //!< draw value with name
  bool      sorted_          { false }; //!< sort nodes by value
  QString   currentRootName_;           //!< current root name
  NodeData  nodeData_;                  //!< node data
  PlaceData placeData_;                 //!< place data
  ColorData colorData_;                 //!< color data
  bool      colorById_       { true };  //!< color by id
};

#endif
