#ifndef CQChartsAnnotationPlot_H
#define CQChartsAnnotationPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsData.h>
#include <CQChartsValueList.h>

class CQChartsGrahamHull;
class CQChartsDensity;
class CQChartsSmooth;

//---

struct CQChartsAnnotationObjData {
  enum class Types {
    NONE    = 0,
    POINT   = (1<<0),
    POLYGON = (1<<1),
    PATH    = (1<<2),
    RECT    = (1<<3)
  };

  using Type = CQChartsAnnotationType;

  Type        type  { Type::NONE };
  QString     id;
  QStringList idList;
  uint        types { uint(Types::NONE) };
  QString     data;

  // geometry
  CQChartsPosition point;
  CQChartsPolygon  polygon;
  CQChartsPath     path;
  CQChartsRect     rect;

  // image
  CQChartsImage image;

  // common data
  QModelIndex ind;
  QVariant    style;
};

//---

/*!
 * \brief Annotation plot type
 * \ingroup Charts
 */
class CQChartsAnnotationPlotType : public CQChartsPlotType {
 public:
  CQChartsAnnotationPlotType();

  QString name() const override { return "annotation"; }
  QString desc() const override { return "Annotation"; }

  void addParameters() override;

  bool canProbe() const override { return false; }

  bool canEqualScale() const override { return true; }

  bool hasObjs() const override { return false; }

  QString description() const override;

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsAnnotationPlot;

//---

class CQChartsAnnotationBaseObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  using AnnotationPlot = CQChartsAnnotationPlot;
  using ObjData        = CQChartsAnnotationObjData;
  using Children       = std::vector<CQChartsAnnotationBaseObj *>;

 public:
  CQChartsAnnotationBaseObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                            const ObjData &data, const ColorInd &iv);

  QString calcId() const override;

  QString calcTipId() const override;

  const ObjData &data() const { return data_; }

  CQChartsAnnotationBaseObj *parent() const { return parent_; }
  const Children &children() const { return children_; }

  void addChild(CQChartsAnnotationBaseObj *child);

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  virtual BBox displayRect() const;

  virtual void invalidate();

 protected:
  const AnnotationPlot* annotationPlot_ { nullptr };

  ObjData       data_;
  CQChartsStyle style_;

  CQChartsAnnotationBaseObj *parent_ { nullptr };
  Children                   children_;
};

//---

class CQChartsAnnotationGroupObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

 public:
  // shape
  Q_PROPERTY(ShapeType shapeType READ shapeType WRITE setShapeType)

  // layout
  Q_PROPERTY(LayoutType      layoutType    READ layoutType    WRITE setLayoutType   )
  Q_PROPERTY(Qt::Orientation layoutOrient  READ layoutOrient  WRITE setLayoutOrient )
  Q_PROPERTY(Qt::Alignment   layoutAlign   READ layoutAlign   WRITE setLayoutAlign  )
  Q_PROPERTY(int             layoutMargin  READ layoutMargin  WRITE setLayoutMargin )
  Q_PROPERTY(int             layoutSpacing READ layoutSpacing WRITE setLayoutSpacing)

  Q_ENUMS(LayoutType)
  Q_ENUMS(ShapeType)

 public:
  enum class ShapeType {
    NONE   = static_cast<int>(CQChartsShapeType::Type::NONE),
    BOX    = static_cast<int>(CQChartsShapeType::Type::BOX),
    CIRCLE = static_cast<int>(CQChartsShapeType::Type::CIRCLE)
  };

  enum class LayoutType {
    NONE         = static_cast<int>(CQChartsLayoutType::NONE),
    HV           = static_cast<int>(CQChartsLayoutType::HV),
    CIRCLE       = static_cast<int>(CQChartsLayoutType::CIRCLE),
    TEXT_OVERLAP = static_cast<int>(CQChartsLayoutType::TEXT_OVERLAP),
    TEXT_CLOUD   = static_cast<int>(CQChartsLayoutType::TEXT_CLOUD),
    TREEMAP      = static_cast<int>(CQChartsLayoutType::TREEMAP),
    GRAPH        = static_cast<int>(CQChartsLayoutType::GRAPH)
  };

 public:
  CQChartsAnnotationGroupObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                             const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "group"; }

  //---

  //! get/set outline shape
  const ShapeType &shapeType() const { return shapeType_; }
  void setShapeType(const ShapeType &s);

  //---

  // layout

  //! get/set layout type
  const LayoutType &layoutType() const { return layoutData_.type; }
  void setLayoutType(const LayoutType &t) { layoutData_.type = t; invalidateLayout(); }

  //! get/set layout orientation
  const Qt::Orientation &layoutOrient() const { return layoutData_.orient; }
  void setLayoutOrient(const Qt::Orientation &o) { layoutData_.orient = o; invalidateLayout(); }

  //! get/set layout alignment
  const Qt::Alignment &layoutAlign() const { return layoutData_.align; }
  void setLayoutAlign(const Qt::Alignment &a) { layoutData_.align = a; invalidateLayout(); }

  //! get/set layout margin
  int layoutMargin() const { return layoutData_.margin; }
  void setLayoutMargin(int i) { layoutData_.margin = i; invalidateLayout(); }

  //! get/set layout spacing
  int layoutSpacing() const { return layoutData_.spacing; }
  void setLayoutSpacing(int i) { layoutData_.spacing = i; invalidateLayout(); }

  //---

  void doLayout();

  void invalidateLayout() { needsLayout_ = true; invalidate(); }

  void invalidate() override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void draw(PaintDevice *device) const override;

 protected:
  void layoutHV();
  void layoutCircle();
  void layoutTextOverlap();
  void layoutTextCloud();
  void layoutTreemap();
  void layoutGraph();

 private:
  struct LayoutData {
    LayoutType      type    { LayoutType::NONE };                    //!< layout type
    Qt::Orientation orient  { Qt::Horizontal };                      //!< layout orientation
    Qt::Alignment   align   { Qt::AlignHCenter | Qt::AlignVCenter }; //!< layout align
    int             margin  { 2 };                                   //!< layout margin in pixels
    int             spacing { 2 };                                   //!< layout spacing in pixels
  };

  ShapeType  shapeType_   { ShapeType::NONE }; //!< shape type
  LayoutData layoutData_;                      //!< layout data

  mutable bool needsLayout_ { true }; //!< layout needed
};

//---

class CQChartsAnnotationRectObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsRect rect READ getRect WRITE setRect)

 public:
  using Rect = CQChartsRect;

 public:
  CQChartsAnnotationRectObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                            const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "rect"; }

  const Rect &getRect() const { return rect_; }
  void setRect(const Rect &r);

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void draw(PaintDevice *device) const override;

 private:
  Rect rect_;
};

//---

class CQChartsAnnotationEllipseObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsRect rect READ getRect WRITE setRect)

 public:
  using Rect = CQChartsRect;

 public:
  CQChartsAnnotationEllipseObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                               const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "ellipse"; }

  const Rect &getRect() const { return rect_; }
  void setRect(const Rect &r);

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void draw(PaintDevice *device) const override;

 private:
  Rect rect_;
};

//---

class CQChartsAnnotationPolygonObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsPolygon polygon  READ getPolygon WRITE setPolygon )
  Q_PROPERTY(bool            smoothed READ isSmoothed WRITE setSmoothed)

 public:
  using Polygon = CQChartsPolygon;

 public:
  CQChartsAnnotationPolygonObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                               const ObjData &data, const ColorInd &iv);
 ~CQChartsAnnotationPolygonObj();

  QString typeName() const override { return "polygon"; }

  //---

  //! get/set polygon
  const Polygon &getPolygon() const { return polygon_; }
  void setPolygon(const Polygon &polygon);

  //! get/set is smoothed
  bool isSmoothed() const { return smoothed_; }
  void setSmoothed(bool b);

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void draw(PaintDevice *device) const override;

 protected:
  void initSmooth() const;

 protected:
  using Smooth  = CQChartsSmooth;
  using SmoothP = std::unique_ptr<Smooth>;

  Polygon polygon_;            //!< polygon points
  Polygon apoly_;              //!< rotated polygon
  bool    smoothed_ { false }; //!< draw smoothed line
  SmoothP smooth_;             //!< smooth object
};

//---

class CQChartsAnnotationPolylineObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsPolygon polygon  READ getPolygon WRITE setPolygon )
  Q_PROPERTY(bool            smoothed READ isSmoothed WRITE setSmoothed)

 public:
  using Polygon = CQChartsPolygon;

 public:
  CQChartsAnnotationPolylineObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                                const ObjData &data, const ColorInd &iv);
 ~CQChartsAnnotationPolylineObj();

  QString typeName() const override { return "polyline"; }

  //---

  //! get/set polygon
  const Polygon &getPolygon() const { return polygon_; }
  void setPolygon(const Polygon &polygon);

  //! get/set is smoothed
  bool isSmoothed() const { return smoothed_; }
  void setSmoothed(bool b);

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void draw(PaintDevice *device) const override;

 protected:
  void initSmooth() const;

 protected:
  using Smooth  = CQChartsSmooth;
  using SmoothP = std::unique_ptr<Smooth>;

  Polygon polygon_;            //!< polygon points
  Polygon apoly_;              //!< rotated polygon
  bool    smoothed_ { false }; //!< draw smoothed line
  SmoothP smooth_;             //!< smooth object
};

//---

class CQChartsAnnotationTextObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsRect     rect  READ getRect  WRITE setRect)
  Q_PROPERTY(CQChartsPosition point READ getPoint WRITE setPoint)
  Q_PROPERTY(QString          text  READ text     WRITE setText)

 public:
  using Rect        = CQChartsRect;
  using Point       = CQChartsPosition;
  using Image       = CQChartsImage;
  using TextOptions = CQChartsTextOptions;

 public:
  CQChartsAnnotationTextObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                            const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "text"; }

  //---

  const Rect &getRect() const { return rect_; }
  virtual void setRect(const Rect &r);

  const Point &getPoint() const { return point_; }
  virtual void setPoint(const Point &p);

  const QString &text() const { return text_; }
  void setText(const QString &s);

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void draw(PaintDevice *device) const override;

 private:
  Rect    rect_;
  Point   point_;
  QString text_;

  TextOptions textOptions_;
};

//---

class CQChartsAnnotationImageObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsRect     rect  READ getRect  WRITE setRect)
  Q_PROPERTY(CQChartsPosition point READ getPoint WRITE setPoint)
  Q_PROPERTY(CQChartsImage    image READ image    WRITE setImage)

 public:
  using Rect  = CQChartsRect;
  using Point = CQChartsPosition;
  using Image = CQChartsImage;

 public:
  CQChartsAnnotationImageObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                             const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "image"; }

  //---

  const Rect &getRect() const { return rect_; }
  virtual void setRect(const Rect &r);

  const Point &getPoint() const { return point_; }
  virtual void setPoint(const Point &r);

  const Image &image() const { return image_; }
  void setImage(const Image &image);

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void draw(PaintDevice *device) const override;

 private:
  Rect  rect_;
  Point point_;
  Image image_;
};

//---

class CQChartsAnnotationPathObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsPath path READ getPath WRITE setPath)

 public:
  using Path = CQChartsPath;

 public:
  CQChartsAnnotationPathObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                            const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "path"; }

  //---

  const Path &getPath() const { return path_; }
  virtual void setPath(const Path &p);

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void draw(PaintDevice *device) const override;

 private:
  Path path_;
};

//---

class CQChartsAnnotationArrowObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

 public:
  CQChartsAnnotationArrowObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                             const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "arrow"; }

  void draw(PaintDevice *device) const override;
};

//---

class CQChartsAnnotationArcObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

 public:
  CQChartsAnnotationArcObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                           const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "arc"; }

  void draw(PaintDevice *device) const override;
};

//---

class CQChartsAnnotationArcConnectorObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

 public:
  CQChartsAnnotationArcConnectorObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                                    const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "arc_connector"; }

  void draw(PaintDevice *device) const override;
};

//---

class CQChartsAnnotationAxisObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

  Q_PROPERTY(Qt::Orientation       direction READ direction WRITE setDirection)
  Q_PROPERTY(double                position  READ position  WRITE setPosition )
  Q_PROPERTY(double                start     READ start     WRITE setStart    )
  Q_PROPERTY(double                end       READ end       WRITE setEnd      )
  Q_PROPERTY(CQChartsAxisValueType valueType READ valueType WRITE setValueType)

 public:
  using Axis      = CQChartsAxis;
  using ValueType = CQChartsAxisValueType;

 public:
  CQChartsAnnotationAxisObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                            const ObjData &data, const ColorInd &iv);

 ~CQChartsAnnotationAxisObj();

  QString typeName() const override { return "axis"; }

  //---

  //! get/set direction
  const Qt::Orientation &direction() const { return direction_; }
  void setDirection(Qt::Orientation &d);

  //! get/set position
  double position() const { return position_; }
  void setPosition(double r);

  //! get/set start
  double start() const { return start_; }
  void setStart(double r);

  //! get/set end
  double end() const { return end_; }
  void setEnd(double r);

  const ValueType &valueType() const { return valueType_; }
  void setValueType(const ValueType &v);

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void draw(PaintDevice *device) const override;

 private:
  using AxisP = std::unique_ptr<Axis>;

  Qt::Orientation direction_ { Qt::Horizontal }; //!< direction
  double          position_  { 0.0 };            //!< position
  double          start_     { 0.0 };            //!< start
  double          end_       { 1.0 };            //!< end
  ValueType       valueType_;                    //!< value type

  AxisP axis_; //!< axis
};

//---

class CQChartsAnnotationPointObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition point  READ getPoint  WRITE setPoint )
  Q_PROPERTY(CQChartsSymbol   symbol READ getSymbol WRITE setSymbol)
  Q_PROPERTY(CQChartsLength   size   READ getSize   WRITE setSize  )

 public:
  using Point  = CQChartsPosition;
  using Symbol = CQChartsSymbol;
  using Length = CQChartsLength;

 public:
  CQChartsAnnotationPointObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                             const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "point"; }

  //---

  const Point &getPoint() const { return point_; }
  virtual void setPoint(const Point &r);

  const Symbol &getSymbol() const { return symbol_; }
  virtual void setSymbol(const Symbol &s);

  const Length &getSize() const { return size_; }
  virtual void setSize(const Length &l);

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void draw(PaintDevice *device) const override;

 private:
  Point  point_;
  Symbol symbol_;
  Length size_;
};

//---

class CQChartsAnnotationPieSliceObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsLength innerRadius READ innerRadius WRITE setInnerRadius)
  Q_PROPERTY(CQChartsLength outerRadius READ outerRadius WRITE setOuterRadius)
  Q_PROPERTY(CQChartsAngle  startAngle  READ startAngle  WRITE setStartAngle )
  Q_PROPERTY(CQChartsAngle  spanAngle   READ spanAngle   WRITE setSpanAngle  )
  Q_PROPERTY(ArcType          arcType     READ arcType     WRITE setArcType    )

  Q_ENUMS(ArcType)

 public:
  enum class ArcType {
    SLICE,
    SEGMENT,
    ARC
  };

  using Angle  = CQChartsAngle;
  using Length = CQChartsLength;

 public:
  CQChartsAnnotationPieSliceObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                                const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "pie_slice"; }

  //---

  //! get/set inner radius
  const Length &innerRadius() const { return innerRadius_; }
  void setInnerRadius(const Length &r);

  //! get/set outer radius
  const Length &outerRadius() const { return outerRadius_; }
  void setOuterRadius(const Length &r);

  //! get/set start angle
  const Angle &startAngle() const { return startAngle_; }
  void setStartAngle(const Angle &a);

  //! get/set span angle
  const Angle &spanAngle() const { return spanAngle_; }
  void setSpanAngle(const Angle &a);

  //! get/set arc type
  const ArcType &arcType() const { return arcType_; }
  void setArcType(const ArcType &t);

  //---

  //! add properties

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  void draw(PaintDevice *device) const override;

  BBox displayRect() const override;

 private:
  Length  innerRadius_;                    //!< inner radius
  Length  outerRadius_;                    //!< outer radius
  Angle   startAngle_  {  0.0 };           //!< start angle
  Angle   spanAngle_   { 90.0 };           //!< span angle
  ArcType arcType_     { ArcType::SLICE }; //!< arc type

  mutable QPainterPath path_;
};

//---

class CQChartsAnnotationKeyObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

 public:
  CQChartsAnnotationKeyObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                           const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "key"; }

  void draw(PaintDevice *device) const override;

 private:
  using PlotKey = CQChartsPlotKey;

  PlotKey* key_ { nullptr };
};

//---

class CQChartsAnnotationPointSetObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

 public:
  CQChartsAnnotationPointSetObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                                const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "point_set"; }

  void draw(PaintDevice *device) const override;

 private:
  using Hull = CQChartsGrahamHull;

  Hull* hull_ { nullptr };
};

//---

class CQChartsAnnotationValueSetObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

 public:
  CQChartsAnnotationValueSetObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                                const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "value_set"; }

  void draw(PaintDevice *device) const override;

 private:
  using Density = CQChartsDensity;

  CQChartsValueList valueList_;

  Density* density_ { nullptr };
};

//---

class CQChartsAnnotationPlotObj : public CQChartsAnnotationBaseObj {
  Q_OBJECT

 public:
  using Angle  = CQChartsAngle;

 public:
  CQChartsAnnotationPlotObj(const AnnotationPlot *annotationPlot, const BBox &bbox,
                            const ObjData &data, const ColorInd &iv);

  QString typeName() const override { return "obj"; }

  void draw(PaintDevice *device) const override;
};

//---

/*!
 * \brief Annotation Plot
 * \ingroup Charts
 */
class CQChartsAnnotationPlot : public CQChartsPlot {
  Q_OBJECT

 public:
  CQChartsAnnotationPlot(View *view, const ModelP &model);
 ~CQChartsAnnotationPlot();

  //---

  void init() override;
  void term() override;

  //---

  int numColumns() const { return numColumns_; }
  void setNumColumns(int i) { numColumns_ = i; }

  //---

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  void postCreateObjs() override;

  //---

  CQChartsAnnotationBaseObj *findObject(const QString &id) const;

  bool addMenuItems(QMenu *menu, const Point &p) override;

 private:
  void addRow(const QAbstractItemModel *model, const ModelVisitor::VisitData &data,
              Range &dataRange) const;

 protected:
  using ObjData  = CQChartsAnnotationObjData;
  using ObjDatas = std::vector<ObjData>;

 protected:
  using Objs = std::vector<CQChartsAnnotationBaseObj *>;

  CQChartsPlotCustomControls *createCustomControls() override;

  ObjDatas objDatas_;
  int      numColumns_ { 0 };

  Objs objs_;
};

//---

#include <CQChartsPlotCustomControls.h>

/*!
 * \brief Annotation Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsAnnotationPlotCustomControls : public CQChartsPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsAnnotationPlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(Plot *plot) override;

 protected:
  CQChartsAnnotationPlot* annotationPlot_ { nullptr }; //!< plot
};

#endif
