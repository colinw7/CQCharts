#ifndef CQChartsPivotPlot_H
#define CQChartsPivotPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsGeom.h>

class CQChartsDataLabel;
class CQChartsPenBrush;

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

  Dimension dimension() const override { return Dimension::TWO_D; }

  void addParameters() override;

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
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

 public:
  CQChartsPivotBarObj(const CQChartsPivotPlot *plot, const CQChartsGeom::BBox &rect,
                      const QModelIndex &ind, const ModelIndices &inds, const ColorInd &ir,
                      const ColorInd &ic, double value);

  //---

  QString typeName() const override { return "pivot_bar"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  double value() const { return value_; }

  //---

  CQChartsGeom::BBox dataLabelRect() const;

  //---

  CQChartsColorType colorType() const override { return CQChartsColorType::INDEX; }

  //---

  void getSelectIndices(Indices &inds) const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  void drawFg(CQChartsPaintDevice *device) const override;

  //---

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  void writeScriptData(std::ostream &os) const override;

 protected:
  const CQChartsPivotPlot* plot_  { nullptr }; //!< parent plot
  double                   value_ { 0.0 };     //!< value
  QModelIndex              ind_;               //!< pivot model index
};

//---

/*!
 * \brief Pivot Plot Line Object
 * \ingroup Charts
 */
class CQChartsPivotLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsPivotLineObj(const CQChartsPivotPlot *plot, const CQChartsGeom::BBox &rect,
                       const ModelIndices &inds, const ColorInd &ig, const QPolygonF &polygon,
                       const QString &name);

  //---

  QString typeName() const override { return "pivot_line"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool inside(const CQChartsGeom::Point&) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 protected:
  const CQChartsPivotPlot* plot_     { nullptr }; //!< parent plot
  QPolygonF                polygon_;              //!< values
  QString                  name_;                 //!< name
};

//---

/*!
 * \brief Pivot Plot Point Object
 * \ingroup Charts
 */
class CQChartsPivotPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsPivotPointObj(const CQChartsPivotPlot *plot, const CQChartsGeom::BBox &rect,
                        const QModelIndex &ind, const ColorInd &ir, const ColorInd &ic,
                        const QPointF &p, double value);

  //---

  QString typeName() const override { return "pivot_point"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  double value() const { return value_; }

  //---

  bool inside(const CQChartsGeom::Point&) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 protected:
  const CQChartsPivotPlot* plot_  { nullptr }; //!< parent plot
  QPointF                  p_;                 //!< position
  double                   value_ { 0.0 };     //!< value
};

//---

/*!
 * \brief Pivot Plot Cell Object
 * \ingroup Charts
 */
class CQChartsPivotCellObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsPivotCellObj(const CQChartsPivotPlot *plot, const CQChartsGeom::BBox &rect,
                       const QModelIndex &ind, const ColorInd &ir, const ColorInd &ic,
                       const QString &name, double value, double hnorm, double vnorm,
                       bool valid);

  //---

  QString typeName() const override { return "pivot_cell"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  double value() const { return value_; }

  //---

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 protected:
  const CQChartsPivotPlot* plot_  { nullptr }; //!< parent plot
  QString                  name_;              //!< name
  double                   value_ { 0.0 };     //!< value
  double                   hnorm_ { 0.0 };     //!< value normalized to horizontal value range
  double                   vnorm_ { 0.0 };     //!< value normalized to vertical value range
  bool                     valid_ { false };   //!< is valid
  CQChartsColor            color_;             //!< background color
};

//---

#include <CQChartsKey.h>

/*!
 * \brief Pivot Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsPivotKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsPivotKeyColor(CQChartsPivotPlot *plot, const ColorInd &ic);

  QBrush fillBrush() const override;
};

/*!
 * \brief Pivot Plot Key Text
 * \ingroup Charts
 */
class CQChartsPivotKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsPivotKeyText(CQChartsPivotPlot *plot, const QString &name);
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
  Q_PROPERTY(PlotType  plotType   READ plotType     WRITE setPlotType  )
  Q_PROPERTY(ValueType valueType  READ valueType    WRITE setValueType )
  Q_PROPERTY(bool      horizontal READ isHorizontal WRITE setHorizontal)
  Q_PROPERTY(bool      xSorted    READ isXSorted    WRITE setXSorted   )
  Q_PROPERTY(bool      ySorted    READ isYSorted    WRITE setYSorted   )
  Q_PROPERTY(bool      gridBars   READ isGridBars   WRITE setGridBars  )

  // bar fill, stroke
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Bar,bar)

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
    AVERAGE,
    MIN,
    MAX
  };

 public:
  CQChartsPivotPlot(CQChartsView *view, const ModelP &model);
 ~CQChartsPivotPlot();

  //---

  const CQChartsColumns &xColumns() const { return xColumns_; }
  void setXColumns(const CQChartsColumns &c);

  const CQChartsColumns &yColumns() const { return yColumns_; }
  void setYColumns(const CQChartsColumns &c);

  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c);

  const PlotType &plotType() const { return plotType_; }
  void setPlotType(const PlotType &v);

  const ValueType &valueType() const { return valueType_; }
  void setValueType(const ValueType &v);

  bool isHorizontal() const { return horizontal_; }

  bool isXSorted() const { return xsorted_; }
  void setXSorted(bool b);

  bool isYSorted() const { return ysorted_; }
  void setYSorted(bool b);

  bool isGridBars() const { return gridBars_; }
  void setGridBars(bool b);

  //--

  CQPivotModel *pivotModel() const { return pivotModel_; }

  //---

  // data label
  const CQChartsDataLabel *dataLabel() const { return dataLabel_; }
  CQChartsDataLabel *dataLabel() { return dataLabel_; }

  //---

  void updatePivot();

  //---

  CQChartsGeom::BBox annotationBBox() const override;

  //---

  void addProperties() override;

  void getPropertyNames(QStringList &names, bool hidden) const override;

  //---

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  CQChartsAxis *mappedXAxis() const override;
  CQChartsAxis *mappedYAxis() const override;

  //---

  void addKeyItems(CQChartsPlotKey *key) override;

  bool addMenuItems(QMenu *menu) override;

  void postObjTree() override;

  //---

  void write(std::ostream &os, const QString &varName="",
             const QString &modelName="") const override;

 private:
  std::vector<PlotType> plotTypes() const { return
    {{ PlotType::BAR, PlotType::STACKED_BAR, PlotType::LINES,
       PlotType::AREA, PlotType::POINTS, PlotType::GRID }};
  }

  std::vector<ValueType> valueTypes() const { return
    {{ ValueType::COUNT, ValueType::COUNT_UNIQUE, ValueType::SUM,
       ValueType::AVERAGE, ValueType::MIN, ValueType::MAX }};
  };

  QString plotTypeName (const PlotType  &plotType ) const;
  QString valueTypeName(const ValueType &valueType) const;

 private slots:
  void setHorizontal(bool b);

  void setPlotTypeSlot (bool b);
  void setValueTypeSlot(bool b);

 private:
  CQChartsColumns    xColumns_;                       //!< x columns
  CQChartsColumns    yColumns_;                       //!< y columns
  CQChartsColumn     valueColumn_;                    //!< value columns
  PlotType           plotType_    { PlotType::BAR };  //!< plot type
  ValueType          valueType_   { ValueType::SUM }; //!< value type
  bool               horizontal_  { false };          //!< horizontal
  bool               xsorted_     { true };           //!< x keys are sorted
  bool               ysorted_     { true };           //!< y keys are sorted
  bool               gridBars_    { true };           //!< draw bars on grid
  CQPivotModel*      pivotModel_  { nullptr };        //!< pivot model
  CQChartsDataLabel* dataLabel_   { nullptr };        //!< data label data
};

#endif
