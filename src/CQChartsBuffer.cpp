#include <CQChartsBuffer.h>
#include <CQChartsEnv.h>
#include <CQChartsUtil.h>
#include <CMathRound.h>

#include <QPainter>

#ifdef CQCHARTS_OPENGL
#include <QOpenGLFramebufferObject>
#include <QOpenGLPaintDevice>
#include <QOpenGLContext>
#include <QWindow>
#include <QThread>
#endif

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
CQChartsBuffer(QWidget *widget, const Type &type) :
 widget_(widget), type_(type)
{
  if      (CQChartsEnv::getBool("CQ_CHARTS_LAYER_PIXMAP"))
    bufferType_ = BufferType::PIXMAP;
#ifdef CQCHARTS_OPENGL
  else if (CQChartsEnv::getBool("CQ_CHARTS_LAYER_OPEN_GL"))
    bufferType_ = BufferType::OPENGL;
#endif
  else
    bufferType_ = BufferType::IMAGE;
}

CQChartsBuffer::
~CQChartsBuffer()
{
  delete image_;
  delete pixmap_;

#ifdef CQCHARTS_OPENGL
  delete glBuffer_;
  delete glDevice_;
  delete glContext_;
  delete glWindow_;
#endif

  delete ipainter_;
}

QPainter *
CQChartsBuffer::
beginPaint(QPainter *painter, const QRectF &rect, bool alias)
{
//std::cerr << "beginPaint: " << typeName(type_) << "\n";
  painter_ = painter;
  rect_    = rect;

  //---

  if (isValid())
    return nullptr;

  //---

  updateSize();

  //---

  clear();

  if      (bufferType() == BufferType::PIXMAP) {
    assert(pixmap_);

    ipainter()->begin(pixmap_);
  }
#ifdef CQCHARTS_OPENGL
  else if (bufferType() == BufferType::OPENGL) {
    //glWindow_ ->moveToThread(QThread::currentThread());
    //glContext_->moveToThread(QThread::currentThread());

    glContext_->makeCurrent(glWindow_);

    glBuffer_->bind();

    ipainter()->begin(glDevice_);

    ipainter()->fillRect(QRect(QPoint(0, 0), size_), Qt::transparent);
  }
#endif
  else if (bufferType() == BufferType::IMAGE) {
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

#ifdef CQCHARTS_OPENGL
    if (bufferType() == BufferType::OPENGL) {
      glBuffer_->release();

      *image_ = glBuffer_->toImage();
    }
#endif

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
  if      (bufferType() == BufferType::PIXMAP) {
    if (pixmap_)
      pixmap_->fill(Qt::transparent);
  }
#ifdef CQCHARTS_OPENGL
  else if (bufferType() == BufferType::OPENGL) {
    //if (glBuffer_)
    //  glBuffer_->fill(Qt::transparent);
  }
#endif
  else if (bufferType() == BufferType::IMAGE) {
    if (image_)
      image_->fill(Qt::transparent);
  }
}

void
CQChartsBuffer::
draw(QPainter *painter)
{
  draw(painter, int(rect_.x()), int(rect_.y()));
}

void
CQChartsBuffer::
draw(QPainter *painter, int x, int y)
{
//std::cerr << "draw: " << typeName(type_) << "\n";
  if      (bufferType() == BufferType::PIXMAP) {
    assert(pixmap_);

    painter->drawPixmap(x, y, *pixmap_);
  }
#ifdef CQCHARTS_OPENGL
  else if (bufferType() == BufferType::OPENGL) {
    assert(glBuffer_);

    painter->drawImage(x, y, *image_);
  }
#endif
  else if (bufferType() == BufferType::IMAGE) {
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
  auto fsize = rect_.size();

  QSize size(CMathRound::RoundUp(fsize.width()), CMathRound::RoundUp(fsize.height()));

  bool hasDrawable = false;

  if      (bufferType() == BufferType::PIXMAP) {
    hasDrawable = (pixmap_ != nullptr);
  }
#ifdef CQCHARTS_OPENGL
  else if (bufferType() == BufferType::OPENGL) {
    hasDrawable = (glBuffer_ != nullptr);

    if (hasDrawable && glWindow_->thread() != QThread::currentThread())
      hasDrawable = false;
  }
#endif
  else if (bufferType() == BufferType::IMAGE) {
    hasDrawable = (image_ != nullptr);
  }

  if (! hasDrawable || size_ != size) {
    delete image_;
    delete pixmap_;

#ifdef CQCHARTS_OPENGL
    delete glBuffer_;
    delete glDevice_;
    delete glWindow_;
    delete glContext_;
#endif

    delete ipainter_;

    size_ = size;

    if      (bufferType() == BufferType::PIXMAP) {
      pixmap_ = new QPixmap(size_);
    }
#ifdef CQCHARTS_OPENGL
    else if (bufferType() == BufferType::OPENGL) {
      QSurfaceFormat format;

      format.setMajorVersion(3);
      format.setMinorVersion(3);

      glWindow_ = new QWindow;

      glWindow_->setSurfaceType(QWindow::OpenGLSurface);
      glWindow_->setFormat(format);
      glWindow_->create();

      glContext_ = new QOpenGLContext();

      glContext_->setFormat(format);

      if (! glContext_->create())
        std::cerr << "Cannot create GL context\n";

      glContext_->makeCurrent(glWindow_);

      QOpenGLFramebufferObjectFormat fbFormat;

      fbFormat.setSamples(16);
      fbFormat.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);

      glBuffer_ = new QOpenGLFramebufferObject(size_, fbFormat);
      glDevice_ = new QOpenGLPaintDevice(size_);

      image_ = CQChartsUtil::newImage(size_);

      image_->fill(Qt::transparent);
    }
#endif
    else if (bufferType() == BufferType::IMAGE) {
      image_ = CQChartsUtil::newImage(size_);

      image_->fill(Qt::transparent);
    }

    ipainter_ = nullptr;

    setValid(false);
  }
}
