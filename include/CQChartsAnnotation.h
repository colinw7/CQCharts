#ifndef CQChartsAnnotation_H
#define CQChartsAnnotation_H

#include <CQChartsTextBoxObj.h>
#include <CQChartsSymbol.h>
#include <CQChartsImage.h>
#include <CQChartsWidget.h>
#include <CQChartsData.h>
#include <CQChartsArrowData.h>
#include <CQChartsOptRect.h>
#include <CQChartsOptPosition.h>
#include <CQChartsPolygon.h>
#include <CQChartsPath.h>
#include <CQChartsPoints.h>
#include <CQChartsReals.h>
#include <CQChartsGeom.h>
#include <CQChartsGrahamHull.h>
#include <CQChartsGridCell.h>
#include <CQChartsAngle.h>
#include <CQChartsObjRef.h>
#include <CQChartsValueSet.h>
#include <CQChartsModelColumn.h>
#include <CQChartsModelIndex.h>
#include <CQChartsCirclePack.h>
#include <CQChartsOptReal.h>
#include <CQChartsObjRefPos.h>

class CQChartsAnnotationGroup;
class CQChartsSmooth;
class CQChartsDensity;
class CQChartsKey;
class CQChartsAxis;
class CQChartsHtmlPaintDevice;
class CQChartsSymbolSizeMapKey;
class CQChartsResizeHandle;
class CQChartsCircleMgr;

class CQPropertyViewItem;

/*!
 * \brief base class for view/plot annotation
 * \ingroup Charts
 *
 * Enable, Checkable, Checked, Fitted properties
 */
class CQChartsAnnotation : public CQChartsTextBoxObj {
  Q_OBJECT

  // ind
  Q_PROPERTY(int ind READ ind WRITE setInd)

  // state
  Q_PROPERTY(bool enabled   READ isEnabled   WRITE setEnabled  )
  Q_PROPERTY(bool checkable READ isCheckable WRITE setCheckable)
  Q_PROPERTY(bool checked   READ isChecked   WRITE setChecked  )
  Q_PROPERTY(bool fitted    READ isFitted    WRITE setFitted   )

  // value
  Q_PROPERTY(CQChartsOptReal value READ value WRITE setValue)

  // associated model data
  Q_PROPERTY(CQChartsModelIndex modelIndex READ modelIndex WRITE setModelIndex)

  // appearance
  Q_PROPERTY(double              disabledLighter  READ disabledLighter  WRITE setDisabledLighter )
  Q_PROPERTY(double              uncheckedLighter READ uncheckedLighter WRITE setUncheckedLighter)
  Q_PROPERTY(DrawLayer           drawLayer        READ drawLayer        WRITE setDrawLayer       )
  Q_PROPERTY(CQChartsPaletteName defaultPalette   READ defaultPalette   WRITE setDefaultPalette  )

  Q_ENUMS(DrawLayer)

 public:
  // all annotation types
  enum class Type {
    NONE           = int(CQChartsAnnotationType::NONE),
    GROUP          = int(CQChartsAnnotationType::GROUP),
    RECT           = int(CQChartsAnnotationType::RECT),
    ELLIPSE        = int(CQChartsAnnotationType::ELLIPSE),
    POLYGON        = int(CQChartsAnnotationType::POLYGON),
    POLYLINE       = int(CQChartsAnnotationType::POLYLINE),
    TEXT           = int(CQChartsAnnotationType::TEXT),
    IMAGE          = int(CQChartsAnnotationType::IMAGE),
    PATH           = int(CQChartsAnnotationType::PATH),
    ARROW          = int(CQChartsAnnotationType::ARROW),
    ARC            = int(CQChartsAnnotationType::ARC),
    ARC_CONNECTOR  = int(CQChartsAnnotationType::ARC_CONNECTOR),
    POINT          = int(CQChartsAnnotationType::POINT),
    PIE_SLICE      = int(CQChartsAnnotationType::PIE_SLICE),
    AXIS           = int(CQChartsAnnotationType::AXIS),
    KEY            = int(CQChartsAnnotationType::KEY),
    POINT3D_SET    = int(CQChartsAnnotationType::POINT3D_SET),
    POINT_SET      = int(CQChartsAnnotationType::POINT_SET),
    VALUE_SET      = int(CQChartsAnnotationType::VALUE_SET),
    BUTTON         = int(CQChartsAnnotationType::BUTTON),
    WIDGET         = int(CQChartsAnnotationType::WIDGET),
    SYMBOL_MAP_KEY = int(CQChartsAnnotationType::SYMBOL_MAP_KEY)
  };

  // annotation sub types (classifcation)
  enum class SubType {
    GROUP,
    SHAPE,
    TEXT,
    CONNECTOR,
    POINT,
    AXIS,
    KEY,
    WIDGET
  };

  // annotation draw layer
  enum class DrawLayer {
    BACKGROUND,
    FOREGROUND
  };

  using View        = CQChartsView;
  using Plot        = CQChartsPlot;
  using Group       = CQChartsAnnotationGroup;
  using ResizeSide  = CQChartsResizeSide;
  using Color       = CQChartsColor;
  using SelMod      = CQChartsSelMod;
  using ColorInd    = CQChartsUtil::ColorInd;
  using ObjRef      = CQChartsObjRef;
  using ObjRefPos   = CQChartsObjRefPos;
  using Units       = CQChartsUnits::Type;
  using Alpha       = CQChartsAlpha;
  using PaletteName = CQChartsPaletteName;
  using OptReal     = CQChartsOptReal;
  using ModelIndex  = CQChartsModelIndex;

  using PaintDevice     = CQChartsPaintDevice;
  using HtmlPaintDevice = CQChartsHtmlPaintDevice;

 public:
  static const QStringList &typeNames();
  static const QStringList &typeDescs();

  static Type stringToType(const QString &str);

 public:
  CQChartsAnnotation(View *view, Type type);
  CQChartsAnnotation(Plot *plot, Type type);

  virtual ~CQChartsAnnotation();

  //---

  //! get/set ind
  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  //! calculate id
  QString calcId() const override;

  //! calculate tip
  QString calcTipId() const override;

  //! get path id
  QString pathId() const;

  //---

  //! get/set parent annotation group
  const Group *group() const { return group_; }
  void setGroup(Group *g) { group_ = g; }

  //---

  //! get type and type name
  const Type &type() const { return type_; }

  virtual const char *typeName() const = 0;

  virtual const char *propertyName() const = 0;

  virtual const char *cmdName() const = 0;

  virtual SubType subType() const = 0;

  //---

  //! get/set enabled
  bool isEnabled() const { return enabled_; }
  void setEnabled(bool b);

  //! get/set checkable
  bool isCheckable() const { return checkable_; }
  void setCheckable(bool b);

  //! get/set checked
  bool isChecked() const { return checked_; }
  void setChecked(bool b);

  //---

  //! get/set annotation used in plot fit calc
  bool isFitted() const { return fitted_; }
  void setFitted(bool b) { fitted_ = b; }

  //---

  //! get/set disabled lighter
  double disabledLighter() const { return disabledLighter_; }
  void setDisabledLighter(double r) { disabledLighter_ = r; }

  //! get/set unchecked lighter
  double uncheckedLighter() const { return uncheckedLighter_; }
  void setUncheckedLighter(double r) { uncheckedLighter_ = r; }

  //---

  //! get/set bounding box
  const BBox &annotationBBox() const { return annotationBBox_; }
  void setAnnotationBBox(const BBox &bbox);

  //---

  //! get/set draw layer
  const DrawLayer &drawLayer() const { return drawLayer_; }
  void setDrawLayer(const DrawLayer &l);

  //---

  //! get/set default palette
  const PaletteName &defaultPalette() const { return defaultPalette_; }
  void setDefaultPalette(const PaletteName &name);

  //---

  //! get/set value
  const OptReal &value() const { return value_; }
  void setValue(const OptReal &r);

  //---

  //! get/set model index
  const ModelIndex &modelIndex() const { return modelIndex_; }
  void setModelIndex(const ModelIndex &c);

  //---

  virtual bool hasMargin() const { return false; }
  virtual bool hasPadding() const { return false; }

  // get margin values
  void getMarginValues(double &xlm, double &xrm, double &ytm, double &ybm) const;

  // get padding values
  void getPaddingValues(double &xlp, double &xrp, double &ytp, double &ybp) const;

  //---

  //! get property path
  virtual QString propertyId() const;

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //! add stroke and fill properties
  void addStrokeFillProperties(PropertyModel *model, const QString &path, bool isSolid=true);

  //! add stroke properties
  void addStrokeProperties(PropertyModel *model, const QString &path, bool isSolid=true);

  //! add fill properties
  void addFillProperties(PropertyModel *model, const QString &path);

  void addTextProperties(PropertyModel *model, const QString &path, uint types);

  CQPropertyViewItem *addStyleProp(PropertyModel *model, const QString &path, const QString &name,
                                   const QString &alias, const QString &desc, bool hidden=false);

  CQPropertyViewItem *addProp(PropertyModel *model, const QString &path, const QString &name,
                              const QString &alias, const QString &desc, bool hidden=false);

  bool setProperties(const QString &properties);

  //! get/set property
  bool getProperty(const QString &name, QVariant &value) const;
  bool setProperty(const QString &name, const QVariant &value);

  bool getTclProperty(const QString &name, QVariant &value) const;

  bool getPropertyDesc    (const QString &name, QString  &desc, bool hidden=false) const;
  bool getPropertyType    (const QString &name, QString  &type, bool hidden=false) const;
  bool getPropertyUserType(const QString &name, QString  &type, bool hidden=false) const;
  bool getPropertyObject  (const QString &name, QObject* &obj , bool hidden=false) const;
  bool getPropertyIsHidden(const QString &name, bool &is_hidden) const;
  bool getPropertyIsStyle (const QString &name, bool &is_style) const;

  //! get property names
  virtual void getPropertyNames(QStringList &names, bool hidden=false) const;

  PropertyModel *propertyModel() const;

  const CQPropertyViewItem *propertyItem(const QString &name, bool hidden=false) const;

  //---

  //! is point inside (also checks visible)
  bool contains(const Point &p) const override;

  //! is point inside
  virtual bool inside(const Point &p) const;

  bool intersects(const BBox &r, bool inside) const;

  //---

  virtual BBox calcBBox() const { return annotationBBox(); }

  //---

  //! interp color
  QColor interpColor(const Color &c, const ColorInd &ind) const;

  //! get palette name
  CQChartsPaletteName calcPalette() const;
  QString calcPaletteName() const;

  //---

  //! handle select press
  bool selectPress(const Point &p, SelData &selData) override;

  //---

  // Implement edit interface
  bool editPress (const Point &) override;
  bool editMove  (const Point &) override;
  bool editMotion(const Point &) override;

  void editMoveBy(const Point &d) override;

  //---

  virtual void flip(Qt::Orientation) { }

  virtual void moveTo(const Point &p);

  //---

  virtual void animateStep();

  //---

  bool isDisableSignals() const { return disableSignals_; }
  void setDisableSignals(bool b) { disableSignals_ = b; }

  //---

  void connectDataChanged(const QObject *obj, const char *slotName) const;

  //! handle box obj data changed
  void boxObjInvalidate() override { emitDataChanged(); }

  void invalidate();

  //---

  void calcPenBrush(PenBrush &penBrush);

  //---

  //! draw
  virtual void draw(PaintDevice *device);

  virtual void drawInit(PaintDevice *device);
  virtual void drawTerm(PaintDevice *device);

  //---

  //! write custom SVG html
  virtual void writeHtml(HtmlPaintDevice *) { }

  //! write tcl script
  void write(std::ostream &os, const QString &parentVarName=QString(),
             const QString &varName=QString()) const;

  //! write details (command to recreate)
  virtual void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                            const QString &varName=QString()) const = 0;

  //! write key values
  void writeKeys(std::ostream &os, const QString &cmd, const QString &parentVarName=QString(),
                 const QString &varName=QString()) const;

  //! write polygon points
  void writePoints(std::ostream &os, const Polygon &polygon) const;

  //! write properties
  void writeProperties(std::ostream &os, const QString &varName=QString()) const;

  //---

  //! initialize state when first resized to explicit rectangle in edit
  virtual void initRectangle();

  //! called when parent view changed
  virtual void parentViewChanged() { }

 Q_SIGNALS:
  //! emitted when data changed
  void dataChanged();

  //! emitted when annotation pressed
  void pressed(const QString &id);

 protected Q_SLOTS:
  void emitDataChanged();

  void invalidateSlot() { invalidate(); }

 protected:
  void init(int &lastInd);

  CQChartsResizeHandle *createExtraHandle() const;

  Units parentUnits() const {
    if      (plot()) return Units::PLOT;
    else if (view()) return Units::VIEW;

    return Units::NONE;
  }

 protected:
  Type type_ { Type::NONE }; //!< type
  int  ind_  { 0 };          //!< unique ind

  Group* group_ { nullptr }; //!< parent group

  // state
  bool enabled_   { true };  //!< is enabled
  bool checkable_ { false }; //!< is checkable
  bool checked_   { false }; //!< is checked
  bool fitted_    { false }; //!< is fitted

  OptReal value_; //!< associated value

  ModelIndex modelIndex_; //!< model index

  // disabled/unchecked lighter
  double disabledLighter_  { 0.8 }; //!< disabled lighter
  double uncheckedLighter_ { 0.5 }; //!< unchecked lighter

  BBox annotationBBox_; //!< bbox (plot coords) (remove ?)

  DrawLayer drawLayer_ { DrawLayer::FOREGROUND }; //!< draw foreground

  // palette
  PaletteName defaultPalette_; //!< default palette

  mutable bool disableSignals_ { false }; //!< disable signals
};

//---

/*!
 * \brief Annotation Group class
 * \ingroup Charts
 *
 * Grouping of annotations or other annotation groups.
 */
class CQChartsAnnotationGroup : public CQChartsAnnotation {
  Q_OBJECT

  // shape
  Q_PROPERTY(CQChartsRect rectangle READ rectangle WRITE setRectangle)
  Q_PROPERTY(ShapeType    shapeType READ shapeType WRITE setShapeType)

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
    NONE   = static_cast<int>(CQChartsAnnotation::ShapeType::NONE),
    BOX    = static_cast<int>(CQChartsAnnotation::ShapeType::BOX),
    CIRCLE = static_cast<int>(CQChartsAnnotation::ShapeType::CIRCLE)
  };

  enum class LayoutType {
    NONE,
    HV,
    CIRCLE,
    TEXT_OVERLAP,
    TEXT_CLOUD,
    TREEMAP,
    GRAPH
  };

  using Rect   = CQChartsRect;
  using Length = CQChartsLength;

 public:
  CQChartsAnnotationGroup(View *view);
  CQChartsAnnotationGroup(Plot *plot);

 ~CQChartsAnnotationGroup();

  const char *typeName() const override { return "group"; }

  const char *propertyName() const override { return "annotationGroup"; }

  const char *cmdName() const override { return "create_charts_annotation_group"; }

  SubType subType() const override { return SubType::GROUP; }

  //---

  //! get/set bounding rectangle
  Rect rectangle() const;
  void setRectangle(const Rect &r);

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

  //! get/set outline shape
  const ShapeType &shapeType() const { return shapeType_; }
  void setShapeType(const ShapeType &s);

  //---

  //! add/remove child annotation
  void addAnnotation   (CQChartsAnnotation *annotation);
  void removeAnnotation(CQChartsAnnotation *annotation);

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  void moveChildren(const BBox &bbox);
  void moveChildren(double dx, double dy);

  void flip(Qt::Orientation orient) override;

  void moveTo(const Point &p) override;

  //---

  bool inside(const Point &p) const override;

  //---

  void doLayout();

  void invalidateLayout() { needsLayout_ = true; invalidate(); }

  BBox childrenBBox() const;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void init();

  void layoutHV();
  void layoutCircle();
  void layoutTextOverlap();
  void layoutTextCloud();
  void layoutTreemap();
  void layoutGraph();

 protected:
  using Annotations = std::vector<CQChartsAnnotation *>;

  struct LayoutData {
    LayoutType      type    { LayoutType::NONE };                    //!< layout type
    Qt::Orientation orient  { Qt::Horizontal };                      //!< layout orientaton
    Qt::Alignment   align   { Qt::AlignHCenter | Qt::AlignVCenter }; //!< layout align
    int             margin  { 2 };                                   //!< layout margin in pixels
    int             spacing { 2 };                                   //!< layout spacing in pixels
  };

  Annotations  annotations_;                      //!< child annotations
  LayoutData   layoutData_;                       //!< layout data
  ShapeType    shapeType_    { ShapeType::NONE }; //!< shape type
  mutable bool initBBox_     { true };            //!< bbox inited
  mutable bool needsLayout_  { true };            //!< layout needed
};

//---

/*!
 * \brief Annotation Shape Base class
 * \ingroup Charts
 *
 * Defines rotation angle, any associated object reference (for position) and
 * any associated text annotation id (for text label).
 */
class CQChartsShapeAnnotationBase : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsAngle  angle   READ angle   WRITE setAngle  )
  Q_PROPERTY(CQChartsObjRef objRef  READ objRef  WRITE setObjRef )
  Q_PROPERTY(QString        textInd READ textInd WRITE setTextInd)

 public:
  CQChartsShapeAnnotationBase(View *view, Type type);
  CQChartsShapeAnnotationBase(Plot *plot, Type type);

 ~CQChartsShapeAnnotationBase();

  //---

  SubType subType() const override { return SubType::SHAPE; }

  //---

  //! get/set angle
  const Angle &angle() const { return angle_; }
  void setAngle(const Angle &a);

  //! get/set object reference
  const ObjRef &objRef() const { return objRef_; }
  void setObjRef(const ObjRef &o);

  //! get/set text ind
  const QString &textInd() const { return textInd_; }
  void setTextInd(const QString &ind);

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  // draw text
  void drawText(PaintDevice *device, const BBox &rect);

 protected:
  Angle   angle_;   //!< rotation angle
  ObjRef  objRef_;  //!< object ref for ppsition
  QString textInd_; //!< text ind for text label
};

//---

/*!
 * \brief Annotation Polygon Shape class
 * \ingroup Charts
 */
class CQChartsPolyShapeAnnotationBase : public CQChartsShapeAnnotationBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsPolygon polygon  READ polygon    WRITE setPolygon )
  Q_PROPERTY(bool            smoothed READ isSmoothed WRITE setSmoothed)

 public:
  using Polygon = CQChartsPolygon;

 public:
  CQChartsPolyShapeAnnotationBase(View *view, Type type, const Polygon &polygon=Polygon());
  CQChartsPolyShapeAnnotationBase(Plot *plot, Type type, const Polygon &polygon=Polygon());

  virtual ~CQChartsPolyShapeAnnotationBase();

  //! get/set polygon
  const Polygon &polygon() const { return polygon_; }
  void setPolygon(const Polygon &polygon);

  //! get/set is smoothed
  bool isSmoothed() const { return smoothed_; }
  void setSmoothed(bool b);

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

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

/*!
 * \brief rectangle annotation
 * \ingroup Charts
 *
 * Filled and/or Stroked rectangle with optional rounded corners
 */
class CQChartsRectangleAnnotation : public CQChartsShapeAnnotationBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsRect     rectangle READ rectangle WRITE setRectangle )
  Q_PROPERTY(CQChartsPosition start     READ start     WRITE setStart     )
  Q_PROPERTY(CQChartsPosition end       READ end       WRITE setEnd       )

  Q_ENUMS(ShapeType)

 public:
  enum class ShapeType {
    NONE          = static_cast<int>(CQChartsShapeType::NONE),
    TRIANGLE      = static_cast<int>(CQChartsShapeType::TRIANGLE),
    DIAMOND       = static_cast<int>(CQChartsShapeType::DIAMOND),
    BOX           = static_cast<int>(CQChartsShapeType::BOX),
    POLYGON       = static_cast<int>(CQChartsShapeType::POLYGON),
    CIRCLE        = static_cast<int>(CQChartsShapeType::CIRCLE),
    DOUBLE_CIRCLE = static_cast<int>(CQChartsShapeType::DOUBLE_CIRCLE),
    DOT_LINE      = static_cast<int>(CQChartsShapeType::DOT_LINE)
  };

 public:
  using Rect       = CQChartsRect;
  using Symbol     = CQChartsSymbol;
  using SymbolType = CQChartsSymbolType;
  using Length     = CQChartsLength;

 public:
  CQChartsRectangleAnnotation(View *view, const Rect &rect=Rect());
  CQChartsRectangleAnnotation(Plot *plot, const Rect &rect=Rect());

  virtual ~CQChartsRectangleAnnotation();

  //---

  const char *typeName() const override { return "rectangle"; }

  const char *propertyName() const override { return "rectangleAnnotation"; }

  const char *cmdName() const override { return "create_charts_rectangle_annotation"; }

  bool hasMargin() const override { return true; }

  //---

  //! get/set rectangle
  const Rect &rectangle() const { return rectangle_; }
  void setRectangle(const Rect &rectangle);
  void setRectangle(const Position &start, const Position &end);

  //! get/set start point (TODO: remove)
  Position start() const;
  void setStart(const Position &p);

  //! get/set end point (TODO: remove)
  Position end() const;
  void setEnd(const Position &p);

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void init();

 protected:
  Rect   rectangle_; //!< rectangle
};

//---

/*!
 * \brief shape annotation
 * \ingroup Charts
 *
 * Filled and/or Stroked shape
 */
class CQChartsShapeAnnotation : public CQChartsShapeAnnotationBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsRect       rectangle  READ rectangle   WRITE setRectangle )
  Q_PROPERTY(CQChartsPosition   start      READ start       WRITE setStart     )
  Q_PROPERTY(CQChartsPosition   end        READ end         WRITE setEnd       )
  Q_PROPERTY(ShapeType          shapeType  READ shapeType   WRITE setShapeType )
  Q_PROPERTY(int                numSides   READ numSides    WRITE setNumSides  )
  Q_PROPERTY(CQChartsLength     lineWidth  READ lineWidth   WRITE setLineWidth )
  Q_PROPERTY(CQChartsSymbol     symbol     READ symbol      WRITE setSymbol    )
  Q_PROPERTY(CQChartsLength     symbolSize READ symbolSize  WRITE setSymbolSize)

  Q_ENUMS(ShapeType)

 public:
  enum class ShapeType {
    NONE          = static_cast<int>(CQChartsShapeType::NONE),
    TRIANGLE      = static_cast<int>(CQChartsShapeType::TRIANGLE),
    DIAMOND       = static_cast<int>(CQChartsShapeType::DIAMOND),
    BOX           = static_cast<int>(CQChartsShapeType::BOX),
    POLYGON       = static_cast<int>(CQChartsShapeType::POLYGON),
    CIRCLE        = static_cast<int>(CQChartsShapeType::CIRCLE),
    DOUBLE_CIRCLE = static_cast<int>(CQChartsShapeType::DOUBLE_CIRCLE),
    DOT_LINE      = static_cast<int>(CQChartsShapeType::DOT_LINE)
  };

 public:
  using Rect       = CQChartsRect;
  using Symbol     = CQChartsSymbol;
  using SymbolType = CQChartsSymbolType;
  using Length     = CQChartsLength;

 public:
  CQChartsShapeAnnotation(View *view, const Rect &rect=Rect());
  CQChartsShapeAnnotation(Plot *plot, const Rect &rect=Rect());

  virtual ~CQChartsShapeAnnotation();

  //---

  const char *typeName() const override { return "shape"; }

  const char *propertyName() const override { return "shapeAnnotation"; }

  const char *cmdName() const override { return "create_charts_shape_annotation"; }

  bool hasMargin() const override { return true; }

  //---

  //! get/set rectangle
  const Rect &rectangle() const { return rectangle_; }
  void setRectangle(const Rect &rectangle);
  void setRectangle(const Position &start, const Position &end);

  //! get/set start point
  Position start() const;
  void setStart(const Position &p);

  //! get/set end point
  Position end() const;
  void setEnd(const Position &p);

  //---

  //! get/set shape
  ShapeType shapeType() const { return static_cast<ShapeType>(shapeTypeData_.shapeType); }
  void setShapeType(const ShapeType &s);

  //! get/set number of sides
  int numSides() const { return shapeTypeData_.numSides; }
  void setNumSides(int n);

  //---

  //! get/set line width (for dot line)
  const Length &lineWidth() const { return lineWidth_; }
  void setLineWidth(const Length &l);

  //! get/set symbol type (for dot line)
  const Symbol &symbol() const { return symbol_; }
  void setSymbol(const Symbol &t);

  //! get/set symbol size (for dot line)
  const Length &symbolSize() const { return symbolSize_; }
  void setSymbolSize(const Length &s);

  //---

  bool intersectShape(const Point &p1, const Point &p2, Point &pi) const override;

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void init();

 protected:
  Rect rectangle_; //!< rectangle

  CQChartsShapeTypeData shapeTypeData_; //!< shape type data

  Length lineWidth_  { Length::plot(1.0) }; //!< dot line width
  Symbol symbol_     { Symbol::circle() };  //!< dot symbol
  Length symbolSize_ { Length::plot(1.0) }; //!< dot symbol size
};

//---

/*!
 * \brief ellipse annotation
 * \ingroup Charts
 *
 * Filled and/or Stroked ellipse/circle
 */
class CQChartsEllipseAnnotation : public CQChartsShapeAnnotationBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition center  READ center  WRITE setCenter )
  Q_PROPERTY(CQChartsLength   xRadius READ xRadius WRITE setXRadius)
  Q_PROPERTY(CQChartsLength   yRadius READ yRadius WRITE setYRadius)

 public:
  using Length = CQChartsLength;
  using Units  = CQChartsUnits::Type;

 public:
  CQChartsEllipseAnnotation(View *view, const ObjRefPos &center=ObjRefPos(),
                            const Length &xRadius=Length::view(1.0),
                            const Length &yRadius=Length::view(1.0));
  CQChartsEllipseAnnotation(Plot *plot, const ObjRefPos &center=ObjRefPos(),
                            const Length &xRadius=Length::plot(1.0),
                            const Length &yRadius=Length::plot(1.0));

  virtual ~CQChartsEllipseAnnotation();

  //---

  const char *typeName() const override { return "ellipse"; }

  const char *propertyName() const override { return "ellipseAnnotation"; }

  const char *cmdName() const override { return "create_charts_ellipse_annotation"; }

  //---

  //! get/set center
  const Position &center() const { return center_; }
  void setCenter(const Position &c);

  //! get/set x radius
  const Length &xRadius() const { return xRadius_; }
  void setXRadius(const Length &r) { xRadius_ = r; }

  //! get/set y radius
  const Length &yRadius() const { return yRadius_; }
  void setYRadius(const Length &r) { yRadius_ = r; }

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  BBox calcBBox() const override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  void moveTo(const Point &p) override;

  //---

  bool inside(const Point &p) const override;

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void init();

 protected:
  Position center_;                        //!< ellipse center
  Length   xRadius_ { Length::plot(1.0) }; //!< ellipse x radius
  Length   yRadius_ { Length::plot(1.0) }; //!< ellipse y radius
};

//---

/*!
 * \brief polygon annotation
 * \ingroup Charts
 *
 * Filled and/or Stroked polygon. Lines can be smoothed
 */
class CQChartsPolygonAnnotation : public CQChartsPolyShapeAnnotationBase {
  Q_OBJECT

 public:
  CQChartsPolygonAnnotation(View *view, const Polygon &polygon=Polygon());
  CQChartsPolygonAnnotation(Plot *plot, const Polygon &polygon=Polygon());

  virtual ~CQChartsPolygonAnnotation();

  //---

  const char *typeName() const override { return "polygon"; }

  const char *propertyName() const override { return "polygonAnnotation"; }

  const char *cmdName() const override { return "create_charts_polygon_annotation"; }

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected Q_SLOTS:
  void moveExtraHandle(const QVariant &data, double dx, double dy);

 protected:
  void init();

  EditHandles *editHandles() const override;
};

//---

/*!
 * \brief polyline annotation
 * \ingroup Charts
 *
 * TODO: draw points and/or line
 *
 * Stroked polyline. Lines can be smoothed
 */
class CQChartsPolylineAnnotation : public CQChartsPolyShapeAnnotationBase {
  Q_OBJECT

  Q_PROPERTY(PointsType pointsType READ pointsType WRITE setPointsType)

  Q_ENUMS(PointsType)

 public:
  enum class PointsType {
    CONTIGUOUS,
    LINE_PAIRS
  };

 public:
  CQChartsPolylineAnnotation(View *view, const Polygon &polygon=Polygon());
  CQChartsPolylineAnnotation(Plot *plot, const Polygon &polygon=Polygon());

  virtual ~CQChartsPolylineAnnotation();

  //---

  const char *typeName() const override { return "polyline"; }

  const char *propertyName() const override { return "polylineAnnotation"; }

  const char *cmdName() const override { return "create_charts_polyline_annotation"; }

  //---

  const PointsType &pointsType() const { return pointsType_; }
  void setPointsType(const PointsType &v) { pointsType_ = v; }

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected Q_SLOTS:
  void moveExtraHandle(const QVariant &data, double dx, double dy);

 protected:
  void init();

  EditHandles *editHandles() const override;

 private:
  PointsType pointsType_ { PointsType::CONTIGUOUS };
};

//---

/*!
 * \brief text annotation
 * \ingroup Charts
 *
 * Formatted text in optionally filled and/or stroked box.
 * Text can be drawn at any angle.
 */
class CQChartsTextAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsOptPosition position  READ position  WRITE setPosition )
  Q_PROPERTY(CQChartsOptRect     rectangle READ rectangle WRITE setRectangle)
  Q_PROPERTY(CQChartsObjRef      objRef    READ objRef    WRITE setObjRef   )

 public:
  using Rect        = CQChartsRect;
  using OptRect     = CQChartsOptRect;
  using OptPosition = CQChartsOptPosition;

 public:
  CQChartsTextAnnotation(View *view, const ObjRefPos &p=ObjRefPos(), const QString &text=QString());
  CQChartsTextAnnotation(Plot *plot, const ObjRefPos &p=ObjRefPos(), const QString &text=QString());

  CQChartsTextAnnotation(View *view, const Rect &r, const QString &text=QString());
  CQChartsTextAnnotation(Plot *plot, const Rect &r, const QString &text=QString());

  virtual ~CQChartsTextAnnotation();

  //---

  const char *typeName() const override { return "text"; }

  const char *propertyName() const override { return "textAnnotation"; }

  const char *cmdName() const override { return "create_charts_text_annotation"; }

  SubType subType() const override { return SubType::TEXT; }

  bool hasMargin() const override { return true; }
  bool hasPadding() const override { return true; }

  //---

  //! get/set position
  const OptPosition &position() const { return position_; }
  void setPosition(const OptPosition &p);

  Position positionValue() const;
  void setPosition(const Position &p);

  const OptRect &rectangle() const { return rectangle_; }
  void setRectangle(const OptRect &r);

  Rect rectangleValue() const;
  void setRectangle(const Rect &r);

  //---

  //! get/set object reference
  const ObjRef &objRef() const { return objRef_; }
  void setObjRef(const ObjRef &o) { objRef_ = o; }

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void drawInRect(PaintDevice *device, const BBox &rect);

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

  //---

  void initRectangle() override;

 protected:
  void init(const QString &text);

  void calcTextSize(Size &psize, Size &wsize) const;

  void positionToLL(double w, double h, double &x, double &y) const;

  void rectToBBox();

  void positionToBBox();

 protected:
  OptPosition position_;  //!< text position
  OptRect     rectangle_; //!< text bounding rect
  ObjRef      objRef_;    //!< reference object
};

//---

/*!
 * \brief image annotation
 * \ingroup Charts
 *
 * Image in rectangle. Annotation can be moved or resized using position
 * or rectangle value. Resized annotation resizes image dynamically.
 */
class CQChartsImageAnnotation : public CQChartsShapeAnnotationBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsOptPosition position      READ position      WRITE setPosition     )
  Q_PROPERTY(CQChartsOptRect     rectangle     READ rectangle     WRITE setRectangle    )
  Q_PROPERTY(CQChartsImage       image         READ image         WRITE setImage        )
  Q_PROPERTY(CQChartsImage       disabledImage READ disabledImage WRITE setDisabledImage)

 public:
  using Image       = CQChartsImage;
  using Rect        = CQChartsRect;
  using OptRect     = CQChartsOptRect;
  using OptPosition = CQChartsOptPosition;

 public:
  CQChartsImageAnnotation(View *view, const ObjRefPos &p=ObjRefPos(), const Image &image=Image());
  CQChartsImageAnnotation(Plot *plot, const ObjRefPos &p=ObjRefPos(), const Image &image=Image());

  CQChartsImageAnnotation(View *view, const Rect &r, const Image &image=Image());
  CQChartsImageAnnotation(Plot *plot, const Rect &r, const Image &image=Image());

  virtual ~CQChartsImageAnnotation();

  //---

  const char *typeName() const override { return "image"; }

  const char *propertyName() const override { return "imageAnnotation"; }

  const char *cmdName() const override { return "create_charts_image_annotation"; }

  bool hasMargin() const override { return true; }
  bool hasPadding() const override { return true; }

  //---

  //! get/set position
  const OptPosition &position() const { return position_; }
  void setPosition(const OptPosition &p);

  Position positionValue() const;
  void setPosition(const Position &p);

  const OptRect &rectangle() const { return rectangle_; }
  void setRectangle(const OptRect &r);

  Rect rectangleValue() const;
  void setRectangle(const Rect &r);

  //---

  const Image &image() const { return image_; }
  void setImage(const Image &image);

  const Image &disabledImage() const { return disabledImage_; }
  void setDisabledImage(const Image &image);

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

  //---

  void initRectangle() override;

 protected:
  enum class DisabledImageType {
    NONE,
    DISABLED,
    UNCHECKED,
    FIXED
  };

  void init();

  void calcImageSize(Size &psize, Size &wsize) const;

  void updateDisabledImage(const DisabledImageType &type);

  void positionToLL(double w, double h, double &x, double &y) const;

  void rectToBBox();

  void positionToBBox();

 protected:
  OptPosition       position_;                                      //!< image position
  OptRect           rectangle_;                                     //!< image bounding rectangle
  Image             image_;                                         //!< image
  Image             disabledImage_;                                 //!< disabled image
  DisabledImageType disabledImageType_ { DisabledImageType::NONE }; //!< disabled image type
};

//---

/*!
 * \brief path annotation (SVG path definition)
 * \ingroup Charts
 *
 * Path shape
 */
class CQChartsPathAnnotation : public CQChartsShapeAnnotationBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsPath path READ path WRITE setPath)

 public:
  using Path = CQChartsPath;

 public:
  CQChartsPathAnnotation(View *view, const Path &path=Path());
  CQChartsPathAnnotation(Plot *plot, const Path &path=Path());

  virtual ~CQChartsPathAnnotation();

  //---

  const char *typeName() const override { return "path"; }

  const char *propertyName() const override { return "pathAnnotation"; }

  const char *cmdName() const override { return "create_charts_path_annotation"; }

  //---

  const Path &path() const { return path_; }
  void setPath(const Path &path);

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  void flip(Qt::Orientation orient) override;

  //---

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

  void drawEditHandles(PaintDevice *device) const override;

 protected Q_SLOTS:
  void moveExtraHandle(const QVariant &data, double dx, double dy);

 protected:
  void init();

  EditHandles *editHandles() const override;

 protected:
  Path path_; //!< path
};

//---

/*!
 * \brief base class for connector annotations
 * \ingroup Charts
 *
 * Defines start and end object references
 */
class CQChartsConnectorAnnotationBase : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsObjRef startObjRef READ startObjRef WRITE setStartObjRef)
  Q_PROPERTY(CQChartsObjRef endObjRef   READ endObjRef   WRITE setEndObjRef  )

 public:
  CQChartsConnectorAnnotationBase(View *view, Type type);
  CQChartsConnectorAnnotationBase(Plot *plot, Type type);

  virtual ~CQChartsConnectorAnnotationBase();

  //---

  SubType subType() const override { return SubType::CONNECTOR; }

  //---

  //! get/set start object reference
  const ObjRef &startObjRef() const { return startObjRef_; }
  void setStartObjRef(const ObjRef &o) { startObjRef_ = o; }

  //! get/set end object reference
  const ObjRef &endObjRef() const { return endObjRef_; }
  void setEndObjRef(const ObjRef &o) { endObjRef_ = o; }

 protected:
  ObjRef startObjRef_; //!< start reference object
  ObjRef endObjRef_;   //!< end reference object
};

//---

class CQChartsArrow;

/*!
 * \brief arrow annotation
 * \ingroup Charts
 *
 * Arrow with custom end point arrows on a filled and/or stroked line
 */
class CQChartsArrowAnnotation : public CQChartsConnectorAnnotationBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition start READ start WRITE setStart)
  Q_PROPERTY(CQChartsPosition end   READ end   WRITE setEnd  )
  Q_PROPERTY(CQChartsPath     path  READ path  WRITE setPath )

 public:
  using ArrowData = CQChartsArrowData;
  using Path      = CQChartsPath;

 public:
  CQChartsArrowAnnotation(View *view, const ObjRefPos &start=ObjRefPos::plot(Point(0, 0)),
                          const ObjRefPos &end=ObjRefPos::plot(Point(1, 1)));
  CQChartsArrowAnnotation(Plot *plot, const ObjRefPos &start=ObjRefPos::plot(Point(0, 0)),
                          const ObjRefPos &end=ObjRefPos::plot(Point(1, 1)));

  virtual ~CQChartsArrowAnnotation();

  //---

  const char *typeName() const override { return "arrow"; }

  const char *propertyName() const override { return "arrowAnnotation"; }

  const char *cmdName() const override { return "create_charts_arrow_annotation"; }

  bool hasMargin() const override { return true; }
  bool hasPadding() const override { return true; }

  //---

  const Position &start() const { return start_; }
  void setStart(const Position &p);

  const Position &end() const { return end_; }
  void setEnd(const Position &p);

  //---

  const Path &path() const { return path_; }
  void setPath(const Path &path);

  //---

  CQChartsArrow *arrow() const { return arrow_.get(); }

  const ArrowData &arrowData() const;
  void setArrowData(const ArrowData &data);

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  void getPropertyNames(QStringList &names, bool hidden) const override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  void flip(Qt::Orientation orient) override;

  //---

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected Q_SLOTS:
  void moveExtraHandle(const QVariant &data, double dx, double dy);

 protected:
  void init();

  EditHandles *editHandles() const override;

  void calcBBox();

  void calcStartEnd(Point &start, Point &end) const;

 protected:
  using ArrowP = std::unique_ptr<CQChartsArrow>;

  Position     start_ { Position::plot(Point(0, 0)) }; //!< arrow start
  Position     end_   { Position::plot(Point(1, 1)) }; //!< arrow end
  ArrowP       arrow_;                                 //!< arrow data
  Path         path_;                                  //!< path
  QPainterPath drawPath_;                              //!< draw path
};

//---

/*!
 * \brief arc annotation
 * \ingroup Charts
 *
 * Connector (Arc or Lines with Arrows) between two rectangles/objects
 */
class CQChartsArcAnnotation : public CQChartsConnectorAnnotationBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition start     READ start     WRITE setStart    )
  Q_PROPERTY(CQChartsPosition end       READ end       WRITE setEnd      )
  Q_PROPERTY(bool             isSolid   READ isSolid   WRITE setSolid    )
  Q_PROPERTY(EdgeType         edgeType  READ edgeType  WRITE setEdgeType )
  Q_PROPERTY(HeadType         frontType READ frontType WRITE setFrontType)
  Q_PROPERTY(HeadType         tailType  READ tailType  WRITE setTailType )
  Q_PROPERTY(HeadType         midType   READ midType   WRITE setMidType  )
  Q_PROPERTY(CQChartsLength   lineWidth READ lineWidth WRITE setLineWidth)
  Q_PROPERTY(double           arrowSize READ arrowSize WRITE setArrowSize)

  Q_ENUMS(EdgeType)
  Q_ENUMS(HeadType)

 public:
  enum class EdgeType {
    ARC         = int(CQChartsDrawUtil::EdgeType::ARC),         // arc edge
    RECTILINEAR = int(CQChartsDrawUtil::EdgeType::RECTILINEAR), // rectilinear edge
    LINE        = int(CQChartsDrawUtil::EdgeType::LINE)         // line edge
  };

  using ArrowData = CQChartsArrowData;

  enum class HeadType {
    NONE     = int(ArrowData::HeadType::NONE),
    ARROW    = int(ArrowData::HeadType::ARROW),
    TRIANGLE = int(ArrowData::HeadType::TRIANGLE),
    STEALTH  = int(ArrowData::HeadType::STEALTH),
    DIAMOND  = int(ArrowData::HeadType::DIAMOND)
  };

  using Length   = CQChartsLength;

 public:
  CQChartsArcAnnotation(View *view, const ObjRefPos &start=ObjRefPos::plot(Point(0, 0)),
                        const ObjRefPos &end=ObjRefPos::plot(Point(1, 1)));
  CQChartsArcAnnotation(Plot *plot, const ObjRefPos &start=ObjRefPos::plot(Point(0, 0)),
                        const ObjRefPos &end=ObjRefPos::plot(Point(1, 1)));

  virtual ~CQChartsArcAnnotation();

  //---

  const char *typeName() const override { return "arc"; }

  const char *propertyName() const override { return "arcAnnotation"; }

  const char *cmdName() const override { return "create_charts_arc_annotation"; }

  //---

  //! get/set start rectangle
  const Position &start() const { return start_; }
  void setStart(const Position &p);

  //! get/set end rectangle
  const Position &end() const { return end_; }
  void setEnd(const Position &p);

  //---

  //! get/set is solid line
  bool isSolid() const { return isSolid_; }
  void setSolid(bool b);

  //! get/set edge line type
  const EdgeType &edgeType() const { return edgeType_; }
  void setEdgeType(const EdgeType &type);

  //---

  //! get/set front arrow head type
  const HeadType &frontType() const { return frontType_; }
  void setFrontType(const HeadType &type);

  //! get/set draw tail arrow head type
  const HeadType &tailType() const { return tailType_; }
  void setTailType(const HeadType &type);

  //! get/set draw mid arrow head type
  const HeadType &midType() const { return midType_; }
  void setMidType(const HeadType &type);

  //! get/set line width
  const Length &lineWidth() const { return lineWidth_; }
  void setLineWidth(const Length &l);

  //! get/set arrow size
  double arrowSize() const { return arrowSize_; }
  void setArrowSize(double r);

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  void getPropertyNames(QStringList &names, bool hidden) const override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  //---

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void init();

  void calcPath(PaintDevice *device, QPainterPath &path) const;

  void setArrowData(ArrowData &arrowData) const;

  EditHandles *editHandles() const override;

 protected:
  Position start_     { CQChartsPosition::plot(Point(0, 0)) }; //!< arc start
  Position end_       { CQChartsPosition::plot(Point(1, 1)) }; //!< arc end
  bool     isSolid_   { true };                                //!< is solid line
  EdgeType edgeType_  { EdgeType::ARC };                       //!< is rectilinear
  HeadType frontType_ { HeadType::NONE };                      //!< front head type
  HeadType tailType_  { HeadType::NONE };                      //!< tail head type
  HeadType midType_   { HeadType::NONE };                      //!< mid head type
  Length   lineWidth_ { Length::pixel(16) };                   //!< line width
  double   arrowSize_ { 2.0 };                                 //!< arrow size
};

//---

/*!
 * \brief arc connector annotation
 * \ingroup Charts
 *
 * Connector (Arc or Lines with Arrows) between two rectangles/objects
 */
class CQChartsArcConnectorAnnotation : public CQChartsConnectorAnnotationBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition center         READ center         WRITE setCenter        )
  Q_PROPERTY(CQChartsLength   radius         READ radius         WRITE setRadius        )
  Q_PROPERTY(CQChartsAngle    srcStartAngle  READ srcStartAngle  WRITE setSrcStartAngle )
  Q_PROPERTY(CQChartsAngle    srcSpanAngle   READ srcSpanAngle   WRITE setSrcSpanAngle  )
  Q_PROPERTY(CQChartsAngle    destStartAngle READ destStartAngle WRITE setDestStartAngle)
  Q_PROPERTY(CQChartsAngle    destSpanAngle  READ destSpanAngle  WRITE setDestSpanAngle )
  Q_PROPERTY(bool             self           READ isSelf         WRITE setSelf          )

  Q_ENUMS(HeadType)

 public:
  using Length = CQChartsLength;
  using Angle  = CQChartsAngle;

 public:
  CQChartsArcConnectorAnnotation(Plot *plot, const ObjRefPos &center=ObjRefPos::plot(Point(0, 0)),
                                 const Length &radius=Length::plot(1),
                                 const Angle &srcStartAngle=Angle(0.0),
                                 const Angle &srcSpanAngle=Angle(90.0),
                                 const Angle &destStartAngle=Angle(90.0),
                                 const Angle &destSpanAngle=Angle(180.0),
                                 bool self=false);

  virtual ~CQChartsArcConnectorAnnotation();

  //---

  const char *typeName() const override { return "arc_connector"; }

  const char *propertyName() const override { return "arcConnectorAnnotation"; }

  const char *cmdName() const override { return "create_charts_arc_connector_annotation"; }

  //---

  //! get/set center
  const Position &center() const { return center_; }
  void setCenter(const Position &c);

  //! get/set radius
  const Length &radius() const { return radius_; }
  void setRadius(const Length &r);

  //! get/set source start angle
  const Angle &srcStartAngle() const { return srcStartAngle_; }
  void setSrcStartAngle(const Angle &a);

  //! get/set source span angle
  const Angle &srcSpanAngle() const { return srcSpanAngle_; }
  void setSrcSpanAngle(const Angle &a);

  //! get/set destination start angle
  const Angle &destStartAngle() const { return destStartAngle_; }
  void setDestStartAngle(const Angle &a);

  //! get/set destination span angle
  const Angle &destSpanAngle() const { return destSpanAngle_; }
  void setDestSpanAngle(const Angle &a);

  //! get/set connects to self
  bool isSelf() const { return self_; }
  void setSelf(bool b);

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  void getPropertyNames(QStringList &names, bool hidden) const override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  //---

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void init();

  void calcPath(QPainterPath &path) const;

 protected:
  Position center_;                   //!< arc center
  Length   radius_;                   //!< arc radius
  Angle    srcStartAngle_;            //!< source start angle
  Angle    srcSpanAngle_;             //!< source span angle
  Angle    destStartAngle_;           //!< destination start angle
  Angle    destSpanAngle_;            //!< destination span angle
  bool     self_           { false }; //!< is self
};

//---

/*!
 * \brief point annotation
 * \ingroup Charts
 *
 * Symbol drawn at point.
 *   + Annotation can be moved.
 *   + Point Symbol can modified (type, size, fill, stroke)
 *   + Can placed relative to an object
 */
class CQChartsPointAnnotation : public CQChartsAnnotation,
 public CQChartsObjPointData<CQChartsPointAnnotation> {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition position  READ position  WRITE setPosition )
  Q_PROPERTY(ShapeType        shapeType READ shapeType WRITE setShapeType)
  Q_PROPERTY(CQChartsObjRef   objRef    READ objRef    WRITE setObjRef   )

  CQCHARTS_POINT_DATA_PROPERTIES

  Q_ENUMS(ShapeType)

 public:
  enum class ShapeType {
    SYMBOL,
    CORNER_HANDLE,
    RESIZE_HANDLE,
    EXTRA_HANDLE
  };

  using Symbol     = CQChartsSymbol;
  using SymbolType = CQChartsSymbolType;
  using PenBrush   = CQChartsPenBrush;
  using PenData    = CQChartsPenData;
  using BrushData  = CQChartsBrushData;
  using ColorInd   = CQChartsUtil::ColorInd;

 public:
  CQChartsPointAnnotation(View *view, const ObjRefPos &p=ObjRefPos(),
                          const Symbol &symbol=Symbol::circle());
  CQChartsPointAnnotation(Plot *plot, const ObjRefPos &p=ObjRefPos(),
                          const Symbol &symbol=Symbol::circle());

  virtual ~CQChartsPointAnnotation();

  //---

  const char *typeName() const override { return "point"; }

  const char *propertyName() const override { return "pointAnnotation"; }

  const char *cmdName() const override { return "create_charts_point_annotation"; }

  SubType subType() const override { return SubType::POINT; }

  bool hasMargin() const override { return true; }
  bool hasPadding() const override { return true; }

  //---

  //! get/set position
  const Position &position() const { return position_; }
  void setPosition(const Position &p);

  //! get/set shape type
  const ShapeType &shapeType() const { return shapeType_; }
  void setShapeType(const ShapeType &t);

  //! get/set object reference
  const ObjRef &objRef() const { return objRef_; }
  void setObjRef(const ObjRef &o);

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void init(const Symbol &symbol);

 protected:
  Position  position_;                        //!< point position
  ShapeType shapeType_ { ShapeType::SYMBOL }; //!< shape type
  ObjRef    objRef_;                          //!< reference object
};

//---

/*!
 * \brief pie slice annotation
 * \ingroup Charts
 *
 * Arc between two angles at a radius with optional inner radius
 */
class CQChartsPieSliceAnnotation : public CQChartsShapeAnnotationBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition position    READ position    WRITE setPosition   )
  Q_PROPERTY(CQChartsLength   innerRadius READ innerRadius WRITE setInnerRadius)
  Q_PROPERTY(CQChartsLength   outerRadius READ outerRadius WRITE setOuterRadius)
  Q_PROPERTY(CQChartsAngle    startAngle  READ startAngle  WRITE setStartAngle )
  Q_PROPERTY(CQChartsAngle    spanAngle   READ spanAngle   WRITE setSpanAngle  )
  Q_PROPERTY(ArcType          arcType     READ arcType     WRITE setArcType    )

  Q_ENUMS(ArcType)

 public:
  enum class ArcType {
    SLICE,
    SEGMENT,
    ARC
  };

  using Length = CQChartsLength;
  using Angle  = CQChartsAngle;

 public:
  CQChartsPieSliceAnnotation(View *view, const ObjRefPos &p=ObjRefPos(),
                             const Length &innerRadius=Length(),
                             const Length &outerRadius=Length(),
                             const Angle &startAngle=Angle(0.0),
                             const Angle &spanAngle=Angle(90.0));
  CQChartsPieSliceAnnotation(Plot *plot, const ObjRefPos &p=ObjRefPos(),
                             const Length &innerRadius=Length(),
                             const Length &outerRadius=Length(),
                             const Angle &startAngle=Angle(0.0),
                             const Angle &spanAngle=Angle(90.0));

  virtual ~CQChartsPieSliceAnnotation();

  //---

  const char *typeName() const override { return "pie_slice"; }

  const char *propertyName() const override { return "pieSliceAnnotation"; }

  const char *cmdName() const override { return "create_charts_pie_slice_annotation"; }

  //---

  //! get/set position
  const Position &position() const { return position_; }
  void setPosition(const Position &p);

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
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void init();

 protected:
  Position position_;                       //!< point position
  Length   innerRadius_;                    //!< inner radius
  Length   outerRadius_;                    //!< outer radius
  Angle    startAngle_  {  0.0 };           //!< start angle
  Angle    spanAngle_   { 90.0 };           //!< span angle
  ArcType  arcType_     { ArcType::SLICE }; //!< arc type
};

//---

/*!
 * \brief axis annotation
 * \ingroup Charts
 */
class CQChartsAxisAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(Qt::Orientation direction READ direction WRITE setDirection)
  Q_PROPERTY(double          position  READ position  WRITE setPosition )
  Q_PROPERTY(double          start     READ start     WRITE setStart    )
  Q_PROPERTY(double          end       READ end       WRITE setEnd      )
  Q_PROPERTY(CQChartsObjRef  objRef    READ objRef    WRITE setObjRef   )

 public:
  using Axis = CQChartsAxis;

 public:
  CQChartsAxisAnnotation(Plot *plot, Qt::Orientation direction=Qt::Horizontal,
                         double start=0.0, double end=1.0);

  virtual ~CQChartsAxisAnnotation();

  //---

  const char *typeName() const override { return "axis"; }

  const char *propertyName() const override { return "axisAnnotation"; }

  const char *cmdName() const override { return "create_charts_axis_annotation"; }

  SubType subType() const override { return SubType::AXIS; }

  //---

  Axis *axis() const { return axis_.get(); }

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

  //! get/set object reference
  const ObjRef &objRef() const { return objRef_; }
  void setObjRef(const ObjRef &o) { objRef_ = o; }

  //---

  void connectAxis(bool b);

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void init();

  void updateAxis();

 protected:
  using AxisP = std::unique_ptr<Axis>;

  Qt::Orientation direction_ { Qt::Horizontal }; //!< direction
  ObjRef          objRef_;                       //!< object ref
  double          position_  { 0.0 };            //!< position
  double          start_     { 0.0 };            //!< start
  double          end_       { 1.0 };            //!< end
  AxisP           axis_;                         //!< axis
};

//---

/*!
 * \brief key annotation
 * \ingroup Charts
 */
class CQChartsKeyAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsObjRef objRef READ objRef WRITE setObjRef)
  Q_PROPERTY(CQChartsColumn column READ column WRITE setColumn)

 public:
  using Column = CQChartsColumn;
  using Key    = CQChartsKey;

 public:
  CQChartsKeyAnnotation(View *plot);
  CQChartsKeyAnnotation(Plot *plot, const Column &column=Column());

  virtual ~CQChartsKeyAnnotation();

  //---

  const char *typeName() const override { return "key"; }

  const char *propertyName() const override { return "keyAnnotation"; }

  const char *cmdName() const override { return "create_charts_key_annotation"; }

  SubType subType() const override { return SubType::KEY; }

  //---

  //! get/set object reference
  const ObjRef &objRef() const { return objRef_; }
  void setObjRef(const ObjRef &o) { objRef_ = o; }

  Column column() const;
  void setColumn(const Column &c);

  //---

  Key *key() const { return key_; }

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  bool editMove(const Point &) override;

  bool inside(const Point &p) const override;

  //---

  //! handle select press
  bool selectPress(const Point &p, SelData &selData) override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void init();

 protected Q_SLOTS:
  void updateLocationSlot();

 protected:
  ObjRef objRef_;              //!< object ref
  Key*   key_     { nullptr }; //!< key
};

//---

class CQChartsContour;

/*!
 * \brief 3d point set annotation
 * \ingroup Charts
 *
 * Set of points draw as symbols, convex hull, best fit line, density gradient or density grid
 *
 * TODO: support column
 */
class CQChartsPoint3DSetAnnotation : public CQChartsShapeAnnotationBase {
  Q_OBJECT

  Q_PROPERTY(DrawType drawType READ drawType WRITE setDrawType)

  Q_ENUMS(DrawType)

 public:
  enum class DrawType {
    SYMBOLS,
    CONTOUR
  };

 public:
  using Point3D = CQChartsGeom::Point3D;
  using Points  = std::vector<Point3D>;

 public:
  CQChartsPoint3DSetAnnotation(View *view, const Points &values=Points());
  CQChartsPoint3DSetAnnotation(Plot *plot, const Points &values=Points());

  virtual ~CQChartsPoint3DSetAnnotation();

  //---

  const char *typeName() const override { return "point3d_set"; }

  const char *propertyName() const override { return "point3DSetAnnotation"; }

  const char *cmdName() const override { return "create_charts_point3d_set_annotation"; }

  //---

  const Points &points() const { return points_; }
  void setValues(const Points &points) { points_ = points; updateValues(); }

  const DrawType &drawType() const { return drawType_; }
  void setDrawType(const DrawType &t) { drawType_ = t; invalidate(); }

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void updateValues();

  void initContour() const;

 protected:
  void init();

 protected:
  using ContourP = std::unique_ptr<CQChartsContour>;

  Points          points_;                         //!< points
  CQChartsRValues xvals_;                          //!< x vals
  CQChartsRValues yvals_;                          //!< y vals
  CQChartsRValues zvals_;                          //!< z vals
  DrawType        drawType_ { DrawType::CONTOUR }; //!< draw type
  ContourP        contour_;                        //!< contour
};

//---

class CQChartsDelaunay;

/*!
 * \brief 2D point set annotation
 * \ingroup Charts
 *
 * Set of points draw as symbols, convex hull, best fit line, density gradient or density grid
 */
class CQChartsPointSetAnnotation : public CQChartsShapeAnnotationBase,
 public CQChartsObjPointData<CQChartsPointSetAnnotation> {
  Q_OBJECT

  Q_PROPERTY(CQChartsRect        rectangle READ rectangle WRITE setRectangle)
  Q_PROPERTY(CQChartsPoints      values    READ values    WRITE setValues   )
  Q_PROPERTY(CQChartsModelColumn xColumn   READ xColumn   WRITE setXColumn  )
  Q_PROPERTY(CQChartsModelColumn yColumn   READ yColumn   WRITE setYColumn  )
  Q_PROPERTY(DrawType            drawType  READ drawType  WRITE setDrawType )

  Q_ENUMS(DrawType)

 public:
  enum class DrawType {
    SYMBOLS,
    HULL,
    BEST_FIT,
    DENSITY,
    GRID,
    DELAUNAY
  };

  using Rect        = CQChartsRect;
  using Points      = CQChartsPoints;
  using ModelColumn = CQChartsModelColumn;
  using PenBrush    = CQChartsPenBrush;
  using PenData     = CQChartsPenData;
  using BrushData   = CQChartsBrushData;
  using Color       = CQChartsColor;
  using ColorInd    = CQChartsUtil::ColorInd;

 public:
  CQChartsPointSetAnnotation(View *view, const Rect &rectangle=Rect(),
                             const Points &values=Points());
  CQChartsPointSetAnnotation(Plot *plot, const Rect &rectangle=Rect(),
                             const Points &values=Points());

  virtual ~CQChartsPointSetAnnotation();

  //---

  const char *typeName() const override { return "point_set"; }

  const char *propertyName() const override { return "pointSetAnnotation"; }

  const char *cmdName() const override { return "create_charts_point_set_annotation"; }

  //---

  const Rect &rectangle() const { return rectangle_; }
  void setRectangle(const Rect &rectangle);

  const Points &values() const { return values_; }
  void setValues(const Points &values);

  const ModelColumn &xColumn() const { return xColumn_; }
  void setXColumn(const ModelColumn &c);

  const ModelColumn &yColumn() const { return yColumn_; }
  void setYColumn(const ModelColumn &c);

  const DrawType &drawType() const { return drawType_; }
  void setDrawType(const DrawType &t) { drawType_ = t; invalidate(); }

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void updateValues();

  void calcValues();

  void drawSymbols (PaintDevice *device);
  void drawHull    (PaintDevice *device);
  void drawDensity (PaintDevice *device);
  void drawBestFit (PaintDevice *device);
  void drawGrid    (PaintDevice *device);
  void drawDelaunay(PaintDevice *device);

 protected:
  void init();

 protected:
  using Hull      = CQChartsGrahamHull;
  using HullP     = std::unique_ptr<Hull>;
  using GridCell  = CQChartsGridCell;
  using GridCellP = std::unique_ptr<GridCell>;
  using Delaunay  = CQChartsDelaunay;
  using DelaunayP = std::unique_ptr<Delaunay>;

  // options
  Rect        rectangle_;                      //!< rectangle
  Points      values_;                         //!< point values
  ModelColumn xColumn_;                        //!< x column
  ModelColumn yColumn_;                        //!< y column
  DrawType    drawType_ { DrawType::SYMBOLS }; //!< draw type

  // data
  RMinMax   xrange_;                 //!< x range
  RMinMax   yrange_;                 //!< y range
  HullP     hull_;                   //!< hull
  bool      hullDirty_     { true }; //!< hull is dirty
  GridCellP gridCell_;               //!< grid cell data
  bool      gridDirty_     { true }; //!< grid is dirty
  DelaunayP delaunay_;               //!< delaunay
  bool      delaunayDirty_ { true }; //!< delaunay is dirty
};

//---

/*!
 * \brief value set annotation
 * \ingroup Charts
 *
 * Set of values draw as statistics or error bar scaled to rectangle
 */
class CQChartsValueSetAnnotation : public CQChartsShapeAnnotationBase {
  Q_OBJECT

  Q_PROPERTY(CQChartsRect        rectangle   READ rectangle   WRITE setRectangle  )
  Q_PROPERTY(CQChartsReals       values      READ values      WRITE setValues     )
  Q_PROPERTY(CQChartsModelColumn modelColumn READ modelColumn WRITE setModelColumn)
  Q_PROPERTY(DrawType            drawType    READ drawType    WRITE setDrawType   )

  Q_ENUMS(DrawType)

 public:
  enum class DrawType {
    BARCHART,
    DENSITY,
    BUBBLE,
    PIE,
    RADAR,
    TREEMAP,
    FACTOR
  };

  using Rect        = CQChartsRect;
  using Reals       = CQChartsReals;
  using ModelColumn = CQChartsModelColumn;

 public:
  CQChartsValueSetAnnotation(View *view, const Rect &rectangle=Rect(),
                             const Reals &values=Reals());
  CQChartsValueSetAnnotation(Plot *plot, const Rect &rectangle=Rect(),
                             const Reals &values=Reals());

  virtual ~CQChartsValueSetAnnotation();

  //---

  const char *typeName() const override { return "value_set"; }

  const char *propertyName() const override { return "valueSetAnnotation"; }

  const char *cmdName() const override { return "create_charts_value_set_annotation"; }

  //---

  const Rect &rectangle() const { return rectangle_; }
  void setRectangle(const Rect &rectangle);

  const Reals &values() const { return reals_; }
  void setValues(const Reals &values);

  const ModelColumn &modelColumn() const { return modelColumn_; }
  void setModelColumn(const ModelColumn &c);

  const DrawType &drawType() const { return drawType_; }
  void setDrawType(const DrawType &t) { drawType_ = t; invalidate(); }

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void animateStep() override;

  //---

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void updateValues();

  void calcReals();

  void drawBarChart(PaintDevice *device);
  void drawBox     (PaintDevice *device);
  void drawBubble  (PaintDevice *device);
  void drawPie     (PaintDevice *device, const QPen &pen);
  void drawRadar   (PaintDevice *device);
  void drawTreeMap (PaintDevice *device, const QPen &pen);
  void drawFactor  (PaintDevice *device);

 protected:
  void init();

 protected:
  using Density     = CQChartsDensity;
  using DensityP    = std::unique_ptr<Density>;
  using CirclePack  = CQChartsCirclePack<CQChartsCircleNode>;
  using CirclePackP = std::unique_ptr<CirclePack>;
  using CircleMgr   = CQChartsCircleMgr;

  // options
  Rect        rectangle_;                        //!< rectangle
  Reals       reals_;                            //!< real values
  ModelColumn modelColumn_;                      //!< model column
  DrawType    drawType_   { DrawType::DENSITY }; //!< draw type

  // data
  DensityP    density_;               //!< density object
  CirclePackP circlePack_;            //!< curcle pack
  CircleMgr*  circleMgr_ { nullptr }; //!< circle mgr
};

//---

/*!
 * \brief button annotation
 * \ingroup Charts
 *
 * Button with text
 */
class CQChartsButtonAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition position READ position WRITE setPosition)
  Q_PROPERTY(CQChartsObjRef   objRef   READ objRef   WRITE setObjRef  )

 public:
  CQChartsButtonAnnotation(View *view, const ObjRefPos &p=ObjRefPos(),
                           const QString &text=QString());
  CQChartsButtonAnnotation(Plot *plot, const ObjRefPos &p=ObjRefPos(),
                           const QString &text=QString());

  virtual ~CQChartsButtonAnnotation();

  //---

  const char *typeName() const override { return "button"; }

  const char *propertyName() const override { return "buttonAnnotation"; }

  const char *cmdName() const override { return "create_charts_button_annotation"; }

  SubType subType() const override { return SubType::WIDGET; }

  //---

  //! get/set position
  const Position &position() const { return position_; }
  void setPosition(const Position &p);

  //! get/set object reference
  const ObjRef &objRef() const { return objRef_; }
  void setObjRef(const ObjRef &o) { objRef_ = o; }

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  bool selectPress  (const Point &w, SelData &) override;
  bool selectMove   (const Point &w) override;
  bool selectRelease(const Point &w) override;

  //---

  bool inside(const Point &p) const override;

  void draw(PaintDevice *device) override;

  //---

  void writeHtml(HtmlPaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

  //---

 protected:
  void init(const QString &text);

  QRect calcPixelRect() const;

 protected:
  Position position_;          //!< button position
  ObjRef   objRef_;            //!< object ref
  QRect    prect_;             //!< pixel rect
  bool     pressed_ { false }; //!< is pressed
};

//---

class CQWinWidget;

/*!
 * \brief widget annotation
 * \ingroup Charts
 *
 * Widget at position or in rectangle
 */
class CQChartsWidgetAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsOptPosition position    READ position      WRITE setPosition   )
  Q_PROPERTY(CQChartsOptRect     rectangle   READ rectangle     WRITE setRectangle  )
  Q_PROPERTY(CQChartsObjRef      objRef      READ objRef        WRITE setObjRef     )
  Q_PROPERTY(CQChartsWidget      widget      READ widget        WRITE setWidget     )
  Q_PROPERTY(Qt::Alignment       align       READ align         WRITE setAlign      )
  Q_PROPERTY(QSizePolicy         sizePolicy  READ sizePolicy    WRITE setSizePolicy )
  Q_PROPERTY(bool                interactive READ isInteractive WRITE setInteractive)

 public:
  using Widget      = CQChartsWidget;
  using Rect        = CQChartsRect;
  using OptRect     = CQChartsOptRect;
  using OptPosition = CQChartsOptPosition;

 public:
  CQChartsWidgetAnnotation(View *view, const ObjRefPos &p=ObjRefPos(),
                           const Widget &widget=Widget());
  CQChartsWidgetAnnotation(Plot *plot, const ObjRefPos &p=ObjRefPos(),
                           const Widget &widget=Widget());

  CQChartsWidgetAnnotation(View *view, const Rect &r, const Widget &widget=Widget());
  CQChartsWidgetAnnotation(Plot *plot, const Rect &r, const Widget &widget=Widget());

  virtual ~CQChartsWidgetAnnotation();

  //---

  const char *typeName() const override { return "widget"; }

  const char *propertyName() const override { return "widgetAnnotation"; }

  const char *cmdName() const override { return "create_charts_widget_annotation"; }

  SubType subType() const override { return SubType::WIDGET; }

  bool hasMargin() const override { return true; }
  bool hasPadding() const override { return true; }

  //---

 public:
  void setVisible(bool b) override;

 private:
  void updateVisible();

  //---

 public:
  //! get/set position
  const OptPosition &position() const { return position_; }
  void setPosition(const OptPosition &p);

  Position positionValue() const;
  void setPosition(const Position &p);

  const OptRect &rectangle() const { return rectangle_; }
  void setRectangle(const OptRect &r);

  Rect rectangleValue() const;
  void setRectangle(const Rect &r);

  //---

  //! get/set object reference
  const ObjRef &objRef() const { return objRef_; }
  void setObjRef(const ObjRef &o) { objRef_ = o; }

  //---

  const Widget &widget() const { return widget_; }
  void setWidget(const Widget &widget);

  const Qt::Alignment &align() const { return align_; }
  void setAlign(const Qt::Alignment &a);

  const QSizePolicy &sizePolicy() const { return sizePolicy_; }
  void setSizePolicy(const QSizePolicy &p);

  bool isInteractive() const { return interactive_; }
  void setInteractive(bool b);

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

  //---

  void initRectangle() override;

  void parentViewChanged() override;

 protected:
  void init();

  void calcWidgetSize(Size &psize, Size &wsize) const;

  void positionToTopLeft(double w, double h, double &x, double &y) const;

  void rectToBBox();

  void positionToBBox();

 protected Q_SLOTS:
  void updateWinGeometry();

 protected:
  OptPosition   position_;                                     //!< widget position
  OptRect       rectangle_;                                    //!< widget bounding rectangle
  ObjRef        objRef_;                                       //!< object ref
  Widget        widget_;                                       //!< widget
  Qt::Alignment align_       { Qt::AlignLeft | Qt::AlignTop }; //!< position alignment
  QSizePolicy   sizePolicy_;                                   //!< size policy
  bool          interactive_ { false };                        //!< is interactive
  CQWinWidget*  winWidget_   { nullptr };                      //!< window frame
};

//---

/*!
 * \brief symbol key annotation
 * \ingroup Charts
 */
class CQChartsSymbolSizeMapKeyAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition position READ position WRITE setPosition)

 public:
  using Key = CQChartsSymbolSizeMapKey;

 public:
  CQChartsSymbolSizeMapKeyAnnotation(Plot *plot);

  virtual ~CQChartsSymbolSizeMapKeyAnnotation();

  //---

  const char *typeName() const override { return "symbolMapKey"; }

  const char *propertyName() const override { return "symbolMapKeyAnnotation"; }

  const char *cmdName() const override { return "create_charts_symbol_map_key_annotation"; }

  SubType subType() const override { return SubType::KEY; }

  //---

  Key *key() const { return key_; }

  //! get/set position
  const Position &position() const { return position_; }
  void setPosition(const Position &p) { position_ = p; }

  //---

  //! add properties
  void addProperties(PropertyModel *model, const QString &path,
                     const QString &desc=QString()) override;

  //---

  void setEditBBox(const BBox &bbox, const ResizeSide &dragSide) override;

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) override;

  void writeDetails(std::ostream &os, const QString &parentVarName=QString(),
                    const QString &varName=QString()) const override;

 protected:
  void init();

 protected Q_SLOTS:
  void updateLocationSlot();

 protected:
  Key*     key_ { nullptr }; //!< key
  Position position_;        //!< position
};

#endif
