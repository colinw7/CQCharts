#include <CQChartsModelColumn.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsModelColumn, toString, fromString)

int CQChartsModelColumn::metaTypeId;

void
CQChartsModelColumn::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsModelColumn);

  CQPropertyViewMgrInst->setUserName("CQChartsModelColumn", "modelColumn");
}

CQChartsModelColumn::
CQChartsModelColumn(const QString &s)
{
  fromString(s);
}

const CQChartsModelData *
CQChartsModelColumn::
modelData() const
{
  if (! charts())
    return nullptr;

  if (modelInd() < 0)
    return nullptr;

  return charts()->getModelDataByInd(modelInd());
}

QString
CQChartsModelColumn::
toString() const
{
  if (! isValid())
    return "";

  return QString("%1:%2").arg(modelInd()).arg(column().toString());
}

bool
CQChartsModelColumn::
fromString(const QString &str)
{
  auto strs = str.split(":");

  if (strs.size() == 2) {
    bool ok;
    long modelInd = CQChartsUtil::toInt(strs[0], ok);
    if (! ok) return false;

    auto column = Column(strs[1]);
    if (! column.isValid()) return false;

    modelInd_ = int(modelInd);
    column_   = column;
  }
  else {
    auto column = Column(str);
    if (! column.isValid()) return false;

    column_ = column;
  }

  return true;
}
