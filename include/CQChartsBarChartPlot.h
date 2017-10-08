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
                      int iset, int nset, int ival, int nval,
                      int isval, int nsval, double value);

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsBarChartPlot *plot_  { nullptr };
  int                   iset_  { -1 };
  int                   nset_  { -1 };
  int                   ival_  { -1 };
  int                   nval_  { -1 };
  int                   isval_ { -1 };
  int                   nsval_ { -1 };
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

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsBarChartPlot : public CQChartsPlot {
  Q_OBJECT

  // properties
  //  bar color

  Q_PROPERTY(int     categoryColumn   READ categoryColumn   WRITE setCategoryColumn  )
  Q_PROPERTY(int     valueColumn      READ valueColumn      WRITE setValueColumn     )
  Q_PROPERTY(QString valueColumns     READ valueColumnsStr  WRITE setValueColumnsStr )
  Q_PROPERTY(int     nameColumn       READ nameColumn       WRITE setNameColumn      )
  Q_PROPERTY(bool    stacked          READ isStacked        WRITE setStacked         )
  Q_PROPERTY(bool    horizontal       READ isHorizontal     WRITE setHorizontal      )
  Q_PROPERTY(double  margin           READ margin           WRITE setMargin          )
  Q_PROPERTY(bool    keySets          READ isKeySets        WRITE setKeySets         )
  Q_PROPERTY(bool    border           READ isBorder         WRITE setBorder          )
  Q_PROPERTY(QColor  borderColor      READ borderColor      WRITE setBorderColor     )
  Q_PROPERTY(double  borderWidth      READ borderWidth      WRITE setBorderWidth     )
  Q_PROPERTY(double  borderCornerSize READ borderCornerSize WRITE setBorderCornerSize)
  Q_PROPERTY(bool    fill             READ isFill           WRITE setFill            )
  Q_PROPERTY(QString barColor         READ barColorStr      WRITE setBarColorStr     )

 public:
  CQChartsBarChartPlot(CQChartsView *view, const ModelP &model);

  int categoryColumn() const { return categoryColumn_; }
  void setCategoryColumn(int i) { categoryColumn_ = i; update(); }

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

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

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
  bool isBorder() const { return borderObj_.isBorder(); }
  void setBorder(bool b) { borderObj_.setBorder(b); update(); }

  const QColor &borderColor() const { return borderObj_.borderColor(); }
  void setBorderColor(const QColor &c) { borderObj_.setBorderColor(c); update(); }

  double borderWidth() const { return borderObj_.borderWidth(); }
  void setBorderWidth(double r) { borderObj_.setBorderWidth(r); update(); }

  double borderCornerSize() const { return borderObj_.borderCornerSize(); }
  void setBorderCornerSize(double r) { borderObj_.setBorderCornerSize(r); update(); }

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
  struct Value {
    double  value;
    QString name;

    Value(double value=0.0, const QString &name="") :
     value(value), name(name) {
    }
  };

  typedef std::vector<Value> Values;

  struct ValueSet {
    QString name;
    int     ind;
    Values  values;

    ValueSet(const QString &name, int ind) :
     name(name), ind(ind) {
    }

    void calcSums(double &posSum, double &negSum) {
      posSum = 0.0;
      negSum = 0.0;

      for (auto &v : values) {
        if (v.value >= 0) posSum += v.value;
        else              negSum += v.value;
      }
    }
  };

  typedef std::vector<ValueSet> ValueSets;
  typedef std::vector<QString>  ValueNames;
  typedef std::map<int,bool>    IdHidden;

 private:
  ValueSet *getValueSet(const QString &name);

 private:
  int               categoryColumn_    { 0 };
  int               valueColumn_       { 1 };
  Columns           valueColumns_;
  int               nameColumn_        { -1 };
  bool              stacked_           { false };
  bool              horizontal_        { false };
  double            margin_            { 2 };
  bool              keySets_           { false };
  CQChartsBoxObj    borderObj_;
  bool              fill_              { true };
  bool              barColorPalette_   { true };
  QColor            barColor_          { 100, 100, 200 };
  CQChartsDataLabel dataLabel_;
  ValueSets         valueSets_;
  ValueNames        valueNames_;
  IdHidden          idHidden_;
};

#endif
