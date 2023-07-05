#ifndef CQChartsGanttPlot_H
#define CQChartsGanttPlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>

//---

/*!
 * \brief Gantt plot type
 * \ingroup Charts
 */
class CQChartsGanttPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsGanttPlotType();

  QString name() const override { return "gantt"; }
  QString desc() const override { return "Gantt"; }

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

class CQChartsGanttPlot;

//---

/*!
 * \brief Gantt Chart Group object
 * \ingroup Charts
 */
class CQChartsGanttGroupObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using GanttPlot = CQChartsGanttPlot;
  using OptReal   = std::optional<double>;

 public:
  CQChartsGanttGroupObj(const GanttPlot *plot, const BBox &rect, const ColorInd &ig);

 ~CQChartsGanttGroupObj() = default;

  //---

  const GanttPlot *ganttPlot() const { return ganttPlot_; }

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
  const GanttPlot* ganttPlot_ { nullptr }; //!< parent plot
  QString          name_;
  OptReal          value_;
};

//---

/*!
 * \brief Gantt Chart Bar object
 * \ingroup Charts
 */
class CQChartsGanttBarObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using GanttPlot = CQChartsGanttPlot;
  using OptReal   = std::optional<double>;

 public:
  CQChartsGanttBarObj(const GanttPlot *plot, const BBox &rect, const QModelIndex &ind,
                      const ColorInd &ig, const ColorInd &iv);

 ~CQChartsGanttBarObj() = default;

  //---

  const GanttPlot *ganttPlot() const { return ganttPlot_; }

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
  const GanttPlot* ganttPlot_ { nullptr }; //!< parent plot
  QString          name_;
  OptReal          value_;
};

//---

/*!
 * \brief Gantt Plot
 * \ingroup Charts
 */
class CQChartsGanttPlot : public CQChartsGroupPlot,
 public CQChartsObjShapeData     <CQChartsGanttPlot>,
 public CQChartsObjTextData      <CQChartsGanttPlot>,
 public CQChartsObjGroupShapeData<CQChartsGanttPlot>,
 public CQChartsObjGroupTextData <CQChartsGanttPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn startColumn READ startColumn WRITE setStartColumn)
  Q_PROPERTY(CQChartsColumn endColumn   READ endColumn   WRITE setEndColumn  )
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn )
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)

  // group and bar shape
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Group, group)

  CQCHARTS_SHAPE_DATA_PROPERTIES

  // group and bar text
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(Group, group)

  CQCHARTS_TEXT_DATA_PROPERTIES

  Q_PROPERTY(bool groupStretch READ isGroupStretch WRITE setGroupStretch)

 public:
  using Color    = CQChartsColor;
  using Alpha    = CQChartsAlpha;
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsGanttPlot(View *view, const ModelP &model);
 ~CQChartsGanttPlot();

  //---

  void init() override;
  void term() override;

  //---

  const Column &startColumn() const { return startColumn_; }
  void setStartColumn(const Column &c);

  const Column &endColumn() const { return endColumn_; }
  void setEndColumn(const Column &c);

  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &c);

  //---

  bool isGroupStretch() const { return groupStretch_; }
  void setGroupStretch(bool b);

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

  virtual CQChartsGanttGroupObj *createGroupObj(const BBox &rect, const ColorInd &ig) const;

  virtual CQChartsGanttBarObj *createBarObj(const BBox &rect, const QModelIndex &ind,
                                            const ColorInd &ig, const ColorInd &iv) const;

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  // columns
  Column startColumn_; //!< start column
  Column endColumn_;   //!< end column
  Column nameColumn_;  //!< name column
  Column valueColumn_; //!< value column

  bool groupStretch_ { true };
};

//---

#include <CQChartsGroupPlotCustomControls.h>

/*!
 * \brief Gantt Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsGanttPlotCustomControls : public CQChartsGroupPlotCustomControls {
  Q_OBJECT

 public:
  using GanttPlot = CQChartsGanttPlot;

 public:
  CQChartsGanttPlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(Plot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void addWidgets() override;

  void addOptionsWidgets() override;

  void connectSlots(bool b) override;

 protected:
  GanttPlot* ganttPlot_ { nullptr };
};

#endif
