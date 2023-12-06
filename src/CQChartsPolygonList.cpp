#include <CQChartsPolygonList.h>
#include <CQChartsUtil.h>

#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsPolygonList, toString, fromString)

int CQChartsPolygonList::metaTypeId;

void
CQChartsPolygonList::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsPolygonList);

  CQPropertyViewMgrInst->setUserName("CQChartsPolygonList", "polygon_list");
}

bool
CQChartsPolygonList::
setValue(const QString &str)
{
  return CQChartsUtil::stringToPolygons(str, polygons_);
}

QString
CQChartsPolygonList::
toString() const
{
  return CQChartsUtil::polygonListToString(polygons_);
}

bool
CQChartsPolygonList::
fromString(const QString &s)
{
  if (s.trimmed() == "") {
    *this = CQChartsPolygonList();
    return true;
  }

  return setValue(s);
}
