#ifndef CQChartsScatterPlot_H
#define CQChartsScatterPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>

//---

class CQChartsScatterPlotType : public CQChartsPlotType {
 public:
  CQChartsScatterPlotType();

  QString name() const override { return "scatter"; }
  QString desc() const override { return "Scatter"; }

  void addParameters() override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsScatterPlot;

class CQChartsScatterPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using OptReal  = boost::optional<double>;
  using OptColor = boost::optional<CQChartsColor>;

 public:
  CQChartsScatterPointObj(CQChartsScatterPlot *plot, const CQChartsGeom::BBox &rect,
                          const QPointF &p, double symbolSize, const OptReal &fontSize,
                          const OptColor &color, int is, int ns, int iv, int nv);

  double symbolSize() const { return symbolSize_; }
  void setSymbolSize(double s) { symbolSize_ = s; }

  const QString &name() const { return name_; }
  void setName(const QString &v) { name_ = v; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &v) { ind_ = v; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsScatterPlot* plot_       { nullptr };
  QPointF              p_;
  double               symbolSize_;
  OptReal              fontSize_;
  OptColor             color_;
  int                  is_         { -1 };
  int                  ns_         { -1 };
  int                  iv_         { -1 };
  int                  nv_         { -1 };
  QString              name_;
  QModelIndex          ind_;
};

//---

#include <CQChartsKey.h>

class CQChartsScatterKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int i, int n);

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &v) { color_ = v; }

  bool selectPress(const CQChartsGeom::Point &p) override;

  QBrush fillBrush() const override;

 private:
  CQChartsColor color_;
};

//---

class CQChartsScatterPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(CQChartsColumn nameColumn       READ nameColumn       WRITE setNameColumn      )
  Q_PROPERTY(CQChartsColumn xColumn          READ xColumn          WRITE setXColumn         )
  Q_PROPERTY(CQChartsColumn yColumn          READ yColumn          WRITE setYColumn         )
  Q_PROPERTY(CQChartsColumn symbolSizeColumn READ symbolSizeColumn WRITE setSymbolSizeColumn)
  Q_PROPERTY(CQChartsColumn fontSizeColumn   READ fontSizeColumn   WRITE setFontSizeColumn  )
  Q_PROPERTY(CQChartsColumn colorColumn      READ colorColumn      WRITE setColorColumn     )

  Q_PROPERTY(bool           symbolBorder      READ isSymbolBorder    WRITE setSymbolBorder     )
  Q_PROPERTY(CQChartsColor  symbolBorderColor READ symbolBorderColor WRITE setSymbolBorderColor)
  Q_PROPERTY(double         symbolBorderAlpha READ symbolBorderAlpha WRITE setSymbolBorderAlpha)
  Q_PROPERTY(CQChartsLength symbolBorderWidth READ symbolBorderWidth WRITE setSymbolBorderWidth)
  Q_PROPERTY(bool           symbolFilled      READ isSymbolFilled    WRITE setSymbolFilled     )
  Q_PROPERTY(CQChartsColor  symbolFillColor   READ symbolFillColor   WRITE setSymbolFillColor  )
  Q_PROPERTY(double         symbolFillAlpha   READ symbolFillAlpha   WRITE setSymbolFillAlpha  )
  Q_PROPERTY(Pattern        symbolFillPattern READ symbolFillPattern WRITE setSymbolFillPattern)
  Q_PROPERTY(double         symbolSize        READ symbolSize        WRITE setSymbolSize       )

  Q_PROPERTY(double fontSize   READ fontSize     WRITE setFontSize  )
  Q_PROPERTY(bool   textLabels READ isTextLabels WRITE setTextLabels)

  Q_PROPERTY(bool   symbolSizeMapped READ isSymbolSizeMapped WRITE setSymbolSizeMapped)
  Q_PROPERTY(double symbolSizeMapMin READ symbolSizeMapMin   WRITE setSymbolSizeMapMin)
  Q_PROPERTY(double symbolSizeMapMax READ symbolSizeMapMax   WRITE setSymbolSizeMapMax)

  Q_PROPERTY(bool   colorMapped READ isColorMapped WRITE setColorMapped)
  Q_PROPERTY(double colorMapMin READ colorMapMin   WRITE setColorMapMin)
  Q_PROPERTY(double colorMapMax READ colorMapMax   WRITE setColorMapMax)

  Q_PROPERTY(bool   fontSizeMapped READ isFontSizeMapped WRITE setFontSizeMapped)
  Q_PROPERTY(double fontSizeMapMin READ fontSizeMapMin   WRITE setFontSizeMapMin)
  Q_PROPERTY(double fontSizeMapMax READ fontSizeMapMax   WRITE setFontSizeMapMax)

  Q_ENUMS(Pattern)

 public:
  struct ValueData {
    QPointF       p;
    int           i;
    QModelIndex   ind;
    QString       symbolSizeStr;
    QString       fontSizeStr;
    CQChartsColor color;

    ValueData(double x, double y, int i, const QModelIndex &ind, const QString &symbolSizeStr="",
              const QString &fontSizeStr="", const CQChartsColor &color=CQChartsColor()) :
     p(x, y), i(i), ind(ind), symbolSizeStr(symbolSizeStr), fontSizeStr(fontSizeStr),
     color(color) {
    }
  };

  using Values     = std::vector<ValueData>;
  using NameValues = std::map<QString,Values>;
  using OptColor   = boost::optional<CQChartsColor>;

  enum class Pattern {
    SOLID,
    HATCH,
    DENSE,
    HORIZ,
    VERT,
    FDIAG,
    BDIAG
  };

 public:
  CQChartsScatterPlot(CQChartsView *view, const ModelP &model);

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c);

  const CQChartsColumn &yColumn() const { return yColumn_; }
  void setYColumn(const CQChartsColumn &c);

  //---

  double symbolSize() const { return symbolSize_; }
  void setSymbolSize(double s);

  //---

  bool isSymbolBorder() const { return symbolData_.border.visible; }
  void setSymbolBorder(bool b);

  const CQChartsColor &symbolBorderColor() const { return symbolData_.border.color; }
  void setSymbolBorderColor(const CQChartsColor &c);

  double symbolBorderAlpha() const { return symbolData_.border.alpha; }
  void setSymbolBorderAlpha(double a);

  const CQChartsLength &symbolBorderWidth() const { return symbolData_.border.width; }
  void setSymbolBorderWidth(const CQChartsLength &l);

  QColor interpSymbolBorderColor(int i, int n) const {
    return symbolData_.border.color.interpColor(this, i, n); }

  //---

  bool isSymbolFilled() const { return symbolData_.background.visible; }
  void setSymbolFilled(bool b);

  const CQChartsColor &symbolFillColor() const { return symbolData_.background.color; }
  void setSymbolFillColor(const CQChartsColor &c);

  double symbolFillAlpha() const { return symbolData_.background.alpha; }
  void setSymbolFillAlpha(double a);

  Pattern symbolFillPattern() const { return (Pattern) symbolData_.background.pattern; }
  void setSymbolFillPattern(const Pattern &p) {
    symbolData_.background.pattern = (CQChartsFillPattern::Type) p; update(); }

  QColor interpSymbolFillColor(int i, int n) const {
    return symbolFillColor().interpColor(this, i, n); }
  QColor interpSymbolFillColor(double r) const {
    return symbolFillColor().interpColor(this, r); }

  //---

  const CQChartsColumn &symbolSizeColumn() const { return valueSetColumn("symbolSize"); }
  void setSymbolSizeColumn(const CQChartsColumn &c) {
    if (setValueSetColumn("symbolSize", c)) updateRangeAndObjs(); }

  bool isSymbolSizeMapped() const { return isValueSetMapped("symbolSize"); }
  void setSymbolSizeMapped(bool b) { setValueSetMapped("symbolSize", b); updateObjs(); }

  double symbolSizeMapMin() const { return valueSetMapMin("symbolSize"); }
  void setSymbolSizeMapMin(double r) { setValueSetMapMin("symbolSize", r); updateObjs(); }

  double symbolSizeMapMax() const { return valueSetMapMax("symbolSize"); }
  void setSymbolSizeMapMax(double r) { setValueSetMapMax("symbolSize", r); updateObjs(); }

  //---

  const CQChartsColumn &colorColumn() const { return valueSetColumn("color"); }
  void setColorColumn(const CQChartsColumn &c) {
    if (setValueSetColumn("color", c)) updateRangeAndObjs(); }

  bool isColorMapped() const { return isValueSetMapped("color"); }
  void setColorMapped(bool b) { setValueSetMapped("color", b); updateObjs(); }

  double colorMapMin() const { return valueSetMapMin("color"); }
  void setColorMapMin(double r) { setValueSetMapMin("color", r); updateObjs(); }

  double colorMapMax() const { return valueSetMapMax("color"); }
  void setColorMapMax(double r) { setValueSetMapMax("color", r); updateObjs(); }

  //---

  bool isTextLabels() const { return dataLabel_.isVisible(); }
  void setTextLabels(bool b) { dataLabel_.setVisible(b); }

  //---

  const CQChartsColumn &fontSizeColumn() const { return valueSetColumn("fontSize"); }
  void setFontSizeColumn(const CQChartsColumn &c) {
    if (setValueSetColumn("fontSize", c)) updateRangeAndObjs(); }

  double fontSize() const { return fontSize_; }
  void setFontSize(double s);

  bool isFontSizeMapped() const { return isValueSetMapped("fontSize"); }
  void setFontSizeMapped(bool b) { setValueSetMapped("fontSize", b); updateObjs(); }

  double fontSizeMapMin() const { return valueSetMapMin("fontSize"); }
  void setFontSizeMapMin(double r) { setValueSetMapMin("fontSize", r); updateObjs(); }

  double fontSizeMapMax() const { return valueSetMapMax("fontSize"); }
  void setFontSizeMapMax(double r) { setValueSetMapMax("fontSize", r); updateObjs(); }

  //---

  void addNameValue(const QString &name, double x, double y, int row, const QModelIndex &xind,
                    const QString &symbolSizeStr, const QString &fontSizeStr,
                    const CQChartsColor &color=CQChartsColor()) {
    nameValues_[name].emplace_back(x, y, row, xind, symbolSizeStr, fontSizeStr, color);
  }

  const NameValues &nameValues() const { return nameValues_; }

  const QString &xname         () const { return xname_         ; }
  const QString &yname         () const { return yname_         ; }
  const QString &symbolSizeName() const { return symbolSizeName_; }
  const QString &fontSizeName  () const { return fontSizeName_  ; }
  const QString &colorName     () const { return colorName_     ; }

  //---

  const CQChartsDataLabel &dataLabel() const { return dataLabel_; }
  CQChartsDataLabel &dataLabel() { return dataLabel_; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void updateObjs() override;

  bool initObjs() override;

  //---

  int numRows() const;

  int nameIndex(const QString &name) const;

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  void draw(QPainter *) override;

  void drawForeground(QPainter *) override;

 private:
  CQChartsColumn    nameColumn_;           // name column
  CQChartsColumn    xColumn_      { 0 };   // x column
  CQChartsColumn    yColumn_      { 1 };   // y column
  double            symbolSize_   { 4.0 }; // symbol size
  CQChartsShapeData symbolData_;           // symbl draw data
  double            fontSize_     { 8.0 }; // font size
  NameValues        nameValues_;           // name values
  CQChartsDataLabel dataLabel_;            // data label style
  QString           xname_;                // x column header
  QString           yname_;                // y column header
  QString           symbolSizeName_;       // symbol size column header
  QString           fontSizeName_;         // font size column header
  QString           colorName_;            // color column header
};

#endif
