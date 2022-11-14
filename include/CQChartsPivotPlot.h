#ifndef CQChartsPivotPlot_H
#define CQChartsPivotPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsGeom.h>

class  CQChartsDataLabel;
struct CQChartsPenBrush;

//---

/*!
 * \brief Pivot Plot type
 * \ingroup Charts
 */
class CQChartsPivotPlotType : public CQChartsPlotType {
 public:
  CQChartsPivotPlotType();

  QString name() const override { return "pivot"; }
  QString desc() const override { return "Pivot"; }

  Category category() const override { return Category::TWO_D; }

  void addParameters() override;

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsPivotPlot;

//---

/*!
 * \brief Pivot Plot Bar Object
 * \ingroup Charts
 */
class CQChartsPivotBarObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double value READ value)

 public:
  using PivotPlot = CQChartsPivotPlot;

 public:
  CQChartsPivotBarObj(const PivotPlot *pivotPlot, const BBox &rect, const QModelIndex &ind,
                      const QModelIndices &inds, const ColorInd &ir, const ColorInd &ic,
                      double value);

  //---

  QString typeName() const override { return "pivot_bar"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  double value() const { return value_; }

  //---

  BBox dataLabelRect() const;

  //---

  CQChartsColorType colorType() const override { return CQChartsColorType::INDEX; }

  //---

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawFg(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

 protected:
  const PivotPlot* pivotPlot_ { nullptr }; //!< parent plot
  double           value_     { 0.0 };     //!< value
  QModelIndex      ind_;                   //!< pivot model index
};

//---

/*!
 * \brief Pivot Plot Line Object
 * \ingroup Charts
 */
class CQChartsPivotLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using PivotPlot = CQChartsPivotPlot;
  using Symbol    = CQChartsSymbol;
  using Length    = CQChartsLength;

 public:
  CQChartsPivotLineObj(const PivotPlot *pivotPlot, const BBox &rect, const QModelIndices &inds,
                       const ColorInd &ig, const Polygon &polygon, const QString &name);

  //---

  QString typeName() const override { return "pivot_line"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

 protected:
  const PivotPlot* pivotPlot_ { nullptr }; //!< parent plot
  Polygon          polygon_;               //!< values
  QString          name_;                  //!< name
};

//---

/*!
 * \brief Pivot Plot Point Object
 * \ingroup Charts
 */
class CQChartsPivotPointObj : public CQChartsPlotPointObj {
  Q_OBJECT

 public:
  using PivotPlot = CQChartsPivotPlot;
  using Symbol    = CQChartsSymbol;

 public:
  CQChartsPivotPointObj(const PivotPlot *pivotPlot, const BBox &rect, const QModelIndices &inds,
                        const ColorInd &ir, const ColorInd &ic, const Point &p, double value);

  //---

  QString typeName() const override { return "pivot_point"; }

  double value() const { return value_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  Length calcSymbolSize() const override;

 protected:
  const PivotPlot* pivotPlot_ { nullptr }; //!< parent plot
  double           value_     { 0.0 };     //!< value
};

//---

/*!
 * \brief Pivot Plot Cell Object
 * \ingroup Charts
 */
class CQChartsPivotCellObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double value READ value)

 public:
  using PivotPlot = CQChartsPivotPlot;
  using Color     = CQChartsColor;
  using Angle     = CQChartsAngle;

 public:
  CQChartsPivotCellObj(const PivotPlot *pivotPlot, const BBox &rect, const QModelIndices &inds,
                       const ColorInd &ir, const ColorInd &ic, const QString &name,
                       double value, double hnorm, double vnorm, bool valid);

  //---

  QString typeName() const override { return "pivot_cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  double value() const { return value_; }

  //---

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &bgPenBrush, bool updateState) const override;

  void calcBgPenBrush(PenBrush &bgPenBrush, bool updateState) const;
  void calcFgPenBrush(PenBrush &bgPenBrush, bool updateState) const;

  //---

  void writeScriptGC(ScriptPaintDevice *device, const PenBrush &penBrush) const override;

  void writeScriptInsideColor(ScriptPaintDevice *device, bool isSave) const override;

 protected:
  const PivotPlot* pivotPlot_ { nullptr }; //!< parent plot
  QString          name_;                  //!< name
  double           value_     { 0.0 };     //!< value
  double           hnorm_     { 0.0 };     //!< value normalized to horizontal value range
  double           vnorm_     { 0.0 };     //!< value normalized to vertical value range
  bool             valid_     { false };   //!< is valid
  Color            color_;                 //!< background color
};

//---

#include <CQChartsKey.h>

/*!
 * \brief Pivot Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsPivotColorKeyItem : public CQChartsColorBoxKeyItem {
  Q_OBJECT

 public:
  using PivotPlot = CQChartsPivotPlot;

 public:
  CQChartsPivotColorKeyItem(PivotPlot *pivotPlot, const ColorInd &ic);

  QBrush fillBrush() const override;

  bool calcHidden() const override;
};

/*!
 * \brief Pivot Plot Key Text
 * \ingroup Charts
 */
class CQChartsPivotTextKeyItem : public CQChartsTextKeyItem {
  Q_OBJECT

 public:
  using PivotPlot = CQChartsPivotPlot;

 public:
  CQChartsPivotTextKeyItem(PivotPlot *pivotPlot, const QString &name);
};

//---

class CQPivotModel;

/*!
 * \brief Pivot Chart Plot
 * \ingroup Charts
 */
class CQChartsPivotPlot : public CQChartsPlot,
 public CQChartsObjBarShapeData<CQChartsPivotPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumns xColumns    READ xColumns    WRITE setXColumns   )
  Q_PROPERTY(CQChartsColumns yColumns    READ yColumns    WRITE setYColumns   )
  Q_PROPERTY(CQChartsColumn  valueColumn READ valueColumn WRITE setValueColumn)

  // options
  Q_PROPERTY(PlotType        plotType    READ plotType    WRITE setPlotType   )
  Q_PROPERTY(ValueType       valueType   READ valueType   WRITE setValueType  )
  Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation)
  Q_PROPERTY(bool            xSorted     READ isXSorted   WRITE setXSorted    )
  Q_PROPERTY(bool            ySorted     READ isYSorted   WRITE setYSorted    )
  Q_PROPERTY(bool            gridBars    READ isGridBars  WRITE setGridBars   )

  // bar fill, stroke
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Bar, bar)

  Q_ENUMS(ValueType)
  Q_ENUMS(PlotType)

 public:
  enum class PlotType {
    BAR,
    STACKED_BAR,
    LINES,
    AREA,
    POINTS,
    GRID
  };

  enum class ValueType {
    COUNT,
    COUNT_UNIQUE,
    SUM,
    MEAN,
    MIN,
    MAX
  };

 public:
  using DataLabel = CQChartsDataLabel;
  using Color     = CQChartsColor;
  using ColorInd  = CQChartsUtil::ColorInd;

 public:
  CQChartsPivotPlot(View *view, const ModelP &model);
 ~CQChartsPivotPlot();

  //---

  void init() override;
  void term() override;

  //---

  const Columns &xColumns() const { return xColumns_; }
  void setXColumns(const Columns &c);

  const Columns &yColumns() const { return yColumns_; }
  void setYColumns(const Columns &c);

  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &c);

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  Columns getNamedColumns(const QString &name) const override;
  void setNamedColumns(const QString &name, const Columns &c) override;

  //---

  const PlotType &plotType() const { return plotType_; }
  void setPlotType(const PlotType &v);

  const ValueType &valueType() const { return valueType_; }
  void setValueType(const ValueType &v);

  //---

  // get/set orientation
  const Qt::Orientation &orientation() const { return orientation_; }
  void setOrientation(const Qt::Orientation &orient);

  bool isHorizontal() const { return orientation() == Qt::Horizontal; }
  bool isVertical  () const { return orientation() == Qt::Vertical  ; }

  //---

  bool isXSorted() const { return xsorted_; }
  void setXSorted(bool b);

  bool isYSorted() const { return ysorted_; }
  void setYSorted(bool b);

  //---

  bool isGridBars() const { return gridBars_; }
  void setGridBars(bool b);

  //--

  CQPivotModel *pivotModel() const { return pivotModel_; }

  //---

  // data label
  const DataLabel *dataLabel() const { return dataLabel_; }
  DataLabel *dataLabel() { return dataLabel_; }

  //---

  void updatePivot();

  //---

  BBox calcExtraFitBBox() const override;

  //---

  void addProperties() override;

  void getPropertyNames(QStringList &names, bool hidden) const override;

  //---

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  Axis *mappedXAxis() const override;
  Axis *mappedYAxis() const override;

  //---

  void addKeyItems(PlotKey *key) override;

  bool addMenuItems(QMenu *menu, const Point &p) override;

  void postObjTree() override;

  //---

  void write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
             const QString &viewVarName) const override;

 protected:
  using BarObj   = CQChartsPivotBarObj;
  using LineObj  = CQChartsPivotLineObj;
  using PointObj = CQChartsPivotPointObj;
  using CellObj  = CQChartsPivotCellObj;

  std::vector<PlotType> plotTypes() const { return
    {{ PlotType::BAR, PlotType::STACKED_BAR, PlotType::LINES,
       PlotType::AREA, PlotType::POINTS, PlotType::GRID }};
  }

  std::vector<ValueType> valueTypes() const { return
    {{ ValueType::COUNT, ValueType::COUNT_UNIQUE, ValueType::SUM,
       ValueType::MEAN, ValueType::MIN, ValueType::MAX }};
  };

  QString plotTypeName (const PlotType  &plotType ) const;
  QString valueTypeName(const ValueType &valueType) const;

  //---

  virtual BarObj *createBarObj(const BBox &rect, const QModelIndex &ind,
                               const QModelIndices &inds, const ColorInd &ir,
                               const ColorInd &ic, double value) const;

  virtual LineObj *createLineObj(const BBox &rect, const QModelIndices &inds,
                                 const ColorInd &ig, const Polygon &polygon,
                                 const QString &name) const;

  virtual PointObj *createPointObj(const BBox &rect, const QModelIndices &inds,
                                   const ColorInd &ir, const ColorInd &ic,
                                   const Point &p, double value) const;

  virtual CellObj *createCellObj(const BBox &rect, const QModelIndices &inds,
                                 const ColorInd &ir, const ColorInd &ic,
                                 const QString &name, double value, double hnorm,
                                 double vnorm, bool valid) const;

 protected Q_SLOTS:
  void setHorizontal(bool b);

  void setPlotTypeSlot (bool b);
  void setValueTypeSlot(bool b);

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  Columns         xColumns_;                       //!< x columns
  Columns         yColumns_;                       //!< y columns
  Column          valueColumn_;                    //!< value columns
  PlotType        plotType_    { PlotType::BAR };  //!< plot type
  ValueType       valueType_   { ValueType::SUM }; //!< value type
  Qt::Orientation orientation_ { Qt::Vertical };   //!< pivot orientation
  bool            xsorted_     { true };           //!< x keys are sorted
  bool            ysorted_     { true };           //!< y keys are sorted
  bool            gridBars_    { true };           //!< draw bars on grid
  CQPivotModel*   pivotModel_  { nullptr };        //!< pivot model
  DataLabel*      dataLabel_   { nullptr };        //!< data label data
};

//---

#include <CQChartsPlotCustomControls.h>

/*!
 * \brief Pivot Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsPivotPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsPivotPlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(Plot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void addWidgets() override;

  void addColumnWidgets() override;

  void addOptionsWidgets() override;

  void connectSlots(bool b) override;

 protected:
  CQChartsPivotPlot* pivotPlot_ { nullptr };
};

#endif
