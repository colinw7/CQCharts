#ifndef CQChartsHierBubblePlot_H
#define CQChartsHierBubblePlot_H

#include <CQChartsHierPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsCirclePack.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsData.h>
#include <QModelIndex>

//---

class CQChartsHierBubblePlotType : public CQChartsHierPlotType {
 public:
  CQChartsHierBubblePlotType();

  QString name() const override { return "hierbubble"; }
  QString desc() const override { return "HierBubble"; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  QString description() const override;

  bool customXRange() const override { return false; }
  bool customYRange() const override { return false; }

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsHierBubblePlot;
class CQChartsHierBubbleHierNode;

class CQChartsHierBubbleNode : public CQChartsCircleNode {
 protected:
  static uint nextId() {
    static int lastId = 0;

    return ++lastId;
  }

 public:
  CQChartsHierBubbleNode(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *parent,
                         const QString &name, double size, const QModelIndex &ind);

  virtual ~CQChartsHierBubbleNode();

  void initRadius();

  CQChartsHierBubblePlot *plot() const { return plot_; }

  CQChartsHierBubbleHierNode *parent() const { return parent_; }

  virtual uint id() const { return id_; }

  virtual const QString &name() const { return name_; }

  virtual double size() const { return size_; }
  virtual void setSize(double s) { size_ = s; }

  virtual double radius() const override { return r_; }
  virtual void setRadius(double r) { r_ = r; }

  double x() const override { return CQChartsCircleNode::x(); }
  void setX(double x) override { CQChartsCircleNode::setX(x); }

  double y() const override { return CQChartsCircleNode::y(); }
  void setY(double y) override { CQChartsCircleNode::setY(y); }

  virtual int colorId() const { return colorId_; }
  virtual void setColorId(int id) { colorId_ = id; }

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &v) { color_ = v; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  virtual int depth() const { return depth_; }
  virtual void setDepth(int i) { depth_ = i; }

  bool isFiller() const { return filler_; }
  void setFiller(bool b) { filler_ = b; }

  virtual double hierSize() const { return size(); }

  virtual QString hierName() const;

  virtual void resetPosition() {
    //CQChartsCircleNode::setPosition(0.0, 0.0);

    //placed_ = false;
  }

  void setPosition(double x, double y) override;

  virtual bool placed() const { return placed_; }

  friend bool operator<(const CQChartsHierBubbleNode &n1, const CQChartsHierBubbleNode &n2) {
    return n1.r_ < n2.r_;
  }

  virtual QColor interpColor(CQChartsHierBubblePlot *plot, int n) const;

 protected:
  CQChartsHierBubblePlot*     plot_    { nullptr }; // parent plot
  CQChartsHierBubbleHierNode *parent_  { nullptr }; // parent hier node
  uint                        id_      { 0 };       // node id
  QString                     name_;                // node name
  double                      size_    { 0.0 };     // node size
  int                         colorId_ { -1 };      // node color index
  CQChartsColor               color_;               // node explicit color
  QModelIndex                 ind_;                 // node model index
  int                         depth_   { 0 };       // node depth
  bool                        filler_  { false };   // is filler
  bool                        placed_  { false };   // is placed
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
  using Pack     = CQChartsCirclePack<CQChartsHierBubbleNode>;

 public:
  CQChartsHierBubbleHierNode(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *parent,
                             const QString &name, const QModelIndex &ind=QModelIndex());

 ~CQChartsHierBubbleHierNode();

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

  void packNodes();

  void addNode(CQChartsHierBubbleNode *node);

  void removeNode(CQChartsHierBubbleNode *node);

  void setPosition(double x, double y) override;

  QColor interpColor(CQChartsHierBubblePlot *plot, int n) const override;

 protected:
  Nodes    nodes_;          // child nodes
  Pack     pack_;           // circle pack
  Children children_;       // child hier nodes
  int      hierInd_ { -1 }; // hier index
};

//---

class CQChartsHierBubbleHierObj;

class CQChartsHierBubbleObj : public CQChartsPlotObj {
 public:
  CQChartsHierBubbleObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleNode *node,
                        CQChartsHierBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                        int i, int n);

  CQChartsHierBubbleNode *node() const { return node_; }

  CQChartsHierBubbleHierObj *parent() const { return hierObj_; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

 protected:
  CQChartsHierBubblePlot*    plot_    { nullptr }; // parent plot
  CQChartsHierBubbleNode*    node_    { nullptr }; // associated node
  CQChartsHierBubbleHierObj* hierObj_ { nullptr }; // parent hier obj
  int                        i_       { 0 };       // index
  int                        n_       { 0 };       // index count
};

//---

class CQChartsHierBubbleHierObj : public CQChartsHierBubbleObj {
 public:
  CQChartsHierBubbleHierObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *hier,
                            CQChartsHierBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                            int i, int n);

  CQChartsHierBubbleHierNode *hierNode() const { return hier_; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

 protected:
  CQChartsHierBubbleHierNode* hier_ { nullptr }; // associated hier node
};

//---

class CQChartsHierBubblePlot : public CQChartsHierPlot {
  Q_OBJECT

  // options
  Q_PROPERTY(bool valueLabel READ isValueLabel WRITE setValueLabel)

  // border
  Q_PROPERTY(bool             border      READ isBorder    WRITE setBorder     )
  Q_PROPERTY(CQChartsColor    borderColor READ borderColor WRITE setBorderColor)
  Q_PROPERTY(double           borderAlpha READ borderAlpha WRITE setBorderAlpha)
  Q_PROPERTY(CQChartsLength   borderWidth READ borderWidth WRITE setBorderWidth)
  Q_PROPERTY(CQChartsLineDash borderDash  READ borderDash  WRITE setBorderDash )

  // fill
  Q_PROPERTY(bool           filled       READ isFilled       WRITE setFilled      )
  Q_PROPERTY(CQChartsColor  fillColor    READ fillColor      WRITE setFillColor   )
  Q_PROPERTY(double         fillAlpha    READ fillAlpha      WRITE setFillAlpha   )
  Q_PROPERTY(Pattern        fillPattern  READ fillPattern    WRITE setFillPattern )

  // text
  Q_PROPERTY(QFont          textFont     READ textFont       WRITE setTextFont    )
  Q_PROPERTY(CQChartsColor  textColor    READ textColor      WRITE setTextColor   )
  Q_PROPERTY(double         textAlpha    READ textAlpha      WRITE setTextAlpha   )
  Q_PROPERTY(bool           textContrast READ isTextContrast WRITE setTextContrast)
  Q_PROPERTY(bool           textScaled   READ isTextScaled   WRITE setTextScaled  )

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

  using Nodes = std::vector<CQChartsHierBubbleNode*>;

 public:
  CQChartsHierBubblePlot(CQChartsView *view, const ModelP &model);

 ~CQChartsHierBubblePlot();

  //---

  bool isValueLabel() const { return valueLabel_; }
  void setValueLabel(bool b);

  //---

  bool isBorder() const;
  void setBorder(bool b);

  const CQChartsColor &borderColor() const;
  void setBorderColor(const CQChartsColor &c);

  QColor interpBorderColor(int i, int n) const;

  double borderAlpha() const;
  void setBorderAlpha(double a);

  const CQChartsLength &borderWidth() const;
  void setBorderWidth(const CQChartsLength &l);

  const CQChartsLineDash &borderDash() const;
  void setBorderDash(const CQChartsLineDash &l);

  //---

  bool isFilled() const;
  void setFilled(bool b);

  const CQChartsColor &fillColor() const;
  void setFillColor(const CQChartsColor &c);

  QColor interpFillColor(int i, int n) const;

  double fillAlpha() const;
  void setFillAlpha(double a);

  Pattern fillPattern() const;
  void setFillPattern(Pattern pattern);

  //---

  const QFont &textFont() const;
  void setTextFont(const QFont &f);

  const CQChartsColor &textColor() const;
  void setTextColor(const CQChartsColor &c);

  double textAlpha() const;
  void setTextAlpha(double a);

  bool isTextContrast() const;
  void setTextContrast(bool b);

  bool isTextScaled() const;
  void setTextScaled(bool b);

  void setTextFontSize(double s);

  QColor interpTextColor(int i, int n) const;

  //---

  CQChartsHierBubbleHierNode *root() const { return root_; }

  CQChartsHierBubbleHierNode *firstHier() const { return firstHier_; }

  CQChartsHierBubbleHierNode *currentRoot() const;
  void setCurrentRoot(CQChartsHierBubbleHierNode *r, bool update=false);

  //---

  const CQChartsGeom::Point &offset() const { return offset_; }
  void setOffset(const CQChartsGeom::Point &o) { offset_ = o; }

  double scale() const { return scale_; }
  void setScale(double r) { scale_ = r; }

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

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void updateObjs() override;

  bool initObjs() override;

  //---

  void handleResize() override;

  //---

  void drawForeground(QPainter *) override;

  //---

  bool addMenuItems(QMenu *menu) override;

 private:
  void initNodeObjs(CQChartsHierBubbleHierNode *hier,
                    CQChartsHierBubbleHierObj *parentObj, int depth);

  void resetNodes();

  void initNodes();

  void replaceNodes();

  void placeNodes(CQChartsHierBubbleHierNode *hier);

  void colorNodes(CQChartsHierBubbleHierNode *hier);

  void colorNode(CQChartsHierBubbleNode *node);

  //---

  void loadHier();

  CQChartsHierBubbleHierNode *addHierNode(CQChartsHierBubbleHierNode *parent, const QString &name,
                                          const QModelIndex &nameInd);

  CQChartsHierBubbleNode *addNode(CQChartsHierBubbleHierNode *parent, const QString &name,
                                  double size, const QModelIndex &nameInd);

  void loadFlat();

  CQChartsHierBubbleNode *addNode(const QStringList &nameStrs, double size,
                                  const QModelIndex &nameInd);

  void addExtraNodes(CQChartsHierBubbleHierNode *hier);

  //---

  CQChartsHierBubbleHierNode *childHierNode(CQChartsHierBubbleHierNode *parent,
                                            const QString &name) const;
  CQChartsHierBubbleNode *childNode(CQChartsHierBubbleHierNode *parent,
                                    const QString &name) const;

  //---

  void initNodes(CQChartsHierBubbleHierNode *hier);

  void transformNodes(CQChartsHierBubbleHierNode *hier);

  void drawBounds(QPainter *painter, CQChartsHierBubbleHierNode *hier);

 public slots:
  void pushSlot();
  void popSlot();
  void popTopSlot();

  void updateCurrentRoot();

 private:
  CQChartsHierBubbleHierNode* root_            { nullptr }; // root node
  CQChartsHierBubbleHierNode* firstHier_       { nullptr }; // first hier node
  QString                     currentRootName_;             // current root name
  bool                        valueLabel_      { false };   // draw value with name
  CQChartsShapeData           shapeData_;                   // bubble fill/border data
  CQChartsTextData            textData_;                    // bubble text data
  CQChartsGeom::Point         offset_          { 0, 0 };    // draw offset
  double                      scale_           { 1.0 };     // draw scale
  int                         colorId_         { -1 };      // current color id
  int                         numColorIds_     { 0 };       // num used color ids
  int                         maxDepth_        { 1 };       // max hier depth
  int                         hierInd_         { 0 };       // current hier ind
};

#endif
