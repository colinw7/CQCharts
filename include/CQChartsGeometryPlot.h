#ifndef CQChartsGeometryPlot_H
#define CQChartsGeometryPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>

class CQChartsGeometryPlot;
class CQChartsBoxObj;

class CQChartsGeometryObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using Polygons = std::vector<QPolygonF>;

 public:
  CQChartsGeometryObj(CQChartsGeometryPlot *plot, const CQChartsGeom::BBox &rect,
                      const Polygons &polygons, double value, const QString &name,
                      const QModelIndex &ind, int i, int n);

  QString calcId() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsGeometryPlot *plot_  { nullptr }; // parent plot
  Polygons              polygons_;          // geometry polygons
  double                value_ { 0.0 };     // geometry value
  QString               name_;              // geometry name
  QModelIndex           ind_;               // model index
  int                   i_     { -1 };      // value index
  int                   n_     { -1 };      // value count
  Polygons              ppolygons_;         // pixel polygons
};

//---

class CQChartsGeometryPlotType : public CQChartsPlotType {
 public:
  CQChartsGeometryPlotType();

  QString name() const override { return "geometry"; }
  QString desc() const override { return "Geometry"; }

  void addParameters() override;

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

  Q_PROPERTY(int            nameColumn     READ nameColumn     WRITE setNameColumn    )
  Q_PROPERTY(int            geometryColumn READ geometryColumn WRITE setGeometryColumn)
  Q_PROPERTY(int            valueColumn    READ valueColumn    WRITE setValueColumn   )
  Q_PROPERTY(double         minValue       READ minValue       WRITE setMinValue      )
  Q_PROPERTY(double         maxValue       READ maxValue       WRITE setMaxValue      )
  Q_PROPERTY(bool           border         READ isBorder       WRITE setBorder        )
  Q_PROPERTY(CQChartsColor  borderColor    READ borderColor    WRITE setBorderColor   )
  Q_PROPERTY(double         borderAlpha    READ borderAlpha    WRITE setBorderAlpha   )
  Q_PROPERTY(CQChartsLength borderWidth    READ borderWidth    WRITE setBorderWidth   )
  Q_PROPERTY(bool           filled         READ isFilled       WRITE setFilled        )
  Q_PROPERTY(CQChartsColor  fillColor      READ fillColor      WRITE setFillColor     )
  Q_PROPERTY(double         fillAlpha      READ fillAlpha      WRITE setFillAlpha     )
  Q_PROPERTY(Pattern        fillPattern    READ fillPattern    WRITE setFillPattern   )

  Q_ENUMS(Pattern)

 public:
  enum class Pattern {
    SOLID,
    HATCH,
    DENSE,
    HORIZ,
    VERT,
    FDIAG,
    BDIAG
  };

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

 ~CQChartsGeometryPlot();

  //---

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; updateRangeAndObjs(); }

  int geometryColumn() const { return geometryColumn_; }
  void setGeometryColumn(int i) { geometryColumn_ = i; updateRangeAndObjs(); }

  int valueColumn() const { return valueColumn_; }
  void setValueColumn(int i) { valueColumn_ = i; updateRangeAndObjs(); }

  //---

  double minValue() const { return minValue_; }
  void setMinValue(double r) { minValue_ = r; update(); }

  double maxValue() const { return maxValue_; }
  void setMaxValue(double r) { maxValue_ = r; update(); }

  //---

  bool isBorder() const;
  void setBorder(bool b);

  const CQChartsColor &borderColor() const;
  void setBorderColor(const CQChartsColor &c);

  QColor interpBorderColor(int i, int n) const;

  double borderAlpha() const;
  void setBorderAlpha(double a);

  const CQChartsLength &borderWidth() const;
  void setBorderWidth(const CQChartsLength &l);

  //---

  bool isFilled() const;
  void setFilled(bool b);

  const CQChartsColor &fillColor() const;
  void setFillColor(const CQChartsColor &c);

  QColor interpFillColor(int i, int n) const;

  double fillAlpha() const;
  void setFillAlpha(double a);

  Pattern fillPattern() const;
  void setFillPattern(Pattern pattern);

  //---

  const CQChartsDataLabel &dataLabel() const { return dataLabel_; }
  CQChartsDataLabel &dataLabel() { return dataLabel_; }

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  bool initObjs() override;

  //---

  void draw(QPainter *) override;

 private:
  void addRow(QAbstractItemModel *model, const QModelIndex &parent, int row);

  bool decodeGeometry(const QString &geomStr, Polygons &polygons);

  bool decodePolygons(const QString &polysStr, Polygons &poly);
  bool decodePolygon(const QString &polyStr, QPolygonF &poly);
  bool decodePoint(const QString &pointStr, QPointF &point, QString &poimtStr1);

 private:
  using Geometries = std::vector<Geometry>;

  int               nameColumn_     { 0 };
  int               geometryColumn_ { 1 };
  int               valueColumn_    { -1 };
  Geometries        geometries_;
  double            minValue_       { 0.0 };
  double            maxValue_       { 0.0 };
  CQChartsBoxObj*   boxObj_         { nullptr }; // polygon fill/border object
  CQChartsDataLabel dataLabel_;
};

#endif
