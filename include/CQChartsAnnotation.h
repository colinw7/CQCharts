#ifndef CQChartsAnnotation_H
#define CQChartsAnnotation_H

#include <CQChartsTextBoxObj.h>
#include <CQChartsSymbol.h>
#include <CQChartsData.h>
#include <CQChartsOptRect.h>
#include <CQChartsOptPosition.h>
#include <CQChartsPolygon.h>
#include <CQChartsGeom.h>

class CQChartsEditHandles;
class CQPropertyViewItem;

/*!
 * \brief base class for view/plot annotation
 * \ingroup Charts
 */
class CQChartsAnnotation : public CQChartsTextBoxObj {
  Q_OBJECT

  Q_PROPERTY(int  ind       READ ind         WRITE setInd      )
  Q_PROPERTY(bool checkable READ isCheckable WRITE setCheckable)
  Q_PROPERTY(bool checked   READ isChecked   WRITE setChecked  )

 public:
  enum class Type {
    NONE,
    RECT,
    ELLIPSE,
    POLYGON,
    POLYLINE,
    TEXT,
    IMAGE,
    ARROW,
    POINT
  };

  using ColorInd = CQChartsUtil::ColorInd;

 public:
  static const QStringList &typeNames();

 public:
  CQChartsAnnotation(CQChartsView *view, Type type);
  CQChartsAnnotation(CQChartsPlot *plot, Type type);

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

  //! get type and type name
  const Type &type() const { return type_; }

  virtual const char *typeName() const = 0;

  //---

  // checkable/checked
  bool isCheckable() const { return checkable_; }
  void setCheckable(bool b);

  bool isChecked() const { return checked_; }
  void setChecked(bool b);

  //---

  //! get bounding box
  const CQChartsGeom::BBox &bbox() const { return bbox_; }

  //! get edit handles
  CQChartsEditHandles *editHandles() { return editHandles_; }

  //---

  //! get property path
  virtual QString propertyId() const = 0;

  //! add properties
  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc="") override;

  //! add stroke and fill properties
  void addStrokeFillProperties(CQPropertyViewModel *model, const QString &path);

  //! add stroke properties
  void addStrokeProperties(CQPropertyViewModel *model, const QString &path);

  //! add fill properties
  void addFillProperties(CQPropertyViewModel *model, const QString &path);

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

  CQPropertyViewModel *propertyModel() const;

  const CQPropertyViewItem *propertyItem(const QString &name, bool hidden=false) const;

  //---

  //! is point inside (also checks visible)
  bool contains(const CQChartsGeom::Point &p) const;

  //! is point inside
  virtual bool inside(const CQChartsGeom::Point &p) const;

  //---

  //! interp color
  QColor interpColor(const CQChartsColor &c, const ColorInd &ind) const;

  //---

  //! handle select press
  virtual bool selectPress(const CQChartsGeom::Point &);

  //! handle edit press, move, motion, release
  virtual bool editPress  (const CQChartsGeom::Point &);
  virtual bool editMove   (const CQChartsGeom::Point &);
  virtual bool editMotion (const CQChartsGeom::Point &);
  virtual bool editRelease(const CQChartsGeom::Point &);

  //! handle edit move by
  virtual void editMoveBy(const QPointF &d);

  //! set new bounding box
  virtual void setBBox(const CQChartsGeom::BBox &, const CQChartsResizeSide &) { }

  //---

  //! handle box obj data changed
  void boxDataInvalidate() override { emit dataChanged(); }

  void invalidate();

  //---

  //! draw
  virtual void draw(CQChartsPaintDevice *device);

  //! draw edit handles
  void drawEditHandles(QPainter *painter) const;

  //---

  //! write details (command to recreate)
  virtual void write(std::ostream &os, const QString &parentVarName="",
                     const QString &varName="") const = 0;

  //! write key values
  void writeKeys(std::ostream &os, const QString &cmd, const QString &parentVarName="",
                 const QString &varName="") const;

#if 0
  //! write fill, stroke values
  void writeFill  (std::ostream &os) const;
  void writeStroke(std::ostream &os) const;
#endif

  //! write polygon points
  void writePoints(std::ostream &os, const QPolygonF &polygon) const;

  //! write properties
  void writeProperties(std::ostream &os, const QString &varName="") const;

  //---

  //! initialize state when first resized to explicit rectangle in edit
  virtual void initRectangle();

 signals:
  //! emitted when data changed
  void dataChanged();

 protected:
  Type                 type_        { Type::NONE }; //!< type
  int                  ind_         { 0 };          //!< unique ind
  bool                 checkable_   { false };      //!< is checkable
  bool                 checked_     { false };      //!< is checked
  CQChartsGeom::BBox   bbox_;                       //!< bbox (plot coords)
  CQChartsEditHandles* editHandles_ { nullptr };    //!< edit handles
};

//---

/*!
 * \brief rectangle annotation
 * \ingroup Charts
 */
class CQChartsRectangleAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsRect     rectangle READ rectangle WRITE setRectangle)
  Q_PROPERTY(CQChartsPosition start     READ start     WRITE setStart    )
  Q_PROPERTY(CQChartsPosition end       READ end       WRITE setEnd      )

 public:
  CQChartsRectangleAnnotation(CQChartsView *view, const CQChartsRect &rect=CQChartsRect());
  CQChartsRectangleAnnotation(CQChartsPlot *plot, const CQChartsRect &rect=CQChartsRect());

  virtual ~CQChartsRectangleAnnotation();

  const char *typeName() const override { return "rectangle"; }

  const CQChartsRect &rectangle() const { return rectangle_; }
  void setRectangle(const CQChartsRect &rectangle);

  void setRectangle(const CQChartsPosition &start, const CQChartsPosition &end);

  CQChartsPosition start() const;
  void setStart(const CQChartsPosition &p);

  CQChartsPosition end() const;
  void setEnd(const CQChartsPosition &p);

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc="") override;

  QString propertyId() const override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &dragSide) override;

  void draw(CQChartsPaintDevice *device) override;

  void write(std::ostream &os, const QString &parentVarName="",
             const QString &varName="") const override;

 private:
  CQChartsRect rectangle_; //!< rectangle
};

//---

/*!
 * \brief ellipse annotation
 * \ingroup Charts
 */
class CQChartsEllipseAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition center  READ center  WRITE setCenter )
  Q_PROPERTY(CQChartsLength   xRadius READ xRadius WRITE setXRadius)
  Q_PROPERTY(CQChartsLength   yRadius READ yRadius WRITE setYRadius)

 public:
  CQChartsEllipseAnnotation(CQChartsView *view, const CQChartsPosition &center=CQChartsPosition(),
                            const CQChartsLength &xRadius=1.0, const CQChartsLength &yRadius=1.0);
  CQChartsEllipseAnnotation(CQChartsPlot *plot, const CQChartsPosition &center=CQChartsPosition(),
                            const CQChartsLength &xRadius=1.0, const CQChartsLength &yRadius=1.0);

  virtual ~CQChartsEllipseAnnotation();

  const char *typeName() const override { return "ellipse"; }

  const CQChartsPosition &center() const { return center_; }
  void setCenter(const CQChartsPosition &c) { center_ = c; emit dataChanged(); }

  const CQChartsLength &xRadius() const { return xRadius_; }
  void setXRadius(const CQChartsLength &r) { xRadius_ = r; }

  const CQChartsLength &yRadius() const { return yRadius_; }
  void setYRadius(const CQChartsLength &r) { yRadius_ = r; }

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc="") override;

  QString propertyId() const override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &dragSide) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(CQChartsPaintDevice *device) override;

  void write(std::ostream &os, const QString &parentVarName="",
             const QString &varName="") const override;

 private:
  CQChartsPosition center_;          //!< ellipse center
  CQChartsLength   xRadius_ { 1.0 }; //!< ellipse x radius
  CQChartsLength   yRadius_ { 1.0 }; //!< ellipse y radius
};

//---

/*!
 * \brief polygon annotation
 * \ingroup Charts
 */
class CQChartsPolygonAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsPolygon polygon READ polygon WRITE setPolygon)

 public:
  CQChartsPolygonAnnotation(CQChartsView *view, const CQChartsPolygon &polygon);
  CQChartsPolygonAnnotation(CQChartsPlot *plot, const CQChartsPolygon &polygon);

  virtual ~CQChartsPolygonAnnotation();

  const char *typeName() const override { return "polygon"; }

  const CQChartsPolygon &polygon() const { return polygon_; }
  void setPolygon(const CQChartsPolygon &polygon) { polygon_ = polygon; emit dataChanged(); }

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc="") override;

  QString propertyId() const override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &dragSide) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(CQChartsPaintDevice *device) override;

  void write(std::ostream &os, const QString &parentVarName="",
             const QString &varName="") const override;

 private:
  CQChartsPolygon polygon_; //!< polygon points
};

//---

/*!
 * \brief polyline annotation
 * \ingroup Charts
 */
class CQChartsPolylineAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsPolygon polygon READ polygon WRITE setPolygon)

 public:
  CQChartsPolylineAnnotation(CQChartsView *view, const CQChartsPolygon &polygon);
  CQChartsPolylineAnnotation(CQChartsPlot *plot, const CQChartsPolygon &polygon);

  virtual ~CQChartsPolylineAnnotation();

  const char *typeName() const override { return "polyline"; }

  const CQChartsPolygon &polygon() const { return polygon_; }
  void setPolygon(const CQChartsPolygon &polygon) { polygon_ = polygon; emit dataChanged(); }

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc="") override;

  QString propertyId() const override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &dragSide) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(CQChartsPaintDevice *device) override;

  void write(std::ostream &os, const QString &parentVarName="",
             const QString &varName="") const override;

 private:
  CQChartsPolygon polygon_; //!< polyline points
};

//---

/*!
 * \brief text annotation
 * \ingroup Charts
 */
class CQChartsTextAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsOptPosition position  READ position  WRITE setPosition )
  Q_PROPERTY(CQChartsOptRect     rectangle READ rectangle WRITE setRectangle)

 public:
  CQChartsTextAnnotation(CQChartsView *view, const CQChartsPosition &p=CQChartsPosition(),
                         const QString &text="");
  CQChartsTextAnnotation(CQChartsPlot *plot, const CQChartsPosition &p=CQChartsPosition(),
                         const QString &text="");

  CQChartsTextAnnotation(CQChartsView *view, const CQChartsRect &r=CQChartsRect(),
                         const QString &text="");
  CQChartsTextAnnotation(CQChartsPlot *plot, const CQChartsRect &r=CQChartsRect(),
                         const QString &text="");

  virtual ~CQChartsTextAnnotation();

  const char *typeName() const override { return "text"; }

  const CQChartsOptPosition &position() const { return position_; }
  void setPosition(const CQChartsOptPosition &p);

  CQChartsPosition positionValue() const;
  void setPosition(const CQChartsPosition &p);

  const CQChartsOptRect &rectangle() const { return rectangle_; }
  void setRectangle(const CQChartsOptRect &r);

  CQChartsRect rectangleValue() const;
  void setRectangle(const CQChartsRect &r);

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc="") override;

  QString propertyId() const override;

  //---

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &dragSide) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(CQChartsPaintDevice *device) override;

  void write(std::ostream &os, const QString &parentVarName="",
             const QString &varName="") const override;

  //---

  void initRectangle() override;

 private:
  void init(const QString &text);

  void calcTextSize(QSizeF &psize, QSizeF &wsize) const;

  void positionToLL(double w, double h, double &x, double &y) const;

  void rectToBBox();

  void positionToBBox();

 private:
  CQChartsOptPosition position_;  //!< text position
  CQChartsOptRect     rectangle_; //!< text bounding rect
};

//---

/*!
 * \brief image annotation
 * \ingroup Charts
 */
class CQChartsImageAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsOptPosition position  READ position  WRITE setPosition )
  Q_PROPERTY(CQChartsOptRect     rectangle READ rectangle WRITE setRectangle)

 public:
  CQChartsImageAnnotation(CQChartsView *view, const CQChartsPosition &p=CQChartsPosition(),
                         const QImage &image=QImage());
  CQChartsImageAnnotation(CQChartsPlot *plot, const CQChartsPosition &p=CQChartsPosition(),
                         const QImage &image=QImage());

  CQChartsImageAnnotation(CQChartsView *view, const CQChartsRect &r=CQChartsRect(),
                         const QImage &image=QImage());
  CQChartsImageAnnotation(CQChartsPlot *plot, const CQChartsRect &r=CQChartsRect(),
                         const QImage &image=QImage());

  virtual ~CQChartsImageAnnotation();

  const char *typeName() const override { return "image"; }

  const CQChartsOptPosition &position() const { return position_; }
  void setPosition(const CQChartsOptPosition &p);

  CQChartsPosition positionValue() const;
  void setPosition(const CQChartsPosition &p);

  const CQChartsOptRect &rectangle() const { return rectangle_; }
  void setRectangle(const CQChartsOptRect &r);

  CQChartsRect rectangleValue() const;
  void setRectangle(const CQChartsRect &r);

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc="") override;

  QString propertyId() const override;

  //---

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &dragSide) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(CQChartsPaintDevice *device) override;

  void write(std::ostream &os, const QString &parentVarName="",
             const QString &varName="") const override;

  //---

  void initRectangle() override;

 private:
  void init(const QImage &image);

  void calcImageSize(QSizeF &psize, QSizeF &wsize) const;

  void positionToLL(double w, double h, double &x, double &y) const;

  void rectToBBox();

  void positionToBBox();

 private:
  CQChartsOptPosition position_;      //!< image position
  CQChartsOptRect     rectangle_;     //!< image bounding rectangle
  QImage              image_;         //!< image
  QImage              disabledImage_; //!< disabled image
};

//---

class CQChartsArrow;

/*!
 * \brief arrow annotation
 * \ingroup Charts
 */
class CQChartsArrowAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition start READ start WRITE setStart)
  Q_PROPERTY(CQChartsPosition end   READ end   WRITE setEnd  )

 public:
  CQChartsArrowAnnotation(CQChartsView *view, const CQChartsPosition &start=CQChartsPosition(),
                          const CQChartsPosition &end=CQChartsPosition(QPointF(1, 1)));
  CQChartsArrowAnnotation(CQChartsPlot *plot, const CQChartsPosition &start=CQChartsPosition(),
                          const CQChartsPosition &end=CQChartsPosition(QPointF(1, 1)));

  virtual ~CQChartsArrowAnnotation();

  const char *typeName() const override { return "arrow"; }

  const CQChartsPosition &start() const { return start_; }
  void setStart(const CQChartsPosition &p) { start_ = p; emit dataChanged(); }

  const CQChartsPosition &end() const { return end_; }
  void setEnd(const CQChartsPosition &p) { end_ = p; emit dataChanged(); }

  CQChartsArrow *arrow() const { return arrow_.get(); }

  const CQChartsArrowData &arrowData() const;
  void setArrowData(const CQChartsArrowData &data);

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc="") override;

  void getPropertyNames(QStringList &names, bool hidden) const override;

  QString propertyId() const override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &dragSide) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(CQChartsPaintDevice *device) override;

  void write(std::ostream &os, const QString &parentVarName="",
             const QString &varName="") const override;

 private:
  using ArrowP = std::unique_ptr<CQChartsArrow>;

  CQChartsPosition start_ { QPointF(0, 0) }; //!< arrow start
  CQChartsPosition end_   { QPointF(1, 1) }; //!< arrow end
  ArrowP           arrow_;                   //!< arrow data
};

//---

/*!
 * \brief point annotation
 * \ingroup Charts
 */
class CQChartsPointAnnotation : public CQChartsAnnotation,
 public CQChartsObjPointData<CQChartsPointAnnotation> {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition position READ position WRITE setPosition)

  CQCHARTS_POINT_DATA_PROPERTIES

 public:
  CQChartsPointAnnotation(CQChartsView *view, const CQChartsPosition &p=CQChartsPosition(),
                          const CQChartsSymbol &type=CQChartsSymbol::Type::CIRCLE);
  CQChartsPointAnnotation(CQChartsPlot *plot, const CQChartsPosition &p=CQChartsPosition(),
                          const CQChartsSymbol &type=CQChartsSymbol::Type::CIRCLE);

  virtual ~CQChartsPointAnnotation();

  const char *typeName() const override { return "point"; }

  const CQChartsPosition &position() const { return position_; }
  void setPosition(const CQChartsPosition &p) { position_ = p; emit dataChanged(); }

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc="") override;

  QString propertyId() const override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeSide &dragSide) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(CQChartsPaintDevice *device) override;

  void write(std::ostream &os, const QString &parentVarName="",
             const QString &varName="") const override;

 private:
  CQChartsPosition position_; //!< point position
};

#endif
