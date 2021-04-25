#include <CQChartsLineCap.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsLineCap, toString, fromString)

int CQChartsLineCap::metaTypeId;

void
CQChartsLineCap::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsLineCap);

  CQPropertyViewMgrInst->setUserName("CQChartsLineCap", "line_cap");
}

CQChartsLineCap::
CQChartsLineCap()
{
}

CQChartsLineCap::
CQChartsLineCap(const Qt::PenCapStyle &cap) :
 cap_(cap)
{
}

CQChartsLineCap::
CQChartsLineCap(const QString &str)
{
  (void) fromString(str);
}

QString
CQChartsLineCap::
toString() const
{
  switch (cap_) {
    case Qt::FlatCap  : return "flat";
    case Qt::RoundCap : return "round";
    case Qt::SquareCap: return "square";
    default: assert(false);
  }

  return "";
}

bool
CQChartsLineCap::
fromString(const QString &str)
{
  auto lstr = str.toLower();

  if      (lstr == "flat"  ) cap_ = Qt::FlatCap;
  else if (lstr == "round" ) cap_ = Qt::RoundCap;
  else if (lstr == "square") cap_ = Qt::SquareCap;
  else return false;

  return true;
}

QStringList
CQChartsLineCap::
enumNames() const
{
  static auto names = QStringList() <<
    "flat" << "round" << "square";

  return names;
}
