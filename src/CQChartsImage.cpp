#include <CQChartsImage.h>
#include <CQPropertyView.h>

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
CQChartsImage(const QString &s, Type type)
{
  fromString(s, type);
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
  if (type_ == Type::ICON) {
    QPixmap pixmap = icon_.pixmap(w, h);

    return pixmap.toImage();
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
  if (type() == Type::ICON)
    return "icon:" + fileName_;
  else
    return fileName_;
}

bool
CQChartsImage::
fromString(const QString &s, Type type)
{
  QString s1 = s;

  if (type == Type::NONE) {
    auto pos = s1.indexOf(':');

    if (pos > 0) {
      QString typeName = s1.mid(0, pos - 1);

      if (typeName == "icon")
        type = Type::ICON;
      else
        type = Type::IMAGE;
    }
    else
      type = Type::IMAGE;
  }

  fileName_ = s;
  type_     = type;

  if (type_ == Type::IMAGE)
    image_ = QImage(fileName_);
  else
    icon_ = QIcon(fileName_);

  image_.setText("", fileName_);

  return true;
}
