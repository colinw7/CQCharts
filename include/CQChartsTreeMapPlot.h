#ifndef CQChartsTreeMapPlot_H
#define CQChartsTreeMapPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsPaletteColor.h>
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
  CQChartsTreeMapNode(CQChartsTreeMapHierNode *parent, const QString &name,
                      double size, const QModelIndex &ind);

  virtual ~CQChartsTreeMapNode() { }

  CQChartsTreeMapHierNode *parent() const { return parent_; }

  virtual uint id() const { return id_; }

  virtual const QString &name() const { return name_; }

  virtual double size() const { return size_; }

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

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  virtual int depth() const { return depth_; }
  virtual void setDepth(int i) { depth_ = i; }

  virtual void setPosition(double x, double y, double w, double h);

  virtual bool contains(double x, double y) const;

  virtual bool placed() const { return placed_; }

  CQChartsTreeMapHierNode *rootNode(CQChartsTreeMapHierNode *root) const;

  friend bool operator<(const CQChartsTreeMapNode &n1, const CQChartsTreeMapNode &n2) {
    return n1.size_ < n2.size_;
  }

 protected:
  CQChartsTreeMapHierNode* parent_  { nullptr };
  uint                     id_      { 0 };
  QString                  name_;
  double                   size_    { 0.0 };
  double                   x_       { 0.0 };
  double                   y_       { 0.0 };
  double                   w_       { 1.0 };
  double                   h_       { 1.0 };
  int                      colorId_ { 0 };
  QModelIndex              ind_;
  int                      depth_   { 0 };
  bool                     placed_  { false };
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

  CQChartsTreeMapPlot *plot() const { return plot_; }

  int hierInd() const { return hierInd_; }
  void setHierInd(int i) { hierInd_ = i; }

  double size() const;

  const Nodes &getNodes() const { return nodes_; }

  const Children &getChildren() const { return children_; }

  void packNodes(double x, double y, double w, double h);

  void packSubNodes(double x, double y, double w, double h, const Nodes &nodes);

  void setPosition(double x, double y, double w, double h);

  void addNode(CQChartsTreeMapNode *node);

 private:
  CQChartsTreeMapPlot* plot_    { nullptr };
  Nodes                nodes_;
  Children             children_;
  int                  hierInd_ { -1 };
};

//---

class CQChartsTreeMapHierObj : public CQChartsPlotObj {
 public:
  CQChartsTreeMapHierObj(CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *hier,
                         CQChartsTreeMapHierObj *hierObj, const CQChartsGeom::BBox &rect,
                         int i, int n);

  QString calcId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  bool isIndex(const QModelIndex &) const override;

  void draw(CQChartsRenderer *renderer, const CQChartsPlot::Layer &) override;

 private:
  CQChartsTreeMapPlot*     plot_    { nullptr };
  CQChartsTreeMapHierNode* hier_    { nullptr };
  CQChartsTreeMapHierObj*  hierObj_ { nullptr };
  int                      i_       { 0 };
  int                      n_       { 0 };
};

//---

class CQChartsTreeMapObj : public CQChartsPlotObj {
 public:
  CQChartsTreeMapObj(CQChartsTreeMapPlot *plot, CQChartsTreeMapNode *node,
                     CQChartsTreeMapHierObj *hierObj, const CQChartsGeom::BBox &rect,
                     int i, int n);

  QString calcId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void clickZoom(const CQChartsGeom::Point &p) override;

  void mousePress(const CQChartsGeom::Point &p) override;

  bool isIndex(const QModelIndex &) const override;

  void draw(CQChartsRenderer *renderer, const CQChartsPlot::Layer &) override;

 private:
  CQChartsTreeMapPlot*    plot_    { nullptr };
  CQChartsTreeMapNode*    node_    { nullptr };
  CQChartsTreeMapHierObj* hierObj_ { nullptr };
  int                     i_       { 0 };
  int                     n_       { 0 };
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

class CQChartsTreeMapPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int     nameColumn   READ nameColumn     WRITE setNameColumn    )
  Q_PROPERTY(int     valueColumn  READ valueColumn    WRITE setValueColumn   )
  Q_PROPERTY(QString separator    READ separator      WRITE setSeparator     )
  Q_PROPERTY(bool    titles       READ isTitles       WRITE setTitles        )
  Q_PROPERTY(double  headerHeight READ headerHeight   WRITE setHeaderHeight  )
  Q_PROPERTY(QString headerColor  READ headerColorStr WRITE setHeaderColorStr)
  Q_PROPERTY(double  marginWidth  READ marginWidth    WRITE setMarginWidth   )
  Q_PROPERTY(bool    border       READ isBorder       WRITE setBorder        )
  Q_PROPERTY(QString borderColor  READ borderColorStr WRITE setBorderColorStr)
  Q_PROPERTY(double  borderAlpha  READ borderAlpha    WRITE setBorderAlpha   )
  Q_PROPERTY(double  borderWidth  READ borderWidth    WRITE setBorderWidth   )
  Q_PROPERTY(QString fillColor    READ fillColorStr   WRITE setFillColorStr  )
  Q_PROPERTY(double  fillAlpha    READ fillAlpha      WRITE setFillAlpha     )
  Q_PROPERTY(QFont   textFont     READ textFont       WRITE setTextFont      )
  Q_PROPERTY(QString textColor    READ textColorStr   WRITE setTextColorStr  )

 public:
  using Nodes = std::vector<CQChartsTreeMapNode*>;

 public:
  CQChartsTreeMapPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsTreeMapPlot();

  //---

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; updateRangeAndObjs(); }

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; updateRangeAndObjs(); }

  //---

  const QString &separator() const { return separator_; }
  void setSeparator(const QString &s) { separator_ = s; }

  bool isTitles() const { return titles_; }
  void setTitles(bool b) { titles_ = b; updateCurrentRoot(); }

  double headerHeight() const { return headerHeight_; }
  void setHeaderHeight(double r) { headerHeight_ = r; updateCurrentRoot(); }

  QString headerColorStr() const { return headerColor_.colorStr(); }
  void setHeaderColorStr(const QString &s) { headerColor_.setColorStr(s); }

  QColor interpHeaderColor(int i, int n) const { return headerColor_.interpColor(this, i, n); }

  double marginWidth() const { return marginWidth_; }
  void setMarginWidth(double r) { marginWidth_ = r; updateCurrentRoot(); }

  bool isBorder() const;
  void setBorder(bool b);

  QString borderColorStr() const;
  void setBorderColorStr(const QString &str);

  QColor interpBorderColor(int i, int n) const;

  double borderAlpha() const;
  void setBorderAlpha(double a);

  double borderWidth() const;
  void setBorderWidth(double r);

  QString fillColorStr() const;
  void setFillColorStr(const QString &s);

  QColor interpFillColor(int i, int n) const;

  double fillAlpha() const;
  void setFillAlpha(double a);

  const QFont &textFont() const { return textFont_; }
  void setTextFont(const QFont &f) { textFont_ = f; update(); }

  QString textColorStr() const { return textColor_.colorStr(); }
  void setTextColorStr(const QString &s) { textColor_.setColorStr(s); update(); }

  QColor interpTextColor(int i, int n) const;

  //---

  CQChartsTreeMapHierNode *root() const { return root_; }

  CQChartsTreeMapHierNode *firstHier() const { return firstHier_; }

  CQChartsTreeMapHierNode *currentRoot() const { return currentRoot_; }
  void setCurrentRoot(CQChartsTreeMapHierNode *r);

  //---

  int maxDepth() const { return maxDepth_; }

  int maxColorId() const { return maxColorId_; }

  int maxHierInd() const { return maxHierInd_; }

  double windowHeaderHeight() const { return windowHeaderHeight_; }
  double windowMarginWidth () const { return windowMarginWidth_ ; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  //---

  void initNodeObjs(CQChartsTreeMapHierNode *hier,
                    CQChartsTreeMapHierObj *parentObj, int depth);

  //---

  CQChartsTreeMapHierNode *childHierNode(CQChartsTreeMapHierNode *parent,
                                         const QString &name) const;
  CQChartsTreeMapNode *childNode(CQChartsTreeMapHierNode *parent,
                                 const QString &name) const;

  //---

  void zoomFull() override;

  //---

  void handleResize();

  void draw(CQChartsRenderer *) override;

 private:
  void initNodes();

  void replaceNodes();

  void placeNodes(CQChartsTreeMapHierNode *hier);

  bool isHier() const;

  void loadChildren(CQChartsTreeMapHierNode *hier, const QModelIndex &index=QModelIndex(),
                    int depth=0, int colorId=-1);

  void loadFlat();

  void transformNodes(CQChartsTreeMapHierNode *hier);

 public slots:
  void updateCurrentRoot();

 private:
  int                      nameColumn_         { 0 };       // name column
  int                      valueColumn_        { 1 };       // value column
  CQChartsTreeMapHierNode* root_               { nullptr }; // root node
  CQChartsTreeMapHierNode* firstHier_          { nullptr }; // first hier node
  CQChartsTreeMapHierNode* currentRoot_        { nullptr }; // current root
  QString                  separator_          { "/" };     // hier name separator
  bool                     titles_             { true };    // show titles
  double                   headerHeight_       { 11.0 };    // header height (should be font based)
  CQChartsPaletteColor     headerColor_;                    // header color
  double                   marginWidth_        { 2.0 };     // box margin
  CQChartsBoxObj*          boxObj_             { nullptr }; // bubble fill/border object
  QFont                    textFont_;                       // text font
  CQChartsPaletteColor     textColor_;                      // text color
  int                      maxDepth_           { 1 };       // max hier depth
  int                      maxColorId_         { 0 };       // max color
  int                      hierInd_            { 0 };       // current hier ind
  int                      maxHierInd_         { 0 };       // max hier ind
  double                   windowHeaderHeight_ { 0.01 };    // window header height in pixels
  double                   windowMarginWidth_  { 0.01 };    // window margin width in pixels
};

#endif
