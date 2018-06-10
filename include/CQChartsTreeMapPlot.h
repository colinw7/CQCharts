#ifndef CQChartsTreeMapPlot_H
#define CQChartsTreeMapPlot_H

#include <CQChartsHierPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsData.h>
#include <QModelIndex>

class CQChartsTreeMapPlot;
class CQChartsTreeMapHierNode;

class CQChartsTreeMapNode {
 private:
  static uint nextId() {
    static int lastId = 0;

    return ++lastId;
  }

 public:
  CQChartsTreeMapNode(CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *parent,
                      const QString &name, double size, const QModelIndex &ind);

  virtual ~CQChartsTreeMapNode() { }

  CQChartsTreeMapPlot *plot() const { return plot_; }

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

  virtual QColor interpColor(CQChartsTreeMapPlot *plot, int n) const;

 protected:
  CQChartsTreeMapPlot*     plot_    { nullptr }; // parent plot
  CQChartsTreeMapHierNode* parent_  { nullptr }; // parent hier node
  uint                     id_      { 0 };       // node id
  QString                  name_;                // node name
  double                   size_    { 0.0 };     // node size
  double                   x_       { 0.0 };     // node x
  double                   y_       { 0.0 };     // node y
  double                   w_       { 1.0 };     // node width
  double                   h_       { 1.0 };     // node height
  int                      colorId_ { -1 };      // node color index
  CQChartsColor            color_   { };         // node explicit color
  QModelIndex              ind_;                 // node model index
  int                      depth_   { 0 };       // node depth
  bool                     filler_  { false };   // is filler
  bool                     placed_  { false };   // is placed
};

//---

struct CQChartsTreeMapNodeCmp {
  bool operator()(const CQChartsTreeMapNode *n1, const CQChartsTreeMapNode *n2) {
    return (*n1) < (*n2);
  }
};

//---

class CQChartsTreeMapHierNode : public CQChartsTreeMapNode {
 public:
  using Nodes    = std::vector<CQChartsTreeMapNode*>;
  using Children = std::vector<CQChartsTreeMapHierNode*>;

 public:
  CQChartsTreeMapHierNode(CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *parent=nullptr,
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

  void setPosition(double x, double y, double w, double h);

  void addNode(CQChartsTreeMapNode *node);

  void removeNode(CQChartsTreeMapNode *node);

  QColor interpColor(CQChartsTreeMapPlot *plot, int n) const override;

 private:
  Nodes    nodes_;          // child nodes
  Children children_;       // child hier nodes
  int      hierInd_ { -1 }; // hier index
};

//---

class CQChartsTreeMapHierObj;

class CQChartsTreeMapObj : public CQChartsPlotObj {
 public:
  CQChartsTreeMapObj(CQChartsTreeMapPlot *plot, CQChartsTreeMapNode *node,
                     CQChartsTreeMapHierObj *hierObj, const CQChartsGeom::BBox &rect,
                     int i, int n);

  CQChartsTreeMapNode *node() const { return node_; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 protected:
  CQChartsTreeMapPlot*    plot_    { nullptr };
  CQChartsTreeMapNode*    node_    { nullptr };
  CQChartsTreeMapHierObj* hierObj_ { nullptr };
  int                     i_       { 0 };
  int                     n_       { 0 };
};

//---

class CQChartsTreeMapHierObj : public CQChartsTreeMapObj {
 public:
  CQChartsTreeMapHierObj(CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *hier,
                         CQChartsTreeMapHierObj *hierObj, const CQChartsGeom::BBox &rect,
                         int i, int n);

  CQChartsTreeMapHierNode *hierNode() const { return hier_; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsTreeMapHierNode* hier_ { nullptr };
};

//---

class CQChartsTreeMapPlotType : public CQChartsHierPlotType {
 public:
  CQChartsTreeMapPlotType();

  QString name() const override { return "treemap"; }
  QString desc() const override { return "TreeMap"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsTreeMapPlot : public CQChartsHierPlot {
  Q_OBJECT

  // title/header shown and height
  Q_PROPERTY(bool           titles         READ isTitles       WRITE setTitles        )
  Q_PROPERTY(double         titleMaxExtent READ titleMaxExtent WRITE setTitleMaxExtent)
  Q_PROPERTY(CQChartsLength headerHeight   READ headerHeight   WRITE setHeaderHeight  )

  // header border, fill, font
  Q_PROPERTY(bool             headerBorder      READ isHeaderBorder    WRITE setHeaderBorder     )
  Q_PROPERTY(CQChartsColor    headerBorderColor READ headerBorderColor WRITE setHeaderBorderColor)
  Q_PROPERTY(double           headerBorderAlpha READ headerBorderAlpha WRITE setHeaderBorderAlpha)
  Q_PROPERTY(CQChartsLength   headerBorderWidth READ headerBorderWidth WRITE setHeaderBorderWidth)
  Q_PROPERTY(CQChartsLineDash headerBorderDash  READ headerBorderDash  WRITE setHeaderBorderDash )

  Q_PROPERTY(bool          headerFilled      READ isHeaderFilled    WRITE setHeaderFilled     )
  Q_PROPERTY(CQChartsColor headerFillColor   READ headerFillColor   WRITE setHeaderFillColor  )
  Q_PROPERTY(double        headerFillAlpha   READ headerFillAlpha   WRITE setHeaderFillAlpha  )
  Q_PROPERTY(Pattern       headerFillPattern READ headerFillPattern WRITE setHeaderFillPattern)

  Q_PROPERTY(QFont         headerTextFont     READ headerTextFont       WRITE setHeaderTextFont    )
  Q_PROPERTY(CQChartsColor headerTextColor    READ headerTextColor      WRITE setHeaderTextColor   )
  Q_PROPERTY(double        headerTextAlpha    READ headerTextAlpha      WRITE setHeaderTextAlpha   )
  Q_PROPERTY(bool          headerTextContrast READ isHeaderTextContrast WRITE setHeaderTextContrast)
  Q_PROPERTY(Qt::Alignment headerTextAlign    READ headerTextAlign      WRITE setHeaderTextAlign   )

  // box margin, border, fill, font
  Q_PROPERTY(CQChartsLength marginWidth READ marginWidth WRITE setMarginWidth)

  Q_PROPERTY(bool             border      READ isBorder    WRITE setBorder     )
  Q_PROPERTY(CQChartsColor    borderColor READ borderColor WRITE setBorderColor)
  Q_PROPERTY(double           borderAlpha READ borderAlpha WRITE setBorderAlpha)
  Q_PROPERTY(CQChartsLength   borderWidth READ borderWidth WRITE setBorderWidth)
  Q_PROPERTY(CQChartsLineDash borderDash  READ borderDash  WRITE setBorderDash )

  Q_PROPERTY(bool          filled      READ isFilled    WRITE setFilled     )
  Q_PROPERTY(CQChartsColor fillColor   READ fillColor   WRITE setFillColor  )
  Q_PROPERTY(double        fillAlpha   READ fillAlpha   WRITE setFillAlpha  )
  Q_PROPERTY(Pattern       fillPattern READ fillPattern WRITE setFillPattern)

  Q_PROPERTY(QFont         textFont      READ textFont        WRITE setTextFont     )
  Q_PROPERTY(CQChartsColor textColor     READ textColor       WRITE setTextColor    )
  Q_PROPERTY(double        textAlpha     READ textAlpha       WRITE setTextAlpha    )
  Q_PROPERTY(bool          textContrast  READ isTextContrast  WRITE setTextContrast )
  Q_PROPERTY(Qt::Alignment textAlign     READ textAlign       WRITE setTextAlign    )
  Q_PROPERTY(bool          textFormatted READ isTextFormatted WRITE setTextFormatted)
  Q_PROPERTY(bool          textScaled    READ isTextScaled    WRITE setTextScaled   )

  // color map
  Q_PROPERTY(bool   colorMapped READ isColorMapped WRITE setColorMapped)
  Q_PROPERTY(double colorMapMin READ colorMapMin   WRITE setColorMapMin)
  Q_PROPERTY(double colorMapMax READ colorMapMax   WRITE setColorMapMax)

  Q_ENUMS(Pattern)

 public:
  enum class Pattern {
    SOLID,
    HATCH,
    DENSE,
    HORIZ,
    VERT,
    FDIAG,
    BDIAG
  };

  using OptColor = boost::optional<CQChartsColor>;

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

  //------

  // header border, fill, font
  bool isHeaderBorder() const;
  void setHeaderBorder(bool b);

  const CQChartsColor &headerBorderColor() const;
  void setHeaderBorderColor(const CQChartsColor &s);

  double headerBorderAlpha() const;
  void setHeaderBorderAlpha(double a);

  const CQChartsLength &headerBorderWidth() const;
  void setHeaderBorderWidth(const CQChartsLength &l);

  const CQChartsLineDash &headerBorderDash() const;
  void setHeaderBorderDash(const CQChartsLineDash &d);

  QColor interpHeaderBorderColor(int i, int n) const;

  //---

  bool isHeaderFilled() const;
  void setHeaderFilled(bool b);

  const CQChartsColor &headerFillColor() const;
  void setHeaderFillColor(const CQChartsColor &c);

  double headerFillAlpha() const;
  void setHeaderFillAlpha(double a);

  Pattern headerFillPattern() const;
  void setHeaderFillPattern(const Pattern &p);

  QColor interpHeaderFillColor(int i, int n) const;

  //---

  const QFont &headerTextFont() const;
  void setHeaderTextFont(const QFont &f);

  const CQChartsColor &headerTextColor() const;
  void setHeaderTextColor(const CQChartsColor &c);

  double headerTextAlpha() const;
  void setHeaderTextAlpha(double a);

  bool isHeaderTextContrast() const;
  void setHeaderTextContrast(bool b);

  const Qt::Alignment &headerTextAlign() const;
  void setHeaderTextAlign(const Qt::Alignment &a);

  void setHeaderTextFontSize(double s);

  QColor interpHeaderTextColor(int i, int n) const;

  //------

  // box margin, border, fill, font
  const CQChartsLength &marginWidth() const { return marginWidth_; }
  void setMarginWidth(const CQChartsLength &l);

  //---

  bool isBorder() const;
  void setBorder(bool b);

  const CQChartsColor &borderColor() const;
  void setBorderColor(const CQChartsColor &c);

  double borderAlpha() const;
  void setBorderAlpha(double a);

  const CQChartsLength &borderWidth() const;
  void setBorderWidth(const CQChartsLength &l);

  const CQChartsLineDash &borderDash() const;
  void setBorderDash(const CQChartsLineDash &d);

  QColor interpBorderColor(int i, int n) const;

  //---

  bool isFilled() const;
  void setFilled(bool b);

  const CQChartsColor &fillColor() const;
  void setFillColor(const CQChartsColor &c);

  double fillAlpha() const;
  void setFillAlpha(double a);

  Pattern fillPattern() const;
  void setFillPattern(Pattern pattern);

  QColor interpFillColor(int i, int n) const;

  //---

  const QFont &textFont() const;
  void setTextFont(const QFont &f);

  const CQChartsColor &textColor() const;
  void setTextColor(const CQChartsColor &c);

  double textAlpha() const;
  void setTextAlpha(double r);

  bool isTextContrast() const;
  void setTextContrast(bool b);

  const Qt::Alignment &textAlign() const;
  void setTextAlign(const Qt::Alignment &a);

  bool isTextFormatted() const;
  void setTextFormatted(bool b);

  bool isTextScaled() const;
  void setTextScaled(bool b);

  void setTextFontSize(double s);

  QColor interpTextColor(int i, int n) const;

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

  void updateRange(bool apply=true) override;

  void updateObjs() override;

  bool initObjs() override;

  //---

  void handleResize() override;

  //---

  void draw(QPainter *) override;

  //---

  bool addMenuItems(QMenu *menu) override;

 private:
  void initNodeObjs(CQChartsTreeMapHierNode *hier,
                    CQChartsTreeMapHierObj *parentObj, int depth);

  void resetNodes();

  void initNodes();

  void replaceNodes();

  void placeNodes(CQChartsTreeMapHierNode *hier);

  void colorNodes(CQChartsTreeMapHierNode *hier);

  void colorNode(CQChartsTreeMapNode *node);

  //---

  void loadHier();

  CQChartsTreeMapHierNode *addHierNode(CQChartsTreeMapHierNode *parent, const QString &name,
                                       const QModelIndex &nameInd);

  CQChartsTreeMapNode *addNode(CQChartsTreeMapHierNode *parent, const QString &name,
                               double size, const QModelIndex &nameInd);

  void loadFlat();

  CQChartsTreeMapNode *addNode(const QStringList &nameStrs, double size,
                               const QModelIndex &nameInd);

  void addExtraNodes(CQChartsTreeMapHierNode *hier);

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

  Node*             root_               { nullptr }; // root node
  Node*             firstHier_          { nullptr }; // first hier node
  QString           currentRootName_;                // current root name
  bool              titles_             { true };    // show title bar (header)
  double            titleMaxExtent_     { 0.5 };     // title bar max extent (0-1)
  CQChartsLength    headerHeight_       { 0.0 };     // header height (should be font based)
  CQChartsShapeData headerShapeData_;                // header fill/border data
  CQChartsTextData  headerTextData_;                 // header text data
  CQChartsLength    marginWidth_        { 2.0 };     // box margin
  CQChartsShapeData shapeData_;                      // box fill/border data
  CQChartsTextData  textData_;                       // box text data
  int               colorId_            { -1 };      // current color id
  int               numColorIds_        { 0 };       // num used color ids
  int               maxDepth_           { 1 };       // max hier depth
  int               hierInd_            { 0 };       // current hier ind
  double            windowHeaderHeight_ { 0.01 };    // calculated window pixel header height
  double            windowMarginWidth_  { 0.01 };    // calculated window pixel margin width
};

#endif
