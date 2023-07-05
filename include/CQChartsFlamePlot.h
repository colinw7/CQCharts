#ifndef CQChartsFlamePlot_H
#define CQChartsFlamePlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

//---

/*!
 * \brief Flame plot type
 * \ingroup Charts
 */
class CQChartsFlamePlotType : public CQChartsGroupPlotType {
 public:
  CQChartsFlamePlotType();

  QString name() const override { return "flame"; }
  QString desc() const override { return "Flame"; }

  Category category() const override { return Category::TWO_D; }

  void addParameters() override;

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; } // TODO

  bool canEqualScale() const override { return false; }

  QString description() const override;

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsFlamePlot;

//---

/*!
 * \brief Flame Chart Bar object
 * \ingroup Charts
 */
class CQChartsFlameBarObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using FlamePlot = CQChartsFlamePlot;
  using OptReal   = std::optional<double>;

 public:
  CQChartsFlameBarObj(const FlamePlot *plot, const BBox &rect, const QModelIndex &ind,
                      const ColorInd &ig, const ColorInd &iv);

 ~CQChartsFlameBarObj() = default;

  //---

  const FlamePlot *flamePlot() const { return flamePlot_; }

  //---

  QString typeName() const override { return "bar"; }

  //---

  const QString &name() const { return name_; }
  void setName(const QString &name) { name_ = name; }

  const OptReal &value() const { return value_; }
  void setValue(const OptReal &r) { value_ = r; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const override;

 private:
  const FlamePlot* flamePlot_ { nullptr }; //!< parent plot
  QString          name_;
  OptReal          value_;
};

//---

/*!
 * \brief Flame Plot
 * \ingroup Charts
 */
class CQChartsFlamePlot : public CQChartsGroupPlot,
 public CQChartsObjShapeData<CQChartsFlamePlot>,
 public CQChartsObjTextData <CQChartsFlamePlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn indColumn   READ indColumn   WRITE setIndColumn  )
  Q_PROPERTY(CQChartsColumn pathColumn  READ pathColumn  WRITE setPathColumn )
  Q_PROPERTY(CQChartsColumn startColumn READ startColumn WRITE setStartColumn)
  Q_PROPERTY(CQChartsColumn endColumn   READ endColumn   WRITE setEndColumn  )
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)

  // bar shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // bar text
  CQCHARTS_TEXT_DATA_PROPERTIES

 public:
  using Color    = CQChartsColor;
  using Alpha    = CQChartsAlpha;
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsFlamePlot(View *view, const ModelP &model);
 ~CQChartsFlamePlot();

  //---

  void init() override;
  void term() override;

  //---

  const Column &indColumn() const { return indColumn_; }
  void setIndColumn(const Column &c);

  const Column &pathColumn() const { return pathColumn_; }
  void setPathColumn(const Column &c);

  const Column &startColumn() const { return startColumn_; }
  void setStartColumn(const Column &c);

  const Column &endColumn() const { return endColumn_; }
  void setEndColumn(const Column &c);

  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &c);

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  //---

  void addProperties() override;

  Range calcRange() const override;

  void postCalcRange() override;

  //---

  bool createObjs(PlotObjs &objs) const override;

  //---

  virtual CQChartsFlameBarObj *createBarObj(const BBox &rect, const QModelIndex &ind,
                                            const ColorInd &ig, const ColorInd &iv) const;

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  // columns
  Column indColumn_;   //!< start column
  Column pathColumn_;  //!< start column
  Column startColumn_; //!< start column
  Column endColumn_;   //!< end column
  Column nameColumn_;  //!< name column
  Column valueColumn_; //!< value column

  mutable int maxDepth_ { 0 };
};

//---

#include <CQChartsGroupPlotCustomControls.h>

/*!
 * \brief Flame Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsFlamePlotCustomControls : public CQChartsGroupPlotCustomControls {
  Q_OBJECT

 public:
  using FlamePlot = CQChartsFlamePlot;

 public:
  CQChartsFlamePlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(Plot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void addWidgets() override;

  void addOptionsWidgets() override;

  void connectSlots(bool b) override;

 protected:
  FlamePlot* flamePlot_ { nullptr };
};

#endif
