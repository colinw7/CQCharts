#ifndef CQChartsAlphaColumnData_H
#define CQChartsAlphaColumnData_H

#include <CQChartsColumn.h>

//! \brief alpha column data
struct CQChartsAlphaColumnData {
  using Column     = CQChartsColumn;
  using ColumnType = CQBaseModelType;

  Column     column;                         //<! alpha column
  bool       valid     { false };            //<! alpha valid
  bool       mapped    { true };             //<! alpha values mapped
  double     map_min   { 0.0 };              //<! map value min
  double     map_max   { 1.0 };              //<! map value max
  double     data_min  { 0.0 };              //<! model data min
  double     data_max  { 1.0 };              //<! model data max
  ColumnType modelType { ColumnType::NONE }; //<! alpha model type
};

#endif
