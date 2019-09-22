#ifndef CQChartsScatterPlot_H
#define CQChartsScatterPlot_H

#include <CQChartsPointPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsBoxWhisker.h>
#include <CQChartsFitData.h>
#include <CQStatData.h>
#include <CInterval.h>

class CQChartsScatterPlot;
class CQChartsGrahamHull;

//---

/*!
 * \brief Scatter plot type
 * \ingroup Charts
 */
class CQChartsScatterPlotType : public CQChartsPointPlotType {
 public:
  CQChartsScatterPlotType();

  QString name() const override { return "scatter"; }
  QString desc() const override { return "Scatter"; }

  void addParameters() override;

  bool canProbe() const override { return true; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

/*!
 * \brief Scatter Plot Point object
 * \ingroup Charts
 */
class CQChartsScatterPointObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(int     groupInd READ groupInd)
  Q_PROPERTY(QPointF point    READ point   )
  Q_PROPERTY(QString name     READ name    )

  Q_PROPERTY(CQChartsSymbol symbolType READ symbolType WRITE setSymbolType)
  Q_PROPERTY(CQChartsLength symbolSize READ symbolSize WRITE setSymbolSize)
  Q_PROPERTY(CQChartsLength fontSize   READ fontSize   WRITE setFontSize  )
  Q_PROPERTY(CQChartsColor  color      READ color      WRITE setColor     )

 public:
  enum Dir {
    X  = (1<<0),
    Y  = (1<<1),
    XY = (X | Y)
  };

 public:
  CQChartsScatterPointObj(const CQChartsScatterPlot *plot, int groupInd,
                          const CQChartsGeom::BBox &rect, const QPointF &p,
                          const ColorInd &is, const ColorInd &ig, const ColorInd &iv);

  const CQChartsScatterPlot *plot() const { return plot_; }

  int groupInd() const { return groupInd_; }

  //---

  // position
  const QPointF &point() const { return pos_; }

  //---

  // name
  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //---

  // image
  const QImage &image() const { return image_; }
  void setImage(const QImage &i) { image_ = i; }

  //---

  QString typeName() const override { return "point"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  // symbol type
  CQChartsSymbol symbolType() const;
  void setSymbolType(const CQChartsSymbol &s) { extraData().symbolType = s; }

  // symbol size
  CQChartsLength symbolSize() const;
  void setSymbolSize(const CQChartsLength &s) { extraData().symbolSize = s; }

  // font size
  CQChartsLength fontSize() const;
  void setFontSize(const CQChartsLength &s) { extraData().fontSize = s; }

  // color
  CQChartsColor color() const;
  void setColor(const CQChartsColor &c) { extraData().color = c; }

  //---

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

  void drawDir(CQChartsPaintDevice *device, const Dir &dir, bool flip=false) const;

  //---

  double xColorValue(bool relative=true) const override;
  double yColorValue(bool relative=true) const override;

 private:
  struct ExtraData {
    CQChartsSymbol symbolType { CQChartsSymbol::Type::NONE }; //!< symbol type
    CQChartsLength symbolSize { CQChartsUnits::NONE, 0.0 };   //!< symbol size
    CQChartsLength fontSize   { CQChartsUnits::NONE, 0.0 };   //!< font size
    CQChartsColor  color;                                     //!< symbol fill color
  };

 private:
  const ExtraData &extraData() const { return edata_; };
  ExtraData &extraData() { return edata_; };

 private:
  const CQChartsScatterPlot* plot_       { nullptr }; //!< scatter plot
  int                        groupInd_   { -1 };      //!< plot group index
  QPointF                    pos_;                    //!< point position
  ExtraData                  edata_;                  //!< extra data
  QString                    name_;                   //!< label name
  QImage                     image_;                  //!< image name
};

//---

/*!
 * \brief Scatter Plot Cell object
 * \ingroup Charts
 */
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
  CQChartsScatterCellObj(const CQChartsScatterPlot *plot, int groupInd,
                         const CQChartsGeom::BBox &rect, const ColorInd &is, const ColorInd &ig,
                         int ix, int iy, const Points &points, int maxn);

  int groupInd() const { return groupInd_; }

  const Points &points() const { return points_; }

  //---

  QString typeName() const override { return "cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

  void drawRugSymbol(CQChartsPaintDevice *device, const Dir &dir, bool flip) const;

 private:

 private:
  const CQChartsScatterPlot* plot_     { nullptr }; //!< scatter plot
  int                        groupInd_ { -1 };      //!< plot group index
  int                        ix_       { -1 };      //!< x index
  int                        iy_       { -1 };      //!< y index
  Points                     points_;               //!< cell points
  int                        maxn_     { 0 };       //!< max number of points
};

//---

#include <CQChartsKey.h>

/*!
 * \brief Scatter Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsScatterKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int groupInd, const ColorInd &ic);

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

/*!
 * \brief Scatter Plot Grid Key Item
 * \ingroup Charts
 */
class CQChartsScatterGridKeyItem : public CQChartsKeyItem {
  Q_OBJECT

 public:
  CQChartsScatterGridKeyItem(CQChartsScatterPlot *plot);

  QSizeF size() const override;

  void draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect) const override;

 private:
  CQChartsScatterPlot* plot_ { nullptr };
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Hull,hull)
CQCHARTS_NAMED_SHAPE_DATA(GridCell,gridCell)

/*!
 * \brief Scatter Plot
 * \ingroup Charts
 */
class CQChartsScatterPlot : public CQChartsPointPlot,
 public CQChartsObjPointData        <CQChartsScatterPlot>,
 public CQChartsObjBestFitShapeData <CQChartsScatterPlot>,
 public CQChartsObjStatsLineData    <CQChartsScatterPlot>,
 public CQChartsObjHullShapeData    <CQChartsScatterPlot>,
 public CQChartsObjRugPointData     <CQChartsScatterPlot>,
 public CQChartsObjGridCellShapeData<CQChartsScatterPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn xColumn     READ xColumn     WRITE setXColumn    )
  Q_PROPERTY(CQChartsColumn yColumn     READ yColumn     WRITE setYColumn    )
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(CQChartsColumn labelColumn READ labelColumn WRITE setLabelColumn)

  // options
  Q_PROPERTY(PlotType plotType READ plotType  WRITE setPlotType)
  Q_PROPERTY(bool     skipBad  READ isSkipBad WRITE setSkipBad )

  // best fit
  Q_PROPERTY(bool bestFit          READ isBestFit          WRITE setBestFit         )
  Q_PROPERTY(bool bestFitOutliers  READ isBestFitOutliers  WRITE setBestFitOutliers )
  Q_PROPERTY(int  bestFitOrder     READ bestFitOrder       WRITE setBestFitOrder    )
  Q_PROPERTY(bool bestFitDeviation READ isBestFitDeviation WRITE setBestFitDeviation)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(BestFit,bestFit)

  // stats
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Stats, stats)

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

  // grid cells
  Q_PROPERTY(int gridNumX READ gridNumX WRITE setGridNumX)
  Q_PROPERTY(int gridNumY READ gridNumY WRITE setGridNumY)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(GridCell,gridCell)

  // symbol map key
  Q_PROPERTY(bool   symbolMapKey       READ isSymbolMapKey     WRITE setSymbolMapKey      )
  Q_PROPERTY(double symbolMapKeyAlpha  READ symbolMapKeyAlpha  WRITE setSymbolMapKeyAlpha )
  Q_PROPERTY(double symbolMapKeyMargin READ symbolMapKeyMargin WRITE setSymbolMapKeyMargin)

  // text labels
  Q_PROPERTY(bool pointLabels READ isPointLabels WRITE setPointLabels)

  Q_ENUMS(PlotType)

  Q_ENUMS(XSide)
  Q_ENUMS(YSide)

 public:
  enum class PlotType {
    SYMBOLS,
    GRID_CELLS
  };

  struct ValueData {
    QPointF       p;
    int           row { -1 };
    QModelIndex   ind;
    CQChartsColor color;

    ValueData(const QPointF &p=QPointF(), int row=-1, const QModelIndex &ind=QModelIndex(),
              const CQChartsColor &color=CQChartsColor()) :
     p(p), row(row), ind(ind), color(color) {
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
 ~CQChartsScatterPlot();

  //---

  // columns
  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const CQChartsColumn &c);

  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c);

  const CQChartsColumn &yColumn() const { return yColumn_; }
  void setYColumn(const CQChartsColumn &c);

  //---

  QString xColumnName(const QString &def="x") const;
  QString yColumnName(const QString &def="y") const;

  //---

  // plot type
  PlotType plotType() const { return plotType_; }

  bool isSymbols  () const { return (plotType() == PlotType::SYMBOLS    ); }
  bool isGridCells() const { return (plotType() == PlotType::GRID_CELLS ); }

  //---

  bool isSkipBad() const { return skipBad_; }
  void setSkipBad(bool b);

  //---

  // best fit
  bool isBestFit() const { return bestFitData_.visible; }

  bool isBestFitOutliers() const { return bestFitData_.includeOutliers; }
  void setBestFitOutliers(bool b);

  int bestFitOrder() const { return bestFitData_.order; }
  void setBestFitOrder(int o);

  bool isBestFitDeviation() const { return bestFitData_.showDeviation; }
  void setBestFitDeviation(bool b);

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

  // grid cells
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

  void addNameValue(int groupInd, const QString &name, double x, double y, int row,
                    const QModelIndex &xind, const CQChartsColor &color=CQChartsColor());

  const GroupNameValues &groupNameValues() const { return groupNameValues_; }

  //---

  void addProperties() override;

  //---

  CQChartsGeom::Range calcRange() const override;

  void clearPlotObjects() override;

  bool createObjs(PlotObjs &obj) const override;

  void addPointObjects(PlotObjs &objs) const;
  void addGridObjects (PlotObjs &objs) const;

  void addNameValues() const;

  //---

  QString xHeaderName() const { return columnHeaderName(xColumn()); }
  QString yHeaderName() const { return columnHeaderName(yColumn()); }

  void updateColumnNames() override;

  //---

  int numRows() const;

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  bool addMenuItems(QMenu *menu) override;

  CQChartsGeom::BBox annotationBBox() const override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(CQChartsPaintDevice *device) const override;

  bool hasForeground() const override;

  void execDrawForeground(CQChartsPaintDevice *device) const override;

  //---

  const GridData &gridData() const { return gridData_; }

 private:
  struct WhiskerData {
    CQChartsBoxWhisker xWhisker;
    CQChartsBoxWhisker yWhisker;
  };

 private:
  void initGridData(const CQChartsGeom::Range &dataRange);

  void resetAxes();

  void initAxes(bool uniqueX, bool uniqueY);

  //---

  void addPointKeyItems(CQChartsPlotKey *key);
  void addGridKeyItems (CQChartsPlotKey *key);

  //---

  bool probe(ProbeData &probeData) const override;

  //---

  void initGroupBestFit(int groupInd) const;
  void initGroupStats  (int groupInd) const;

  void drawBestFit(CQChartsPaintDevice *device) const;

  void drawStatsLines(CQChartsPaintDevice *device) const;

  //---

  void drawHull(CQChartsPaintDevice *device) const;

  //---

  void drawXRug(CQChartsPaintDevice *device) const;
  void drawYRug(CQChartsPaintDevice *device) const;

  //---

  void drawXDensity(CQChartsPaintDevice *device) const;
  void drawYDensity(CQChartsPaintDevice *device) const;

  void drawXDensityWhisker(CQChartsPaintDevice *device, const WhiskerData &whiskerData,
                           const ColorInd &ig) const;
  void drawYDensityWhisker(CQChartsPaintDevice *device, const WhiskerData &whiskerData,
                           const ColorInd &ig) const;

  void drawXWhisker(CQChartsPaintDevice *device) const;
  void drawYWhisker(CQChartsPaintDevice *device) const;

  void drawXWhiskerWhisker(CQChartsPaintDevice *device, const WhiskerData &whiskerData,
                           const ColorInd &ig) const;
  void drawYWhiskerWhisker(CQChartsPaintDevice *device, const WhiskerData &whiskerData,
                           const ColorInd &ig) const;

  void initWhiskerData() const;

  //---

  void drawDensityMap(CQChartsPaintDevice *device) const;

  //---

  void drawSymbolMapKey(CQChartsPaintDevice *device) const;

  //---

 public slots:
  // set plot type
  void setPlotType(PlotType plotType);

  // set symbols, grid cells
  void setSymbols  (bool b);
  void setGridCells(bool b);

  // overlays
  void setBestFit       (bool b);
  void setHull          (bool b);
  void setStatsLinesSlot(bool b);
  void setDensityMap    (bool b);

  // x axis annotations
  void setXRug    (bool b);
  void setXDensity(bool b);
  void setXWhisker(bool b);

  // y axis annotations
  void setYRug    (bool b);
  void setYDensity(bool b);
  void setYWhisker(bool b);

 private:
  struct SymbolMapKeyData {
    bool   displayed { false }; //!< is symbol map key displayed
    double alpha     { 0.2 };   //!< symbol map key background alpha
    double margin    { 16.0 };  //!< symbol map key margin in pixels
  };

  struct StatData {
    CQStatData xstat;
    CQStatData ystat;
  };

  using GroupPoints   = std::map<int,Points>;
  using GroupFitData  = std::map<int,CQChartsFitData>;
  using GroupStatData = std::map<int,StatData>;
  using GroupHull     = std::map<int,CQChartsGrahamHull *>;
  using GroupWhiskers = std::map<int,WhiskerData>;

  struct BestFitData {
    bool visible         { false }; //!< show fit
    bool showDeviation   { false }; //!< show fit deviation
    int  order           { 3 };     //!< fit order
    bool includeOutliers { true };  //!< include outliers
  };

  struct HullData {
    bool visible { false }; //!< show convex hull
  };

  struct AxisRugData {
    bool  xVisible { false };         //!< x rug
    YSide xSide    { YSide::BOTTOM }; //!< x rug side
    bool  yVisible { false };         //!< y rug
    XSide ySide    { XSide::LEFT };   //!< y rug side
  };

  struct AxisDensityData {
    bool           xVisible { false };        //!< x visible
    YSide          xSide    { YSide::TOP };   //!< x side
    bool           yVisible { false };        //!< y visible
    XSide          ySide    { XSide::RIGHT }; //!< y side
    CQChartsLength width    { "48px" };       //!< width
    double         alpha    { 0.5 };          //!< alpha
  };

  struct AxisWhiskerData {
    bool           xVisible { false };        //!< x visible
    YSide          xSide    { YSide::TOP };   //!< x side
    bool           yVisible { false };        //!< y visible
    XSide          ySide    { XSide::RIGHT }; //!< y side
    CQChartsLength width    { "24px" };       //!< width
    CQChartsLength margin   { "8px" };        //!< margin
    double         alpha    { 0.5 };          //!< alpha
  };

  struct DensityMapData {
    bool   visible  { false }; //!< visible
    int    gridSize { 16 };    //!< grid size
    double delta    { 0.0 };   //!< value delta
  };

 private:
  // columns
  CQChartsColumn xColumn_;     //!< x column
  CQChartsColumn yColumn_;     //!< y column
  CQChartsColumn nameColumn_;  //!< name column
  CQChartsColumn labelColumn_; //!< label column

  // options
  PlotType plotType_ { PlotType::SYMBOLS }; //!< plot type
  bool     skipBad_  { true };              //!< skip bad values

  // axis annotation data
  AxisRugData     axisRugData_;     //!< axis rug data
  AxisDensityData axisDensityData_; //!< axis density data
  AxisWhiskerData axisWhiskerData_; //!< axis whisker data

  // plot overlay data
  BestFitData    bestFitData_;    //!< best fit data
  HullData       hullData_;       //!< hull data
  DensityMapData densityMapData_; //!< density map data
  GridData       gridData_;       //!< grid data

  // group data
  GroupNameValues   groupNameValues_;   //!< group name values
  GroupNameGridData groupNameGridData_; //!< grid values
  GroupPoints       groupPoints_;       //!< group fit points
  GroupFitData      groupFitData_;      //!< group fit data
  GroupStatData     groupStatData_;     //!< group stat data
  GroupHull         groupHull_;         //!< group hull
  GroupWhiskers     groupWhiskers_;     //!< group whiskers

  // symbol map
  SymbolMapKeyData symbolMapKeyData_; //!< symbol map key data
};

#endif
