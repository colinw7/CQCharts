#ifndef CQChartsLayer_H
#define CQChartsLayer_H

#include <QImage>
#include <QPainter>

class CQChartsBuffer;

class CQChartsLayer {
 public:
  enum class Type {
    NONE,
    BACKGROUND,
    BG_AXES,
    BG_KEY,
    BG_PLOT,
    MID_PLOT,
    FG_PLOT,
    FG_AXES,
    FG_KEY,
    TITLE,
    ANNOTATION,
    FOREGROUND,
    EDIT_HANDLE,
    BOXES,
    SELECTION,
    MOUSE_OVER
  };

 public:
  static const char *typeName(const Type &type);

  static Type nameType(const QString &name);

  static Type firstLayer() { return Type::BACKGROUND; }
  static Type lastLayer () { return Type::MOUSE_OVER; }

 public:
  CQChartsLayer(const Type &type, CQChartsBuffer *buffer);
 ~CQChartsLayer();

  const Type &type() const { return type_; }

  const CQChartsBuffer *buffer() const { return buffer_; }

  QString name() const { return typeName(type_); }

  bool isActive() const { return active_; }
  void setActive(bool b) { active_ = b; }

 private:
  Type            type_   { Type::NONE };
  CQChartsBuffer* buffer_ { nullptr };
  bool            active_ { false };
};

//----

class CQChartsBuffer {
 public:
  enum class Type {
    NONE,
    BACKGROUND,
    MIDDLE,
    FOREGROUND,
    OVERLAY
  };

 public:
  CQChartsBuffer(const Type &type);
 ~CQChartsBuffer();

  const Type &type() const { return type_; }

  bool isActive() const { return active_; }
  void setActive(bool b) { active_ = b; }

  bool isValid() const { return valid_; }
  void setValid(bool b) { valid_ = b; }

  QImage  *image () const { return image_ ; }
  QPixmap *pixmap() const { return pixmap_; }

  const QRectF &rect() const { return rect_; }

  QPainter *beginPaint(QPainter *painter, const QRectF &rect, bool alias=true);

  void endPaint();

 private:
  QPainter *ipainter();

  void updateSize();

 private:
  Type      type_     { Type::NONE };
  bool      active_   { true };
  bool      valid_    { false };
  QPixmap*  pixmap_   { nullptr };
  QImage*   image_    { nullptr };
  QRectF    rect_;
  QSize     size_;
  QPainter* ipainter_ { nullptr };
  QPainter* painter_  { nullptr };
};

#endif
