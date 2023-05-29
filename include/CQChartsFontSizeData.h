#ifndef CQChartsFontSizeData_H
#define CQChartsFontSizeData_H

#include <CQChartsMapColumnData.h>
#include <CQChartsUnits.h>

//! \brief font size (column) data
class CQChartsFontSizeData : public CQChartsMapColumnData<double> {
 public:
  using Units = CQChartsUnits;

  CQChartsFontSizeData() = default;

  const Units &units() const { return units_; }
  void setUnits(const Units &u) { units_ = u; setValid(false); }

  const Qt::Orientation &direction() const { return direction_; }
  void setDirection(const Qt::Orientation &d) { direction_ = d; setValid(false); }

 private:
  Units           units_     { Units::Type::PIXEL }; //!< mapped size units
  Qt::Orientation direction_ { Qt::Horizontal };     //!< mapped size direction
};

#endif
