#include <CQChartsStyle.h>
#include <CQChartsSVGUtil.h>
#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsStyle, toString, fromString)

int CQChartsStyle::metaTypeId;

void
CQChartsStyle::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsStyle);

  CQPropertyViewMgrInst->setUserName("CQChartsStyle", "style");
}

QString
CQChartsStyle::
toString() const
{
  if (! pen_ || ! brush_)
    return "";

  return CQChartsSVGUtil::penBrushToString(*pen_, *brush_);
}

bool
CQChartsStyle::
fromString(const QString &str)
{
  return setValue(str);
}

bool
CQChartsStyle::
setValue(const QString &str)
{
  str_ = str;

  delete pen_;
  delete brush_;

  pen_   = nullptr;
  brush_ = nullptr;

  auto str1 = str.trimmed();

  if (str1 == "")
    return true;

  if (! CQChartsSVGUtil::stringToStyle(plot_, str, styleData_))
    return false;

  pen_   = new QPen  (styleData_.pen);
  brush_ = new QBrush(styleData_.brush);

  return true;
}
