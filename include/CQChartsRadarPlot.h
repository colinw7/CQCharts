#ifndef CQChartsRadarPlot_H
#define CQChartsRadarPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>

//---

/*!
 * \brief Radar plot type
 * \ingroup Charts
 */
class CQChartsRadarPlotType : public CQChartsPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsRadarPlotType();

  QString name() const override { return "radar"; }
  QString desc() const override { return "Radar"; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

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

// TODO:
//  Stacked
//  mouse feedback depend on angle (actual value)
//  axes on spokes

class CQChartsRadarPlot;

/*!
 * \brief Radar Plot object
 * \ingroup Charts
 */
class CQChartsRadarObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString name READ name)

 public:
  using RadarPlot  = CQChartsRadarPlot;
  using NameValues = std::map<QString, double>;

 public:
  CQChartsRadarObj(const RadarPlot *plot, const BBox &rect, const QString &name,
                   const Polygon &poly, const NameValues &nameValues, const QModelIndex &ind,
                   const ColorInd &iv);

  QString typeName() const override { return "poly"; }

  const QString &name() const { return name_; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isPolygon() const override { return true; }
  Polygon polygon() const override { return poly_; }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  bool inside(const Point &p) const override;

  BBox extraFitBBox() const;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

 private:
  const RadarPlot* plot_ { nullptr }; //!< parent plot
  QString          name_;             //!< row name
  Polygon          poly_;             //!< polygon
  NameValues       nameValues_;       //!< column values
};

//---

/*!
 * \brief Radar Plot
 * \ingroup Charts
 */
class CQChartsRadarPlot : public CQChartsPlot,
 public CQChartsObjShapeData   <CQChartsRadarPlot>,
 public CQChartsObjTextData    <CQChartsRadarPlot>,
 public CQChartsObjGridLineData<CQChartsRadarPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn  nameColumn   READ nameColumn   WRITE setNameColumn  )
  Q_PROPERTY(CQChartsColumns valueColumns READ valueColumns WRITE setValueColumns)

  // options
  Q_PROPERTY(CQChartsAngle angleStart  READ angleStart  WRITE setAngleStart )
  Q_PROPERTY(CQChartsAngle angleExtent READ angleExtent WRITE setAngleExtent)

  // grid
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Grid, grid)

  // shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // text
  CQCHARTS_TEXT_DATA_PROPERTIES

 public:
  using Angle    = CQChartsAngle;
  using Color    = CQChartsColor;
  using Alpha    = CQChartsAlpha;
  using ColorInd = CQChartsUtil::ColorInd;
  using PenBrush = CQChartsPenBrush;

 public:
  CQChartsRadarPlot(View *view, const ModelP &model);
 ~CQChartsRadarPlot();

  //---

  void init() override;
  void term() override;

  //---

  const Column &nameColumn() const { return nameColumn_; }
  void setNameColumn(const Column &c);

  //---

  const Columns &valueColumns() const { return valueColumns_; }
  void setValueColumns(const Columns &c);

  //---

  const Angle &angleStart() const { return angleStart_; }
  void setAngleStart(const Angle &a);

  const Angle &angleExtent() const { return angleExtent_; }
  void setAngleExtent(const Angle &a);

  //----

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  BBox calcExtraFitBBox() const override;

  void addKeyItems(PlotKey *key) override;

  //---

  void postResize() override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(PaintDevice *device) const override;

  //---

  using RadarObj = CQChartsRadarObj;

  virtual RadarObj *createObj(const BBox &rect, const QString &name, const Polygon &poly,
                              const RadarObj::NameValues &nameValues, const QModelIndex &ind,
                              const ColorInd &iv);

 private:
  bool addRow(const ModelVisitor::VisitData &data, int nr, PlotObjs &objs) const;

  bool columnValue(const ModelIndex &ind, double &value) const;

  Qt::Alignment alignForPosition(double x, double y) const;

 private:
  class ValueData {
   public:
    ValueData() { }

    void add(double v) {
      if (values_.empty()) {
        min_ = v;
        max_ = v;
        sum_ = v;
      }
      else {
        min_  = std::min(min_, v);
        max_  = std::max(max_, v);
        sum_ += v;
      }

      values_.push_back(v);
    }

    double min() const { return min_; }
    double max() const { return max_; }
    double sum() const { return sum_; }

   private:
    using Values = std::vector<double>;

    Values values_;
    double min_ { 0.0 };
    double max_ { 0.0 };
    double sum_ { 0.0 };
  };

  using ValueDatas = std::map<int, ValueData>;

  Column     nameColumn_;             //!< name column
  Columns    valueColumns_;           //!< value columns
  Angle      angleStart_   { 90.0 };  //!< angle start
  Angle      angleExtent_  { 360.0 }; //!< angle extent
  ValueDatas valueDatas_;             //!< value
  double     valueRadius_  { 1.0 };   //!< max value (radius)
};

#endif
