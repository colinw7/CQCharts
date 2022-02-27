#include <CQChartsSymbolTypeMap.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>
#include <CQTclUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsSymbolTypeMap, toString, fromString)

int CQChartsSymbolTypeMap::metaTypeId;

void
CQChartsSymbolTypeMap::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsSymbolTypeMap);

  CQPropertyViewMgrInst->setUserName("CQChartsSymbolTypeMap", "symbol_type_map");
}

QString
CQChartsSymbolTypeMap::
toString() const
{
  QStringList strs;

  for (const auto &value : valueSymbol_) {
    QStringList strs1;

    strs1 << value.first.toString();  // value
    strs1 << value.second.toString(); // symbol

    strs << CQTcl::mergeList(strs1);
  }

  return CQTcl::mergeList(strs);
}

bool
CQChartsSymbolTypeMap::
fromString(const QString &str)
{
  ValueSymbol valueSymbol;

  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  for (const auto &str : strs) {
    QStringList strs1;

    if (! CQTcl::splitList(str, strs1))
      continue;

    if (strs1.size() != 2)
      continue;

    QVariant value(strs1[0]);
    Symbol   symbol(strs1[1]);

    valueSymbol[value] = symbol;
  }

  valueSymbol_ = valueSymbol;

  symbolValue_.clear();

  return true;
}

bool
CQChartsSymbolTypeMap::
valueToSymbol(const QVariant &value, Symbol &symbol) const
{
  auto p = valueSymbol_.find(value);
  if (p == valueSymbol_.end()) return false;

  symbol = (*p).second;

  return true;
}

bool
CQChartsSymbolTypeMap::
symbolToValue(const Symbol &symbol, QVariant &value) const
{
  updateSymbolValue();

  auto p = symbolValue_.find(symbol);
  if (p == symbolValue_.end()) return false;

  value = (*p).second;

  return true;
}

int
CQChartsSymbolTypeMap::
cmp(const CQChartsSymbolTypeMap &s) const
{
  auto n1 =   valueSymbol_.size();
  auto n2 = s.valueSymbol_.size();

  if (n1 != n2)
    return int(n1 - n2);

  using Symbols      = std::vector<Symbol>;
  using ValueSymbols = std::map<QVariant, Symbols>;

  ValueSymbols valueSymbols;

  for (const auto &value : valueSymbol_)
    valueSymbols[value.first].push_back(value.second);

  for (const auto &value : s.valueSymbol_)
    valueSymbols[value.first].push_back(value.second);

  for (const auto &value : valueSymbols) {
    if (value.second.size() == 1) {
      auto p = valueSymbol_.find(value.first);

      return (p != valueSymbol_.end() ? 1 : -1);
    }

    const auto &value1 = value.second[0];
    const auto &value2 = value.second[1];

    if (value1 == value2)
      continue;

    return (value1 < value2 ? -1 : 1);
  }

  return 0;
}

void
CQChartsSymbolTypeMap::
updateSymbolValue() const
{
  if (symbolValue_.empty()) {
    for (const auto &value : valueSymbol_)
      symbolValue_[value.second] = value.first;
  }
}
