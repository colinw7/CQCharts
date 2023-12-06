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

CQChartsNamePair::
CQChartsNamePair(const QString &str, const QString &separator)
{
  setValue(str, separator);
}

CQChartsNamePair::
CQChartsNamePair(const CQChartsNamePair &rhs) :
 names_(rhs.names_)
{
}

CQChartsNamePair &
CQChartsNamePair::
operator=(const CQChartsNamePair &rhs)
{
  names_ = rhs.names_;

  return *this;
}

bool
CQChartsNamePair::
setValue(const QString &str, const QString &separator)
{
  separator_ = separator;

  return stringToNames(str, names_, separator_);
}

QString
CQChartsNamePair::
toString() const
{
  if (! names_.valid)
    return "";

  return namesToString(names_, separator_);
}

bool
CQChartsNamePair::
fromString(const QString &s)
{
  if (s.trimmed() == "") {
    *this = CQChartsNamePair();
    return true;
  }

  return setValue(s, separator_);
}

bool
CQChartsNamePair::
stringToNames(const QString &str, Names &names, const QString &separator)
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
namesToString(const Names &names, const QString &separator)
{
  return QString("%1%2%3").arg(names.name1).arg(separator).arg(names.name2);
}
