#include <CQChartsBuffer.h>
#include <CQChartsEnv.h>
#include <CQChartsUtil.h>
#include <CMathRound.h>

#include <QPainter>
#include <cassert>
#include <iostream>

const char *
CQChartsBuffer::
typeName(const Type &type)
{
  switch (type) {
    case Type::BACKGROUND: return "background";
    case Type::MIDDLE    : return "middle";
    case Type::FOREGROUND: return "foreground";
    case Type::OVERLAY   : return "overlay";
    default              : return "none";
  }
}

CQChartsBuffer::Type
CQChartsBuffer::
nameType(const QString &name)
{
  if      (name == "background") return Type::BACKGROUND;
  else if (name == "middle"    ) return Type::MIDDLE;
  else if (name == "foreground") return Type::FOREGROUND;
  else if (name == "overlay"   ) return Type::OVERLAY;
  else                           return Type::NONE;
}

//---

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
//std::cerr << "beginPaint: " << typeName(type_) << "\n";
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
//std::cerr << "endPaint: " << typeName(type_) << "\n";
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
//std::cerr << "draw: " << typeName(type_) << "\n";
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
      image_ = CQChartsUtil::newImage(size_);

    ipainter_ = nullptr;

    setValid(false);
  }
}
