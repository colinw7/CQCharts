#ifndef CQChartsTitle_H
#define CQChartsTitle_H

#include <CQChartsTextBoxObj.h>
#include <CQChartsEditHandles.h>
#include <CQChartsPosition.h>
#include <CQChartsGeom.h>
#include <QPointF>
#include <QSizeF>
#include <vector>

class CQChartsPlot;
class CQPropertyViewModel;
class QPainter;

class CQChartsTitle : public CQChartsTextBoxObj {
  Q_OBJECT

  Q_PROPERTY(LocationType     location    READ location    WRITE setLocation   )
  Q_PROPERTY(CQChartsPosition absPosition READ absPosition WRITE setAbsPosition)
  Q_PROPERTY(QRectF           absRect     READ absRect     WRITE setAbsRect    )
  Q_PROPERTY(bool             inside      READ isInside    WRITE setInside     )

  Q_ENUMS(LocationType)

 public:
  enum LocationType {
    TOP,
    CENTER,
    BOTTOM,
    ABS_POS,
    ABS_RECT
  };

 public:
  CQChartsTitle(CQChartsPlot *plot);

  QString id() const;

  //---

  const LocationType &location() const { return location_.location; }
  void setLocation(const LocationType &l) { location_.location = l; redraw(); }

  const CQChartsPosition &absPosition() const { return location_.absPosition; }
  void setAbsPosition(const CQChartsPosition &p) { location_.absPosition = p; redraw(); }

  const QRectF &absRect() const { return location_.absRect; }
  void setAbsRect(const QRectF &r) { location_.absRect = r; redraw(); }

  bool isInside() const { return location_.inside; }
  void setInside(bool b) { location_.inside = b; redraw(); }

  //---

  const QPointF &position() const { return position_; }
  void setPosition(const QPointF &p) { position_ = p; }

  CQChartsEditHandles &editHandles() { return editHandles_; }

  //---

  QString locationStr() const;
  void setLocationStr(const QString &s);

  //---

  QPointF absPlotPosition() const;
  void setAbsPlotPosition(const QPointF &p);

  //---

  const CQChartsGeom::BBox &bbox() const { return bbox_; }
  void setBBox(const CQChartsGeom::BBox &b) { bbox_ = b; }

  //---

  void redrawBoxObj() override { redraw(); }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  QSizeF calcSize();

  //---

  void redraw();

  //---

  bool contains(const CQChartsGeom::Point &p) const;

  //---

  virtual bool selectPress  (const CQChartsGeom::Point &) { return false; }
  virtual bool selectMove   (const CQChartsGeom::Point &) { return false; }
  virtual bool selectRelease(const CQChartsGeom::Point &) { return false; }

  virtual bool editPress  (const CQChartsGeom::Point &);
  virtual bool editMove   (const CQChartsGeom::Point &);
  virtual bool editMotion (const CQChartsGeom::Point &);
  virtual bool editRelease(const CQChartsGeom::Point &);

  virtual void editMoveBy(const QPointF &d);

  //---

  void draw(QPainter *painter);

  void drawEditHandles(QPainter *painter);

 private:
  void updateLocation();

 private:
  struct Location {
    LocationType     location    { LocationType::TOP}; // loction
    CQChartsPosition absPosition;                      // position (relative to plot box)
    QRectF           absRect;                          // rect (relative to plot box)
    bool             inside      { false };            // is inside
  };

  Location                   location_;          // location
  QPointF                    position_ { 0, 0 }; // position
  QSizeF                     size_;              // size
  mutable CQChartsGeom::BBox bbox_;              // bbox
  CQChartsEditHandles        editHandles_;       // edit handles
};

#endif
