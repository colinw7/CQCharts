#ifndef CQChartsBubblePlot_H
#define CQChartsBubblePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsCirclePack.h>
#include <CQChartsDisplayRange.h>
#include <QModelIndex>

class CQChartsBubblePlot;

class CQChartsBubbleNode : public CQChartsCircleNode {
 private:
  static uint nextId() {
    static int lastId = 0;

    return ++lastId;
  }

 public:
  CQChartsBubbleNode() = default;

  CQChartsBubbleNode(const std::string &name, double size, int colorId,
                     const QModelIndex &ind) :
   id_(nextId()), name_(name), size_(size), colorId_(colorId), ind_(ind) {
    r_ = sqrt(size_/(2*M_PI));
  }

  uint id() const { return id_; }

  const std::string &name() const { return name_; }

  double size() const { return size_; }

  double x() const override { return CQChartsCircleNode::x(); }
  void setX(double x) override { CQChartsCircleNode::setX(x); }

  double y() const override { return CQChartsCircleNode::y(); }
  void setY(double y) override { CQChartsCircleNode::setY(y); }

  int colorId() const { return colorId_; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  double radius() const override { return r_; }
  void setRadius(double r) { r_ = r; }

  void setPosition(double x, double y) override {
    CQChartsCircleNode::setPosition(x, y);

    placed_ = true;
  }

  bool placed() const { return placed_; }

 private:
  uint        id_      { 0 };
  std::string name_;
  double      size_    { 1.0 };
  int         colorId_ { 0 };
  QModelIndex ind_;
  bool        placed_  { false };
};

//---

class CQChartsBubbleObj : public CQChartsPlotObj {
 public:
  CQChartsBubbleObj(CQChartsBubblePlot *plot, CQChartsBubbleNode *node,
                    const CQChartsGeom::BBox &rect, int i, int n);

  bool inside(const CQChartsGeom::Point &p) const override;

  void mousePress(const CQChartsGeom::Point &) override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsBubblePlot *plot_ { nullptr };
  CQChartsBubbleNode *node_ { nullptr };
  int                 i_    { 0 };
  int                 n_    { 0 };
};

//---

class CQChartsBubblePlotType : public CQChartsPlotType {
 public:
  CQChartsBubblePlotType();

  QString name() const override { return "bubble"; }
  QString desc() const override { return "Bubble"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsBubblePlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int    nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(int    valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(double fontHeight  READ fontHeight  WRITE setFontHeight )

 public:
  using Pack  = CQChartsCirclePack<CQChartsBubbleNode>;
  using Nodes = std::vector<CQChartsBubbleNode*>;

 public:
  CQChartsBubblePlot(CQChartsView *view, const ModelP &model);

 ~CQChartsBubblePlot();

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; update(); }

  double fontHeight() const { return fontHeight_; }
  void setFontHeight(double r) { fontHeight_ = r; update(); }

  const CQChartsGeom::Point &offset() const { return offset_; }
  void setOffset(const CQChartsGeom::Point &o) { offset_ = o; }

  double scale() const { return scale_; }
  void setScale(double r) { scale_ = r; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initObjs() override;

  //---

  void draw(QPainter *) override;

  void drawForeground(QPainter *p) override;

  QColor nodeColor(CQChartsBubbleNode *node) const;

 private:
  void initNodes();

  void loadChildren(const QModelIndex &index=QModelIndex());

 private:
  int                  nameColumn_  { 0 };
  int                  valueColumn_ { 1 };
  CQChartsDisplayRange range_;
  Nodes                nodes_;
  Pack                 pack_;
  double               fontHeight_  { 6.0 };
  CQChartsGeom::Point  offset_      { 0, 0 };
  double               scale_       { 1.0 };
  int                  numColors_   { 1 };
};

#endif
