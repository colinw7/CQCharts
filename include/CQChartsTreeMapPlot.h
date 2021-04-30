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

  bool canEqualScale() const override { return true; }

  bool supportsIdColumn() const override { return true; }

  QString description() const override;

  Plot *create(View *view, const ModelP &model) const override;
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
  using Plot     = CQChartsTreeMapPlot;
  using HierNode = CQChartsTreeMapHierNode;
  using Node     = CQChartsTreeMapNode;
  using Color    = CQChartsColor;
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsTreeMapNode(const Plot *plot, HierNode *parent, const QString &name,
                      double size, const QModelIndex &ind);

  virtual ~CQChartsTreeMapNode() { }

  const Plot *plot() const { return plot_; }

  HierNode *parent() const { return parent_; }

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

  const Color &color() const { return color_; }
  void setColor(const Color &c) { color_ = c; }

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

  HierNode *rootNode(HierNode *root) const;

  //---

  friend bool operator<(const Node &n1, const Node &n2) {
    return n1.hierSize() < n2.hierSize();
  }

  friend bool operator>(const Node &n1, const Node &n2) {
    return n1.hierSize() > n2.hierSize();
  }

  //---

  virtual QColor interpColor(const Plot *plot, const Color &c,
                             const ColorInd &colorInd, int n) const;

 protected:
  const Plot* plot_    { nullptr }; //!< parent plot
  HierNode*   parent_  { nullptr }; //!< parent hier node
  uint        id_      { 0 };       //!< node id
  QString     name_;                //!< node name
  double      size_    { 0.0 };     //!< node size
  double      x_       { 0.0 };     //!< node x
  double      y_       { 0.0 };     //!< node y
  double      w_       { 1.0 };     //!< node width
  double      h_       { 1.0 };     //!< node height
  int         colorId_ { -1 };      //!< node color index
  Color       color_   { };         //!< node explicit color
  QModelIndex ind_;                 //!< node model index
  int         depth_   { 0 };       //!< node depth
  bool        filler_  { false };   //!< is filler
  bool        placed_  { false };   //!< is placed
};

//---

//! tree map node compare
struct CQChartsTreeMapNodeCmp {
  using Node = CQChartsTreeMapNode;

  bool operator()(const Node *n1, const Node *n2) {
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
  using Plot     = CQChartsTreeMapPlot;
  using Node     = CQChartsTreeMapNode;
  using Nodes    = std::vector<Node*>;
  using HierNode = CQChartsTreeMapHierNode;
  using Children = std::vector<HierNode*>;

 public:
  CQChartsTreeMapHierNode(const Plot *plot, HierNode *parent=nullptr, const QString &name="",
                          const QModelIndex &ind=QModelIndex());

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

  void addChild(HierNode *child);

  void removeChild(HierNode *child);

  bool hasChildren() const { return ! children_.empty(); }

  int numChildren() const { return children_.size(); }

  const Children &getChildren() const { return children_; }

  HierNode *childAt(int i) {
    assert(i >= 0 && i < numChildren());

    return children_[i];
  }

  //---

  void packNodes(double x, double y, double w, double h);

  void packSubNodes(double x, double y, double w, double h, const Nodes &nodes);

  void setPosition(double x, double y, double w, double h) override;

  void addNode(Node *node);

  void removeNode(Node *node);

  QColor interpColor(const Plot *plot, const Color &c,
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
  using Plot    = CQChartsTreeMapPlot;
  using Node    = CQChartsTreeMapNode;
  using HierObj = CQChartsTreeMapHierObj;
  using NodeObj = CQChartsTreeMapNodeObj;

 public:
  CQChartsTreeMapNodeObj(const Plot *plot, Node *node, HierObj *hierObj,
                         const BBox &rect, const ColorInd &is);

  Node *node() const { return node_; }

  HierObj *parent() const { return hierObj_; }

  int ind() const { return i_; }
  void setInd(int i) { i_ = i; }

  //---

  QString typeName() const override { return "cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addChild(NodeObj *child) { children_.push_back(child); }

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawText(PaintDevice *device, const BBox &bbox) const;

  void calcPenBrush(PenBrush &penBrush, bool isNodePoint, bool updateState) const;

  bool isNodePoint() const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

  //---

  bool isChildSelected() const;

 protected:
  using Children = std::vector<NodeObj *>;

  const Plot* plot_    { nullptr }; //!< parent plot
  Node*       node_    { nullptr }; //!< associated tree node
  HierObj*    hierObj_ { nullptr }; //!< parent hierarchical objects
  Children    children_;            //!< child objects
  int         i_       { 0 };       //!< node index
};

//---

/*!
 * \brief Tree Map Hier Box Object
 * \ingroup Charts
 */
class CQChartsTreeMapHierObj : public CQChartsTreeMapNodeObj {
  Q_OBJECT

 public:
  using Plot     = CQChartsTreeMapPlot;
  using HierNode = CQChartsTreeMapHierNode;
  using HierObj  = CQChartsTreeMapHierObj;

 public:
  CQChartsTreeMapHierObj(const Plot *plot, HierNode *hier, HierObj *hierObj,
                         const BBox &rect, const ColorInd &is);

  HierNode *hierNode() const { return hier_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawText(PaintDevice *device, const BBox &bbox) const;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

 private:
  HierNode* hier_ { nullptr }; //!< associated tree hier
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Header, header)

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
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Header, header)

  // header font, color
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Header, header)

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
  using Node     = CQChartsTreeMapNode;
  using Nodes    = std::vector<Node*>;
  using HierNode = CQChartsTreeMapHierNode;
  using HierObj  = CQChartsTreeMapHierObj;
  using NodeObj  = CQChartsTreeMapNodeObj;

  using OptLength = CQChartsOptLength;
  using OptReal   = CQChartsOptReal;
  using Length    = CQChartsLength;
  using Color     = CQChartsColor;
  using Alpha     = CQChartsAlpha;
  using ColorInd  = CQChartsUtil::ColorInd;

 public:
  CQChartsTreeMapPlot(View *view, const ModelP &model);
 ~CQChartsTreeMapPlot();

  //---

  void init() override;
  void term() override;

  //---

  // title/header shown and height
  bool isTitles() const { return titleData_.visible; }
  void setTitles(bool b);

  bool isTitleAutoHide() const { return titleData_.autoHide; }
  void setTitleAutoHide(bool b);

  const OptReal &titleMaxExtent() const { return titleData_.maxExtent; }
  void setTitleMaxExtent(const OptReal &r);

  const OptLength &titleHeight() const { return titleData_.height; }
  void setTitleHeight(const OptLength &l);

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
  const Length &marginWidth() const { return nodeData_.marginWidth; }
  void setMarginWidth(const Length &l);

  //---

  void setHeaderTextFontSize(double s);

  void setTextFontSize(double s);

  //---

  // is color by id
  bool isColorById() const { return colorById_; }
  void setColorById(bool b);

  //------

  HierNode *root() const { return root_; }

  HierNode *firstHier() const { return firstHier_; }

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

  void colorNode(Node *node) const;

  //---

  void loadHier() const;

  HierNode *addHierNode(HierNode *parent, const QString &name, const QModelIndex &nameInd) const;

  void removeHierNode(HierNode *hier);

  Node *hierAddNode(HierNode *parent, const QString &name,
                    double size, const QModelIndex &nameInd) const;

  void loadFlat() const;

  Node *flatAddNode(const QStringList &nameStrs, double size,
                    const QModelIndex &nameInd, const QString &name) const;

  void addExtraNodes(HierNode *hier) const;

  //---

  HierNode *childHierNode(HierNode *parent, const QString &name) const;
  Node *childNode(HierNode *parent, const QString &name) const;

  //---

  void transformNodes(HierNode *hier);

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

 private:
  void updateCurrentRoot();

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  struct TitleData {
    bool      visible     { true };  //!< show title bar (header)
    bool      autoHide    { true };  //!< auto hide if larger than max extent
    OptReal   maxExtent;             //!< user specified max height extent (0-1)
    OptLength height;                //!< user specified height
    bool      hierName    { false }; //!< show hierarchical name
    bool      textClipped { true };  //!< is text clipped
    double    margin      { 3 };     //!< margin (pixels)
    int       depth       { -1 };    //!< max depth for header
  };

  struct NodeData {
    bool   hierName    { false }; //!< show hierarchical name
    bool   textClipped { true };  //!< is text clipped
    int    numSkipHier { 0 };     //!< number of levels of hier name to skip
    bool   valueLabel  { false }; //!< draw value with name
    Length marginWidth { "2px" }; //!< box margin
  };

  TitleData   titleData_;                      //!< title data
  NodeData    nodeData_;                       //!< node data
  bool        colorById_          { true };    //!< color by id
  HierNode*   root_               { nullptr }; //!< root node
  HierNode*   firstHier_          { nullptr }; //!< first hier node
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

//---

class CQChartsTreeMapPlotCustomControls : public CQChartsHierPlotCustomControls {
  Q_OBJECT

 public:
  using TreeMapPlot = CQChartsTreeMapPlot;

 public:
  CQChartsTreeMapPlotCustomControls(CQCharts *charts);

  void setPlot(CQChartsPlot *plot) override;

 private:
  void connectSlots(bool b);

 public slots:
  void updateWidgets() override;

 private slots:
  void valueSlot();

 private:
  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 private:
  TreeMapPlot* plot_       { nullptr };
  QCheckBox*   valueCheck_ { nullptr };
};

#endif
