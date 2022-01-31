#ifndef CQChartsLineDash_H
#define CQChartsLineDash_H

#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>
#include <CLineDash.h>

#include <QVector>
#include <QVariant>

#include <sys/types.h>

#include <vector>
#include <sstream>

/*!
 * \brief line dash
 * \ingroup Charts
 */
class CQChartsLineDash :
  public CQChartsEqBase<CQChartsLineDash>,
  public CQChartsToStringBase<CQChartsLineDash> {
 public:
  using QLengths = QVector<qreal>;
  using Lengths  = std::vector<double>;

 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsLineDash, metaTypeId)

 public:
  CQChartsLineDash();

  CQChartsLineDash(const CQChartsLineDash &dash);

  explicit CQChartsLineDash(const CLineDash &dash);

  explicit CQChartsLineDash(const Lengths &lengths, double offset=0.0);

  explicit CQChartsLineDash(ushort pattern);

  explicit CQChartsLineDash(const QString &str);

 ~CQChartsLineDash() { }

  CQChartsLineDash &operator=(const CQChartsLineDash &dash);

  //---

  friend bool operator==(const CQChartsLineDash &lhs, const CQChartsLineDash &rhs) {
    return (lhs.cmp(rhs) == 0);
  }

  int cmp(const CQChartsLineDash &d) const;

  //---

  bool isSet() const { return set_; }

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

  //---

  QString toString() const;

  bool fromString(const QString &str);

  //---

  bool isValid() const { return true; }

 private:
  void init(const Lengths &lengths, double offset);
  void init();

 private:
  bool     set_    { false };
  double   offset_ { 0.0 };
  QLengths lengths_;
};

//------

CQUTIL_DCL_META_TYPE(CQChartsLineDash)

#endif
