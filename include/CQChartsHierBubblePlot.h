#ifndef CQChartsHierBubblePlot_H
#define CQChartsHierBubblePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CirclePack.h>
#include <QModelIndex>

class CQChartsHierBubblePlot;

class CQChartsHierBubbleNode : public CircleNode {
 private:
  static uint nextId() {
    static int lastId = 0;

    return ++lastId;
  }

 public:
  CQChartsHierBubbleNode(const std::string &name="", double size=1.0, int colorId=0) :
   id_(nextId()), name_(name), size_(size), colorId_(colorId) {
    r_ = sqrt(size_/(2*M_PI));
  }

  virtual ~CQChartsHierBubbleNode() { }

  virtual uint id() const { return id_; }

  virtual const std::string &name() const { return name_; }

  virtual double size() const { return size_; }

  virtual double x() const override { return x_; }
  virtual void setX(double x) { x_ = x; }

  virtual double y() const override { return y_; }
  virtual void setY(double y) { y_ = y; }

  virtual int colorId() const { return colorId_; }

  virtual double radius() const override { return r_; }
  virtual void setRadius(double r) { r_ = r; }

  virtual void setPosition(double x, double y) override {
    x_ = x;
    y_ = y;

    placed_ = true;
  }

  virtual bool placed() const { return placed_; }

  friend bool operator<(const CQChartsHierBubbleNode &n1, const CQChartsHierBubbleNode &n2) {
    return n1.r_ < n2.r_;
  }

 protected:
  uint        id_      { 0 };
  std::string name_;
  double      size_    { 1.0 };
  int         colorId_ { 0 };
  bool        placed_  { false };
};

//---

struct CQChartsHierBubbleNodeCmp {
  bool operator()(const CQChartsHierBubbleNode *n1, const CQChartsHierBubbleNode *n2) {
    return (*n1) < (*n2);
  }
};

//---

class CQChartsHierBubbleHierNode : public CQChartsHierBubbleNode {
 public:
  typedef std::vector<CQChartsHierBubbleNode *>     Nodes;
  typedef std::vector<CQChartsHierBubbleHierNode *> Children;
  typedef CirclePack<CQChartsHierBubbleNode>        Pack;

 public:
  CQChartsHierBubbleHierNode(CQChartsHierBubbleHierNode *parent=nullptr,
                             const std::string &name="") :
   CQChartsHierBubbleNode(name), parent_(parent) {
    if (parent_)
      parent_->children_.push_back(this);
  }

  const Nodes &getNodes() const { return nodes_; }

  const Children &getChildren() const { return children_; }

  void packNodes() {
    // pack child hier nodes first
    for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
      (*p)->packNodes();

    //---

    // make single list of nodes to pack
    Nodes packNodes;

    for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p)
      packNodes.push_back(*p);

    for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn)
      packNodes.push_back(*pn);

    // sort nodes
    std::sort(packNodes.begin(), packNodes.end(), CQChartsHierBubbleNodeCmp());

    // pack nodes
    for (Nodes::const_iterator pp = packNodes.begin(); pp != packNodes.end(); ++pp)
      pack_.addNode(*pp);

    //---

    // get bounding circle
    double xc { 0.0 }, yc { 0.0 }, r { 1.0 };

    pack_.boundingCircle(xc, yc, r);

    // set center and radius
    x_ = xc;
    y_ = yc;

    setRadius(r);

    //setRadius(std::max(std::max(fabs(xmin), xmax), std::max(fabs(ymin), ymax)));
  }

  void addNode(CQChartsHierBubbleNode *node) {
    nodes_.push_back(node);

    size_ += node->size();

    CQChartsHierBubbleHierNode *parent = parent_;

    while (parent) {
      parent->size_ += node->size();

      parent = parent->parent_;
    }
  }

  void setPosition(double x, double y) {
    double dx = x - this->x();
    double dy = y - this->y();

    CQChartsHierBubbleNode::setPosition(x, y);

    for (Nodes::const_iterator pn = nodes_.begin(); pn != nodes_.end(); ++pn) {
      CQChartsHierBubbleNode *node = *pn;

      node->setPosition(node->x() + dx, node->y() + dy);
    }

    for (Children::const_iterator p = children_.begin(); p != children_.end(); ++p) {
      CQChartsHierBubbleHierNode *hierNode = *p;

      hierNode->setPosition(hierNode->x() + dx, hierNode->y() + dy);
    }
  }

 private:
  CQChartsHierBubbleHierNode *parent_;
  Nodes                       nodes_;
  Pack                        pack_;
  Children                    children_;
};

//---

class CQChartsHierBubbleHierObj : public CQChartsPlotObj {
 public:
  CQChartsHierBubbleHierObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *hier,
                            const CBBox2D &rect, int i, int n);

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsHierBubblePlot     *plot_ { nullptr };
  CQChartsHierBubbleHierNode *hier_ { nullptr };
  int                         i_    { 0 };
  int                         n_    { 0 };
};

//---

class CQChartsHierBubbleObj : public CQChartsPlotObj {
 public:
  CQChartsHierBubbleObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleNode *node,
                    const CBBox2D &rect, int i, int n);

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsHierBubblePlot *plot_ { nullptr };
  CQChartsHierBubbleNode *node_ { nullptr };
  int                     i_    { 0 };
  int                     n_    { 0 };
};

//---

class CQChartsHierBubblePlotType : public CQChartsPlotType {
 public:
  CQChartsHierBubblePlotType();

  QString name() const override { return "hierbubble"; }
  QString desc() const override { return "HierBubble"; }

  CQChartsPlot *create(CQChartsView *view, QAbstractItemModel *model) const;
};

//---

class CQChartsHierBubblePlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int    nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(int    valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(double fontHeight  READ fontHeight  WRITE setFontHeight )

 public:
  typedef std::vector<CQChartsHierBubbleNode *> Nodes;

 public:
  CQChartsHierBubblePlot(CQChartsView *view, QAbstractItemModel *model);

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; update(); }

  double fontHeight() const { return fontHeight_; }
  void setFontHeight(double r) { fontHeight_ = r; update(); }

  const CPoint2D &offset() const { return offset_; }
  void setOffset(const CPoint2D &o) { offset_ = o; }

  double scale() const { return scale_; }
  void setScale(double r) { scale_ = r; }

  int maxDepth() const { return maxDepth_; }

  //---

  void addProperties() override;

  void updateRange() override;

  void initObjs(bool force=false) override;

  //---

  void initObjs(CQChartsHierBubbleHierNode *hier, int depth);

  void draw(QPainter *) override;

  void drawForeground(QPainter *) override;

  QColor nodeColor(CQChartsHierBubbleNode *node) const;

 private:
  void initNodes();

  void loadChildren(CQChartsHierBubbleHierNode *hier, const QModelIndex &index, int depth);

  void transformNodes(CQChartsHierBubbleHierNode *hier);

  void drawBounds(QPainter *p, CQChartsHierBubbleHierNode *hier);

 private:
  int                         nameColumn_  { 0 };
  int                         valueColumn_ { 1 };
  CDisplayRange2D             range_;
  CQChartsHierBubbleHierNode* root_        { nullptr };
  double                      fontHeight_  { 6.0 };
  CPoint2D                    offset_      { 0, 0 };
  double                      scale_       { 1.0 };
  int                         maxDepth_    { 1 };
};

#endif
