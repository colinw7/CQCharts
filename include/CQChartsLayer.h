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
  static const char *typeName(const Type &type)  {
    switch (type) {
      case Type::BACKGROUND : return "background";
      case Type::BG_AXES    : return "bg_axes";
      case Type::BG_KEY     : return "bg_key";
      case Type::BG_PLOT    : return "bg_plot";
      case Type::MID_PLOT   : return "mid_plot";
      case Type::FG_PLOT    : return "fg_plot";
      case Type::FG_AXES    : return "fg_axes";
      case Type::FG_KEY     : return "fg_key";
      case Type::TITLE      : return "title";
      case Type::ANNOTATION : return "annotation";
      case Type::FOREGROUND : return "foreground";
      case Type::EDIT_HANDLE: return "edit_handle";
      case Type::BOXES      : return "boxes";
      case Type::SELECTION  : return "selection";
      case Type::MOUSE_OVER : return "mouse_over";
      default               : return "none";
    }
  }

  static Type nameType(const QString &name)  {
    if      (name == "background" ) return Type::BACKGROUND;
    else if (name == "bg_axes"    ) return Type::BG_AXES;
    else if (name == "bg_key"     ) return Type::BG_KEY;
    else if (name == "bg_plot"    ) return Type::BG_PLOT;
    else if (name == "mid_plot"   ) return Type::MID_PLOT;
    else if (name == "fg_plot"    ) return Type::FG_PLOT;
    else if (name == "fg_axes"    ) return Type::FG_AXES;
    else if (name == "fg_key"     ) return Type::FG_KEY;
    else if (name == "title"      ) return Type::TITLE;
    else if (name == "annotation" ) return Type::ANNOTATION;
    else if (name == "foreground" ) return Type::FOREGROUND;
    else if (name == "edit_handle") return Type::EDIT_HANDLE;
    else if (name == "boxes"      ) return Type::BOXES;
    else if (name == "selection"  ) return Type::SELECTION;
    else if (name == "mouse_over" ) return Type::MOUSE_OVER;
    else                            return Type::NONE;
  }

 public:
  CQChartsLayer(Type type) :
   type_(type) {
  }

 ~CQChartsLayer() {
    delete image_;
    delete painter_;
  }
  const Type &type() const { return type_; }

  bool isActive() const { return active_; }
  void setActive(bool b) { active_ = b; }

  bool isValid() const { return valid_; }
  void setValid(bool b) { valid_ = b; }

  void updateSize(const QSize &size) {
    if (! image_ || size_ != size) {
      delete image_;
      delete ipainter_;

      size_     = size;
      image_    = new QImage(size_, QImage::Format_ARGB32);
      ipainter_ = nullptr;

      valid_ = false;
    }
  }

  QImage *image() const { return image_; }

  QPainter *ipainter() {
    if (! ipainter_)
      ipainter_ = new QPainter;

    return ipainter_;
  }

  QPainter *beginPaint(QPainter *painter) {
    painter_ = painter;

    int w = painter_->device()->width ();
    int h = painter_->device()->height();

    updateSize(QSize(w, h));

    //---

    if (valid_)
      return nullptr;

    image_->fill(QColor(0,0,0,0));

    ipainter()->begin(image_);

    ipainter()->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

    return ipainter();
  }

  void endPaint() {
    if (! valid_) {
      ipainter()->end();

      valid_ = true;
    }

    painter_->drawImage(0, 0, *image());

    painter_ = nullptr;
  }

 private:
  Type      type_     { Type::NONE };
  bool      active_   { false };
  bool      valid_    { false };
  QImage*   image_    { nullptr };
//QPixmap   pixmap_;
  QSize     size_;
  QPainter* ipainter_ { nullptr };
  QPainter* painter_  { nullptr };
};

#endif
