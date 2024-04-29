#ifndef CQChartsDataLabel_H
#define CQChartsDataLabel_H

#include <CQChartsTextBoxObj.h>
#include <CQChartsGeom.h>

class CQChartsPlot;
class CQChartsTextPlacer;

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
  using LabelPosition = CQChartsLabelPosition;

  enum class Position {
    TOP_INSIDE     = int(LabelPosition::TOP_INSIDE),
    TOP_OUTSIDE    = int(LabelPosition::TOP_OUTSIDE),
    CENTER         = int(LabelPosition::CENTER),
    BOTTOM_INSIDE  = int(LabelPosition::BOTTOM_INSIDE),
    BOTTOM_OUTSIDE = int(LabelPosition::BOTTOM_OUTSIDE),
    LEFT_INSIDE    = int(LabelPosition::LEFT_INSIDE),
    LEFT_OUTSIDE   = int(LabelPosition::LEFT_OUTSIDE),
    RIGHT_INSIDE   = int(LabelPosition::RIGHT_INSIDE),
    RIGHT_OUTSIDE  = int(LabelPosition::RIGHT_OUTSIDE)
  };

  using Plot        = CQChartsPlot;
  using BoxObj      = CQChartsBoxObj;
  using PaintDevice = CQChartsPaintDevice;
  using PenBrush    = CQChartsPenBrush;
  using PenData     = CQChartsPenData;
  using BrushData   = CQChartsBrushData;
  using Font        = CQChartsFont;
  using Angle       = CQChartsAngle;
  using Alpha       = CQChartsAlpha;
  using TextPlacer  = CQChartsTextPlacer;
  using Margin      = CQChartsGeom::Margin;
  using BBox        = CQChartsGeom::BBox;
  using ColorInd    = CQChartsUtil::ColorInd;

 public:
  CQChartsDataLabel(Plot *plot);

  virtual ~CQChartsDataLabel() = default;

  //--

  void setVisible(bool b, bool notify=true) override {
    if (b != CQChartsTextBoxObj::isVisible()) {
      CQChartsTextBoxObj::setVisible(b);

      if (notify)
        textBoxObjInvalidate();
    }
  }

  //! position
  const Position &position() const { return position_; }
  void setPosition(const Position &p) { position_ = p; textBoxObjInvalidate(); }

  //! direction
  const Qt::Orientation &direction() const { return direction_; }
  void setDirection(const Qt::Orientation &v) { direction_ = v; textBoxObjInvalidate(); }

  //! clip by rectangle
  bool isClip() const { return clip_; }
  void setClip(bool b) { clip_ = b; textBoxObjInvalidate(); }

  //! draw clipped text
  bool drawClipped() const { return drawClipped_; }
  void setDrawClipped(bool b) { drawClipped_ = b; textBoxObjInvalidate(); }

  //! move clipped text
  bool moveClipped() const { return moveClipped_; }
  void setMoveClipped(bool b) { moveClipped_ = b; textBoxObjInvalidate(); }

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
      case Position::TOP_INSIDE    : return Position::BOTTOM_INSIDE;
      case Position::TOP_OUTSIDE   : return Position::BOTTOM_OUTSIDE;
      case Position::BOTTOM_INSIDE : return Position::TOP_INSIDE;
      case Position::BOTTOM_OUTSIDE: return Position::TOP_OUTSIDE;
      default                      : return position;
    }
  }

  static LabelPosition flipPosition(const LabelPosition &position) {
    return static_cast<LabelPosition>(flipPosition(static_cast<Position>(position)));
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
            const Position &position, const PenBrush &tpen, const Font &font) const;

  //---

  bool isAdjustedPositionOutside(const BBox &pbbox, const QString &ystr) const;

  //---

  BBox calcRect(const BBox &pbbox, const QString &ystr) const;
  BBox calcRect(const BBox &pbbox, const QString &ystr, const Position &position) const;
  BBox calcRect(const BBox &pbbox, const QString &ystr, const Position &position,
                const Font &font) const;

  Qt::Alignment textAlignment() const;

  static Qt::Alignment textAlignment(const Position &position);

  //---

  const TextPlacer *textPlacer() const { return textPlacer_; }
  void setTextPlacer(TextPlacer *p) { textPlacer_ = p; }

  //---

  void connectDataChanged(const QObject *obj, const char *slotName) const;

  void textBoxObjInvalidate() override;

  //---

  bool contains(const Point &p) const override;

  //---

  void write(std::ostream &os, const QString &varName) const;

 Q_SIGNALS:
  //! emitted when data changed
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
  TextPlacer*     textPlacer_  { nullptr };
};

#endif
