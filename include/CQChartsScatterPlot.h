#ifndef CQChartsScatterPlot_H
#define CQChartsScatterPlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>
#include <CQChartsGrahamHull.h>
#include <CQChartsBoxWhisker.h>
#include <CQChartsFitData.h>
#include <CInterval.h>

class CQChartsScatterPlot;

//---

class CQChartsScatterPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsScatterPlotType();

  QString name() const override { return "scatter"; }
  QString desc() const override { return "Scatter"; }

  Dimension dimension() const override { return Dimension::TWO_D; }

  void addParameters() override;

  QString description() const override;

  const char *xColumnName() const override { return "x"; }
  const char *yColumnName() const override { return "y"; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsScatterPointObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(int     groupInd READ groupInd)
  Q_PROPERTY(QPointF point    READ point   )
  Q_PROPERTY(QString name     READ name    )

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

class CQChartsScatterCellObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  enum Dir {
    X  = (1<<0),
    Y  = (1<<1),
    XY = (X | Y)
  };

  using Points = std::vector<QPointF>;

 public:
  CQChartsScatterCellObj(CQChartsScatterPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                         int ig, int ng, int is, int ns, int ix, int iy,
                         const Points &points, int maxn);

  int groupInd() const { return groupInd_; }

  const Points &points() const { return points_; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  void drawRugSymbol(QPainter *painter, const Dir &dir, bool flip) const;

 private:
  CQChartsScatterPlot* plot_     { nullptr };
  int                  groupInd_ { -1 };
  int                  ig_       { -1 };
  int                  ng_       { -1 };
  int                  is_       { -1 };
  int                  ns_       { -1 };
  int                  ix_       { -1 };
  int                  iy_       { -1 };
  Points               points_;
  int                  maxn_     { 0 };
};

//---

#include <CQChartsKey.h>

class CQChartsScatterKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int groupInd, int i, int n);

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &c) { color_ = c; }

  bool selectPress(const CQChartsGeom::Point &p, CQChartsSelMod selMod) override;

  QBrush fillBrush() const override;

 private:
  int hideIndex() const;

 private:
  int           groupInd_ { -1 };
  CQChartsColor color_;
};

class CQChartsScatterGridKeyItem : public CQChartsKeyItem {
  Q_OBJECT

 public:
  CQChartsScatterGridKeyItem(CQChartsScatterPlot *plot);

  QSizeF size() const override;

  void draw(QPainter *painter, const CQChartsGeom::BBox &rect) override;

 private:
  CQChartsScatterPlot *plot_ { nullptr };
};

//---

CQCHARTS_NAMED_SHAPE_DATA(BestFit,bestFit)
CQCHARTS_NAMED_SHAPE_DATA(Hull,hull)

class CQChartsScatterPlot : public CQChartsGroupPlot,
 public CQChartsPlotPointData       <CQChartsScatterPlot>,
 public CQChartsPlotBestFitShapeData<CQChartsScatterPlot>,
 public CQChartsPlotHullShapeData   <CQChartsScatterPlot>,
 public CQChartsPlotRugPointData    <CQChartsScatterPlot> {
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
  Q_PROPERTY(bool bestFit          READ isBestFit          WRITE setBestFit         )
  Q_PROPERTY(bool bestFitDeviation READ isBestFitDeviation WRITE setBestFitDeviation)
  Q_PROPERTY(int  bestFitOrder     READ bestFitOrder       WRITE setBestFitOrder    )

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(BestFit,bestFit)

  // convex hull
  Q_PROPERTY(bool hull READ isHull WRITE setHull)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Hull,hull)

  // axis rug
  Q_PROPERTY(bool  xRug     READ isXRug   WRITE setXRug    )
  Q_PROPERTY(YSide xRugSide READ xRugSide WRITE setXRugSide)
  Q_PROPERTY(bool  yRug     READ isYRug   WRITE setYRug    )
  Q_PROPERTY(XSide yRugSide READ yRugSide WRITE setYRugSide)

  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Rug,rug)

  // axis density
  Q_PROPERTY(bool           xDensity     READ isXDensity   WRITE setXDensity    )
  Q_PROPERTY(YSide          xDensitySide READ xDensitySide WRITE setXDensitySide)
  Q_PROPERTY(bool           yDensity     READ isYDensity   WRITE setYDensity    )
  Q_PROPERTY(XSide          yDensitySide READ yDensitySide WRITE setYDensitySide)
  Q_PROPERTY(CQChartsLength densityWidth READ densityWidth WRITE setDensityWidth)
  Q_PROPERTY(double         densityAlpha READ densityAlpha WRITE setDensityAlpha)

  // density map
  Q_PROPERTY(bool   densityMap         READ isDensityMap       WRITE setDensityMap        )
  Q_PROPERTY(int    densityMapGridSize READ densityMapGridSize WRITE setDensityMapGridSize)
  Q_PROPERTY(double densityMapDelta    READ densityMapDelta    WRITE setDensityMapDelta   )

  // axis whisker
  Q_PROPERTY(bool           xWhisker      READ isXWhisker    WRITE setXWhisker     )
  Q_PROPERTY(YSide          xWhiskerSide  READ xWhiskerSide  WRITE setXWhiskerSide )
  Q_PROPERTY(bool           yWhisker      READ isYWhisker    WRITE setYWhisker     )
  Q_PROPERTY(XSide          yWhiskerSide  READ yWhiskerSide  WRITE setYWhiskerSide )
  Q_PROPERTY(CQChartsLength whiskerWidth  READ whiskerWidth  WRITE setWhiskerWidth )
  Q_PROPERTY(CQChartsLength whiskerMargin READ whiskerMargin WRITE setWhiskerMargin)
  Q_PROPERTY(double         whiskerAlpha  READ whiskerAlpha  WRITE setWhiskerAlpha )

  // symbol
  CQCHARTS_POINT_DATA_PROPERTIES

  // grid
  Q_PROPERTY(bool gridded  READ isGridded WRITE setGridded )
  Q_PROPERTY(int  gridNumX READ gridNumX  WRITE setGridNumX)
  Q_PROPERTY(int  gridNumY READ gridNumY  WRITE setGridNumY)

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
  CQCHARTS_COLOR_MAP_PROPERTIES

  // font size map
  Q_PROPERTY(bool   fontSizeMapped READ isFontSizeMapped WRITE setFontSizeMapped)
  Q_PROPERTY(double fontSizeMapMin READ fontSizeMapMin   WRITE setFontSizeMapMin)
  Q_PROPERTY(double fontSizeMapMax READ fontSizeMapMax   WRITE setFontSizeMapMax)

  Q_ENUMS(XSide)
  Q_ENUMS(YSide)

 public:
  struct ValueData {
    QPointF       p;
    int           i;
    QModelIndex   ind;
    CQChartsColor color;

    ValueData(const QPointF &p, int i, const QModelIndex &ind,
              const CQChartsColor &color=CQChartsColor()) :
     p(p), i(i), ind(ind), color(color) {
    }
  };

  using Values = std::vector<ValueData>;

  struct ValuesData {
    Values                values;
    CQChartsGeom::RMinMax xrange;
    CQChartsGeom::RMinMax yrange;
  };

  using NameValues      = std::map<QString,ValuesData>;
  using GroupNameValues = std::map<int,NameValues>;

  using Points   = std::vector<QPointF>;
  using YPoints  = std::map<int,Points>;
  using XYPoints = std::map<int,YPoints>;

  struct CellPointData {
    int      maxN { 0 };
    XYPoints xyPoints;
  };

  using NameGridData      = std::map<QString,CellPointData>;
  using GroupNameGridData = std::map<int,NameGridData>;

  struct GridData {
    bool      enabled   { false };
    CInterval xinterval;
    CInterval yinterval;
    int       nx        { 40 };
    int       ny        { 40 };
    int       maxN      { 0 };
  };

  //---

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

  // columns
  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c);

  const CQChartsColumn &yColumn() const { return yColumn_; }
  void setYColumn(const CQChartsColumn &c);

  //---

  // best fit
  bool isBestFit() const { return bestFitData_.visible; }

  bool isBestFitDeviation() const { return bestFitData_.showDeviation; }
  void setBestFitDeviation(bool b);

  int bestFitOrder() const { return bestFitData_.order; }
  void setBestFitOrder(int o);

  //---

  // convex hull
  bool isHull() const { return hullData_.visible; }

  //---

  // axis rug
  bool isXRug() const { return axisRugData_.xVisible; }
  bool isYRug() const { return axisRugData_.yVisible; }

  const YSide &xRugSide() const { return axisRugData_.xSide; }
  void setXRugSide(const YSide &s);

  const XSide &yRugSide() const { return axisRugData_.ySide; }
  void setYRugSide(const XSide &s);

  //---

  // axis density
  bool isXDensity() const { return axisDensityData_.xVisible; }
  bool isYDensity() const { return axisDensityData_.yVisible; }

  const YSide &xDensitySide() const { return axisDensityData_.xSide; }
  void setXDensitySide(const YSide &s);

  const XSide &yDensitySide() const { return axisDensityData_.ySide; }
  void setYDensitySide(const XSide &s);

  const CQChartsLength &densityWidth() const { return axisDensityData_.width; }
  void setDensityWidth(const CQChartsLength &w);

  double densityAlpha() const { return axisDensityData_.alpha; }
  void setDensityAlpha(double a);

  //---

  // density map
  bool isDensityMap() const { return densityMapData_.visible; }

  int densityMapGridSize() const { return densityMapData_.gridSize; }
  void setDensityMapGridSize(int i);

  double densityMapDelta() const { return densityMapData_.delta; }
  void setDensityMapDelta(double d);

  //---

  // axis whisker
  bool isXWhisker() const { return axisWhiskerData_.xVisible; }
  bool isYWhisker() const { return axisWhiskerData_.yVisible; }

  const YSide &xWhiskerSide() const { return axisWhiskerData_.xSide; }
  void setXWhiskerSide(const YSide &s);

  const XSide &yWhiskerSide() const { return axisWhiskerData_.ySide; }
  void setYWhiskerSide(const XSide &s);

  const CQChartsLength &whiskerWidth() const { return axisWhiskerData_.width; }
  void setWhiskerWidth(const CQChartsLength &w);

  const CQChartsLength &whiskerMargin() const { return axisWhiskerData_.margin; }
  void setWhiskerMargin(const CQChartsLength &w);

  double whiskerAlpha() const { return axisWhiskerData_.alpha; }
  void setWhiskerAlpha(double a);

  //---

  // gridded
  bool isGridded() const { return gridData_.enabled; }

  int gridNumX() const { return gridData_.nx; }
  void setGridNumX(int n);

  int gridNumY() const { return gridData_.ny; }
  void setGridNumY(int n);

  //---

  // symbol map key
  bool isSymbolMapKey() const { return symbolMapKeyData_.displayed; }
  void setSymbolMapKey(bool b);

  double symbolMapKeyAlpha() const { return symbolMapKeyData_.alpha; }
  void setSymbolMapKeyAlpha(double r);

  double symbolMapKeyMargin() const { return symbolMapKeyData_.margin; }
  void setSymbolMapKeyMargin(double r);

  //---

  // symbol type column and map
  const CQChartsColumn &symbolTypeColumn() const { return valueSetColumn("symbolType"); }
  void setSymbolTypeColumn(const CQChartsColumn &c);

  bool isSymbolTypeMapped() const { return isValueSetMapped("symbolType"); }
  void setSymbolTypeMapped(bool b);

  double symbolTypeMapMin() const { return valueSetMapMin("symbolType"); }
  void setSymbolTypeMapMin(double r);

  double symbolTypeMapMax() const { return valueSetMapMax("symbolType"); }
  void setSymbolTypeMapMax(double r);

  //---

  // symbol size column and map
  const CQChartsColumn &symbolSizeColumn() const { return valueSetColumn("symbolSize"); }
  void setSymbolSizeColumn(const CQChartsColumn &c);

  bool isSymbolSizeMapped() const { return isValueSetMapped("symbolSize"); }
  void setSymbolSizeMapped(bool b);

  double symbolSizeMapMin() const { return valueSetMapMin("symbolSize"); }
  void setSymbolSizeMapMin(double r);

  double symbolSizeMapMax() const { return valueSetMapMax("symbolSize"); }
  void setSymbolSizeMapMax(double r);

  //---

  // color column and map
  const CQChartsColumn &colorColumn() const { return valueSetColumn("color"); }
  void setColorColumn(const CQChartsColumn &c);

  bool isColorMapped() const { return isValueSetMapped("color"); }
  void setColorMapped(bool b);

  double colorMapMin() const { return valueSetMapMin("color"); }
  void setColorMapMin(double r);

  double colorMapMax() const { return valueSetMapMax("color"); }
  void setColorMapMax(double r);

  //---

  // font size column and map
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
                    const QModelIndex &xind, const CQChartsColor &color=CQChartsColor());

  const GroupNameValues &groupNameValues() const { return groupNameValues_; }

  //---

  // cached column names
  const QString &xname         () const { return xname_         ; }
  const QString &yname         () const { return yname_         ; }
  const QString &symbolTypeName() const { return symbolTypeName_; }
  const QString &symbolSizeName() const { return symbolSizeName_; }
  const QString &fontSizeName  () const { return fontSizeName_  ; }
  const QString &colorName     () const { return colorName_     ; }

  //---

  // data label
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

  bool hasBackground() const override;

  void drawBackground(QPainter *painter) override;

  bool hasForeground() const override;

  void drawForeground(QPainter *painter) override;

  //---

  const GridData &gridData() const { return gridData_; }

 private:
  struct WhiskerData {
    CQChartsBoxWhisker xWhisker;
    CQChartsBoxWhisker yWhisker;
  };

 private:
  void addPointKeyItems(CQChartsPlotKey *key);
  void addGridKeyItems (CQChartsPlotKey *key);

  //---

  void drawBestFit(QPainter *painter);

  //---

  void drawHull(QPainter *painter);

  //---

  void drawXRug(QPainter *painter);
  void drawYRug(QPainter *painter);

  //---

  void drawXDensity(QPainter *painter);
  void drawYDensity(QPainter *painter);

  void drawXDensityWhisker(QPainter *painter, const WhiskerData &whiskerData, int ig, int ng);
  void drawYDensityWhisker(QPainter *painter, const WhiskerData &whiskerData, int ig, int ng);

  void drawXWhisker(QPainter *painter);
  void drawYWhisker(QPainter *painter);

  void drawXWhiskerWhisker(QPainter *painter, const WhiskerData &whiskerData, int ig, int ng);
  void drawYWhiskerWhisker(QPainter *painter, const WhiskerData &whiskerData, int ig, int ng);

  void initWhiskerData();

  //---

  void drawDensityMap(QPainter *painter);

  //---

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

  void setDensityMap(bool b);

  void setGridded(bool b);

 private:
  struct SymbolMapKeyData {
    bool   displayed { true };
    double alpha     { 0.2 };
    double margin    { 16.0 };
  };

  using GroupPoints   = std::map<int,Points>;
  using GroupFitData  = std::map<int,CQChartsFitData>;
  using GroupHull     = std::map<int,CQChartsGrahamHull>;
  using GroupWhiskers = std::map<int,WhiskerData>;

  struct BestFitData {
    bool visible       { false }; // show fit
    bool showDeviation { false }; // show deviation
    int  order         { 3 };     // order
  };

  struct HullData {
    bool visible { false }; // show convex hull
  };

  struct AxisRugData {
    bool  xVisible { false };         // x rug
    YSide xSide    { YSide::BOTTOM }; // x rug side
    bool  yVisible { false };         // y rug
    XSide ySide    { XSide::LEFT };   // y rug side
  };

  struct AxisDensityData {
    bool           xVisible { false };        // x visible
    YSide          xSide    { YSide::TOP };   // x side
    bool           yVisible { false };        // y visible
    XSide          ySide    { XSide::RIGHT }; // y side
    CQChartsLength width    { "48px" };       // width
    double         alpha    { 0.5 };          // alpha
  };

  struct AxisWhiskerData {
    bool           xVisible { false };        // x visible
    YSide          xSide    { YSide::TOP };   // x side
    bool           yVisible { false };        // y visible
    XSide          ySide    { XSide::RIGHT }; // y side
    CQChartsLength width    { "24px" };       // width
    CQChartsLength margin   { "8px" };        // margin
    double         alpha    { 0.5 };          // alpha
  };

  struct DensityMapData {
    bool   visible  { false }; // visible
    int    gridSize { 16 };    // grid size
    double delta    { 0.0 };   // value delta
  };

  CQChartsColumn    nameColumn_;               // name column
  CQChartsColumn    xColumn_            { 0 }; // x column
  CQChartsColumn    yColumn_            { 1 }; // y column
  BestFitData       bestFitData_;              // best fit data
  HullData          hullData_;                 // hull data
  AxisRugData       axisRugData_;              // axis rug data
  AxisDensityData   axisDensityData_;          // axis density data
  DensityMapData    densityMapData_;           // density map data
  AxisWhiskerData   axisWhiskerData_;          // axis whisker data
  GroupNameValues   groupNameValues_;          // name values
  GroupNameGridData groupNameGridData_;        // grid values
  CQChartsDataLabel dataLabel_;                // data label style
  GridData          gridData_;                 // grid data
  QString           xname_;                    // x column header
  QString           yname_;                    // y column header
  SymbolMapKeyData  symbolMapKeyData_;         // symbol map key data
  QString           symbolTypeName_;           // symbol type column header
  QString           symbolSizeName_;           // symbol size column header
  QString           fontSizeName_;             // font size column header
  QString           colorName_;                // color column header
  GroupPoints       groupPoints_;              // group fit points
  GroupFitData      groupFitData_;             // group fit data
  GroupHull         groupHull_;                // group hull
  GroupWhiskers     groupWhiskers_;            // group whiskers
};

#endif
