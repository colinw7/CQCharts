#ifndef CQChartsVennPlot_H
#define CQChartsVennPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>

//---

/*!
 * \brief Venn diagram plot type
 * \ingroup Charts
 */
class CQChartsVennPlotType : public CQChartsPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsVennPlotType();

  QString name() const override { return "venn"; }
  QString desc() const override { return "Venn"; }

  Category category() const override { return Category::ONE_D; }

  void addParameters() override;

  bool hasAxes() const override { return false; }
  bool hasKey () const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  bool canEqualScale() const override { return true; }

  QString description() const override;

  //---

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

struct CQChartsPathData {
  using OptReal = CQChartsOptReal;
  using Inds    = std::vector<int>;

  QPolygonF    poly;
  QStringList  names;
  OptReal      value;
  Inds         inds;
  int          ind   { -1 };
  int          level { -1 };

  CQChartsPathData(const QPolygonF &poly) :
   poly(poly) {
  }
};

//---

// TODO:
//  Stacked
//  mouse feedback depend on angle (actual value)
//  axes on spokes

class CQChartsVennPlot;

/*!
 * \brief Venn Diagram Plot object
 * \ingroup Charts
 */
class CQChartsCircleObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using VennPlot = CQChartsVennPlot;
  using Angle    = CQChartsAngle;

 public:
  CQChartsCircleObj(const VennPlot *plot, const BBox &rect, const CQChartsPathData &pathData);

  QString typeName() const override { return "circle"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  void calcTextPenBrush(PenBrush &penBrush, bool updateState) const;

 private:
  const VennPlot*  vennPlot_ { nullptr }; //!< parent plot
  CQChartsPathData pathData_;             //!< path data
};

//---

/*!
 * \brief Venn Diagram Plot
 * \ingroup Charts
 */
class CQChartsVennPlot : public CQChartsPlot,
 public CQChartsObjShapeData<CQChartsVennPlot>,
 public CQChartsObjTextData <CQChartsVennPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn nameColumn  READ nameColumn  WRITE setNameColumn)
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)

  // shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // text
  CQCHARTS_TEXT_DATA_PROPERTIES

  Q_PROPERTY(CQChartsAngle startAngle READ startAngle WRITE setStartAngle)

  Q_PROPERTY(LabelType level1LabelType READ level1LabelType WRITE setLevel1LabelType)
  Q_PROPERTY(LabelType level2LabelType READ level2LabelType WRITE setLevel2LabelType)
  Q_PROPERTY(LabelType level3LabelType READ level3LabelType WRITE setLevel3LabelType)

  Q_ENUMS(LabelType)

 public:
  enum class LabelType {
    NAME,
    VALUE,
    NAME_VALUE
  };

  using Color    = CQChartsColor;
  using Alpha    = CQChartsAlpha;
  using Angle    = CQChartsAngle;
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsVennPlot(View *view, const ModelP &model);
 ~CQChartsVennPlot();

  //---

  void init() override;
  void term() override;

  //---

  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &c);

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  //---

  const Angle &startAngle() const { return startAngle_; }
  void setStartAngle(const Angle &a);

  //---

  const LabelType &level1LabelType() const { return level1LabelType_; }
  void setLevel1LabelType(const LabelType &t);

  const LabelType &level2LabelType() const { return level2LabelType_; }
  void setLevel2LabelType(const LabelType &t);

  const LabelType &level3LabelType() const { return level3LabelType_; }
  void setLevel3LabelType(const LabelType &t);

  //---

  int numNames() const { return n_; }

  //---

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *) const override;

  //---

  bool addMenuItems(QMenu *menu, const Point &p) override;

  //---

  using CircleObj = CQChartsCircleObj;

  virtual CircleObj *createCircleObj(const BBox &rect, const CQChartsPathData &pathData);

 private:
  bool addRow(const ModelVisitor::VisitData &data, int nr, PlotObjs &objs) const;

  bool columnValue(const ModelIndex &ind, double &value) const;

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  Column nameColumn_;  //!< name column
  Column valueColumn_; //!< value column

  Angle startAngle_ { Angle::degrees(90) };

  LabelType level1LabelType_ { LabelType::NAME_VALUE };
  LabelType level2LabelType_ { LabelType::VALUE };
  LabelType level3LabelType_ { LabelType::VALUE };

  int n_ { 0 };
};

//---

#include <CQChartsPlotCustomControls.h>

/*!
 * \brief Venn Diagram Cloud Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsVennPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsVennPlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(Plot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void addWidgets() override;

  void addColumnWidgets() override;

  void connectSlots(bool b) override;

  //---

  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 protected:
  CQChartsVennPlot* vennPlot_ { nullptr };
};

#endif
