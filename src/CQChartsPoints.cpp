#include <CQChartsPoints.h>
#include <CQPropertyView.h>
#include <CQTclUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsPoints, toString, fromString)

int CQChartsPoints::metaTypeId;

void
CQChartsPoints::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPoints);

  CQPropertyViewMgrInst->setUserName("CQChartsPoints", "points");
}

QString
CQChartsPoints::
toString() const
{
  QStringList strs;

  for (auto &p : points_) {
    strs << p.toString();
  }

  return CQTcl::mergeList(strs);
}

bool
CQChartsPoints::
fromString(const QString &str, const CQChartsUnits &defUnits)
{
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  points_.clear();

  for (int i = 0; i < strs.length(); ++i) {
    Position p(strs[i], defUnits);

    if (p.isValid())
      points_.push_back(std::move(p));
  }

  return true;
}
