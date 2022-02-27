#include <CQChartsSymbolSizeMap.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>
#include <CQTclUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsSymbolSizeMap, toString, fromString)

int CQChartsSymbolSizeMap::metaTypeId;

void
CQChartsSymbolSizeMap::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsSymbolSizeMap);

  CQPropertyViewMgrInst->setUserName("CQChartsSymbolSizeMap", "symbol_size_map");
}

QString
CQChartsSymbolSizeMap::
toString() const
{
  QStringList strs;

  for (const auto &value : valueLength_) {
    QStringList strs1;

    strs1 << value.first.toString();  // value
    strs1 << value.second.toString(); // length

    strs << CQTcl::mergeList(strs1);
  }

  return CQTcl::mergeList(strs);
}

bool
CQChartsSymbolSizeMap::
fromString(const QString &str)
{
  ValueLength valueLength;

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
    Length   length(strs1[1]);

    valueLength[value] = length;
  }

  valueLength_ = valueLength;

  lengthValue_.clear();

  return true;
}

bool
CQChartsSymbolSizeMap::
valueToLength(const QVariant &value, Length &length) const
{
  auto p = valueLength_.find(value);
  if (p == valueLength_.end()) return false;

  length = (*p).second;

  return true;
}

bool
CQChartsSymbolSizeMap::
lengthToValue(const Length &length, QVariant &value) const
{
  updateLengthValue();

  auto p = lengthValue_.find(length);
  if (p == lengthValue_.end()) return false;

  value = (*p).second;

  return true;
}

int
CQChartsSymbolSizeMap::
cmp(const CQChartsSymbolSizeMap &s) const
{
  auto n1 =   valueLength_.size();
  auto n2 = s.valueLength_.size();

  if (n1 != n2)
    return int(n1 - n2);

  using Lengths      = std::vector<Length>;
  using ValueLengths = std::map<QVariant, Lengths>;

  ValueLengths valueLengths;

  for (const auto &value : valueLength_)
    valueLengths[value.first].push_back(value.second);

  for (const auto &value : s.valueLength_)
    valueLengths[value.first].push_back(value.second);

  for (const auto &value : valueLengths) {
    if (value.second.size() == 1) {
      auto p = valueLength_.find(value.first);

      return (p != valueLength_.end() ? 1 : -1);
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
CQChartsSymbolSizeMap::
updateLengthValue() const
{
  if (lengthValue_.empty()) {
    for (const auto &value : valueLength_)
      lengthValue_[value.second] = value.first;
  }
}
