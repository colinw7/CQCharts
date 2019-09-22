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

  Dimension dimension() const override { return Dimension::NONE; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

/*!
 * \brief Sunburst Plot Node object
 * \ingroup Charts
 */
class CQChartsSunburstNodeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsSunburstNodeObj(const CQChartsSunburstPlot *plot, const CQChartsGeom::BBox &rect,
                          CQChartsSunburstNode *node);

  QString typeName() const override { return "node"; }

  CQChartsSunburstNode *node() const { return node_; }

  int ind() const { return ind_; }
  void setInd(int ind) { ind_ = ind; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  const CQChartsSunburstPlot* plot_ { nullptr }; //!< parent plot
  CQChartsSunburstNode*       node_ { nullptr }; //!< associated node
  int                         ind_  { 0 };       //!< ind
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
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsSunburstNode(const CQChartsSunburstPlot *plot, CQChartsSunburstHierNode *parent,
                       const QString &name="");

  virtual ~CQChartsSunburstNode() { }

  const CQChartsSunburstPlot *plot() const { return plot_; }

  CQChartsSunburstHierNode *parent() const { return parent_; }

  uint id() const { return id_; }

  const QString &name() const { return name_; }

  virtual double size() const { return size_; }
  void setSize(double size) { size_ = size; }

  virtual int depth() const { return 1; }

  virtual int numNodes() const { return 1; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &v) { ind_ = v; }

  double r () const { return r_ ; }
  double a () const { return a_ ; }
  double dr() const { return dr_; }
  double da() const { return da_; }

  int colorId() const { return colorId_; }
  virtual void setColorId(int colorId) { colorId_ = colorId; }

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &v) { color_ = v; }

  bool isFiller() const { return filler_; }
  void setFiller(bool b) { filler_ = b; }

  virtual double hierSize() const { return size(); }

  virtual QString hierName(const QChar &separator='/') const;

  virtual void setPosition(double r, double a, double dr, double da);

  void unplace() { placed_ = false; }

  bool placed() const { return placed_; }

  CQChartsSunburstNodeObj *obj() const { return obj_; }
  void setObj(CQChartsSunburstNodeObj *obj) { obj_ = obj; }

  //---

  static double normalizeAngle(double a) {
    while (a <    0.0) a += 360.0;
    while (a >= 360.0) a -= 360.0;

    return a;
  }

  bool pointInside(double x, double y);

  virtual QColor interpColor(const CQChartsSunburstPlot *plot, const CQChartsColor &c,
                             const ColorInd &colorInd, int n) const;

 protected:
  const CQChartsSunburstPlot* plot_    { nullptr }; //!< parent plot
  CQChartsSunburstHierNode*   parent_  { nullptr }; //!< parent hier node
  uint                        id_      { 0 };       //!< node id
  QString                     name_;                //!< node name
  double                      size_    { 0.0 };     //!< node size
  QModelIndex                 ind_;                 //!< node index
  double                      r_       { 0.0 };     //!< node radius
  double                      a_       { 0.0 };     //!< node angle
  double                      dr_      { 0.0 };     //!< node delta radius
  double                      da_      { 0.0 };     //!< node delta angle
  int                         colorId_ { -1 };      //!< node color index
  CQChartsColor               color_   { };         //!< node explicit color
  bool                        filler_  { false };   //!< is filler
  bool                        placed_  { false };   //!< is place
  CQChartsSunburstNodeObj*    obj_     { nullptr }; //!< associated object
};

//---

struct CQChartsSunburstNodeNameCmp {
  // sort reverse alphabetic no case
  bool operator()(const CQChartsSunburstNode *n1, const CQChartsSunburstNode *n2);
};

struct CQChartsSunburstNodeSizeCmp {
  bool operator()(const CQChartsSunburstNode *n1, const CQChartsSunburstNode *n2) {
    return n1->size() < n2->size();
  }
};

struct CQChartsSunburstNodeCountCmp {
  bool operator()(const CQChartsSunburstNode *n1, const CQChartsSunburstNode *n2) {
    return n1->numNodes() < n2->numNodes();
  }
};

//---

/*!
 * \brief Sunburst Plot Hierarchical Node
 * \ingroup Charts
 */
class CQChartsSunburstHierNode : public CQChartsSunburstNode {
 public:
  enum class Order {
    SIZE,
    COUNT
  };

 public:
  using Nodes    = std::vector<CQChartsSunburstNode*>;
  using Children = std::vector<CQChartsSunburstHierNode*>;

 public:
  CQChartsSunburstHierNode(const CQChartsSunburstPlot *plot,
                           CQChartsSunburstHierNode *parent=nullptr,
                           const QString &name="");

 ~CQChartsSunburstHierNode();

  double hierSize() const override;

  int depth() const override;

  int numNodes() const override;

  bool hasNodes() const { return ! nodes_.empty(); }

  const Nodes &getNodes() const { return nodes_; }

  bool hasChildren() const { return ! children_.empty(); }

  const Children &getChildren() const { return children_; }

  void unplace();

  void unplaceNodes();

  void packNodes(CQChartsSunburstHierNode *root, double ri, double ro,
                 double dr, double a, double da, const Order &order, bool sort);

  void packSubNodes(CQChartsSunburstHierNode *root, double ri,
                    double dr, double a, double da, const Order &order, bool sort);

  void addNode(CQChartsSunburstNode *node);

  void removeNode(CQChartsSunburstNode *node);

  QColor interpColor(const CQChartsSunburstPlot *plot, const CQChartsColor &c,
                     const ColorInd &colorInd, int n) const override;

 private:
  Nodes    nodes_;    //!< child nodes
  Children children_; //!< child hier nodes
};

//---

/*!
 * \brief Sunburst Plot Root Node
 * \ingroup Charts
 */
class CQChartsSunburstRootNode : public CQChartsSunburstHierNode {
 public:
  CQChartsSunburstRootNode(const CQChartsSunburstPlot *plot, const QString &name="") :
   CQChartsSunburstHierNode(plot, 0, name), sort_(true), order_(Order::SIZE) {
  }

  bool sort() const { return sort_; }
  void setSort(bool sort) { sort_ = sort; }

  Order order() const { return order_; }
  void setOrder(Order order) { order_ = order; }

  void packNodes(double ri=0.5, double ro=1.0, double dr=0.0, double a=0.0, double da=360.0) {
    CQChartsSunburstHierNode::packNodes(this, ri, ro, dr, a, da, order(), sort());
  }

 private:
  bool  sort_  { true };        //!< is sorted
  Order order_ { Order::SIZE }; //!< sort order
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
  Q_PROPERTY(double innerRadius READ innerRadius WRITE setInnerRadius)
  Q_PROPERTY(double outerRadius READ outerRadius WRITE setOuterRadius)
  Q_PROPERTY(double startAngle  READ startAngle  WRITE setStartAngle )
  Q_PROPERTY(bool   multiRoot   READ isMultiRoot WRITE setMultiRoot  )

  // color
  Q_PROPERTY(bool colorById READ isColorById WRITE setColorById)

  // shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // text
  CQCHARTS_TEXT_DATA_PROPERTIES

 public:
  using RootNodes = std::vector<CQChartsSunburstRootNode*>;

 public:
  CQChartsSunburstPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsSunburstPlot();

  //---

  double innerRadius() const { return innerRadius_; }
  void setInnerRadius(double r);

  double outerRadius() const { return outerRadius_; }
  void setOuterRadius(double r);

  double startAngle() const { return startAngle_; }
  void setStartAngle(double a);

  //---

  const RootNodes &roots() const { return roots_; }

  bool isMultiRoot() const { return multiRoot_; }
  void setMultiRoot(bool b);

  //---

  void setTextFontSize(double s);

  //---

  CQChartsSunburstHierNode *currentRoot() const;
  void setCurrentRoot(CQChartsSunburstHierNode *r, bool update=true);

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

  CQChartsGeom::Range calcRange() const override;

  void clearPlotObjects() override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void postResize() override;

  //---

  void drawNode(CQChartsPaintDevice *device, CQChartsSunburstNodeObj *nodeObj,
                CQChartsSunburstNode *node) const;

  //---

  bool addMenuItems(QMenu *menu) override;

 private:
  void resetRoots();

  void initRoots();

  void replaceRoots() const;

  void colorNodes(CQChartsSunburstHierNode *hier) const;

  void colorNode(CQChartsSunburstNode *node) const;

  //---

  void loadHier(CQChartsSunburstHierNode *hier) const;

  CQChartsSunburstHierNode *addHierNode(CQChartsSunburstHierNode *hier, const QString &name,
                                        const QModelIndex &nameInd) const;

  CQChartsSunburstNode *addNode(CQChartsSunburstHierNode *hier, const QString &name, double size,
                                const QModelIndex &nameInd, const QModelIndex &valueInd) const;

  void loadFlat(CQChartsSunburstHierNode *hier) const;

  CQChartsSunburstNode *addNode(CQChartsSunburstHierNode *root, const QStringList &nameStrs,
                                double size, const QModelIndex &nameInd,
                                const QModelIndex &valueInd) const;

  void addExtraNodes(CQChartsSunburstHierNode *hier) const;

  //---

  CQChartsSunburstRootNode *createRootNode(const QString &name="");

  CQChartsSunburstRootNode *rootNode(const QString &name) const;

  CQChartsSunburstHierNode *childHierNode(CQChartsSunburstHierNode *parent,
                                          const QString &name) const;

  CQChartsSunburstNode *childNode(CQChartsSunburstHierNode *parent,
                                  const QString &name) const;

  //---

  void addPlotObjs(CQChartsSunburstHierNode *parent, PlotObjs &objs, const ColorInd &ir) const;

  void addPlotObj(CQChartsSunburstNode *node, PlotObjs &objs, const ColorInd &ir) const;

  //---

  void drawNodes(CQChartsPaintDevice *device, CQChartsSunburstHierNode *hier) const;

 public slots:
  void pushSlot();
  void popSlot();
  void popTopSlot();

 private:
  double    innerRadius_ { 0.5 };   //!< inner radius
  double    outerRadius_ { 1.0 };   //!< outer radius
  double    startAngle_  { -90 };   //!< start angle
  bool      multiRoot_   { false }; //!< has multiple roots
  RootNodes roots_;                 //!< root nodes
  QString   currentRootName_;       //!< current root name
  int       colorId_     { -1 };    //!< current color id
  int       numColorIds_ { 0 };     //!< num used color ids
  bool      colorById_   { true };  //!< color by id
};

#endif
