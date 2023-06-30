#include <CQChartsModelProcess.h>
#include <CQChartsExprModel.h>
#include <CQChartsModelUtil.h>
#include <CQChartsModelData.h>
#include <CQCharts.h>

namespace CQChartsModelProcess {

ErrorType
processColumn(CQCharts *charts, ModelP model, CQChartsModelProcessData &data)
{
  switch (data.type()) {
    case CQChartsModelProcessData::Type::ADD:
      return addColumn(model, *data.addData());
    case CQChartsModelProcessData::Type::DELETE:
      return (deleteColumn(model, *data.deleteData()) ?
               ErrorType::NONE : ErrorType::DELETE_FAILED);
    case CQChartsModelProcessData::Type::MODIFY:
      return modifyColumn(model, *data.modifyData());
    case CQChartsModelProcessData::Type::REPLACE:
      return (replaceColumn(charts, model, *data.replaceData()) ?
               ErrorType::NONE : ErrorType::REPLACE_FAILED);
    case CQChartsModelProcessData::Type::REPLACE_NULL:
      return (replaceNullColumn(charts, model, *data.replaceNullData()) ?
                ErrorType::NONE : ErrorType::REPLACE_NULL_FAILED);
    default:
      assert(false);
  }
}

ErrorType
addColumn(ModelP model, CQChartsModelProcessData::AddData &addData)
{
  auto *exprModel = CQChartsModelUtil::getExprModel(model.data());
  assert(exprModel);

  // tcl expression
  int column = -1;

  if      (addData.type == CQChartsModelProcessData::AddData::Type::EXPR) {
    if (! exprModel->addExtraColumnExpr(addData.header, addData.expr,
                                        column, addData.varNameValues))
      return ErrorType::ADD_FAILED;
  }
  // tcl array data
  else if (addData.type == CQChartsModelProcessData::AddData::Type::TCL) {
    if (! exprModel->addExtraColumnStrs(addData.header, addData.strs, column))
      return ErrorType::ADD_FAILED;
  }
  else {
    assert(false);
  }

  addData.column = CQChartsColumn(column);

  //---

  if (addData.columnType.length()) {
    if (! CQChartsModelUtil::setColumnTypeStr(exprModel->charts(), model.data(),
                                              addData.column, addData.columnType))
      return ErrorType::SET_TYPE_FAILED;
  }

  return ErrorType::NONE;
}

bool
deleteColumn(ModelP model, const CQChartsModelProcessData::DeleteData &deleteData)
{
  auto *exprModel = CQChartsModelUtil::getExprModel(model.data());
  assert(exprModel);

  return exprModel->removeExtraColumn(deleteData.column.column());
}

ErrorType
modifyColumn(ModelP model, const CQChartsModelProcessData::ModifyData &modifyData)
{
  auto *exprModel = CQChartsModelUtil::getExprModel(model.data());
  assert(exprModel);

  if (exprModel->isOrigColumn(modifyData.column.column())) {
    bool rc;

    if (exprModel->isReadOnly()) {
      exprModel->setReadOnly(false);

      rc = exprModel->assignColumn(modifyData.header, modifyData.column.column(), modifyData.expr);

      exprModel->setReadOnly(true);
    }
    else
      rc = exprModel->assignColumn(modifyData.header, modifyData.column.column(), modifyData.expr);

    if (! rc)
      return ErrorType::MODIFY_FAILED;
  }
  else {
    if (! exprModel->assignExtraColumn(modifyData.header, modifyData.column.column(),
                                       modifyData.expr))
      return ErrorType::MODIFY_FAILED;
  }

  //---

  if (modifyData.columnType.length()) {
    if (! CQChartsModelUtil::setColumnTypeStr(exprModel->charts(), model.data(),
                                              modifyData.column, modifyData.columnType))
      return ErrorType::SET_TYPE_FAILED;
  }

  return ErrorType::NONE;
}

int
replaceColumn(CQCharts *charts, ModelP model,
              const CQChartsModelProcessData::ReplaceData &replaceData)
{
  auto *modelData = charts->getModelData(model);

  auto n = modelData->replaceValue(replaceData.column, replaceData.oldValue, replaceData.newValue);

  return n;
}

int
replaceNullColumn(CQCharts *charts, ModelP model,
                  const CQChartsModelProcessData::ReplaceNullData &replaceData)
{
  auto *modelData = charts->getModelData(model);

  auto n = modelData->replaceNullValues(replaceData.column, replaceData.value);

  return n;
}

}
