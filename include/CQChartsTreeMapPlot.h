#ifndef CQChartsTreeMapPlot_H
#define CQChartsTreeMapPlot_H

#include <CQChartsHierPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsData.h>
#include <QModelIndex>

//---

/*!
 * \brief Tree Map Plot Type
 */
class CQChartsTreeMapPlotType : public CQChartsHierPlotType {
 public:
  CQChartsTreeMapPlotType();

  QString name() const override { return "treemap"; }
  QString desc() const override { return "TreeMap"; }

  void addParameters() override;

  bool customXRange() const override { return false; }
  bool customYRange() const override { return false; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  bool hasAxes() const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsTreeMapPlot;
class CQChartsTreeMapHierNode;

//! \brief Tree Map Node
class CQChartsTreeMapNode {
 private:
  static uint nextId() {
    static int lastId = 0;

    return ++lastId;
  }

 public:
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsTreeMapNode(const CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *parent,
                      const QString &name, double size, const QModelIndex &ind);

  virtual ~CQChartsTreeMapNode() { }

  const CQChartsTreeMapPlot *plot() const { return plot_; }

  CQChartsTreeMapHierNode *parent() const { return parent_; }

  virtual uint id() const { return id_; }

  virtual const QString &name() const { return name_; }

  virtual double size() const { return size_; }
  virtual void setSize(double s) { size_ = s; }

  virtual double x() const { return x_; }
  virtual void setX(double x) { x_ = x; }

  virtual double y() const { return y_; }
  virtual void setY(double y) { y_ = y; }

  virtual double w() const { return w_; }
  virtual void setW(double r) { w_ = r; }

  virtual double h() const { return h_; }
  virtual void setH(double r) { h_ = r; }

  virtual int colorId() const { return colorId_; }
  virtual void setColorId(int id) { colorId_ = id; }

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &c) { color_ = c; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  virtual int depth() const { return depth_; }
  virtual void setDepth(int i) { depth_ = i; }

  bool isFiller() const { return filler_; }
  void setFiller(bool b) { filler_ = b; }

  virtual double hierSize() const { return size(); }

  virtual QString hierName() const;

  virtual void setPosition(double x, double y, double w, double h);

  virtual bool contains(double x, double y) const;

  virtual bool placed() const { return placed_; }

  CQChartsTreeMapHierNode *rootNode(CQChartsTreeMapHierNode *root) const;

  friend bool operator<(const CQChartsTreeMapNode &n1, const CQChartsTreeMapNode &n2) {
    return n1.size_ < n2.size_;
  }

  virtual QColor interpColor(const CQChartsTreeMapPlot *plot, const CQChartsColor &c,
                             const ColorInd &colorInd, int n) const;

 protected:
  const CQChartsTreeMapPlot* plot_    { nullptr }; //!< parent plot
  CQChartsTreeMapHierNode*   parent_  { nullptr }; //!< parent hier node
  uint                       id_      { 0 };       //!< node id
  QString                    name_;                //!< node name
  double                     size_    { 0.0 };     //!< node size
  double                     x_       { 0.0 };     //!< node x
  double                     y_       { 0.0 };     //!< node y
  double                     w_       { 1.0 };     //!< node width
  double                     h_       { 1.0 };     //!< node height
  int                        colorId_ { -1 };      //!< node color index
  CQChartsColor              color_   { };         //!< node explicit color
  QModelIndex                ind_;                 //!< node model index
  int                        depth_   { 0 };       //!< node depth
  bool                       filler_  { false };   //!< is filler
  bool                       placed_  { false };   //!< is placed
};

//---

struct CQChartsTreeMapNodeCmp {
  bool operator()(const CQChartsTreeMapNode *n1, const CQChartsTreeMapNode *n2) {
    return (*n1) < (*n2);
  }
};

//---

//! \brief Tree Map Hier Node
class CQChartsTreeMapHierNode : public CQChartsTreeMapNode {
 public:
  using Nodes    = std::vector<CQChartsTreeMapNode*>;
  using Children = std::vector<CQChartsTreeMapHierNode*>;

 public:
  CQChartsTreeMapHierNode(const CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *parent=nullptr,
                          const QString &name="", const QModelIndex &ind=QModelIndex());

 ~CQChartsTreeMapHierNode();

  int hierInd() const { return hierInd_; }
  void setHierInd(int i) { hierInd_ = i; }

  //---

  double hierSize() const override;

  //---

  bool hasNodes() const { return ! nodes_.empty(); }

  const Nodes &getNodes() const { return nodes_; }

  //---

  bool hasChildren() const { return ! children_.empty(); }

  int numChildren() const { return children_.size(); }

  const Children &getChildren() const { return children_; }

  CQChartsTreeMapHierNode *childAt(int i) {
    assert(i >= 0 && i < numChildren());

    return children_[i];
  }

  //---

  void packNodes(double x, double y, double w, double h);

  void packSubNodes(double x, double y, double w, double h, const Nodes &nodes);

  void setPosition(double x, double y, double w, double h) override;

  void addNode(CQChartsTreeMapNode *node);

  void removeNode(CQChartsTreeMapNode *node);

  QColor interpColor(const CQChartsTreeMapPlot *plot, const CQChartsColor &c,
                     const ColorInd &colorInd, int n) const override;

 private:
  Nodes    nodes_;          //!< child nodes
  Children children_;       //!< child hier nodes
  int      hierInd_ { -1 }; //!< hier index
};

//---

class CQChartsTreeMapHierObj;

/*!
 * \brief Tree Map Plot Node object
 */
class CQChartsTreeMapNodeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsTreeMapNodeObj(const CQChartsTreeMapPlot *plot, CQChartsTreeMapNode *node,
                         CQChartsTreeMapHierObj *hierObj, const CQChartsGeom::BBox &rect,
                         const ColorInd &is);

  CQChartsTreeMapNode *node() const { return node_; }

  CQChartsTreeMapHierObj *parent() const { return hierObj_; }

  int ind() const { return i_; }
  void setInd(int i) { i_ = i; }

  //---

  QString typeName() const override { return "cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  void addChild(CQChartsTreeMapNodeObj *child) { children_.push_back(child); }

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  bool isChildSelected() const;

 protected:
  using Children = std::vector<CQChartsTreeMapNodeObj *>;

  const CQChartsTreeMapPlot* plot_    { nullptr }; //!< parent plot
  CQChartsTreeMapNode*       node_    { nullptr }; //!< associated tree node
  CQChartsTreeMapHierObj*    hierObj_ { nullptr }; //!< parent hierchical objects
  Children                   children_;            //!< child objects
  int                        i_       { 0 };       //!< node index
};

//---

//! \brief Tree Map Hier Box Object
class CQChartsTreeMapHierObj : public CQChartsTreeMapNodeObj {
 public:
  CQChartsTreeMapHierObj(const CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *hier,
                         CQChartsTreeMapHierObj *hierObj, const CQChartsGeom::BBox &rect,
                         const ColorInd &is);

  CQChartsTreeMapHierNode *hierNode() const { return hier_; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

 private:
  CQChartsTreeMapHierNode* hier_ { nullptr }; //!< associated tree hier
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Header,header)

/*!
 * \brief Tree Map Plot
 */
class CQChartsTreeMapPlot : public CQChartsHierPlot,
 public CQChartsObjHeaderShapeData<CQChartsTreeMapPlot>,
 public CQChartsObjHeaderTextData <CQChartsTreeMapPlot>,
 public CQChartsObjShapeData      <CQChartsTreeMapPlot>,
 public CQChartsObjTextData       <CQChartsTreeMapPlot> {
  Q_OBJECT

  // title/header shown and height
  Q_PROPERTY(bool           titles         READ isTitles       WRITE setTitles        )
  Q_PROPERTY(double         titleMaxExtent READ titleMaxExtent WRITE setTitleMaxExtent)
  Q_PROPERTY(CQChartsLength headerHeight   READ headerHeight   WRITE setHeaderHeight  )
  Q_PROPERTY(bool           colorById      READ isColorById    WRITE setColorById     )

  // header shape (border, fill)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Header,header)

  // header font, color
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Header,header)

  // box margin
  Q_PROPERTY(CQChartsLength marginWidth READ marginWidth WRITE setMarginWidth)

  // shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // text
  CQCHARTS_TEXT_DATA_PROPERTIES

 public:
  using Nodes = std::vector<CQChartsTreeMapNode*>;

 public:
  CQChartsTreeMapPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsTreeMapPlot();

  //---

  // title/header shown and height
  bool isTitles() const { return titles_; }
  void setTitles(bool b);

  double titleMaxExtent() const { return titleMaxExtent_; }
  void setTitleMaxExtent(double r);

  const CQChartsLength &headerHeight() const { return headerHeight_; }
  void setHeaderHeight(const CQChartsLength &l);

  double calcHeaderHeight() const;

  //---

  bool isColorById() const { return colorById_; }
  void setColorById(bool b);

  //---

  void setHeaderTextFontSize(double s);

  //---

  // box margin, border, fill, font
  const CQChartsLength &marginWidth() const { return marginWidth_; }
  void setMarginWidth(const CQChartsLength &l);

  //---

  void setTextFontSize(double s);

  //------

  CQChartsTreeMapHierNode *root() const { return root_; }

  CQChartsTreeMapHierNode *firstHier() const { return firstHier_; }

  CQChartsTreeMapHierNode *currentRoot() const;
  void setCurrentRoot(CQChartsTreeMapHierNode *r, bool update=true);

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

  double windowHeaderHeight() const { return windowHeaderHeight_; }
  double windowMarginWidth () const { return windowMarginWidth_ ; }

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  void clearPlotObjects() override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void postResize() override;

  //---

  bool addMenuItems(QMenu *menu) override;

 private:
  void initNodeObjs(CQChartsTreeMapHierNode *hier, CQChartsTreeMapHierObj *parentObj,
                    int depth, PlotObjs &objs) const;

  void resetNodes();

  void initNodes() const;

  void replaceNodes() const;

  void placeNodes(CQChartsTreeMapHierNode *hier) const;

  void colorNodes(CQChartsTreeMapHierNode *hier) const;

  void colorNode(CQChartsTreeMapNode *node) const;

  //---

  void loadHier() const;

  CQChartsTreeMapHierNode *addHierNode(CQChartsTreeMapHierNode *parent, const QString &name,
                                       const QModelIndex &nameInd) const;

  CQChartsTreeMapNode *addNode(CQChartsTreeMapHierNode *parent, const QString &name,
                               double size, const QModelIndex &nameInd) const;

  void loadFlat() const;

  CQChartsTreeMapNode *addNode(const QStringList &nameStrs, double size,
                               const QModelIndex &nameInd) const;

  void addExtraNodes(CQChartsTreeMapHierNode *hier) const;

  //---

  CQChartsTreeMapHierNode *childHierNode(CQChartsTreeMapHierNode *parent,
                                         const QString &name) const;
  CQChartsTreeMapNode *childNode(CQChartsTreeMapHierNode *parent,
                                 const QString &name) const;

  //---

  void transformNodes(CQChartsTreeMapHierNode *hier);

 public slots:
  void pushSlot();
  void popSlot();
  void popTopSlot();

  void updateCurrentRoot();

 private:
  using Node = CQChartsTreeMapHierNode;

  bool           titles_             { true };    //!< show title bar (header)
  double         titleMaxExtent_     { 0.5 };     //!< title bar max extent (0-1)
  CQChartsLength headerHeight_       { "0px" };   //!< header height (should be font based)
  CQChartsLength marginWidth_        { "2px" };   //!< box margin
  bool           colorById_          { true };    //!< color by id
  Node*          root_               { nullptr }; //!< root node
  Node*          firstHier_          { nullptr }; //!< first hier node
  QString        currentRootName_;                //!< current root name
  int            colorId_            { -1 };      //!< current color id
  int            numColorIds_        { 0 };       //!< num used color ids
  int            maxDepth_           { 1 };       //!< max hier depth
  int            hierInd_            { 0 };       //!< current hier ind
  mutable int    ig_                 { 0 };       //!< current group index
  mutable int    in_                 { 0 };       //!< current node index
  double         windowHeaderHeight_ { 0.01 };    //!< calculated window pixel header height
  double         windowMarginWidth_  { 0.01 };    //!< calculated window pixel margin width
};

#endif
