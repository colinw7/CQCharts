#ifndef CQChartsTreeMapPlot_H
#define CQChartsTreeMapPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDisplayRange.h>
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

  bool inside(const CQChartsGeom::Point &p) const override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

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

  bool inside(const CQChartsGeom::Point &p) const override;

  void clickZoom(const CQChartsGeom::Point &p) override;

  void mousePress(const CQChartsGeom::Point &p) override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

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

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsTreeMapPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int     nameColumn   READ nameColumn   WRITE setNameColumn  )
  Q_PROPERTY(int     valueColumn  READ valueColumn  WRITE setValueColumn )
  Q_PROPERTY(QString separator    READ separator    WRITE setSeparator   )
  Q_PROPERTY(bool    titles       READ isTitles     WRITE setTitles      )
  Q_PROPERTY(double  fontHeight   READ fontHeight   WRITE setFontHeight  )
  Q_PROPERTY(double  headerHeight READ headerHeight WRITE setHeaderHeight)
  Q_PROPERTY(QColor  headerColor  READ headerColor  WRITE setHeaderColor )
  Q_PROPERTY(double  marginWidth  READ marginWidth  WRITE setMarginWidth )

 public:
  using Nodes = std::vector<CQChartsTreeMapNode*>;

 public:
  CQChartsTreeMapPlot(CQChartsView *view, const ModelP &model);

  //---

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; update(); }

  //---

  const QString &separator() const { return separator_; }
  void setSeparator(const QString &s) { separator_ = s; }

  bool isTitles() const { return titles_; }
  void setTitles(bool b) { titles_ = b; updateCurrentRoot(); }

  double fontHeight() const { return fontHeight_; }
  void setFontHeight(double r) { fontHeight_ = r; update(); }

  double headerHeight() const { return headerHeight_; }
  void setHeaderHeight(double r) { headerHeight_ = r; updateCurrentRoot(); }

  const QColor &headerColor() const { return headerColor_; }
  void setHeaderColor(const QColor &v) { headerColor_ = v; }

  double marginWidth() const { return marginWidth_; }
  void setMarginWidth(double r) { marginWidth_ = r; updateCurrentRoot(); }

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

  void initObjs() override;

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

  void draw(QPainter *) override;

  QColor nodeColor(CQChartsTreeMapNode *node) const;

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
  int                      nameColumn_         { 0 };
  int                      valueColumn_        { 1 };
  CQChartsDisplayRange          range_;
  CQChartsTreeMapHierNode* root_               { nullptr };
  CQChartsTreeMapHierNode* firstHier_          { nullptr };
  CQChartsTreeMapHierNode* currentRoot_        { nullptr };
  QString                  separator_          { "/" };
  bool                     titles_             { true };
  double                   fontHeight_         { 9.0 };
  double                   headerHeight_       { 11.0 };
  QColor                   headerColor_        { 128, 128, 128 };
  double                   marginWidth_        { 2.0 };
  int                      maxDepth_           { 1 };
  int                      maxColorId_         { 0 };
  int                      hierInd_            { 0 };
  int                      maxHierInd_         { 0 };
  double                   windowHeaderHeight_ { 0.01 };
  double                   windowMarginWidth_  { 0.01 };
};

#endif
