#ifndef CQChartsSymbolTypeData_H
#define CQChartsSymbolTypeData_H

#include <CQChartsColumn.h>
#include <CQChartsSymbolTypeMap.h>

//! \brief symbol type (column) data
struct CQChartsSymbolTypeData {
  using Column        = CQChartsColumn;
  using SymbolTypeMap = CQChartsSymbolTypeMap;

  Column        column;             //!< symbol type column
  bool          valid    { false }; //!< symbol type valid
  bool          mapped   { false }; //!< symbol type values mapped
  long          data_min { 0 };     //!< model data min
  long          data_max { 1 };     //!< model data max
  long          map_min  { 0 };     //!< mapped size min
  long          map_max  { 1 };     //!< mapped size max
  QString       setName;            //!< symbol set name
  SymbolTypeMap typeMap;            //!< symbol type map
};

#endif
