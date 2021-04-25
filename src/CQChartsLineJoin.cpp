#include <CQChartsLineJoin.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsLineJoin, toString, fromString)

int CQChartsLineJoin::metaTypeId;

void
CQChartsLineJoin::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsLineJoin);

  CQPropertyViewMgrInst->setUserName("CQChartsLineJoin", "line_join");
}

CQChartsLineJoin::
CQChartsLineJoin()
{
}

CQChartsLineJoin::
CQChartsLineJoin(const Qt::PenJoinStyle &join) :
 join_(join)
{
}

CQChartsLineJoin::
CQChartsLineJoin(const QString &str)
{
  (void) fromString(str);
}

QString
CQChartsLineJoin::
toString() const
{
  switch (join_) {
    case Qt::MiterJoin: return "miter";
    case Qt::BevelJoin: return "bevel";
    case Qt::RoundJoin: return "round";
    default: assert(false);
  }

  return "";
}

bool
CQChartsLineJoin::
fromString(const QString &str)
{
  auto lstr = str.toLower();

  if      (lstr == "miter") join_ = Qt::MiterJoin;
  else if (lstr == "mitre") join_ = Qt::MiterJoin;
  else if (lstr == "bevel") join_ = Qt::BevelJoin;
  else if (lstr == "round") join_ = Qt::RoundJoin;
  else return false;

  return true;
}

QStringList
CQChartsLineJoin::
enumNames() const
{
  static auto names = QStringList() <<
    "miter" << "bevel" << "round";

  return names;
}
