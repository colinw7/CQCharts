#ifndef CQChartsImage_H
#define CQChartsImage_H

#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>

#include <QString>
#include <QImage>
#include <QIcon>
#include <QVariant>

#include <cassert>

class CQCharts;

/*!
 * \brief Image Data
 * \ingroup Charts
 *
 * An image is bitmap (manually resizable) shape supporting:
 *  + PNG, JPEG, ...
 *  + SVG
 */
class CQChartsImage :
  public CQChartsEqBase<CQChartsImage> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsImage, metaTypeId)

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

  const QString &filename() const { return filename_; }

  //---

  bool isValid() const { return filename_.length() || ! image_.isNull(); }

  int width () const;
  int height() const;

  QSize size() const { return QSize(width(), height()); }
  void setSize(const QSize &size);

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
    return ((lhs.type() == rhs.type()) && (lhs.filename() == rhs.filename()));
  }

  //---

  void setImageType(const QString &type);

  //---

  void resolve(CQCharts *charts) const;

 private:
  QImage iconToImage(const QIcon &icon, int w, int h) const;

  static QImage svgToImage(const QString &filename, int w, int h);

 private:
  QImage           image_;                   //!< qimage
  QIcon            icon_;                    //!< qicon
  Type             type_     { Type::NONE }; //!< file type
  QString          filename_;                //!< filename
  bool             resolved_ { false };      //!< resolved
  mutable QPixmap* pixmap_   { nullptr };    //!< cached svg pixmap
};

//---

CQUTIL_DCL_META_TYPE(CQChartsImage)

#endif
