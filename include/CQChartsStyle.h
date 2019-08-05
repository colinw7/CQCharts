#ifndef CQChartsStyle_H
#define CQChartsStyle_H

#include <QBrush>
#include <QPen>
#include <iostream>

class CQChartsPlot;

/*!
 * \brief Pen/Brush Style
 * \ingroup Charts
 */
class CQChartsStyle {
 public:
  static void registerMetaType();

  static int metaTypeId;

 public:
  CQChartsStyle() = default;

  explicit CQChartsStyle(const QString &str) {
    (void) fromString(str);
  }

  CQChartsStyle(const CQChartsStyle &rhs) {
    if (! rhs.pen_) return;

    pen_   = new QPen  (*rhs.pen_  );
    brush_ = new QBrush(*rhs.brush_);
  }

 ~CQChartsStyle() {
    delete pen_;
    delete brush_;
  }

  CQChartsStyle &operator=(const CQChartsStyle &rhs) {
    delete pen_;
    delete brush_;

    if (rhs.pen_) {
      pen_   = new QPen  (*rhs.pen_  );
      brush_ = new QBrush(*rhs.brush_);
    }
    else {
      pen_   = nullptr;
      brush_ = nullptr;
    }

    return *this;
  }

  //---

  bool isValid() const { return pen_ && brush_; }

  //---

  QPen   &pen  () { return *penPtr  (); }
  QBrush &brush() { return *brushPtr(); }

  const QPen   &pen  () const { return *const_cast<CQChartsStyle *>(this)->penPtr  (); }
  const QBrush &brush() const { return *const_cast<CQChartsStyle *>(this)->brushPtr(); }

  //---

  QString toString() const;

  bool fromString(const QString &s);

  //---

  friend bool operator==(const CQChartsStyle &lhs, const CQChartsStyle &rhs) {
    if (! lhs.pen_ || ! rhs.pen_) {
      if (lhs.pen_ == rhs.pen_ && lhs.brush_ == rhs.brush_) return true;
    }
    else {
      if (*lhs.pen_ == *rhs.pen_ && *lhs.brush_ == *rhs.brush_) return true;
    }

    return false;
  }

  friend bool operator!=(const CQChartsStyle &lhs, const CQChartsStyle &rhs) {
    return ! operator==(lhs, rhs);
  }

  //---

  void print(std::ostream &os) const {
    os << toString().toStdString();
  }

  friend std::ostream &operator<<(std::ostream &os, const CQChartsStyle &c) {
    c.print(os);

    return os;
  }

  //---

 private:
  QPen *penPtr() {
    static QPen s_pen;

    if (! pen_)
      return &s_pen;

    return pen_;
  }

  QBrush *brushPtr() {
    static QBrush s_brush;

    if (! brush_)
      return &s_brush;

    return brush_;
  }

 private:
  QPen*   pen_   { nullptr };
  QBrush* brush_ { nullptr };
};

//---

#include <CQUtilMeta.h>

CQUTIL_DCL_META_TYPE(CQChartsStyle)

#endif
