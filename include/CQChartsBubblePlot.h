#ifndef CQChartsBubblePlot_H
#define CQChartsBubblePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CirclePack.h>
#include <QModelIndex>

class CQChartsBubblePlot;

class CQChartsBubbleNode : public CircleNode {
 private:
  static uint nextId() {
    static int lastId = 0;

    return ++lastId;
  }

 public:
  CQChartsBubbleNode(const std::string &name="", double size=1.0, int colorId=0) :
   id_(nextId()), name_(name), size_(size), colorId_(colorId) {
    r_ = sqrt(size_/(2*M_PI));
  }

  uint id() const { return id_; }

  const std::string &name() const { return name_; }

  double size() const { return size_; }

  double x() const override { return x_; }
  void setX(double x) { x_ = x; }

  double y() const override { return y_; }
  void setY(double y) { y_ = y; }

  int colorId() const { return colorId_; }

  double radius() const override { return r_; }
  void setRadius(double r) { r_ = r; }

  void setPosition(double x, double y) override {
    x_ = x;
    y_ = y;

    placed_ = true;
  }

  bool placed() const { return placed_; }

 private:
  uint        id_      { 0 };
  std::string name_;
  double      size_    { 1.0 };
  int         colorId_ { 0 };
  bool        placed_  { false };
};

//---

class CQChartsBubbleObj : public CQChartsPlotObj {
 public:
  CQChartsBubbleObj(CQChartsBubblePlot *plot, CQChartsBubbleNode *node,
                    const CBBox2D &rect, int i, int n);

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 private:
  CQChartsBubblePlot *plot_ { nullptr };
  CQChartsBubbleNode *node_ { nullptr };
  int                 i_    { 0 };
  int                 n_    { 0 };
};

//---

class CQChartsBubblePlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(double fontHeight READ fontHeight WRITE setFontHeight)

 public:
  typedef CirclePack<CQChartsBubbleNode>    Pack;
  typedef std::vector<CQChartsBubbleNode *> Nodes;

 public:
  CQChartsBubblePlot(CQChartsView *view, QAbstractItemModel *model);

  const char *typeName() const override { return "Bubble"; }

  double fontHeight() const { return fontHeight_; }
  void setFontHeight(double r) { fontHeight_ = r; update(); }

  const CPoint2D &offset() const { return offset_; }
  void setOffset(const CPoint2D &v) { offset_ = v; }

  double scale() const { return scale_; }
  void setScale(double r) { scale_ = r; }

  void addProperties();

  void initObjs(bool force=false);

  void draw(QPainter *) override;

  QColor nodeColor(CQChartsBubbleNode *node) const;

 private:
  void initNodes();

  void loadChildren(const QModelIndex &index);

  void drawBounds(QPainter *p);

 private:
  CDisplayRange2D range_;
  Nodes           nodes_;
  Pack            pack_;
  double          fontHeight_ { 6.0 };
  CPoint2D        offset_     { 0, 0 };
  double          scale_      { 1.0 };
};

#endif
