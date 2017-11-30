#ifndef CQChartsBarChartPlot_H
#define CQChartsBarChartPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsValueSet.h>
#include <CQChartsDataLabel.h>
#include <CQChartsPaletteColor.h>

class CQChartsBarChartPlot;
class CQChartsBoxObj;
class CQChartsFillObj;

// bar object
class CQChartsBarChartObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using OptColor = boost::optional<CQChartsPaletteColor>;

 public:
  CQChartsBarChartObj(CQChartsBarChartPlot *plot, const CQChartsGeom::BBox &rect,
                      int iset, int nset, int ival, int nval,
                      int isval, int nsval, double value, const QModelIndex &ind);

  QString calcId() const override;

  void setColor(const CQChartsPaletteColor &color) { color_ = color; }

  void setLabel(const QString &label) { label_ = label; }

  void mousePress(const CQChartsGeom::Point &p) override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsBarChartPlot *plot_  { nullptr }; // parent plot
  int                   iset_  { -1 };      // set number
  int                   nset_  { -1 };      // number of sets
  int                   ival_  { -1 };      // value number
  int                   nval_  { -1 };      // number of values
  int                   isval_ { -1 };      // sub set number
  int                   nsval_ { -1 };      // number of sub sets
  double                value_ { -1 };      // value
  QModelIndex           ind_;               // model index
  OptColor              color_;             // custom color
  QString               label_;             // custom data label
};

//---

#include <CQChartsKey.h>

// key color box
class CQChartsBarKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  using OptColor = boost::optional<CQChartsPaletteColor>;

 public:
  CQChartsBarKeyColor(CQChartsBarChartPlot *plot, int i, int n);

  void setColor(const CQChartsPaletteColor &color) { color_ = color; }

  bool mousePress(const CQChartsGeom::Point &p) override;

  QBrush fillBrush() const override;

 private:
  OptColor color_; // custom color
};

// key text
class CQChartsBarKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsBarKeyText(CQChartsBarChartPlot *plot, int i, const QString &text);

  QColor interpTextColor(int i, int n) const override;

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

  void addParameters() override;

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
  Q_PROPERTY(int     labelColumn      READ labelColumn       WRITE setLabelColumn     )
  Q_PROPERTY(bool    stacked          READ isStacked         WRITE setStacked         )
  Q_PROPERTY(bool    horizontal       READ isHorizontal      WRITE setHorizontal      )
  Q_PROPERTY(double  margin           READ margin            WRITE setMargin          )
  Q_PROPERTY(bool    keySets          READ isKeySets         WRITE setKeySets         )
  Q_PROPERTY(bool    border           READ isBorder          WRITE setBorder          )
  Q_PROPERTY(QString borderColor      READ borderColorStr    WRITE setBorderColorStr  )
  Q_PROPERTY(double  borderWidth      READ borderWidth       WRITE setBorderWidth     )
  Q_PROPERTY(double  borderCornerSize READ borderCornerSize  WRITE setBorderCornerSize)
  Q_PROPERTY(bool    barFill          READ isBarFill         WRITE setBarFill         )
  Q_PROPERTY(QString barColor         READ barColorStr       WRITE setBarColorStr     )
  Q_PROPERTY(double  barAlpha         READ barAlpha          WRITE setBarAlpha        )
  Q_PROPERTY(Pattern barPattern       READ barPattern        WRITE setBarPattern      )
  Q_PROPERTY(bool    colorMapEnabled  READ isColorMapEnabled WRITE setColorMapEnabled )
  Q_PROPERTY(double  colorMapMin      READ colorMapMin       WRITE setColorMapMin     )
  Q_PROPERTY(double  colorMapMax      READ colorMapMax       WRITE setColorMapMax     )

  Q_ENUMS(Pattern)

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

 public:
  CQChartsBarChartPlot(CQChartsView *view, const ModelP &model);
 ~CQChartsBarChartPlot();

  //---

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

  int labelColumn() const { return labelColumn_; }
  void setLabelColumn(int i) { labelColumn_ = i; }

  //---

  bool isStacked() const { return stacked_; }
  void setStacked(bool b) { stacked_ = b; updateRange(); }

  bool isHorizontal() const { return horizontal_; }
  void setHorizontal(bool b) { horizontal_ = b; updateRange(); }

  bool isKeySets() const { return keySets_; }
  void setKeySets(bool b) { keySets_ = b; resetSetHidden(); updateRange(); updateObjs(); }

  //---

  // bar margin
  int margin() const { return margin_; }
  void setMargin(int i) { margin_ = i; update(); }

  //---

  // bar stroke
  bool isBorder() const;
  void setBorder(bool b);

  QString borderColorStr() const;
  void setBorderColorStr(const QString &s);

  QColor interpBorderColor(int i, int n) const;

  double borderWidth() const;
  void setBorderWidth(double r);

  double borderCornerSize() const;
  void setBorderCornerSize(double r);

  //---

  // bar fill
  bool isBarFill() const;
  void setBarFill(bool b);

  QString barColorStr() const;
  void setBarColorStr(const QString &str);

  double barAlpha() const;
  void setBarAlpha(double a);

  Pattern barPattern() const;
  void setBarPattern(Pattern pattern);

  //---

  void initColorSet();

  bool colorSetColor(int i, OptColor &color);

  bool isColorMapEnabled() const { return colorSet_.isMapEnabled(); }
  void setColorMapEnabled(bool b) { colorSet_.setMapEnabled(b); updateObjs(); }

  double colorMapMin() const { return colorSet_.mapMin(); }
  void setColorMapMin(double r) { colorSet_.setMapMin(r); updateObjs(); }

  double colorMapMax() const { return colorSet_.mapMax(); }
  void setColorMapMax(double r) { colorSet_.setMapMax(r); updateObjs(); }

  //---

  const CQChartsDataLabel &dataLabel() const { return dataLabel_; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void updateObjs() override;

  void initObjs() override;

  //---

  QColor interpBarColor(int i, int n) const;

  QString valueStr(double v) const;

  void addKeyItems(CQChartsKey *key) override;

  int numSets() const;

  int getSetColumn(int i) const;

  int numValueSets() const;

  int numSetValues() const;

  //---

  const QString &valueName(int i) const { return valueNames_[i]; }

  const QString &valueSetName(int i) const { return valueSets_[i].name; }

  const QString &valueSetValueName(int i, int j) const { return valueSets_[i].values[j].name; }

  //---

  bool probe(ProbeData &probeData) const override;

  void draw(QPainter *) override;

  void drawDataLabel(QPainter *p, const QRectF &qrect, const QString &ystr);

 private:
  struct Value {
    double      value { 0.0 };
    QString     name;
    QString     label;
    QModelIndex ind;

    Value() = default;

    Value(double value, const QString &name, const QString &label, const QModelIndex &ind) :
     value(value), name(name), label(label), ind(ind) {
    }
  };

  using Values = std::vector<Value>;

  struct ValueSet {
    QString name;
    int     ind { 0 };
    Values  values;

    ValueSet() = default;

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

  using ValueSets    = std::vector<ValueSet>;
  using ValueNames   = std::vector<QString>;
  using ValueNameInd = std::map<QString,int>;

 private:
  ValueSet *getValueSet(const QString &name);

 private:
  int                  categoryColumn_ { 0 };       // category column
  int                  valueColumn_    { 1 };       // value column
  Columns              valueColumns_;               // value columns
  int                  nameColumn_     { -1 };      // name column
  int                  colorColumn_    { -1 };      // color column
  int                  labelColumn_    { -1 };      // data label column
  bool                 stacked_        { false };   // stacked bars
  bool                 horizontal_     { false };   // horizontal bars
  double               margin_         { 2 };       // bar margin
  bool                 keySets_        { false };   // use set in key
  CQChartsBoxObj*      borderObj_      { nullptr }; // border data
  CQChartsFillObj*     fillObj_        { nullptr }; // fill data
  CQChartsValueSet     colorSet_;                   // color column value set
  CQChartsDataLabel    dataLabel_;                  // data label data
  ValueSets            valueSets_;                  // value sets
  ValueNames           valueNames_;                 // value names
  ValueNameInd         valueNameInd_;               // name value index map
};

#endif
