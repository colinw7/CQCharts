#ifndef CQChartsHierBubblePlot_H
#define CQChartsHierBubblePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CirclePack.h>
#include <QModelIndex>

class CQChartsHierBubblePlot;
class CQChartsHierBubbleHierNode;

class CQChartsHierBubbleNode : public CircleNode {
 private:
  static uint nextId() {
    static int lastId = 0;

    return ++lastId;
  }

 public:
  CQChartsHierBubbleNode(CQChartsHierBubbleHierNode *parent, const QString &name,
                         double size, int colorId, const QModelIndex &ind) :
   parent_(parent), id_(nextId()), name_(name), size_(size), colorId_(colorId), ind_(ind) {
    initRadius();
  }

  virtual ~CQChartsHierBubbleNode() { }

  void initRadius() {
    r_ = sqrt(size_/(2*M_PI));
  }

  CQChartsHierBubbleHierNode *parent() const { return parent_; }

  virtual uint id() const { return id_; }

  virtual const QString &name() const { return name_; }

  virtual double size() const { return size_; }

  virtual double radius() const override { return r_; }
  virtual void setRadius(double r) { r_ = r; }

  double x() const override { return CircleNode::x(); }
  void setX(double x) override { CircleNode::setX(x); }

  double y() const override { return CircleNode::y(); }
  void setY(double y) override { CircleNode::setY(y); }

  virtual int colorId() const { return colorId_; }
  virtual void setColorId(int id) { colorId_ = id; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  virtual int depth() const { return depth_; }
  virtual void setDepth(int i) { depth_ = i; }

  virtual void resetPosition() {
    //CircleNode::setPosition(0.0, 0.0);

    //placed_ = false;
  }

  virtual void setPosition(double x, double y) override {
    CircleNode::setPosition(x, y);

    placed_ = true;
  }

  virtual bool placed() const { return placed_; }

  friend bool operator<(const CQChartsHierBubbleNode &n1, const CQChartsHierBubbleNode &n2) {
    return n1.r_ < n2.r_;
  }

 protected:
  CQChartsHierBubbleHierNode *parent_  { nullptr };
  uint                        id_      { 0 };
  QString                     name_;
  double                      size_    { 1.0 };
  int                         colorId_ { 0 };
  QModelIndex                 ind_;
  int                         depth_   { 0 };
  bool                        placed_  { false };
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
  using Nodes    = std::vector<CQChartsHierBubbleNode*>;
  using Children = std::vector<CQChartsHierBubbleHierNode*>;
  using Pack     = CirclePack<CQChartsHierBubbleNode>;

 public:
  CQChartsHierBubbleHierNode(CQChartsHierBubbleHierNode *parent, const QString &name);

  const Nodes &getNodes() const { return nodes_; }

  const Children &getChildren() const { return children_; }

  void packNodes();

  void addNode(CQChartsHierBubbleNode *node);

  void setPosition(double x, double y);

 private:
  CQChartsHierBubbleHierNode *parent_ { nullptr };
  Nodes                       nodes_;
  Pack                        pack_;
  Children                    children_;
};

//---

class CQChartsHierBubbleHierObj : public CQChartsPlotObj {
 public:
  CQChartsHierBubbleHierObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *hier,
                            CQChartsHierBubbleHierObj *hierObj, const CBBox2D &rect, int i, int n);

  bool inside(const CPoint2D &p) const override;

  void clickZoom(const CPoint2D &p) override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsHierBubblePlot*     plot_    { nullptr };
  CQChartsHierBubbleHierNode* hier_    { nullptr };
  CQChartsHierBubbleHierObj*  hierObj_ { nullptr };
  int                         i_       { 0 };
  int                         n_       { 0 };
};

//---

class CQChartsHierBubbleObj : public CQChartsPlotObj {
 public:
  CQChartsHierBubbleObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleNode *node,
                        CQChartsHierBubbleHierObj *hierObj, const CBBox2D &rect, int i, int n);

  bool inside(const CPoint2D &p) const override;

  void clickZoom(const CPoint2D &p) override;

  void mousePress(const CPoint2D &p) override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsHierBubblePlot*    plot_    { nullptr };
  CQChartsHierBubbleNode*    node_    { nullptr };
  CQChartsHierBubbleHierObj* hierObj_ { nullptr };
  int                        i_       { 0 };
  int                        n_       { 0 };
};

//---

class CQChartsHierBubblePlotType : public CQChartsPlotType {
 public:
  CQChartsHierBubblePlotType();

  QString name() const override { return "hierbubble"; }
  QString desc() const override { return "HierBubble"; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsHierBubblePlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int     nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(int     valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(QString separator   READ separator   WRITE setSeparator  )
  Q_PROPERTY(double  fontHeight  READ fontHeight  WRITE setFontHeight )

 public:
  using Nodes = std::vector<CQChartsHierBubbleNode*>;

 public:
  CQChartsHierBubblePlot(CQChartsView *view, const ModelP &model);

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

  const CPoint2D &offset() const { return offset_; }
  void setOffset(const CPoint2D &o) { offset_ = o; }

  double scale() const { return scale_; }
  void setScale(double r) { scale_ = r; }

  int maxDepth() const { return maxDepth_; }

  //---

  CQChartsHierBubbleHierNode *root() const { return root_; }

  CQChartsHierBubbleHierNode *currentRoot() const { return currentRoot_; }
  void setCurrentRoot(CQChartsHierBubbleHierNode *r);

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initObjs() override;

  //---

  void initNodeObjs(CQChartsHierBubbleHierNode *hier,
                    CQChartsHierBubbleHierObj *parentObj, int depth);

  //---

  CQChartsHierBubbleHierNode *childHierNode(CQChartsHierBubbleHierNode *parent,
                                            const QString &name) const;
  CQChartsHierBubbleNode *childNode(CQChartsHierBubbleHierNode *parent,
                                    const QString &name) const;

  //---

  bool isClickZoom() const override { return true; }

  void zoomFull() override;

  //---

  void draw(QPainter *) override;

  void drawForeground(QPainter *) override;

  QColor nodeColor(CQChartsHierBubbleNode *node) const;

 private:
  void initNodes();

  void placeNodes(CQChartsHierBubbleHierNode *hier);

  bool isHier() const;

  void loadChildren(CQChartsHierBubbleHierNode *hier,
                    const QModelIndex &index=QModelIndex(), int depth=0);

  void loadFlat();

  void initNodes(CQChartsHierBubbleHierNode *hier);

  void transformNodes(CQChartsHierBubbleHierNode *hier);

  void drawBounds(QPainter *p, CQChartsHierBubbleHierNode *hier);

 public slots:
  void updateCurrentRoot();

 private:
  int                         nameColumn_  { 0 };
  int                         valueColumn_ { 1 };
  CDisplayRange2D             range_;
  CQChartsHierBubbleHierNode* root_        { nullptr };
  CQChartsHierBubbleHierNode* currentRoot_ { nullptr };
  QString                     separator_   { "/" };
  double                      fontHeight_  { 9.0 };
  CPoint2D                    offset_      { 0, 0 };
  double                      scale_       { 1.0 };
  int                         maxDepth_    { 1 };
};

#endif
