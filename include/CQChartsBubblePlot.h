#ifndef CQChartsBubblePlot_H
#define CQChartsBubblePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsCirclePack.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsPaletteColor.h>
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

  CQChartsBubbleNode(const QString &name, double size, int colorId, const QModelIndex &ind) :
   id_(nextId()), name_(name), size_(size), colorId_(colorId), ind_(ind) {
    r_ = sqrt(size_/(2*M_PI));
  }

  uint id() const { return id_; }

  const QString &name() const { return name_; }

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
  uint        id_      { 0 };     // node id
  QString     name_;              // node name
  double      size_    { 1.0 };   // node size
  int         colorId_ { 0 };     // node color index
  QModelIndex ind_;               // data model index
  bool        placed_  { false }; // is placed
};

//---

class CQChartsBubbleObj : public CQChartsPlotObj {
 public:
  CQChartsBubbleObj(CQChartsBubblePlot *plot, CQChartsBubbleNode *node,
                    const CQChartsGeom::BBox &rect, int i, int n);

  QString calcId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsBubblePlot *plot_ { nullptr }; // parent plot
  CQChartsBubbleNode *node_ { nullptr }; // associated node
  int                 i_    { 0 };       // data index
  int                 n_    { 0 };       // data count
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

  Q_PROPERTY(int     nameColumn  READ nameColumn     WRITE setNameColumn    )
  Q_PROPERTY(int     valueColumn READ valueColumn    WRITE setValueColumn   )
  Q_PROPERTY(bool    border      READ isBorder       WRITE setBorder        )
  Q_PROPERTY(QString borderColor READ borderColorStr WRITE setBorderColorStr)
  Q_PROPERTY(double  borderAlpha READ borderAlpha    WRITE setBorderAlpha   )
  Q_PROPERTY(double  borderWidth READ borderWidth    WRITE setBorderWidth   )
  Q_PROPERTY(QString fillColor   READ fillColorStr   WRITE setFillColorStr  )
  Q_PROPERTY(double  fillAlpha   READ fillAlpha      WRITE setFillAlpha     )
  Q_PROPERTY(QFont   textFont    READ textFont       WRITE setTextFont      )
  Q_PROPERTY(QString textColor   READ textColorStr   WRITE setTextColorStr  )

 public:
  using Pack  = CQChartsCirclePack<CQChartsBubbleNode>;
  using Nodes = std::vector<CQChartsBubbleNode*>;

 public:
  CQChartsBubblePlot(CQChartsView *view, const ModelP &model);

 ~CQChartsBubblePlot();

  //---

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; updateRangeAndObjs(); }

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; updateRangeAndObjs(); }

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

  const CQChartsGeom::Point &offset() const { return offset_; }
  void setOffset(const CQChartsGeom::Point &o) { offset_ = o; }

  double scale() const { return scale_; }
  void setScale(double r) { scale_ = r; }

  //---

  int numColors() const { return numColors_; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  //---

  void draw(QPainter *) override;

  void drawForeground(QPainter *painter) override;

 private:
  void initNodes();

  void loadChildren(const QModelIndex &index=QModelIndex());

 private:
  int                  nameColumn_  { 0 };       // name column
  int                  valueColumn_ { 1 };       // value column
  Nodes                nodes_;                   // nodes
  Pack                 pack_;                    // packed nodes
  CQChartsBoxObj*      bubbleObj_   { nullptr }; // bubble fill/border object
  QFont                textFont_;                // text font
  CQChartsPaletteColor textColor_;               // text color
  CQChartsGeom::Point  offset_      { 0, 0 };    // draw offset
  double               scale_       { 1.0 };     // draw scale
  int                  numColors_   { 1 };       // max number of colors used
};

#endif
