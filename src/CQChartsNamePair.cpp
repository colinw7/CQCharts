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
stringToNames(const QString &str, Names &names, const QChar &separator)
{
  names.valid = false;

  // <name1>/<name2>
  int pos = str.indexOf(separator);

  if (pos == -1)
    return names.valid;

  names.name1 = str.mid(0, pos ).trimmed();
  names.name2 = str.mid(pos + 1).trimmed();

  names.valid = true;

  return names.valid;
}

QString
CQChartsNamePair::
namesToString(const Names &names, const QChar &separator)
{
  return QString("%1%2%3").arg(names.name1).arg(separator).arg(names.name2);
}
