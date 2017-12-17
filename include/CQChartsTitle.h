#ifndef CQChartsTitle_H
#define CQChartsTitle_H

#include <CQChartsTextBoxObj.h>
#include <CQChartsGeom.h>
#include <QPointF>
#include <QSizeF>
#include <vector>

class CQChartsPlot;
class CQPropertyViewModel;
class QPainter;

class CQChartsTitle : public CQChartsTextBoxObj {
  Q_OBJECT

  Q_PROPERTY(bool    visible     READ isVisible   WRITE setVisible    )
  Q_PROPERTY(QString location    READ locationStr WRITE setLocationStr)
  Q_PROPERTY(QPointF absPosition READ absPosition WRITE setAbsPosition)
  Q_PROPERTY(bool    inside      READ isInside    WRITE setInside     )

 public:
  enum LocationType {
    TOP,
    CENTER,
    BOTTOM,
    ABSOLUTE
  };

 public:
  CQChartsTitle(CQChartsPlot *plot);

  CQChartsPlot *plot() const { return plot_; }

  //---

  void setMargin (double r) override { CQChartsBoxObj::setMargin (r); redraw(); }
  void setPadding(double r) override { CQChartsBoxObj::setPadding(r); redraw(); }

  void setText(const QString &s) override { CQChartsTextBoxObj::setText(s); redraw(); }
  void setFont(const QFont &f) override { CQChartsTextBoxObj::setFont(f); redraw(); }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; redraw(); }

  const LocationType &location() const { return location_.location; }
  void setLocation(const LocationType &l) { location_.location = l; redraw(); }

  QString locationStr() const;
  void setLocationStr(const QString &s);

  const QPointF &absPosition() const { return location_.absPosition; }
  void setAbsPosition(const QPointF &p) { location_.absPosition = p; redraw(); }

  bool isInside() const { return location_.inside; }
  void setInside(bool b) { location_.inside = b; redraw(); }

  //---

  const CQChartsGeom::BBox &bbox() const { return bbox_; }
  void setBBox(const CQChartsGeom::BBox &b) { bbox_ = b; }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  const QPointF &position() const { return position_; }
  void setPosition(const QPointF &p) { position_ = p; }

  //---

  QPointF absPlotPosition() const;
  void setAbsPlotPosition(const QPointF &p);

  //---

  QSizeF calcSize();

  //---

  void redraw();

  //---

  bool contains(const CQChartsGeom::Point &p) const;

  //---

  virtual bool mousePress  (const CQChartsGeom::Point &) { return false; }
  virtual bool mouseMove   (const CQChartsGeom::Point &) { return false; }
  virtual void mouseRelease(const CQChartsGeom::Point &) { }

  virtual bool mouseDragPress  (const CQChartsGeom::Point &);
  virtual bool mouseDragMove   (const CQChartsGeom::Point &);
  virtual void mouseDragRelease(const CQChartsGeom::Point &);

  //---

  void draw(QPainter *painter);

 private:
  void updateLocation();

 private:
  struct Location {
    LocationType location    { LocationType::TOP};
    QPointF      absPosition;
    bool         inside      { false };
  };

  bool                       visible_  { true };
  Location                   location_;
  QPointF                    position_ { 0, 0 };
  QSizeF                     size_;
  mutable CQChartsGeom::BBox bbox_;
  CQChartsGeom::Point        dragPos_;
};

#endif
