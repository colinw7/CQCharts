#ifndef CQChartsStyle_H
#define CQChartsStyle_H

#include <CQChartsTmpl.h>
#include <CQChartsSVGStyleData.h>
#include <CQUtilMeta.h>

#include <QBrush>
#include <QPen>
#include <QVariant>

#include <iostream>

class CQChartsPlot;

/*!
 * \brief Pen/Brush Style
 * \ingroup Charts
 */
class CQChartsStyle :
  public CQChartsEqBase<CQChartsStyle>,
  public CQChartsToStringBase<CQChartsStyle> {
 public:
  using Plot       = CQChartsPlot;
  using StyleData  = CQChartsSVGStyleData;
  using ArrowData  = CQChartsArrowData;
  using Angle      = CQChartsAngle;
  using NameValues = std::map<QString, QString>;

 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsStyle, metaTypeId)

 public:
  CQChartsStyle() = default;

  explicit CQChartsStyle(const QString &str) {
    (void) setValue(str);
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

  const Plot *plot() const { return plot_; }
  void setPlot(Plot *plot) { plot_ = plot; }

  //---

  bool setValue(const QString &s);
  const QString &value() const { return str_; }

  //---

  QPen   &pen  () { return *penPtr  (); }
  QBrush &brush() { return *brushPtr(); }

  const QPen   &pen  () const { return *const_cast<CQChartsStyle *>(this)->penPtr  (); }
  const QBrush &brush() const { return *const_cast<CQChartsStyle *>(this)->brushPtr(); }

  //---

  // text
  const Qt::Alignment &textAlign() const { return styleData_.textAlign; }
  bool isTextScaled() const { return styleData_.textScaled; }
  const QString &textFont() const { return styleData_.textFont; }
  const QString &textAngle() const { return styleData_.textAngle; }

  //---

  // symbol
  const QString &symbolType() const { return styleData_.symbolType; }
  double symbolSize() const { return styleData_.symbolSize; }

  //---

  // arrow
  const ArrowData &arrowData() const { return styleData_.arrowData; }

  //---

  // arc
  const Angle &arcStart() const { return styleData_.arcStart; }
  const Angle &arcDelta() const { return styleData_.arcDelta; }

  const Angle &arcEnd() const { return styleData_.arcEnd; }
  const Angle &arcEndDelta() const { return styleData_.arcEndDelta; }

  double innerRadius() const { return styleData_.innerRadius; }
  double outerRadius() const { return styleData_.outerRadius; }

  //---

  // misc
  const QString &direction() const { return styleData_.direction; }

  const QString &drawType() const { return styleData_.drawType; }

  const QString &tip() const { return styleData_.tip; }

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
  QString str_;

  Plot* plot_ { nullptr };

  QPen*   pen_   { nullptr };
  QBrush* brush_ { nullptr };

  StyleData styleData_;
};

//---

CQUTIL_DCL_META_TYPE(CQChartsStyle)

#endif
