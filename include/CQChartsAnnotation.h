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

  bool contains(const CQChartsGeom::Point &p) const;

  //---

  virtual bool selectPress(const CQChartsGeom::Point &) { return false; }

  virtual bool editPress  (const CQChartsGeom::Point &);
  virtual bool editMove   (const CQChartsGeom::Point &);
  virtual bool editMotion (const CQChartsGeom::Point &);
  virtual bool editRelease(const CQChartsGeom::Point &);

  virtual void setBBox(const CQChartsGeom::BBox &) { }

  //---

  virtual void draw(QPainter *painter);

 signals:
  void dataChanged();

 protected:
  int                 ind_ { 0 }; // unique ind
  CQChartsGeom::BBox  bbox_;      // bbox (plot coords)
  CQChartsEditHandles editHandles_;
};

//---

class CQChartsRectAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(QPointF start READ start WRITE setStart)
  Q_PROPERTY(QPointF end   READ end   WRITE setEnd  )

 public:
  CQChartsRectAnnotation(CQChartsPlot *plot, const QPointF &p1=QPointF(0, 0),
                         const QPointF &p2=QPointF(1, 1));

  virtual ~CQChartsRectAnnotation();

  const QPointF &start() const { return start_; }
  void setStart(const QPointF &p) { start_ = p; emit dataChanged(); }

  const QPointF &end() const { return end_; }
  void setEnd(const QPointF &p) { end_ = p; emit dataChanged(); }

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void setBBox(const CQChartsGeom::BBox &bbox) override;

  void draw(QPainter *painter) override;

 private:
  QPointF start_;
  QPointF end_;
};

//---

class CQChartsEllipseAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(QPointF center  READ center  WRITE setCenter )
  Q_PROPERTY(double  xRadius READ xRadius WRITE setXRadius)
  Q_PROPERTY(double  yRadius READ yRadius WRITE setYRadius)

 public:
  CQChartsEllipseAnnotation(CQChartsPlot *plot, const QPointF &center=QPointF(0, 0),
                            double xRadius=1.0, double yRadius=1.0);

  virtual ~CQChartsEllipseAnnotation();

  const QPointF &center() const { return center_; }
  void setCenter(const QPointF &c) { center_ = c; emit dataChanged(); }

  int xRadius() const { return xRadius_; }
  void setXRadius(int i) { xRadius_ = i; }

  int yRadius() const { return yRadius_; }
  void setYRadius(int i) { yRadius_ = i; }

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void setBBox(const CQChartsGeom::BBox &bbox) override;

  void draw(QPainter *painter) override;

 private:
  QPointF center_;
  double  xRadius_ { 1.0 };
  double  yRadius_ { 1.0 };
};

//---

class CQChartsPolyAnnotation : public CQChartsAnnotation {
  Q_OBJECT

 public:
  CQChartsPolyAnnotation(CQChartsPlot *plot, const QPolygonF &points);

  virtual ~CQChartsPolyAnnotation();

  const QPolygonF &points() const { return points_; }
  void setPoints(const QPolygonF &points) { points_ = points; emit dataChanged(); }

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void setBBox(const CQChartsGeom::BBox &bbox) override;

  void setData(const CQChartsShapeData &data);

  void draw(QPainter *painter) override;

 private:
  QPolygonF points_;
};

//---

class CQChartsTextAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(QPointF position READ position WRITE setPosition)

 public:
  CQChartsTextAnnotation(CQChartsPlot *plot, const QPointF &p=QPointF(0, 0),
                         const QString &text="");

  virtual ~CQChartsTextAnnotation();

  const QPointF &position() const { return position_; }
  void setPosition(const QPointF &p) { position_ = p; emit dataChanged(); }

  const QString &textStr() const { return textStr_; }
  void setTextStr(const QString &s) { textStr_ = s; emit dataChanged(); }

  const QFont &textFont() const { return textData_.font; }
  void setTextFont(const QFont &f) { textData_.font = f; emit dataChanged(); }

  const CQChartsColor &textColor() const { return textData_.color; }
  void setTextColor(const CQChartsColor &c) { textData_.color = c; emit dataChanged(); }

  double textAlpha() const { return textData_.alpha; }
  void setTextAlpha(double r) { textData_.alpha = r; emit dataChanged(); }

  double textAngle() const { return textData_.angle; }
  void setTextAngle(double a) { textData_.angle = a; emit dataChanged(); }

  bool isTextContrast() const { return textData_.contrast; }
  void setTextContrast(bool b) { textData_.contrast = b; emit dataChanged(); }

  const CQChartsTextData &textData() const { return textData_; }
  void setTextData(const CQChartsTextData &d) { textData_ = d; emit dataChanged(); }

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void setBBox(const CQChartsGeom::BBox &bbox) override;

  void draw(QPainter *painter) override;

 private:
  QPointF          position_;
  QString          textStr_;
  CQChartsTextData textData_;
};

//---

class CQChartsArrow;

class CQChartsArrowAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(QPointF start READ start WRITE setStart)
  Q_PROPERTY(QPointF end   READ end   WRITE setEnd  )

 public:
  CQChartsArrowAnnotation(CQChartsPlot *plot, const QPointF &start=QPointF(0, 0),
                          const QPointF &end=QPointF(1, 1));

  virtual ~CQChartsArrowAnnotation();

  const QPointF &start() const { return start_; }
  void setStart(const QPointF &p) { start_ = p; emit dataChanged(); }

  const QPointF &end() const { return end_; }
  void setEnd(const QPointF &p) { end_ = p; emit dataChanged(); }

  void setData(const CQChartsArrowData &data);

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void setBBox(const CQChartsGeom::BBox &bbox) override;

  void draw(QPainter *painter) override;

 private:
  QPointF        start_ { 0, 0 };
  QPointF        end_   { 1, 1 };
  CQChartsArrow *arrow_ { nullptr };
};

//---

class CQChartsPointAnnotation : public CQChartsAnnotation {
  Q_OBJECT

  Q_PROPERTY(QPointF position READ position WRITE setPosition)

 public:
  CQChartsPointAnnotation(CQChartsPlot *plot, const QPointF &p=QPointF(0, 0),
                          const CQChartsPlotSymbol::Type &type=CQChartsPlotSymbol::Type::CROSS);

  virtual ~CQChartsPointAnnotation();

  const QPointF &position() const { return position_; }
  void setPosition(const QPointF &p) { position_ = p; emit dataChanged(); }

  const CQChartsPlotSymbol::Type &type() const { return pointData_.type; }
  void setType(const CQChartsPlotSymbol::Type &t) { pointData_.type = t; emit dataChanged(); }

  const CQChartsSymbolData &pointData() const { return pointData_; }
  void setPointData(const CQChartsSymbolData &v) { pointData_ = v; emit dataChanged(); }

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void setBBox(const CQChartsGeom::BBox &bbox) override;

  void draw(QPainter *painter) override;

 private:
  QPointF            position_  { 0, 0 };
  CQChartsSymbolData pointData_;
};

#endif
