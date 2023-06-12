#ifndef CQChartsMapColumnData_H
#define CQChartsMapColumnData_H

#include <CQChartsColumn.h>

//! \brief base class for mapping of column data to type
template<typename T>
class CQChartsMapColumnData {
 public:
  using Column     = CQChartsColumn;
  using ColumnType = CQBaseModelType;

 public:
  CQChartsMapColumnData() = default;

  //! get/set column
  const Column &column() const { return column_; }
  void setColumn(const Column &c) { column_ = c; setValid(false); }

  //! get/set is valid
  bool isValid() const { return valid_; }
  void setValid(bool b) { valid_ = b; }

  //! get/set is mapped
  bool isMapped() const { return mapped_; }
  void setMapped(bool b) { mapped_ = b; setValid(false); }

  //---

  //! get/set data min/max/range
  T dataMin() const { return dataMin_; }
  void setDataMin(T t) { dataMin_ = t; setValid(false); }

  T dataMax() const { return dataMax_; }
  void setDataMax(T t) { dataMax_ = t; setValid(false); }

  T dataMean() const { return dataMean_; }
  void setDataMean(T t) { dataMean_ = t; setValid(false); }

  void setDataRange(T min, T max) {
    dataMin_ = min; dataMax_ = max; setValid(false);
  }

  //---

  //! get/set map min/max/range
  T mapMin() const { return mapMin_; }
  void setMapMin(T t) { mapMin_ = t; setValid(false); }

  T mapMax() const { return mapMax_; }
  void setMapMax(T t) { mapMax_ = t; setValid(false); }

  void setMapRange(T min, T max) {
    mapMin_ = min; mapMax_ = max; setValid(false);
  }

  //---

  //! get/set user map min/max/range
  T userMapMin() const { return userMapMin_; }
  void setUserMapMin(T t) { userMapMin_ = t; }

  double userMapMax() const { return userMapMax_; }
  void setUserMapMax(T t) { userMapMax_ = t; }

  void setUserMapRange(T min, T max) { userMapMin_ = min; userMapMax_ = max; }

  //---

  //! get/set model type
  const ColumnType &modelType() const { return modelType_; }
  void setModelType(const ColumnType &t) { modelType_ = t; setValid(false); }

 protected:
  Column column_;           //!< color column
  bool   valid_  { false }; //!< color valid
  bool   mapped_ { true };  //!< values mapped

  T dataMin_  { 0 }; //!< model data min
  T dataMax_  { 1 }; //!< model data max
  T dataMean_ { 0 }; //!< model data mean

  T mapMin_ { 0 }; //!< mapped size min
  T mapMax_ { 1 }; //!< mapped size max

  T userMapMin_ { 0 }; //!< user specified mapped min
  T userMapMax_ { 1 }; //!< user specified mapped max

  ColumnType modelType_ { ColumnType::NONE }; //!< model type
};

#endif
