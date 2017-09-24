#ifndef CQPixmapCache_H
#define CQPixmapCache_H

#include <QString>
#include <QPixmap>
#include <QIcon>
#include <map>

#define CQPixmapCacheInst CQPixmapCache::instance()

class CQPixmapCache {
 public:
  static CQPixmapCache *instance();

  void addData(const QString &id, const uchar *data, int len);

  const QPixmap &getPixmap(const QString &id);

  QIcon getIcon(const QString &id);

  QPixmap getSizedPixmap(const QString &id, const QSize &s);

 private:
  CQPixmapCache();

 private:
  struct Data {
    const uchar *data   = 0;
    int          len    = 0;
    QPixmap     *pixmap = 0;

    Data(const uchar *data1=0, int len1=0) :
     data(data1), len(len1) {
    }
  };

  typedef std::map<QString,Data> IdData;

  IdData idData_;
};

#endif
