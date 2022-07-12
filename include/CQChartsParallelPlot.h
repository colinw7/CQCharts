#ifndef CQChartsParallelPlot_H
#define CQChartsParallelPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CSafeIndex.h>

//---

/*!
 * \brief Parallel plot type
 * \ingroup Charts
 */
class CQChartsParallelPlotType : public CQChartsPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsParallelPlotType();

  QString name() const override { return "parallel"; }
  QString desc() const override { return "Parallel"; }

  Category category() const override { return Category::TWO_D; }

  void addParameters() override;

  bool allowXLog() const override { return false; }

  bool canProbe() const override { return true; }

  QString description() const override;

  //---

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsParallelPlot;

/*!
 * \brief Parallel Plot Line object
 * \ingroup Charts
 */
class CQChartsParallelLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Plot = CQChartsParallelPlot;

 public:
  CQChartsParallelLineObj(const Plot *plot, const BBox &rect, const Polygon &poly,
                          const QModelIndex &ind, const ColorInd &is);

  //---

  QString typeName() const override { return "line"; }

  //---

  bool isPolygon() const override { return true; }
  Polygon polygon() const override { return polyLine_; }

  bool isSolid() const override { return false; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isVisible() const override;

  bool inside(const Point &p) const override;

  //bool interpY(double x, std::vector<double> &yvals) const;

  //---

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const override;

 private:
  void getPolyLine(Polygon &poly) const;

  QString xName() const;

 private:
  const Plot*     plot_ { nullptr }; //!< plot
  Polygon         poly_;             //!< polygon
  mutable Polygon polyLine_;         //!< polyline
};

//---

/*!
 * \brief Parallel Plot Point object
 * \ingroup Charts
 */
class CQChartsParallelPointObj : public CQChartsPlotPointObj {
  Q_OBJECT

 public:
  using Plot   = CQChartsParallelPlot;
  using Column = CQChartsColumn;
  using Length = CQChartsLength;
  using Symbol = CQChartsSymbol;

 public:
  CQChartsParallelPointObj(const Plot *plot, const BBox &rect, double yval, const Point &p,
                           const QModelIndex &ind, const ColorInd &is, const ColorInd &iv);

  //---

  const Plot *plot() const { return plot_; }

  double yval() const { return yval_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isVisible() const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  Length calcSymbolSize() const override;

 private:
  QString xName() const;

 private:
  const Plot* plot_ { nullptr }; //!< plot
  double      yval_ { 0.0 };     //!< y value
};

//---

/*!
 * \brief Parallel Plot
 * \ingroup Charts
 */
class CQChartsParallelPlot : public CQChartsPlot,
 public CQChartsObjLineData <CQChartsParallelPlot>,
 public CQChartsObjPointData<CQChartsParallelPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn  xColumn  READ xColumn  WRITE setXColumn )
  Q_PROPERTY(CQChartsColumns yColumns READ yColumns WRITE setYColumns)

  // options
  Q_PROPERTY(Qt::Orientation orientation  READ orientation  WRITE setOrientation)
  Q_PROPERTY(AxisLabelPos    axisLabelPos READ axisLabelPos WRITE setAxisLabelPos)

  // lines (display, stroke)
  CQCHARTS_LINE_DATA_PROPERTIES

  Q_PROPERTY(bool linesSelectable READ isLinesSelectable WRITE setLinesSelectable)

  // points (display, symbol)
  CQCHARTS_POINT_DATA_PROPERTIES

  Q_ENUMS(AxisLabelPos)

 public:
  using Color    = CQChartsColor;
  using Alpha    = CQChartsAlpha;
  using Symbol   = CQChartsSymbol;
  using PenBrush = CQChartsPenBrush;
  using PenData  = CQChartsPenData;
  using Length   = CQChartsLength;
  using ColorInd = CQChartsUtil::ColorInd;

  enum class AxisLabelPos {
    AXIS,
    TOP,
    BOTTOM,
    ALTERNATE
  };

 public:
  CQChartsParallelPlot(View *view, const ModelP &model);
 ~CQChartsParallelPlot();

  //---

  void init() override;
  void term() override;

  //---

  // columns
  const Column &xColumn() const { return xColumn_; }
  void setXColumn(const Column &c);

  const Columns &yColumns() const { return yColumns_; }
  void setYColumns(const Columns &c);

  //---

  //! get visible y columns
  const Columns &visibleYColumns() const { return visibleYColumns_; }

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  Columns getNamedColumns(const QString &name) const override;
  void setNamedColumns(const QString &name, const Columns &c) override;

  //---

  //! get/set orientation
  const Qt::Orientation &orientation() const { return orientation_; }
  void setOrientation(const Qt::Orientation &orient);

  bool isHorizontal() const { return orientation() == Qt::Horizontal; }
  bool isVertical  () const { return orientation() == Qt::Vertical  ; }

  //---

  //! get/set lines selectable
  bool isLinesSelectable() const { return linesSelectable_; }
  void setLinesSelectable(bool b);

  //---

  //! get/set axis label pos
  const AxisLabelPos &axisLabelPos() const { return axisLabelPos_; }
  void setAxisLabelPos(const AxisLabelPos &p);

  //---

  Range setRange(int i) const {
    if (i >= 0 && i < int(setRanges_.size()))
      return setRanges_[size_t(i)];

    return Range();
  }

  Axis *axis(int i) const { return CUtil::safeIndex(axes_, i).get(); }

  //---

  void addProperties() override;

  Range calcRange() const override;

  void updateAxes();

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool rowColValue(const CQChartsModelColumnDetails *details,
                   const ModelIndex &ind, double &value, double defVal) const;

  //---

  bool probe(ProbeData &probeData) const override;

  bool addMenuItems(QMenu *menu, const Point &p) override;

  //---

  BBox axesFitBBox() const override;

  BBox calcExtraFitBBox() const override;

  bool hasFgAxes() const override;

  void drawFgAxes(PaintDevice *device) const override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *) const override;

  //---

  void resetYColumnVisible();

  bool isYColumnVisible(int ic) const;
  void setYColumnVisible(int ic, bool visible);

 protected:
  using LineObj  = CQChartsParallelLineObj;
  using PointObj = CQChartsParallelPointObj;

  virtual LineObj *createLineObj(const BBox &rect, const Polygon &poly, const QModelIndex &ind,
                                 const ColorInd &is) const;

  virtual PointObj *createPointObj(const BBox &rect, double yval, const Point &p,
                                   const QModelIndex &ind, const ColorInd &is,
                                   const ColorInd &iv) const;

 public Q_SLOTS:
  // set horizontal
  void setHorizontal(bool b);

 protected:
  void updateVisibleYColumns();

  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  using Ranges        = std::vector<Range>;
  using AxisP         = std::unique_ptr<CQChartsAxis>;
  using YAxes         = std::vector<AxisP>;
  using ColumnVisible = std::map<int, bool>;

  Column  xColumn_;  //!< x value column
  Columns yColumns_; //!< y value columns

  Columns       visibleYColumns_; //!< calculated visible y columns
  ColumnVisible yColumnVisible_;  //!< visible y column list

  Qt::Orientation orientation_     { Qt::Vertical }; //!< axis orientation
  bool            linesSelectable_ { false };        //!< are lines selectable

  Ranges setRanges_; //!< value set ranges

  Qt::Orientation    adir_                 { Qt::Horizontal }; //!< axis direction
  AxisP              masterAxis_;                              //!< master axis
  YAxes              axes_;                                    //!< value axes
  mutable std::mutex axesMutex_;                               //!< value axes
  Range              normalizedDataRange_;                     //!< normalized data range
  BBox               axesBBox_;                                //!< axes bbox

  double max_tw_ { 0.0 };             //!< max text width

  AxisLabelPos axisLabelPos_ { AxisLabelPos::TOP };
};

//---

#include <CQChartsPlotCustomControls.h>

class CQChartsParallelPlotColumnChooser;

/*!
 * \brief Parallel Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsParallelPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsParallelPlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(CQChartsPlot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void addWidgets() override;

  void addColumnWidgets() override;

  void addOptionsWidgets() override;

  void connectSlots(bool b) override;

  //---

  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 protected Q_SLOTS:
  void orientationSlot();

 protected:
  CQChartsParallelPlot* plot_ { nullptr };

  FrameData optionsFrame_;

  CQChartsEnumParameterEdit* orientationCombo_ { nullptr };

  CQChartsParallelPlotColumnChooser* chooser_ { nullptr };
};

//---

/*!
 * \brief Parallel Plot Column Chooser Widget
 * \ingroup Charts
 */
class CQChartsParallelPlotColumnChooser : public CQChartsPlotColumnChooser {
  Q_OBJECT

 public:
  CQChartsParallelPlotColumnChooser(CQChartsParallelPlot *plot=nullptr);

  const CQChartsColumns &getColumns() const override;

  bool isColumnVisible(int ic) const override;
  void setColumnVisible(int ic, bool visible) override;
};

#endif
