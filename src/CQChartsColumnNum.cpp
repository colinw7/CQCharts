#include <CQChartsColumnNum.h>
#include <CQChartsUtil.h>
#include <CQPropertyView.h>
#include <CQTclUtil.h>

CQUTIL_DEF_META_TYPE(CQChartsColumnNum, toString, fromString)

int CQChartsColumnNum::metaTypeId;

void
CQChartsColumnNum::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsColumnNum);

  CQPropertyViewMgrInst->setUserName("CQChartsColumnNum", "columnNum");
}

CQChartsColumnNum::
CQChartsColumnNum(int column) :
 column_(column)
{
}

CQChartsColumnNum::
CQChartsColumnNum(const QString &s)
{
  if (! fromString(s))
    column_ = -1;
}

bool
CQChartsColumnNum::
fromString(const QString &str)
{
  if (str.trimmed() == "") {
    column_ = -1;
    return true;
  }

  bool ok;

  int column = str.toInt(&ok);

  if (! ok || column < -1)
    return false;

  column_ = column;

  return true;
}

QString
CQChartsColumnNum::
toString() const
{
  if (column_ >= 0)
    return QString::number(column_);
  else
    return "";
}
