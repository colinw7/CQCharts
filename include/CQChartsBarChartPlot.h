#ifndef CQChartsBarChartPlot_H
#define CQChartsBarChartPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>

class CQChartsBarChartPlot;

class CQChartsBarChartObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBarChartObj(CQChartsBarChartPlot *plot, const CBBox2D &rect,
                      int iset, int nset, int ival, int nval, double value);

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

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

class CQChartsBarChartPlotType : public CQChartsPlotType {
 public:
  CQChartsBarChartPlotType();

  QString name() const override { return "barchart"; }
  QString desc() const override { return "BarChart"; }
};

//---

class CQChartsBarChartPlot : public CQChartsPlot {
  Q_OBJECT

  // properties
  //  bar color

  Q_PROPERTY(int     nameColumn   READ nameColumn      WRITE setNameColumn     )
  Q_PROPERTY(int     valueColumn  READ valueColumn     WRITE setValueColumn    )
  Q_PROPERTY(QString valueColumns READ valueColumnsStr WRITE setValueColumnsStr)
  Q_PROPERTY(bool    stacked      READ isStacked       WRITE setStacked        )
  Q_PROPERTY(bool    horizontal   READ isHorizontal    WRITE setHorizontal     )
  Q_PROPERTY(double  margin       READ margin          WRITE setMargin         )
  Q_PROPERTY(bool    keySets      READ isKeySets       WRITE setKeySets        )
  Q_PROPERTY(bool    border       READ isBorder        WRITE setBorder         )
  Q_PROPERTY(QColor  borderColor  READ borderColor     WRITE setBorderColor    )
  Q_PROPERTY(double  borderWidth  READ borderWidth     WRITE setBorderWidth    )
  Q_PROPERTY(double  cornerSize   READ cornerSize      WRITE setCornerSize     )
  Q_PROPERTY(bool    fill         READ isFill          WRITE setFill           )
  Q_PROPERTY(QString barColor     READ barColorStr     WRITE setBarColorStr    )

 public:
  CQChartsBarChartPlot(CQChartsView *view, QAbstractItemModel *model);

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  int valueColumn() const { return valueColumn_; }

  void setValueColumn(int i) {
    valueColumn_ = i;

    valueColumns_.clear();

    if (valueColumn_ >= 0)
      valueColumns_.push_back(valueColumn_);

    update();
  }

  const Columns &valueColumns() const { return valueColumns_; }

  void setValueColumns(const Columns &valueColumns) {
    valueColumns_ = valueColumns;

    if (! valueColumns_.empty())
      valueColumn_ = valueColumns_[0];
    else
      valueColumn_ = -1;

    update();
  }

  QString valueColumnsStr() const;
  bool setValueColumnsStr(const QString &s);

  //---

  bool isStacked() const { return stacked_; }
  void setStacked(bool b) { stacked_ = b; initObjs(/*force*/true); update(); }

  bool isHorizontal() const { return horizontal_; }
  void setHorizontal(bool b) { horizontal_ = b; initObjs(/*force*/true); update(); }

  // bar margin
  int margin() const { return margin_; }
  void setMargin(int i) { margin_ = i; update(); }

  bool isKeySets() const { return keySets_; }
  void setKeySets(bool b) { keySets_ = b; resetSetHidden(); initObjs(true); }

  //---

  // bar stroke
  bool isBorder() const { return border_; }
  void setBorder(bool b) { border_ = b; update(); }

  const QColor &borderColor() const { return borderColor_; }
  void setBorderColor(const QColor &c) { borderColor_ = c; update(); }

  double borderWidth() const { return borderWidth_; }
  void setBorderWidth(double r) { borderWidth_ = r; update(); }

  double cornerSize() const { return cornerSize_; }
  void setCornerSize(double r) { cornerSize_ = r; update(); }

  // bar fill
  bool isFill() const { return fill_; }
  void setFill(bool b) { fill_ = b; update(); }

  QString barColorStr() const;
  void setBarColorStr(const QString &str);

  //---

  const CQChartsDataLabel &dataLabel() const { return dataLabel_; }

  //---

  void updateRange() override;

  void addProperties() override;

  void initObjs(bool force=false) override;

  //---

  QColor barColor(int i, int n) const;

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
  int               nameColumn_        { 0 };
  int               valueColumn_       { 1 };
  Columns           valueColumns_;
  bool              stacked_           { false };
  bool              horizontal_        { false };
  double            margin_            { 2 };
  bool              keySets_           { false };
  bool              border_            { true };
  QColor            borderColor_       { 0, 0, 0 };
  double            borderWidth_       { 0 };
  double            cornerSize_        { 0 };
  bool              fill_              { true };
  bool              barColorPalette_   { true };
  QColor            barColor_          { 100, 100, 200 };
  CQChartsDataLabel dataLabel_;
  ValueSets         valueSets_;
  ValueNames        valueNames_;
  IdHidden          idHidden_;
};

#endif
