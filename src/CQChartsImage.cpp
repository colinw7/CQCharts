#include <CQChartsImage.h>
#include <CQPropertyView.h>

#include <QSvgRenderer>
#include <QPainter>

CQUTIL_DEF_META_TYPE(CQChartsImage, toString, fromString)

int CQChartsImage::metaTypeId;

//---

void
CQChartsImage::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsImage);

  CQPropertyViewMgrInst->setUserName("CQChartsImage", "symbol");
}

CQChartsImage::
CQChartsImage(const QImage &image) :
 image_(image), type_(Type::IMAGE)
{
  fileName_ = image.text("filename");

  image_.setText("", fileName_);
}

CQChartsImage::
CQChartsImage(const CQChartsImage &image) :
 image_(image.image_), icon_(image.icon_), type_(image.type_), fileName_(image.fileName_)
{
  if (! image_.isNull())
    image_.setText("", fileName_);
}

CQChartsImage::
CQChartsImage(const QString &s, Type type)
{
  fromString(s, type);
}

CQChartsImage::
~CQChartsImage()
{
  delete pixmap_;
}

CQChartsImage &
CQChartsImage::
operator=(const CQChartsImage &image)
{
  image_    = image.image_;
  icon_     = image.icon_  ;
  type_     = image.type_;
  fileName_ = image.fileName_;

  if (! image_.isNull())
    image_.setText("", fileName_);

  return *this;
}

const QImage &
CQChartsImage::
image() const
{
  return image_;
}

QImage
CQChartsImage::
sizedImage(int w, int h) const
{
  if      (type_ == Type::ICON) {
    auto pixmap = icon_.pixmap(w, h);

    return pixmap.toImage();
  }
  else if (type_ == Type::SVG) {
    if (! pixmap_ || pixmap_->width() != w || pixmap_->height()) {
      delete pixmap_;

      QSvgRenderer renderer;

      renderer.load(fileName_);

      pixmap_ = new QPixmap(w, h);

      pixmap_->fill(Qt::transparent);

      QPainter painter(pixmap_);

      renderer.render(&painter);
    }

    return pixmap_->toImage();
  }
  else {
    return image_.scaled(int(w), int(h), Qt::IgnoreAspectRatio);
  }
}

int
CQChartsImage::
width() const
{
  if (! image_.isNull())
    return image_.width();

  return 100;
}

int
CQChartsImage::
height() const
{
  if (! image_.isNull())
    return image_.height();

  return 100;
}

QString
CQChartsImage::
id() const
{
  return image_.text("id");
}

void
CQChartsImage::
setId(const QString &id)
{
  image_.setText("id", id);
}

QString
CQChartsImage::
toString() const
{
  if      (type() == Type::ICON)
    return "icon:" + fileName_;
  else if (type() == Type::ICON)
    return "svg:" + fileName_;
  else
    return fileName_;
}

bool
CQChartsImage::
fromString(const QString &s, Type type)
{
  auto s1 = s;

  if (type == Type::NONE) {
    auto pos = s1.indexOf(':');

    if (pos > 0) {
      auto typeName = s1.mid(0, pos);

      if      (typeName == "icon") {
        type = Type::ICON;
        s1   = s1.mid(pos + 1);
      }
      else if (typeName == "svg") {
        type = Type::SVG;
        s1   = s1.mid(pos + 1);
      }
      else
        type = Type::IMAGE;
    }
    else
      type = Type::IMAGE;
  }

  fileName_ = s1;
  type_     = type;

  if (type_ == Type::IMAGE) {
    image_ = QImage();

    if (fileName_ != "")
      image_.load(fileName_);
  }
  else {
    icon_ = QIcon();

    if (fileName_ != "")
      icon_ = QIcon(fileName_);
  }

  image_.setText("", fileName_);

  return true;
}

void
CQChartsImage::
setImageType(const QString &type)
{
  image_.setText("imageType", type);
}
