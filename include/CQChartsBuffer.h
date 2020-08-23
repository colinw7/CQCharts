#ifndef CQChartsBuffer_H
#define CQChartsBuffer_H

#include <QObject>
#include <QRectF>

class QImage;
class QPainter;

#ifdef CQCHARTS_OPENGL
class QOpenGLFramebufferObject;
class QOpenGLPaintDevice;
class QOpenGLContext;
class QWindow;
#endif

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

  enum class BufferType {
    PIXMAP,
    IMAGE,
    OPENGL
  };

 public:
  static const char *typeName(const Type &type);

  static Type nameType(const QString &name);

 public:
#ifdef CQCHARTS_OPENGL
  using GLBuffer  = QOpenGLFramebufferObject;
  using GLDevice  = QOpenGLPaintDevice;
  using GLContext = QOpenGLContext;
#endif

 public:
  CQChartsBuffer(QWidget *widget=nullptr, const Type &type=CQChartsBuffer::Type::NONE);
 ~CQChartsBuffer();

  const Type &type() const { return type_; }

  bool isActive() const { return active_; }
  void setActive(bool b) { active_ = b; }

  bool isValid() const { return valid_; }
  void setValid(bool b) { valid_ = b; }

  const BufferType &bufferType() const { return bufferType_; }

  QImage  *image () const { return image_ ; }
  QPixmap *pixmap() const { return pixmap_; }

#ifdef CQCHARTS_OPENGL
  GLBuffer  *glBuffer () const { return glBuffer_; }
  GLDevice  *glDevice () const { return glDevice_; }
  GLContext *glContext() const { return glContext_; }
#endif

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
  QWidget* widget_ { nullptr };
  Type     type_   { Type::NONE };
  bool     active_ { true };
  bool     valid_  { false };

  // buffer type and object
  BufferType bufferType_ { BufferType::IMAGE };
  QPixmap*   pixmap_     { nullptr };
  QImage*    image_      { nullptr };
#ifdef CQCHARTS_OPENGL
  GLBuffer*  glBuffer_   { nullptr };
  GLDevice*  glDevice_   { nullptr };
  GLContext* glContext_  { nullptr };
  QWindow*   glWindow_   { nullptr };
#endif

  QRectF     rect_;
  QSize      size_;
  QPainter*  ipainter_ { nullptr };
  QPainter*  painter_  { nullptr };
};

#endif
