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

  static void release();

 public:
 ~CQPixmapCache();

  bool isDark() const { return dark_; }
  void setDark(bool b) { dark_ = b; }

  void clear();

  void addData(const QString &id, const uchar *data, int len);

  const QPixmap &getPixmap(const QString &id);

  QIcon getIcon(const QString &lightId, const QString &darkId="");

  QIcon getPixmapIcon(const QString &id);

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

   ~Data() {
      delete pixmap;
    }
  };

  typedef std::map<QString,Data> IdData;

  IdData idData_;
  bool   dark_ { false };
};

#endif
