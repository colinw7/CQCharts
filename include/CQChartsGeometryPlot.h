#ifndef CQChartsGeometryPlot_H
#define CQChartsGeometryPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsDataLabel.h>

class CQChartsGeometryPlot;
class CQChartsBoxObj;

class CQChartsGeometryObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double        value READ value WRITE setValue)
  Q_PROPERTY(QString       name  READ name  WRITE setName )
  Q_PROPERTY(CQChartsColor color READ color WRITE setColor)

 public:
  using Polygons = std::vector<QPolygonF>;

 public:
  CQChartsGeometryObj(CQChartsGeometryPlot *plot, const CQChartsGeom::BBox &rect,
                      const Polygons &polygons, const QModelIndex &ind, int i, int n);

  QString calcId() const override;

  QString calcTipId() const override;

  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

  const QString &name() const { return name_; }
  void setName(const QString &v) { name_ = v; }

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &v) { color_ = v; }

  bool inside(const CQChartsGeom::Point &p) const override;

  void addSelectIndex() override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsGeometryPlot *plot_  { nullptr }; // parent plot
  Polygons              polygons_;          // geometry polygons
  double                value_ { 0.0 };     // geometry value
  QString               name_;              // geometry name
  CQChartsColor         color_;             // optional color
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

  Q_PROPERTY(CQChartsColumn nameColumn     READ nameColumn     WRITE setNameColumn    )
  Q_PROPERTY(CQChartsColumn geometryColumn READ geometryColumn WRITE setGeometryColumn)
  Q_PROPERTY(CQChartsColumn valueColumn    READ valueColumn    WRITE setValueColumn   )
  Q_PROPERTY(CQChartsColumn colorColumn    READ colorColumn    WRITE setColorColumn   )
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
    CQChartsColor      color;
    CQChartsGeom::BBox bbox;
    QModelIndex        ind;
  };

 public:
  CQChartsGeometryPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsGeometryPlot();

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c) { nameColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &geometryColumn() const { return geometryColumn_; }
  void setGeometryColumn(const CQChartsColumn &c) { geometryColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c) { valueColumn_ = c; updateRangeAndObjs(); }

  const CQChartsColumn &colorColumn() const { return colorColumn_; }
  void setColorColumn(const CQChartsColumn &c) { colorColumn_ = c; updateRangeAndObjs(); }

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

  //bool decodePoint(const QString &pointStr, QPointF &point, QString &poimtStr1);

 private:
  using Geometries = std::vector<Geometry>;

  CQChartsColumn    nameColumn_     { 0 };   // name column
  CQChartsColumn    geometryColumn_ { 1 };   // geometry column
  CQChartsColumn    valueColumn_;            // value column
  CQChartsColumn    colorColumn_;            // color column
  ColumnType        geometryColumnType_;     // geometry column type
  ColumnType        colorColumnType_;        // color column type
  Geometries        geometries_;             // geometry shapes
  double            minValue_       { 0.0 }; // min value
  double            maxValue_       { 0.0 }; // max value
  CQChartsShapeData shapeData_;              // polygon fill/border object
  CQChartsDataLabel dataLabel_;              // data label style
};

#endif
