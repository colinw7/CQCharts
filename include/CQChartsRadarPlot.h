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

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
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
  using NameValues = std::map<QString,double>;

 public:
  CQChartsRadarObj(const CQChartsRadarPlot *plot, const CQChartsGeom::BBox &rect,
                   const QString &name, const QPolygonF &poly, const NameValues &nameValues,
                   const QModelIndex &ind, const ColorInd &iv);

  QString typeName() const override { return "poly"; }

  const QString &name() const { return name_; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isPolygon() const override { return true; }
  QPolygonF polygon() const override { return poly_; }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  CQChartsGeom::BBox annotationBBox() const;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  const CQChartsRadarPlot* plot_       { nullptr }; //!< parent plot
  QString                  name_;                   //!< row name
  QPolygonF                poly_;                   //!< polygon
  NameValues               nameValues_;             //!< column values
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
  Q_PROPERTY(double angleStart  READ angleStart  WRITE setAngleStart )
  Q_PROPERTY(double angleExtent READ angleExtent WRITE setAngleExtent)

  // grid
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Grid,grid)

  // shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // text
  CQCHARTS_TEXT_DATA_PROPERTIES

 public:
  CQChartsRadarPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsRadarPlot();

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  //---

  const CQChartsColumns &valueColumns() const { return valueColumns_; }
  void setValueColumns(const CQChartsColumns &c);

  //---

  double angleStart() const { return angleStart_; }
  void setAngleStart(double r);

  double angleExtent() const { return angleExtent_; }
  void setAngleExtent(double r);

  //----

  void addProperties() override;

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  CQChartsGeom::BBox annotationBBox() const override;

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  void postResize() override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(CQChartsPaintDevice *device) const override;

 private:
  void addRow(const ModelVisitor::VisitData &data, int nr, PlotObjs &objs) const;

  bool columnValue(const CQChartsModelIndex &ind, double &value) const;

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

  using ValueDatas = std::map<int,ValueData>;

  CQChartsColumn  nameColumn_;             //!< name column
  CQChartsColumns valueColumns_;           //!< value columns
  double          angleStart_   { 90.0 };  //!< angle start
  double          angleExtent_  { 360.0 }; //!< angle extent
  ValueDatas      valueDatas_;             //!< value
  double          valueRadius_  { 1.0 };   //!< max value (radius)
};

#endif
