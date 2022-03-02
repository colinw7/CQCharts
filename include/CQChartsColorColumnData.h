#ifndef CQChartsColorColumnData_H
#define CQChartsColorColumnData_H

#include <CQChartsColumn.h>
#include <CQChartsColorStops.h>
#include <CQChartsColorMap.h>
#include <CQChartsTypes.h>
#include <CQBaseModelTypes.h>

//! \brief color column data
struct CQChartsColorColumnData {
  using Column      = CQChartsColumn;
  using ColorType   = CQChartsColorType;
  using ColumnType  = CQBaseModelType;
  using PaletteName = CQChartsPaletteName;
  using ColorStops  = CQChartsColorStops;
  using ColorMap    = CQChartsColorMap;

  Column      column;                         //!< color column
  ColorType   colorType { ColorType::AUTO };  //!< color type
  bool        valid     { false };            //!< color valid
  bool        mapped    { true };             //!< color values mapped
  double      map_min   { 0.0 };              //!< map value min
  double      map_max   { 1.0 };              //!< map value max
  double      data_min  { 0.0 };              //!< model data min
  double      data_max  { 1.0 };              //!< model data max
  ColumnType  modelType { ColumnType::NONE }; //!< color model type
  PaletteName palette;                        //!< color palette
  ColorStops  xStops;                         //!< color x stops
  ColorStops  yStops;                         //!< color y stops
  ColorMap    colorMap;                       //!< color map
};

#endif
