#ifndef CQChartsSunburstPlot_H
#define CQChartsSunburstPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <QModelIndex>

class CQChartsSunburstPlot;
class CQChartsSunburstRootNode;
class CQChartsSunburstHierNode;
class CQChartsSunburstNode;

//---

class CQChartsSunburstNodeObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsSunburstNodeObj(CQChartsSunburstPlot *plot, const CBBox2D &rect,
                          CQChartsSunburstNode *node);

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsSunburstPlot *plot_ { nullptr };
  CQChartsSunburstNode *node_ { nullptr };
};

//---

class CQChartsSunburstNode {
 private:
  static uint nextId() {
    static int lastId = 0;

    return ++lastId;
  }

 public:
  CQChartsSunburstNode(CQChartsSunburstHierNode *parent, const QString &name="") :
   parent_(parent), id_(nextId()), name_(name) {
  }

  virtual ~CQChartsSunburstNode() { }

  CQChartsSunburstHierNode *parent() const { return parent_; }

  uint id() const { return id_; }

  const QString &name() const { return name_; }

  virtual double size() const { return size_; }
  void setSize(double size) { size_ = size; }

  virtual int depth() const { return 1; }

  virtual int numNodes() const { return 1; }

  double r () const { return r_ ; }
  double a () const { return a_ ; }
  double dr() const { return dr_; }
  double da() const { return da_; }

  const int &colorId() const { return colorId_; }
  virtual void setColorId(int colorId) { colorId_ = colorId; }

  virtual void setPosition(double r, double a, double dr, double da) {
    r_  = r ; a_  = a ;
    dr_ = dr; da_ = da;

    placed_ = true;
  }

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

  bool pointInside(double x, double y) {
    if (! placed_) return false;

    double r = sqrt(x*x + y*y);

    if (r < r_ || r > r_ + dr_) return false;

    double a = normalizeAngle(180.0*atan2(y, x)/M_PI);

    double a1 = normalizeAngle(a_);
    double a2 = a1 + da_;

    if (a2 > a1) {
      if (a2 >= 360.0) {
        double da = a2 - 360.0; a -= da; a1 -= da; a2 = 360.0;
        a = normalizeAngle(a);
      }

      if (a < a1 || a > a2)
        return false;
    }
    else {
      if (a2 < 0.0) {
        double da = -a2; a += da; a1 += da; a2 = 0.0;

        a = normalizeAngle(a);
      }

      if (a < a2 || a > a1)
        return false;
    }

    return true;
  }

 protected:
  CQChartsSunburstHierNode* parent_  { nullptr };
  uint                      id_      { 0 };
  QString                   name_;
  double                    size_    { 1.0 };
  double                    r_       { 0.0 };
  double                    a_       { 0.0 };
  double                    dr_      { 0.0 };
  double                    da_      { 0.0 };
  int                       colorId_ { 0 };
  bool                      placed_  { false };
  CQChartsSunburstNodeObj*  obj_     { nullptr };
};

//---

struct CQChartsSunburstNodeNameCmp {
  // sort reverse alphabetic no case
  bool operator()(const CQChartsSunburstNode *n1, const CQChartsSunburstNode *n2) {
    const QString &name1 = n1->name();
    const QString &name2 = n2->name();

    int l1 = name1.size();
    int l2 = name2.size();

    for (int i = 0; i < std::max(l1, l2); ++i) {
      char c1 = (i < l1 ? tolower(name1[i].toLatin1()) : '\0');
      char c2 = (i < l2 ? tolower(name2[i].toLatin1()) : '\0');

      if (c1 > c2) return true;
      if (c1 < c2) return false;
    }

    return false;
  }
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

class CQChartsSunburstHierNode : public CQChartsSunburstNode {
 public:
  enum class Order {
    SIZE,
    COUNT
  };

 public:
  typedef std::vector<CQChartsSunburstNode *>     Nodes;
  typedef std::vector<CQChartsSunburstHierNode *> Children;

 public:
  CQChartsSunburstHierNode(CQChartsSunburstHierNode *parent=0, const QString &name="") :
   CQChartsSunburstNode(parent, name) {
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

  int depth() const {
    int depth = 1;

    for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
      depth = std::max(depth, (*p)->depth() + 1);

    return depth;
  }

  int numNodes() const {
    int num = nodes_.size();

    for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
      num += (*p)->numNodes();

    return std::max(num, 1);
  }

  const Nodes &getNodes() const { return nodes_; }

  const Children &getChildren() const { return children_; }

  void unplace() {
    unplaceNodes();
  }

  void unplaceNodes() {
    CQChartsSunburstNode::unplace();

    for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
      (*p)->unplaceNodes();

    for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn)
      (*pn)->unplace();
  }

  void packNodes(CQChartsSunburstHierNode *root, double ri, double ro,
                 double dr, double a, double da, const Order &order, bool sort) {
    int d = depth();

    if (dr <= 0.0)
      dr = (ro - ri)/d;

    double s = (order == Order::SIZE ? size() : numNodes());

    double da1 = da/s;

    packSubNodes(root, ri, dr, a, da1, order, sort);
  }

  void packSubNodes(CQChartsSunburstHierNode *root, double ri,
                    double dr, double a, double da, const Order &order, bool sort) {
    // make single list of nodes to pack
    Nodes nodes;

    for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
      nodes.push_back(*p);

    for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn)
      nodes.push_back(*pn);

    if (sort) {
#if 0
      if (root->order() == Order::SIZE)
        std::sort(nodes.begin(), nodes.end(), CQChartsSunburstNodeSizeCmp());
      else
        std::sort(nodes.begin(), nodes.end(), CQChartsSunburstNodeCountCmp());
#else
      std::sort(nodes.begin(), nodes.end(), CQChartsSunburstNodeNameCmp());
    }
#endif

    //---

    placed_ = true;

    // place each node
    double a1 = a;

    for (Nodes::const_iterator pn = nodes.begin(); pn != nodes.end(); ++pn) {
      CQChartsSunburstNode *node = *pn;

      double s = (order == Order::SIZE ? node->size() : node->numNodes());

      node->setPosition(ri, a1, dr, s*da);

      CQChartsSunburstHierNode *hierNode = dynamic_cast<CQChartsSunburstHierNode *>(node);

      if (hierNode)
        hierNode->packSubNodes(root, ri + dr, dr, a1, da, order, sort);

      a1 += s*da;
    }
  }

  void addNode(CQChartsSunburstNode *node) {
    nodes_.push_back(node);
  }

 private:
  Nodes     nodes_;
  Children  children_;
};

//---

class CQChartsSunburstRootNode : public CQChartsSunburstHierNode {
 public:
  CQChartsSunburstRootNode(const QString &name="") :
   CQChartsSunburstHierNode(0, name), sort_(true), order_(Order::SIZE) {
  }

  bool sort() const { return sort_; }
  void setSort(bool sort) { sort_ = sort; }

  Order order() const { return order_; }
  void setOrder(Order order) { order_ = order; }

  void packNodes(double ri=0.5, double ro=1.0, double dr=0.0, double a=0.0, double da=360.0) {
    CQChartsSunburstHierNode::packNodes(this, ri, ro, dr, a, da, order(), sort());
  }

 private:
  bool  sort_  { true };
  Order order_ { Order::SIZE };
};

//---

class CQChartsSunburstPlotType : public CQChartsPlotType {
 public:
  CQChartsSunburstPlotType();

  QString name() const override { return "sunburst"; }
  QString desc() const override { return "Sunburst"; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsSunburstPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int    nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(int    valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(double fontHeight  READ fontHeight  WRITE setFontHeight )

 public:
  CQChartsSunburstPlot(CQChartsView *view, const ModelP &model);

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; update(); }

  double fontHeight() const { return fontHeight_; }
  void setFontHeight(double r) { fontHeight_ = r; update(); }

  //---

  void addProperties() override;

  void updateRange() override;

  void initObjs(bool force=false) override;

  //---

  void handleResize() override;

  void draw(QPainter *) override;

  void drawNode(QPainter *p, CQChartsSunburstNode *node);

 private:
  void loadChildren(CQChartsSunburstHierNode *hier, const QModelIndex &index,
                    int depth, int colorId=-1);

  void addPlotObjs(CQChartsSunburstHierNode *parent);

  void addPlotObj(CQChartsSunburstNode *node);

  void drawNodes(QPainter *p, CQChartsSunburstHierNode *hier);

  QColor nodeColor(CQChartsSunburstNode *node) const;

  QColor nodeColor(int colorId) const;

 private:
  typedef std::vector<CQChartsSunburstRootNode *> RootNodes;

  int       nameColumn_  { 0 };
  int       valueColumn_ { 1 };
  RootNodes roots_;
  double    fontHeight_  { 6.0 };
};

#endif
