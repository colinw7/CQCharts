#ifndef CQChartsSymbolSizeData_H
#define CQChartsSymbolSizeData_H

#include <CQChartsColumn.h>
#include <CQChartsUnits.h>
#include <CQChartsSymbolSizeMap.h>

//! \brief symbol size (column) data
class CQChartsSymbolSizeData {
 public:
  using Column        = CQChartsColumn;
  using Units         = CQChartsUnits;
  using SymbolSizeMap = CQChartsSymbolSizeMap;

  CQChartsSymbolSizeData() { }

  bool isValid() const { return valid_; }
  void setValid(bool b) { valid_ = b; }

  const Column &column() const { return column_; }
  void setColumn(const Column &c) { column_ = c; setValid(false); }

  bool isMapped() const { return mapped_; }
  void setMapped(bool b) { mapped_ = b; setValid(false); }

  double dataMin() const { return data_min_; }
  void setDataMin(double l) { data_min_ = l; setValid(false); }

  double dataMax() const { return data_max_; }
  void setDataMax(double l) { data_max_ = l; setValid(false); }

  double dataMean() const { return data_mean_; }
  void setDataMean(double l) { data_mean_ = l; setValid(false); }

  void setDataRange(double min, double max) {
    data_min_ = min; data_max_ = max; setValid(false);
  }

  double mapMin() const { return map_min_; }
  void setMapMin(double l) { map_min_ = l; setValid(false); }

  double mapMax() const { return map_max_; }
  void setMapMax(double l) { map_max_ = l; setValid(false); }

  void setMapRange(double min, double max) {
    map_min_ = min; map_max_ = max; setValid(false);
  }

  double userMapMin() const { return user_map_min_; }
  void setUserMapMin(double l) { user_map_min_ = l; setValid(false); }

  double userMapMax() const { return user_map_max_; }
  void setUserMapMax(double l) { user_map_max_ = l; setValid(false); }

  const Units &units() const { return units_; }
  void setUnits(const Units &u) { units_ = u; setValid(false); }

  const Qt::Orientation &direction() const { return direction_; }
  void setDirection(const Qt::Orientation &d) { direction_ = d; setValid(false); }

  const SymbolSizeMap &sizeMap() const { return sizeMap_; }
  void setSizeMap(const SymbolSizeMap &m) { sizeMap_ = m; setValid(false); }

 private:
  Column          column_;                              //!< symbol size column
  bool            valid_        { false };              //!< symbol size valid
  bool            mapped_       { false };              //!< symbol size values mapped
  double          data_min_     { 0.0 };                //!< model data min
  double          data_max_     { 1.0 };                //!< model data max
  double          data_mean_    { 0.0 };                //!< model data mean
  double          map_min_      { 0.0 };                //!< mapped size min
  double          map_max_      { 1.0 };                //!< mapped size max
  double          user_map_min_ { 0.0 };                //!< user specified mapped size min
  double          user_map_max_ { 1.0 };                //!< user specified mapped size max
  Units           units_        { Units::Type::PIXEL }; //!< mapped size units
  Qt::Orientation direction_    { Qt::Horizontal };     //!< mapped size direction
  SymbolSizeMap   sizeMap_;                             //!< symbol size map
};

#endif
