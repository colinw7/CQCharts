#ifndef CQChartsGeometryPlot_H
#define CQChartsGeometryPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsPath.h>
#include <CQChartsStyle.h>

class CQChartsDataLabel;

//---

struct CQChartsGeometryShape {
  enum class Type {
    NONE,
    POLYGON_LIST,
    POLYGON,
    RECT,
    PATH
  };

  CQChartsGeometryShape() = default;

  CQChartsGeometryShape(const QString &str);

  Type                   type { Type::NONE };
  CQChartsGeom::Polygons polygonList;
  CQChartsGeom::Polygon  polygon;
  CQChartsGeom::BBox     rect;
  CQChartsPath           path;
};

/*!
 * \brief Geometry plot type
 * \ingroup Charts
 */
class CQChartsGeometryPlotType : public CQChartsPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsGeometryPlotType();

  QString name() const override { return "geometry"; }
  QString desc() const override { return "Geometry"; }

  void addParameters() override;

  bool canProbe() const override { return true; }

  QString description() const override;

  //---

  bool isColumnForParameter(CQChartsModelColumnDetails *columnDetails,
                            CQChartsPlotParameter *parameter) const override;

  void analyzeModel(CQChartsModelData *modelData,
                    CQChartsAnalyzeModelData &analyzeModelData) override;

  //---

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsGeometryPlot;

/*!
 * \brief Geometry Plot object
 * \ingroup Charts
 */
class CQChartsGeometryObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double        value READ value WRITE setValue)
  Q_PROPERTY(QString       name  READ name  WRITE setName )
  Q_PROPERTY(CQChartsColor color READ color WRITE setColor)
  Q_PROPERTY(CQChartsStyle style READ style WRITE setStyle)

 public:
  CQChartsGeometryObj(const CQChartsGeometryPlot *plot, const CQChartsGeom::BBox &rect,
                      const CQChartsGeom::Polygons &polygons, const QModelIndex &ind,
                      const ColorInd &iv);

  QString typeName() const override { return "geom"; }

  QString calcId() const override;

  QString calcTipId() const override;

  bool isPolygon() const override { return true; }
  CQChartsGeom::Polygon polygon() const override { return polygons_[0]; }

  //---

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &c) { color_ = c; }

  const CQChartsStyle &style() const { return style_; }
  void setStyle(const CQChartsStyle &s) { style_ = s; }

  double value() const { return value_; }
  void setValue(double r) { value_ = r; hasValue_ = true; }

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  //---

  void draw(CQChartsPaintDevice *device) override;

  void drawFg(CQChartsPaintDevice *device) const override;

  //---

  void calcPenBrush(CQChartsPenBrush &penBrush, bool updateState) const;

  void writeScriptData(CQChartsScriptPainter *device) const override;

 private:

 private:
  const CQChartsGeometryPlot* plot_     { nullptr }; //!< parent plot
  CQChartsGeom::Polygons      polygons_;             //!< geometry polygons
  QString                     name_;                 //!< geometry name
  CQChartsColor               color_;                //!< optional color
  CQChartsStyle               style_;                //!< optional style
  double                      value_    { 0.0 };     //!< geometry value
  bool                        hasValue_ { false };   //!< has value
};

//---

/*!
 * \brief Geometry Plot
 * \ingroup Charts
 */
class CQChartsGeometryPlot : public CQChartsPlot,
 public CQChartsObjShapeData<CQChartsGeometryPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn nameColumn     READ nameColumn     WRITE setNameColumn    )
  Q_PROPERTY(CQChartsColumn geometryColumn READ geometryColumn WRITE setGeometryColumn)
  Q_PROPERTY(CQChartsColumn valueColumn    READ valueColumn    WRITE setValueColumn   )
  Q_PROPERTY(CQChartsColumn styleColumn    READ styleColumn    WRITE setStyleColumn   )

  // color
  Q_PROPERTY(bool colorByValue READ isColorByValue WRITE setColorByValue)

  // value range
  Q_PROPERTY(double minValue READ minValue WRITE setMinValue)
  Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue)

  // cell
  Q_PROPERTY(ValueStyle valueStyle READ valueStyle WRITE setValueStyle)

  // selectable
  Q_PROPERTY(bool geometrySelectable READ isGeometrySelectable WRITE setGeometrySelectable)

  // shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  Q_ENUMS(ValueStyle)

 public:
  enum class ValueStyle {
    NONE,
    COLOR,
    BALLOON
  };

  using OptReal = boost::optional<double>;

  //! geometry data
  struct Geometry {
    QString                name;     //!< name
    CQChartsGeom::Polygons polygons; //!< polygon list
    OptReal                value;    //!< value
    CQChartsColor          color;    //!< custom color
    CQChartsStyle          style;    //!< custom style
    CQChartsGeom::BBox     bbox;     //!< bounding box
    QModelIndex            ind;      //!< associated model index
  };

 public:
  CQChartsGeometryPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsGeometryPlot();

  //---

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &geometryColumn() const { return geometryColumn_; }
  void setGeometryColumn(const CQChartsColumn &c);

  const CQChartsColumn &valueColumn() const { return valueColumn_; }
  void setValueColumn(const CQChartsColumn &c);

  const CQChartsColumn &styleColumn() const { return styleColumn_; }
  void setStyleColumn(const CQChartsColumn &c);

  //---

  bool isColorByValue() const { return valueStyle_ == ValueStyle::COLOR; }
  void setColorByValue(bool b);

  //---

  // value range
  double minValue() const;
  void setMinValue(double r);

  double maxValue() const;
  void setMaxValue(double r);

  //---

  // value style
  const ValueStyle &valueStyle() const { return valueStyle_; }

  //---

  bool isGeometrySelectable() const { return geometrySelectable_; }
  void setGeometrySelectable(bool b) { geometrySelectable_ = b; }

  //---

  // balloon min/max size
  double minBalloonSize() const { return minBalloonSize_; }
  void setMinBalloonSize(double r) { minBalloonSize_ = r; }

  double maxBalloonSize() const { return maxBalloonSize_; }
  void setMaxBalloonSize(double r) { maxBalloonSize_ = r; }

  //---

  const CQChartsDataLabel *dataLabel() const { return dataLabel_; }
  CQChartsDataLabel *dataLabel() { return dataLabel_; }

  //---

  void addProperties() override;

  void getPropertyNames(QStringList &names, bool hidden) const override;

  //---

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool probe(ProbeData &probeData) const override;

  //---

  void write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
             const QString &viewVarName) const override;

 public slots:
  void setValueStyle(const ValueStyle &style);

 private:
  void addRow(const QAbstractItemModel *model, const ModelVisitor::VisitData &data,
              CQChartsGeom::Range &dataRange) const;

  bool decodeGeometry(const QString &geomStr, CQChartsGeom::Polygons &polygons) const;

 private:
  using Geometries = std::vector<Geometry>;

  // columns
  CQChartsColumn nameColumn_;                              //!< name column
  CQChartsColumn geometryColumn_;                          //!< geometry column
  CQChartsColumn valueColumn_;                             //!< value column
  CQChartsColumn styleColumn_;                             //!< style column
  ColumnType     geometryColumnType_ { ColumnType::NONE }; //!< geometry column type
  ColumnType     colorColumnType_    { ColumnType::NONE }; //!< color column type
  ColumnType     styleColumnType_    { ColumnType::NONE }; //!< style column type

  // labels
  CQChartsDataLabel* dataLabel_ { nullptr }; //!< data label style

  // value
  OptReal               minValue_;                         //!< user min value
  OptReal               maxValue_;                         //!< user max value
  CQChartsGeom::RMinMax valueRange_;                       //!< data value range
  ValueStyle            valueStyle_ { ValueStyle::COLOR }; //!< value style

  // selectable
  bool geometrySelectable_ { true }; //!< is geometry object selectable

  // balloon
  double minBalloonSize_ { 0.01 }; //!< min balloon size (fraction of height)
  double maxBalloonSize_ { 0.25 }; //!< max balloon size (fraction of height)

  // shapes
  Geometries geometries_; //!< geometry shapes
};

#endif
