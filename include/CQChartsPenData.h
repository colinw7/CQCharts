#ifndef CQChartsPenData_H
#define CQChartsPenData_H

#include <CQChartsAlpha.h>
#include <CQChartsLength.h>
#include <CQChartsLineDash.h>

/*!
 * \brief Pen Data
 *
 * visible, color, alpha, width, dash
 */
class CQChartsPenData {
 public:
  using Alpha    = CQChartsAlpha;
  using Length   = CQChartsLength;
  using LineDash = CQChartsLineDash;
  using LineCap  = CQChartsLineCap;
  using LineJoin = CQChartsLineJoin;

 public:
  CQChartsPenData() = default;

  explicit CQChartsPenData(bool visible, const QColor &color=QColor(), const Alpha &alpha=Alpha(),
                           const Length &width=Length(), const LineDash &dash=LineDash(),
                           const LineCap &lineCap=LineCap(), const LineJoin &lineJoin=LineJoin()) :
   visible_(visible), color_(color), alpha_(alpha), width_(width),
   dash_(dash), lineCap_(lineCap), lineJoin_(lineJoin) {
  }

  CQChartsPenData(bool visible, const QColor &color, const CQChartsStrokeData &strokeData) :
   visible_(visible), color_(color), alpha_(strokeData.alpha()), width_(strokeData.width()),
   dash_(strokeData.dash()) {
  }

  //---

  //! get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  //! get/set color
  const QColor &color() const { return color_; }
  void setColor(const QColor &v) { color_ = v; }

  //! get/set alpha
  const Alpha &alpha() const { return alpha_; }
  void setAlpha(const Alpha &a) { alpha_ = a; }

  //! get/set width
  const Length &width() const { return width_; }
  void setWidth(const Length &v) { width_ = v; }

  //! get/set line dash
  const LineDash &dash() const { return dash_; }
  void setDash(const LineDash &v) { dash_ = v; }

  //! get/set line cap
  const LineCap &lineCap() const { return lineCap_; }
  void setLineCap(const LineCap &c) { lineCap_ = c; }

  //! get/set line join
  const LineJoin &lineJoin() const { return lineJoin_; }
  void setLineJoin(const LineJoin &j) { lineJoin_ = j; }

 private:
  bool     visible_  { true };  //!< visible
  QColor   color_;              //!< pen color
  Alpha    alpha_;              //!< pen alpha
  Length   width_    { "0px" }; //!< pen width
  LineDash dash_;               //!< pen line dash
  LineCap  lineCap_;            //!< pen line cap
  LineJoin lineJoin_;           //!< pen line join
};

#endif
