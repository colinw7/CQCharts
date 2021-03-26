#ifndef CQChartsScatterPlot_H
#define CQChartsScatterPlot_H

#include <CQChartsPointPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsBoxWhisker.h>
#include <CQChartsGridCell.h>
#include <CQChartsImage.h>
#include <CQChartsKey.h>
#include <CInterval.h>
#include <CHexMap.h>

class CQChartsScatterPlot;
class CQChartsBivariateDensity;

class CQThreadObject;

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

  bool supportsAlphaColumn() const override { return true; }
  bool supportsFontColumn () const override { return true; }
  bool supportsImageColumn() const override { return true; }

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
  Q_PROPERTY(CQChartsAlpha  alpha      READ alpha      WRITE setAlpha     )
  Q_PROPERTY(CQChartsFont   font       READ font       WRITE setFont      )

 public:
  using Plot    = CQChartsScatterPlot;
  using Column  = CQChartsColumn;
  using Image   = CQChartsImage;
  using Symbol  = CQChartsSymbol;
  using Length  = CQChartsLength;
  using Color   = CQChartsColor;
  using Font    = CQChartsFont;
  using Units   = CQChartsUnits;
  using OptBool = boost::optional<bool>;

 public:
  CQChartsScatterPointObj(const Plot *plot, int groupInd, const BBox &rect,
                          const Point &p, const ColorInd &is, const ColorInd &ig,
                          const ColorInd &iv);

  const Plot *plot() const { return plot_; }

  int groupInd() const { return groupInd_; }

  //---

  QString typeName() const override { return "point"; }

  //---

  bool isPoint() const override { return true; }

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

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  // symbol type
  bool hasSymbolType() const;

  const Symbol &symbolType() const { return extraData().symbolType; }
  void setSymbolType(const Symbol &s) { extraData().symbolType = s; }
  Symbol calcSymbolType() const;

  // symbol filled
  const OptBool &isSymbolFilled() const { return extraData().symbolFilled; }
  void setSymbolFilled(const OptBool &b) { extraData().symbolFilled = b; }

  // symbol size
  const Length &symbolSize() const { return extraData().symbolSize; }
  void setSymbolSize(const Length &s) { extraData().symbolSize = s; }
  Length calcSymbolSize() const;

  // font size
  const Length &fontSize() const { return extraData().fontSize; }
  void setFontSize(const Length &s) { extraData().fontSize = s; }
  Length calcFontSize() const;

  // color
  Color color() const { return extraData().color; }
  void setColor(const Color &c) { extraData().color = c; }

  // alpha
  Alpha alpha() const { return extraData().alpha; }
  void setAlpha(const Alpha &a) { extraData().alpha = a; }

  // font
  Font font() const { return extraData().font; }
  void setFont(const Font &f) { extraData().font = f; }

  //---

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawPoint(PaintDevice *device) const;

  void drawDataLabel(PaintDevice *device) const;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  //---

  double xColorValue(bool relative=true) const override;
  double yColorValue(bool relative=true) const override;

 private:
  struct ExtraData {
    Symbol  symbolType;                      //!< symbol type
    Length  symbolSize { Units::NONE, 0.0 }; //!< symbol size
    OptBool symbolFilled;                    //!< optional symbol fill override
    Color   color;                           //!< symbol fill color
    Alpha   alpha;                           //!< symbol fill alpha
    Length  fontSize   { Units::NONE, 0.0 }; //!< font size
    Font    font;                            //!< text font
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
 * \brief Scatter Plot Grid Cell object
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

  void draw(PaintDevice *device) const override;

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

  void draw(PaintDevice *device) const override;

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

/*!
 * \brief Scatter Plot Density object
 * \ingroup Charts
 */
class CQChartsScatterDensityObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot = CQChartsScatterPlot;

 public:
  CQChartsScatterDensityObj(const Plot *plot, int groupInd,
                            const QString &name, const BBox &rect);

  int groupInd() const { return groupInd_; }

  const QString &name() const { return name_; }

  //---

  QString typeName() const override { return "density"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  void draw(PaintDevice *device) const override;

  bool drawMouseOver() const override { return false; }

 private:
  const Plot* plot_     { nullptr }; //!< scatter plot
  int         groupInd_ { -1 };      //!< plot group index
  QString     name_;                 //!< name
};

//---

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

  // select interface
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
  CQChartsScatterGridKeyItem(Plot *plot, int n);

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
  CQChartsScatterHexKeyItem(Plot *plot, int n);

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
  Q_PROPERTY(bool      densityMap         READ isDensityMap       WRITE setDensityMap        )
  Q_PROPERTY(int       densityMapGridSize READ densityMapGridSize WRITE setDensityMapGridSize)
  Q_PROPERTY(double    densityMapDelta    READ densityMapDelta    WRITE setDensityMapDelta   )
  Q_PROPERTY(DrawLayer densityMapLayer    READ densityMapLayer    WRITE setDensityMapLayer   )

  // symbol data
  CQCHARTS_POINT_DATA_PROPERTIES

  // grid cells
  Q_PROPERTY(int gridNumX READ gridNumX WRITE setGridNumX)
  Q_PROPERTY(int gridNumY READ gridNumY WRITE setGridNumY)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(GridCell, gridCell)

  Q_ENUMS(PlotType)

  Q_ENUMS(XSide)
  Q_ENUMS(YSide)

//Q_ENUMS(DrawLayer)

 public:
  enum class PlotType {
    NONE,
    SYMBOLS,
    GRID_CELLS,
    HEX_CELLS
  };

#if 0
  enum class DrawLayer {
    NONE,
    BACKGROUND,
    MIDDLE,
    FOREGROUND
  };
#endif

  //--

  using GridCell          = CQChartsGridCell;
  using NameGridData      = std::map<QString, GridCell>;
  using GroupNameGridData = std::map<int, NameGridData>;

  //--

  using HexMap = CHexMap<void>;

  using NameHexData      = std::map<QString, HexMap>;
  using GroupNameHexData = std::map<int, NameHexData>;

  //--

  using Density = CQChartsBivariateDensity;

  //--

  enum XSide {
    LEFT  = (int) CQChartsAxisSide::Type::BOTTOM_LEFT,
    RIGHT = (int) CQChartsAxisSide::Type::TOP_RIGHT
  };

  enum YSide {
    BOTTOM = (int) CQChartsAxisSide::Type::BOTTOM_LEFT,
    TOP    = (int) CQChartsAxisSide::Type::TOP_RIGHT
  };

//using WhiskerSide = CQChartsAxisBoxWhisker::Side;

  using Length   = CQChartsLength;
  using Alpha    = CQChartsAlpha;
  using Color    = CQChartsColor;
  using ColorInd = CQChartsUtil::ColorInd;
  using PenBrush = CQChartsPenBrush;
  using Symbol   = CQChartsSymbol;

 public:
  CQChartsScatterPlot(View *view, const ModelP &model);
 ~CQChartsScatterPlot();

  //---

  void init() override;
  void term() override;

  //---

  // name, label, x, y columns
  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  const Column &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const Column &c);

  const Column &xColumn() const { return xColumn_; }
  void setXColumn(const Column &c);

  const Column &yColumn() const { return yColumn_; }
  void setYColumn(const Column &c);

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  //---

  // get x/y column type
  const ColumnType &xColumnType() const { return xColumnType_; }
  const ColumnType &yColumnType() const { return yColumnType_; }

  // get x/y unique values
  bool isUniqueX() const { return uniqueX_; }
  bool isUniqueY() const { return uniqueY_; }

  //---

  // get x/y axis names
  bool xAxisName(QString &name, const QString &def="") const override;
  bool yAxisName(QString &name, const QString &def="") const override;

  //---

  // plot type
  const PlotType &plotType() const { return plotType_; }

  bool isNoType   () const { return (plotType() == PlotType::NONE      ); }
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

  const DrawLayer &densityMapLayer() const { return densityMapData_.layer; }
  void setDensityMapLayer(const DrawLayer &layer);

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

  void setFixedSymbolSize(const Length &s) override { setSymbolSize(s); }
  const Length &fixedSymbolSize() const override { return symbolSize(); }

  void setFixedSymbolType(const Symbol &s) override { setSymbolType(s); }
  const Symbol &fixedSymbolType() const override { return symbolType(); }


  //---

  void addNameValue(int groupInd, const QString &name, const Point &p, int row,
                    const QModelIndex &xind, const Color &color=Color());

  //---

  // custom color interp (for overlay)
  QColor interpColor(const Color &c, const ColorInd &ind) const override;

  //---

  // add properties
  void addProperties() override;

  //---

  Range calcRange() const override;

  void postCalcRange() override;

  void clearPlotObjects() override;

  bool createObjs(PlotObjs &obj) const override;

  void addPointObjects  (PlotObjs &objs) const;
  void addGridObjects   (PlotObjs &objs) const;
  void addHexObjects    (PlotObjs &objs) const;
  void addBestFitObjects(PlotObjs &objs) const;
  void addHullObjects   (PlotObjs &objs) const;
  void addDensityObjects(PlotObjs &objs) const;

  void addNameValues() const;

  QString singleGroupName(ColorInd &ind) const override;

  //---

  Density *getDensity(int groupInd, const QString &name) const;

  //---

  QString xHeaderName(bool tip=false) const { return columnHeaderName(xColumn(), tip); }
  QString yHeaderName(bool tip=false) const { return columnHeaderName(yColumn(), tip); }

  void updateColumnNames() override;

  //---

  int numRows() const;

  //---

  using PointObj   = CQChartsScatterPointObj;
  using CellObj    = CQChartsScatterCellObj;
  using HexObj     = CQChartsScatterHexObj;
  using DensityObj = CQChartsScatterDensityObj;

  virtual PointObj *createPointObj(int groupInd, const BBox &rect, const Point &p,
                                   const ColorInd &is, const ColorInd &ig,
                                   const ColorInd &iv) const;

  virtual CellObj *createCellObj(int groupInd, const BBox &rect, const ColorInd &is,
                                 const ColorInd &ig, int ix, int iy, const Points &points,
                                 int maxN) const;

  virtual HexObj *createHexObj(int groupInd, const BBox &rect, const ColorInd &is,
                               const ColorInd &ig, int ix, int iy, const Polygon &poly, int n,
                               int maxN) const;

  virtual DensityObj *createDensityObj(int groupInd, const QString &name,
                                       const BBox &rect) const;

  //---

  void addKeyItems(PlotKey *key) override;

  //---

  bool addMenuItems(QMenu *menu) override;

  BBox calcExtraFitBBox() const override;

  double xAxisHeight(const CQChartsAxisSide::Type &side) const override;
  double yAxisWidth (const CQChartsAxisSide::Type &side) const override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(PaintDevice *device) const override;

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *device) const override;

  //---

  void preDrawObjs (PaintDevice *) const override;
  void postDrawObjs(PaintDevice *) const override;

  //---

  void drawXAxisAt(PaintDevice *device, Plot *plot, double pos) const override;
  void drawYAxisAt(PaintDevice *device, Plot *plot, double pos) const override;

  //---

  void addDataLabelData(const BBox &bbox, const QString &text,
                        const CQChartsLabelPosition &position,
                        const PenBrush &penBrush, const Font &font);

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

  void drawStatsLines(PaintDevice *device) const;

  //---

  void drawXRug(PaintDevice *device) const;
  void drawYRug(PaintDevice *device) const;

  void drawXYRug(PaintDevice *device, const RugP &rug, double delta=0.0) const;

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

  void clearDensityData();

  //---

  void drawDataLabelDatas(PaintDevice *device) const;

 private:
  void calcDensityMap(int groupInd);

  static void calcDensityMapThread(CQChartsScatterPlot *plot, int groupInd);

  void calcDensityMapImpl(int groupInd);

 signals:
  void customDataChanged();

 public slots:
  // set plot type
  void setPlotType(PlotType plotType);

  // set symbols, grid cells
  void setNoType   (bool b);
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

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  using GroupInds         = std::set<int>;
  using NamedDensity      = std::map<QString, Density *>;
  using GroupNamedDensity = std::map<int, NamedDensity>;
  using GroupWhiskers     = std::map<int, AxisBoxWhisker *>;

  // global density data (all groups)
  //
  // Depends on groupNameValues which are calculated on draw objs
  struct DensityMapData {
    bool            visible   { false };                 //!< visible
    int             gridSize  { 16 };                    //!< grid size
    double          delta     { 0.0 };                   //!< value delta
    DrawLayer       layer     { DrawLayer::BACKGROUND }; //!< draw layer
    mutable Size    psize;                               //!< last calculated pixel size
    CQThreadObject* thread    { nullptr };               //!< calc thread
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
  AxisDensity* xAxisDensity_ { nullptr }; //!< x axis whisker density object
  AxisDensity* yAxisDensity_ { nullptr }; //!< x axis whisker density object

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
  GroupInds         groupInds_;         //!< group indices
  GroupNameGridData groupNameGridData_; //!< grid cell values
  GroupNameHexData  groupNameHexData_;  //!< hex cell values
  GroupNamedDensity groupNamedDensity_; //!< group named density

  // axis side data
  using AxisSideSize = std::map<CQChartsAxisSide::Type, double>;

  mutable AxisSideSize xAxisSideHeight_; //!< top or bottom
  mutable AxisSideSize yAxisSideWidth_;  //!< left or right

  // cached labels
  struct DataLabelData {
    BBox                  bbox;
    QString               text;
    CQChartsLabelPosition position;
    PenBrush              penBrush;
    Font                  font;
  };

  using DataLabelDatas = std::vector<DataLabelData>;

  DataLabelDatas dataLabelDatas_;
};

//---

class CQChartsEnumParameterEdit;
class CQChartsFontLineEdit;
class CQEnumCombo;
class CQCheckBox;

class CQChartsScatterPlotCustomControls : public CQChartsPointPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsScatterPlotCustomControls(CQCharts *charts);

  void setPlot(CQChartsPlot *plot) override;

  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 private:
  void connectSlots(bool b);

 public slots:
  void updateWidgets() override;

 private slots:
  void plotTypeSlot();

  void pointLabelsSlot();
  void labelColumnSlot();
  void positionSlot();
  void fontSlot();
  void fontSizeColumnSlot();
  void fontSizeRangeSlot(double, double);

 private:
  CQChartsScatterPlot*       plot_                { nullptr };
  CQChartsEnumParameterEdit* plotTypeCombo_       { nullptr };
  CQCheckBox*                pointLabelsCheck_    { nullptr };
  CQChartsColumnCombo*       labelColumnCombo_    { nullptr };
  CQEnumCombo*               positionEdit_        { nullptr };
  CQChartsFontLineEdit*      fontEdit_            { nullptr };
  CQChartsColumnCombo*       fontSizeColumnCombo_ { nullptr };
  CQDoubleRangeSlider*       fontSizeRange_       { nullptr };
};

#endif
