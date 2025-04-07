#ifndef CQChartsTernaryPlot_H
#define CQChartsTernaryPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>

//---

/*!
 * \brief Ternary plot type
 * \ingroup Charts
 */
class CQChartsTernaryPlotType : public CQChartsPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsTernaryPlotType();

  QString name() const override { return "ternary"; }
  QString desc() const override { return "Ternary"; }

  Category category() const override { return Category::TWO_D; }

  void addParameters() override;

  bool hasAxes() const override { return false; } // own axes

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; } // ??

  bool canEqualScale() const override { return false; }

  QString description() const override;

  //---

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsTernaryPlot;

/*!
 * \brief Ternary Point object
 * \ingroup Charts
 */
class CQChartsTernaryPointObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString name READ name)

 public:
  using TernaryPlot = CQChartsTernaryPlot;
  using Symbol      = CQChartsSymbol;
  using Colot       = CQChartsColor;

 public:
  CQChartsTernaryPointObj(const TernaryPlot *plot, const BBox &rect,
                          double x, double y, double z, const QModelIndex &ind,
                          const ColorInd &ig, const ColorInd &iv);

  QString typeName() const override { return "point"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  const QString &name() const { return name_; }
  void setName(const QString &n) { name_ = n; }

  const Color &color() const { return color_; }
  void setColor(const Color &c) { color_ = c; }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  Point mapPoint() const;

 private:
  const TernaryPlot* ternaryPlot_ { nullptr }; //!< parent plot
  QString            name_;                    //!< row name
  double             x_ { 0.0 };               //!< x
  double             y_ { 0.0 };               //!< y
  double             z_ { 0.0 };               //!< z
  Color              color_;
};

//---

/*!
 * \brief Ternary Plot
 * \ingroup Charts
 */
class CQChartsTernaryPlot : public CQChartsPlot,
 public CQChartsObjPointData<CQChartsTernaryPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn xColumn    READ xColumn    WRITE setXColumn   )
  Q_PROPERTY(CQChartsColumn yColumn    READ yColumn    WRITE setYColumn   )
  Q_PROPERTY(CQChartsColumn zColumn    READ zColumn    WRITE setZColumn   )
  Q_PROPERTY(CQChartsColumn nameColumn READ nameColumn WRITE setNameColumn)

  // point data
  CQCHARTS_POINT_DATA_PROPERTIES

 public:
  using Color    = CQChartsColor;
  using Alpha    = CQChartsAlpha;
  using Symbol   = CQChartsSymbol;
  using Length   = CQChartsLength;
  using PenBrush = CQChartsPenBrush;
  using PenData  = CQChartsPenData;
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsTernaryPlot(View *view, const ModelP &model);
 ~CQChartsTernaryPlot();

  //---

  void init() override;
  void term() override;

  //---

  const Column &xColumn() const { return xColumn_; }
  void setXColumn(const Column &c);

  const Column &yColumn() const { return yColumn_; }
  void setYColumn(const Column &c);

  const Column &zColumn() const { return zColumn_; }
  void setZColumn(const Column &c);

  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  //---

  const QString &xLabel() const { return xLabel_; }
  const QString &yLabel() const { return yLabel_; }
  const QString &zLabel() const { return zLabel_; }

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  //---

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  Point mapPoint(double x, double y, double z) const;

  //---

  bool hasFgAxes() const override;

  void drawFgAxes(PaintDevice *device) const override;

  //---

  void addKeyItems(PlotKey *key) override;

  //---

  bool addMenuItems(QMenu *menu, const Point &p) override;

  //---

  using PointObj = CQChartsTernaryPointObj;

  virtual PointObj *createPointObj(const BBox &rect, double x, double y, double z,
                                   const QModelIndex &ind, const ColorInd &ig,
                                   const ColorInd &iv);

  //---

  const RMinMax &xrange() const { return xrange_; }
  const RMinMax &yrange() const { return yrange_; }
  const RMinMax &zrange() const { return zrange_; }

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  Column xColumn_;    //!< x column
  Column yColumn_;    //!< y column
  Column zColumn_;    //!< z column
  Column nameColumn_; //!< name column

  QString xLabel_;
  QString yLabel_;
  QString zLabel_;

  RMinMax xrange_;
  RMinMax yrange_;
  RMinMax zrange_;

  double height_ { 1.0 };

  AxisP xAxis_; //!< x axis
  AxisP yAxis_; //!< y axis
  AxisP zAxis_; //!< z axis
};

//---

#include <CQChartsPlotCustomControls.h>

/*!
 * \brief Ternary Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsTernaryPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsTernaryPlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(Plot *plot) override;

 public Q_SLOTS:
  void updateWidgets() override;

 protected:
  void addWidgets() override;

  void addColumnWidgets() override;

  void connectSlots(bool b) override;

 protected:
  CQChartsTernaryPlot* ternaryPlot_ { nullptr };
};

#endif
