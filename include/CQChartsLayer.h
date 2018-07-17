#ifndef CQChartsLayer_H
#define CQChartsLayer_H

#include <QImage>
#include <QPainter>

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

 public:
  CQChartsLayer(Type type);
 ~CQChartsLayer();

  const Type &type() const { return type_; }

  bool isActive() const { return active_; }
  void setActive(bool b) { active_ = b; }

  bool isValid() const { return valid_; }
  void setValid(bool b) { valid_ = b; }

  void updateSize();

  QImage *image() const { return image_; }

  QPainter *ipainter();

  QPainter *beginPaint(QPainter *painter, const QRectF &rect);

  void endPaint();

 private:
  Type      type_     { Type::NONE };
  bool      active_   { false };
  bool      valid_    { false };
  QImage*   image_    { nullptr };
  QRectF    rect_;
  QSize     size_;
  QPainter* ipainter_ { nullptr };
  QPainter* painter_  { nullptr };
};

#endif
