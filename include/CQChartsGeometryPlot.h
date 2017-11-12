#ifndef CQChartsGeometryPlot_H
#define CQChartsGeometryPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>

class CQChartsGeometryPlot;

class CQChartsGeometryObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Polygons = std::vector<QPolygonF>;

 public:
  CQChartsGeometryObj(CQChartsGeometryPlot *plot, const CQChartsGeom::BBox &rect,
                      const Polygons &polygons, double value, const QString &name,
                      const QModelIndex &ind, int i, int n);

  bool inside(const CQChartsGeom::Point &p) const override;

  void mousePress(const CQChartsGeom::Point &) override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsGeometryPlot *plot_  { nullptr };
  Polygons              polygons_;
  double                value_ { 0.0 };
  QString               name_;
  QModelIndex           ind_;
  int                   i_     { -1 };
  int                   n_     { -1 };
  Polygons              ppolygons_;
};

//---

class CQChartsGeometryPlotType : public CQChartsPlotType {
 public:
  CQChartsGeometryPlotType();

  QString name() const override { return "geometry"; }
  QString desc() const override { return "Geometry"; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsGeometryPlot : public CQChartsPlot {
  Q_OBJECT

  // properties
  //  line
  //   display, stroke
  //  fill
  //   display, brush

  Q_PROPERTY(int    nameColumn     READ nameColumn     WRITE setNameColumn    )
  Q_PROPERTY(int    geometryColumn READ geometryColumn WRITE setGeometryColumn)
  Q_PROPERTY(int    valueColumn    READ valueColumn    WRITE setValueColumn   )
  Q_PROPERTY(double minValue       READ minValue       WRITE setMinValue      )
  Q_PROPERTY(double maxValue       READ maxValue       WRITE setMaxValue      )
  Q_PROPERTY(QColor lineColor      READ lineColor      WRITE setLineColor     )

 public:
  using Polygons = std::vector<QPolygonF>;

  struct Geometry {
    QString            name;
    Polygons           polygons;
    double             value { 0.0 };
    CQChartsGeom::BBox bbox;
    QModelIndex        ind;
  };

 public:
  CQChartsGeometryPlot(CQChartsView *view, const ModelP &model);

  //---

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  int geometryColumn() const { return geometryColumn_; }
  void setGeometryColumn(int i) { geometryColumn_ = i; update(); }

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; update(); }

  double minValue() const { return minValue_; }
  void setMinValue(double r) { minValue_ = r; update(); }

  double maxValue() const { return maxValue_; }
  void setMaxValue(double r) { maxValue_ = r; update(); }

  const QColor &lineColor() const { return lineColor_; }
  void setLineColor(const QColor &c) { lineColor_ = c; update(); }

  //---

  const CQChartsDataLabel &dataLabel() const { return dataLabel_; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void initObjs() override;

  //---

  void draw(QPainter *) override;

  void drawDataLabel(QPainter *p, const QRectF &qrect, const QString &str);

 private:
  bool decodeGeometry(const QString &geomStr, Polygons &polygons);

  bool decodePolygons(const QString &polysStr, Polygons &poly);
  bool decodePolygon(const QString &polyStr, QPolygonF &poly);
  bool decodePoint(const QString &pointStr, QPointF &point);

 private:
  using Geometries = std::vector<Geometry>;

  int               nameColumn_     { 0 };
  int               geometryColumn_ { 1 };
  int               valueColumn_    { -1 };
  Geometries        geometries_;
  double            minValue_       { 0.0 };
  double            maxValue_       { 0.0 };
  QColor            lineColor_      { 0, 0, 0 };
  CQChartsDataLabel dataLabel_;
};

#endif
