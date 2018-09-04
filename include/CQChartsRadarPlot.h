#ifndef CQChartsRadarPlot_H
#define CQChartsRadarPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>

//---

class CQChartsRadarPlotType : public CQChartsPlotType {
 public:
  CQChartsRadarPlotType();

  QString name() const override { return "radar"; }
  QString desc() const override { return "Radar"; }

  Dimension dimension() const override { return Dimension::ONE_D; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

// TODO:
//  Stacked
//  mouse feedback depend on angle (actual value)
//  axes on spokes

class CQChartsRadarPlot;

class CQChartsRadarObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using NameValues = std::map<QString,double>;

 public:
  CQChartsRadarObj(CQChartsRadarPlot *plot, const CQChartsGeom::BBox &rect, const QString &name,
                   const QPolygonF &poly, const NameValues &nameValues, const QModelIndex &ind,
                   int i, int n);

  QString calcId() const override;

  QString calcTipId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  CQChartsGeom::BBox annotationBBox() const;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

 private:
  CQChartsRadarPlot *plot_       { nullptr }; // parent plot
  QString            name_;                   // row name
  QPolygonF          poly_;                   // polygon
  NameValues         nameValues_;             // column values
  QModelIndex        ind_;                    // data index
  int                i_          { 0 };       // value ind
  int                n_          { 1 };       // value count
};

//---

class CQChartsRadarPlot : public CQChartsPlot,
 public CQChartsPlotShapeData   <CQChartsRadarPlot>,
 public CQChartsPlotTextData    <CQChartsRadarPlot>,
 public CQChartsPlotGridLineData<CQChartsRadarPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn nameColumn   READ nameColumn      WRITE setNameColumn     )
  Q_PROPERTY(CQChartsColumn valueColumn  READ valueColumn     WRITE setValueColumn    )
  Q_PROPERTY(QString        valueColumns READ valueColumnsStr WRITE setValueColumnsStr)

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

  const CQChartsColumn &valueColumn() const { return valueColumns_.column(); }
  void setValueColumn(const CQChartsColumn &c);

  const Columns &valueColumns() const { return valueColumns_.columns(); }
  void setValueColumns(const Columns &valueColumns);

  QString valueColumnsStr() const;
  bool setValueColumnsStr(const QString &s);

  const CQChartsColumn &valueColumnAt(int i) const;

  int numValueColumns() const;

  //---

  double angleStart() const { return angleStart_; }
  void setAngleStart(double r);

  double angleExtent() const { return angleExtent_; }
  void setAngleExtent(double r);

  //----

  void addProperties() override;

  void calcRange() override;

  bool initObjs() override;

  CQChartsGeom::BBox annotationBBox() const override;

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  void handleResize() override;

  //---

  void drawBackground(QPainter *) override;

 private:
  void addRow(const ModelVisitor::VisitData &data, int nr);

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

  CQChartsColumn  nameColumn_;             // name column
  CQChartsColumns valueColumns_ { 1 };     // value columns
  double          angleStart_   { 90.0 };  // angle start
  double          angleExtent_  { 360.0 }; // angle extent
  ValueDatas      valueDatas_;             // value
  double          valueRadius_  { 1.0 };   // max value (radius)
};

#endif
