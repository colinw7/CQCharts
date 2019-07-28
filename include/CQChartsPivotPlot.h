#ifndef CQChartsPivotPlot_H
#define CQChartsPivotPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsGeom.h>

//---

/*!
 * \brief Pivot Plot type
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
 */
class CQChartsPivotBarObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsPivotBarObj(const CQChartsPivotPlot *plot, const CQChartsGeom::BBox &rect,
                      const QModelIndex &ind, const ColorInd &ir, const ColorInd &ic,
                      double value);

  //---

  QString typeName() const override { return "pivot_bar"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  //---

 protected:
  const CQChartsPivotPlot* plot_  { nullptr }; //!< parent plot
  QModelIndex              ind_;               //!< model index
  double                   value_ { 0.0 };     //!< value
};

//---

/*!
 * \brief Pivot Plot Line Object
 */
class CQChartsPivotLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using ModelIndices = std::vector<QModelIndex>;

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

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  //---

 protected:
  const CQChartsPivotPlot* plot_     { nullptr }; //!< parent plot
  ModelIndices             inds_;                 //!< model index
  QPolygonF                polygon_;              //!< values
  QString                  name_;                 //!< name
};

//---

/*!
 * \brief Pivot Plot Point Object
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

  bool inside(const CQChartsGeom::Point&) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  //---

 protected:
  const CQChartsPivotPlot* plot_   { nullptr }; //!< parent plot
  QModelIndex              ind_;                //!< model index
  QPointF                  p_;                  //!< position
  double                   value_  { 0.0 };     //!< value
};

//---

/*!
 * \brief Pivot Plot Cell Object
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

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

  //---

 protected:
  const CQChartsPivotPlot* plot_    { nullptr }; //!< parent plot
  QModelIndex              ind_;                 //!< model index
  QString                  name_;                //!< name
  double                   value_   { 0.0 };     //!< value
  double                   hnorm_   { 0.0 };     //!< value normalized to horizontal value range
  double                   vnorm_   { 0.0 };     //!< value normalized to vertical value range
  bool                     valid_   { false };   //!< is valid
  CQColorsPalette*         palette_ { nullptr }; //!< background palette
  CQChartsColor            color_;               //!< background color
};

//---

#include <CQChartsKey.h>

/*!
 * \brief Pivot Plot Key Color Box
 */
class CQChartsPivotKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsPivotKeyColor(CQChartsPivotPlot *plot, const ColorInd &ic);
};

/*!
 * \brief Pivot Plot Key Text
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
 */
class CQChartsPivotPlot : public CQChartsPlot,
 public CQChartsObjBarShapeData<CQChartsPivotPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumns xColumns    READ xColumns     WRITE setXColumns   )
  Q_PROPERTY(CQChartsColumns yColumns    READ yColumns     WRITE setYColumns   )
  Q_PROPERTY(CQChartsColumn  valueColumn READ valueColumn  WRITE setValueColumn)
  Q_PROPERTY(PlotType        plotType    READ plotType     WRITE setPlotType   )
  Q_PROPERTY(ValueType       valueType   READ valueType    WRITE setValueType  )
  Q_PROPERTY(bool            horizontal  READ isHorizontal WRITE setHorizontal )

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

  //--

  CQPivotModel *pivotModel() const { return pivotModel_; }

  //---

  void updatePivot();

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  CQChartsAxis *mappedXAxis() const;
  CQChartsAxis *mappedYAxis() const;

  //---

  void addKeyItems(CQChartsPlotKey *key) override;

  bool addMenuItems(QMenu *menu) override;

  //---

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
  CQChartsColumns xColumns_;                      //!< x columns
  CQChartsColumns yColumns_;                      //!< y columns
  CQChartsColumn  valueColumn_;                   //!< value columns
  PlotType        plotType_   { PlotType::BAR };  //!< plot type
  ValueType       valueType_  { ValueType::SUM }; //!< value type
  bool            horizontal_ { false };          //!< horizontal
  CQPivotModel*   pivotModel_ { nullptr };        //!< pivot model
};

#endif
