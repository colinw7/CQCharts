#ifndef CQChartsSymbolSizeData_H
#define CQChartsSymbolSizeData_H

#include <CQChartsColumn.h>
#include <CQChartsUnits.h>
#include <CQChartsSymbolSizeMap.h>

//! \brief symbol size (column) data
struct CQChartsSymbolSizeData {
  using Column        = CQChartsColumn;
  using Units         = CQChartsUnits;
  using SymbolSizeMap = CQChartsSymbolSizeMap;

  Column          column;                              //!< symbol size column
  bool            valid        { false };              //!< symbol size valid
  bool            mapped       { false };              //!< symbol size values mapped
  double          data_min     { 0.0 };                //!< model data min
  double          data_max     { 1.0 };                //!< model data max
  double          data_mean    { 0.0 };                //!< model data mean
  double          map_min      { 0.0 };                //!< mapped size min
  double          map_max      { 1.0 };                //!< mapped size max
  double          user_map_min { 0.0 };                //!< user specified mapped size min
  double          user_map_max { 1.0 };                //!< user specified mapped size max
  Units           units        { Units::Type::PIXEL }; //!< mapped size units
  Qt::Orientation direction    { Qt::Horizontal };     //!< mapped size direction
  SymbolSizeMap   sizeMap;                             //!< symbol size map
};

#endif
