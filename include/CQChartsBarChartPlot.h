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
  Q_PROPERTY(bool    dataLabel   READ isDataLabel WRITE setDataLabel  )
  Q_PROPERTY(QFont   dataFont    READ dataFont    WRITE setDataFont   )
  Q_PROPERTY(QColor  dataColor   READ dataColor   WRITE setDataColor  )
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

  bool isDataLabel() const { return dataLabel_; }
  void setDataLabel(bool b) { dataLabel_ = b; update(); }

  const QFont &dataFont() const { return dataFont_; }
  void setDataFont(const QFont &f) { dataFont_ = f; update(); }

  const QColor &dataColor() const { return dataColor_; }
  void setDataColor(const QColor &c) { dataColor_ = c; update(); }

  //---

  int margin() const { return margin_; }
  void setMargin(int i) { margin_ = i; update(); }

  bool isKeySets() const { return keySets_; }
  void setKeySets(bool b) { keySets_ = b; resetSetHidden(); initObjs(true); }

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
  int        xColumn_         { 0 };
  int        yColumn_         { 1 };
  Columns    yColumns_;
  bool       stacked_         { false };
  bool       barColorPalette_ { true };
  bool       fill_            { true };
  QColor     barColor_        { 100, 100, 200 };
  bool       border_          { true };
  QColor     borderColor_     { 0, 0, 0 };
  double     borderWidth_     { 0 };
  int        margin_          { 2 };
  bool       dataLabel_       { false };
  QFont      dataFont_;
  QColor     dataColor_       { 0, 0, 0 };
  bool       keySets_         { false };
  ValueSets  valueSets_;
  ValueNames valueNames_;
  IdHidden   idHidden_;
};

#endif
