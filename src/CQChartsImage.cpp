#include <CQChartsImage.h>
#include <CQCharts.h>
#include <CQPropertyView.h>

#include <QSvgRenderer>
#include <QPainter>

using NamedImages = std::map<QString, QImage>;
using NamedIcons  = std::map<QString, QIcon>;

static NamedImages s_namedImages;
static NamedIcons  s_namedIcons;

//---

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
  filename_ = image.text("filename");

  if (filename_ == "") {
    filename_ = QString("@image.%1").arg(s_namedImages.size() + 1);

    s_namedImages[filename_] = image;
  }

  image_.setText("", filename_);
}

CQChartsImage::
CQChartsImage(const CQChartsImage &image) :
 image_(image.image_), icon_(image.icon_), type_(image.type_),
 filename_(image.filename_), resolved_(image.resolved_)
{
  if (! image_.isNull())
    image_.setText("", filename_);
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
  delete pixmap_;

  pixmap_ = nullptr;

  image_    = image.image_;
  icon_     = image.icon_;
  type_     = image.type_;
  filename_ = image.filename_;
  resolved_ = image.resolved_;

  if (! image_.isNull())
    image_.setText("", filename_);

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
  if      (type_ == Type::ICON)
    return iconToImage(icon_, w, h);
  else if (type_ == Type::SVG) {
    return svgToImage(filename_, w, h);
  }
  else {
    if (image_.isNull())
      return image_;

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

void
CQChartsImage::
setSize(const QSize &size)
{
  image_ = sizedImage(size.width(), size.height());
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
  if (! isValid())
    return "";

  if      (type() == Type::ICON) {
    return QString("icon:%1@%2x%3").arg(filename_).arg(width()).arg(height());
  }
  else if (type() == Type::SVG) {
    return QString("svg:%1@%2x%3").arg(filename_).arg(width()).arg(height());
  }
  else if (type() == Type::IMAGE) {
    auto p = s_namedImages.find(filename_);

    int w = (p != s_namedImages.end() ? (*p).second.width () : 100);
    int h = (p != s_namedImages.end() ? (*p).second.height() : 100);

    if (w != width() || h != height())
      return QString("image:%1@%2x%3").arg(filename_).arg(width()).arg(height());
    else
      return QString("image:%1").arg(filename_);
  }
  else
    return "";
}

bool
CQChartsImage::
fromString(const QString &s, Type type)
{
  delete pixmap_;

  pixmap_ = nullptr;

  //---

  // derive type and name from string if not specified
  auto s1 = s;

  int w = -1;
  int h = -1;

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
      else if (typeName == "image") {
        type = Type::IMAGE;
        s1   = s1.mid(pos + 1);
      }
      else
        type = Type::IMAGE;
    }
    else
      type = Type::IMAGE;
  }

  auto pos = s1.indexOf('@');

  if (pos > 0) {
    auto sizeStr = s1.mid(pos + 1);

    auto xpos = sizeStr.indexOf('x');

    if (xpos > 0) {
      auto wstr = sizeStr.mid(0, xpos);
      auto hstr = sizeStr.mid(xpos + 1);

      bool ok;

      w = wstr.toInt(&ok); if (! ok) w = -1;
      h = wstr.toInt(&ok); if (! ok) h = -1;
    }

    s1 = s1.mid(0, pos);
  }

  filename_ = s1;
  type_     = type;

  //---

  // convert name into image
  if      (type_ == Type::IMAGE) {
    image_ = QImage();

    if (filename_ != "") {
      auto p = s_namedImages.find(filename_);

      if (p == s_namedImages.end()) {
        if (image_.load(filename_)) {
          s_namedImages[filename_] = image_;

          resolved_ = true;
        }
      }
      else
        image_ = (*p).second;

      if (w > 0 && h > 0) {
        if (! image_.isNull())
          image_ = image_.scaled(int(w), int(h), Qt::IgnoreAspectRatio);
      }
    }
  }
  else if (type_ == Type::ICON) {
    icon_ = QIcon();

    if (filename_ != "") {
      auto p = s_namedIcons.find(filename_);

      if (p == s_namedIcons.end()) {
        icon_ = QIcon(filename_);

        if (! icon_.isNull()) {
          s_namedIcons[filename_] = icon_;

          resolved_ = true;
        }
      }
      else
        icon_ = (*p).second;
    }

    if (w < 0) w = 100;
    if (h < 0) h = 100;

    image_ = iconToImage(icon_, w, h);
  }
  else if (type_ == Type::SVG) {
    if (filename_ != "") {
      // TODO: config default size
      if (w < 0) w = 100;
      if (h < 0) h = 100;

      image_ = svgToImage(filename_, w, h);

      if (! image_.isNull())
        resolved_ = true;
    }
  }

  image_.setText("", filename_);

  return true;
}

void
CQChartsImage::
resolve(CQCharts *charts) const
{
  if (! resolved_ && charts) {
    QString filename1 = charts->lookupFile(filename_);

    if (filename1 != "") {
      auto *th = const_cast<CQChartsImage *>(this);

      *th = CQChartsImage(filename1);
    }
  }
}

void
CQChartsImage::
setImageType(const QString &type)
{
  image_.setText("imageType", type);
}

QImage
CQChartsImage::
iconToImage(const QIcon &icon, int w, int h) const
{
  // resize pixmap to image
  if (! pixmap_ || pixmap_->width() != w || pixmap_->height()) {
    delete pixmap_;

    pixmap_ = new QPixmap(icon.pixmap(w, h));
  }

  return pixmap_->toImage();
}

// rerender svg into image
QImage
CQChartsImage::
svgToImage(const QString &filename, int w, int h)
{
  QSvgRenderer renderer;

  if (! renderer.load(filename))
    return QImage();

  QPixmap pixmap(w, h);

  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);

  renderer.render(&painter);

  return pixmap.toImage();
}
