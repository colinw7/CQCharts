#include <CQChartsLayer.h>
#include <CQChartsUtil.h>
#include <CQChartsEnv.h>
#include <CMathRound.h>

const char *
CQChartsLayer::
typeName(const Type &type)
{
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

CQChartsLayer::Type
CQChartsLayer::
nameType(const QString &name)
{
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

CQChartsLayer::
CQChartsLayer(const Type &type, const CQChartsBuffer::Type &buffer) :
 type_(type), buffer_(buffer)
{
}

CQChartsLayer::
~CQChartsLayer()
{
}

//------

CQChartsBuffer::
CQChartsBuffer(const Type &type) :
 type_(type)
{
  usePixmap_ = CQChartsEnv::getBool("CQ_CHARTS_LAYER_PIXMAP");
}

CQChartsBuffer::
~CQChartsBuffer()
{
  delete image_;
  delete pixmap_;
  delete ipainter_;
}

QPainter *
CQChartsBuffer::
beginPaint(QPainter *painter, const QRectF &rect, bool alias)
{
  painter_ = painter;
  rect_    = rect;

  updateSize();

  //---

  if (isValid())
    return nullptr;

  clear();

  if (usePixmap_) {
    assert(pixmap_);

    ipainter()->begin(pixmap_);
  }
  else {
    assert(image_);

    ipainter()->begin(image_);
  }

  QTransform t;

  t.translate(-rect_.x(), -rect_.y());

  ipainter()->setTransform(t);

  //ipainter()->setViewTransformEnabled(true);

  if (alias)
    ipainter()->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);

  return ipainter();
}

void
CQChartsBuffer::
endPaint(bool draw)
{
  if (! isValid()) {
    ipainter()->end();

    setValid(true);
  }

  if (draw)
    this->draw(painter_);

  painter_ = nullptr;
}

void
CQChartsBuffer::
clear()
{
  if (usePixmap_) {
    if (! pixmap_) return;

    pixmap_->fill(QColor(0,0,0,0));
  }
  else {
    if (! image_) return;

    image_->fill(QColor(0,0,0,0));
  }
}

void
CQChartsBuffer::
draw(QPainter *painter)
{
  draw(painter, rect_.x(), rect_.y());
}

void
CQChartsBuffer::
draw(QPainter *painter, int x, int y)
{
  if (usePixmap_) {
    assert(pixmap_);

    painter->drawPixmap(x, y, *pixmap_);
  }
  else {
    assert(image_);

    painter->drawImage(x, y, *image_);
  }
}

QPainter *
CQChartsBuffer::
ipainter()
{
  if (! ipainter_)
    ipainter_ = new QPainter;

  return ipainter_;
}

void
CQChartsBuffer::
updateSize()
{
  QSizeF fsize = rect_.size();

  QSize size(CMathRound::RoundUp(fsize.width()), CMathRound::RoundUp(fsize.height()));

  bool hasDrawable = (usePixmap_ ? (pixmap_ != 0) : (image_ != 0));

  if (! hasDrawable || size_ != size) {
    delete image_;
    delete pixmap_;
    delete ipainter_;

    size_ = size;

    if (usePixmap_)
      pixmap_ = new QPixmap(size_);
    else
      image_ = new QImage(size_, QImage::Format_ARGB32);

    ipainter_ = nullptr;

    setValid(false);
  }
}
