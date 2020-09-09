#ifndef CQChartsScatterPlot_H
#define CQChartsScatterPlot_H

#include <CQChartsPointPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsBoxWhisker.h>
#include <CQChartsFitData.h>
#include <CQChartsGridCell.h>
#include <CQChartsImage.h>
#include <CQStatData.h>
#include <CInterval.h>
#include <CHexMap.h>

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

  Plot *create(View *view, const ModelP &model) const override;
};

//---

/*!
 * \brief Scatter Plot Point object
 * \ingroup Charts
 */
class CQChartsScatterPointObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(int                 groupInd READ groupInd)
  Q_PROPERTY(CQChartsGeom::Point point    READ point   )
  Q_PROPERTY(QString             name     READ name    )

  Q_PROPERTY(CQChartsSymbol symbolType READ symbolType WRITE setSymbolType)
  Q_PROPERTY(CQChartsLength symbolSize READ symbolSize WRITE setSymbolSize)
  Q_PROPERTY(CQChartsLength fontSize   READ fontSize   WRITE setFontSize  )
  Q_PROPERTY(CQChartsColor  color      READ color      WRITE setColor     )
  Q_PROPERTY(CQChartsFont   font       READ font       WRITE setFont      )

 public:
  using Plot   = CQChartsScatterPlot;
  using Column = CQChartsColumn;
  using Image  = CQChartsImage;
  using Symbol = CQChartsSymbol;
  using Length = CQChartsLength;
  using Color  = CQChartsColor;
  using Font   = CQChartsFont;
  using Units  = CQChartsUnits;

 public:
  CQChartsScatterPointObj(const Plot *plot, int groupInd, const BBox &rect,
                          const Point &p, const ColorInd &is, const ColorInd &ig,
                          const ColorInd &iv);

  const Plot *plot() const { return plot_; }

  int groupInd() const { return groupInd_; }

  //---

  // position
  const Point &point() const { return pos_; }

  //---

  // name and associated column
  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c) { nameColumn_ = c; }

  //---

  // image
  const Image &image() const { return image_; }
  void setImage(const Image &i) { image_ = i; }

  //---

  QString typeName() const override { return "point"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  // symbol type
  Symbol symbolType() const;
  void setSymbolType(const Symbol &s) { extraData().symbolType = s; }

  // symbol size
  Length symbolSize() const;
  void setSymbolSize(const Length &s) { extraData().symbolSize = s; }

  // font size
  Length fontSize() const;
  void setFontSize(const Length &s) { extraData().fontSize = s; }

  // color
  Color color() const;
  void setColor(const Color &c) { extraData().color = c; }

  // font
  Font font() const;
  void setFont(const Font &f) { extraData().font = f; }

  //---

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) override;

  void drawPoint(PaintDevice *device) const;

  void drawDataLabel(PaintDevice *device) const;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  //---

  double xColorValue(bool relative=true) const override;
  double yColorValue(bool relative=true) const override;

 private:
  struct ExtraData {
    Symbol symbolType { Symbol::Type::NONE }; //!< symbol type
    Length symbolSize { Units::NONE, 0.0 };   //!< symbol size
    Color  color;                             //!< symbol fill color
    Length fontSize   { Units::NONE, 0.0 };   //!< font size
    Font   font;                              //!< text font
  };

 private:
  const ExtraData &extraData() const { return edata_; };
  ExtraData &extraData() { return edata_; };

 private:
  const Plot* plot_       { nullptr }; //!< scatter plot
  int         groupInd_   { -1 };      //!< plot group index
  Point       pos_;                    //!< point position
  ExtraData   edata_;                  //!< extra data
  QString     name_;                   //!< label name
  Column      nameColumn_;             //!< label name column
  Image       image_;                  //!< image data
};

//---

/*!
 * \brief Scatter Plot Cell object
 * \ingroup Charts
 */
class CQChartsScatterCellObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot   = CQChartsScatterPlot;
  using Points = std::vector<Point>;

 public:
  CQChartsScatterCellObj(const Plot *plot, int groupInd, const BBox &rect,
                         const ColorInd &is, const ColorInd &ig, int ix, int iy,
                         const Points &points, int maxN);

  int groupInd() const { return groupInd_; }

  const Points &points() const { return points_; }

  //---

  QString typeName() const override { return "cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  void calcRugPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

 private:
  const Plot* plot_     { nullptr }; //!< scatter plot
  int         groupInd_ { -1 };      //!< plot group index
  int         ix_       { -1 };      //!< x index
  int         iy_       { -1 };      //!< y index
  Points      points_;               //!< cell points
  int         maxN_     { 0 };       //!< max number of points
};

//---

/*!
 * \brief Scatter Plot Hex Cell object
 * \ingroup Charts
 */
class CQChartsScatterHexObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot   = CQChartsScatterPlot;
  using Points = std::vector<Point>;

 public:
  CQChartsScatterHexObj(const Plot *plot, int groupInd, const BBox &rect,
                        const ColorInd &is, const ColorInd &ig, int ix, int iy,
                        const Polygon &poly, int n, int maxN);

  int groupInd() const { return groupInd_; }

  //---

  QString typeName() const override { return "hex"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

 private:
  const Plot* plot_     { nullptr }; //!< scatter plot
  int         groupInd_ { -1 };      //!< plot group index
  int         ix_       { -1 };      //!< x index
  int         iy_       { -1 };      //!< y index
  Polygon     poly_;                 //!< polygon
  int         n_        { 0 };       //!< number of points
  int         maxN_     { 0 };       //!< max number of points
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
  using Plot = CQChartsScatterPlot;

 public:
  CQChartsScatterKeyColor(Plot *plot, int groupInd, const ColorInd &ic);

  const Color &color() const { return color_; }
  void setColor(const Color &c) { color_ = c; }

  bool selectPress(const Point &p, SelMod selMod) override;

  QBrush fillBrush() const override;

 private:
  int hideIndex() const;

 private:
  int   groupInd_ { -1 };
  Color color_;
};

/*!
 * \brief Scatter Plot Grid Key Item
 * \ingroup Charts
 */
class CQChartsScatterGridKeyItem : public CQChartsGradientKeyItem {
  Q_OBJECT

 public:
  using Plot = CQChartsScatterPlot;

 public:
  CQChartsScatterGridKeyItem(Plot *plot);

  int maxN() const override;

 private:
  Plot* plot_ { nullptr };
};

/*!
 * \brief Scatter Plot Hex Key Item
 * \ingroup Charts
 */
class CQChartsScatterHexKeyItem : public CQChartsGradientKeyItem {
  Q_OBJECT

 public:
  using Plot = CQChartsScatterPlot;

 public:
  CQChartsScatterHexKeyItem(Plot *plot);

  int maxN() const override;

 private:
  Plot* plot_ { nullptr };
};

//---

CQCHARTS_NAMED_SHAPE_DATA(GridCell, gridCell)

/*!
 * \brief Scatter Plot
 * \ingroup Charts
 */
class CQChartsScatterPlot : public CQChartsPointPlot,
 public CQChartsObjPointData        <CQChartsScatterPlot>,
 public CQChartsObjGridCellShapeData<CQChartsScatterPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn xColumn     READ xColumn     WRITE setXColumn    )
  Q_PROPERTY(CQChartsColumn yColumn     READ yColumn     WRITE setYColumn    )
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(CQChartsColumn labelColumn READ labelColumn WRITE setLabelColumn)

  // options
  Q_PROPERTY(PlotType plotType READ plotType WRITE setPlotType)

  // density map
  Q_PROPERTY(bool   densityMap         READ isDensityMap       WRITE setDensityMap        )
  Q_PROPERTY(int    densityMapGridSize READ densityMapGridSize WRITE setDensityMapGridSize)
  Q_PROPERTY(double densityMapDelta    READ densityMapDelta    WRITE setDensityMapDelta   )

  // symbol data
  CQCHARTS_POINT_DATA_PROPERTIES

  // grid cells
  Q_PROPERTY(int gridNumX READ gridNumX WRITE setGridNumX)
  Q_PROPERTY(int gridNumY READ gridNumY WRITE setGridNumY)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(GridCell, gridCell)

  // symbol map key
  Q_PROPERTY(bool          symbolMapKey       READ isSymbolMapKey     WRITE setSymbolMapKey      )
  Q_PROPERTY(CQChartsAlpha symbolMapKeyAlpha  READ symbolMapKeyAlpha  WRITE setSymbolMapKeyAlpha )
  Q_PROPERTY(double        symbolMapKeyMargin READ symbolMapKeyMargin WRITE setSymbolMapKeyMargin)

  Q_ENUMS(PlotType)

  Q_ENUMS(XSide)
  Q_ENUMS(YSide)

 public:
  enum class PlotType {
    SYMBOLS,
    GRID_CELLS,
    HEX_CELLS
  };

  using Points = std::vector<Point>;

  //! point value data
  struct ValueData {
    Point       p;
    int         row { -1 };
    QModelIndex ind;
    Color       color;

    ValueData(const Point &p=Point(), int row=-1, const QModelIndex &ind=QModelIndex(),
              const Color &color=Color()) :
     p(p), row(row), ind(ind), color(color) {
    }
  };

  using Values = std::vector<ValueData>;

  //! real values data
  struct ValuesData {
    Values  values;
    RMinMax xrange;
    RMinMax yrange;
  };

  using NameValues      = std::map<QString, ValuesData>;
  using GroupNameValues = std::map<int, NameValues>;

  //--

  using GridCell          = CQChartsGridCell;
  using NameGridData      = std::map<QString, GridCell>;
  using GroupNameGridData = std::map<int, NameGridData>;

  //--

  using HexMap = CHexMap<void>;

  using NameHexData      = std::map<QString, HexMap>;
  using GroupNameHexData = std::map<int, NameHexData>;

  //---

  enum XSide {
    LEFT  = (int) CQChartsAxisBoxWhisker::Side::BOTTOM_LEFT,
    RIGHT = (int) CQChartsAxisBoxWhisker::Side::TOP_RIGHT
  };

  enum YSide {
    BOTTOM = (int) CQChartsAxisBoxWhisker::Side::BOTTOM_LEFT,
    TOP    = (int) CQChartsAxisBoxWhisker::Side::TOP_RIGHT
  };

  enum XYSide {
    BOTTOM_LEFT,
    TOP_RIGHT
  };

//using WhiskerSide = CQChartsAxisBoxWhisker::Side;

 public:
  CQChartsScatterPlot(View *view, const ModelP &model);
 ~CQChartsScatterPlot();

  //---

  // columns
  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  const Column &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const Column &c);

  const Column &xColumn() const { return xColumn_; }
  void setXColumn(const Column &c);

  const Column &yColumn() const { return yColumn_; }
  void setYColumn(const Column &c);

  //---

  ColumnType xColumnType() const { return xColumnType_; }
  ColumnType yColumnType() const { return yColumnType_; }

  bool isUniqueX() const { return uniqueX_; }
  bool isUniqueY() const { return uniqueY_; }

  //---

  // axis names
  bool xAxisName(QString &name, const QString &def="") const override;
  bool yAxisName(QString &name, const QString &def="") const override;

  //---

  // plot type
  PlotType plotType() const { return plotType_; }

  bool isSymbols  () const { return (plotType() == PlotType::SYMBOLS   ); }
  bool isGridCells() const { return (plotType() == PlotType::GRID_CELLS); }
  bool isHexCells () const { return (plotType() == PlotType::HEX_CELLS ); }

  //---

  // x/y axis density
  bool isXDensity() const;
  bool isYDensity() const;

  //---

  // x/y axis whisker
  bool isXWhisker() const;
  bool isYWhisker() const;

  //---

  // density map
  bool isDensityMap() const { return densityMapData_.visible; }

  int densityMapGridSize() const { return densityMapData_.gridSize; }
  void setDensityMapGridSize(int i);

  double densityMapDelta() const { return densityMapData_.delta; }
  void setDensityMapDelta(double d);

  //---

  // grid cells
  int gridNumX() const { return gridData_.nx(); }
  void setGridNumX(int n);

  int gridNumY() const { return gridData_.ny(); }
  void setGridNumY(int n);

  const GridCell &gridData() const { return gridData_; }

  //---

  // hex cells
  const HexMap &hexMap() const { return hexMap_; }
  int hexMapMaxN() const { return hexMapMaxN_; }

  //---

  // symbol map key
  bool isSymbolMapKey() const { return symbolMapKeyData_.displayed; }
  void setSymbolMapKey(bool b);

  const Alpha &symbolMapKeyAlpha() const { return symbolMapKeyData_.alpha; }
  void setSymbolMapKeyAlpha(const Alpha &a);

  double symbolMapKeyMargin() const { return symbolMapKeyData_.margin; }
  void setSymbolMapKeyMargin(double r);

  //---

  void addNameValue(int groupInd, const QString &name, const Point &p, int row,
                    const QModelIndex &xind, const Color &color=Color());

  const GroupNameValues &groupNameValues() const { return groupNameValues_; }

  //---

  void addProperties() override;

  //---

  Range calcRange() const override;

  void clearPlotObjects() override;

  bool createObjs(PlotObjs &obj) const override;

  void addPointObjects(PlotObjs &objs) const;
  void addGridObjects (PlotObjs &objs) const;
  void addHexObjects  (PlotObjs &objs) const;

  void addNameValues() const;

  //---

  QString xHeaderName(bool tip=false) const { return columnHeaderName(xColumn(), tip); }
  QString yHeaderName(bool tip=false) const { return columnHeaderName(yColumn(), tip); }

  void updateColumnNames() override;

  //---

  int numRows() const;

  //---

  using PointObj = CQChartsScatterPointObj;
  using CellObj  = CQChartsScatterCellObj;
  using HexObj   = CQChartsScatterHexObj;

  virtual PointObj *createPointObj(int groupInd, const BBox &rect, const Point &p,
                                   const ColorInd &is, const ColorInd &ig,
                                   const ColorInd &iv) const;

  virtual CellObj *createCellObj(int groupInd, const BBox &rect, const ColorInd &is,
                                 const ColorInd &ig, int ix, int iy, const Points &points,
                                 int maxN) const;

  virtual HexObj *createHexObj(int groupInd, const BBox &rect, const ColorInd &is,
                               const ColorInd &ig, int ix, int iy, const Polygon &poly, int n,
                               int maxN) const;

  //---

  void addKeyItems(PlotKey *key) override;

  //---

  bool addMenuItems(QMenu *menu) override;

  BBox calcAnnotationBBox() const override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(PaintDevice *device) const override;

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *device) const override;

  //---

 private:
  using AxisBoxWhisker = CQChartsAxisBoxWhisker;
  using AxisDensity    = CQChartsAxisDensity;

 private:
  void initGridData(const Range &dataRange);

  void initAxes();

  //---

  void addPointKeyItems(PlotKey *key);
  void addGridKeyItems (PlotKey *key);
  void addHexKeyItems  (PlotKey *key);

  //---

  bool probe(ProbeData &probeData) const override;

  //---

  void initGroupBestFit(int groupInd) const;

  void drawBestFit(PaintDevice *device) const;

  //---

  void initGroupStats(int groupInd) const;

  void drawStatsLines(PaintDevice *device) const;

  //---

  void drawHull(PaintDevice *device) const;

  //---

  void drawXRug(PaintDevice *device) const;
  void drawYRug(PaintDevice *device) const;

  void drawXYRug(PaintDevice *device, const RugP &rug) const;

  //---

  void drawXDensity(PaintDevice *device) const;
  void drawYDensity(PaintDevice *device) const;

  void drawXYDensityWhisker(PaintDevice *device, const AxisBoxWhisker *whiskerData,
                            const ColorInd &ig, double delta=0.0) const;

  void drawXWhisker(PaintDevice *device) const;
  void drawYWhisker(PaintDevice *device) const;

  void drawXYWhiskerWhisker(PaintDevice *device, const AxisBoxWhisker *boxWhisker,
                            const ColorInd &ig, double delta=0.0) const;

  void initWhiskerData() const;

  //---

  void drawDensityMap(PaintDevice *device) const;

  //---

  void drawSymbolMapKey(PaintDevice *device) const;

  //---

 public slots:
  // set plot type
  void setPlotType(PlotType plotType);

  // set symbols, grid cells
  void setSymbols  (bool b);
  void setGridCells(bool b);
  void setHexCells (bool b);

  // overlays
  void setDensityMap(bool b);

  // x axis annotations
  void setXDensity(bool b);
  void setXWhisker(bool b);

  // y axis annotations
  void setYDensity(bool b);
  void setYWhisker(bool b);

 private:
  //! symbol map key daya
  struct SymbolMapKeyData {
    bool   displayed { false }; //!< is symbol map key displayed
    Alpha  alpha     { 0.2 };   //!< symbol map key background alpha
    double margin    { 16.0 };  //!< symbol map key margin in pixels
  };

  struct StatData {
    CQStatData xstat;
    CQStatData ystat;
  };

  using GroupInds     = std::set<int>;
  using GroupPoints   = std::map<int, Points>;
  using GroupFitData  = std::map<int, CQChartsFitData>;
  using GroupStatData = std::map<int, StatData>;
  using GroupHull     = std::map<int, CQChartsGrahamHull *>;
  using GroupWhiskers = std::map<int, AxisBoxWhisker *>;

  struct DensityMapData {
    bool   visible  { false }; //!< visible
    int    gridSize { 16 };    //!< grid size
    double delta    { 0.0 };   //!< value delta
  };

 private:
  // columns
  Column xColumn_;     //!< x column
  Column yColumn_;     //!< y column
  Column nameColumn_;  //!< name column
  Column labelColumn_; //!< label column

  ColumnType xColumnType_ { ColumnType::NONE }; //!< x column type
  ColumnType yColumnType_ { ColumnType::NONE }; //!< y column type

  bool uniqueX_ { false }; //!< are x values uniquified (string to int)
  bool uniqueY_ { false }; //!< are y values uniquified (string to int)

  // options
  PlotType plotType_ { PlotType::SYMBOLS }; //!< plot type

  // axis density data
  AxisDensity*    xAxisDensity_ { nullptr }; //!< x axis whisker density object
  AxisDensity*    yAxisDensity_ { nullptr }; //!< x axis whisker density object

  // axis whisker data
  GroupWhiskers   groupXWhiskers_;           //!< group x whiskers
  GroupWhiskers   groupYWhiskers_;           //!< group y whiskers
  AxisBoxWhisker* xAxisWhisker_ { nullptr }; //!< x axis whisker master object
  AxisBoxWhisker* yAxisWhisker_ { nullptr }; //!< y axis whisker master object

  // plot overlay data
  DensityMapData densityMapData_;   //!< density map data
  GridCell       gridData_;         //!< grid data
  HexMap         hexMap_;           //!< hex map
  int            hexMapMaxN_ { 0 }; //!< hex map max N

  // group data
  GroupInds         groupInds_;
  GroupNameValues   groupNameValues_;   //!< group name values (individual points)
  GroupNameGridData groupNameGridData_; //!< grid cell values
  GroupNameHexData  groupNameHexData_;  //!< hex cell values
  GroupPoints       groupPoints_;       //!< group fit points
  GroupFitData      groupFitData_;      //!< group fit data
  GroupStatData     groupStatData_;     //!< group stat data
  GroupHull         groupHull_;         //!< group hull

  // symbol map
  SymbolMapKeyData symbolMapKeyData_; //!< symbol map key data

  // axis side data
  using AxisSideBBox = std::map<XYSide,BBox>;

  mutable AxisSideBBox xAxisSideBBox_; //!< top or bottom
  mutable AxisSideBBox yAxisSideBBox_; //!< left or right
};

#endif
