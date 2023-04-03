#ifndef CQChartsSymbolTypeData_H
#define CQChartsSymbolTypeData_H

#include <CQChartsColumn.h>
#include <CQChartsSymbolTypeMap.h>

//! \brief symbol type (column) data
class CQChartsSymbolTypeData {
 public:
  using Column        = CQChartsColumn;
  using SymbolTypeMap = CQChartsSymbolTypeMap;

  CQChartsSymbolTypeData() = default;

  bool isValid() const { return valid_; }
  void setValid(bool b) { valid_ = b; }

  const Column &column() const { return column_; }
  void setColumn(const Column &c) { column_ = c; setValid(false); }

  bool isMapped() const { return mapped_; }
  void setMapped(bool b) { mapped_ = b; setValid(false); }

  long dataMin() const { return data_min_; }
  void setDataMin(long l) { data_min_ = l; setValid(false); }

  long dataMax() const { return data_max_; }
  void setDataMax(long l) { data_max_ = l; setValid(false); }

  void setDataRange(long min, long max) {
    data_min_ = min; data_max_ = max; setValid(false);
  }

  long mapMin() const { return map_min_; }
  void setMapMin(long l) { map_min_ = l; setValid(false); }

  long mapMax() const { return map_max_; }
  void setMapMax(long l) { map_max_ = l; setValid(false); }

  void setMapRange(long min, long max) {
    map_min_ = min; map_max_ = max; setValid(false);
  }

  const QString &setName() const { return setName_; }
  void setSetName(const QString &s) { setName_ = s; setValid(false); }

  const SymbolTypeMap &typeMap() const { return typeMap_; }
  void setTypeMap(const SymbolTypeMap &m) { typeMap_ = m; setValid(false); }

 private:
  Column        column_;             //!< symbol type column
  bool          valid_    { false }; //!< symbol type valid
  bool          mapped_   { false }; //!< symbol type values mapped
  long          data_min_ { 0 };     //!< model data min
  long          data_max_ { 1 };     //!< model data max
  long          map_min_  { 0 };     //!< mapped size min
  long          map_max_  { 1 };     //!< mapped size max
  QString       setName_;            //!< symbol set name
  SymbolTypeMap typeMap_;            //!< symbol type map
};

#endif
