#ifndef CQHtmlTextPainter_H
#define CQHtmlTextPainter_H

#include <QObject>
#include <QFont>
#include <QColor>

class QPainter;

class CQHtmlTextPainter : public QObject {
  Q_OBJECT

  Q_PROPERTY(QString       text      READ text       WRITE setText)
  Q_PROPERTY(int           margin    READ margin     WRITE setMargin)
  Q_PROPERTY(QFont         font      READ font       WRITE setFont)
  Q_PROPERTY(QColor        textColor READ textColor  WRITE setTextColor)
  Q_PROPERTY(double        angle     READ angle      WRITE setAngle)
  Q_PROPERTY(bool          contrast  READ isContrast WRITE setContrast)
  Q_PROPERTY(bool          clip      READ isClip     WRITE setClip)
  Q_PROPERTY(bool          align     READ isAlign    WRITE setAlign)
  Q_PROPERTY(Qt::Alignment alignment READ alignment  WRITE setAlignment)

 public:
  CQHtmlTextPainter();

  const QString &text() const { return text_; }
  void setText(const QString &s) { text_ = s; Q_EMIT stateChanged(); }

  int margin() const { return margin_; }
  void setMargin(int i) { margin_ = i; Q_EMIT stateChanged(); }

  const QFont &font() const { return font_; }
  void setFont(const QFont &f) { font_ = f; Q_EMIT stateChanged(); }

  const QColor &textColor() const { return textColor_; }
  void setTextColor(const QColor &c) { textColor_ = c; Q_EMIT stateChanged(); }

  double angle() const { return angle_; }
  void setAngle(double r) { angle_ = r; Q_EMIT stateChanged(); }

  bool isContrast() const { return contrast_; }
  void setContrast(bool b) { contrast_ = b; Q_EMIT stateChanged(); }

  bool isClip() const { return clip_; }
  void setClip(bool b) { clip_ = b; Q_EMIT stateChanged(); }

  bool isAlign() const { return align_; }
  void setAlign(bool b) { align_ = b; Q_EMIT stateChanged(); }

  const Qt::Alignment &alignment() const { return alignment_; }
  void setAlignment(const Qt::Alignment &v) { alignment_ = v; Q_EMIT stateChanged(); }

  //---

  void drawScaledInRect(QPainter *painter, const QRect &rect);

  void drawInRect(QPainter *painter, const QRect &rect);
  void drawInRect(QPainter *painter, const QPoint &center, const QRect &rect);

  void drawTextBg(QPainter *painter, const QRect &rect);

  //---

  QSizeF textSize() const;
  QSizeF textWidthSize(int width) const;

 Q_SIGNALS:
  void stateChanged();

 private:
  QString       text_;
  int           margin_    { 2 };
  QFont         font_;
  QColor        textColor_ { Qt::black };
  double        angle_     { 0 };
  bool          contrast_  { false };
  bool          clip_      { true };
  bool          align_     { false };
  Qt::Alignment alignment_ { Qt::AlignCenter };
};

#endif
