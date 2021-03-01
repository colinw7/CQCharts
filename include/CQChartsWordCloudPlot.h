#ifndef CQChartsWordCloudPlot_H
#define CQChartsWordCloudPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>

//---

/*!
 * \brief Word cloud plot type
 * \ingroup Charts
 */
class CQChartsWordCloudPlotType : public CQChartsPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsWordCloudPlotType();

  QString name() const override { return "wordCloud"; }
  QString desc() const override { return "WordCloud"; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

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

// TODO:
//  Stacked
//  mouse feedback depend on angle (actual value)
//  axes on spokes

class CQChartsWordCloudPlot;

/*!
 * \brief Word Cloud Plot object
 * \ingroup Charts
 */
class CQChartsWordObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString name READ name)

 public:
  using WordCloudPlot = CQChartsWordCloudPlot;

 public:
  CQChartsWordObj(const WordCloudPlot *plot, const BBox &rect, const QString &name,
                  const QModelIndex &ind, const ColorInd &iv);

  QString typeName() const override { return "word"; }

  const QString &name() const { return name_; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

 private:
  const WordCloudPlot* plot_ { nullptr }; //!< parent plot
  QString              name_;             //!< row name
};

//---

/*!
 * \brief Word Count Plot
 * \ingroup Charts
 */
class CQChartsWordCloudPlot : public CQChartsPlot,
 public CQChartsObjTextData<CQChartsWordCloudPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn valueColumn READ valueColumn WRITE setValueColumn)
  Q_PROPERTY(CQChartsColumn countColumn READ countColumn WRITE setCountColumn)

  // text
  CQCHARTS_TEXT_DATA_PROPERTIES

 public:
  using Color    = CQChartsColor;
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsWordCloudPlot(View *view, const ModelP &model);
 ~CQChartsWordCloudPlot();

  //---

  void init() override;
  void term() override;

  //---

  const Column &valueColumn() const { return valueColumn_; }
  void setValueColumn(const Column &c);

  const Column &countColumn() const { return countColumn_; }
  void setCountColumn(const Column &c);

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  //---

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  using WordObj = CQChartsWordObj;

  virtual WordObj *createObj(const BBox &rect, const QString &name, const QModelIndex &ind,
                             const ColorInd &iv);

 private:
  bool addRow(const ModelVisitor::VisitData &data, int nr, PlotObjs &objs) const;

  bool columnValue(const ModelIndex &ind, double &value) const;

 protected:
  CQChartsPlotCustomControls *createCustomControls(CQCharts *charts) override;

 private:
  Column valueColumn_; //!< value column
  Column countColumn_; //!< count column
};

//---

class CQChartsWordCloudPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsWordCloudPlotCustomControls(CQCharts *charts);

  void setPlot(CQChartsPlot *plot) override;

 private:
  void connectSlots(bool b);

 public slots:
  void updateWidgets() override;

 private:
  CQChartsWordCloudPlot* plot_ { nullptr };
};

#endif
