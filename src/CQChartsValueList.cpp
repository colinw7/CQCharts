#include <CQChartsValueList.h>
#include <CQPropertyView.h>
#include <CQTclUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsValueList, toString, fromString)

int CQChartsValueList::metaTypeId;

void
CQChartsValueList::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsValueList);

  CQPropertyViewMgrInst->setUserName("CQChartsValueList", "value_list");
}

QString
CQChartsValueList::
toString() const
{
  QStringList strs;

  for (const auto &value : valueList_) {
    QString str;

    if (! CQChartsVariant::toString(value, str))
      str = value.toString();

    strs += str;
  }

  strs += QString("#%1").arg(currentInd());

  return CQTcl::mergeList(strs);
}

bool
CQChartsValueList::
fromString(const QString &s)
{
  QStringList strs;

  if (! CQTcl::splitList(s, strs))
    return false;

  valueList_.clear();

  currentInd_ = -1;

  int n = strs.length();
  if (n < 0) return true;

  currentInd_ = 0;

  if (n > 1 && strs[n - 1].left(1) == "#") {
    bool ok;
    int i = strs[n - 1].mid(1).toInt(&ok);

    if (ok) {
      currentInd_ = i;
      --n;
    }
  }

  for (int i = 0; i < n; ++i) {
    auto var = CQChartsVariant::fromString(strs[i]); // TODO: allow parent type ?

    valueList_.push_back(var);
  }

  return true;
}
