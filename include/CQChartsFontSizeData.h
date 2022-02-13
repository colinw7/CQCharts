#ifndef CQChartsFontSizeData_H
#define CQChartsFontSizeData_H

#include <CQChartsColumn.h>
#include <CQChartsUnits.h>

//! \brief font size (column) data
struct CQChartsFontSizeData {
  using Column = CQChartsColumn;
  using Units  = CQChartsUnits;

  Column          column;                              //!< font size column
  bool            valid        { false };              //!< font size valid
  bool            mapped       { false };              //!< font size values mapped
  double          data_min     { 0.0 };                //!< model data min
  double          data_max     { 1.0 };                //!< model data max
  double          map_min      { 0.0 };                //!< mapped size min
  double          map_max      { 1.0 };                //!< mapped size max
  double          user_map_min { 0.0 };                //!< user specified mapped size min
  double          user_map_max { 1.0 };                //!< user specified mapped size max
  Units           units        { Units::Type::PIXEL }; //!< mapped size units
  Qt::Orientation direction    { Qt::Horizontal };     //!< mapped size direction
};

#endif
