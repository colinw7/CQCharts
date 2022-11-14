#ifndef CQChartsSummaryPlot_H
#define CQChartsSummaryPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

class CQChartsScatterPlot;
class CQChartsDistributionPlot;
class CQChartsParallelPlot;

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

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

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

/*!
 * \brief Summary Plot
 * \ingroup Charts
 */
class CQChartsSummaryPlot : public CQChartsPlot,
 public CQChartsObjXLabelTextData<CQChartsSummaryPlot>,
 public CQChartsObjYLabelTextData<CQChartsSummaryPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumns columns     READ columns     WRITE setColumns    )
  Q_PROPERTY(CQChartsColumn  groupColumn READ groupColumn WRITE setGroupColumn)

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

  // scatter
  Q_PROPERTY(CQChartsLength symbolSize READ symbolSize WRITE setSymbolSize)

  // best fit
  Q_PROPERTY(bool bestFit READ isBestFit WRITE setBestFit)

  // density
  Q_PROPERTY(bool density READ isDensity WRITE setDensity)

  // TODO: hull

  Q_ENUMS(PlotType)
  Q_ENUMS(DiagonalType)
  Q_ENUMS(OffDiagonalType)

 public:
  enum class PlotType {
    MATRIX,
    PARALLEL
  };

  enum class DiagonalType {
    NONE,
    BOXPLOT,
    DISTRIBUTION,
    PIE,
    TREEMAP,
    BUBBLE
  };

  enum class OffDiagonalType {
    NONE,
    SCATTER,
    CORRELATION
  };

  using ScatterPlot      = CQChartsScatterPlot;
  using DistributionPlot = CQChartsDistributionPlot;
  using ParallelPlot     = CQChartsParallelPlot;
  using CellObj          = CQChartsSummaryCellObj;
  using Length           = CQChartsLength;
  using ColorInd         = CQChartsUtil::ColorInd;

 public:
  CQChartsSummaryPlot(View *view, const ModelP &model);
 ~CQChartsSummaryPlot();

  //---

  void init() override;
  void term() override;

  //---

  //! get/set columns
  const Columns &columns() const { return columns_; }
  void setColumns(const Columns &c);

  //! get/set group column
  const Column &groupColumn() const { return groupColumn_; }
  void setGroupColumn(const Column &c);

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

  //---

  //! get/set matrix cell types
  const DiagonalType &diagonalType() const { return diagonalType_; }
  void setDiagonalType(const DiagonalType &t);

  const OffDiagonalType &upperDiagonalType() const { return upperDiagonalType_; }
  void setUpperDiagonalType(const OffDiagonalType &t);

  const OffDiagonalType &lowerDiagonalType() const { return lowerDiagonalType_; }
  void setLowerDiagonalType(const OffDiagonalType &t);

  //---

  const Length &symbolSize() const { return symbolSize_; }
  void setSymbolSize(const Length &l);

  //---

  // overlays

  //! get/set best fit
  bool isBestFit() const { return bestFit_; }
  void setBestFit(bool b);

  //! get/set density
  bool isDensity() const { return density_; }
  void setDensity(bool b);

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

  bool addMenuItems(QMenu *menu, const Point &p) override;

  //---

  void postResize() override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(PaintDevice *) const override;

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

  CellObj *selectedCellObj() const;

  void expandCell(CellObj *cellObj);
  void collapseCell();

 protected:
  void updatePlots();

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

 private:
  using ColumnVisible = std::map<int, bool>;

  Columns columns_;        //!< columns
  Column  groupColumn_;    //!< group column
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

  ScatterPlot*      scatterPlot_      { nullptr };
  DistributionPlot* distributionPlot_ { nullptr };
  ParallelPlot*     parallelPlot_     { nullptr };

  bool bestFit_ { false };
  bool density_ { false };

  Length symbolSize_ { Length::plot(0.03) }; //!< scatter symbol size

  CQChartsPlotObj* menuObj_ { nullptr }; //!< menu plot object

  ColumnVisible columnVisible_;
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

 public:
  CQChartsSummaryCellObj(const SummaryPlot *plot, const BBox &bbox, int row, int col);

  int row() const { return row_; }
  int col() const { return col_; }

  QString typeName() const override { return "cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

 private:
  void drawScatter     (PaintDevice *device) const;
  void drawBestFit     (PaintDevice *device) const;
  void drawCorrelation (PaintDevice *device) const;
  void drawBoxPlot     (PaintDevice *device) const;
  void drawDistribution(PaintDevice *device) const;
  void drawDensity     (PaintDevice *device) const;
  void drawPie         (PaintDevice *device) const;

  void initGroupedValues();

 private:
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

  const SummaryPlot* summaryPlot_ { nullptr }; //!< parent plot
  int                row_         { -1 };      //!< row
  int                col_         { -1 };      //!< column
  mutable Polygon    poly_;
  GroupValues        groupValues_;
  mutable double     pxmin_       { 0.0 };
  mutable double     pymin_       { 0.0 };
  mutable double     pxmax_       { 1.0 };
  mutable double     pymax_       { 1.0 };
  mutable double     xmin_        { 0.0 };
  mutable double     ymin_        { 0.0 };
  mutable double     xmax_        { 1.0 };
  mutable double     ymax_        { 1.0 };
  mutable double     bmin_        { 0.0 };
  mutable double     bmax_        { 1.0 };
};

//---

#include <CQChartsPlotCustomControls.h>

class CQChartsSummaryPlotGroupStats;
class CQChartsSummaryPlotColumnChooser;

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

  void expandSlot();

 protected:
  CQChartsSummaryPlot* summaryPlot_ { nullptr };

  FrameData optionsFrame_;

  CQChartsEnumParameterEdit* plotTypeCombo_          { nullptr };
  CQChartsEnumParameterEdit* diagonalTypeCombo_      { nullptr };
  CQChartsEnumParameterEdit* upperDiagonalTypeCombo_ { nullptr };
  CQChartsEnumParameterEdit* lowerDiagonalTypeCombo_ { nullptr };

  QCheckBox* bestFitCheck_ { nullptr };
  QCheckBox* densityCheck_ { nullptr };

  CQChartsSummaryPlotGroupStats*    stats_   { nullptr };
  CQChartsSummaryPlotColumnChooser* chooser_ { nullptr };

  QPushButton *expandButton_ { nullptr };
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
