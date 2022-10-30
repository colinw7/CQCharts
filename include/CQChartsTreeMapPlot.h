#ifndef CQChartsTreeMapPlot_H
#define CQChartsTreeMapPlot_H

#include <CQChartsHierPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsData.h>
#include <CQChartsArea.h>
#include <CSafeIndex.h>
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

  Category category() const override { return Category::ONE_D; }

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
    static uint lastId = 0;

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

  //---

  virtual int colorId() const { return colorId_; }
  virtual void setColorId(int id) { colorId_ = id; }

  const Color &color() const { return color_; }
  void setColor(const Color &c) { color_ = c; }

  //---

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  virtual int depth() const { return depth_; }
  virtual void setDepth(int i) { depth_ = i; }

  virtual bool isHier() const { return false; }

  bool isFiller() const { return filler_; }
  void setFiller(bool b) { filler_ = b; }

  virtual double hierSize() const { return size(); }

  virtual QString hierName(const QString &sep) const;

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

  //---

  virtual QString calcGroupName() const;

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

  mutable QString hierName_;    //!< hier name
  mutable QChar   hierNameSep_; //!< hier name separator
  mutable int     ig_ { -1 };   //!< group ind
  mutable int     ng_ { -1 };   //!< num groups
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
  using HierNode = CQChartsTreeMapHierNode;
  using Node     = CQChartsTreeMapNode;
  using Nodes    = std::vector<Node*>;
  using Children = std::vector<HierNode*>;

 public:
  CQChartsTreeMapHierNode(const Plot *plot, HierNode *parent=nullptr, const QString &name="",
                          const QModelIndex &ind=QModelIndex());

 ~CQChartsTreeMapHierNode();

  //---

  bool isHier() const override { return true; }

  //---

  //! get/set hierarchical index
  int hierInd() const { return hierInd_; }
  void setHierInd(int i) { hierInd_ = i; }

  //---

  //! get/set group name
  const QString &groupName() const { return groupName_; }
  void setGroupName(const QString &v) { groupName_ = v; }

  //---

  //! get/set show title
  bool isShowTitle() const { return showTitle_; }
  void setShowTitle(bool b) { showTitle_ = b; }

  //---

  //! get/set is visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  bool isHierVisible() const;

  //---

  //! get/set expanded
  bool isExpanded() const { return expanded_; }
  void setExpanded(bool b);

  bool isHierExpanded() const;

  //---

  //! get hierarchical size
  double hierSize() const override;

  //---

  //! get has child nodes
  bool hasNodes() const { return ! nodes_.empty(); }

  //! get child nodes
  const Nodes &getNodes() const { return nodes_; }

  //---

  //! has child hier nodes
  bool hasChildren() const { return ! children_.empty(); }

  //! get child hier nodes
  const Children &getChildren() const { return children_; }

  int numChildren() const { return int(children_.size()); }

  HierNode *childAt(int i) { return CUtil::safeIndex(children_, i); }

  void addChild(HierNode *child);

  void removeChild(HierNode *child);

  //---

  //! pack child nodes
  void packNodes(double x, double y, double w, double h);

  void packSubNodes(double x, double y, double w, double h, const Nodes &nodes);

  //! add child node
  void addNode(Node *node);

  //! remove child node
  void removeNode(Node *node);

  //! set node position
  void setPosition(double x, double y, double w, double h) override;

  //! interp color
  QColor interpColor(const Plot *plot, const Color &c,
                     const ColorInd &colorInd, int n) const override;

  //---

  QString calcGroupName() const override;

 private:
  QString  groupName_;           //!< group name
  Nodes    nodes_;               //!< child nodes
  Children children_;            //!< child hier nodes
  int      hierInd_   { -1 };    //!< hier index
  bool     showTitle_ { false }; //!< show title
  bool     visible_   { true };  //!< is visible
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

  Q_PROPERTY(int     ind  READ ind WRITE setInd)
  Q_PROPERTY(QString name READ hierName)
  Q_PROPERTY(double  size READ hierSize)

 public:
  using Plot    = CQChartsTreeMapPlot;
  using Node    = CQChartsTreeMapNode;
  using HierObj = CQChartsTreeMapHierObj;
  using NodeObj = CQChartsTreeMapNodeObj;
  using Units   = CQChartsUnits::Type;

 public:
  CQChartsTreeMapNodeObj(const Plot *plot, Node *node, HierObj *hierObj,
                         const BBox &rect, const ColorInd &is);

  Node *node() const { return node_; }

  HierObj *parent() const { return hierObj_; }

  //! get/set index
  int ind() const { return i_; }
  void setInd(int i) { i_ = i; }

  QString hierName() const { return node_->name(); }
  double  hierSize() const { return node_->hierSize(); }

  //---

  QString typeName() const override { return "cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  virtual QString calcGroupName() const;

  //---

  void addChild(NodeObj *child) { children_.push_back(child); }

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawText(PaintDevice *device, const BBox &bbox, bool updateState) const;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  void calcPenBrushNodePoint(PenBrush &penBrush, bool isNodePoint, bool updateState) const;

  bool isMinArea() const;

  bool isNodePoint() const;

  //---

  bool isChildSelected() const;

 protected:
  using Children = std::vector<NodeObj *>;

  const Plot* plot_    { nullptr }; //!< parent plot
  Node*       node_    { nullptr }; //!< associated tree node
  HierObj*    hierObj_ { nullptr }; //!< parent hierarchical object
  Children    children_;            //!< child objects
  int         i_       { 0 };       //!< node index

  mutable int numColorIds_ { -1 }; //!< associated group's number of color ids
};

//---

/*!
 * \brief Tree Map Hier Box Object
 * \ingroup Charts
 */
class CQChartsTreeMapHierObj : public CQChartsTreeMapNodeObj {
  Q_OBJECT

  Q_PROPERTY(QString name READ hierName)
  Q_PROPERTY(double  size READ hierSize)

 public:
  using Plot     = CQChartsTreeMapPlot;
  using HierNode = CQChartsTreeMapHierNode;
  using HierObj  = CQChartsTreeMapHierObj;

 public:
  CQChartsTreeMapHierObj(const Plot *plot, HierNode *hier, HierObj *hierObj,
                         const BBox &rect, const ColorInd &is);

  HierNode *hierNode() const { return hier_; }

  QString hierName() const { return node_->name(); }
  double  hierSize() const { return node_->hierSize(); }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  QString calcGroupName() const override;

  //---

  bool inside(const Point &p) const override;

  bool isSelectable() const override;
  bool isClickable() const override;

  void getObjSelectIndices(Indices &inds) const override;

  bool selectDoubleClick(const Point &, SelMod) override;

  //---

  void draw(PaintDevice *device) const override;

  void drawHeader(PaintDevice *device, const BBox &bbox, bool updateState) const;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  void calcHeaderPenBrush(PenBrush &penBrush, bool updateState) const;

 private:
  HierNode* hier_ { nullptr }; //!< associated tree hier
};

//---

/*!
 * \brief Tree Map Plot
 * \ingroup Charts
 */
class CQChartsTreeMapPlot : public CQChartsHierPlot,
 public CQChartsObjHierShapeData  <CQChartsTreeMapPlot>,
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

  // coloring
  Q_PROPERTY(NodeColorType nodeColorType READ nodeColorType WRITE setNodeColorType)
  Q_PROPERTY(HierColorType hierColorType READ hierColorType WRITE setHierColorType)

  // hier node shape (stroke, fill)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Hier, hier)

  // hier header shape (stroke, fill)
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Header, header)

  // hier header (font, color, ...)
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Header, header)

  // node options
  Q_PROPERTY(bool         valueLabel   READ isValueLabel WRITE setValueLabel)
  Q_PROPERTY(ValueCombine valueCombine READ valueCombine WRITE setValueCombine)

  // box margin
  Q_PROPERTY(CQChartsLength marginWidth READ marginWidth WRITE setMarginWidth)

  // node shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // node text
  CQCHARTS_TEXT_DATA_PROPERTIES

  // global options
  Q_PROPERTY(bool         hierSelect  READ isHierSelect  WRITE setHierSelect )
  Q_PROPERTY(bool         hierName    READ isHierName    WRITE setHierName   )
  Q_PROPERTY(int          numSkipHier READ numSkipHier   WRITE setNumSkipHier)
  Q_PROPERTY(bool         textClipped READ isTextClipped WRITE setTextClipped)
  Q_PROPERTY(CQChartsArea minArea     READ minArea       WRITE setMinArea    )

  // grouping
  Q_PROPERTY(bool splitGroups  READ isSplitGroups  WRITE setSplitGroups )
  Q_PROPERTY(bool groupPalette READ isGroupPalette WRITE setGroupPalette)

  Q_ENUMS(ValueCombine)
  Q_ENUMS(NodeColorType)
  Q_ENUMS(HierColorType)

 public:
  enum class ValueCombine {
    SKIP,
    REPLACE,
    WARN,
    SUM
  };

  enum class NodeColorType {
    ID,
    PARENT_VALUE,
    GLOBAL_VALUE
  };

  enum class HierColorType {
    ID,
    BLEND,
    PARENT_VALUE,
    GLOBAL_VALUE
  };

  using Node     = CQChartsTreeMapNode;
  using Nodes    = std::vector<Node*>;
  using HierNode = CQChartsTreeMapHierNode;
  using HierObj  = CQChartsTreeMapHierObj;
  using NodeObj  = CQChartsTreeMapNodeObj;

  using OptLength = CQChartsOptLength;
  using OptReal   = CQChartsOptReal;
  using Length    = CQChartsLength;
  using Area      = CQChartsArea;
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

  //! get/set title hierarchical name
  bool isTitleHierName() const { return titleData_.hierName; }
  void setTitleHierName(bool b);

  //! get/set header text clipped
  bool isTitleTextClipped() const { return titleData_.textClipped; }
  void setTitleTextClipped(bool b);

  //! get/set title margin
  double titleMargin() const { return titleData_.margin; }
  void setTitleMargin(double r);

  //! get/set title depth
  int titleDepth() const { return titleData_.depth; }
  void setTitleDepth(int d);

  //--

  double calcTitleHeight() const;

  //---

  //! get/set hier node selectable
  bool isHierSelect() const { return hierSelect_; }
  void setHierSelect(bool b) { hierSelect_ = b; }

  //---

  //! get/set node hierarchical name
  bool isHierName() const { return nodeData_.hierName; }
  void setHierName(bool b);

  //! get/set num of levels to skip (from the start) for hierarchical name
  int numSkipHier() const { return nodeData_.numSkipHier; }
  void setNumSkipHier(int n);

  //! get/set node text clipped
  bool isTextClipped() const { return nodeData_.textClipped; }
  void setTextClipped(bool b);

  //! get/set value combine policy
  const ValueCombine &valueCombine() const { return nodeData_.valueCombine; }
  void setValueCombine(const ValueCombine &combine);

  //! get/set value label
  bool isValueLabel() const { return nodeData_.valueLabel; }
  void setValueLabel(bool b);

  //! get/set box margin
  const Length &marginWidth() const { return nodeData_.marginWidth; }
  void setMarginWidth(const Length &l);

  //! get/set min area
  const Area &minArea() const { return treeData_.minArea; }
  void setMinArea(const Area &a);

  //---

  //! get/set split groups
  bool isSplitGroups() const { return treeData_.splitGroups; }
  void setSplitGroups(bool b);

  //! get/set group palette
  bool isGroupPalette() const { return treeData_.groupPalette; }
  void setGroupPalette(bool b);

  //---

  void setHeaderTextFontSize(double s);

  void setTextFontSize(double s);

  //---

  //! get/set node color type
  const NodeColorType &nodeColorType() const { return nodeColorType_; }
  void setNodeColorType(const NodeColorType &type);

  //! get/set hier color type
  const HierColorType &hierColorType() const { return hierColorType_; }
  void setHierColorType(const HierColorType &type);

  //------

  HierNode *root(const QString &groupName) const;

  HierNode *firstHier(const QString &groupName) const;

  //! get/set current root
  HierNode *currentRoot(const QString &groupName) const;
  void setCurrentRoot(const QString &groupName, HierNode *r, bool update=true);

  //---

  //int colorId(const QString &groupName) const;

  int numColorIds(const QString &groupName) const;

  void initColorIds(const QString &groupName);

  int nextColorId(const QString &groupName);

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

  bool addMenuItems(QMenu *menu, const Point &p) override;

  //---

  bool isPathExpanded(const QString &path) const;
  void setPathExpanded(const QString &path, bool expanded);

  void resetPathExpanded();

  //---

  int numGroups() const override;
  int groupNum(const QString &groupName) const;

  //---

  bool hasValueRange() const { return hasValueRange_; }

  double minValue() const { return minValue_; }
  double maxValue() const { return maxValue_; }

 protected:
  void initNodeObjs(HierNode *hier, const QString &groupName, HierObj *parentObj,
                    int depth, PlotObjs &objs) const;

  void resetNodes();

  void initNodes() const;

  void replaceNodes() const;

  void placeNodes(HierNode *hier, double x1, double y1, double x2, double y2) const;

  //---

  void colorGroupNodes(const QString &groupName) const;

  void colorNodes(const QString &groupName, HierNode *hier) const;

  void colorNode(const QString &groupName, Node *node) const;

  //---

  void loadHier() const;

  HierNode *addHierNode(const QString &groupName, HierNode *parent, const QString &name,
                        const QModelIndex &nameInd) const;

  void removeHierNode(HierNode *hier);

  Node *hierAddNode(const QString &groupName, HierNode *parent, const QString &name,
                    double size, const QModelIndex &nameInd) const;

  void loadFlat() const;

  Node *flatAddNode(const QString &groupName, const QStringList &nameStrs, double size,
                    const QModelIndex &nameInd, const QString &name) const;

  void addExtraNodes(HierNode *hier) const;

  //---

  int nextHierInd(const QString &groupName) const;

  int groupInd(const QString &groupName) const;
  int nextGroupInd(const QString &groupName) const;

  int valueInd(const QString &groupName) const;
  int nextValueInd(const QString &groupName) const;

  int maxDepth(const QString &groupName) const;
  void updateMaxDepth(const QString &groupName, int depth) const;

  //---

  HierNode *childHierNode(HierNode *parent, const QString &name) const;
  Node *childNode(HierNode *parent, const QString &name) const;

  //---

  void transformNodes(HierNode *hier);

  //---

  void followViewExpandChanged() override;

  void modelViewExpansionChanged() override;
  void setNodeExpansion(HierNode *hierNode, const std::set<QModelIndex> &indSet);

  void resetNodeExpansion(bool expanded);
  void resetNodeExpansion(HierNode *hierNode, bool expanded);

  //---

  bool getValueSize(const ModelIndex &ind, double &size) const;

  //---

  virtual HierNode *createHierNode(HierNode *parent, const QString &name,
                                   const QModelIndex &nameInd) const;

  virtual Node *createNode(HierNode *parent, const QString &name,
                           double size, const QModelIndex &nameInd) const;

  virtual HierObj *createHierObj(HierNode *hier, HierObj *hierObj,
                                 const BBox &rect, const ColorInd &is) const;
  virtual NodeObj *createNodeObj(Node *node, HierObj *hierObj,
                                 const BBox &rect, const ColorInd &is) const;

  //---

 public Q_SLOTS:
  void pushSlot();
  void popSlot();
  void popTopSlot();

  void expandSlot();
  void collapseSlot();

 private:
  void popTop(bool update);

  void updateCurrentRoot();

  void menuPlotObjs(PlotObjs &objs) const;

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

  struct TreeData {
    Area    minArea;                //!< min area
    bool    splitGroups  { false }; //!< split groups
    bool    groupPalette { false }; //!< use separate palette per group
    QString currentGroupName;       //!< current group name
  };

  struct NodeData {
    bool         hierName     { false };              //!< show hierarchical name
    bool         textClipped  { true };               //!< is text clipped
    int          numSkipHier  { 0 };                  //!< number of levels of hier name to skip
    bool         valueLabel   { false };              //!< draw value with name
    ValueCombine valueCombine { ValueCombine::SKIP }; //!< value combine policy
    Length       marginWidth  { Length::pixel(2) };   //!< box margin
  };

  struct ColorData {
    int colorId     { -1 }; //!< current color id
    int numColorIds { 0 };  //!< num used color ids

    void reset() {
      colorId     = -1;
      numColorIds = 0;
    }
  };

  // tree map data
  struct TreeMapData {
    HierNode*   root             { nullptr }; //!< root node
    HierNode*   firstHier        { nullptr }; //!< first hier node
    QString     currentRootName;              //!< persistent current root name (push in)
    mutable int hierInd          { 0 };       //!< current hier ind
    mutable int maxDepth         { 1 };       //!< max hier depth
    mutable int ig               { 0 };       //!< current group index
    mutable int in               { 0 };       //!< current node index
    ColorData   colorData;                    //!< color index data

    void reset() {
      root      = nullptr;
      firstHier = nullptr;
      hierInd   = 0;
      maxDepth  = 1;
      ig        = 0;
      in        = 0;

      colorData.reset();
    }
  };

  using GroupTreeMapData = std::map<QString, TreeMapData>;
  using GroupNameSet     = std::set<QString>;

  using PathExpanded = std::map<QString, bool>;

 private:
  TreeMapData &getTreeMapData(const QString &groupName) const;

 private:
  TitleData titleData_;            //!< title config data
  TreeData  treeData_;             //!< tree config data
  NodeData  nodeData_;             //!< node config data
  bool      hierSelect_ { false }; //!< allow select hier node

  NodeColorType nodeColorType_ { NodeColorType::ID };    //!< node color type
  HierColorType hierColorType_ { HierColorType::BLEND }; //!< hier node color type

  GroupTreeMapData groupTreeMapData_; //!< grouped tree map data
  TreeMapData      treeMapData_;      //!< tree map data

  GroupNameSet groupNameSet_; //!< group name set

  double windowHeaderHeight_ { 0.01 }; //!< calculated window pixel header height
  double windowMarginWidth_  { 0.01 }; //!< calculated window pixel margin width

  PathExpanded pathExpanded_;

  mutable QString menuGroupName_; //!< group name for object at menu invocation

  mutable bool   hasValueRange_ { false };
  mutable double minValue_      { 0.0 };
  mutable double maxValue_      { 1.0 };
};

//---

/*!
 * \brief Treemap Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsTreeMapPlotCustomControls : public CQChartsHierPlotCustomControls {
  Q_OBJECT

 public:
  using TreeMapPlot = CQChartsTreeMapPlot;

 public:
  CQChartsTreeMapPlotCustomControls(CQCharts *charts);

  void init() override;

  void addWidgets() override;

  void addOptionsWidgets() override;

  void setPlot(CQChartsPlot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void connectSlots(bool b) override;

  //---

  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 protected Q_SLOTS:
  void headerSlot();
  void valueSlot();
  void followViewSlot();

 protected:
  TreeMapPlot* plot_ { nullptr };

  FrameData optionsFrame_;

  QCheckBox* headerCheck_     { nullptr };
  QCheckBox* valueCheck_      { nullptr };
  QCheckBox* followViewCheck_ { nullptr };
};

#endif
