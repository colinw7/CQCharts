#ifndef CQChartsImage_H
#define CQChartsImage_H

#include <QString>
#include <QImage>
#include <cassert>

/*!
 * \brief Image Data
 * \ingroup Charts
 */
class CQChartsImage {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsImage() = default;
  CQChartsImage(const QImage &image);
  CQChartsImage(const QString &s);

  bool isValid() const { return fileName_.length() || ! image_.isNull(); }

  //---

  const QString &fileName() const { return fileName_; }

  const QImage &image() const { return image_; }

  int width() const {
    if (! image_.isNull()) return image_.width();
    return 100;
  }

  int height() const {
    if (! image_.isNull()) return image_.height();
    return 100;
  }

  QSize size() const { return QSize(width(), height()); }

  //---

  QString id() const {
    return image_.text("id");
  }

  void setId(const QString &id) {
    image_.setText("id", id);
  }

  //---

  QString toString() const;

  bool fromString(const QString &s);

  //---

  friend bool operator==(const CQChartsImage &lhs, const CQChartsImage &rhs) {
    return (lhs.fileName_ == rhs.fileName_);
  }

  friend bool operator!=(const CQChartsImage &lhs, const CQChartsImage &rhs) {
    return ! operator==(lhs, rhs);
  }

 private:
  QImage  image_;
  QString fileName_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsImage)

#endif
