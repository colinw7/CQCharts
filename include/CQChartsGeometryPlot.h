#ifndef CQChartsGeometryPlot_H
#define CQChartsGeometryPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQUtil.h>

class CQChartsGeometryPlot;

class CQChartsGeometryObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  typedef std::vector<QPolygonF> Polygons;

 public:
  CQChartsGeometryObj(CQChartsGeometryPlot *plot, const CBBox2D &rect,
                      const Polygons &polygons, double value, int ind, int n);

  bool inside(const CPoint2D &p) const override;

  void handleResize() override;

  void draw(QPainter *p) override;

 private:
  CQChartsGeometryPlot *plot_  { nullptr };
  Polygons              polygons_;
  double                value_ { 0.0 };
  int                   ind_   { -1 };
  int                   n_     { -1 };
  Polygons              ppolygons_;
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
  Q_PROPERTY(QColor lineColor      READ lineColor      WRITE setLineColor     )
  Q_PROPERTY(QColor fillColor      READ fillColor      WRITE setFillColor     )

 public:
  typedef std::vector<QPolygonF> Polygons;

  struct Geometry {
    QString  name;
    Polygons polygons;
    double   value { 0.0 };
    CBBox2D  bbox  { 0, 0, 1, 1 };
  };

 public:
  CQChartsGeometryPlot(CQChartsView *view, QAbstractItemModel *model);

  const char *typeName() const override { return "Geometry"; }

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  int geometryColumn() const { return geometryColumn_; }
  void setGeometryColumn(int i) { geometryColumn_ = i; update(); }

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; update(); }

  double minValue() const { return minValue_; }
  double maxValue() const { return maxValue_; }

  const QColor &lineColor() const { return lineColor_; }
  void setLineColor(const QColor &v) { lineColor_ = v; }

  const QColor &fillColor() const { return fillColor_; }
  void setFillColor(const QColor &v) { fillColor_ = v; }

  void addProperties();

  void updateRange();

  void initObjs();

  void draw(QPainter *) override;

 private:
  bool decodeGeometry(const QString &geomStr, Polygons &polygons);

  bool decodePolygons(const QString &polysStr, Polygons &poly);
  bool decodePolygon(const QString &polyStr, QPolygonF &poly);
  bool decodePoint(const QString &pointStr, QPointF &point);

 private:
  typedef std::vector<Geometry> Geometries;

  int        nameColumn_     { 0 };
  int        geometryColumn_ { 1 };
  int        valueColumn_    { -1 };
  Geometries geometries_;
  double     minValue_       { 0.0 };
  double     maxValue_       { 0.0 };
  QColor     lineColor_      { 0, 0, 0 };
  QColor     fillColor_      { 0, 0, 0, 0 };
};

#endif
