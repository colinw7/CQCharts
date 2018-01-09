#ifndef CQChartsHierBubblePlot_H
#define CQChartsHierBubblePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsCirclePack.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsPaletteColor.h>
#include <CQChartsColorSet.h>
#include <QModelIndex>

class CQChartsHierBubblePlot;
class CQChartsHierBubbleHierNode;
class CQChartsTextBoxObj;

class CQChartsHierBubbleNode : public CQChartsCircleNode {
 private:
  static uint nextId() {
    static int lastId = 0;

    return ++lastId;
  }

 public:
  CQChartsHierBubbleNode(CQChartsHierBubbleHierNode *parent, const QString &name,
                         double size, const QModelIndex &ind);

  virtual ~CQChartsHierBubbleNode();

  void initRadius();

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

  const CQChartsPaletteColor &color() const { return color_; }
  void setColor(const CQChartsPaletteColor &v) { color_ = v; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  virtual int depth() const { return depth_; }
  virtual void setDepth(int i) { depth_ = i; }

  bool isFiller() const { return filler_; }
  void setFiller(bool b) { filler_ = b; }

  virtual double hierSize() const { return size(); }

  virtual void resetPosition() {
    //CQChartsCircleNode::setPosition(0.0, 0.0);

    //placed_ = false;
  }

  virtual void setPosition(double x, double y) override;

  virtual bool placed() const { return placed_; }

  friend bool operator<(const CQChartsHierBubbleNode &n1, const CQChartsHierBubbleNode &n2) {
    return n1.r_ < n2.r_;
  }

  virtual QColor interpColor(CQChartsHierBubblePlot *plot, int n) const;

 protected:
  CQChartsHierBubbleHierNode *parent_  { nullptr }; // parent hier node
  uint                        id_      { 0 };       // node id
  QString                     name_;                // node name
  double                      size_    { 0.0 };     // node size
  int                         colorId_ { -1 };      // node color index
  CQChartsPaletteColor        color_   { };         // node explicit color
  QModelIndex                 ind_;                 // node model index
  int                         depth_   { 0 };       // node depth
  bool                        filler_  { false };   // is fillter
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

  CQChartsHierBubblePlot *plot() const { return plot_; }

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

  void setPosition(double x, double y);

  QColor interpColor(CQChartsHierBubblePlot *plot, int n) const override;

 private:
  CQChartsHierBubblePlot* plot_    { nullptr };
  Nodes                   nodes_;
  Pack                    pack_;
  Children                children_;
  int                     hierInd_ { -1 };
};

//---

class CQChartsHierBubbleHierObj : public CQChartsPlotObj {
 public:
  CQChartsHierBubbleHierObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleHierNode *hier,
                            CQChartsHierBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                            int i, int n);

  CQChartsHierBubbleHierNode *node() const { return hier_; }

  CQChartsHierBubbleHierObj *parent() const { return hierObj_; }

  QString calcId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsHierBubblePlot*     plot_    { nullptr }; // parent plot
  CQChartsHierBubbleHierNode* hier_    { nullptr }; // associated hier node
  CQChartsHierBubbleHierObj*  hierObj_ { nullptr }; // parent hier obj
  int                         i_       { 0 };       // index
  int                         n_       { 0 };       // index count
};

//---

class CQChartsHierBubbleObj : public CQChartsPlotObj {
 public:
  CQChartsHierBubbleObj(CQChartsHierBubblePlot *plot, CQChartsHierBubbleNode *node,
                        CQChartsHierBubbleHierObj *hierObj, const CQChartsGeom::BBox &rect,
                        int i, int n);

  CQChartsHierBubbleNode *node() const { return node_; }

  CQChartsHierBubbleHierObj *parent() const { return hierObj_; }

  QString calcId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsHierBubblePlot*    plot_    { nullptr }; // parent plot
  CQChartsHierBubbleNode*    node_    { nullptr }; // associated node
  CQChartsHierBubbleHierObj* hierObj_ { nullptr }; // parent hier obj
  int                        i_       { 0 };       // index
  int                        n_       { 0 };       // index count
};

//---

class CQChartsHierBubblePlotType : public CQChartsPlotType {
 public:
  CQChartsHierBubblePlotType();

  QString name() const override { return "hierbubble"; }
  QString desc() const override { return "HierBubble"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsHierBubblePlot : public CQChartsHierPlot {
  Q_OBJECT

  Q_PROPERTY(bool    border          READ isBorder          WRITE setBorder          )
  Q_PROPERTY(QString borderColor     READ borderColorStr    WRITE setBorderColorStr  )
  Q_PROPERTY(double  borderAlpha     READ borderAlpha       WRITE setBorderAlpha     )
  Q_PROPERTY(double  borderWidth     READ borderWidth       WRITE setBorderWidth     )
  Q_PROPERTY(bool    filled          READ isFilled          WRITE setFilled          )
  Q_PROPERTY(QString fillColor       READ fillColorStr      WRITE setFillColorStr    )
  Q_PROPERTY(double  fillAlpha       READ fillAlpha         WRITE setFillAlpha       )
  Q_PROPERTY(Pattern fillPattern     READ fillPattern       WRITE setFillPattern     )
  Q_PROPERTY(QFont   textFont        READ textFont          WRITE setTextFont        )
  Q_PROPERTY(QString textColor       READ textColorStr      WRITE setTextColorStr    )
  Q_PROPERTY(bool    textContrast    READ isTextContrast    WRITE setTextContrast    )
  Q_PROPERTY(bool    colorMapEnabled READ isColorMapEnabled WRITE setColorMapEnabled )
  Q_PROPERTY(double  colorMapMin     READ colorMapMin       WRITE setColorMapMin     )
  Q_PROPERTY(double  colorMapMax     READ colorMapMax       WRITE setColorMapMax     )

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

  using Nodes = std::vector<CQChartsHierBubbleNode*>;

 public:
  CQChartsHierBubblePlot(CQChartsView *view, const ModelP &model);

 ~CQChartsHierBubblePlot();

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

  CQChartsHierBubbleHierNode *root() const { return root_; }

  CQChartsHierBubbleHierNode *firstHier() const { return firstHier_; }

  CQChartsHierBubbleHierNode *currentRoot() const { return currentRoot_; }
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

  void initColorSet();

  bool colorSetColor(int i, OptColor &color);

  bool initObjs() override;

  //---

  void handleResize() override;

  //---

  void draw(QPainter *) override;

  void drawForeground(QPainter *) override;

  //---

  bool addMenuItems(QMenu *menu) override;

 private:
  void updateHierColumns() override;

  void initNodeObjs(CQChartsHierBubbleHierNode *hier,
                    CQChartsHierBubbleHierObj *parentObj, int depth);

  void resetNodes();

  void initNodes();

  void replaceNodes();

  void placeNodes(CQChartsHierBubbleHierNode *hier);

  void colorNodes(CQChartsHierBubbleHierNode *hier);

  void colorNode(CQChartsHierBubbleNode *node);

  //---

  void loadChildren(CQChartsHierBubbleHierNode *hier,
                    const QModelIndex &index=QModelIndex(), int depth=0);

  void loadFlat();

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
  CQChartsHierBubbleHierNode* root_        { nullptr }; // root node
  CQChartsHierBubbleHierNode* firstHier_   { nullptr }; // first hier node
  CQChartsHierBubbleHierNode* currentRoot_ { nullptr }; // current root node
  CQChartsTextBoxObj*         textBoxObj_  { nullptr }; // bubble fill/border/text object
  CQChartsGeom::Point         offset_      { 0, 0 };    // draw offset
  double                      scale_       { 1.0 };     // draw scale
  CQChartsColorSet            colorSet_;                // color value set
  int                         colorId_     { -1 };      // current color id
  int                         numColorIds_ { 0 };       // num used color ids
  int                         maxDepth_    { 1 };       // max hier depth
  int                         hierInd_     { 0 };       // current hier ind
};

#endif
