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
 * \ingroup Charts
 */
class CQChartsTreeMapPlotType : public CQChartsHierPlotType {
 public:
  CQChartsTreeMapPlotType();

  QString name() const override { return "treemap"; }
  QString desc() const override { return "TreeMap"; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  bool customXRange() const override { return false; }
  bool customYRange() const override { return false; }

  bool hasAxes() const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsTreeMapPlot;
class CQChartsTreeMapHierNode;

/*!
 * \brief Tree Map Node
 * \ingroup Charts
 */
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

  virtual QString hierName(QChar sep='/') const;

  virtual void setPosition(double x, double y, double w, double h);

  virtual bool contains(double x, double y) const;

  virtual bool placed() const { return placed_; }

  CQChartsTreeMapHierNode *rootNode(CQChartsTreeMapHierNode *root) const;

  friend bool operator<(const CQChartsTreeMapNode &n1, const CQChartsTreeMapNode &n2) {
    return n1.hierSize() < n2.hierSize();
  }

  friend bool operator>(const CQChartsTreeMapNode &n1, const CQChartsTreeMapNode &n2) {
    return n1.hierSize() > n2.hierSize();
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
    return (*n1) > (*n2);
  }
};

//---

/*!
 * \brief Tree Map Hier Node
 * \ingroup Charts
 */
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

  bool isShowTitle() const { return showTitle_; }
  void setShowTitle(bool b) { showTitle_ = b; }

  //---

  bool isExpanded() const { return expanded_; }
  void setExpanded(bool b) { expanded_ = b; }

  bool isHierExpanded() const;

  //---

  double hierSize() const override;

  //---

  bool hasNodes() const { return ! nodes_.empty(); }

  const Nodes &getNodes() const { return nodes_; }

  //---

  void addChild(CQChartsTreeMapHierNode *child);

  void removeChild(CQChartsTreeMapHierNode *child);

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
  Nodes    nodes_;               //!< child nodes
  Children children_;            //!< child hier nodes
  int      hierInd_   { -1 };    //!< hier index
  bool     showTitle_ { false }; //!< show title
  bool     expanded_  { true };  //!< is expanded
};

//---

class CQChartsTreeMapHierObj;

/*!
 * \brief Tree Map Plot Node object
 * \ingroup Charts
 */
class CQChartsTreeMapNodeObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(int ind READ ind WRITE setInd)

 public:
  CQChartsTreeMapNodeObj(const CQChartsTreeMapPlot *plot, CQChartsTreeMapNode *node,
                         CQChartsTreeMapHierObj *hierObj, const BBox &rect, const ColorInd &is);

  CQChartsTreeMapNode *node() const { return node_; }

  CQChartsTreeMapHierObj *parent() const { return hierObj_; }

  int ind() const { return i_; }
  void setInd(int i) { i_ = i; }

  //---

  QString typeName() const override { return "cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addChild(CQChartsTreeMapNodeObj *child) { children_.push_back(child); }

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  void drawText(CQChartsPaintDevice *device, const BBox &bbox);

  void calcPenBrush(CQChartsPenBrush &penBrush, bool isPoint, bool updateState) const;

  bool isPoint() const;

  //---

  void writeScriptData(CQChartsScriptPaintDevice *device) const override;

  //---

  bool isChildSelected() const;

 protected:
  using Children = std::vector<CQChartsTreeMapNodeObj *>;

  const CQChartsTreeMapPlot* plot_    { nullptr }; //!< parent plot
  CQChartsTreeMapNode*       node_    { nullptr }; //!< associated tree node
  CQChartsTreeMapHierObj*    hierObj_ { nullptr }; //!< parent hierarchical objects
  Children                   children_;            //!< child objects
  int                        i_       { 0 };       //!< node index
};

//---

/*!
 * \brief Tree Map Hier Box Object
 * \ingroup Charts
 */
class CQChartsTreeMapHierObj : public CQChartsTreeMapNodeObj {
  Q_OBJECT

 public:
  CQChartsTreeMapHierObj(const CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *hier,
                         CQChartsTreeMapHierObj *hierObj, const BBox &rect, const ColorInd &is);

  CQChartsTreeMapHierNode *hierNode() const { return hier_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(CQChartsScriptPaintDevice *device) const override;

 private:
  CQChartsTreeMapHierNode* hier_ { nullptr }; //!< associated tree hier
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Header,header)

/*!
 * \brief Tree Map Plot
 * \ingroup Charts
 */
class CQChartsTreeMapPlot : public CQChartsHierPlot,
 public CQChartsObjHeaderShapeData<CQChartsTreeMapPlot>,
 public CQChartsObjHeaderTextData <CQChartsTreeMapPlot>,
 public CQChartsObjShapeData      <CQChartsTreeMapPlot>,
 public CQChartsObjTextData       <CQChartsTreeMapPlot> {
  Q_OBJECT

  // title/header shown and height
  Q_PROPERTY(bool              titles           READ isTitles           WRITE setTitles          )
  Q_PROPERTY(bool              titleAutoHide    READ isTitleAutoHide    WRITE setTitleAutoHide   )
  Q_PROPERTY(CQChartsOptReal   titleMaxExtent   READ titleMaxExtent     WRITE setTitleMaxExtent  )
  Q_PROPERTY(CQChartsOptLength titleHeight      READ titleHeight        WRITE setTitleHeight     )
  Q_PROPERTY(bool              titleHierName    READ isTitleHierName    WRITE setTitleHierName   )
  Q_PROPERTY(bool              titleTextClipped READ isTitleTextClipped WRITE setTitleTextClipped)
  Q_PROPERTY(double            titleMargin      READ titleMargin        WRITE setTitleMargin     )
  Q_PROPERTY(int               titleDepth       READ titleDepth         WRITE setTitleDepth      )

  // color
  Q_PROPERTY(bool colorById READ isColorById WRITE setColorById)

  // header shape (stroke, fill)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Header,header)

  // header font, color
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Header,header)

  // options
  Q_PROPERTY(bool valueLabel READ isValueLabel WRITE setValueLabel)

  // box margin
  Q_PROPERTY(CQChartsLength marginWidth READ marginWidth WRITE setMarginWidth)

  // shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // text
  CQCHARTS_TEXT_DATA_PROPERTIES

  Q_PROPERTY(bool hierName    READ isHierName    WRITE setHierName   )
  Q_PROPERTY(int  numSkipHier READ numSkipHier   WRITE setNumSkipHier)
  Q_PROPERTY(bool textClipped READ isTextClipped WRITE setTextClipped)

 public:
  using Nodes = std::vector<CQChartsTreeMapNode*>;

 public:
  CQChartsTreeMapPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsTreeMapPlot();

  //---

  // title/header shown and height
  bool isTitles() const { return titleData_.visible; }
  void setTitles(bool b);

  bool isTitleAutoHide() const { return titleData_.autoHide; }
  void setTitleAutoHide(bool b);

  const CQChartsOptReal &titleMaxExtent() const { return titleData_.maxExtent; }
  void setTitleMaxExtent(const CQChartsOptReal &r);

  const CQChartsOptLength &titleHeight() const { return titleData_.height; }
  void setTitleHeight(const CQChartsOptLength &l);

  // get/set title hierarchical name
  bool isTitleHierName() const { return titleData_.hierName; }
  void setTitleHierName(bool b);

  // get/set header text clipped
  bool isTitleTextClipped() const { return titleData_.textClipped; }
  void setTitleTextClipped(bool b);

  // get/set title margin
  double titleMargin() const { return titleData_.margin; }
  void setTitleMargin(double r);

  // get/set title depth
  int titleDepth() const { return titleData_.depth; }
  void setTitleDepth(int d);

  //--

  double calcTitleHeight() const;

  //---

  // get/set node hierarchical name
  bool isHierName() const { return nodeData_.hierName; }
  void setHierName(bool b);

  // num of levels to skip (from the start) for hierarchical name
  int numSkipHier() const { return nodeData_.numSkipHier; }
  void setNumSkipHier(int n);

  // get/set node text clipped
  bool isTextClipped() const { return nodeData_.textClipped; }
  void setTextClipped(bool b);

  // get/set value label
  bool isValueLabel() const { return nodeData_.valueLabel; }
  void setValueLabel(bool b);

  // box margin
  const CQChartsLength &marginWidth() const { return nodeData_.marginWidth; }
  void setMarginWidth(const CQChartsLength &l);

  //---

  void setHeaderTextFontSize(double s);

  void setTextFontSize(double s);

  //---

  // is color by id
  bool isColorById() const { return colorById_; }
  void setColorById(bool b);

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

  Range calcRange() const override;

  void clearPlotObjects() override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void postResize() override;

  //---

  bool addMenuItems(QMenu *menu) override;

 protected:
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

  void removeHierNode(CQChartsTreeMapHierNode *hier);

  CQChartsTreeMapNode *hierAddNode(CQChartsTreeMapHierNode *parent, const QString &name,
                                   double size, const QModelIndex &nameInd) const;

  void loadFlat() const;

  CQChartsTreeMapNode *flatAddNode(const QStringList &nameStrs, double size,
                                   const QModelIndex &nameInd, const QString &name) const;

  void addExtraNodes(CQChartsTreeMapHierNode *hier) const;

  //---

  CQChartsTreeMapHierNode *childHierNode(CQChartsTreeMapHierNode *parent,
                                         const QString &name) const;
  CQChartsTreeMapNode *childNode(CQChartsTreeMapHierNode *parent,
                                 const QString &name) const;

  //---

  void transformNodes(CQChartsTreeMapHierNode *hier);

  //---

  void followViewExpandChanged() override;

  void modelViewExpansionChanged() override;
  void setNodeExpansion(CQChartsTreeMapHierNode *hierNode, const std::set<QModelIndex> &indSet);

  void resetNodeExpansion();
  void resetNodeExpansion(CQChartsTreeMapHierNode *hierNode);

  //---

  bool getValueSize(const CQChartsModelIndex &ind, double &size) const;

  //---

  virtual CQChartsTreeMapHierObj *createHierObj(CQChartsTreeMapHierNode *hier,
                                                CQChartsTreeMapHierObj *hierObj,
                                                const BBox &rect, const ColorInd &is) const;
  virtual CQChartsTreeMapNodeObj *createNodeObj(CQChartsTreeMapNode *node,
                                                CQChartsTreeMapHierObj *hierObj,
                                                const BBox &rect, const ColorInd &is) const;

 public slots:
  void pushSlot();
  void popSlot();
  void popTopSlot();

  void updateCurrentRoot();

 private:
  using Node = CQChartsTreeMapHierNode;

  struct TitleData {
    bool              visible     { true };  //!< show title bar (header)
    bool              autoHide    { true };  //!< auto hide if larger than max extent
    CQChartsOptReal   maxExtent;             //!< user specified max height extent (0-1)
    CQChartsOptLength height;                //!< user specified height
    bool              hierName    { false }; //!< show hierarchical name
    bool              textClipped { true };  //!< is text clipped
    double            margin      { 3 };     //!< margin (pixels)
    int               depth       { -1 };    //!< max depth for header
  };

  struct NodeData {
    bool           hierName    { false }; //!< show hierarchical name
    bool           textClipped { true };  //!< is text clipped
    int            numSkipHier { 0 };     //!< number of levels of hier name to skip
    bool           valueLabel  { false }; //!< draw value with name
    CQChartsLength marginWidth { "2px" }; //!< box margin
  };

  TitleData   titleData_;                      //!< title data
  NodeData    nodeData_;                       //!< node data
  bool        colorById_          { true };    //!< color by id
  Node*       root_               { nullptr }; //!< root node
  Node*       firstHier_          { nullptr }; //!< first hier node
  QString     currentRootName_;                //!< current root name
  int         colorId_            { -1 };      //!< current color id
  int         numColorIds_        { 0 };       //!< num used color ids
  int         maxDepth_           { 1 };       //!< max hier depth
  int         hierInd_            { 0 };       //!< current hier ind
  mutable int ig_                 { 0 };       //!< current group index
  mutable int in_                 { 0 };       //!< current node index
  double      windowHeaderHeight_ { 0.01 };    //!< calculated window pixel header height
  double      windowMarginWidth_  { 0.01 };    //!< calculated window pixel margin width
};

#endif
