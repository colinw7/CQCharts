#ifndef CQChartsSunburstPlot_H
#define CQChartsSunburstPlot_H

#include <CQChartsHierPlot.h>
#include <CQChartsPlotObj.h>
#include <QModelIndex>

class CQChartsSunburstPlot;
class CQChartsSunburstRootNode;
class CQChartsSunburstHierNode;
class CQChartsSunburstNode;

//---

/*!
 * \brief Sunburst Plot Type
 * \ingroup Charts
 */
class CQChartsSunburstPlotType : public CQChartsHierPlotType {
 public:
  CQChartsSunburstPlotType();

  QString name() const override { return "sunburst"; }
  QString desc() const override { return "Sunburst"; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  Plot *create(View *view, const ModelP &model) const override;
};

//---

/*!
 * \brief Sunburst Plot Node object
 * \ingroup Charts
 */
class CQChartsSunburstNodeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot = CQChartsSunburstPlot;
  using Node = CQChartsSunburstNode;

 public:
  CQChartsSunburstNodeObj(const Plot *plot, const BBox &rect, Node *node);

  QString typeName() const override { return "node"; }

  Node *node() const { return node_; }

  int ind() const { return ind_; }
  void setInd(int ind) { ind_ = ind; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) const override;

 private:
  const Plot* plot_ { nullptr }; //!< parent plot
  Node*       node_ { nullptr }; //!< associated node
  int         ind_  { 0 };       //!< ind
};

//---

/*!
 * \brief Sunburst Plot Node
 * \ingroup Charts
 */
class CQChartsSunburstNode {
 private:
  static uint nextId() {
    static int lastId = 0;

    return ++lastId;
  }

 public:
  enum SortType {
    NONE,
    SIZE,
    COUNT,
    NAME
  };

  using Plot     = CQChartsSunburstPlot;
  using HierNode = CQChartsSunburstHierNode;
  using NodeObj  = CQChartsSunburstNodeObj;
  using Angle    = CQChartsAngle;
  using Color    = CQChartsColor;
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsSunburstNode(const Plot *plot, HierNode *parent, const QString &name="");

  virtual ~CQChartsSunburstNode() { }

  const Plot *plot() const { return plot_; }

  HierNode *parent() const { return parent_; }

  uint id() const { return id_; }

  const QString &name() const { return name_; }

  virtual double size() const { return size_; }
  void setSize(double size) { size_ = size; }

  virtual int depth() const { return 1; }

  virtual int numNodes() const { return 1; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &v) { ind_ = v; }

  double r () const { return r_ ; }
  Angle  a () const { return a_ ; }
  double dr() const { return dr_; }
  Angle  da() const { return da_; }

  int colorId() const { return colorId_; }
  virtual void setColorId(int colorId) { colorId_ = colorId; }

  const Color &color() const { return color_; }
  void setColor(const Color &v) { color_ = v; }

  bool isFiller() const { return filler_; }
  void setFiller(bool b) { filler_ = b; }

  virtual double hierSize() const { return size(); }

  virtual QString hierName(const QChar &separator='/') const;

  virtual void setPosition(double r, const Angle &a, double dr, const Angle &da);

  void unplace() { placed_ = false; }

  bool placed() const { return placed_; }

  NodeObj *obj() const { return obj_; }
  void setObj(NodeObj *obj) { obj_ = obj; }

  //---

  bool pointInside(double x, double y);

  virtual QColor interpColor(const Plot *plot, const Color &c,
                             const ColorInd &colorInd, int n) const;

 protected:
  const Plot* plot_    { nullptr }; //!< parent plot
  HierNode*   parent_  { nullptr }; //!< parent hier node
  uint        id_      { 0 };       //!< node id
  QString     name_;                //!< node name
  double      size_    { 0.0 };     //!< node size
  QModelIndex ind_;                 //!< node index
  double      r_       { 0.0 };     //!< node radius
  Angle       a_;                   //!< node angle
  double      dr_      { 0.0 };     //!< node delta radius
  Angle       da_;                  //!< node delta angle
  int         colorId_ { -1 };      //!< node color index
  Color       color_   { };         //!< node explicit color
  bool        filler_  { false };   //!< is filler
  bool        placed_  { false };   //!< is place
  NodeObj*    obj_     { nullptr }; //!< associated object
};

//---

//! sort reverse alphabetic no case
struct CQChartsSunburstNodeNameCmp {
  using Node = CQChartsSunburstNode;

  bool operator()(const Node *n1, const Node *n2);
};

//! sort size
struct CQChartsSunburstNodeSizeCmp {
  using Node = CQChartsSunburstNode;

  bool operator()(const Node *n1, const Node *n2);
};

//! sort node count
struct CQChartsSunburstNodeCountCmp {
  using Node = CQChartsSunburstNode;

  bool operator()(const Node *n1, const Node *n2);
};

//---

/*!
 * \brief Sunburst Plot Hierarchical Node
 * \ingroup Charts
 */
class CQChartsSunburstHierNode : public CQChartsSunburstNode {
 public:
  using Plot     = CQChartsSunburstPlot;
  using Node     = CQChartsSunburstNode;
  using Nodes    = std::vector<Node *>;
  using HierNode = CQChartsSunburstHierNode;
  using Children = std::vector<HierNode *>;

 public:
  CQChartsSunburstHierNode(const Plot *plot, HierNode *parent=nullptr, const QString &name="");

 ~CQChartsSunburstHierNode();

  //---

  bool isExpanded() const { return expanded_; }
  void setExpanded(bool b) { expanded_ = b; }

  bool isHierExpanded() const;

  //---

  double hierSize() const override;

  //---

  int depth() const override;

  int numNodes() const override;

  bool hasNodes() const { return ! nodes_.empty(); }

  const Nodes &getNodes() const { return nodes_; }

  bool hasChildren() const { return ! children_.empty(); }

  const Children &getChildren() const { return children_; }

  //---

  void unplace();

  void unplaceNodes();

  void packNodes(HierNode *root, double ri, double ro, double dr, const Angle &a,
                 const Angle &da, const SortType &sortType);

  void packSubNodes(HierNode *root, double ri, double dr, const Angle &a,
                    const Angle &da, const SortType &sortType);

  void addNode(Node *node);

  void removeNode(Node *node);

  QColor interpColor(const Plot *plot, const Color &c,
                     const ColorInd &colorInd, int n) const override;

 private:
  Nodes    nodes_;             //!< child nodes
  Children children_;          //!< child hier nodes
  bool     expanded_ { true }; //!< is expanded
};

//---

/*!
 * \brief Sunburst Plot Root Node
 * \ingroup Charts
 */
class CQChartsSunburstRootNode : public CQChartsSunburstHierNode {
 public:
  CQChartsSunburstRootNode(const Plot *plot, const QString &name="") :
   CQChartsSunburstHierNode(plot, 0, name) {
  }

  void packNodes(double ri=0.5, double ro=1.0, double dr=0.0,
                 const Angle &a=Angle(0.0), const Angle &da=Angle(360.0),
                 const SortType &sortType=SortType::NAME) {
    CQChartsSunburstHierNode::packNodes(this, ri, ro, dr, a, da, sortType);
  }
};

//---

/*!
 * \brief Sunburst Plot
 * \ingroup Charts
 */
class CQChartsSunburstPlot : public CQChartsHierPlot,
 public CQChartsObjShapeData<CQChartsSunburstPlot>,
 public CQChartsObjTextData <CQChartsSunburstPlot> {
  Q_OBJECT

  // options
  Q_PROPERTY(double        innerRadius READ innerRadius WRITE setInnerRadius)
  Q_PROPERTY(double        outerRadius READ outerRadius WRITE setOuterRadius)
  Q_PROPERTY(CQChartsAngle startAngle  READ startAngle  WRITE setStartAngle )
  Q_PROPERTY(bool          multiRoot   READ isMultiRoot WRITE setMultiRoot  )
  Q_PROPERTY(SortType      sortType    READ sortType    WRITE setSortType   )

  // color
  Q_PROPERTY(bool colorById READ isColorById WRITE setColorById)

  // shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // text
  Q_PROPERTY(bool clipText READ isClipText WRITE setClipText)

  CQCHARTS_TEXT_DATA_PROPERTIES

  Q_ENUMS(SortType)

 public:
  enum SortType {
    NONE,
    SIZE,
    COUNT,
    NAME
  };

  using RootNode  = CQChartsSunburstRootNode;
  using RootNodes = std::vector<RootNode*>;
  using HierNode  = CQChartsSunburstHierNode;
  using Node      = CQChartsSunburstNode;
  using NodeObj   = CQChartsSunburstNodeObj;

 public:
  CQChartsSunburstPlot(View *view, const ModelP &model);
 ~CQChartsSunburstPlot();

  //---

  void init() override;
  void term() override;

  //---

  double innerRadius() const { return innerRadius_; }
  void setInnerRadius(double r);

  double outerRadius() const { return outerRadius_; }
  void setOuterRadius(double r);

  const Angle &startAngle() const { return startAngle_; }
  void setStartAngle(const Angle &a);

  //---

  bool isMultiRoot() const { return multiRoot_; }
  void setMultiRoot(bool b);

  //---

  const SortType &sortType() const { return sortType_; }
  void setSortType(const SortType &t);

  //---

  bool isClipText() const { return clipText_; }
  void setClipText(bool b);

  //---

  const RootNodes &roots() const { return roots_; }

  bool isRoot(const HierNode *node) const;

  //---

  void setTextFontSize(double s);

  //---

  HierNode *currentRoot() const;
  void setCurrentRoot(HierNode *r, bool update=true);

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

  bool isColorById() const { return colorById_; }
  void setColorById(bool b);

  //---

  void addProperties() override;

  Range calcRange() const override;

  void clearPlotObjects() override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void postResize() override;

  //---

  void drawNode(PaintDevice *device, NodeObj *nodeObj, Node *node) const;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  virtual NodeObj *createNodeObj(const BBox &rect, Node *node) const;

 private:
  void resetRoots();

  void initRoots();

  void replaceRoots() const;

  void colorNodes(HierNode *hier) const;

  void colorNode(Node *node) const;

  //---

  void loadHier(HierNode *hier) const;

  HierNode *addHierNode(HierNode *hier, const QString &name, const QModelIndex &nameInd) const;

  Node *hierAddNode(HierNode *hier, const QString &name, double size, const QModelIndex &nameInd,
                    const QModelIndex &valueInd) const;

  void loadFlat(HierNode *hier) const;

  Node *flatAddNode(HierNode *root, const QStringList &nameStrs, double size,
                    const QModelIndex &nameInd, const QModelIndex &valueInd) const;

  void addExtraNodes(HierNode *hier) const;

  //---

  RootNode *createRootNode(const QString &name="");

  RootNode *rootNode(const QString &name) const;

  HierNode *childHierNode(HierNode *parent, const QString &name) const;

  Node *childNode(HierNode *parent, const QString &name) const;

  //---

  void addPlotObjs(HierNode *parent, PlotObjs &objs, const ColorInd &ir) const;

  void addPlotObj(Node *node, PlotObjs &objs, const ColorInd &ir) const;

  //---

//void drawNodes(PaintDevice *device, HierNode *hier) const;

  //---

  void followViewExpandChanged() override;

  void modelViewExpansionChanged() override;
  void setNodeExpansion(HierNode *hierNode, const std::set<QModelIndex> &indSet);

  void resetNodeExpansion();
  void resetNodeExpansion(HierNode *hierNode);

  //---

  bool getValueSize(const ModelIndex &ind, double &size) const;

 public slots:
  void pushSlot();
  void popSlot();
  void popTopSlot();

 private:
  double    innerRadius_      { 0.5 };            //!< inner radius
  double    outerRadius_      { 1.0 };            //!< outer radius
  Angle     startAngle_       { -90 };            //!< start angle
  bool      multiRoot_        { false };          //!< has multiple roots
  SortType  sortType_         { SortType::NAME }; //!< sort type
  bool      clipText_         { true };           //!< clip text
  RootNodes roots_;                               //!< root nodes
  QString   currentRootName_;                     //!< current root name
  int       colorId_          { -1 };             //!< current color id
  int       numColorIds_      { 0 };              //!< num used color ids
  bool      colorById_        { true };           //!< color by id
};

#endif
