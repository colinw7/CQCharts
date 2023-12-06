#include <CQChartsColorMap.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>
#include <CQTclUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsColorMap, toString, fromString)

int CQChartsColorMap::metaTypeId;

void
CQChartsColorMap::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsColorMap);

  CQPropertyViewMgrInst->setUserName("CQChartsColorMap", "color_map");
}

QString
CQChartsColorMap::
toString() const
{
  QStringList strs;

  for (const auto &value : valueColor_) {
    QStringList strs1;

    strs1 << value.first.toString();  // value
    strs1 << value.second.toString(); // color

    strs << CQTcl::mergeList(strs1);
  }

  return CQTcl::mergeList(strs);
}

bool
CQChartsColorMap::
fromString(const QString &str)
{
  ValueColor valueColor;

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
    Color    color(strs1[1]);

    valueColor[value] = color;
  }

  valueColor_ = valueColor;

  colorValue_.clear();

  return true;
}

bool
CQChartsColorMap::
valueToColor(const QVariant &value, Color &color) const
{
  auto p = valueColor_.find(value);
  if (p == valueColor_.end()) return false;

  color = (*p).second;

  return true;
}

bool
CQChartsColorMap::
colorToValue(const Color &color, QVariant &value) const
{
  updateColorValue();

  auto p = colorValue_.find(color);
  if (p == colorValue_.end()) return false;

  value = (*p).second;

  return true;
}

int
CQChartsColorMap::
cmp(const CQChartsColorMap &s) const
{
  auto n1 =   valueColor_.size();
  auto n2 = s.valueColor_.size();

  if (n1 != n2)
    return int(n1 - n2);

  using Colors      = std::vector<Color>;
  using ValueColors = std::map<QVariant, Colors>;

  ValueColors valueColors;

  for (const auto &value : valueColor_)
    valueColors[value.first].push_back(value.second);

  for (const auto &value : s.valueColor_)
    valueColors[value.first].push_back(value.second);

  for (const auto &value : valueColors) {
    if (value.second.size() == 1) {
      auto p = valueColor_.find(value.first);

      return (p != valueColor_.end() ? 1 : -1);
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
CQChartsColorMap::
updateColorValue() const
{
  if (colorValue_.empty()) {
    for (const auto &value : valueColor_)
      colorValue_[value.second] = value.first;
  }
}
