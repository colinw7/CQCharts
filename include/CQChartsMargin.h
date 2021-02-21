#ifndef CQChartsMargin_H
#define CQChartsMargin_H

#include <CQChartsLength.h>
#include <QStringList>

/*!
 * \brief plot margin
 * \ingroup Charts
 */
class CQChartsMargin :
  public CQChartsEqBase<CQChartsMargin> {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  using Length = CQChartsLength;
  using Units  = CQChartsUnits;

 public:
  explicit CQChartsMargin(const QString &str) {
    setValue(str);
  }

  CQChartsMargin() { }

  CQChartsMargin(const Length &l, const Length &t, const Length &r, const Length &b) {
    set(l, t, r, b);
  }

  CQChartsMargin(double l, double t, double r, double b) {
    set(Length(l, Units::PERCENT), Length(t, Units::PERCENT),
        Length(r, Units::PERCENT), Length(b, Units::PERCENT));
  }

  explicit CQChartsMargin(const Length &l) {
    set(l);
  }

  explicit CQChartsMargin(double l) {
    set(Length(l, Units::PERCENT));
  }

  virtual ~CQChartsMargin() { }

  //---

  void set(const Length &l, const Length &t, const Length &r, const Length &b) {
    left_   = l;
    top_    = t;
    right_  = r;
    bottom_ = b;
  }

  void set(const Length &l) {
    left_   = l;
    top_    = l;
    right_  = l;
    bottom_ = l;
  }

  bool setValue(const QString &str) { return fromString(str); }

  //---

  bool isValid() const { return left ().isValid() && top   ().isValid() &&
                                right().isValid() && bottom().isValid(); }

  //---

  const Length &left() const { return left_; }
  void setLeft(const Length &v) { left_ = v; }

  const Length &top() const { return top_; }
  void setTop(const Length &v) { top_ = v; }

  const Length &right() const { return right_; }
  void setRight(const Length &v) { right_ = v; }

  const Length &bottom() const { return bottom_; }
  void setBottom(const Length &v) { bottom_ = v; }

  //---

  QString toString() const;

  bool fromString(const QString &s);

  //---

  friend bool operator==(const CQChartsMargin &lhs, const CQChartsMargin &rhs) {
    return (lhs.left_  == rhs.left_  && lhs.top_    == rhs.top_ &&
            lhs.right_ == rhs.right_ && lhs.bottom_ == rhs.bottom_);
  }

 protected:
  Length left_;
  Length top_;
  Length right_;
  Length bottom_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsMargin)

#endif
