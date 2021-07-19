#ifndef CQChartsSummaryPlot_H
#define CQChartsSummaryPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CQChartsColumnNum.h>
#include <CQSummaryModel.h>

class CQChartsScatterPlot;
class CQChartsDistributionPlot;

class QScrollBar;

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
class CQChartsSummaryPlot : public CQChartsPlot {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumns columns     READ columns     WRITE setColumns    )
  Q_PROPERTY(CQChartsColumn  groupColumn READ groupColumn WRITE setGroupColumn)

  // border (TODO: length)
  Q_PROPERTY(double border READ border WRITE setBorder)

  Q_PROPERTY(bool xLabels READ isXLabels WRITE setXLabels)
  Q_PROPERTY(bool yLabels READ isYLabels WRITE setYLabels)

  // cell types
  Q_PROPERTY(DiagonalType    diagonalType      READ diagonalType      WRITE setDiagonalType     )
  Q_PROPERTY(OffDiagonalType upperDiagonalType READ upperDiagonalType WRITE setUpperDiagonalType)
  Q_PROPERTY(OffDiagonalType lowerDiagonalType READ lowerDiagonalType WRITE setLowerDiagonalType)

  // best fit
  Q_PROPERTY(bool bestFit READ isBestFit WRITE setBestFit)

  // density
  Q_PROPERTY(bool density READ isDensity WRITE setDensity)

  // TODO: hull

  Q_ENUMS(DiagonalType)
  Q_ENUMS(OffDiagonalType)

 public:
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

  using ScatterPlot       = CQChartsScatterPlot;
  using DistributionPlot = CQChartsDistributionPlot;

 public:
  CQChartsSummaryPlot(View *view, const ModelP &model);
 ~CQChartsSummaryPlot();

  //---

  void init() override;
  void term() override;

  //---

  void setModel(const ModelP &model) override;

  //---

  //! get/set columns
  const Columns &columns() const { return columns_; }
  void setColumns(const Columns &c);

  //! get/set group column
  const Column &groupColumn() const { return groupColumn_; }
  void setGroupColumn(const Column &c);

  //---

  //! get/set border
  double border() const { return border_; }
  void setBorder(double r) { border_ = r; }

  //---

  //! get x labels
  bool isXLabels() const { return xLabels_; }

  //! get y labels
  bool isYLabels() const { return yLabels_; }

  //---

  // cell types
  const DiagonalType &diagonalType() const { return diagonalType_; }
  void setDiagonalType(const DiagonalType &t);

  const OffDiagonalType &upperDiagonalType() const { return upperDiagonalType_; }
  void setUpperDiagonalType(const OffDiagonalType &t);

  const OffDiagonalType &lowerDiagonalType() const { return lowerDiagonalType_; }
  void setLowerDiagonalType(const OffDiagonalType &t);

  //---

  bool isBestFit() const { return bestFit_; }
  void setBestFit(bool b);

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

  bool addMenuItems(QMenu *menu) override;

  //---

  void postResize() override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(PaintDevice *) const override;

 protected:
  using CellObj = CQChartsSummaryCellObj;

  virtual CellObj *createCellObj(const BBox &bbox, int row, int col) const;

 public slots:
  void setXLabels(bool b);
  void setYLabels(bool b);

  void expandSlot();

 private slots:
  void modelTypeChangedSlot(int modelId);

  void diagonalTypeSlot(bool);
  void upperDiagonalTypeSlot(bool);
  void lowerDiagonalTypeSlot(bool);

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

  BBox cellBBox(int row, int col) const;

 private:
  Columns columns_;     //!< columns
  Column  groupColumn_; //!< group column

  double border_  { 0.05 }; //!< border
  bool   xLabels_ { true }; //!< x labels
  bool   yLabels_ { true }; //!< y labels

  DiagonalType    diagonalType_      { DiagonalType::DISTRIBUTION };   //!< diagonal type
  OffDiagonalType upperDiagonalType_ { OffDiagonalType::SCATTER };     //!< upper diagonal type
  OffDiagonalType lowerDiagonalType_ { OffDiagonalType::CORRELATION }; //!< lower diagonal type

  ScatterPlot*      scatterPlot_      { nullptr };
  DistributionPlot* distributionPlot_ { nullptr };

  bool bestFit_ { false };
  bool density_ { false };

  CQChartsPlotObj* menuObj_ { nullptr }; //!< menu plot object
};

//---

/*!
 * \brief Summary Cell object
 * \ingroup Charts
 */
class CQChartsSummaryCellObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot = CQChartsSummaryPlot;

 public:
  CQChartsSummaryCellObj(const Plot *plot, const BBox &bbox, int row, int col);

  int row() const { return row_; }
  int col() const { return col_; }

  QString typeName() const override { return "cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  void draw(PaintDevice *device) const override;

 private:
  using BucketCount = std::map<int, int>;
  using ValueCount  = std::pair<QVariant, int>;
  using ValueCounts = std::vector<ValueCount>;

 private:
  void drawScatter     (PaintDevice *device) const;
  void drawBestFit     (PaintDevice *device) const;
  void drawCorrelation (PaintDevice *device) const;
  void drawBoxPlot     (PaintDevice *device) const;
  void drawDistribution(PaintDevice *device) const;
  void drawDensity     (PaintDevice *device) const;
  void drawPie         (PaintDevice *device) const;

  void calcBucketCounts(BucketCount &bucketCount, int &maxCount, double &rmin, double &rmax) const;
  void calcValueCounts(ValueCounts &valueCouns, int &maxCount) const;

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

  const Plot*     plot_ { nullptr }; //!< parent plot
  int             row_  { -1 };      //!< row
  int             col_  { -1 };      //!< column
  mutable Polygon poly_;
  GroupValues     groupValues_;
  mutable double  pxmin_ { 0.0 };
  mutable double  pymin_ { 0.0 };
  mutable double  pxmax_ { 1.0 };
  mutable double  pymax_ { 1.0 };
  mutable double  xmin_  { 0.0 };
  mutable double  ymin_  { 0.0 };
  mutable double  xmax_  { 1.0 };
  mutable double  ymax_  { 1.0 };
  mutable double  bmin_  { 0.0 };
  mutable double  bmax_  { 1.0 };
};

//---

#include <CQChartsPlotCustomControls.h>

class QCheckBox;

class CQChartsSummaryPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsSummaryPlotCustomControls(CQCharts *charts);

  virtual void init();

  virtual void addWidgets();

  void setPlot(CQChartsPlot *plot) override;

 protected:
  virtual void connectSlots(bool b);

 public slots:
  void updateWidgets() override;

 protected slots:
  void diagonalTypeSlot();
  void upperDiagonalTypeSlot();
  void lowerDiagonalTypeSlot();

  void bestFitSlot(int);
  void densitySlot(int);

 private:
  CQChartsSummaryPlot* plot_ { nullptr };

  CQChartsEnumParameterEdit* diagonalTypeCombo_      { nullptr };
  CQChartsEnumParameterEdit* upperDiagonalTypeCombo_ { nullptr };
  CQChartsEnumParameterEdit* lowerDiagonalTypeCombo_ { nullptr };

  QCheckBox* bestFitCheck_ { nullptr };
  QCheckBox* densityCheck_ { nullptr };
};

#endif
