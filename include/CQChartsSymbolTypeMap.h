#ifndef CQChartsSymbolTypeMap_H
#define CQChartsSymbolTypeMap_H

#include <CQChartsSymbol.h>
#include <CQChartsTmpl.h>
#include <CQUtilMeta.h>

/*!
 * \brief class to contain mapping from unique value to symbol type
 * \ingroup Charts
 */
class CQChartsSymbolTypeMap :
  public CQChartsComparatorBase<CQChartsSymbolTypeMap> {
 public:
  static void registerMetaType();

  static int metaTypeId;

  //---

  CQUTIL_DEF_META_CONVERSIONS(CQChartsSymbolTypeMap, metaTypeId)

 public:
  using Symbol = CQChartsSymbol;

  using SymbolValue = std::map<Symbol, QVariant>;
  using ValueSymbol = std::map<QVariant, Symbol>;

 public:
  //! default constructor
  CQChartsSymbolTypeMap() = default;

  //! construct from string
  explicit CQChartsSymbolTypeMap(const QString &str) {
    fromString(str);
  }

  //---

  //! get/set values
  const ValueSymbol &valueSymbol() const { return valueSymbol_; }

  //---

  bool isValid() const { return ! valueSymbol_.empty(); }

  //---

  //! to/from string for QVariant
  QString toString() const;
  bool fromString(const QString &s);

  //---

  bool valueToSymbol(const QVariant &value, Symbol &symbol) const;
  bool symbolToValue(const Symbol &symbol, QVariant &value) const;

  //---

  int cmp(const CQChartsSymbolTypeMap &s) const;

  friend int cmp(const CQChartsSymbolTypeMap &s1, const CQChartsSymbolTypeMap &s2) {
    return s1.cmp(s2);
  }

  //---

 private:
  void updateSymbolValue() const;

 private:
  ValueSymbol         valueSymbol_; //!< map of value to symbol
  mutable SymbolValue symbolValue_; //!< map of symbol to value
};

//---

CQUTIL_DCL_META_TYPE(CQChartsSymbolTypeMap)

#endif
