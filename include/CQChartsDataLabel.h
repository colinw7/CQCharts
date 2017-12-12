#ifndef CQChartsDataLabel_H
#define CQChartsDataLabel_H

#include <CQChartsTextBoxObj.h>

class CQChartsPlot;

class CQChartsDataLabel : public CQChartsTextBoxObj {
  Q_OBJECT

  Q_PROPERTY(bool     visible  READ isVisible WRITE setVisible )
  Q_PROPERTY(Position position READ position  WRITE setPosition)
  Q_PROPERTY(bool     clip     READ isClip    WRITE setClip    )
  Q_PROPERTY(double   margin   READ margin    WRITE setMargin  )

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

  // data label
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; update(); }

  const Position &position() const { return position_; }
  void setPosition(const Position &p) { position_ = p; update(); }

  bool isClip() const { return clip_; }
  void setClip(bool b) { clip_ = b; update(); }

  ///--

  void addProperties(const QString &path);

  void update();

  void draw(QPainter *painter, const QRectF &qrect, const QString &ystr);

 private:
  bool     visible_  { false };
  Position position_ { Position::TOP_INSIDE };
  bool     clip_     { false };
};

#endif
