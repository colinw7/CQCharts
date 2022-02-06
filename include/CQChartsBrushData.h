#ifndef CQChartsBrushData_H
#define CQChartsBrushData_H

#include <CQChartsAlpha.h>
#include <CQChartsFillPattern.h>

/*!
 * \brief Brush Data
 *
 * visible, color, alpha, pattern
 */
class CQChartsBrushData {
 public:
  using Alpha       = CQChartsAlpha;
  using FillPattern = CQChartsFillPattern;

 public:
  CQChartsBrushData() = default;

  explicit CQChartsBrushData(bool visible, const QColor &color=QColor(),
                             const Alpha &alpha=Alpha(),
                             const FillPattern &pattern=FillPattern::makeSolid()) :
   visible_(visible), color_(color), alpha_(alpha), pattern_(pattern) {
  }

  CQChartsBrushData(bool visible, const QColor &color, const CQChartsFillData &fillData) :
   visible_(visible), color_(color), alpha_(fillData.alpha()), pattern_(fillData.pattern()) {
  }

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  const Alpha &alpha() const { return alpha_; }
  void setAlpha(const Alpha &a) { alpha_ = a; }

  const FillPattern &pattern() const { return pattern_; }
  void setPattern(const FillPattern &v) { pattern_ = v; }

  //---

 private:
  bool        visible_ { true };                     //!< visible
  QColor      color_;                                //!< fill color
  Alpha       alpha_;                                //!< fill alpha
  FillPattern pattern_ { FillPattern::Type::SOLID }; //!< fill pattern
};

#endif
