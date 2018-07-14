#ifndef CQChartsScatterPlot_H
#define CQChartsScatterPlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>

//---

class CQChartsScatterPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsScatterPlotType();

  QString name() const override { return "scatter"; }
  QString desc() const override { return "Scatter"; }

  Dimension dimension() const override { return Dimension::TWO_D; }

  void addParameters() override;

  QString description() const;

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
  CQChartsScatterPointObj(CQChartsScatterPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                          const QPointF &p, const CQChartsLength &symbolSize,
                          const OptReal &fontSize, const OptColor &color,
                          int ig, int ng, int is, int ns, int iv, int nv);

  const CQChartsLength &symbolSize() const { return symbolSize_; }
  void setSymbolSize(const CQChartsLength &s);

  const QString &name() const { return name_; }
  void setName(const QString &v) { name_ = v; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &v) { ind_ = v; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

 private:
  CQChartsScatterPlot* plot_       { nullptr };
  int                  groupInd_   { -1 };
  QPointF              p_;
  CQChartsLength       symbolSize_;
  OptReal              fontSize_;
  OptColor             color_;
  int                  ig_         { -1 };
  int                  ng_         { -1 };
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

class CQChartsScatterPlot : public CQChartsGroupPlot {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn xColumn          READ xColumn          WRITE setXColumn         )
  Q_PROPERTY(CQChartsColumn yColumn          READ yColumn          WRITE setYColumn         )
  Q_PROPERTY(CQChartsColumn nameColumn       READ nameColumn       WRITE setNameColumn      )
  Q_PROPERTY(CQChartsColumn symbolSizeColumn READ symbolSizeColumn WRITE setSymbolSizeColumn)
  Q_PROPERTY(CQChartsColumn fontSizeColumn   READ fontSizeColumn   WRITE setFontSizeColumn  )
  Q_PROPERTY(CQChartsColumn colorColumn      READ colorColumn      WRITE setColorColumn     )

  // options
  Q_PROPERTY(bool bestFit READ isBestFit WRITE setBestFit)

  // symbol
  Q_PROPERTY(CQChartsSymbol symbolType        READ symbolType        WRITE setSymbolType       )
  Q_PROPERTY(CQChartsLength symbolSize        READ symbolSize        WRITE setSymbolSize       )
  Q_PROPERTY(bool           symbolStroked     READ isSymbolStroked   WRITE setSymbolStroked    )
  Q_PROPERTY(CQChartsColor  symbolStrokeColor READ symbolStrokeColor WRITE setSymbolStrokeColor)
  Q_PROPERTY(double         symbolStrokeAlpha READ symbolStrokeAlpha WRITE setSymbolStrokeAlpha)
  Q_PROPERTY(CQChartsLength symbolStrokeWidth READ symbolStrokeWidth WRITE setSymbolStrokeWidth)
  Q_PROPERTY(bool           symbolFilled      READ isSymbolFilled    WRITE setSymbolFilled     )
  Q_PROPERTY(CQChartsColor  symbolFillColor   READ symbolFillColor   WRITE setSymbolFillColor  )
  Q_PROPERTY(double         symbolFillAlpha   READ symbolFillAlpha   WRITE setSymbolFillAlpha  )
  Q_PROPERTY(Pattern        symbolFillPattern READ symbolFillPattern WRITE setSymbolFillPattern)

  // labels
  Q_PROPERTY(double fontSize   READ fontSize     WRITE setFontSize  )
  Q_PROPERTY(bool   textLabels READ isTextLabels WRITE setTextLabels)

  // symbol size map
  Q_PROPERTY(bool   symbolMapKey     READ isSymbolMapKey     WRITE setSymbolMapKey    )
  Q_PROPERTY(bool   symbolSizeMapped READ isSymbolSizeMapped WRITE setSymbolSizeMapped)
  Q_PROPERTY(double symbolSizeMapMin READ symbolSizeMapMin   WRITE setSymbolSizeMapMin)
  Q_PROPERTY(double symbolSizeMapMax READ symbolSizeMapMax   WRITE setSymbolSizeMapMax)

  // color map
  Q_PROPERTY(bool   colorMapped READ isColorMapped WRITE setColorMapped)
  Q_PROPERTY(double colorMapMin READ colorMapMin   WRITE setColorMapMin)
  Q_PROPERTY(double colorMapMax READ colorMapMax   WRITE setColorMapMax)

  // font size map
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

  using Values          = std::vector<ValueData>;
  using NameValues      = std::map<QString,Values>;
  using GroupNameValues = std::map<int,NameValues>;

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

  //----

  bool isBestFit() const { return bestFit_; }

  //----

  // symbol
  const CQChartsSymbol &symbolType() const { return symbolData_.type; }
  void setSymbolType(const CQChartsSymbol &t);

  const CQChartsLength &symbolSize() const { return symbolData_.size; }
  void setSymbolSize(const CQChartsLength &s);

  //--

  bool isSymbolStroked() const { return symbolData_.stroke.visible; }
  void setSymbolStroked(bool b);

  const CQChartsColor &symbolStrokeColor() const { return symbolData_.stroke.color; }
  void setSymbolStrokeColor(const CQChartsColor &c);

  double symbolStrokeAlpha() const { return symbolData_.stroke.alpha; }
  void setSymbolStrokeAlpha(double a);

  const CQChartsLength &symbolStrokeWidth() const { return symbolData_.stroke.width; }
  void setSymbolStrokeWidth(const CQChartsLength &l);

  QColor interpSymbolStrokeColor(int i, int n) const {
    return symbolData_.stroke.color.interpColor(this, i, n); }

  //--

  bool isSymbolFilled() const { return symbolData_.fill.visible; }
  void setSymbolFilled(bool b);

  const CQChartsColor &symbolFillColor() const { return symbolData_.fill.color; }
  void setSymbolFillColor(const CQChartsColor &c);

  double symbolFillAlpha() const { return symbolData_.fill.alpha; }
  void setSymbolFillAlpha(double a);

  Pattern symbolFillPattern() const;
  void setSymbolFillPattern(const Pattern &p);

  QColor interpSymbolFillColor(int i, int n) const {
    return symbolFillColor().interpColor(this, i, n); }
  QColor interpSymbolFillColor(double r) const {
    return symbolFillColor().interpColor(this, r); }

  //---

  bool isSymbolMapKey() const { return symbolMapKey_; }
  void setSymbolMapKey(bool b);

  const CQChartsColumn &symbolSizeColumn() const { return valueSetColumn("symbolSize"); }
  void setSymbolSizeColumn(const CQChartsColumn &c);

  bool isSymbolSizeMapped() const { return isValueSetMapped("symbolSize"); }
  void setSymbolSizeMapped(bool b);

  double symbolSizeMapMin() const { return valueSetMapMin("symbolSize"); }
  void setSymbolSizeMapMin(double r);

  double symbolSizeMapMax() const { return valueSetMapMax("symbolSize"); }
  void setSymbolSizeMapMax(double r);

  //---

  const CQChartsColumn &colorColumn() const { return valueSetColumn("color"); }
  void setColorColumn(const CQChartsColumn &c);

  bool isColorMapped() const { return isValueSetMapped("color"); }
  void setColorMapped(bool b);

  double colorMapMin() const { return valueSetMapMin("color"); }
  void setColorMapMin(double r);

  double colorMapMax() const { return valueSetMapMax("color"); }
  void setColorMapMax(double r);

  //---

  bool isTextLabels() const { return dataLabel_.isVisible(); }
  void setTextLabels(bool b);

  //---

  const CQChartsColumn &fontSizeColumn() const { return valueSetColumn("fontSize"); }
  void setFontSizeColumn(const CQChartsColumn &c);

  double fontSize() const { return fontSize_; }
  void setFontSize(double s);

  bool isFontSizeMapped() const { return isValueSetMapped("fontSize"); }
  void setFontSizeMapped(bool b);

  double fontSizeMapMin() const { return valueSetMapMin("fontSize"); }
  void setFontSizeMapMin(double r);

  double fontSizeMapMax() const { return valueSetMapMax("fontSize"); }
  void setFontSizeMapMax(double r);

  //---

  void addNameValue(int groupInd, const QString &name, double x, double y, int row,
                    const QModelIndex &xind, const QString &symbolSizeStr,
                    const QString &fontSizeStr, const CQChartsColor &color=CQChartsColor());

  const GroupNameValues &groupNameValues() const { return groupNameValues_; }

  //---

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

  void addNameValues();

  //---

  int numRows() const;

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  void drawBackground(QPainter *painter) override;
  void drawForeground(QPainter *painter) override;

 private:
  void drawBestFit     (QPainter *painter);
  void drawSymbolMapKey(QPainter *painter);

 public slots:
  void setBestFit(bool b);

 private:
  struct FitData {
    bool   fitted         { false };
    double coeffs     [8] {0, 0, 0, 0, 0, 0, 0, 0};
    int    coeffs_free[8] {1, 1, 1, 1, 1, 1, 1, 1};
    int    num_coeffs     { 3 };

    void reset() { fitted = false; }
  };

  using Points = std::vector<QPointF>;

  CQChartsColumn     nameColumn_;             // name column
  CQChartsColumn     xColumn_      { 0 };     // x column
  CQChartsColumn     yColumn_      { 1 };     // y column
  bool               bestFit_      { false }; // best fit
  CQChartsSymbolData symbolData_;             // symbol draw data
  double             fontSize_     { 8.0 };   // font size
  GroupNameValues    groupNameValues_;        // name values
  CQChartsDataLabel  dataLabel_;              // data label style
  QString            xname_;                  // x column header
  QString            yname_;                  // y column header
  bool               symbolMapKey_ { true };  // draw symbol map key
  QString            symbolSizeName_;         // symbol size column header
  QString            fontSizeName_;           // font size column header
  QString            colorName_;              // color column header
  Points             points_;
  FitData            fitData_;
};

#endif
