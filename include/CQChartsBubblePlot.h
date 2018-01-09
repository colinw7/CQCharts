#ifndef CQChartsBubblePlot_H
#define CQChartsBubblePlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsCirclePack.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsPaletteColor.h>
#include <CQChartsColorSet.h>
#include <QModelIndex>

class CQChartsBubblePlot;
class CQChartsTextBoxObj;

class CQChartsBubbleNode : public CQChartsCircleNode {
 private:
  static uint nextId() {
    static int lastId = 0;

    return ++lastId;
  }

 public:
  CQChartsBubbleNode() = default;

  CQChartsBubbleNode(const QString &name, double size, const QModelIndex &ind);

  void initRadius() {
    r_ = sqrt(size_/(2*M_PI));
  }

  uint id() const { return id_; }

  const QString &name() const { return name_; }

  double size() const { return size_; }

  double radius() const override { return r_; }
  void setRadius(double r) { r_ = r; }

  double x() const override { return CQChartsCircleNode::x(); }
  void setX(double x) override { CQChartsCircleNode::setX(x); }

  double y() const override { return CQChartsCircleNode::y(); }
  void setY(double y) override { CQChartsCircleNode::setY(y); }

  int colorId() const { return colorId_; }
  void setColorId(int id) { colorId_ = id; }

  const CQChartsPaletteColor &color() const { return color_; }
  void setColor(const CQChartsPaletteColor &v) { color_ = v; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  void setPosition(double x, double y) override;

  bool placed() const { return placed_; }

  QColor interpColor(CQChartsBubblePlot *plot, int n) const;

 private:
  uint                 id_      { 0 };     // node id
  QString              name_;              // node name
  double               size_    { 1.0 };   // node size
  int                  colorId_ { 0 };     // node color index
  CQChartsPaletteColor color_   { };       // node explicit color
  QModelIndex          ind_;               // data model index
  bool                 placed_  { false }; // is placed
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

  Q_PROPERTY(int     nameColumn      READ nameColumn        WRITE setNameColumn      )
  Q_PROPERTY(int     valueColumn     READ valueColumn       WRITE setValueColumn     )
  Q_PROPERTY(int     colorColumn     READ colorColumn       WRITE setColorColumn     )
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

  using Pack = CQChartsCirclePack<CQChartsBubbleNode>;

  using Nodes = std::vector<CQChartsBubbleNode*>;

 public:
  CQChartsBubblePlot(CQChartsView *view, const ModelP &model);

 ~CQChartsBubblePlot();

  //---

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i);

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i);

  int colorColumn() const { return colorColumn_; }
  void setColorColumn(int i);

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

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initColorSet();

  bool colorSetColor(int i, OptColor &color);

  bool initObjs() override;

  //---

  void handleResize() override;

  //---

  void draw(QPainter *) override;

  void drawForeground(QPainter *painter) override;

 private:
  void resetNodes();

  void initNodes();

  void colorNode(CQChartsBubbleNode *node);

  void loadChildren(const QModelIndex &index=QModelIndex());

 private:
  int                  nameColumn_  { 0 };       // name column
  int                  valueColumn_ { -1 };      // value column
  int                  colorColumn_ { -1 };      // color column
  Nodes                nodes_;                   // nodes
  Pack                 pack_;                    // packed nodes
  CQChartsTextBoxObj*  textBoxObj_  { nullptr }; // bubble fill/border/text object
  CQChartsGeom::Point  offset_      { 0, 0 };    // draw offset
  double               scale_       { 1.0 };     // draw scale
  CQChartsColorSet     colorSet_;                // color value set
  int                  colorId_     { -1 };      // current color id
  int                  numColorIds_ { 0 };       // num used color ids
};

#endif
