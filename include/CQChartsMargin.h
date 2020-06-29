#ifndef CQChartsMargin_H
#define CQChartsMargin_H

#include <CQChartsLength.h>
#include <QStringList>

/*!
 * \brief plot margin
 * \ingroup Charts
 */
class CQChartsMargin {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  explicit CQChartsMargin(const QString &str) {
    setValue(str);
  }

  CQChartsMargin() { }

  CQChartsMargin(const CQChartsLength &l, const CQChartsLength &t,
                 const CQChartsLength &r, const CQChartsLength &b) {
    set(l, t, r, b);
  }

  CQChartsMargin(double l, double t, double r, double b) {
    set(CQChartsLength(l, CQChartsUnits::PERCENT),
        CQChartsLength(t, CQChartsUnits::PERCENT),
        CQChartsLength(r, CQChartsUnits::PERCENT),
        CQChartsLength(b, CQChartsUnits::PERCENT));
  }

  explicit CQChartsMargin(const CQChartsLength &l) {
    set(l);
  }

  explicit CQChartsMargin(double l) {
    set(CQChartsLength(l, CQChartsUnits::PERCENT));
  }

  virtual ~CQChartsMargin() { }

  //---

  void set(const CQChartsLength &l, const CQChartsLength &t,
           const CQChartsLength &r, const CQChartsLength &b) {
    left_   = l;
    top_    = t;
    right_  = r;
    bottom_ = b;
  }

  void set(const CQChartsLength &l) {
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

  const CQChartsLength &left() const { return left_; }
  void setLeft(const CQChartsLength &v) { left_ = v; }

  const CQChartsLength &top() const { return top_; }
  void setTop(const CQChartsLength &v) { top_ = v; }

  const CQChartsLength &right() const { return right_; }
  void setRight(const CQChartsLength &v) { right_ = v; }

  const CQChartsLength &bottom() const { return bottom_; }
  void setBottom(const CQChartsLength &v) { bottom_ = v; }

  //---

  QString toString() const;

  bool fromString(const QString &s);

  //---

  friend bool operator==(const CQChartsMargin &lhs, const CQChartsMargin &rhs) {
    return (lhs.left_  == rhs.left_  && lhs.top_    == rhs.top_ &&
            lhs.right_ == rhs.right_ && lhs.bottom_ == rhs.bottom_);
  }

  friend bool operator!=(const CQChartsMargin &lhs, const CQChartsMargin &rhs) {
    return ! operator==(lhs, rhs);
  }

 protected:
  CQChartsLength left_;
  CQChartsLength top_;
  CQChartsLength right_;
  CQChartsLength bottom_;
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsMargin)

#endif
