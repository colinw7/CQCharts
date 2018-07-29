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

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsScatterPlot;

class CQChartsScatterPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  enum Dir {
    X  = (1<<0),
    Y  = (1<<1),
    XY = (X | Y)
  };

  using OptReal = boost::optional<double>;

 public:
  CQChartsScatterPointObj(CQChartsScatterPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                          const QPointF &p, const CQChartsSymbol &symbolType,
                          const CQChartsLength &symbolSize, const OptReal &fontSize,
                          const CQChartsColor &color,
                          int ig, int ng, int is, int ns, int iv, int nv);

  const CQChartsSymbol &symbolType() const { return symbolType_; }
  void setSymbolType(const CQChartsSymbol &s) { symbolType_ = s; }

  const CQChartsLength &symbolSize() const { return symbolSize_; }
  void setSymbolSize(const CQChartsLength &s);

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  const QModelIndex &ind() const { return ind_; }
  void setInd(const QModelIndex &i) { ind_ = i; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  void drawDir(QPainter *painter, const Dir &dir) const;

 private:
  CQChartsScatterPlot* plot_       { nullptr };
  int                  groupInd_   { -1 };
  QPointF              p_;
  CQChartsSymbol       symbolType_;
  CQChartsLength       symbolSize_;
  OptReal              fontSize_;
  CQChartsColor        color_;
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
  void setColor(const CQChartsColor &c) { color_ = c; }

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
  Q_PROPERTY(CQChartsColumn symbolTypeColumn READ symbolTypeColumn WRITE setSymbolTypeColumn)
  Q_PROPERTY(CQChartsColumn symbolSizeColumn READ symbolSizeColumn WRITE setSymbolSizeColumn)
  Q_PROPERTY(CQChartsColumn fontSizeColumn   READ fontSizeColumn   WRITE setFontSizeColumn  )
  Q_PROPERTY(CQChartsColumn colorColumn      READ colorColumn      WRITE setColorColumn     )

  // options
  Q_PROPERTY(bool bestFit READ isBestFit WRITE setBestFit)

  // rug
  Q_PROPERTY(bool           xRug          READ isXRug        WRITE setXRug         )
  Q_PROPERTY(bool           yRug          READ isYRug        WRITE setYRug         )
  Q_PROPERTY(CQChartsSymbol rugSymbolType READ rugSymbolType WRITE setRugSymbolType)
  Q_PROPERTY(CQChartsLength rugSymbolSize READ rugSymbolSize WRITE setRugSymbolSize)

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

  Q_PROPERTY(bool symbolMapKey READ isSymbolMapKey WRITE setSymbolMapKey)

  // symbol type map
  Q_PROPERTY(bool   symbolTypeMapped READ isSymbolTypeMapped WRITE setSymbolTypeMapped)
  Q_PROPERTY(double symbolTypeMapMin READ symbolTypeMapMin   WRITE setSymbolTypeMapMin)
  Q_PROPERTY(double symbolTypeMapMax READ symbolTypeMapMax   WRITE setSymbolTypeMapMax)

  // symbol size map
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
    QString       symbolTypeStr;
    QString       symbolSizeStr;
    QString       fontSizeStr;
    CQChartsColor color;

    ValueData(double x, double y, int i, const QModelIndex &ind, const QString &symbolTypeStr="",
              const QString &symbolSizeStr="", const QString &fontSizeStr="",
              const CQChartsColor &color=CQChartsColor()) :
     p(x, y), i(i), ind(ind), symbolTypeStr(symbolTypeStr), symbolSizeStr(symbolSizeStr),
     fontSizeStr(fontSizeStr), color(color) {
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

  //---

  bool isBestFit() const { return bestFit_; }

  //---

  bool isXRug() const { return xRug_; }
  bool isYRug() const { return yRug_; }

  const CQChartsSymbol &rugSymbolType() const { return rugSymbolType_; }
  void setRugSymbolType(const CQChartsSymbol &s);

  const CQChartsLength &rugSymbolSize() const { return rugSymbolSize_; }
  void setRugSymbolSize(const CQChartsLength &r);

  //---

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

  //---

  const CQChartsColumn &symbolTypeColumn() const { return valueSetColumn("symbolType"); }
  void setSymbolTypeColumn(const CQChartsColumn &c);

  bool isSymbolTypeMapped() const { return isValueSetMapped("symbolType"); }
  void setSymbolTypeMapped(bool b);

  double symbolTypeMapMin() const { return valueSetMapMin("symbolType"); }
  void setSymbolTypeMapMin(double r);

  double symbolTypeMapMax() const { return valueSetMapMax("symbolType"); }
  void setSymbolTypeMapMax(double r);

  //---

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
                    const QModelIndex &xind, const QString &symbolTypeStr,
                    const QString &symbolSizeStr, const QString &fontSizeStr,
                    const CQChartsColor &color=CQChartsColor());

  const GroupNameValues &groupNameValues() const { return groupNameValues_; }

  //---

  const QString &xname         () const { return xname_         ; }
  const QString &yname         () const { return yname_         ; }
  const QString &symbolTypeName() const { return symbolTypeName_; }
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

  CQChartsGeom::BBox annotationBBox() const override;

  //---

  void drawBackground(QPainter *painter) override;
  void drawForeground(QPainter *painter) override;

 private:
  void drawBestFit     (QPainter *painter);
  void drawXRug        (QPainter *painter);
  void drawYRug        (QPainter *painter);
  void drawSymbolMapKey(QPainter *painter);

 public slots:
  void setBestFit(bool b);

  void setXRug(bool b);
  void setYRug(bool b);

 private:
  struct FitData {
    bool   fitted         { false };
    double coeffs     [8] {0, 0, 0, 0, 0, 0, 0, 0};
    int    coeffs_free[8] {1, 1, 1, 1, 1, 1, 1, 1};
    int    num_coeffs     { 3 };

    void reset() { fitted = false; }
  };

  using Points = std::vector<QPointF>;

  CQChartsColumn     nameColumn_;              // name column
  CQChartsColumn     xColumn_       { 0 };     // x column
  CQChartsColumn     yColumn_       { 1 };     // y column
  bool               bestFit_       { false }; // best fit
  bool               xRug_          { false }; // x rug
  bool               yRug_          { false }; // y rug
  CQChartsSymbol     rugSymbolType_;           // rug symbol type
  CQChartsLength     rugSymbolSize_ { "5px" }; // rug symbol size
  CQChartsSymbolData symbolData_;              // symbol draw data
  double             fontSize_      { 8.0 };   // font size
  GroupNameValues    groupNameValues_;         // name values
  CQChartsDataLabel  dataLabel_;               // data label style
  QString            xname_;                   // x column header
  QString            yname_;                   // y column header
  bool               symbolMapKey_  { true };  // draw symbol map key
  QString            symbolTypeName_;          // symbol type column header
  QString            symbolSizeName_;          // symbol size column header
  QString            fontSizeName_;            // font size column header
  QString            colorName_;               // color column header
  Points             points_;                  // fit points
  FitData            fitData_;                 // fit data
};

#endif
