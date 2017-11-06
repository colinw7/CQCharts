#ifndef CQChartsTreeMapPlot_H
#define CQChartsTreeMapPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <COSNaN.h>
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
  CQChartsTreeMapNode(CQChartsTreeMapHierNode *parent, const QString &name, double size,
                      const QModelIndex &ind) :
   parent_(parent), id_(nextId()), name_(name), size_(size), ind_(ind) {
  }

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

  virtual void setPosition(double x, double y, double w, double h) {
    assert(! COSNaN::is_nan(x) && ! COSNaN::is_nan(y) &&
           ! COSNaN::is_nan(w) && ! COSNaN::is_nan(h));

    x_ = x; y_ = y;
    w_ = w; h_ = h;

    //std::cerr << "Node: " << name() << " @ ( " << x_ << "," << y_ << ")" <<
    //             " [" << w_ << "," << h_ << "]" << "\n";

    placed_ = true;
  }

  virtual bool contains(double x, double y) {
    return (x >= x_ && x <= (x_ + w_) && y >= y_ && y <= (y_ + h_));
  }

  virtual bool placed() const { return placed_; }

  friend bool operator<(const CQChartsTreeMapNode &n1, const CQChartsTreeMapNode &n2) {
    return n1.size_ < n2.size_;
  }

 protected:
  CQChartsTreeMapHierNode *parent_  { nullptr };
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
  CQChartsTreeMapHierNode(CQChartsTreeMapHierNode *parent=nullptr, const QString &name="") :
   CQChartsTreeMapNode(parent, name, 0.0, QModelIndex()) {
    if (parent_)
      parent_->children_.push_back(this);
  }

  double size() const {
    double s = 0.0;

    for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
      s += (*p)->size();

    for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn)
     s += (*pn)->size();

    return s;
  }

  const Nodes &getNodes() const { return nodes_; }

  const Children &getChildren() const { return children_; }

  void packNodes(double x, double y, double w, double h) {
    // make single list of nodes to pack
    Nodes nodes;

    for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
      nodes.push_back(*p);

    for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn)
      nodes.push_back(*pn);

    // sort nodes by size
    std::sort(nodes.begin(), nodes.end(), CQChartsTreeMapNodeCmp());

    //std::cerr << name() << "\n";
    //for (uint i = 0; i < nodes.size(); ++i)
    //  std::cerr << " " << nodes[i]->name() << ":" << nodes[i]->size() << "\n";

    packSubNodes(x, y, w, h, nodes);
  }

  void packSubNodes(double x, double y, double w, double h, const Nodes &nodes) {
    // place nodes
    int n = nodes.size();
    if (n == 0) return;

    if (n >= 2) {
      int n1 = n/2;

      Nodes  nodes1, nodes2;
      double size1 = 0.0, size2 = 0.0;

      for (int i = 0; i < n1; ++i) {
        size1 += nodes[i]->size();

        nodes1.push_back(nodes[i]);
      }

      for (int i = n1; i <  n; ++i) {
        size2 += nodes[i]->size();

        nodes2.push_back(nodes[i]);
      }

      // split area = (w*h) if largest direction
      // e.g. split at w1. area1 = w1*h; area2 = (w - w1)*h;
      // area1/area2 = w1/(w - w1) = size1/size2;
      // w1*size2 = w*size1 - w1*size1;
      // w1 = (w*size1)/(size1 + size2);

      double size12 = size1 + size2;

      if (size12 == 0.0)
        return;

      double f = size1/size12;

      if (w >= h) {
        double w1 = f*w;

        packSubNodes(x     , y,     w1, h, nodes1);
        packSubNodes(x + w1, y, w - w1, h, nodes2);
      }
      else {
        double h1 = f*h;

        packSubNodes(x, y     , w,     h1, nodes1);
        packSubNodes(x, y + h1, w, h - h1, nodes2);
      }
    }
    else {
      CQChartsTreeMapNode *node = nodes[0];

      node->setPosition(x, y, w, h);
    }
  }

  void setPosition(double x, double y, double w, double h) {
    CQChartsTreeMapNode::setPosition(x, y, w, h);

    packNodes(x, y, w, h);
  }

  void addNode(CQChartsTreeMapNode *node) {
    nodes_.push_back(node);
  }

 private:
  Nodes    nodes_;
  Children children_;
};

//---

class CQChartsTreeMapHierObj : public CQChartsPlotObj {
 public:
  CQChartsTreeMapHierObj(CQChartsTreeMapPlot *plot, CQChartsTreeMapHierNode *hier,
                         CQChartsTreeMapHierObj *hierObj, const CBBox2D &rect, int i, int n);

  bool inside(const CPoint2D &p) const override;

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
                     CQChartsTreeMapHierObj *hierObj, const CBBox2D &rect, int i, int n);

  bool inside(const CPoint2D &p) const override;

  void clickZoom(const CPoint2D &p) override;

  void mousePress(const CPoint2D &p) override;

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

  Q_PROPERTY(int     nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(int     valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(QString separator   READ separator   WRITE setSeparator  )
  Q_PROPERTY(double  fontHeight  READ fontHeight  WRITE setFontHeight )

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

  double fontHeight() const { return fontHeight_; }
  void setFontHeight(double r) { fontHeight_ = r; update(); }

  //---

  CQChartsTreeMapHierNode *root() const { return root_; }

  CQChartsTreeMapHierNode *currentRoot() const { return currentRoot_; }
  void setCurrentRoot(CQChartsTreeMapHierNode *r);

  //---

  int maxDepth() const { return maxDepth_; }

  int maxColorId() const { return maxColorId_; }

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

  void draw(QPainter *) override;

  QColor nodeColor(CQChartsTreeMapNode *node) const;

 private:
  void initNodes();

  void placeNodes(CQChartsTreeMapHierNode *hier);

  bool isHier() const;

  void loadChildren(CQChartsTreeMapHierNode *hier, const QModelIndex &index=QModelIndex(),
                    int depth=0, int colorId=-1);

  void loadFlat();

  void transformNodes(CQChartsTreeMapHierNode *hier);

 public slots:
  void updateCurrentRoot();

 private:
  int                      nameColumn_  { 0 };
  int                      valueColumn_ { 1 };
  CDisplayRange2D          range_;
  CQChartsTreeMapHierNode* root_        { nullptr };
  CQChartsTreeMapHierNode* currentRoot_ { nullptr };
  QString                  separator_   { "/" };
  double                   fontHeight_  { 9.0 };
  int                      maxDepth_    { 1 };
  int                      maxColorId_  { 0 };
};

#endif
