#ifndef CQChartsSymbolSizeData_H
#define CQChartsSymbolSizeData_H

#include <CQChartsMapColumnData.h>
#include <CQChartsUnits.h>
#include <CQChartsSymbolSizeMap.h>

//! \brief symbol size (column) data
class CQChartsSymbolSizeData : public CQChartsMapColumnData<double> {
 public:
  using Column        = CQChartsColumn;
  using Units         = CQChartsUnits;
  using SymbolSizeMap = CQChartsSymbolSizeMap;

  CQChartsSymbolSizeData() = default;

  const Units &units() const { return units_; }
  void setUnits(const Units &u) { units_ = u; setValid(false); }

  const Qt::Orientation &direction() const { return direction_; }
  void setDirection(const Qt::Orientation &d) { direction_ = d; setValid(false); }

  const SymbolSizeMap &sizeMap() const { return sizeMap_; }
  void setSizeMap(const SymbolSizeMap &m) { sizeMap_ = m; setValid(false); }

 private:
  Units           units_     { Units::Type::PIXEL }; //!< mapped size units
  Qt::Orientation direction_ { Qt::Horizontal };     //!< mapped size direction
  SymbolSizeMap   sizeMap_;                          //!< symbol size map
};

#endif
