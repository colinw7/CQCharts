#ifndef CQChartsAnnotation_H
#define CQChartsAnnotation_H

#include <CQChartsTextBoxObj.h>
#include <CQChartsEditHandles.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsData.h>
#include <CQChartsRect.h>
#include <CQChartsPolygon.h>
#include <CQChartsGeom.h>
#include <QFont>

class CQChartsAnnotation : public CQChartsTextBoxObj {
  Q_OBJECT

  Q_PROPERTY(int ind READ ind WRITE setInd)

 public:
  CQChartsAnnotation(CQChartsView *view);
  CQChartsAnnotation(CQChartsPlot *plot);

  virtual ~CQChartsAnnotation();

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  QString calcId() const override;

  QString calcTipId() const override;

  virtual const char *typeName() const = 0;

  QString pathId() const;

  const CQChartsGeom::BBox &bbox() const { return bbox_; }

  CQChartsEditHandles &editHandles() { return editHandles_; }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  virtual QString propertyId() const = 0;

  void addStrokeFillProperties(CQPropertyViewModel *model, const QString &path);

  void addStrokeProperties(CQPropertyViewModel *model, const QString &path);

  void addFillProperties(CQPropertyViewModel *model, const QString &path);

  bool getProperty(const QString &name, QVariant &value);
  bool setProperty(const QString &name, const QVariant &value);

  void getPropertyNames(QStringList &names) const;

  //---

  bool contains(const CQChartsGeom::Point &p) const;

  virtual bool inside(const CQChartsGeom::Point &p) const;

  //---

  virtual bool selectPress(const CQChartsGeom::Point &);

  virtual bool editPress  (const CQChartsGeom::Point &);
  virtual bool editMove   (const CQChartsGeom::Point &);
  virtual bool editMotion (const CQChartsGeom::Point &);
  virtual bool editRelease(const CQChartsGeom::Point &);

  virtual void editMoveBy(const QPointF &d);

  virtual void setBBox(const CQChartsGeom::BBox &, const CQChartsResizeHandle::Side &) { }

  //---

  void redrawBoxObj() override { emit dataChanged(); }

  virtual void draw(QPainter *painter);

  void drawEditHandles(QPainter *painter);

  //---

  CQChartsView *view() const;

  //---

  virtual void write(std::ostream &os) const = 0;

  void writeKeys(std::ostream &os, const QString &cmd) const;

  void writeFill  (std::ostream &os) const;
  void writeStroke(std::ostream &os) const;

 signals:
  void dataChanged();

 protected:
  int                 ind_         { 0 };    // unique ind
  CQChartsGeom::BBox  bbox_;                 // bbox (plot coords)
  CQChartsEditHandles editHandles_;          // edit handles
  bool                autoSize_    { true }; // set bbox from contents
};

//---

class CQChartsRectAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsRect     rect  READ rect  WRITE setRect )
  Q_PROPERTY(CQChartsPosition start READ start WRITE setStart)
  Q_PROPERTY(CQChartsPosition end   READ end   WRITE setEnd  )

 public:
  CQChartsRectAnnotation(CQChartsView *view, const CQChartsRect &rect=CQChartsRect());
  CQChartsRectAnnotation(CQChartsPlot *plot, const CQChartsRect &rect=CQChartsRect());

  virtual ~CQChartsRectAnnotation();

  const char *typeName() const override { return "rect"; }

  const CQChartsRect &rect() const { return rect_; }
  void setRect(const CQChartsRect &rect);

  void setRect(const CQChartsPosition &start, const CQChartsPosition &end);

  CQChartsPosition start() const;
  void setStart(const CQChartsPosition &p);

  CQChartsPosition end() const;
  void setEnd(const CQChartsPosition &p);

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  QString propertyId() const override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  void draw(QPainter *painter) override;

  void write(std::ostream &os) const override;

 private:
  CQChartsRect rect_;
};

//---

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

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  QString propertyId() const override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(QPainter *painter) override;

  void write(std::ostream &os) const override;

 private:
  CQChartsPosition center_;
  CQChartsLength   xRadius_ { 1.0 };
  CQChartsLength   yRadius_ { 1.0 };
};

//---

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

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  QString propertyId() const override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(QPainter *painter) override;

  void write(std::ostream &os) const override;

 private:
  CQChartsPolygon polygon_;
};

//---

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

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  QString propertyId() const override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(QPainter *painter) override;

  void write(std::ostream &os) const override;

 private:
  CQChartsPolygon polygon_;
};

//---

class CQChartsTextAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition position READ position WRITE setPosition)

 public:
  CQChartsTextAnnotation(CQChartsView *view, const CQChartsPosition &p=CQChartsPosition(),
                         const QString &text="");
  CQChartsTextAnnotation(CQChartsPlot *plot, const CQChartsPosition &p=CQChartsPosition(),
                         const QString &text="");

  virtual ~CQChartsTextAnnotation();

  const char *typeName() const override { return "text"; }

  const CQChartsPosition &position() const { return position_; }
  void setPosition(const CQChartsPosition &p) { position_ = p; emit dataChanged(); }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  QString propertyId() const override;

  //---

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(QPainter *painter) override;

  void write(std::ostream &os) const override;

 private:
  CQChartsPosition position_;
};

//---

class CQChartsArrow;

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

  CQChartsArrow *arrow() const { return arrow_; }

  void setArrowData(const CQChartsArrowData &data);

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  QString propertyId() const override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(QPainter *painter) override;

  void write(std::ostream &os) const override;

 private:
  CQChartsPosition start_ { QPointF(0, 0) };
  CQChartsPosition end_   { QPointF(1, 1) };
  CQChartsArrow*   arrow_ { nullptr };
};

//---

class CQChartsPointAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition position READ position WRITE setPosition)

 public:
  CQChartsPointAnnotation(CQChartsView *view, const CQChartsPosition &p=CQChartsPosition(),
                          const CQChartsSymbol &type=CQChartsSymbol::Type::CIRCLE);
  CQChartsPointAnnotation(CQChartsPlot *plot, const CQChartsPosition &p=CQChartsPosition(),
                          const CQChartsSymbol &type=CQChartsSymbol::Type::CIRCLE);

  virtual ~CQChartsPointAnnotation();

  const char *typeName() const override { return "point"; }

  const CQChartsPosition &position() const { return position_; }
  void setPosition(const CQChartsPosition &p) { position_ = p; emit dataChanged(); }

  const CQChartsSymbol &type() const { return pointData_.type; }
  void setType(const CQChartsSymbol &t) { pointData_.type = t; emit dataChanged(); }

  const CQChartsSymbolData &pointData() const { return pointData_; }
  void setPointData(const CQChartsSymbolData &p) { pointData_ = p; emit dataChanged(); }

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  QString propertyId() const override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void draw(QPainter *painter) override;

  void write(std::ostream &os) const override;

 private:
  CQChartsPosition   position_;
  CQChartsSymbolData pointData_;
};

#endif
