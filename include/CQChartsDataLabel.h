#ifndef CQChartsDataLabel_H
#define CQChartsDataLabel_H

#include <CQChartsTextBoxObj.h>
#include <CQChartsGeom.h>

class CQChartsPlot;

/*!
 * \brief class for plot text box object
 * \ingroup Charts
 */
class CQChartsDataLabel : public CQChartsTextBoxObj {
  Q_OBJECT

  Q_PROPERTY(Position        position    READ position     WRITE setPosition   )
  Q_PROPERTY(Qt::Orientation direction   READ direction    WRITE setDirection  )
  Q_PROPERTY(bool            clip        READ isClip       WRITE setClip       )
  Q_PROPERTY(bool            drawClipped READ drawClipped  WRITE setDrawClipped)
  Q_PROPERTY(bool            moveClipped READ moveClipped  WRITE setMoveClipped)
  Q_PROPERTY(bool            sendSignal  READ isSendSignal WRITE setSendSignal )

  Q_ENUMS(Position)

 public:
  enum Position {
    TOP_INSIDE,
    TOP_OUTSIDE,
    CENTER,
    BOTTOM_INSIDE,
    BOTTOM_OUTSIDE,
    LEFT_INSIDE,
    LEFT_OUTSIDE,
    RIGHT_INSIDE,
    RIGHT_OUTSIDE
  };

  using Plot        = CQChartsPlot;
  using BoxObj      = CQChartsBoxObj;
  using PaintDevice = CQChartsPaintDevice;
  using PenBrush    = CQChartsPenBrush;
  using Angle       = CQChartsAngle;
  using Margin      = CQChartsGeom::Margin;
  using BBox        = CQChartsGeom::BBox;

 public:
  CQChartsDataLabel(Plot *plot);

  virtual ~CQChartsDataLabel() { }

  //--

  void setVisible(bool b) override {
    CQChartsTextBoxObj::setVisible(b); textBoxDataInvalidate();
  }

  //! position
  const Position &position() const { return position_; }
  void setPosition(const Position &p) { position_ = p; textBoxDataInvalidate(); }

  //! direction
  const Qt::Orientation &direction() const { return direction_; }
  void setDirection(const Qt::Orientation &v) { direction_ = v; textBoxDataInvalidate(); }

  //! clip by rectangle
  bool isClip() const { return clip_; }
  void setClip(bool b) { clip_ = b; textBoxDataInvalidate(); }

  //! draw clipped text
  bool drawClipped() const { return drawClipped_; }
  void setDrawClipped(bool b) { drawClipped_ = b; textBoxDataInvalidate(); }

  //! move clipped text
  bool moveClipped() const { return moveClipped_; }
  void setMoveClipped(bool b) { moveClipped_ = b; textBoxDataInvalidate(); }

  //! send signal
  bool isSendSignal() const { return sendSignal_; }
  void setSendSignal(bool b) { sendSignal_ = b; }

  //--

  bool isPositionInside () const { return isPositionInside (position()); }
  bool isPositionOutside() const { return isPositionOutside(position()); }

  static bool isPositionInside(const Position &position) {
    return (position == Position::TOP_INSIDE || position == Position::BOTTOM_INSIDE);
  }

  static bool isPositionOutside(const Position &position) {
    return (position == Position::TOP_OUTSIDE || position == Position::BOTTOM_OUTSIDE);
  }

  //--

  Position flipPosition() const { return flipPosition(position()); }

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

  void addPathProperties (const QString &path, const QString &desc);
  void addBasicProperties(const QString &path, const QString &desc);
  void addTextProperties (const QString &path, const QString &desc);

  //---

  void draw(PaintDevice *device, const BBox &qrect, const QString &str) const;
  void draw(PaintDevice *device, const BBox &qrect, const QString &str,
            const Position &position) const;
  void draw(PaintDevice *device, const BBox &qrect, const QString &str,
            const Position &position, const PenBrush &tpen) const;

  //---

  bool isAdjustedPositionOutside(const BBox &pbbox, const QString &ystr) const;

  //---

  BBox calcRect(const BBox &pbbox, const QString &ystr) const;
  BBox calcRect(const BBox &pbbox, const QString &ystr, const Position &position) const;

  Qt::Alignment textAlignment() const;

  static Qt::Alignment textAlignment(const Position &position);

  //---

  void textBoxDataInvalidate() override;

  //---

  bool contains(const Point &p) const override;

  //---

  void write(std::ostream &os, const QString &varName) const;

 signals:
  void dataChanged();

 private:
  Position adjustPosition(Position position) const;

 private:
  Position        position_    { Position::TOP_INSIDE }; //!< position relative to parent
  Qt::Orientation direction_   { Qt::Vertical };         //!< parent object direction
  bool            clip_        { true };                 //!< clip to parent rect
  bool            drawClipped_ { true };                 //!< draw clipped text
  bool            moveClipped_ { true };                 //!< move clipped
  bool            sendSignal_  { false };                //!< send signal on data change
};

#endif
