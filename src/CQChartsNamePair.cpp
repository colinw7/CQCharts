#include <CQChartsNamePair.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsNamePair, toString, fromString)

int CQChartsNamePair::metaTypeId;

void
CQChartsNamePair::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsNamePair);

  CQPropertyViewMgrInst->setUserName("CQChartsNamePair", "name_pair");
}

bool
CQChartsNamePair::
stringToNames(const QString &str, Names &names)
{
  names.valid = false;

  // <name1>/<name2>
  int pos = str.indexOf("/");

  if (pos == -1)
    return names.valid;

  names.name1 = str.mid(0, pos ).simplified();
  names.name2 = str.mid(pos + 1).simplified();

  names.valid = true;

  return names.valid;
}

QString
CQChartsNamePair::
namesToString(const Names &names)
{
  return QString("%1/%2").arg(names.name1).arg(names.name2);
}
