#include <CQChartsModelUtil.h>
#include <CQChartsModelFilter.h>
#include <CQChartsModelVisitor.h>
#include <CQChartsColumnType.h>
#include <CQChartsColumnEval.h>
#include <CQChartsExprTcl.h>
#include <CQChartsVariant.h>
#include <CQChartsValueSet.h>
#include <CQChartsFilterModel.h>
#include <CQCharts.h>

#include <CQCsvModel.h>
#include <CQTsvModel.h>
#include <CQGnuDataModel.h>
#include <CQJsonModel.h>
#include <CQDataModel.h>
#include <CQPivotModel.h>
#include <CQHierSepModel.h>
#include <CQModelUtil.h>

#include <CQPerfMonitor.h>
#include <CQStrParse.h>
#include <CQTclUtil.h>

#include <QSortFilterProxyModel>

//------

namespace {

void errorMsg(const QString &msg) {
  std::cerr << msg.toStdString() << std::endl;
}

}

//------

namespace CQChartsModelUtil {

bool isHierarchical(const QAbstractItemModel *model) {
  return CQModelUtil::isHierarchical(model);
}

void hierData(CQCharts *charts, const QAbstractItemModel *model, int &nr, int &maxDepth) {
  if (! isHierarchical(model)) {
    nr       = model->rowCount();
    maxDepth = 0;
  }
  else {
    CQChartsModelVisitor visitor;

    CQChartsModelVisit::exec(charts, model, visitor);

    nr       = visitor.numProcessedRows();
    maxDepth = visitor.maxDepth();
  }
}

int hierRowCount(CQCharts *charts, const QAbstractItemModel *model) {
  int nr, maxDepth;

  hierData(charts, model, nr, maxDepth);

  return nr;
}

QString parentPath(const QAbstractItemModel *model, const QModelIndex &parent) {
  QString path;

  QModelIndex pind = parent;

  while (pind.isValid()) {
    bool ok;

    QString str = modelString(model, pind, ok);

    if (! ok)
      break;

    if (path.length())
      path = "/" + path;

    path = str + path;

    pind = pind.parent();
  }

  return path;
}

//------

CQBaseModelType
calcColumnType(CQCharts *charts, const QAbstractItemModel *model, int icolumn)
{
  CQPerfTrace trace("CQChartsUtil::calcColumnType");

  // determine column type from values
  // TODO: cache (in plot ?), max visited values

  // process model data
  class ColumnTypeVisitor : public CQChartsModelVisitor {
   public:
    ColumnTypeVisitor(int column) :
     column_(column) {
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      QModelIndex ind = model->index(data.row, column_, data.parent);

      // if column can be integral, check if value is valid integer
      if (isInt_) {
        bool ok;

        (void) modelInteger(model, ind, ok);

        if (ok)
          return State::SKIP;

        QString str = modelString(model, ind, ok);

        if (! str.length())
          return State::SKIP;

        isInt_ = false;
      }

      // if column can be real, check if value is valid real
      if (isReal_) {
        bool ok;

        (void) modelReal(model, ind, ok);

        if (ok)
          return State::SKIP;

        QString str = modelString(model, ind, ok);

        if (! str.length())
          return State::SKIP;

        isReal_ = false;
      }

      // not value real or integer so assume string and we are done
      return State::TERMINATE;
    }

    CQBaseModelType columnType() {
      if      (isInt_ ) return CQBaseModelType::INTEGER;
      else if (isReal_) return CQBaseModelType::REAL;
      else              return CQBaseModelType::STRING;
    }

   private:
    int  column_ { -1 };   // column to check
    bool isInt_  { true }; // could be integeral
    bool isReal_ { true }; // could be real
  };

  // determine column value type by looking at model values
  ColumnTypeVisitor columnTypeVisitor(icolumn);

  CQChartsModelVisit::exec(charts, model, columnTypeVisitor);

  return columnTypeVisitor.columnType();
}

// get type and associated name values for column
//  . column can be model column, header or custom expression
bool
columnValueType(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                CQChartsModelTypeData &columnTypeData) {
  assert(model);

  auto setRetType = [&](const CQBaseModelType &type) {
    columnTypeData.type     = type;
    columnTypeData.baseType = type;

    return (columnTypeData.type != CQBaseModelType::NONE);
  };

  if (column.type() == CQChartsColumn::Type::DATA ||
      column.type() == CQChartsColumn::Type::DATA_INDEX ||
      column.type() == CQChartsColumn::Type::HHEADER) {
    // get column number and validate
    int icolumn = column.column();

    if (icolumn < 0 || icolumn >= model->columnCount())
      return setRetType(CQBaseModelType::NONE);

    //---

    // use defined column type if available
    auto *columnTypeMgr = charts->columnTypeMgr();

    if (columnTypeMgr->getModelColumnType(model, CQChartsColumn(icolumn), columnTypeData)) {
      if (column.type() == CQChartsColumn::Type::DATA_INDEX) {
        const auto *typeData = columnTypeMgr->getType(columnTypeData.type);

        if (typeData)
          columnTypeData.type = typeData->indexType(column.index());
      }

      return true;
    }

    //---

    // determine column type from values
    // TODO: cache (in plot ?), max visited values

    if (column.type() != CQChartsColumn::Type::HHEADER)
      return setRetType(calcColumnType(charts, model, icolumn));
    else
      return setRetType(CQBaseModelType::STRING);
  }
  else if (column.type() == CQChartsColumn::Type::GROUP) {
    return setRetType(CQBaseModelType::INTEGER);
  }
  else {
    // TODO: for custom expression should determine expression result type (if possible)
    return setRetType(CQBaseModelType::STRING);
  }
}

// use column format string to format a value as data (used by axis)
//  TODO: separate format string from column type to remove dependence
bool
formatColumnTypeValue(CQCharts *charts, const QAbstractItemModel *model,
                      const CQChartsColumn &column, const QString &formatStr,
                      double value, QString &str) {
#if 0
  CQChartsNameValues nameValues;

  auto *columnTypeMgr = charts->columnTypeMgr();

  const auto *typeData = columnTypeMgr->decodeTypeData(formatStr, nameValues);

  if (! typeData)
    return false;
#else
  CQChartsModelTypeData columnTypeData;

  if (! columnValueType(charts, model, column, columnTypeData))
    return false;

  auto *columnTypeMgr = charts->columnTypeMgr();

  const auto *typeData = columnTypeMgr->getType(columnTypeData.type);

  if (! typeData)
    return false;

  columnTypeData.nameValues.setNameValue(typeData->formatName(), formatStr);
#endif

  return formatColumnTypeValue(charts, model, column, typeData, columnTypeData, value, str);
}

// use column type details to format an internal value (real) to a display value
//  TODO: value should be variant ?
bool
formatColumnValue(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                  double value, QString &str) {
  CQChartsModelTypeData typeData;

  if (! columnValueType(charts, model, column, typeData))
    return false;

  auto *columnTypeMgr = charts->columnTypeMgr();

  if (column.type() == CQChartsColumn::Type::HHEADER) {
    auto columnType = columnTypeMgr->getType(typeData.headerType);
    if (! columnType) return false;

    CQChartsModelTypeData headerTypeData;

    headerTypeData.type       = typeData.headerType;
    headerTypeData.baseType   = headerTypeData.type;
    headerTypeData.nameValues = typeData.headerNameValues;

    return formatColumnTypeValue(charts, model, column, columnType, headerTypeData, value, str);
  }
  else {
    auto columnType = columnTypeMgr->getType(typeData.type);
    if (! columnType) return false;

    return formatColumnTypeValue(charts, model, column, columnType, typeData, value, str);
  }
}

bool
formatColumnTypeValue(CQCharts *charts, const QAbstractItemModel *model,
                      const CQChartsColumn &column, const CQChartsColumnType *columnType,
                      const CQChartsModelTypeData &typeData, double value, QString &str) {
  bool converted;

  QVariant var = columnType->dataName(charts, model, column, value, typeData, converted);

  if (! var.isValid())
    return false;

  CQChartsVariant::toString(var, str);

  return true;
}

// use column type details to format an internal model value (variant) to a display value
QVariant
columnDisplayData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                  const QVariant &var, bool &converted) {
  auto *columnTypeMgr = charts->columnTypeMgr();

  // TODO: use columnValueType not CQChartsColumnTypeMgr::getModelColumnType
  QVariant var1 = columnTypeMgr->getDisplayData(model, column, var, converted);

  return var1;
}

// use column type details to format an internal model value (variant) to a editable value
QVariant
columnUserData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
               const QVariant &var, bool &converted) {
  auto *columnTypeMgr = charts->columnTypeMgr();

  // TODO: use columnValueType not CQChartsColumnTypeMgr::getModelColumnType
  QVariant var1 = columnTypeMgr->getUserData(model, column, var, converted);

  return var1;
}

QVariant
columnHeaderUserData(CQCharts *charts, const QAbstractItemModel *model, int section,
                     const QVariant &var, bool &converted) {
  auto *columnTypeMgr = charts->columnTypeMgr();

  QVariant var1 = columnTypeMgr->getHeaderUserData(model, section, var, converted);

  return var1;
}

#if 0
// get type string for column (type name and name values)
bool
columnTypeStr(CQCharts *charts, const QAbstractItemModel *model,
              const CQChartsColumn &column, QString &typeStr) {
  CQChartsModelTypeData columnTypeData;

  if (! columnValueType(charts, model, column, columnTypeData))
    return false;

  auto *columnTypeMgr = charts->columnTypeMgr();

  typeStr = columnTypeMgr->encodeTypeData(columnTypeData.type, columnTypeData.nameValues);

  return true;
}
#endif

// get type string for column (type name and name values)
bool
columnTypeStr(CQCharts *charts, const QAbstractItemModel *model,
              const CQChartsColumn &column, QString &typeStr) {
  CQChartsModelTypeData columnTypeData;

  if (! columnValueType(charts, model, column, columnTypeData))
    return false;

  QStringList strs;

  strs << CQBaseModel::typeName(columnTypeData.type);

  for (const auto &nv : columnTypeData.nameValues.nameValues()) {
    QStringList strs1;

    strs1 << nv.first;
    strs1 << nv.second.toString();

    strs << CQTcl::mergeList(strs1);
  }

  typeStr = CQTcl::mergeList(strs);

  return true;
}

#if 0
// column_types: <column_type>;<column_type>;...
// column_type : <column>#<type> | <type>
// type        : <base_type>:<name_values> | <base_type>
// base_type   : integer|real|string|...
// name_values : <name_value>,<name_value>,...
// name_value  : <name>=<value> | <name>
bool
setColumnTypeStrs(CQCharts *charts, QAbstractItemModel *model, const QString &columnTypes)
{
  bool rc = true;

  // split into multiple column type definitions
  QStringList fstrs = columnTypes.split(";", QString::KeepEmptyParts);

  for (int i = 0; i < fstrs.length(); ++i) {
    QString typeStr = fstrs[i].simplified();

    if (! typeStr.length())
      continue;

    if (! setColumnTypeIndexStr(charts, model, i, typeStr))
      rc = false;
  }

  return rc;
}
#endif

bool
setColumnTypeStrs(CQCharts *charts, QAbstractItemModel *model, const QString &columnTypes)
{
  bool rc = true;

  // split into multiple column type definitions
  QStringList fstrs;

  if (! CQTcl::splitList(columnTypes, fstrs))
    return false;

  for (int i = 0; i < fstrs.length(); ++i) {
    QString typeStr = fstrs[i].simplified();

    if (! typeStr.length())
      continue;

    if (! setColumnTypeIndexStr(charts, model, i, typeStr))
      rc = false;
  }

  return rc;
}

#if 0
bool
setColumnTypeIndexStr(CQCharts *charts, QAbstractItemModel *model,
                      int i, const QString &typeStr)
{
  QString typeStr1 = typeStr;

  // default column to index
  CQChartsColumn column(i);

  // if #<col> then use that for column index
  int pos = typeStr1.indexOf("#");

  if (pos >= 0) {
    QString columnStr = typeStr1.mid(0, pos).simplified();

    CQChartsColumn column1;

    if (stringToColumn(model, columnStr, column1))
      column = column1;
    else {
      charts->errorMsg("Bad column name '" + columnStr + "'");
      return false;
    }

    typeStr1 = typeStr1.mid(pos + 1).simplified();
  }

  //---

  if (! setColumnTypeStr(charts, model, column, typeStr1)) {
    charts->errorMsg(QString("Invalid type '" + typeStr + "' for column '%1'").
                       arg(column.toString()));
    return false;
  }

  //---

  return true;
}
#endif

#if 0
// set type string for column (type name and name values)
bool
setColumnTypeStr(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                 const QString &typeStr) {
  auto *columnTypeMgr = charts->columnTypeMgr();

  // decode to type name and name values
  CQChartsNameValues nameValues;

  const auto *typeData = columnTypeMgr->decodeTypeData(typeStr, nameValues);

  if (! typeData)
    return false;

  // store in model
  CQBaseModelType columnType = typeData->type();

  if (! columnTypeMgr->setModelColumnType(model, column, columnType, nameValues))
    return false;

  return true;
}
#endif

bool
setColumnTypeIndexStr(CQCharts *charts, QAbstractItemModel *model, int ind,
                      const QString &typeStr)
{
  QString errorMsg;

  // { type         {name value} ...}
  // {{column type} {name value} ...}
  QStringList strs;

  if (! CQTcl::splitList(typeStr, strs)) {
    errorMsg = QString("Invalid column type string '%1'").arg(typeStr);
    return false;
  }

  if (strs.length() < 1) {
    errorMsg = QString("Invalid column type string '%1'").arg(typeStr);
    return false;
  }

  // skip empty definition
  if (strs.length() == 1 && strs[0].simplified() == "")
    return true;

  CQChartsColumn column;
  QString        typeName;

  QStringList strs1;

  if (CQTcl::splitList(strs[0], strs1) && strs1.length() > 1) {
    if (strs1.length() != 2) {
      errorMsg = QString("Invalid column type string '%1'").arg(strs[0]);
      return false;
    }

    if (! CQChartsModelUtil::stringToColumn(model, strs1[0], column)) {
      errorMsg = QString("Invalid column string '%1'").arg(strs1[0]);
      return false;
    }

    typeName = strs1[1];
  }
  else {
    column   = CQChartsColumn(ind);
    typeName = strs[0];
  }

  return setColumnTypeStrI(charts, model, column, typeName, typeStr, strs.mid(1), errorMsg);
}

bool
setColumnTypeStr(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                 const QString &typeStr)
{
  QString errorMsg;

  // {type {name value} ...}
  QStringList strs;

  if (! CQTcl::splitList(typeStr, strs)) {
    errorMsg = QString("Invalid column type string '%1'").arg(typeStr);
    return false;
  }

  if (strs.length() < 1) {
    errorMsg = QString("Invalid column type string '%1'").arg(typeStr);
    return false;
  }

  // skip empty definition
  if (strs.length() == 1 && strs[0].simplified() == "")
    return true;

  QString typeName = strs[0];

  return setColumnTypeStrI(charts, model, column, typeName, typeStr, strs.mid(1), errorMsg);
}

bool
setColumnTypeStrI(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                  const QString &typeName, const QString &typeStr, const QStringList &strs,
                  QString &errorMsg) {
  CQChartsNameValues nameValues;

  for (int i = 0; i < strs.length(); ++i) {
    QStringList strs1;

    if (! CQTcl::splitList(strs[i], strs1)) {
      errorMsg = QString("Invalid column type string '%1'").arg(strs[i]);
      return false;
    }

    if (strs1.length() != 2) {
      errorMsg = QString("Invalid column type string '%1'").arg(strs[i]);
      return false;
    }

    nameValues.setNameValue(strs1[0], strs1[1]);
  }

  auto *columnTypeMgr = charts->columnTypeMgr();

  const auto *typeData = columnTypeMgr->getType(CQBaseModel::nameType(typeName));

  if (! typeData) {
    errorMsg = QString("Invalid column type '%1'").arg(typeName);
    return false;
  }

  // store in model
  CQBaseModelType columnType = typeData->type();

  if (! columnTypeMgr->setModelColumnType(model, column, columnType, nameValues)) {
    errorMsg = QString("Failed to set column type '%1'").arg(typeStr);
    return false;
  }

  return true;
}

//---

bool
setHeaderTypeStrs(CQCharts *charts, QAbstractItemModel *model, const QString &columnTypes)
{
  bool rc = true;

  // split into multiple column type definitions
  QStringList fstrs;

  if (! CQTcl::splitList(columnTypes, fstrs))
    return false;

  for (int i = 0; i < fstrs.length(); ++i) {
    QString typeStr = fstrs[i].simplified();

    if (! typeStr.length())
      continue;

    if (! setHeaderTypeIndexStr(charts, model, i, typeStr))
      rc = false;
  }

  return rc;
}

bool
setHeaderTypeIndexStr(CQCharts *charts, QAbstractItemModel *model, int ind,
                      const QString &typeStr)
{
  QString errorMsg;

  // { type         {name value} ...}
  // {{header type} {name value} ...}
  QStringList strs;

  if (! CQTcl::splitList(typeStr, strs)) {
    errorMsg = QString("Invalid header type string '%1'").arg(typeStr);
    return false;
  }

  if (strs.length() < 1) {
    errorMsg = QString("Invalid header type string '%1'").arg(typeStr);
    return false;
  }

  // skip empty definition
  if (strs.length() == 1 && strs[0].simplified() == "")
    return true;

  CQChartsColumn column;
  QString        typeName;

  QStringList strs1;

  if (CQTcl::splitList(strs[0], strs1) && strs1.length() > 1) {
    if (strs1.length() != 2) {
      errorMsg = QString("Invalid header type string '%1'").arg(strs[0]);
      return false;
    }

    if (! CQChartsModelUtil::stringToColumn(model, strs1[0], column)) {
      errorMsg = QString("Invalid header string '%1'").arg(strs1[0]);
      return false;
    }

    typeName = strs1[1];
  }
  else {
    column   = CQChartsColumn(ind);
    typeName = strs[0];
  }

  return setHeaderTypeStrI(charts, model, column, typeName, typeStr, strs.mid(1), errorMsg);
}

bool
setHeaderTypeStr(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                 const QString &typeStr)
{
  QString errorMsg;

  // {type {name value} ...}
  QStringList strs;

  if (! CQTcl::splitList(typeStr, strs)) {
    errorMsg = QString("Invalid header type string '%1'").arg(typeStr);
    return false;
  }

  if (strs.length() < 1) {
    errorMsg = QString("Invalid header type string '%1'").arg(typeStr);
    return false;
  }

  // skip empty definition
  if (strs.length() == 1 && strs[0].simplified() == "")
    return true;

  QString typeName = strs[0];

  return setHeaderTypeStrI(charts, model, column, typeName, typeStr, strs.mid(1), errorMsg);
}

bool
setHeaderTypeStrI(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                  const QString &typeName, const QString &typeStr, const QStringList &strs,
                  QString &errorMsg) {
  CQChartsNameValues nameValues;

  for (int i = 0; i < strs.length(); ++i) {
    QStringList strs1;

    if (! CQTcl::splitList(strs[i], strs1)) {
      errorMsg = QString("Invalid header type string '%1'").arg(strs[i]);
      return false;
    }

    if (strs1.length() != 2) {
      errorMsg = QString("Invalid header type string '%1'").arg(strs[i]);
      return false;
    }

    nameValues.setNameValue(strs1[0], strs1[1]);
  }

  auto *columnTypeMgr = charts->columnTypeMgr();

  const auto *typeData = columnTypeMgr->getType(CQBaseModel::nameType(typeName));

  if (! typeData) {
    errorMsg = QString("Invalid header type '%1'").arg(typeName);
    return false;
  }

  // store in model
  CQBaseModelType columnType = typeData->type();

  if (! columnTypeMgr->setModelHeaderType(model, column, columnType, nameValues)) {
    errorMsg = QString("Failed to set header type '%1'").arg(typeStr);
    return false;
  }

  return true;
}

}

//------

namespace CQChartsModelUtil {

void
processAddExpression(QAbstractItemModel *model, const QString &exprStr)
{
  auto *exprModel = getExprModel(model);

  if (! exprModel) {
    errorMsg("Expression not supported for model");
    return;
  }

  int column;

  exprModel->addExtraColumn(exprStr, column);
}

int
processExpression(QAbstractItemModel *model, const QString &exprStr)
{
  auto *exprModel = getExprModel(model);

  if (! exprModel) {
    errorMsg("Expression not supported for model");
    return -1;
  }

  CQChartsExprModel::Function function { CQChartsExprModel::Function::EVAL };
  int                         icolumn  { -1 };
  QString                     expr;

  if (! exprModel->decodeExpressionFn(exprStr, function, icolumn, expr)) {
    errorMsg("Invalid expression '" + exprStr + "'");
    return -1;
  }

  CQChartsColumn column(icolumn);

  return processExpression(model, function, column, expr);
}

int
processExpression(QAbstractItemModel *model, CQChartsExprModel::Function function,
                  const CQChartsColumn &column, const QString &expr)
{
  auto *exprModel = getExprModel(model);

  if (! exprModel) {
    errorMsg("Expression not supported for model");
    return -1;
  }

  // add column <expr>
  if      (function == CQChartsExprModel::Function::ADD) {
    int column1;

    if (! exprModel->addExtraColumn(expr, column1))
      return -1;

    return column1;
  }
  // delete column <n>
  else if (function == CQChartsExprModel::Function::DELETE) {
    int icolumn = column.column();

    if (icolumn < 0) {
      errorMsg("Invalid column");
      return -1;
    }

    bool rc = exprModel->removeExtraColumn(icolumn);

    if (! rc) {
      errorMsg(QString("Failed to delete column '%1'").arg(icolumn));
      return -1;
    }

    return icolumn;
  }
  // modify column <n>:<expr>
  else if (function == CQChartsExprModel::Function::ASSIGN) {
    int icolumn = column.column();

    if (icolumn < 0) {
      errorMsg("Invalid column");
      return -1;
    }

    if (! exprModel->assignExtraColumn(icolumn, expr))
      return -1;

    return icolumn;
  }
  else {
    exprModel->processExpr(expr);

    return -1;
  }
}

const CQChartsExprModel *
getExprModel(const QAbstractItemModel *model)
{
  return getExprModel(const_cast<QAbstractItemModel *>(model));
}

CQChartsExprModel *
getExprModel(QAbstractItemModel *model)
{
//std::cerr << (model ? model->objectName().toStdString() : "null") << "\n";
  auto *exprModel = qobject_cast<CQChartsExprModel *>(model);

  if (exprModel)
    return exprModel;

  auto *sortModel = getSortFilterProxyModel(model);
  if (! sortModel) return nullptr;

  auto *sourceModel = sortModel->sourceModel();
//std::cerr << (sourceModel ? sourceModel->objectName().toStdString() : "null") << "\n";

  exprModel = qobject_cast<CQChartsExprModel *>(sourceModel);

  if (exprModel)
    return exprModel;

  auto *proxyModel = qobject_cast<QAbstractProxyModel *>(sourceModel);

  if (! proxyModel)
    return nullptr;

  sourceModel = proxyModel->sourceModel();
//std::cerr << (sourceModel ? sourceModel->objectName().toStdString() : "null") << "\n";

  return getExprModel(sourceModel);

#if 0
  exprModel = qobject_cast<CQChartsExprModel *>(sourceModel);

  if (exprModel)
    return exprModel;

  sortModel = getSortFilterProxyModel(sourceModel);
  if (! sortModel) return nullptr;

  sourceModel = sortModel->sourceModel();
//std::cerr << (sourceModel ? sourceModel->objectName().toStdString() : "null") << "\n";

  exprModel = qobject_cast<CQChartsExprModel *>(sourceModel);

  if (exprModel)
    return exprModel;

  return nullptr;
#endif
}

CQHierSepModel *
getHierSepModel(QAbstractItemModel *model)
{
  auto *hierSepModel = qobject_cast<CQHierSepModel *>(model);

  if (hierSepModel)
    return hierSepModel;

  auto *sortModel = getSortFilterProxyModel(model);
  if (! sortModel) return nullptr;

  auto *sourceModel = sortModel->sourceModel();

  hierSepModel = qobject_cast<CQHierSepModel *>(sourceModel);

  if (hierSepModel)
    return hierSepModel;

  return nullptr;

}

QSortFilterProxyModel *
getSortFilterProxyModel(QAbstractItemModel *model)
{
  auto *sortModel = qobject_cast<QSortFilterProxyModel *>(model);
  if (! sortModel) return nullptr;

  return sortModel;
}

const CQDataModel *
getDataModel(const QAbstractItemModel *model)
{
  return getDataModel(const_cast<QAbstractItemModel *>(model));
}

CQDataModel *
getDataModel(QAbstractItemModel *model)
{
  auto *modelFilter = dynamic_cast<CQChartsModelFilter *>(model);
  if (! modelFilter) return nullptr;

  auto *dataModel = dynamic_cast<CQDataModel *>(modelFilter->baseModel());
  if (! dataModel) return nullptr;

  return dataModel;
}

QAbstractItemModel *
getBaseModel(QAbstractItemModel *model)
{
  return CQModelUtil::getBaseModel(model);
}

//---

#if 0
bool
getProperty(const QAbstractItemModel *model, const QString &name, QVariant &value)
{
  if (CQUtil::getTclProperty(model, name, value))
    return true;

  auto *baseModel = getBaseModel(const_cast<QAbstractItemModel *>(model));

  if (baseModel && baseModel != model) {
    if (CQUtil::getTclProperty(baseModel, name, value))
      return true;
  }

  return false;
}

bool
setProperty(QAbstractItemModel *model, const QString &name, const QVariant &value)
{
  if (CQUtil::setProperty(model, name, value))
    return true;

  auto *baseModel = getBaseModel(model);

  if (baseModel && baseModel != model) {
    if (CQUtil::setProperty(baseModel, name, value))
      return true;
  }

  return false;
}
#endif

//---

QVariant
getModelMetaValue(const QAbstractItemModel *model, const QString &name)
{
  const auto *dataModel = getDataModel(model);
  if (! dataModel) return QVariant();

  return dataModel->nameValue(name);
}

bool
setModelMetaValue(QAbstractItemModel *model, const QString &name, const QVariant &value)
{
  auto *dataModel = getDataModel(model);
  if (! dataModel) return false;

  dataModel->setNameValue(name, value);

  return true;
}

}

//------

namespace CQChartsModelUtil {

QVariant modelHeaderValueI(const QAbstractItemModel *model, const CQChartsColumn &column,
                           Qt::Orientation orient, int role, bool &ok) {
  ok = false;

  if (! column.isValid())
    return QVariant();

  if (column.hasName())
    return column.name();

  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return QVariant();

  int icolumn = column.column();

  if (icolumn < 0)
    return QVariant();

  return CQModelUtil::modelHeaderValue(model, icolumn, orient, role, ok);
}

QVariant modelHeaderValue(const QAbstractItemModel *model, int section,
                          Qt::Orientation orient, int role, bool &ok) {
  return modelHeaderValueI(model, CQChartsColumn(section), orient, role, ok);
}

QVariant modelHeaderValue(const QAbstractItemModel *model, int section,
                          Qt::Orientation orient, bool &ok) {
  return modelHeaderValueI(model, CQChartsColumn(section), orient, Qt::DisplayRole, ok);
}

QVariant modelHeaderValue(const QAbstractItemModel *model, const CQChartsColumn &column,
                          int role, bool &ok) {
  return modelHeaderValueI(model, column, Qt::Horizontal, role, ok);
}

QVariant modelHeaderValue(const QAbstractItemModel *model, const CQChartsColumn &column,
                          bool &ok) {
  return modelHeaderValueI(model, column, Qt::Horizontal, column.role(Qt::DisplayRole), ok);
}

//--

QString modelHeaderStringI(const QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, int role, bool &ok) {
  QVariant var = modelHeaderValueI(model, column, orient, role, ok);
  if (! var.isValid()) return "";

  QString str;

  bool rc = CQChartsVariant::toString(var, str);
  assert(rc);

  return str;
}

QString modelHeaderString(const QAbstractItemModel *model, int section,
                          Qt::Orientation orient, int role, bool &ok) {
  return modelHeaderStringI(model, CQChartsColumn(section), orient, role, ok);
}

QString modelHeaderString(const QAbstractItemModel *model, int section,
                          Qt::Orientation orient, bool &ok) {
  return modelHeaderStringI(model, CQChartsColumn(section), orient, Qt::DisplayRole, ok);
}

QString modelHHeaderString(const QAbstractItemModel *model, const CQChartsColumn &column,
                           int role, bool &ok) {
  return modelHeaderStringI(model, column, Qt::Horizontal, role, ok);
}

QString modelHHeaderString(const QAbstractItemModel *model, const CQChartsColumn &column,
                           bool &ok) {
  return modelHeaderStringI(model, column, Qt::Horizontal, column.role(Qt::DisplayRole), ok);
}

//--

bool setModelHeaderValueI(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, const QVariant &var, int role) {
  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return false;

  return model->setHeaderData(column.column(), orient, var, role);
}

bool setModelHeaderValue(QAbstractItemModel *model, int section,
                         Qt::Orientation orient, const QVariant &var, int role) {
  return setModelHeaderValueI(model, CQChartsColumn(section), orient, var, role);
}

bool setModelHeaderValue(QAbstractItemModel *model, int section,
                         Qt::Orientation orient, const QVariant &var) {
  return setModelHeaderValueI(model, CQChartsColumn(section), orient, var, Qt::EditRole);
}

bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         const QVariant &var, int role) {
  return setModelHeaderValueI(model, column, Qt::Horizontal, var, role);
}

bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         const QVariant &var) {
  return setModelHeaderValueI(model, column, Qt::Horizontal, var, column.role(Qt::EditRole));
}

//--

bool setModelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                   const QVariant &var, int role) {
  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return false;

  QModelIndex ind = model->index(row, column.column(), QModelIndex());

  return model->setData(ind, var, role);
}

bool setModelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                   const QVariant &var) {
  return setModelValue(model, row, column, var, column.role(Qt::EditRole));
}

//--

QVariant modelValue(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  return CQModelUtil::modelValue(model, ind, role, ok);
}

QVariant modelValue(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QVariant var = modelValue(model, ind, Qt::EditRole, ok);

  if (! ok)
    var = modelValue(model, ind, Qt::DisplayRole, ok);

  return var;
}

QVariant modelValue(CQCharts *charts, const QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent,
                    int role, bool &ok) {
  if (! column.isValid()) {
    ok = false;

    return QVariant();
  }

  if      (column.type() == CQChartsColumn::Type::DATA) {
    QModelIndex ind = model->index(row, column.column(), parent);

    return modelValue(model, ind, role, ok);
  }
  else if (column.type() == CQChartsColumn::Type::DATA_INDEX) {
    QModelIndex ind = model->index(row, column.column(), parent);

    QVariant var = modelValue(model, ind, role, ok);
    if (! ok) return QVariant();

    CQChartsModelTypeData columnTypeData;

    if (! columnValueType(charts, model, column, columnTypeData))
      return var;

    auto *columnTypeMgr = charts->columnTypeMgr();

    const auto *typeData = columnTypeMgr->getType(columnTypeData.type);

    return typeData->indexVar(var, column.index());
  }
  else if (column.type() == CQChartsColumn::Type::ROW) {
    ok = true;

    return row + column.rowOffset();
  }
  else if (column.type() == CQChartsColumn::Type::VHEADER) {
    bool ok;

    QVariant var = CQModelUtil::modelHeaderValue(model, row, Qt::Vertical, role, ok);

    return var;
  }
  else if (column.type() == CQChartsColumn::Type::GROUP) {
    bool ok;

    QVariant var =
      CQModelUtil::modelHeaderValue(model, row, Qt::Vertical, CQBaseModelRole::Group, ok);

    return var;
  }
  else if (column.type() == CQChartsColumn::Type::EXPR) {
    bool showError = false;

    QVariant var;

    auto *qtcl = const_cast<CQChartsExprTcl *>(charts->currentExpr());

    if (! qtcl) {
      auto *eval = CQChartsColumnEvalInst;

      eval->setModel(model);
      eval->setRow  (row);

      ok = eval->evaluateExpression(column.expr(), var, showError);
    }
    else {
      qtcl->setRow(row);

      ok = qtcl->evaluateExpression(column.expr(), var, showError);
    }

    return var;
  }
  else {
    ok = false;

    return QVariant();
  }
}

QVariant modelValue(CQCharts *charts, const QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  QVariant var;

  if (! column.hasRole()) {
    var = modelValue(charts, model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      var = modelValue(charts, model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    var = modelValue(charts, model, row, column, parent, column.role(), ok);

  return var;
}

//---

QString modelString(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  QVariant var = modelValue(model, ind, role, ok);
  if (! ok) return "";

  QString str;

  bool rc = CQChartsVariant::toString(var, str);
  assert(rc);

  return str;
}

QString modelString(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  QString str = modelString(model, ind, Qt::EditRole, ok);

  if (! ok)
    str = modelString(model, ind, Qt::DisplayRole, ok);

  return str;
}

QString modelString(CQCharts *charts, const QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok) {
  QVariant var = modelValue(charts, model, row, column, parent, role, ok);
  if (! ok) return "";

  QString str;

  bool rc = CQChartsVariant::toString(var, str);
  assert(rc);

  return str;
}

QString modelString(CQCharts *charts, const QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  QString str;

  if (! column.hasRole()) {
    str = modelString(charts, model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      str = modelString(charts, model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    str = modelString(charts, model, row, column, parent, column.role(), ok);

  return str;
}

QString modelString(CQCharts *charts, const QAbstractItemModel *model,
                    const CQChartsModelIndex &ind, bool &ok)
{
  return modelString(charts, model, ind.row, ind.column, ind.parent, ok);
}

//---

#if 0
QString modelHierString(CQCharts *charts, const QAbstractItemModel *model, int row,
                        const CQChartsColumn &column, const QModelIndex &parent,
                        int role, bool &ok) {
  QString s = modelString(charts, model, row, column, parent, role, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      s = modelString(charts, model, row1, column, parent1, role, ok);
    }
  }

  return s;
}

QString modelHierString(CQCharts *charts, const QAbstractItemModel *model, int row,
                        const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  QString s = modelString(charts, model, row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      s = modelString(charts, model, row1, column, parent1, ok);
    }
  }

  return s;
}
#endif

//---

double modelReal(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  QVariant var = modelValue(model, ind, role, ok);
  if (! ok) return 0.0;

  return CQChartsVariant::toReal(var, ok);
}

double modelReal(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  double r = modelReal(model, ind, Qt::EditRole, ok);

  if (! ok)
    r = modelReal(model, ind, Qt::DisplayRole, ok);

  return r;
}

double modelReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                 const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok) {
  QVariant var = modelValue(charts, model, row, column, parent, role, ok);
  if (! ok) return 0.0;

  return CQChartsVariant::toReal(var, ok);
}

double modelReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                 const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  double r;

  if (! column.hasRole()) {
    r = modelReal(charts, model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      r = modelReal(charts, model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    r = modelReal(charts, model, row, column, parent, column.role(), ok);

  return r;
}

#if 0
double modelHierReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                     const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok) {
  double r = modelReal(charts, model, row, column, parent, role, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      r = modelReal(charts, model, row1, column, parent1, role, ok);
    }
  }

  return r;
}

double modelHierReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                     const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  double r = modelReal(charts, model, row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      r = modelReal(charts, model, row1, column, parent1, ok);
    }
  }

  return r;
}
#endif

//---

long modelInteger(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok) {
  QVariant var = modelValue(model, ind, role, ok);
  if (! ok) return 0;

  return CQChartsVariant::toInt(var, ok);
}

long modelInteger(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  long l = modelInteger(model, ind, Qt::EditRole, ok);

  if (! ok)
    l = modelInteger(model, ind, Qt::DisplayRole, ok);

  return l;
}

long modelInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                  const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok) {
  QVariant var = modelValue(charts, model, row, column, parent, role, ok);
  if (! ok) return 0;

  return CQChartsVariant::toInt(var, ok);
}

long modelInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                  const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  long l;

  if (! column.hasRole()) {
    l = modelInteger(charts, model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      l = modelInteger(charts, model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    l = modelInteger(charts, model, row, column, parent, column.role(), ok);

  return l;
}

#if 0
long modelHierInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                      const CQChartsColumn &column, const QModelIndex &parent,
                      int role, bool &ok) {
  long l = modelInteger(charts, model, row, column, parent, role, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      l = modelInteger(charts, model, row1, column, parent1, role, ok);
    }
  }

  return l;
}

long modelHierInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                      const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  long l = modelInteger(charts, model, row, column, parent, ok);

  if (! ok && column.column() == 0 && parent.isValid()) {
    QModelIndex parent1 = parent;
    int         row1    = row;

    while (! ok && parent1.isValid()) {
      row1    = parent1.row();
      parent1 = parent1.parent();

      l = modelInteger(charts, model, row1, column, parent1, ok);
    }
  }

  return l;
}
#endif

//---

CQChartsColor variantToColor(const QVariant &var) {
  bool ok;

  if (CQChartsVariant::isColor(var))
    return CQChartsVariant::toColor(var, ok);

  CQChartsColor color;

  if (CQChartsVariant::isReal(var)) {
    double r;

    if (CQChartsVariant::toReal(var, r))
      color = CQChartsColor(CQChartsColor::Type::PALETTE, r);
  }
  else {
    QString str;

    if (CQChartsVariant::toString(var, str))
      color = CQChartsColor(str);
  }

  return color;
}

#if 0
CQChartsColor modelColor(const QAbstractItemModel *model, const QModelIndex &ind,
                         int role, bool &ok) {
  QVariant var = modelValue(model, ind, role, ok);
  if (! ok) return CQChartsColor();

  return variantToColor(var);
}

CQChartsColor modelColor(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok) {
  CQChartsColor c = modelColor(model, ind, Qt::EditRole, ok);

  if (! ok)
    c = modelColor(model, ind, Qt::DisplayRole, ok);

  return c;
}

CQChartsColor modelColor(CQCharts *charts, const QAbstractItemModel *model, int row,
                         const CQChartsColumn &column, const QModelIndex &parent,
                         int role, bool &ok) {
  QVariant var = modelValue(charts, model, row, column, parent, role, ok);
  if (! ok) return CQChartsColor();

  return variantToColor(var);
}

CQChartsColor modelColor(CQCharts *charts, const QAbstractItemModel *model, int row,
                         const CQChartsColumn &column, const QModelIndex &parent, bool &ok) {
  CQChartsColor c;

  if (! column.hasRole()) {
    c = modelColor(charts, model, row, column, parent, Qt::EditRole, ok);

    if (! ok)
      c = modelColor(charts, model, row, column, parent, Qt::DisplayRole, ok);
  }
  else
    c = modelColor(charts, model, row, column, parent, column.role(), ok);

  return c;
}
#endif

}

//------

namespace CQChartsModelUtil {

#if 0
bool isValidModelColumn(const QAbstractItemModel *model, int column) {
  return (column >= 0 && column < model->columnCount());
}
#endif

int modelColumnNameToInd(const QAbstractItemModel *model, const QString &name) {
  assert(model);

#if 0
  int role = Qt::DisplayRole;

  for (int icolumn = 0; icolumn < model->columnCount(); ++icolumn) {
    bool ok;

    QVariant var = modelHeaderValue(model, icolumn, role, ok);
    if (! ok)) continue;

    //QString name1 = CQChartsVariant::toString(var, rc);

    QString name1;

    bool rc = CQChartsVariant::toString(var, name1);
    assert(rc);

    if (name == name1)
      return icolumn;
  }

  //---

  bool ok;

  int column = CQChartsUtil::toInt(name, ok);

  if (ok)
    return column;

  return -1;
#else
  return CQBaseModel::modelColumnNameToInd(model, name);
#endif
}

bool stringToColumn(const QAbstractItemModel *model, const QString &str, CQChartsColumn &column) {
  assert(model);

  if (! str.length())
    return false;

  //---

  // handle special column syntax or expression
  CQChartsColumn column1(str);

  if (column1.isValid()) {
    column = column1;

    return true;
  }

  //---

  // get column from name (exact match)
  int icolumn = modelColumnNameToInd(model, str);

  if (icolumn >= 0) {
    column = CQChartsColumn(icolumn);
    return true;
  }

  //---

  // check for column sub value
  QString str1 = str.simplified();

  if (str1.right(1) == "]") {
    int pos = str1.lastIndexOf('[');

    QString lhs = str1.mid(0, pos);
    QString rhs = str1.mid(pos + 1, str1.length() - pos - 2);

    CQChartsColumn column1;

    if (! stringToColumn(model, lhs, column1))
      return false;

    if (column1.type() != CQChartsColumn::Type::DATA)
      return false;

    column = CQChartsColumn(CQChartsColumn::Type::DATA_INDEX,
                            column1.column(), rhs, column1.role());

    return true;
  }

  //---

  // fail
  return false;
}

bool stringToColumns(const QAbstractItemModel *model, const QString &str,
                     std::vector<CQChartsColumn> &columns) {
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  bool rc = true;

  for (int i = 0; i < strs.length(); ++i) {
    const QString &str = strs[i];

    int pos = str.indexOf('~');

    if (pos >= 0) {
      QString lhs = str.mid(0, pos);
      QString rhs = str.mid(pos + 1);

      CQChartsColumn c1, c2;

      if (stringToColumn(model, lhs, c1) && stringToColumn(model, rhs, c2)) {
        if (c1.hasColumn() && c2.hasColumn()) {
          int col1 = c1.column();
          int col2 = c2.column();

          if (col1 > col2)
            std::swap(col1, col2);

          for (int c = col1; c <= col2; ++c)
            columns.push_back(CQChartsColumn(c));
        }
        else
          rc = false;
      }
      else
        rc = false;
    }
    else {
      CQChartsColumn c;

      if (! stringToColumn(model, str, c))
        rc = false;

      columns.push_back(c);
    }
  }

  return rc;
}

bool stringToModelInd(const QAbstractItemModel *model, const QString &str,
                      CQChartsModelIndex &ind) {
  QStringList strs;

  if (! CQTcl::splitList(str, strs))
    return false;

  if (strs.length() < 2)
    return false;

  bool ok;

  int row = strs[0].toInt(&ok);

  if (! ok)
    return false;

  CQChartsColumn column;

  if (! stringToColumn(model, strs[1], column)) {
    bool ok;

    int icol = strs[1].toInt(&ok);

    if (! ok)
      return false;

    column = CQChartsColumn(icol);
  }

  ind = CQChartsModelIndex(row, column);

  return true;
}

}

//------

namespace CQChartsModelUtil {

QString
replaceModelExprVars(const QString &expr, const QAbstractItemModel *model,
                     const QModelIndex &ind, int nr, int nc)
{
  assert(model);

  auto quoteStr = [](const QString &str, bool doQuote) {
    return (doQuote ? "\"" + str + "\"" : str);
  };

  CQStrParse parse(expr);

  QString expr1;

  while (! parse.eof()) {
    // @<n> get column value (current row)
    if      (parse.isChar('@')) {
      parse.skipChar();

      bool stringify = false;

      if (parse.isChar('#')) {
        parse.skipChar();

        stringify = true;
      }

      // @<n> get column value for current row
      if      (parse.isDigit()) {
        int pos = parse.getPos();

        while (parse.isDigit())
          parse.skipChar();

        QString str = parse.getBefore(pos);

        bool ok;

        int column1 = (int) CQChartsUtil::toInt(str, ok);

        if (stringify) {
          QModelIndex ind1 = model->index(ind.row(), column1, ind.parent());

          bool ok;

          QVariant var = CQModelUtil::modelValue(model, ind1, Qt::DisplayRole, ok);

          expr1 += quoteStr(var.toString(), true);
        }
        else
          expr1 += QString("column(%1)").arg(column1);
      }
      // @c get column number
      else if (parse.isChar('c')) {
        parse.skipChar();

        if (ind.isValid() && ind.column() >= 0)
          expr1 += quoteStr(QString("%1").arg(ind.column()), stringify);
        else
          expr1 += "@c";
      }
      // @r get row number
      else if (parse.isChar('r')) {
        parse.skipChar();

        if (ind.isValid() && ind.row() >= 0)
          expr1 += quoteStr(QString("%1").arg(ind.row()), stringify);
        else
          expr1 += "@r";
      }
      else if (parse.isChar('n')) {
        parse.skipChar();

        // @nc get number of columns
        if      (parse.isChar('c')) {
          parse.skipChar();

          if (nc >= 0)
            expr1 += quoteStr(QString("%1").arg(nc), stringify);
          else
            expr1 += "@nc";
        }
        // @nr get number of rows
        else if (parse.isChar('r')) {
          parse.skipChar();

          if (nr >= 0)
            expr1 += quoteStr(QString("%1").arg(nr), stringify);
          else
            expr1 += "@nr";
        }
        else {
          if (stringify)
            expr1 += "@#n";
          else
            expr1 += "@n";
        }
      }
      // @v current value
      else if (parse.isChar('v')) {
        parse.skipChar();

        if (model && ind.isValid()) {
          if (stringify) {
            bool ok;

            QVariant var = CQModelUtil::modelValue(model, ind, Qt::DisplayRole, ok);

            expr1 += quoteStr(var.toString(), true);
          }
          else
            expr1 += QString("column(%1)").arg(ind.column());
        }
        else
          expr1 += "@v";
      }
      // @{<name>} named column value
      else if (parse.isChar('{')) {
        int pos = parse.getPos();

        parse.skipChar();

        while (! parse.eof() && ! parse.isChar('}'))
          parse.skipChar();

        QString str = parse.getBefore(pos + 1);

        if (parse.isChar('}'))
          parse.skipChar();

        CQChartsColumn c;

        // @{<name>} column value for current row
        if (model && stringToColumn(model, str, c))
          expr1 += QString("column(%1)").arg(c.column());
        else {
          parse.setPos(pos);

          expr1 += "@";
        }
      }
      else {
        if (stringify)
          expr1 += "@#";
        else
          expr1 += "@";
      }
    }
    else if (parse.isChar('#')) {
      parse.skipChar();

      // #{name} get column number for name
      if (parse.isChar('{')) {
        int pos = parse.getPos();

        parse.skipChar();

        while (! parse.eof() && ! parse.isChar('}'))
          parse.skipChar();

        QString str = parse.getBefore(pos + 1);

        if (parse.isChar('}'))
          parse.skipChar();

        CQChartsColumn c;

        if (model && stringToColumn(model, str, c))
          expr1 += QString("%1").arg(c.column());
        else {
          parse.setPos(pos);

          expr1 += "#";
        }
      }
      else {
        expr1 += "#";
      }
    }
    else {
      expr1 += parse.getChar();
    }
  }

  return expr1;
}

}

//------

namespace CQChartsModelUtil {

bool decodeModelFilterStr(const QAbstractItemModel *model, const QString &filter,
                          QString &filter1, int &column) {
  assert(model);

  filter1 = filter;
  column  = -1;

  if (! filter1.length())
    return false;

  QStringList strs = filter1.split(':', QString::KeepEmptyParts);

  if (strs.size() != 2)
    return false;

  column = modelColumnNameToInd(model, strs[0]);

  if (column < 0)
    return false;

  filter1 = strs[1];

  return true;
}

bool decodeModelFilterStrs(const QAbstractItemModel *model, const QString &filter,
                           FilterColumns &filterColumns) {
  assert(model);

  if (! filter.length())
    return false;

  QStringList strs = filter.split(' ', QString::SkipEmptyParts);

  for (auto &str : strs) {
    QString filter;
    int     column = -1;

    if (! decodeModelFilterStr(model, str, filter, column))
      continue;

    filterColumns.push_back(FilterColumn(filter, column));
  }

  if (filterColumns.empty())
    return false;

  return true;
}

}

//------

namespace CQChartsModelUtil {

bool
exportModel(const QAbstractItemModel *model, CQBaseModelDataType type,
            bool hheader, bool vheader, std::ostream &os) {
  if      (type == CQBaseModelDataType::CSV) {
    CQCsvModel csv;

    csv.setFirstLineHeader  (hheader);
    csv.setFirstColumnHeader(vheader);

    csv.save(const_cast<QAbstractItemModel *>(model), os);
  }
  else if (type == CQBaseModelDataType::TSV) {
    CQTsvModel tsv;

    tsv.setFirstLineHeader  (hheader);
    tsv.setFirstColumnHeader(vheader);

    tsv.save(const_cast<QAbstractItemModel *>(model), os);
  }
  else if (type == CQBaseModelDataType::JSON) {
    CQJsonModel json;

    json.save(const_cast<QAbstractItemModel *>(model), os);
  }
  else {
    assert(false);
  }

  return true;
}

}

//------

namespace CQChartsModelUtil {

CQChartsFilterModel *flattenModel(CQCharts *charts, QAbstractItemModel *model,
                                  const FlattenData &flattenData) {
  class FlattenVisitor : public CQChartsModelVisitor {
   public:
    FlattenVisitor(CQCharts *charts, const CQChartsColumn &groupColumn) :
     charts_(charts), groupColumn_(groupColumn) {
    }

    State hierVisit(const QAbstractItemModel *model, const VisitData &data) override {
      ++hierRow_;

      bool ok;

      groupValue_[hierRow_] =
        modelValue(charts_, model, data.row, CQChartsColumn(0), data.parent, ok);

      return State::OK;
    }

    State visit(const QAbstractItemModel *model, const VisitData &data) override {
      int nc = numCols();

      if      (isHierarchical()) {
        for (int c = 1; c < nc; ++c) {
          bool ok;

          auto var = modelValue(charts_, model, data.row, CQChartsColumn(c), data.parent, ok);

          if (ok)
            rowColValueSet_[hierRow_][c - 1].addValue(var);
        }
      }
      else if (groupColumn_.isValid()) {
        bool ok;

        auto groupVar = modelValue(charts_, model, data.row, groupColumn_, data.parent, ok);

        auto p = valueGroup_.find(groupVar);

        if (p == valueGroup_.end()) {
          int group = valueGroup_.size();

          p = valueGroup_.insert(p, ValueGroup::value_type(groupVar, group));

          groupValue_[group] = groupVar;
        }

        int group = (*p).second;

        for (int c = 0; c < nc; ++c) {
          bool ok;

          auto var = modelValue(charts_, model, data.row, CQChartsColumn(c), data.parent, ok);

          if (ok)
            rowColValueSet_[group][c].addValue(var);
        }
      }
      else {
        for (int c = 0; c < nc; ++c) {
          bool ok;

          auto var = modelValue(charts_, model, data.row, CQChartsColumn(c), data.parent, ok);

          if (ok)
            rowColValueSet_[0][c].addValue(var);
        }
      }

      return State::OK;
    }

    int numHierColumns() const { return (isHierarchical() ? 1 : 0); }

    int numFlatColumns() const { return numCols() - numHierColumns(); }

    int numHierRows() const { return rowColValueSet_.size(); }

    QVariant groupValue(int row) {
      assert(row >= 0 && row <= int(groupValue_.size()));

      return groupValue_[row];
    }

    double hierSum(int r, int c) const {
      return valueSet(r, c).rsum();
    }

    double hierMean(int r, int c) const {
      return valueSet(r, c).rmean();
    }

    QVariant uniqueValue(int r, int c) const {
      return valueSet(r, c).uniqueValue();
    }

   private:
    CQChartsValueSet &valueSet(int r, int c) const {
      assert(r >= 0 && r <= int(rowColValueSet_.size()));
      assert(c >= 0 && c <= numFlatColumns());

      auto *th = const_cast<FlattenVisitor *>(this);

      return th->rowColValueSet_[r][c];
    }

   private:
    using ValueGroup     = std::map<QVariant,int>;
    using GroupValue     = std::map<int,QVariant>;
    using ColValueSet    = std::map<int,CQChartsValueSet>;
    using RowColValueSet = std::map<QVariant,ColValueSet>;

    CQCharts*      charts_ { nullptr };
    CQChartsColumn groupColumn_;        // grouping column
    int            hierRow_ { -1 };
    ValueGroup     valueGroup_;         // map group column value to group number
    GroupValue     groupValue_;         // map group number to group column value
    RowColValueSet rowColValueSet_;     // values per hier row or group
  };

  FlattenVisitor flattenVisitor(charts, flattenData.groupColumn);

  CQChartsModelVisit::exec(charts, model, flattenVisitor);

  int nh = flattenVisitor.numHierColumns();
  int nc = flattenVisitor.numFlatColumns();
  int nr = flattenVisitor.numHierRows();

  //---

  auto *columnTypeMgr = charts->columnTypeMgr();

  auto *dataModel = new CQDataModel(nc + nh, nr);

  auto *filterModel = new CQChartsFilterModel(charts, dataModel);

  filterModel->setObjectName("flattenModel");

  //---

  auto initModelColumn = [&](int c, bool isGroup) {
    bool ok;

    QString name = CQChartsModelUtil::modelHeaderString(model, c, Qt::Horizontal, ok);

    CQChartsModelUtil::setModelHeaderValue(dataModel, c, Qt::Horizontal, name);

    CQChartsColumn column(c);

    CQChartsModelTypeData columnTypeData;

    if (! CQChartsModelUtil::columnValueType(charts, model, column, columnTypeData))
      return;

    const auto *typeData = columnTypeMgr->getType(columnTypeData.type);

    if (typeData) {
      if (isGroup || typeData->isNumeric()) {
        (void) columnTypeMgr->setModelColumnType(dataModel, column, columnTypeData.type,
                                                 columnTypeData.nameValues);
      }
    }

    const auto *headerTypeData = columnTypeMgr->getType(columnTypeData.headerType);

    if (headerTypeData) {
      (void) columnTypeMgr->setModelHeaderType(dataModel, column, columnTypeData.headerType,
                                               columnTypeData.headerNameValues);
    }
  };

  //---

  // set hierarchical column
  if (flattenVisitor.isHierarchical()) {
    initModelColumn(0, /*isGroup*/ true);
  }

  // set other columns and types
  for (int c = 0; c < nc; ++c) {
    bool isGroup = (flattenData.groupColumn.column() == c);

    initModelColumn(c + nh, isGroup);
  }

  //--

  for (int r = 0; r < nr; ++r) {
    if (flattenVisitor.isHierarchical()) {
      QVariant var = flattenVisitor.groupValue(r);

      CQChartsModelUtil::setModelValue(dataModel, r, CQChartsColumn(0), var);
    }

    for (int c = 0; c < nc; ++c) {
      QVariant v;

      bool isGroup = (flattenData.groupColumn.column() == c);

      if (! isGroup) {
        CQChartsColumn column(c);

        auto po = flattenData.columnOp.find(column);

        if (po != flattenData.columnOp.end()) {
          auto flattenOp = (*po).second;

          double rv = 0.0;

          if      (flattenOp == FlattenOp::SUM)
            rv = flattenVisitor.hierSum(r, c);
          else if (flattenOp == FlattenOp::MEAN)
            rv = flattenVisitor.hierMean(r, c);

          v = QVariant(rv);
        }
        else {
          v = flattenVisitor.uniqueValue(r, c);
        }
      }
      else {
        v = flattenVisitor.groupValue(r);
      }

      CQChartsModelUtil::setModelValue(dataModel, r, CQChartsColumn(c + nh), v);
    }
  }

  return filterModel;
}

}

//------

namespace CQChartsModelUtil {

const QStringList &roleNames() {
  return CQModelUtil::roleNames();
};

int nameToRole(const QString &name) {
  return CQModelUtil::nameToRole(name);
}

}
