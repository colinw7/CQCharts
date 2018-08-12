#ifndef CQChartsScatterPlot_H
#define CQChartsScatterPlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>
#include <CQChartsGrahamHull.h>
#include <CQChartsBoxWhisker.h>
#include <CQChartsLeastSquaresFit.h>

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

  Q_PROPERTY(QPointF point READ point)
  Q_PROPERTY(QString name  READ name )

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

  int groupInd() const { return groupInd_; }

  const QPointF &point() const { return p_; }

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

  void drawDir(QPainter *painter, const Dir &dir, bool flip=false) const;

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

  // best fit
  Q_PROPERTY(bool          bestFit          READ isBestFit          WRITE setBestFit         )
  Q_PROPERTY(bool          bestFitDeviation READ isBestFitDeviation WRITE setBestFitDeviation)
  Q_PROPERTY(int           bestFitOrder     READ bestFitOrder       WRITE setBestFitOrder    )
  Q_PROPERTY(CQChartsColor bestFitFillColor READ bestFitFillColor   WRITE setBestFitFillColor)
  Q_PROPERTY(double        bestFitFillAlpha READ bestFitFillAlpha   WRITE setBestFitFillAlpha)

  // convex hull
  Q_PROPERTY(bool          hull          READ isHull        WRITE setHull         )
  Q_PROPERTY(CQChartsColor hullFillColor READ hullFillColor WRITE setHullFillColor)
  Q_PROPERTY(double        hullFillAlpha READ hullFillAlpha WRITE setHullFillAlpha)

  // axis rug
  Q_PROPERTY(bool           xRug          READ isXRug        WRITE setXRug         )
  Q_PROPERTY(YSide          xRugSide      READ xRugSide      WRITE setXRugSide     )
  Q_PROPERTY(bool           yRug          READ isYRug        WRITE setYRug         )
  Q_PROPERTY(XSide          yRugSide      READ yRugSide      WRITE setYRugSide     )
  Q_PROPERTY(CQChartsSymbol rugSymbolType READ rugSymbolType WRITE setRugSymbolType)
  Q_PROPERTY(CQChartsLength rugSymbolSize READ rugSymbolSize WRITE setRugSymbolSize)

  // axis density
  Q_PROPERTY(bool           xDensity     READ isXDensity   WRITE setXDensity    )
  Q_PROPERTY(YSide          xDensitySide READ xDensitySide WRITE setXDensitySide)
  Q_PROPERTY(bool           yDensity     READ isYDensity   WRITE setYDensity    )
  Q_PROPERTY(XSide          yDensitySide READ yDensitySide WRITE setYDensitySide)
  Q_PROPERTY(CQChartsLength densityWidth READ densityWidth WRITE setDensityWidth)
  Q_PROPERTY(double         densityAlpha READ densityAlpha WRITE setDensityAlpha)

  // axis whisker
  Q_PROPERTY(bool           xWhisker      READ isXWhisker    WRITE setXWhisker     )
  Q_PROPERTY(YSide          xWhiskerSide  READ xWhiskerSide  WRITE setXWhiskerSide )
  Q_PROPERTY(bool           yWhisker      READ isYWhisker    WRITE setYWhisker     )
  Q_PROPERTY(XSide          yWhiskerSide  READ yWhiskerSide  WRITE setYWhiskerSide )
  Q_PROPERTY(CQChartsLength whiskerWidth  READ whiskerWidth  WRITE setWhiskerWidth )
  Q_PROPERTY(CQChartsLength whiskerMargin READ whiskerMargin WRITE setWhiskerMargin)
  Q_PROPERTY(double         whiskerAlpha  READ whiskerAlpha  WRITE setWhiskerAlpha )

  // symbol
  Q_PROPERTY(CQChartsSymbol   symbolType        READ symbolType        WRITE setSymbolType       )
  Q_PROPERTY(CQChartsLength   symbolSize        READ symbolSize        WRITE setSymbolSize       )
  Q_PROPERTY(bool             symbolStroked     READ isSymbolStroked   WRITE setSymbolStroked    )
  Q_PROPERTY(CQChartsColor    symbolStrokeColor READ symbolStrokeColor WRITE setSymbolStrokeColor)
  Q_PROPERTY(double           symbolStrokeAlpha READ symbolStrokeAlpha WRITE setSymbolStrokeAlpha)
  Q_PROPERTY(CQChartsLength   symbolStrokeWidth READ symbolStrokeWidth WRITE setSymbolStrokeWidth)
  Q_PROPERTY(CQChartsLineDash symbolStrokeDash  READ symbolStrokeDash  WRITE setSymbolStrokeDash )
  Q_PROPERTY(bool             symbolFilled      READ isSymbolFilled    WRITE setSymbolFilled     )
  Q_PROPERTY(CQChartsColor    symbolFillColor   READ symbolFillColor   WRITE setSymbolFillColor  )
  Q_PROPERTY(double           symbolFillAlpha   READ symbolFillAlpha   WRITE setSymbolFillAlpha  )
  Q_PROPERTY(Pattern          symbolFillPattern READ symbolFillPattern WRITE setSymbolFillPattern)

  // symbol map key
  Q_PROPERTY(bool   symbolMapKey       READ isSymbolMapKey     WRITE setSymbolMapKey      )
  Q_PROPERTY(double symbolMapKeyAlpha  READ symbolMapKeyAlpha  WRITE setSymbolMapKeyAlpha )
  Q_PROPERTY(double symbolMapKeyMargin READ symbolMapKeyMargin WRITE setSymbolMapKeyMargin)

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

  Q_ENUMS(XSide)
  Q_ENUMS(YSide)
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

  enum XSide {
    LEFT,
    RIGHT
  };

  enum YSide {
    BOTTOM,
    TOP
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

  // best fit
  bool isBestFit() const { return bestFit_; }

  bool isBestFitDeviation() const { return bestFitDeviation_; }
  void setBestFitDeviation(bool b);

  int bestFitOrder() const { return bestFitOrder_; }
  void setBestFitOrder(int o);

  const CQChartsColor &bestFitFillColor() const { return bestFitFillColor_; }
  void setBestFitFillColor(const CQChartsColor &c);

  double bestFitFillAlpha() const { return bestFitFillAlpha_; }
  void setBestFitFillAlpha(double r);

  //---

  // convex hull
  bool isHull() const { return hull_; }

  const CQChartsColor &hullFillColor() const { return hullFillColor_; }
  void setHullFillColor(const CQChartsColor &c);

  double hullFillAlpha() const { return hullFillAlpha_; }
  void setHullFillAlpha(double a);

  //---

  // axis rug
  bool isXRug() const { return xRug_; }
  bool isYRug() const { return yRug_; }

  const YSide &xRugSide() const { return xRugSide_; }
  void setXRugSide(const YSide &s);

  const XSide &yRugSide() const { return yRugSide_; }
  void setYRugSide(const XSide &s);

  const CQChartsSymbol &rugSymbolType() const { return rugSymbolType_; }
  void setRugSymbolType(const CQChartsSymbol &s);

  const CQChartsLength &rugSymbolSize() const { return rugSymbolSize_; }
  void setRugSymbolSize(const CQChartsLength &r);

  //---

  // axis density
  bool isXDensity() const { return xDensity_; }
  bool isYDensity() const { return yDensity_; }

  const YSide &xDensitySide() const { return xDensitySide_; }
  void setXDensitySide(const YSide &s);

  const XSide &yDensitySide() const { return yDensitySide_; }
  void setYDensitySide(const XSide &s);

  const CQChartsLength &densityWidth() const { return densityWidth_; }
  void setDensityWidth(const CQChartsLength &w);

  double densityAlpha() const { return densityAlpha_; }
  void setDensityAlpha(double a);

  //---

  // axis whisker
  bool isXWhisker() const { return xWhisker_; }
  bool isYWhisker() const { return yWhisker_; }

  const YSide &xWhiskerSide() const { return xWhiskerSide_; }
  void setXWhiskerSide(const YSide &s);

  const XSide &yWhiskerSide() const { return yWhiskerSide_; }
  void setYWhiskerSide(const XSide &s);

  const CQChartsLength &whiskerWidth() const { return whiskerWidth_; }
  void setWhiskerWidth(const CQChartsLength &w);

  const CQChartsLength &whiskerMargin() const { return whiskerMargin_; }
  void setWhiskerMargin(const CQChartsLength &w);

  double whiskerAlpha() const { return whiskerAlpha_; }
  void setWhiskerAlpha(double a);

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

  const CQChartsLineDash &symbolStrokeDash() const { return symbolData_.stroke.dash; }
  void setSymbolStrokeDash(const CQChartsLineDash &d);

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

  bool isSymbolMapKey() const { return symbolMapKeyData_.displayed; }
  void setSymbolMapKey(bool b);

  double symbolMapKeyAlpha() const { return symbolMapKeyData_.alpha; }
  void setSymbolMapKeyAlpha(double r);

  double symbolMapKeyMargin() const { return symbolMapKeyData_.margin; }
  void setSymbolMapKeyMargin(double r);

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

  const CQChartsColumn &fontSizeColumn() const { return valueSetColumn("fontSize"); }
  void setFontSizeColumn(const CQChartsColumn &c);

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

  void calcRange() override;

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
  void drawHull        (QPainter *painter);
  void drawXRug        (QPainter *painter);
  void drawYRug        (QPainter *painter);
  void drawXDensity    (QPainter *painter);
  void drawYDensity    (QPainter *painter);
  void drawXWhisker    (QPainter *painter);
  void drawYWhisker    (QPainter *painter);
  void drawSymbolMapKey(QPainter *painter);

 public slots:
  void setBestFit(bool b);
  void setHull   (bool b);

  void setXRug(bool b);
  void setYRug(bool b);

  void setXDensity(bool b);
  void setYDensity(bool b);

  void setXWhisker(bool b);
  void setYWhisker(bool b);

 private:
  struct SymbolMapKeyData {
    bool   displayed { true };
    double alpha     { 0.2 };
    double margin    { 16 };
  };

  struct WhiskerData {
    CQChartsBoxWhisker xWhisker;
    CQChartsBoxWhisker yWhisker;
  };

  using Points        = std::vector<QPointF>;
  using GroupPoints   = std::map<int,Points>;
  using GroupFitData  = std::map<int,CQChartsLeastSquaresFit>;
  using GroupHull     = std::map<int,CQChartsGrahamHull>;
  using GroupWhiskers = std::map<int,WhiskerData>;

  CQChartsColumn     nameColumn_;                         // name column
  CQChartsColumn     xColumn_          { 0 };             // x column
  CQChartsColumn     yColumn_          { 1 };             // y column
  bool               bestFit_          { false };         // show best fit
  bool               bestFitDeviation_ { false };         // show best fit deviation
  int                bestFitOrder_     { 3 };             // best fit order
  CQChartsColor      bestFitFillColor_;                   // best fit fill color
  double             bestFitFillAlpha_ { 1.0 };           // best fit fill alpha
  bool               hull_             { false };         // show convex hull
  CQChartsColor      hullFillColor_;                      // hull fill color
  double             hullFillAlpha_    { 1.0 };           // hull fill alpha
  bool               xRug_             { false };         // x rug
  YSide              xRugSide_         { YSide::BOTTOM }; // x rug side
  bool               yRug_             { false };         // y rug
  XSide              yRugSide_         { XSide::LEFT };   // y rug side
  CQChartsSymbol     rugSymbolType_;                      // rug symbol type
  CQChartsLength     rugSymbolSize_    { "5px" };         // rug symbol size
  bool               xDensity_         { false };         // x density
  YSide              xDensitySide_     { YSide::TOP };    // x density side
  bool               yDensity_         { false };         // y density
  XSide              yDensitySide_     { XSide::RIGHT };  // y density side
  CQChartsLength     densityWidth_     { "48px" };        // density width
  double             densityAlpha_     { 0.5 };           // density alpha
  bool               xWhisker_         { false };         // x whisker
  YSide              xWhiskerSide_     { YSide::TOP };    // x whisker side
  bool               yWhisker_         { false };         // y whisker
  XSide              yWhiskerSide_     { XSide::RIGHT };  // y whisker side
  CQChartsLength     whiskerWidth_     { "24px" };        // whisker width
  CQChartsLength     whiskerMargin_    { "8px" };         // whisker margin
  double             whiskerAlpha_     { 0.5 };           // whisker alpha
  CQChartsSymbolData symbolData_;                         // symbol draw data
  GroupNameValues    groupNameValues_;                    // name values
  CQChartsDataLabel  dataLabel_;                          // data label style
  QString            xname_;                              // x column header
  QString            yname_;                              // y column header
  SymbolMapKeyData   symbolMapKeyData_;                   // symbol map key data
  QString            symbolTypeName_;                     // symbol type column header
  QString            symbolSizeName_;                     // symbol size column header
  QString            fontSizeName_;                       // font size column header
  QString            colorName_;                          // color column header
  GroupPoints        groupPoints_;                        // group fit points
  GroupFitData       groupFitData_;                       // group fit data
  GroupHull          groupHull_;                          // group hull
  GroupWhiskers      groupWhiskers_;                      // group whiskers
};

#endif
