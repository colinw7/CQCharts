#include <CQPixmapCache.h>
#include <cassert>
#include <iostream>

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
getIcon(const QString &id)
{
  return QIcon(getPixmap(id));
}
