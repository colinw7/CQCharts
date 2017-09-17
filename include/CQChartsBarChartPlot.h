#ifndef CQChartsBarChartPlot_H
#define CQChartsBarChartPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>

class CQChartsBarChartPlot;

class CQChartsBarChartObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBarChartObj(CQChartsBarChartPlot *plot, const CBBox2D &rect,
                      int iset, int nset, int ival, int nval, double value);

  void draw(QPainter *p) override;

 private:
  CQChartsBarChartPlot *plot_  { nullptr };
  int                   iset_  { -1 };
  int                   nset_  { -1 };
  int                   ival_  { -1 };
  int                   nval_  { -1 };
  double                value_ { -1 };
};

//---

#include <CQChartsKey.h>

class CQChartsBarKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsBarKeyColor(CQChartsBarChartPlot *plot, int i, int n);

  bool mousePress(const CPoint2D &p) override;

  QColor fillColor() const override;
};

class CQChartsBarKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsBarKeyText(CQChartsBarChartPlot *plot, int i, const QString &text);

  QColor textColor() const override;

 private:
  int i_ { 0 };
};

//---

class CQChartsBarDataLabel : public CQChartsBoxObj {
  Q_OBJECT

  Q_PROPERTY(bool     visible  READ isVisible WRITE setVisible )
  Q_PROPERTY(Position position READ position  WRITE setPosition)
  Q_PROPERTY(bool     clip     READ isClip    WRITE setClip    )
  Q_PROPERTY(double   angle    READ angle     WRITE setAngle   )
  Q_PROPERTY(QFont    font     READ font      WRITE setFont    )
  Q_PROPERTY(QColor   color    READ color     WRITE setColor   )
  Q_PROPERTY(double   margin   READ margin    WRITE setMargin  )

  Q_ENUMS(Position)

 public:
  enum Position {
    TOP_INSIDE,
    TOP_OUTSIDE,
    BOTTOM_INSIDE,
    BOTTOM_OUTSIDE,
  };

 public:
  CQChartsBarDataLabel(CQChartsBarChartPlot *plot);

  // data label
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; update(); }

  const Position &position() const { return position_; }
  void setPosition(const Position &p) { position_ = p; update(); }

  bool isClip() const { return clip_; }
  void setClip(bool b) { clip_ = b; update(); }

  double angle() const { return angle_; }
  void setAngle(double r) { angle_ = r; update(); }

  const QFont &font() const { return font_; }
  void setFont(const QFont &f) { font_ = f; update(); }

  const QColor &color() const { return color_; }
  void setColor(const QColor &c) { color_ = c; update(); }

  void addProperties(const QString &path);

  void redraw() { }

  void update();

  void draw(QPainter *p, const QRectF &qrect, const QString &ystr);

 private:
  CQChartsBarChartPlot* plot_     { nullptr };
  bool                  visible_  { false };
  Position              position_ { Position::TOP_INSIDE };
  bool                  clip_     { true };
  double                angle_    { 0.0 };
  QFont                 font_;
  QColor                color_    { 0, 0, 0 };
};

//---

class CQChartsBarChartPlot : public CQChartsPlot {
  Q_OBJECT

  // properties
  //  bar color

  Q_PROPERTY(int     xColumn     READ xColumn     WRITE setXColumn    )
  Q_PROPERTY(int     yColumn     READ yColumn     WRITE setYColumn    )
  Q_PROPERTY(bool    stacked     READ isStacked   WRITE setStacked    )
  Q_PROPERTY(bool    border      READ isBorder    WRITE setBorder     )
  Q_PROPERTY(QColor  borderColor READ borderColor WRITE setBorderColor)
  Q_PROPERTY(double  borderWidth READ borderWidth WRITE setBorderWidth)
  Q_PROPERTY(bool    fill        READ isFill      WRITE setFill       )
  Q_PROPERTY(QString barColor    READ barColorStr WRITE setBarColorStr)
  Q_PROPERTY(int     margin      READ margin      WRITE setMargin     )
  Q_PROPERTY(bool    keySets     READ isKeySets   WRITE setKeySets    )

 public:
  CQChartsBarChartPlot(CQChartsView *view, QAbstractItemModel *model);

  const char *typeName() const override { return "BarChart"; }

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; }

  const Columns &yColumns() const { return yColumns_; }
  void setYColumns(const Columns &yColumns) { yColumns_ = yColumns; }

  bool isStacked() const { return stacked_; }
  void setStacked(bool b) { stacked_ = b; initObjs(/*force*/true); update(); }

  //---

  // fill
  bool isFill() const { return fill_; }
  void setFill(bool b) { fill_ = b; update(); }

  QString barColorStr() const;
  void setBarColorStr(const QString &str);

  // stroke
  bool isBorder() const { return border_; }
  void setBorder(bool b) { border_ = b; update(); }

  const QColor &borderColor() const { return borderColor_; }
  void setBorderColor(const QColor &c) { borderColor_ = c; update(); }

  double borderWidth() const { return borderWidth_; }
  void setBorderWidth(double r) { borderWidth_ = r; update(); }

  //---

  int margin() const { return margin_; }
  void setMargin(int i) { margin_ = i; update(); }

  bool isKeySets() const { return keySets_; }
  void setKeySets(bool b) { keySets_ = b; resetSetHidden(); initObjs(true); }

  //---

  const CQChartsBarDataLabel &dataLabel() const { return dataLabel_; }

  //---

  QColor barColor(int i, int n) const;

  void updateRange();

  void addProperties();

  void initObjs(bool force=false);

  QString valueStr(double v) const;

  void addKeyItems(CQChartsKey *key) override;

  int numSets() const;

  int getSetColumn(int i) const;

  int numValueSets() const;

  int numSetValues() const;

  //---

  bool isSetHidden(int i) const {
    auto p = idHidden_.find(i);

    if (p == idHidden_.end())
      return false;

    return (*p).second;
  }

  void setSetHidden(int i, bool hidden) { idHidden_[i] = hidden; }

  void resetSetHidden() { idHidden_.clear(); }

  //---

  void draw(QPainter *) override;

  void drawDataLabel(QPainter *p, const QRectF &qrect, const QString &ystr);

 private:
  typedef std::vector<double> Values;

  struct ValueSet {
    QString name;
    Values  values;

    ValueSet(const QString &name) :
     name(name) {
    }
  };

  typedef std::vector<ValueSet> ValueSets;
  typedef std::vector<QString>  ValueNames;
  typedef std::map<int,bool>    IdHidden;

 private:
  ValueSet *getValueSet(const QString &name);

 private:
  int                  xColumn_           { 0 };
  int                  yColumn_           { 1 };
  Columns              yColumns_;
  bool                 stacked_           { false };
  bool                 barColorPalette_   { true };
  bool                 fill_              { true };
  QColor               barColor_          { 100, 100, 200 };
  bool                 border_            { true };
  QColor               borderColor_       { 0, 0, 0 };
  double               borderWidth_       { 0 };
  int                  margin_            { 2 };
  CQChartsBarDataLabel dataLabel_;
  bool                 keySets_           { false };
  ValueSets            valueSets_;
  ValueNames           valueNames_;
  IdHidden             idHidden_;
};

#endif
