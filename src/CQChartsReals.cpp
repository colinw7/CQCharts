#include <CQChartsReals.h>
#include <CQPropertyView.h>
#include <CQTclUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsReals, toString, fromString)

int CQChartsReals::metaTypeId;

void
CQChartsReals::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsReals);

  CQPropertyViewMgrInst->setUserName("CQChartsReals", "reals");
}

QString
CQChartsReals::
toString() const
{
  QStringList strs;

  for (auto &r : reals_)
    strs << QString::number(r);

  return CQTcl::mergeList(strs);
}

bool
CQChartsReals::
fromString(const QString &str)
{
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  reals_.clear();

  for (int i = 0; i < strs.length(); ++i) {
    bool ok;

    double r = CQChartsUtil::toReal(strs[i], ok);

    if (ok)
      reals_.push_back(r);
  }

  return true;
}
