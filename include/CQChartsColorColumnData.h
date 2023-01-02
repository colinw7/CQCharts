#ifndef CQChartsColorColumnData_H
#define CQChartsColorColumnData_H

#include <CQChartsColumn.h>
#include <CQChartsColorStops.h>
#include <CQChartsColorMap.h>
#include <CQChartsTypes.h>
#include <CQBaseModelTypes.h>

//! \brief color column data
class CQChartsColorColumnData {
 public:
  using Column      = CQChartsColumn;
  using ColorType   = CQChartsColorType;
  using ColumnType  = CQBaseModelType;
  using PaletteName = CQChartsPaletteName;
  using ColorStops  = CQChartsColorStops;
  using ColorMap    = CQChartsColorMap;

 public:
  CQChartsColorColumnData() { }

  bool isValid() const { return valid_; }
  void setValid(bool b) { valid_ = b; }

  const Column &column() const { return column_; }
  void setColumn(const Column &c) { column_ = c; setValid(false); }

  const ColorType &colorType() const { return colorType_; }
  void setColorType(const ColorType &t) { colorType_ = t; setValid(false); }

  bool isMapped() const { return mapped_; }
  void setMapped(bool b) { mapped_ = b; setValid(false); }

  double dataMin() const { return data_min_; }
  void setDataMin(double l) { data_min_ = l; setValid(false); }

  double dataMax() const { return data_max_; }
  void setDataMax(double l) { data_max_ = l; setValid(false); }

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

  const ColumnType &modelType() const { return modelType_; }
  void setModelType(const ColumnType &v) { modelType_ = v; }

  const PaletteName &palette() const { return palette_; }
  void setPalette(const PaletteName &v) { palette_ = v; }

  const ColorStops &xStops() const { return xStops_; }
  void setXStops(const ColorStops &v) { xStops_ = v; }

  const ColorStops &yStops() const { return yStops_; }
  void setYStops(const ColorStops &v) { yStops_ = v; }

  const ColorMap &colorMap() const { return colorMap_; }
  void setColorMap(const ColorMap &m) { colorMap_ = m; }

 private:
  Column      column_;                         //!< color column
  ColorType   colorType_ { ColorType::AUTO };  //!< color type
  bool        valid_     { false };            //!< color valid
  bool        mapped_    { true };             //!< color values mapped
  double      data_min_  { 0.0 };              //!< model data min
  double      data_max_  { 1.0 };              //!< model data max
  double      map_min_   { 0.0 };              //!< map value min
  double      map_max_   { 1.0 };              //!< map value max
  ColumnType  modelType_ { ColumnType::NONE }; //!< color model type
  PaletteName palette_;                        //!< color palette
  ColorStops  xStops_;                         //!< color x stops
  ColorStops  yStops_;                         //!< color y stops
  ColorMap    colorMap_;                       //!< color map
};

#endif
