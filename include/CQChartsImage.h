#ifndef CQChartsImage_H
#define CQChartsImage_H

#include <CQChartsTmpl.h>
#include <QString>
#include <QImage>
#include <QIcon>
#include <cassert>

/*!
 * \brief Image Data
 * \ingroup Charts
 */
class CQChartsImage :
  public CQChartsEqBase<CQChartsImage> {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  enum class Type {
    NONE,
    IMAGE,
    ICON,
    SVG
  };

 public:
  CQChartsImage() = default;

  CQChartsImage(const CQChartsImage &image);

  explicit CQChartsImage(const QImage &image);
  explicit CQChartsImage(const QString &s, Type type=Type::NONE);

 ~CQChartsImage();

  CQChartsImage &operator=(const CQChartsImage &image);

  //---

  const QImage &image() const;

  Type type() const { return type_; }

  const QString &fileName() const { return fileName_; }

  //---

  bool isValid() const { return fileName_.length() || ! image_.isNull(); }

  int width () const;
  int height() const;

  QSize size() const { return QSize(width(), height()); }

  //---

  QString id() const;
  void setId(const QString &id);

  //---

  QImage sizedImage(int w, int h) const;

  //---

  QString toString() const;

  bool fromString(const QString &s, Type type=Type::NONE);

  //---

  friend bool operator==(const CQChartsImage &lhs, const CQChartsImage &rhs) {
    return ((lhs.type() == rhs.type()) && (lhs.fileName() == rhs.fileName()));
  }

  //---

  void setImageType(const QString &type);

 private:
  QImage           image_;
  QIcon            icon_;
  Type             type_    { Type::NONE };
  QString          fileName_;
  mutable QPixmap* pixmap_  { nullptr };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsImage)

#endif
