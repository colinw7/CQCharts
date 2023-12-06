#include <CQChartsModelInd.h>
#include <CQChartsModelData.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

#include <CQPropertyView.h>

CQUTIL_DEF_META_TYPE(CQChartsModelInd, toString, fromString)

int CQChartsModelInd::metaTypeId;

void
CQChartsModelInd::
registerMetaType()
{
  metaTypeId = CQUTIL_REGISTER_META(CQChartsModelInd);

  CQPropertyViewMgrInst->setUserName("CQChartsModelInd", "modelInd");
}

CQChartsModelInd::
CQChartsModelInd(const QString &s)
{
  fromString(s);
}

const CQChartsModelData *
CQChartsModelInd::
modelData() const
{
  if (! charts())
    return nullptr;

  if (modelInd() < 0)
    return nullptr;

  return charts()->getModelDataByInd(modelInd());
}

QString
CQChartsModelInd::
toString() const
{
  if (! isValid())
    return "";

  if (charts()) {
    auto *modelData = charts()->getModelDataByInd(modelInd());

    if (modelData)
      return modelData->id();
  }

  return QString::number(modelInd());
}

bool
CQChartsModelInd::
fromString(const QString &str)
{
  if (str.trimmed() == "") {
    modelInd_ = -1;
    return true;
  }

  CQChartsModelData *modelData = nullptr;

  if (charts()) {
    modelData = charts()->getModelDataById(str);

    if (! modelData) {
      bool ok;
      long ind = CQChartsUtil::toInt(str, ok);
      if (! ok) return false;

      modelData = charts()->getModelDataByInd(ind);
    }
  }

  if (modelData) {
    modelInd_ = modelData->ind();
  }
  else {
    bool ok;
    long ind = CQChartsUtil::toInt(str, ok);
    if (! ok) return false;

    modelInd_ = int(ind);
  }

  return true;
}
