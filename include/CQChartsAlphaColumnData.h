#ifndef CQChartsAlphaColumnData_H
#define CQChartsAlphaColumnData_H

#include <CQChartsColumn.h>

//! \brief alpha column data
class CQChartsAlphaColumnData {
 public:
  using Column     = CQChartsColumn;
  using ColumnType = CQBaseModelType;

 public:
  CQChartsAlphaColumnData() = default;

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
  void setModelType(const ColumnType &t) { modelType_ = t; setValid(false); }

 private:
  Column     column_;                         //!< alpha column
  bool       valid_     { false };            //!< alpha valid
  bool       mapped_    { true };             //!< alpha values mapped
  double     data_min_  { 0.0 };              //!< model data min
  double     data_max_  { 1.0 };              //!< model data max
  double     map_min_   { 0.0 };              //!< map value min
  double     map_max_   { 1.0 };              //!< map value max
  ColumnType modelType_ { ColumnType::NONE }; //!< alpha model type
};

#endif
