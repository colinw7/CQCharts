#ifndef CQChartsSummaryPlot_H
#define CQChartsSummaryPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

class CQChartsScatterPlot;
class CQChartsDistributionPlot;
class CQChartsParallelPlot;
class CQChartsBoxPlot;
class CQChartsPiePlot;

//---

/*!
 * \brief Summary plot type
 * \ingroup Charts
 */
class CQChartsSummaryPlotType : public CQChartsPlotType {
 public:
  CQChartsSummaryPlotType();

  QString name() const override { return "summary"; }
  QString desc() const override { return "Summary"; }

  // no category (uses whole model)
  Category category() const override { return Category::NONE; }

  void addParameters() override;

  bool hasKey() const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool allowInvertX() const override { return false; }
  bool allowInvertY() const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  //---

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsSummaryPlot;
class CQChartsSummaryCellObj;

//---

CQCHARTS_NAMED_POINT_DATA(Scatter     , scatter)
CQCHARTS_NAMED_SHAPE_DATA(Distribution, distribution)
CQCHARTS_NAMED_SHAPE_DATA(BoxPlot     , boxPlot)
CQCHARTS_NAMED_TEXT_DATA (BoxPlot     , boxPlot)
CQCHARTS_NAMED_TEXT_DATA (Correlation , correlation)
CQCHARTS_NAMED_SHAPE_DATA(Pie         , pie)
CQCHARTS_NAMED_SHAPE_DATA(Region      , region)

/*!
 * \brief Summary Plot
 * \ingroup Charts
 */
class CQChartsSummaryPlot : public CQChartsPlot,
 public CQChartsObjScatterPointData     <CQChartsSummaryPlot>,
 public CQChartsObjDistributionShapeData<CQChartsSummaryPlot>,
 public CQChartsObjBoxPlotShapeData     <CQChartsSummaryPlot>,
 public CQChartsObjBoxPlotTextData      <CQChartsSummaryPlot>,
 public CQChartsObjPieShapeData         <CQChartsSummaryPlot>,
 public CQChartsObjCorrelationTextData  <CQChartsSummaryPlot>,
 public CQChartsObjXLabelTextData       <CQChartsSummaryPlot>,
 public CQChartsObjYLabelTextData       <CQChartsSummaryPlot>,
 public CQChartsObjRegionShapeData      <CQChartsSummaryPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumns columns     READ columns     WRITE setColumns    )
  Q_PROPERTY(CQChartsColumn  groupColumn READ groupColumn WRITE setGroupColumn)

  Q_PROPERTY(CQChartsColumn symbolTypeColumn READ symbolTypeColumn WRITE setSymbolTypeColumn)
  Q_PROPERTY(CQChartsColumn symbolSizeColumn READ symbolSizeColumn WRITE setSymbolSizeColumn)

  // plot type
  Q_PROPERTY(PlotType plotType READ plotType WRITE setPlotType)

  // border
  Q_PROPERTY(CQChartsLength border READ border WRITE setBorder)

  // x/y labels
  Q_PROPERTY(bool xLabels READ isXLabels WRITE setXLabels)

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(XLabel, xLabel)

  Q_PROPERTY(bool yLabels READ isYLabels WRITE setYLabels)

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(YLabel, yLabel)

  // cell types
  Q_PROPERTY(DiagonalType    diagonalType      READ diagonalType      WRITE setDiagonalType     )
  Q_PROPERTY(OffDiagonalType upperDiagonalType READ upperDiagonalType WRITE setUpperDiagonalType)
  Q_PROPERTY(OffDiagonalType lowerDiagonalType READ lowerDiagonalType WRITE setLowerDiagonalType)

  // options
  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)

  // select mode
  Q_PROPERTY(SelectMode selectMode READ selectMode WRITE setSelectMode)

  // scatter
  CQCHARTS_NAMED_POINT_DATA_PROPERTIES(Scatter, scatter)

  // distribution
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Distribution, distribution)

  // boxplot
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(BoxPlot, boxPlot)

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(BoxPlot, boxPlot)

  // pie
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Pie, pie)

  // correlation
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Correlation, correlation)

  // region
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Region, region)

  // best fit
  Q_PROPERTY(bool bestFit READ isBestFit WRITE setBestFit)

  // density
  Q_PROPERTY(bool density READ isDensity WRITE setDensity)

  // TODO: hull

  // pareto
  Q_PROPERTY(bool           pareto            READ isPareto          WRITE setPareto)
  Q_PROPERTY(CQChartsLength paretoWidth       READ paretoWidth       WRITE setParetoWidth)
  Q_PROPERTY(CQChartsColor  paretoLineColor   READ paretoLineColor   WRITE setParetoLineColor)
  Q_PROPERTY(CQChartsColor  paretoOriginColor READ paretoOriginColor WRITE setParetoOriginColor)

  Q_ENUMS(PlotType)
  Q_ENUMS(DiagonalType)
  Q_ENUMS(OffDiagonalType)
  Q_ENUMS(SelectMode)

 public:
  enum class PlotType {
    MATRIX,
    PARALLEL
  };

  enum class CellType {
    NONE,

    // 1D
    BOXPLOT,
    DISTRIBUTION,
    PIE,
    TREEMAP,
    BUBBLE,

    // 2D
    SCATTER,
    CORRELATION
  };

  enum class DiagonalType {
    NONE         = uint(CellType::NONE),
    BOXPLOT      = uint(CellType::BOXPLOT),
    DISTRIBUTION = uint(CellType::DISTRIBUTION),
    PIE          = uint(CellType::PIE),
    TREEMAP      = uint(CellType::TREEMAP),
    BUBBLE       = uint(CellType::BUBBLE)
  };

  enum class OffDiagonalType {
    NONE        = uint(CellType::NONE),
    SCATTER     = uint(CellType::SCATTER),
    CORRELATION = uint(CellType::CORRELATION)
  };

  enum class SelectMode {
    NONE,
    CELL,
    DATA
  };

  using CellObj          = CQChartsSummaryCellObj;
  using ScatterPlot      = CQChartsScatterPlot;
  using DistributionPlot = CQChartsDistributionPlot;
  using ParallelPlot     = CQChartsParallelPlot;
  using BoxPlot          = CQChartsBoxPlot;
  using PiePlot          = CQChartsPiePlot;
  using PenBrush         = CQChartsPenBrush;
  using PenData          = CQChartsPenData;
  using Symbol           = CQChartsSymbol;
  using Length           = CQChartsLength;
  using Angle            = CQChartsAngle;
  using Color            = CQChartsColor;
  using Alpha            = CQChartsAlpha;
  using ColorInd         = CQChartsUtil::ColorInd;
  using MinMax           = CQChartsGeom::RMinMax;

 public:
  CQChartsSummaryPlot(View *view, const ModelP &model);
 ~CQChartsSummaryPlot();

  //---

  void init() override;
  void term() override;

  //---

  CQChartsPlot *createNamedPlot(const QString &typeName);

  virtual CQChartsPlot *createNamedPlotInstance(const QString &typeName);

  //---

  //! get/set columns
  const Columns &columns() const { return columns_; }
  void setColumns(const Columns &c);

  //! get/set group column
  const Column &groupColumn() const { return groupColumn_; }
  void setGroupColumn(const Column &c);

  // symbol type and size columns
  const Column &symbolTypeColumn() const { return symbolTypeColumn_; }
  void setSymbolTypeColumn(const Column &c);

  const Column &symbolSizeColumn() const { return symbolSizeColumn_; }
  void setSymbolSizeColumn(const Column &c);

  //---

  //! get visible columns
  const Columns &visibleColumns() const { return visibleColumns_; }

  //---

  //! get/set border
  const Length &border() const { return border_; }
  void setBorder(const Length &l);

  //---

  //! get x labels
  bool isXLabels() const { return xLabels_; }

  //! get y labels
  bool isYLabels() const { return yLabels_; }

  //---

  //! get/set plot type
  const PlotType &plotType() const { return plotType_; }
  void setPlotType(const PlotType &t, bool update=true);

  //! get/set expanded
  bool isExpanded() const { return expanded_; }
  void setExpanded(bool b);

  int expandRow() const { return expandRow_; }
  int expandCol() const { return expandCol_; }

  //---

  //! get/set matrix cell types
  const DiagonalType &diagonalType() const { return diagonalType_; }
  void setDiagonalType(const DiagonalType &t);

  const OffDiagonalType &upperDiagonalType() const { return upperDiagonalType_; }
  void setUpperDiagonalType(const OffDiagonalType &t);

  const OffDiagonalType &lowerDiagonalType() const { return lowerDiagonalType_; }
  void setLowerDiagonalType(const OffDiagonalType &t);

  //---

  const Qt::Orientation &orientation() const { return orientation_; }
  void setOrientation(const Qt::Orientation &orient);

  //---

  const SelectMode &selectMode() const { return selectMode_; }
  void setSelectMode(const SelectMode &v);

  //---

  ScatterPlot*      scatterPlot     () const { return scatterPlot_; }
  DistributionPlot* distributionPlot() const { return distributionPlot_; }
  ParallelPlot*     parallelPlot    () const { return parallelPlot_; }
  BoxPlot*          boxPlot         () const { return boxPlot_; }
  PiePlot*          piePlot         () const { return piePlot_; }

  //---

  Length calcScatterSymbolSize() const;

  //---

  // overlays

  //! get/set best fit
  bool isBestFit() const { return bestFit_; }
  void setBestFit(bool b);

  //! get/set density
  bool isDensity() const { return density_; }
  void setDensity(bool b);

  //! get/set pareto
  bool isPareto() const { return pareto_; }
  void setPareto(bool b);

  const Length &paretoWidth() const { return paretoWidth_; }
  void setParetoWidth(const Length &l);

  const Color &paretoLineColor() const { return paretoLineColor_; }
  void setParetoLineColor(const Color &v);

  const Color &paretoOriginColor() const { return paretoOriginColor_; }
  void setParetoOriginColor(const Color &v);

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  Columns getNamedColumns(const QString &name) const override;
  void setNamedColumns(const QString &name, const Columns &c) override;

  //---

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void updateRootChild(Plot *plot) override;

  //---

  QString posStr(const Point &w) const override;

  //---

  bool addMenuItems(QMenu *menu, const Point &p) override;

  //---

  void postResize() override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(PaintDevice *) const override;

  void redrawAxis(CQChartsAxis *axis, bool wait) override;

  void drawXAxis(PaintDevice *device) const override;
  void drawYAxis(PaintDevice *device) const override;

  void drawXGrid(PaintDevice *device) const override;
  void drawYGrid(PaintDevice *device) const override;

  bool hasOverlay() const override;
  void execDrawOverlay(PaintDevice *device) const override;

  //---

  CQChartsGeom::BBox fitBBox() const override;

  //---

  void resetColumnVisible();

  bool isColumnVisible(int ic) const;
  void setColumnVisible(int ic, bool visible);

  //---

  using BucketCount = std::map<int, int>;

  void calcBucketCounts(int ic, BucketCount &bucketCount, int &maxCount,
                        double &rmin, double &rmax) const;
  void calcBucketCounts(const Column &column, BucketCount &bucketCount, int &maxCount,
                        double &rmin, double &rmax) const;

  using ValueCount  = std::pair<QVariant, int>;
  using ValueCounts = std::vector<ValueCount>;

  void calcValueCounts(int ic, ValueCounts &valueCounts, int &maxCount) const;
  void calcValueCounts(const Column &column, ValueCounts &valueCounts, int &maxCount) const;

  //---

  //! plot select interface
  bool handleSelectPress  (const Point &p, SelMod selMod) override;
  bool handleSelectMove   (const Point &p, Constraints constraints, bool first=false) override;
  bool handleSelectRelease(const Point &p) override;

  bool pointSelect(const Point &p, SelMod selMod) override;
  bool rectSelect(const BBox &r, SelMod selMod) override;

  //---

  CellObj *selectedCellObj() const;

  CellObj *cellObjAtPoint(const Point &p) const;

  bool hasColumnRange(const Column &c) const;
  MinMax columnRange(const Column &c) const;
  void setColumnRange(const Column &c, double min, double max);
  void resetColumnRange(const Column &c);

  bool calcColumnRange(const Column &c, double &min, double &max) const;

  void updateColumnRanges(bool notify=true);

  void updateSelectedRows() const;

  void clearColumnRanges();
  void selectColumnRanges();

  bool isRangeSelectedRow(int r) const;
  bool isModelSelectedRow(int r) const;

  //---

  void selectCellPoint(CellObj *obj, int ind, bool add) const;
//void selectCellRect (CellObj *obj, const MinMax &minMax, bool add) const;

  //---

  void expandCell(CellObj *cellObj);
  void collapseCell();

 protected:
  void updatePlots();

  //---

  //! handle double click
  bool handleSelectDoubleClick(const Point &p, SelMod selMod) override;

  //---

  virtual CellObj *createCellObj(const BBox &bbox, int row, int col) const;

 public Q_SLOTS:
  void setXLabels(bool b);
  void setYLabels(bool b);

  void expandSlot();

  void updatePlotsSlot();

 private Q_SLOTS:
  void modelTypeChangedSlot(int modelId);

  void diagonalTypeSlot(bool);
  void upperDiagonalTypeSlot(bool);
  void lowerDiagonalTypeSlot(bool);

 protected:
  void updateVisibleColumns();

  void notifyCollapse() override;

  CQChartsPlotCustomControls *createCustomControls() override;

  BBox cellBBox(int row, int col) const;

 protected:
  using ColumnVisible      = std::map<int, bool>;
  using SelectedColumns    = std::set<int>;
  using SelectedRowColumns = std::map<int, SelectedColumns>;

  Columns columns_;     //!< columns
  Column  groupColumn_; //!< group column

  Column symbolTypeColumn_; //!< symbol type column
  Column symbolSizeColumn_; //!< symbol size column

  Columns visibleColumns_; //!< visible columns

  Length border_ { Length::plot(0.05) }; //!< border

  bool xLabels_ { true }; //!< x labels
  bool yLabels_ { true }; //!< y labels

  PlotType plotType_ { PlotType::MATRIX }; //!< unexpanded plot type

  bool expanded_  { false };
  int  expandRow_ { 0 };
  int  expandCol_ { 0 };

  DiagonalType    diagonalType_      { DiagonalType::DISTRIBUTION };   //!< diagonal type
  OffDiagonalType upperDiagonalType_ { OffDiagonalType::SCATTER };     //!< upper diagonal type
  OffDiagonalType lowerDiagonalType_ { OffDiagonalType::CORRELATION }; //!< lower diagonal type

  Qt::Orientation orientation_ { Qt::Vertical };

  SelectMode selectMode_ { SelectMode::CELL };

  ScatterPlot*      scatterPlot_      { nullptr };
  DistributionPlot* distributionPlot_ { nullptr };
  ParallelPlot*     parallelPlot_     { nullptr };
  BoxPlot*          boxPlot_          { nullptr };
  PiePlot*          piePlot_          { nullptr };

  bool bestFit_ { false };
  bool density_ { false };

  bool   pareto_            { false };
  Length paretoWidth_       { Length::pixel(5) };
  Color  paretoLineColor_   { Color::makePalette() };
  Color  paretoOriginColor_ { Color::makeInterfaceValue(0.5) };

  CQChartsPlotObj* menuObj_ { nullptr }; //!< menu plot object

  ColumnVisible columnVisible_;

  using ColumnRange = std::map<Column, MinMax>;

  ColumnRange columnRange_;

  mutable int                nc_ { 0 };
  mutable SelectedRowColumns rangeSelectedRows_;
  mutable SelectedRowColumns modelSelectedRows_;

  bool selectAdd_ { false };
};

//---

/*!
 * \brief Summary Cell object
 * \ingroup Charts
 */
class CQChartsSummaryCellObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using SummaryPlot = CQChartsSummaryPlot;
  using Length      = CQChartsLength;
  using Constraints = CQChartsPlot::Constraints;
  using MinMax      = CQChartsGeom::RMinMax;

 public:
  CQChartsSummaryCellObj(const SummaryPlot *plot, const BBox &bbox, int row, int col);

  int row() const { return row_; }
  int col() const { return col_; }

  int maxCount() const { return maxCount_; }

  //---

  Column rowColumn() const;
  Column colColumn() const;

  Column visibleColumn(int i) const;

  QString typeName() const override { return "cell"; }

  QString calcId() const override;

  //---

  bool dynamicTipId() const override;

  QString calcTipId() const override;

  //---

  void initCoords() const;

  void draw(PaintDevice *device) const override;

  void drawOverlay(PaintDevice *device) const;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  CQChartsSummaryPlot::CellType getCellType() const;

  Point plotToParent(const Point &w) const;
  Point parentToPlot(const Point &p) const;

  //---

  virtual bool handleSelectPress  (const Point &p, SelMod selMod);
  virtual bool handleSelectMove   (const Point &p, Constraints constraints, bool first=false);
  virtual bool handleSelectRelease(const Point &p, bool add);

  //---

  void resetInside();
  void updateSelectData(const Point &p);

 protected:
  void drawXAxis(PaintDevice *device) const;
  void drawXGrid(PaintDevice *device) const;
  void initXAxis() const;

  void drawYAxis(PaintDevice *device) const;
  void drawYGrid(PaintDevice *device) const;
  void initYAxis() const;

  void drawScatter     (PaintDevice *device) const;
  void drawBestFit     (PaintDevice *device) const;
  void drawCorrelation (PaintDevice *device) const;
  void drawBoxPlot     (PaintDevice *device) const;
  void drawDistribution(PaintDevice *device) const;
  void drawDensity     (PaintDevice *device) const;
  void drawPareto      (PaintDevice *device) const;
  void drawPie         (PaintDevice *device) const;

  void initGroupedValues();

 protected:
  using Values = std::vector<QVariant>;
  using Reals  = std::vector<double>;

  struct IndData {
    Values  xvals;
    Values  yvals;
    Reals   x;
    Reals   y;
    Polygon poly;
  };

  using GroupIndData = std::map<int, IndData>;

  struct GroupValues {
    bool         set { false };
    GroupIndData groupIndData;
  };

  //---

  struct PointData {
    int            ind           { -1 }; // model row
    Point          p;
    CQChartsSymbol symbol;
    Length         symbolSize;
    bool           inside        { false };
    bool           rangeSelected { false };
    bool           modelSelected { false };
  };

  using PointDatas = std::vector<PointData>;

  struct RectData {
    int  ind { -1 }; // model row
    BBox bbox;
    BBox pbbox;
    bool inside { false };
  };

  using RectDatas = std::vector<RectData>;

  mutable PointDatas pointDatas_;
  mutable RectDatas  rectDatas_;

  mutable PointData* selectPointData_ { nullptr };
  mutable RectData*  selectRectData_  { nullptr };

  //---

  const SummaryPlot* summaryPlot_ { nullptr }; //!< parent plot
  int                row_         { -1 };      //!< row
  int                col_         { -1 };      //!< column
  GroupValues        groupValues_;

  mutable Polygon poly_;
  mutable BBox    rangeBox_;

  // global
  mutable int nc_ { 0 };

  // border
  mutable double bx_ { 0.0 };
  mutable double by_ { 0.0 };

  // child plot position in parent plot coords
  mutable double pxmin_ { 0.0 };
  mutable double pymin_ { 0.0 };
  mutable double pxmax_ { 1.0 };
  mutable double pymax_ { 1.0 };

  // child plot data range
  mutable double xmin_ { 0.0 };
  mutable double ymin_ { 0.0 };
  mutable double xmax_ { 1.0 };
  mutable double ymax_ { 1.0 };

  // bucket x min/max and max bucket height for child distribution plot
  // min/max range for box plot
  mutable double bmin_     { 0.0 };
  mutable double bmax_     { 1.0 };
  mutable int    maxCount_ { 1 };
};

//---

#include <CQChartsPlotCustomControls.h>

class CQChartsSummaryPlotGroupStats;
class CQChartsSummaryPlotColumnChooser;
class CQChartsSummaryPlotRangeList;

class QCheckBox;
class QPushButton;

/*!
 * \brief Summary Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsSummaryPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsSummaryPlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(Plot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void addWidgets() override;

  void addColumnWidgets() override;

  virtual void addGroupStatsWidgets();

  virtual void addRangeList();

  void addOptionsWidgets() override;

  virtual void addExpandControls();

  void connectSlots(bool b) override;

 protected Q_SLOTS:
  void plotTypeSlot();

  void diagonalTypeSlot();
  void upperDiagonalTypeSlot();
  void lowerDiagonalTypeSlot();

  void bestFitSlot(int);
  void densitySlot(int);
  void paretoSlot(int);

  void expandSlot();
  void deselectSlot();
  void selectSlot();

 protected:
  CQChartsSummaryPlot* summaryPlot_ { nullptr };

  FrameData optionsFrame_;
  FrameData buttonsFrame_;

  CQChartsEnumParameterEdit* plotTypeCombo_          { nullptr };
  CQChartsEnumParameterEdit* diagonalTypeCombo_      { nullptr };
  CQChartsEnumParameterEdit* upperDiagonalTypeCombo_ { nullptr };
  CQChartsEnumParameterEdit* lowerDiagonalTypeCombo_ { nullptr };

  QCheckBox* bestFitCheck_ { nullptr };
  QCheckBox* densityCheck_ { nullptr };
  QCheckBox* paretoCheck_  { nullptr };

  CQChartsSummaryPlotGroupStats*    stats_     { nullptr };
  CQChartsSummaryPlotColumnChooser* chooser_   { nullptr };
  CQChartsSummaryPlotRangeList*     rangeList_ { nullptr };

  QPushButton *expandButton_   { nullptr };
  QPushButton *deselectButton_ { nullptr };
  QPushButton *selectButton_   { nullptr };
};

//---

/*!
 * \brief Summary Plot Stats Widget
 * \ingroup Charts
 */
class CQChartsSummaryPlotGroupStats : public QFrame {
  Q_OBJECT

 public:
  CQChartsSummaryPlotGroupStats(CQChartsSummaryPlot *summaryPlot=nullptr);

  CQChartsSummaryPlot *summaryPlot() const { return summaryPlot_; }
  void setPlot(CQChartsSummaryPlot *summaryPlot) { summaryPlot_ = summaryPlot; }

  void updateWidgets();

  QSize sizeHint() const override;

 private:
  CQChartsSummaryPlot* summaryPlot_ { nullptr };
  CQTableWidget*       valueList_   { nullptr };
};

//---

class CQChartsSummaryPlotRangeLabel;
class CQChartsSummaryPlotRangeNoEdit;
class CQChartsGeomMinMaxEdit;
class CQIconButton;

/*!
 * \brief Summary Plot Range List
 * \ingroup Charts
 */
class CQChartsSummaryPlotRangeList : public QFrame {
  Q_OBJECT

 public:
  CQChartsSummaryPlotRangeList(CQChartsSummaryPlot *summaryPlot=nullptr);

  CQChartsSummaryPlot *summaryPlot() const { return summaryPlot_; }
  void setPlot(CQChartsSummaryPlot *summaryPlot) { summaryPlot_ = summaryPlot; }

  void updateWidgets();

  QSize sizeHint() const override;

 private Q_SLOTS:
  void rangeChanged();
  void clearRange();

 private:
  struct WidgetData {
    CQChartsSummaryPlotRangeLabel*  label  { nullptr };
    QFrame*                         frame  { nullptr };
    CQChartsGeomMinMaxEdit*         edit   { nullptr };
    CQChartsSummaryPlotRangeNoEdit* noedit { nullptr };
    CQIconButton*                   clear  { nullptr };
  };

  using Widgets = std::vector<WidgetData>;

  CQChartsSummaryPlot* summaryPlot_ { nullptr };
  QGridLayout*         layout_      { nullptr };
  Widgets              widgets_;
};

#include <QLabel>

class CQChartsSummaryPlotRangeLabel : public QLabel {
  Q_OBJECT

 public:
  CQChartsSummaryPlotRangeLabel(CQChartsSummaryPlotRangeList *list, const QString &label);

 private:
  CQChartsSummaryPlotRangeList* list_ { nullptr };
};

class CQChartsSummaryPlotRangeNoEdit : public QLabel {
  Q_OBJECT

 public:
  CQChartsSummaryPlotRangeNoEdit(CQChartsSummaryPlotRangeList *list);

  void mouseDoubleClickEvent(QMouseEvent *) override;

 private:
  CQChartsSummaryPlotRangeList* list_ { nullptr };
};

//---

/*!
 * \brief Summary Plot Column Chooser Widget
 * \ingroup Charts
 */
class CQChartsSummaryPlotColumnChooser : public CQChartsPlotColumnChooser {
  Q_OBJECT

 public:
  CQChartsSummaryPlotColumnChooser(CQChartsSummaryPlot *plot=nullptr);

  const CQChartsColumns &getColumns() const override;

  bool isColumnVisible(int ic) const override;
  void setColumnVisible(int ic, bool visible) override;
};

#endif
