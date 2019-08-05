#ifndef CQChartsLineDash_H
#define CQChartsLineDash_H

#include <QPen>
#include <CLineDash.h>

#include <sys/types.h>

#include <vector>
#include <sstream>

/*!
 * \brief line dash
 * \ingroup Charts
 */
class CQChartsLineDash {
 public:
  using QLengths = QVector<qreal>;
  using Lengths  = std::vector<double>;

 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsLineDash();

  CQChartsLineDash(const CQChartsLineDash &dash);

  CQChartsLineDash(const CLineDash &dash);

  CQChartsLineDash(const Lengths &lengths, double offset=0.0);

  explicit CQChartsLineDash(ushort pattern);

  explicit CQChartsLineDash(const QString &str);

 ~CQChartsLineDash() { }

  CQChartsLineDash &operator=(const CQChartsLineDash &dash);

  bool operator==(const CQChartsLineDash &dash) const;

  bool operator!=(const CQChartsLineDash &dash) const {
    return ! operator==(dash);
  }

  void scale(double factor);

  CQChartsLineDash &copy(const CQChartsLineDash &dash);

  double getOffset() const { return offset_; }
  void setOffset(double offset) { offset_ = offset; }

  QLengths getLengths() const { return lengths_; }
  void setLengths(const QLengths &lengths) { lengths_ = lengths; }

  int getNumLengths() const { return lengths_.length(); }

  double getLength(int i) const { return lengths_[i]; }

  bool isSolid() const { return lengths_.length() == 0; }

  void setDashes(const Lengths &lengths, double offset=0.0);

  void setDashes(ushort pattern);

  CLineDash lineDash() const;

  QString toString() const;

  bool fromString(const QString &str);

  bool isValid() const { return true; }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsLineDash &dash) {
    os << dash.toString().toStdString();

    return os;
  }

 private:
  void init(const Lengths &lengths, double offset);
  void init();

 private:
  double   offset_;
  QLengths lengths_;
};

//------

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsLineDash)

#endif
