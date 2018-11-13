#ifndef CQChartsBubblePlot_H
#define CQChartsBubblePlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsCirclePack.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsData.h>
#include <QModelIndex>

//---

class CQChartsBubblePlotType : public CQChartsGroupPlotType {
 public:
  CQChartsBubblePlotType();

  QString name() const override { return "bubble"; }
  QString desc() const override { return "Bubble"; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  QString description() const override;

  bool customXRange() const override { return false; }
  bool customYRange() const override { return false; }

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsBubblePlot;
class CQChartsBubbleHierNode;

class CQChartsBubbleNode : public CQChartsCircleNode {
 protected:
  static uint nextId() {
    static int lastId = 0;

    return ++lastId;
  }

 public:
  CQChartsBubbleNode(CQChartsBubblePlot *plot, CQChartsBubbleHierNode *parent,
                     const QString &name, double size, const QModelIndex &ind);

  virtual ~CQChartsBubbleNode();

  void initRadius();

  CQChartsBubblePlot *plot() const { return plot_; }

  CQChartsBubbleHierNode *parent() const { return parent_; }

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

  friend bool operator<(const CQChartsBubbleNode &n1, const CQChartsBubbleNode &n2) {
    return n1.r_ < n2.r_;
  }

  virtual QColor interpColor(CQChartsBubblePlot *plot, int n) const;

 protected:
  CQChartsBubblePlot*     plot_    { nullptr }; // parent plot
  CQChartsBubbleHierNode *parent_  { nullptr }; // parent hier node
  uint                    id_      { 0 };       // node id
  QString                 name_;                // node name
  double                  size_    { 0.0 };     // node size
  int                     colorId_ { -1 };      // node color index
  CQChartsColor           color_;               // node explicit color
  QModelIndex             ind_;                 // data model index
  int                     depth_   { 0 };       // node depth
  bool                    filler_  { false };   // is filler
  bool                    placed_  { false };   // is placed
};

//---

struct CQChartsBubbleNodeCmp {
  bool operator()(const CQChartsBubbleNode *n1, const CQChartsBubbleNode *n2) {
    return (*n1) < (*n2);
  }
};

//---

class CQChartsBubbleHierNode : public CQChartsBubbleNode {
 public:
  using Nodes    = std::vector<CQChartsBubbleNode*>;
  using Children = std::vector<CQChartsBubbleHierNode*>;
  using Pack     = CQChartsCirclePack<CQChartsBubbleNode>;

 public:
  CQChartsBubbleHierNode(CQChartsBubblePlot *plot, CQChartsBubbleHierNode *parent,
                         const QString &name, const QModelIndex &ind=QModelIndex());

 ~CQChartsBubbleHierNode();

  int hierInd() const { return hierInd_; }
  void setHierInd(int i) { hierInd_ = i; }

  //---

  double hierSize() const override;

  //---

  bool hasNodes() const { return ! nodes_.empty(); }

  const Nodes &getNodes() const { return nodes_; }
  Nodes &getNodes() { return nodes_; }

  const Pack &pack() const { return pack_; }
  Pack &pack() { return pack_; }

  bool hasChildren() const { return ! children_.empty(); }

  const Children &getChildren() const { return children_; }

  //---

  void packNodes();

  void addNode(CQChartsBubbleNode *node);

  void removeNode(CQChartsBubbleNode *node);

  void setPosition(double x, double y) override;

  QColor interpColor(CQChartsBubblePlot *plot, int n) const override;

 protected:
  Nodes    nodes_;          // child nodes
  Pack     pack_;           // circle pack
  Children children_;       // child hier nodes
  int      hierInd_ { -1 }; // hier index
};

//---

class CQChartsBubbleHierObj;

class CQChartsBubbleObj : public CQChartsPlotObj {
 public:
  CQChartsBubbleObj(CQChartsBubblePlot *plot, CQChartsBubbleNode *node,
                    CQChartsBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                    int i, int n);

  CQChartsBubbleNode *node() const { return node_; }

  CQChartsBubbleHierObj *parent() const { return hierObj_; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

 protected:
  CQChartsBubblePlot*    plot_    { nullptr }; // parent plot
  CQChartsBubbleNode*    node_    { nullptr }; // associated node
  CQChartsBubbleHierObj* hierObj_ { nullptr }; // parent hier obj
  int                    i_       { 0 };       // data index
  int                    n_       { 0 };       // data count
};

//---

class CQChartsBubbleHierObj : public CQChartsBubbleObj {
 public:
  CQChartsBubbleHierObj(CQChartsBubblePlot *plot, CQChartsBubbleHierNode *hier,
                        CQChartsBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                        int i, int n);

  CQChartsBubbleHierNode *hierNode() const { return hier_; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

 protected:
  CQChartsBubbleHierNode* hier_ { nullptr }; // associated hier node
};

//---

class CQChartsBubblePlot : public CQChartsGroupPlot,
 public CQChartsObjShapeData<CQChartsBubblePlot>,
 public CQChartsObjTextData <CQChartsBubblePlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)

  // options
  Q_PROPERTY(bool valueLabel READ isValueLabel WRITE setValueLabel)
  Q_PROPERTY(bool sorted     READ isSorted     WRITE setSorted    )

  // shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // text
  CQCHARTS_TEXT_DATA_PROPERTIES

 public:
  using Pack  = CQChartsCirclePack<CQChartsBubbleNode>;
  using Nodes = std::vector<CQChartsBubbleNode*>;

 public:
  CQChartsBubblePlot(CQChartsView *view, const ModelP &model);

 ~CQChartsBubblePlot();

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c);

  //---

  bool isValueLabel() const { return valueLabel_; }
  void setValueLabel(bool b);

  //---

  bool isSorted() const { return sorted_; }
  void setSorted(bool b) { sorted_ = b; }

  //---

  void setTextFontSize(double s);

  //---

  CQChartsBubbleHierNode *root() const { return root_; }

  //CQChartsBubbleHierNode *firstHier() const { return firstHier_; }

  CQChartsBubbleHierNode *currentRoot() const;
  void setCurrentRoot(CQChartsBubbleHierNode *r, bool update=false);

  //---

  const CQChartsGeom::Point &offset() const { return offset_; }
  void setOffset(const CQChartsGeom::Point &o) { offset_ = o; }

  double scale() const { return scale_; }
  void setScale(double r) { scale_ = r; }

  //---

  int colorId() const { return colorId_; }

  int numColorIds() const { return numColorIds_; }

  void initColorIds() {
    colorId_     = -1;
    numColorIds_ = 0;
  }

  int nextColorId() {
    ++colorId_;

    if (colorId_ >= numColorIds_)
      numColorIds_ = colorId_ + 1;

    return colorId_;
  }

  //---

  int maxDepth() const { return maxDepth_; }

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() override;

  void updateObjs() override;

  bool createObjs() override;

  //---

  void handleResize() override;

  //---

  bool hasForeground() const override;

  void drawForeground(QPainter *painter) override;

 private:
  void initNodeObjs(CQChartsBubbleHierNode *hier,
                    CQChartsBubbleHierObj *parentObj, int depth);

  void resetNodes();

  void initNodes();

  void replaceNodes();

  void placeNodes(CQChartsBubbleHierNode *hier);

  void colorNodes(CQChartsBubbleHierNode *hier);

  void colorNode(CQChartsBubbleNode *node);

  //---

  void loadModel();

  CQChartsBubbleHierNode *groupHierNode(CQChartsBubbleHierNode *hier, int groupInd);

  CQChartsBubbleHierNode *addHierNode(CQChartsBubbleHierNode *hier, const QString &name,
                                      const QModelIndex &nameInd);

  CQChartsBubbleNode *addNode(CQChartsBubbleHierNode *hierNode, const QString &name,
                              double size, const QModelIndex &nameInd);

  //---

  void initNodes(CQChartsBubbleHierNode *hier);

  void transformNodes(CQChartsBubbleHierNode *hier);

  void drawBounds(QPainter *painter, CQChartsBubbleHierNode *hier);

 private:
  using GroupHierNodes = std::map<int,CQChartsBubbleHierNode *>;

  CQChartsBubbleHierNode* root_        { nullptr }; // root node
  CQChartsColumn          nameColumn_;              // name column
  CQChartsColumn          valueColumn_;             // value column
  bool                    valueLabel_  { false };   // draw value with name
  bool                    sorted_      { false };   // sort nodes by value
  CQChartsGeom::Point     offset_      { 0, 0 };    // draw offset
  double                  scale_       { 1.0 };     // draw scale
  int                     colorId_     { -1 };      // current color id
  int                     numColorIds_ { 0 };       // num used color ids
  int                     maxDepth_    { 1 };       // max hier depth
  int                     hierInd_     { 0 };       // current hier ind
  GroupHierNodes          groupHierNodes_;          // hier group nodes
};

#endif
