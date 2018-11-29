#ifndef CQChartsDataLabel_H
#define CQChartsDataLabel_H

#include <CQChartsTextBoxObj.h>
#include <CQChartsGeom.h>

class CQChartsPlot;

class CQChartsDataLabel : public CQChartsTextBoxObj {
  Q_OBJECT

  Q_PROPERTY(Position        position  READ position  WRITE setPosition )
  Q_PROPERTY(Qt::Orientation direction READ direction WRITE setDirection)
  Q_PROPERTY(bool            clip      READ isClip    WRITE setClip     )
  Q_PROPERTY(double          margin    READ margin    WRITE setMargin   )

  Q_ENUMS(Position)

 public:
  enum Position {
    TOP_INSIDE,
    TOP_OUTSIDE,
    CENTER,
    BOTTOM_INSIDE,
    BOTTOM_OUTSIDE,
  };

 public:
  CQChartsDataLabel(CQChartsPlot *plot);

  virtual ~CQChartsDataLabel() { }

  // data label
  const Position &position() const { return position_; }
  void setPosition(const Position &p) { position_ = p; update(); }

  const Qt::Orientation &direction() const { return direction_; }
  void setDirection(const Qt::Orientation &v) { direction_ = v; }

  bool isClip() const { return clip_; }
  void setClip(bool b) { clip_ = b; update(); }

  //--

  bool isPositionInside() const {
    return (position_ == Position::TOP_INSIDE || position_ == Position::BOTTOM_INSIDE);
  }

  bool isPositionOutside() const {
    return (position_ == Position::TOP_OUTSIDE || position_ == Position::BOTTOM_OUTSIDE);
  }

  Position flipPosition() const {
    return flipPosition(position());
  }

  static Position flipPosition(const Position &position) {
    switch (position) {
      case TOP_INSIDE    : return BOTTOM_INSIDE;
      case TOP_OUTSIDE   : return BOTTOM_OUTSIDE;
      case BOTTOM_INSIDE : return TOP_INSIDE;
      case BOTTOM_OUTSIDE: return TOP_OUTSIDE;
      default            : return position;
    }
  }

  //--

  void addPathProperties(const QString &path);

  virtual void update();

  //---

  void draw(QPainter *painter, const QRectF &qrect, const QString &ystr);

  void draw(QPainter *painter, const QRectF &qrect, const QString &ystr,
            const Position &position);

  void draw(QPainter *painter, const QRectF &qrect, const QString &ystr,
            const Position &position, const QPen &tpen);

  //---

  CQChartsGeom::BBox calcRect(const QRectF &qrect, const QString &ystr) const;

  CQChartsGeom::BBox calcRect(const QRectF &qrect, const QString &ystr,
                              const Position &position) const;

  Qt::Alignment textAlignment() const;

  static Qt::Alignment textAlignment(const Position &position);

 private:
  Position        position_  { Position::TOP_INSIDE };
  Qt::Orientation direction_ { Qt::Vertical };
  bool            clip_      { false };
};

#endif
