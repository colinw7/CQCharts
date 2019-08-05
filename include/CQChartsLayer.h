#ifndef CQChartsLayer_H
#define CQChartsLayer_H

#include <CQChartsBuffer.h>

/*!
 * \brief draw layer
 * \ingroup Charts
 */
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
  CQChartsLayer(const Type &type, const CQChartsBuffer::Type &buffer);
 ~CQChartsLayer();

  const Type &type() const { return type_; }

  const CQChartsBuffer::Type &buffer() const { return buffer_; }

  QString name() const { return typeName(type_); }

  bool isActive() const { return active_; }
  void setActive(bool b) { active_ = b; }

 private:
  Type                 type_   { Type::NONE };
  CQChartsBuffer::Type buffer_ { CQChartsBuffer::Type::NONE };
  bool                 active_ { false };
};

#endif
