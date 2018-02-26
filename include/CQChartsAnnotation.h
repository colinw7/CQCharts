#ifndef CQChartsAnnotation_H
#define CQChartsAnnotation_H

#include <CQChartsTextBoxObj.h>
#include <CQChartsEditHandles.h>
#include <CQChartsPlotSymbol.h>
#include <CQChartsArrow.h>
#include <CQChartsData.h>
#include <CQChartsGeom.h>
#include <QFont>

class CQChartsAnnotation : public CQChartsTextBoxObj {
  Q_OBJECT

 public:
  CQChartsAnnotation(CQChartsPlot *plot);

  virtual ~CQChartsAnnotation();

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  const CQChartsGeom::BBox &bbox() const { return bbox_; }

  CQChartsEditHandles &editHandles() { return editHandles_; }

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void addStrokeFillProperties(CQPropertyViewModel *model, const QString &path);

  void addStrokeProperties(CQPropertyViewModel *model, const QString &path);

  void addFillProperties(CQPropertyViewModel *model, const QString &path);

  bool contains(const CQChartsGeom::Point &p) const;

  //---

  virtual bool selectPress(const CQChartsGeom::Point &) { return false; }

  virtual bool editPress  (const CQChartsGeom::Point &);
  virtual bool editMove   (const CQChartsGeom::Point &);
  virtual bool editMotion (const CQChartsGeom::Point &);
  virtual bool editRelease(const CQChartsGeom::Point &);

  virtual void setBBox(const CQChartsGeom::BBox &, const CQChartsResizeHandle::Side &) { }

  //---

  void redrawBoxObj() override { emit dataChanged(); }

  virtual void draw(QPainter *painter);

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

  Q_PROPERTY(CQChartsPosition start READ start WRITE setStart)
  Q_PROPERTY(CQChartsPosition end   READ end   WRITE setEnd  )

 public:
  CQChartsRectAnnotation(CQChartsPlot *plot, const CQChartsPosition &p1=CQChartsPosition(),
                         const CQChartsPosition &p2=CQChartsPosition(QPointF(1, 1)));

  virtual ~CQChartsRectAnnotation();

  const CQChartsPosition &start() const { return start_; }
  void setStart(const CQChartsPosition &p) { start_ = p; emit dataChanged(); }

  const CQChartsPosition &end() const { return end_; }
  void setEnd(const CQChartsPosition &p) { end_ = p; emit dataChanged(); }

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  void draw(QPainter *painter) override;

 private:
  CQChartsPosition start_;
  CQChartsPosition end_   { QPointF(1, 1) };
};

//---

class CQChartsEllipseAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition center  READ center  WRITE setCenter )
  Q_PROPERTY(double           xRadius READ xRadius WRITE setXRadius)
  Q_PROPERTY(double           yRadius READ yRadius WRITE setYRadius)

 public:
  CQChartsEllipseAnnotation(CQChartsPlot *plot, const CQChartsPosition &center=CQChartsPosition(),
                            double xRadius=1.0, double yRadius=1.0);

  virtual ~CQChartsEllipseAnnotation();

  const CQChartsPosition &center() const { return center_; }
  void setCenter(const CQChartsPosition &c) { center_ = c; emit dataChanged(); }

  int xRadius() const { return xRadius_; }
  void setXRadius(int i) { xRadius_ = i; }

  int yRadius() const { return yRadius_; }
  void setYRadius(int i) { yRadius_ = i; }

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  void draw(QPainter *painter) override;

 private:
  CQChartsPosition center_;
  double           xRadius_ { 1.0 };
  double           yRadius_ { 1.0 };
};

//---

class CQChartsPolygonAnnotation : public CQChartsAnnotation {
  Q_OBJECT

 public:
  CQChartsPolygonAnnotation(CQChartsPlot *plot, const QPolygonF &points);

  virtual ~CQChartsPolygonAnnotation();

  const QPolygonF &points() const { return points_; }
  void setPoints(const QPolygonF &points) { points_ = points; emit dataChanged(); }

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  void draw(QPainter *painter) override;

 private:
  QPolygonF points_;
};

//---

class CQChartsPolylineAnnotation : public CQChartsAnnotation {
  Q_OBJECT

 public:
  CQChartsPolylineAnnotation(CQChartsPlot *plot, const QPolygonF &points);

  virtual ~CQChartsPolylineAnnotation();

  const QPolygonF &points() const { return points_; }
  void setPoints(const QPolygonF &points) { points_ = points; emit dataChanged(); }

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  void draw(QPainter *painter) override;

 private:
  QPolygonF points_;
};

//---

class CQChartsTextAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition position READ position WRITE setPosition)

 public:
  CQChartsTextAnnotation(CQChartsPlot *plot, const CQChartsPosition &p=CQChartsPosition(),
                         const QString &text="");

  virtual ~CQChartsTextAnnotation();

  const CQChartsPosition &position() const { return position_; }
  void setPosition(const CQChartsPosition &p) { position_ = p; emit dataChanged(); }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  void draw(QPainter *painter) override;

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
  CQChartsArrowAnnotation(CQChartsPlot *plot, const CQChartsPosition &start=CQChartsPosition(),
                          const CQChartsPosition &end=CQChartsPosition(QPointF(1, 1)));

  virtual ~CQChartsArrowAnnotation();

  const CQChartsPosition &start() const { return start_; }
  void setStart(const CQChartsPosition &p) { start_ = p; emit dataChanged(); }

  const CQChartsPosition &end() const { return end_; }
  void setEnd(const CQChartsPosition &p) { end_ = p; emit dataChanged(); }

  void setData(const CQChartsArrowData &data);

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  void draw(QPainter *painter) override;

 private:
  CQChartsPosition start_;
  CQChartsPosition end_   { QPointF(1, 1) };
  CQChartsArrow*   arrow_ { nullptr };
};

//---

class CQChartsPointAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(CQChartsPosition position READ position WRITE setPosition)

 public:
  CQChartsPointAnnotation(CQChartsPlot *plot, const CQChartsPosition &p=CQChartsPosition(),
                          const CQChartsPlotSymbol::Type &type=CQChartsPlotSymbol::Type::CROSS);

  virtual ~CQChartsPointAnnotation();

  const CQChartsPosition &position() const { return position_; }
  void setPosition(const CQChartsPosition &p) { position_ = p; emit dataChanged(); }

  const CQChartsPlotSymbol::Type &type() const { return pointData_.type; }
  void setType(const CQChartsPlotSymbol::Type &t) { pointData_.type = t; emit dataChanged(); }

  const CQChartsSymbolData &pointData() const { return pointData_; }
  void setPointData(const CQChartsSymbolData &v) { pointData_ = v; emit dataChanged(); }

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void setBBox(const CQChartsGeom::BBox &bbox, const CQChartsResizeHandle::Side &dragSide) override;

  void draw(QPainter *painter) override;

 private:
  CQChartsPosition   position_;
  CQChartsSymbolData pointData_;
};

#endif
