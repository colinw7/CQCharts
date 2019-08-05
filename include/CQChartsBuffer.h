#ifndef CQChartsBuffer_H
#define CQChartsBuffer_H

#include <QObject>
#include <QRectF>

class QImage;
class QPainter;

/*!
 * \brief Draw image/pixmap buffer
 * \ingroup Charts
 */
class CQChartsBuffer : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool active READ isActive WRITE setActive)
  Q_PROPERTY(bool valid  READ isValid  WRITE setValid )

 public:
  enum class Type {
    NONE,
    BACKGROUND,
    MIDDLE,
    FOREGROUND,
    OVERLAY
  };

 public:
  static const char *typeName(const Type &type);

  static Type nameType(const QString &name);

 public:
  CQChartsBuffer(const Type &type=CQChartsBuffer::Type::NONE);
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

  void endPaint(bool draw=true);

  void clear();

  void draw(QPainter *painter);
  void draw(QPainter *painter, int x, int y);

 private:
  QPainter *ipainter();

  void updateSize();

 private:
  Type      type_      { Type::NONE };
  bool      active_    { true };
  bool      valid_     { false };
  bool      usePixmap_ { false };
  QPixmap*  pixmap_    { nullptr };
  QImage*   image_     { nullptr };
  QRectF    rect_;
  QSize     size_;
  QPainter* ipainter_  { nullptr };
  QPainter* painter_   { nullptr };
};

#endif
