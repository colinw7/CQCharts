#include <CQChartsMargin.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsMargin, toString, fromString)

int CQChartsMargin::metaTypeId;

void
CQChartsMargin::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsMargin);

  CQPropertyViewMgrInst->setUserName("CQChartsMargin", "margin");
}

QString
CQChartsMargin::
toString() const
{
  QString lstr = left  ().toString();
  QString tstr = top   ().toString();
  QString rstr = right ().toString();
  QString bstr = bottom().toString();

  if (lstr == tstr && lstr == rstr && lstr == bstr)
    return lstr;

  return QString("%1 %2 %3 %4").arg(lstr).arg(tstr).arg(rstr).arg(bstr);
}
