#include <CQChartsImage.h>
#include <CQCharts.h>
#include <CQPropertyView.h>

#include <QSvgRenderer>
#include <QPainter>

#include <future>

class CQChartsImageMgr {
 public:
  static CQChartsImageMgr *instance() {
    static CQChartsImageMgr *inst;

    if (! inst)
      inst = new CQChartsImageMgr;

    return inst;
  }

  QImage getImage(const QString &filename) {
    std::unique_lock<std::mutex> lock(mutex_);

    auto p = namedImages_.find(filename);

    return (p != namedImages_.end() ? (*p).second : QImage());
  }

  QImage loadImage(const QString &filename) {
    std::unique_lock<std::mutex> lock(mutex_);

    auto p = namedImages_.find(filename);

    if (p == namedImages_.end()) {
      QImage image;

      if (! image.load(filename))
        return QImage();

      p = namedImages_.emplace_hint(p, filename, image);
    }

    return (*p).second;
  }

  void setImage(const QString &name, const QImage &image) {
    std::unique_lock<std::mutex> lock(mutex_);

    namedImages_[name] = image;
  }

  uint numImages() const { return uint(namedImages_.size()); }

 private:
  using NamedImages = std::map<QString, QImage>;

  NamedImages namedImages_;
  std::mutex  mutex_;
};

#define CQChartsImageMgrInst CQChartsImageMgr::instance()

//---

class CQChartsIconMgr {
 public:
  static CQChartsIconMgr *instance() {
    static CQChartsIconMgr *inst;

    if (! inst)
      inst = new CQChartsIconMgr;

    return inst;
  }

  QIcon getIcon(const QString &filename) {
    std::unique_lock<std::mutex> lock(mutex_);

    auto p = namedIcons_.find(filename);

    return (p != namedIcons_.end() ? (*p).second : QIcon());
  }

  QIcon loadIcon(const QString &filename) {
    std::unique_lock<std::mutex> lock(mutex_);

    auto p = namedIcons_.find(filename);

    if (p == namedIcons_.end()) {
      QIcon icon(filename);

      if (icon.isNull())
        return QIcon();

      p = namedIcons_.emplace_hint(p, filename, icon);
    }

    return (*p).second;
  }

  void setIcon(const QString &name, const QIcon &icon) {
    std::unique_lock<std::mutex> lock(mutex_);

    namedIcons_[name] = icon;
  }

  uint numIcons() const { return uint(namedIcons_.size()); }

 private:
  using NamedIcons = std::map<QString, QIcon>;

  NamedIcons namedIcons_;
  std::mutex mutex_;
};

#define CQChartsIconMgrInst CQChartsIconMgr::instance()

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
    filename_ = QString("@image.%1").arg(CQChartsImageMgrInst->numImages() + 1);

    CQChartsImageMgrInst->setImage(filename_, image);
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
    auto image = CQChartsImageMgrInst->getImage(filename_);

    int w = (! image.isNull() ? image.width () : 100);
    int h = (! image.isNull() ? image.height() : 100);

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

      w = int(CQChartsUtil::toInt(wstr, ok)); if (! ok) w = -1;
      h = int(CQChartsUtil::toInt(wstr, ok)); if (! ok) h = -1;
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
      image_ = CQChartsImageMgrInst->loadImage(filename_);

      if (! image_.isNull())
        resolved_ = true;

      if (w > 0 && h > 0) {
        if (! image_.isNull())
          image_ = image_.scaled(int(w), int(h), Qt::IgnoreAspectRatio);
      }
    }
  }
  else if (type_ == Type::ICON) {
    icon_ = QIcon();

    if (filename_ != "") {
      icon_ = CQChartsIconMgrInst->loadIcon(filename_);

      if (! icon_.isNull())
        resolved_ = true;
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
    auto filename1 = charts->lookupFile(filename_);

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
