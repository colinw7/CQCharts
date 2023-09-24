#ifndef CQChartsColorColumnData_H
#define CQChartsColorColumnData_H

#include <CQChartsMapColumnData.h>
#include <CQChartsColorStops.h>
#include <CQChartsColorMap.h>
#include <CQChartsTypes.h>
#include <CQBaseModelTypes.h>

//! \brief color column data
class CQChartsColorColumnData : public CQChartsMapColumnData<double> {
 public:
  using ColorType   = CQChartsColorType;
  using PaletteName = CQChartsPaletteName;
  using ColorStops  = CQChartsColorStops;
  using ColorMap    = CQChartsColorMap;
  using Column      = CQChartsColumn;

 public:
  CQChartsColorColumnData() = default;

  bool isIntMapped() const { return intMapped_; }
  void setIntMapped(bool b) { intMapped_ = b; }

  const ColorType &colorType() const { return colorType_; }
  void setColorType(const ColorType &t) { colorType_ = t; setValid(false); }

  const PaletteName &palette() const { return palette_; }
  void setPalette(const PaletteName &v) { palette_ = v; }

  const ColorStops &xStops() const { return xStops_; }
  void setXStops(const ColorStops &v) { xStops_ = v; }

  const ColorStops &yStops() const { return yStops_; }
  void setYStops(const ColorStops &v) { yStops_ = v; }

  const ColorMap &colorMap() const { return colorMap_; }
  void setColorMap(const ColorMap &m) { colorMap_ = m; }

  const Column &colorColumn() const { return colorColumn_; }
  void setColorColumn(const Column &c) { colorColumn_ = c; }

 private:
  bool        intMapped_ { true };            //!< integer mapped
  ColorType   colorType_ { ColorType::AUTO }; //!< color type
  PaletteName palette_;                       //!< color palette
  ColorStops  xStops_;                        //!< color x stops
  ColorStops  yStops_;                        //!< color y stops
  ColorMap    colorMap_;                      //!< color map
  Column      colorColumn_;                   //!< color map column
};

#endif
