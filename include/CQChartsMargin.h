#ifndef CQChartsMargin_H
#define CQChartsMargin_H

#include <CQChartsLength.h>
#include <CQUtilMeta.h>

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

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsMargin, metaTypeId)

 public:
  using Margin = CQChartsMargin;
  using Length = CQChartsLength;
  using Units  = CQChartsUnits::Type;

 public:
  static Margin pixel(double l, double t, double r, double b) {
    return Margin(Length::pixel(l), Length::pixel(t), Length::pixel(r), Length::pixel(b));
  }

 public:
  explicit CQChartsMargin(const QString &str) {
    setValue(str);
  }

  CQChartsMargin() = default;

  CQChartsMargin(const Length &l, const Length &t, const Length &r, const Length &b) {
    set(l, t, r, b);
  }

  CQChartsMargin(double l, double t, double r, double b) {
    set(Length::percent(l), Length::percent(t), Length::percent(r), Length::percent(b));
  }

  explicit CQChartsMargin(const Length &l) {
    set(l);
  }

  explicit CQChartsMargin(double l) {
    set(Length::percent(l));
  }

  virtual ~CQChartsMargin() = default;

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

  friend bool operator==(const Margin &lhs, const Margin &rhs) {
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

CQUTIL_DCL_META_TYPE(CQChartsMargin)

#endif
