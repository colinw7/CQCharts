#ifndef CQChartsTreeMapPlot_H
#define CQChartsTreeMapPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsPaletteColor.h>
#include <CQChartsColorSet.h>
#include <QModelIndex>

class CQChartsTreeMapPlot;
class CQChartsTreeMapHierNode;
class CQChartsTextBoxObj;

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

  const CQChartsPaletteColor &color() const { return color_; }
  void setColor(const CQChartsPaletteColor &v) { color_ = v; }

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
  CQChartsPaletteColor     color_   { };         // node explicit color
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

  bool hasChildren() const { return ! children_.empty(); }

  const Children &getChildren() const { return children_; }

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

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

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

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsTreeMapHierNode* hier_    { nullptr };
};

//---

class CQChartsTreeMapPlotType : public CQChartsPlotType {
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

  Q_PROPERTY(bool    titles             READ isTitles             WRITE setTitles              )
  Q_PROPERTY(double  headerHeight       READ headerHeight         WRITE setHeaderHeight        )
  Q_PROPERTY(bool    headerBorder       READ isHeaderBorder       WRITE setHeaderBorder        )
  Q_PROPERTY(QString headerBorderColor  READ headerBorderColorStr WRITE setHeaderBorderColorStr)
  Q_PROPERTY(double  headerBorderAlpha  READ headerBorderAlpha    WRITE setHeaderBorderAlpha   )
  Q_PROPERTY(double  headerBorderWidth  READ headerBorderWidth    WRITE setHeaderBorderWidth   )
  Q_PROPERTY(bool    headerFilled       READ isHeaderFilled       WRITE setHeaderFilled        )
  Q_PROPERTY(QString headerFillColor    READ headerFillColorStr   WRITE setHeaderFillColorStr  )
  Q_PROPERTY(double  headerFillAlpha    READ headerFillAlpha      WRITE setHeaderFillAlpha     )
  Q_PROPERTY(QFont   headerTextFont     READ headerTextFont       WRITE setHeaderTextFont      )
  Q_PROPERTY(QString headerTextColor    READ headerTextColorStr   WRITE setHeaderTextColorStr  )
  Q_PROPERTY(bool    headerTextContrast READ isHeaderTextContrast WRITE setHeaderTextContrast  )
  Q_PROPERTY(double  marginWidth        READ marginWidth          WRITE setMarginWidth         )
  Q_PROPERTY(bool    border             READ isBorder             WRITE setBorder              )
  Q_PROPERTY(QString borderColor        READ borderColorStr       WRITE setBorderColorStr      )
  Q_PROPERTY(double  borderAlpha        READ borderAlpha          WRITE setBorderAlpha         )
  Q_PROPERTY(double  borderWidth        READ borderWidth          WRITE setBorderWidth         )
  Q_PROPERTY(bool    filled             READ isFilled             WRITE setFilled              )
  Q_PROPERTY(QString fillColor          READ fillColorStr         WRITE setFillColorStr        )
  Q_PROPERTY(double  fillAlpha          READ fillAlpha            WRITE setFillAlpha           )
  Q_PROPERTY(Pattern fillPattern        READ fillPattern          WRITE setFillPattern         )
  Q_PROPERTY(QFont   textFont           READ textFont             WRITE setTextFont            )
  Q_PROPERTY(QString textColor          READ textColorStr         WRITE setTextColorStr        )
  Q_PROPERTY(bool    textContrast       READ isTextContrast       WRITE setTextContrast        )
  Q_PROPERTY(bool    colorMapEnabled    READ isColorMapEnabled    WRITE setColorMapEnabled     )
  Q_PROPERTY(double  colorMapMin        READ colorMapMin          WRITE setColorMapMin         )
  Q_PROPERTY(double  colorMapMax        READ colorMapMax          WRITE setColorMapMax         )

  Q_ENUMS(Pattern);

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

  using OptColor = boost::optional<CQChartsPaletteColor>;

  using Nodes = std::vector<CQChartsTreeMapNode*>;

 public:
  CQChartsTreeMapPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsTreeMapPlot();

  //---

  bool isTitles() const { return titles_; }
  void setTitles(bool b) { titles_ = b; updateCurrentRoot(); }

  double headerHeight() const { return headerHeight_; }
  void setHeaderHeight(double r) { headerHeight_ = r; updateCurrentRoot(); }

  double calcHeaderHeight() const;

  //---

  bool isHeaderBorder() const;
  void setHeaderBorder(bool b);

  QString headerBorderColorStr() const;
  void setHeaderBorderColorStr(const QString &s);

  QColor interpHeaderBorderColor(int i, int n) const;

  double headerBorderAlpha() const;
  void setHeaderBorderAlpha(double a);

  double headerBorderWidth() const;
  void setHeaderBorderWidth(double r);

  //---

  bool isHeaderFilled() const;
  void setHeaderFilled(bool b);

  QString headerFillColorStr() const;
  void setHeaderFillColorStr(const QString &s);

  QColor interpHeaderFillColor(int i, int n) const;

  double headerFillAlpha() const;
  void setHeaderFillAlpha(double a);

  //---

  const QFont &headerTextFont() const;
  void setHeaderTextFont(const QFont &f);

  QString headerTextColorStr() const;
  void setHeaderTextColorStr(const QString &s);

  QColor interpHeaderTextColor(int i, int n) const;

  bool isHeaderTextContrast() const;
  void setHeaderTextContrast(bool b);

  //---

  double marginWidth() const { return marginWidth_; }
  void setMarginWidth(double r) { marginWidth_ = r; updateCurrentRoot(); }

  //---

  bool isBorder() const;
  void setBorder(bool b);

  QString borderColorStr() const;
  void setBorderColorStr(const QString &str);

  QColor interpBorderColor(int i, int n) const;

  double borderAlpha() const;
  void setBorderAlpha(double a);

  double borderWidth() const;
  void setBorderWidth(double r);

  //---

  bool isFilled() const;
  void setFilled(bool b);

  QString fillColorStr() const;
  void setFillColorStr(const QString &s);

  QColor interpFillColor(int i, int n) const;

  double fillAlpha() const;
  void setFillAlpha(double a);

  Pattern fillPattern() const;
  void setFillPattern(Pattern pattern);

  //---

  const QFont &textFont() const;
  void setTextFont(const QFont &f);

  QString textColorStr() const;
  void setTextColorStr(const QString &s);

  QColor interpTextColor(int i, int n) const;

  bool isTextContrast() const;
  void setTextContrast(bool b);

  //---

  bool isColorMapEnabled() const { return colorSet_.isMapEnabled(); }
  void setColorMapEnabled(bool b) { colorSet_.setMapEnabled(b); updateObjs(); }

  double colorMapMin() const { return colorSet_.mapMin(); }
  void setColorMapMin(double r) { colorSet_.setMapMin(r); updateObjs(); }

  double colorMapMax() const { return colorSet_.mapMax(); }
  void setColorMapMax(double r) { colorSet_.setMapMax(r); updateObjs(); }

  //---

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

  void initColorSet();

  bool colorSetColor(int i, OptColor &color);

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

  void loadChildren(CQChartsTreeMapHierNode *hier, const QModelIndex &index=QModelIndex(),
                    int depth=0);

  void loadFlat();

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
  CQChartsTreeMapHierNode* root_               { nullptr }; // root node
  CQChartsTreeMapHierNode* firstHier_          { nullptr }; // first hier node
  QString                  currentRootName_;                // current root name
  bool                     titles_             { true };    // show titles
  double                   headerHeight_       { -1 };      // header height (should be font based)
  CQChartsTextBoxObj*      headerTextBoxObj_   { nullptr }; // header fill/border/text object
  double                   marginWidth_        { 2.0 };     // box margin
  CQChartsTextBoxObj*      textBoxObj_         { nullptr }; // bubble fill/border/text object
  CQChartsColorSet         colorSet_;                       // color value set
  int                      colorId_            { -1 };      // current color id
  int                      numColorIds_        { 0 };       // num used color ids
  int                      maxDepth_           { 1 };       // max hier depth
  int                      hierInd_            { 0 };       // current hier ind
  double                   windowHeaderHeight_ { 0.01 };    // window header height in pixels
  double                   windowMarginWidth_  { 0.01 };    // window margin width in pixels
};

#endif
