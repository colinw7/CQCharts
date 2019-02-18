#ifndef CQChartsLineDash_H
#define CQChartsLineDash_H

#include <QPen>

#include <sys/types.h>

#include <vector>
#include <sstream>

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

  const QPen &pen() const { return pen_; }

  double   getOffset    () const { return pen_.dashOffset(); }
  QLengths getLengths   () const { return pen_.dashPattern(); }
  int      getNumLengths() const { return pen_.dashPattern().size(); }

  double getLength(int i) const { return pen_.dashPattern()[i]; }

  void setOffset(double offset) { pen_.setDashOffset(offset); }

  bool isSolid() const { return pen_.dashPattern().empty(); }

  void setDashes(const Lengths &lengths, double offset=0.0);

  void setDashes(ushort pattern);

  QString toString() const;

  bool fromString(const QString &str);

  bool isValid() const { return true; }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsLineDash &dash) {
    os << dash.toString().toStdString();

    return os;
  }

 private:
  void init();

 private:
  QPen pen_;
};

//------

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsLineDash)

#endif
