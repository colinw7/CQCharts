#include <CQPixmapCache.h>
#include <QIconEngine>
#include <QPainter>
#include <cassert>
#include <iostream>

class CQPixmapIcon : public QIconEngine {
 public:
  CQPixmapIcon(const QString &lightId, const QString &darkId) :
   lightId_(lightId), darkId_(darkId), dark_(CQPixmapCacheInst->isDark()) {
    updateIcon();
  }

  void updateIcon() {
    if (dark_ && darkId_.length())
      icon_ = CQPixmapCacheInst->getPixmapIcon(darkId_);
    else
      icon_ = CQPixmapCacheInst->getPixmapIcon(lightId_);
  }

  QSize actualSize(const QSize &size, QIcon::Mode, QIcon::State) {
    return size;
  }

  QString iconName() const {
    if (dark_ && darkId_.length())
      return darkId_;

    return lightId_;
  }

  void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) {
    painter->drawPixmap(0, 0, pixmap(rect.size(), mode, state));
  }

  QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) {
    bool dark = CQPixmapCacheInst->isDark();

    if (dark_ != dark) {
      dark_ = dark;

      if (darkId_.length())
        updateIcon();
    }

    if (size != state_.size || mode != state_.mode || state != state_.state) {
      pixmap_ = icon_.pixmap(size, mode, state);
    }

    return pixmap_;
  }

  QIconEngine *clone() const {
    return new CQPixmapIcon(lightId_, darkId_);
  }

 private:
  struct State {
    QSize        size;
    QIcon::Mode  mode;
    QIcon::State state;
  };

  QString lightId_;
  QString darkId_;
  QIcon   icon_;
  QPixmap pixmap_;
  State   state_;
  bool    dark_ { false };
};

//------

static CQPixmapCache *s_inst;

CQPixmapCache *
CQPixmapCache::
instance()
{
  if (! s_inst)
    s_inst = new CQPixmapCache;

  return s_inst;
}

void
CQPixmapCache::
release()
{
  delete s_inst;

  s_inst = nullptr;
}

CQPixmapCache::
CQPixmapCache()
{
}

CQPixmapCache::
~CQPixmapCache()
{
}

void
CQPixmapCache::
clear()
{
  idData_.clear();
}

void
CQPixmapCache::
addData(const QString &id, const uchar *data, int len)
{
  idData_[id] = Data(data, len);
}

const QPixmap &
CQPixmapCache::
getPixmap(const QString &id)
{
  auto p = idData_.find(id);

  if (p == idData_.end())
    std::cout << "Pixmap not found: " << id.toStdString() << std::endl;

  assert(p != idData_.end());

  if (! (*p).second.pixmap) {
    (*p).second.pixmap = new QPixmap;

    bool rc = (*p).second.pixmap->loadFromData((*p).second.data, (*p).second.len);

    assert(rc);
  }

  return *(*p).second.pixmap;
}

QPixmap
CQPixmapCache::
getSizedPixmap(const QString &id, const QSize &s)
{
  return getPixmap(id).scaled(s, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
}

QIcon
CQPixmapCache::
getIcon(const QString &lightId, const QString &darkId)
{
  return QIcon(new CQPixmapIcon(lightId, darkId));
}

QIcon
CQPixmapCache::
getPixmapIcon(const QString &id)
{
  return QIcon(getPixmap(id));
}
