#ifndef CQChartsSymbolTypeData_H
#define CQChartsSymbolTypeData_H

#include <CQChartsColumn.h>
#include <CQChartsSymbolTypeMap.h>

//! \brief symbol type (column) data
class CQChartsSymbolTypeData : public CQChartsMapColumnData<long> {
 public:
  using SymbolTypeMap = CQChartsSymbolTypeMap;

  CQChartsSymbolTypeData() = default;

  const QString &setName() const { return setName_; }
  void setSetName(const QString &s) { setName_ = s; setValid(false); }

  const SymbolTypeMap &typeMap() const { return typeMap_; }
  void setTypeMap(const SymbolTypeMap &m) { typeMap_ = m; setValid(false); }

 private:
  QString       setName_; //!< symbol set name
  SymbolTypeMap typeMap_; //!< symbol type map
};

#endif
