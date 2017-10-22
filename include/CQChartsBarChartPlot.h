#ifndef CQChartsBarChartPlot_H
#define CQChartsBarChartPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsValueSet.h>
#include <CQChartsDataLabel.h>

class CQChartsBarChartPlot;

// bar object
class CQChartsBarChartObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBarChartObj(CQChartsBarChartPlot *plot, const CBBox2D &rect,
                      int iset, int nset, int ival, int nval,
                      int isval, int nsval, double value);

  void setColor(double color) { color_ = color; }

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  typedef boost::optional<double> OptReal;

  CQChartsBarChartPlot *plot_  { nullptr };
  int                   iset_  { -1 };      // set number
  int                   nset_  { -1 };      // number of sets
  int                   ival_  { -1 };      // value number
  int                   nval_  { -1 };      // number of values
  int                   isval_ { -1 };      // sub set number
  int                   nsval_ { -1 };      // number of sub sets
  double                value_ { -1 };      // value
  OptReal               color_;             // custom color
};

//---

#include <CQChartsKey.h>

// key color box
class CQChartsBarKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsBarKeyColor(CQChartsBarChartPlot *plot, int i, int n);

  void setColor(double color) { color_ = color; }

  bool mousePress(const CPoint2D &p) override;

  QColor fillColor() const override;

 private:
  typedef boost::optional<double> OptReal;

  OptReal color_; // custom color
};

// key text
class CQChartsBarKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsBarKeyText(CQChartsBarChartPlot *plot, int i, const QString &text);

  QColor textColor() const override;

 private:
  int i_ { 0 }; // set id
};

//---

// bar chart plot type
class CQChartsBarChartPlotType : public CQChartsPlotType {
 public:
  CQChartsBarChartPlotType();

  QString name() const override { return "barchart"; }
  QString desc() const override { return "BarChart"; }

  void addParameters();

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

// bar chart plot
//  x   : category, name, ordered by set (keySets)
//  y   : value, values
//  bar : custom color, stacked, horizontal, margin, border, fill
class CQChartsBarChartPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int     categoryColumn   READ categoryColumn    WRITE setCategoryColumn  )
  Q_PROPERTY(int     valueColumn      READ valueColumn       WRITE setValueColumn     )
  Q_PROPERTY(QString valueColumns     READ valueColumnsStr   WRITE setValueColumnsStr )
  Q_PROPERTY(int     nameColumn       READ nameColumn        WRITE setNameColumn      )
  Q_PROPERTY(int     colorColumn      READ colorColumn       WRITE setColorColumn     )
  Q_PROPERTY(bool    stacked          READ isStacked         WRITE setStacked         )
  Q_PROPERTY(bool    horizontal       READ isHorizontal      WRITE setHorizontal      )
  Q_PROPERTY(double  margin           READ margin            WRITE setMargin          )
  Q_PROPERTY(bool    keySets          READ isKeySets         WRITE setKeySets         )
  Q_PROPERTY(bool    border           READ isBorder          WRITE setBorder          )
  Q_PROPERTY(QColor  borderColor      READ borderColor       WRITE setBorderColor     )
  Q_PROPERTY(double  borderWidth      READ borderWidth       WRITE setBorderWidth     )
  Q_PROPERTY(double  borderCornerSize READ borderCornerSize  WRITE setBorderCornerSize)
  Q_PROPERTY(bool    fill             READ isFill            WRITE setFill            )
  Q_PROPERTY(QString barColor         READ barColorStr       WRITE setBarColorStr     )
  Q_PROPERTY(bool    colorMapEnabled  READ isColorMapEnabled WRITE setColorMapEnabled )
  Q_PROPERTY(double  colorMapMin      READ colorMapMin       WRITE setColorMapMin     )
  Q_PROPERTY(double  colorMapMax      READ colorMapMax       WRITE setColorMapMax     )

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

  int colorColumn() const { return colorColumn_; }
  void setColorColumn(int i) { colorColumn_ = i; }

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

  bool isColorMapEnabled() const { return colorSet_.isMapEnabled(); }
  void setColorMapEnabled(bool b) { colorSet_.setMapEnabled(b); initObjs(/*force*/true); update(); }

  double colorMapMin() const { return colorSet_.mapMin(); }
  void setColorMapMin(double r) { colorSet_.setMapMin(r); initObjs(/*force*/true); update(); }

  double colorMapMax() const { return colorSet_.mapMax(); }
  void setColorMapMax(double r) { colorSet_.setMapMax(r); initObjs(/*force*/true); update(); }

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

  void draw(QPainter *) override;

  void drawDataLabel(QPainter *p, const QRectF &qrect, const QString &ystr);

 private:
  struct Value {
    double  value;
    QString name;
    int     i;

    Value(double value=0.0, const QString &name="", int i=-1) :
     value(value), name(name), i(i) {
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

 private:
  ValueSet *getValueSet(const QString &name);

 private:
  int               categoryColumn_  { 0 };
  int               valueColumn_     { 1 };
  Columns           valueColumns_;
  int               nameColumn_      { -1 };
  int               colorColumn_     { -1 };
  bool              stacked_         { false };
  bool              horizontal_      { false };
  double            margin_          { 2 };
  bool              keySets_         { false };
  CQChartsBoxObj    borderObj_;
  bool              fill_            { true };
  bool              barColorPalette_ { true };
  QColor            barColor_        { 100, 100, 200 };
  CQChartsValueSet  colorSet_;
  CQChartsDataLabel dataLabel_;
  ValueSets         valueSets_;
  ValueNames        valueNames_;
};

#endif
