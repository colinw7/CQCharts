#include <CQChartsModelUtil.h>
#include <CQChartsModelFilter.h>
#include <CQChartsModelVisitor.h>
#include <CQChartsColumnType.h>
#include <CQChartsVariant.h>
#include <CQChartsEval.h>
#include <CQCharts.h>

#include <CQCsvModel.h>
#include <CQTsvModel.h>
#include <CQGnuDataModel.h>
#include <CQJsonModel.h>
#include <CQDataModel.h>
#include <CQPivotModel.h>
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

#if 0
int hierRowCount(CQCharts *charts, const QAbstractItemModel *model) {
  CQChartsModelVisitor visitor;

  CQChartsModelVisit::exec(charts, model, visitor);

  return visitor.numRows();
}
#endif

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
                CQBaseModelType &columnType, CQBaseModelType &columnBaseType,
                CQChartsNameValues &nameValues) {
  assert(model);

  if (column.type() == CQChartsColumn::Type::DATA ||
      column.type() == CQChartsColumn::Type::DATA_INDEX) {
    // get column number and validate
    int icolumn = column.column();

    if (icolumn < 0 || icolumn >= model->columnCount()) {
      columnType     = CQBaseModelType::NONE;
      columnBaseType = CQBaseModelType::NONE;
      return false;
    }

    //---

    // use defined column type if available
    CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

    if (columnTypeMgr->getModelColumnType(model, icolumn, columnType, columnBaseType, nameValues)) {
      if (column.type() == CQChartsColumn::Type::DATA_INDEX) {
        const CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

        if (typeData) {
          columnType = typeData->indexType(column.index());
        }
      }

      return true;
    }

    //---

    // determine column type from values
    // TODO: cache (in plot ?), max visited values

    columnType     = calcColumnType(charts, model, icolumn);
    columnBaseType = columnType;

    return true;
  }
  else if (column.type() == CQChartsColumn::Type::GROUP) {
    columnType     = CQBaseModelType::INTEGER;
    columnBaseType = CQBaseModelType::INTEGER;
    return true;
  }
  else {
    // TODO: for custom expression should determine expression result type (if possible)
    columnType     = CQBaseModelType::STRING;
    columnBaseType = CQBaseModelType::STRING;
    return true;
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

  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  const CQChartsColumnType *typeData = columnTypeMgr->decodeTypeData(formatStr, nameValues);

  if (! typeData)
    return false;
#else
  CQBaseModelType    columnType;
  CQBaseModelType    columnBaseType;
  CQChartsNameValues nameValues;

  if (! columnValueType(charts, model, column, columnType, columnBaseType, nameValues))
    return false;

  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  const CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

  if (! typeData)
    return false;

  nameValues.setNameValue(typeData->formatName(), formatStr);
#endif

  return formatColumnTypeValue(charts, model, column, typeData, nameValues, value, str);
}

// use column type details to format an internal value (real) to a display value
//  TODO: value should be variant ?
bool
formatColumnValue(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                  double value, QString &str) {
  CQBaseModelType    columnType;
  CQBaseModelType    columnBaseType;
  CQChartsNameValues nameValues;

  if (! columnValueType(charts, model, column, columnType, columnBaseType, nameValues))
    return false;

  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  const CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

  if (! typeData)
    return false;

  return formatColumnTypeValue(charts, model, column, typeData, nameValues, value, str);
}

bool
formatColumnTypeValue(CQCharts *charts, const QAbstractItemModel *model,
                      const CQChartsColumn &column, const CQChartsColumnType *typeData,
                      const CQChartsNameValues &nameValues, double value, QString &str) {
  bool converted;

  QVariant var = typeData->dataName(charts, model, column, value, nameValues, converted);

  if (! var.isValid())
    return false;

  CQChartsVariant::toString(var, str);

  return true;
}

// use column type details to format an internal model value (variant) to a display value
QVariant
columnDisplayData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
                  const QVariant &var, bool &converted) {
  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  // TODO: use columnValueType not CQChartsColumnTypeMgr::getModelColumnType
  QVariant var1 = columnTypeMgr->getDisplayData(model, column, var, converted);

  return var1;
}

// use column type details to format an internal model value (variant) to a editable value
QVariant
columnUserData(CQCharts *charts, const QAbstractItemModel *model, const CQChartsColumn &column,
               const QVariant &var, bool &converted) {
  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  // TODO: use columnValueType not CQChartsColumnTypeMgr::getModelColumnType
  QVariant var1 = columnTypeMgr->getUserData(model, column, var, converted);

  return var1;
}

#if 0
// get type string for column (type name and name values)
bool
columnTypeStr(CQCharts *charts, const QAbstractItemModel *model,
              const CQChartsColumn &column, QString &typeStr) {
  CQBaseModelType    columnType;
  CQBaseModelType    columnBaseType;
  CQChartsNameValues nameValues;

  if (! columnValueType(charts, model, column, columnType, columnBaseType, nameValues))
    return false;

  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  typeStr = columnTypeMgr->encodeTypeData(columnType, nameValues);

  return true;
}
#endif

// get type string for column (type name and name values)
bool
columnTypeStr(CQCharts *charts, const QAbstractItemModel *model,
              const CQChartsColumn &column, QString &typeStr) {
  CQBaseModelType    columnType;
  CQBaseModelType    columnBaseType;
  CQChartsNameValues nameValues;

  if (! columnValueType(charts, model, column, columnType, columnBaseType, nameValues))
    return false;

  QStringList strs;

  strs << CQBaseModel::typeName(columnType);

  for (const auto &nv : nameValues.nameValues()) {
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
  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  // decode to type name and name values
  CQChartsNameValues nameValues;

  const CQChartsColumnType *typeData = columnTypeMgr->decodeTypeData(typeStr, nameValues);

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
setColumnTypeI(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
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

  CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

  const CQChartsColumnType *typeData = columnTypeMgr->getType(CQBaseModel::nameType(typeName));

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

  return setColumnTypeI(charts, model, column, typeName, typeStr, strs.mid(1), errorMsg);
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

  return setColumnTypeI(charts, model, column, typeName, typeStr, strs.mid(1), errorMsg);
}

}

//------

namespace CQChartsModelUtil {

void
processAddExpression(QAbstractItemModel *model, const QString &exprStr)
{
  CQChartsExprModel *exprModel = getExprModel(model);

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
  CQChartsExprModel *exprModel = getExprModel(model);

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
  CQChartsExprModel *exprModel = getExprModel(model);

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
  CQChartsExprModel *exprModel = qobject_cast<CQChartsExprModel *>(model);

  if (exprModel)
    return exprModel;

  QSortFilterProxyModel *sortModel = qobject_cast<QSortFilterProxyModel *>(model);

  if (! sortModel)
    return nullptr;

  QAbstractItemModel *sourceModel = sortModel->sourceModel();
//std::cerr << (sourceModel ? sourceModel->objectName().toStdString() : "null") << "\n";

  exprModel = qobject_cast<CQChartsExprModel *>(sourceModel);

  if (exprModel)
    return exprModel;

  QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel *>(sourceModel);

  if (! proxyModel)
    return nullptr;

  sourceModel = proxyModel->sourceModel();
//std::cerr << (sourceModel ? sourceModel->objectName().toStdString() : "null") << "\n";

  return getExprModel(sourceModel);

#if 0
  exprModel = qobject_cast<CQChartsExprModel *>(sourceModel);

  if (exprModel)
    return exprModel;

  sortModel = qobject_cast<QSortFilterProxyModel *>(sourceModel);

  if (! sortModel)
    return nullptr;

  sourceModel = sortModel->sourceModel();
//std::cerr << (sourceModel ? sourceModel->objectName().toStdString() : "null") << "\n";

  exprModel = qobject_cast<CQChartsExprModel *>(sourceModel);

  if (exprModel)
    return exprModel;

  return nullptr;
#endif
}

const CQDataModel *
getDataModel(const QAbstractItemModel *model)
{
  return getDataModel(const_cast<QAbstractItemModel *>(model));
}

CQDataModel *
getDataModel(QAbstractItemModel *model)
{
  CQChartsModelFilter *modelFilter = dynamic_cast<CQChartsModelFilter *>(model);
  if (! modelFilter) return nullptr;

  CQDataModel *dataModel = dynamic_cast<CQDataModel *>(modelFilter->baseModel());
  if (! dataModel) return nullptr;

  return dataModel;
}

QAbstractItemModel *
getBaseModel(QAbstractItemModel *model)
{
  return CQModelUtil::getBaseModel(model);
}

//---

void
getPropertyNames(const QAbstractItemModel *model, ModelNames &names)
{
  // TODO: proxy models
  QAbstractItemModel *model1 = const_cast<QAbstractItemModel *>(model);

  QAbstractItemModel *absModel = getBaseModel(model1);

  CQBaseModel *baseModel = qobject_cast<CQBaseModel *>(absModel);
  CQDataModel *dataModel = qobject_cast<CQDataModel *>(absModel);

  CQChartsExprModel *exprModel = getExprModel(absModel);

  CQChartsModelFilter *modelFilter = qobject_cast<CQChartsModelFilter *>(absModel);

  CQPivotModel *pivotModel = qobject_cast<CQPivotModel *>(baseModel);

  CQCsvModel     *csvModel  = qobject_cast<CQCsvModel     *>(absModel);
  CQTsvModel     *tsvModel  = qobject_cast<CQTsvModel     *>(absModel);
  CQGnuDataModel *gnuModel  = qobject_cast<CQGnuDataModel *>(absModel);
  CQJsonModel    *jsonModel = qobject_cast<CQJsonModel    *>(absModel);

  if (baseModel)
    names[baseModel] << "title" << "maxTypeRows";

  if (dataModel)
    names[dataModel] << "readOnly" << "filter";

  if (exprModel)
    names[exprModel] << "debug";

  if (modelFilter)
    names[modelFilter] << "filter" << "type" << "invert";

  if (pivotModel)
    names[pivotModel] << "valueType" << "includeTotals";

  if (csvModel)
    names[csvModel] << "filename" << "commentHeader" << "firstLineHeader" << "firstColumnHeader" <<
                       "separator";

  if (tsvModel)
    names[tsvModel] << "filename" << "commentHeader" << "firstLineHeader" << "firstColumnHeader";

  if (gnuModel)
    names[gnuModel] << "filename" << "commentHeader" << "firstLineHeader" << "firstColumnHeader" <<
                       "commentChars" << "missingStr" << "separator" << "parseStrings" <<
                       "setBlankLines" << "subSetBlankLines" << "keepQuotes";

  if (jsonModel)
    names[jsonModel] << "hierarchical" << "flat";
}

void
getPropertyNames(const QAbstractItemModel *model, QStringList &names)
{
  ModelNames modelNames;

  getPropertyNames(model, modelNames);

  for (const auto &pn : modelNames)
    names << pn.second;
}

bool
getProperty(const QAbstractItemModel *model, const QString &name, QVariant &value)
{
  if (CQUtil::getTclProperty(model, name, value))
    return true;

  const QAbstractItemModel *baseModel = getBaseModel(const_cast<QAbstractItemModel *>(model));

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

  QAbstractItemModel *baseModel = getBaseModel(model);

  if (baseModel && baseModel != model) {
    if (CQUtil::setProperty(baseModel, name, value))
      return true;
  }

  return false;
}

QVariant
getModelMetaValue(const QAbstractItemModel *model, const QString &name)
{
  const CQDataModel *dataModel = getDataModel(model);
  if (! dataModel) return QVariant();

  return dataModel->nameValue(name);
}

bool
setModelMetaValue(QAbstractItemModel *model, const QString &name, const QVariant &value)
{
  CQDataModel *dataModel = getDataModel(model);
  if (! dataModel) return false;

  dataModel->setNameValue(name, value);

  return true;
}

}

//------

namespace CQChartsModelUtil {

QVariant modelHeaderValue(const QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orientation, int role, bool &ok) {
  ok = false;

  if (! column.isValid())
    return QVariant();

  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return QVariant();

  int icolumn = column.column();

  if (icolumn < 0)
    return QVariant();

  return CQModelUtil::modelHeaderValue(model, icolumn, orientation, role, ok);
}

QVariant modelHeaderValue(const QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orientation, bool &ok) {
  return modelHeaderValue(model, column, orientation, column.role(Qt::DisplayRole), ok);
}

QVariant modelHeaderValue(const QAbstractItemModel *model, const CQChartsColumn &column,
                          int role, bool &ok) {
  return modelHeaderValue(model, column, Qt::Horizontal, role, ok);
}

QVariant modelHeaderValue(const QAbstractItemModel *model, const CQChartsColumn &column, bool &ok) {
  return modelHeaderValue(model, column, Qt::Horizontal, column.role(Qt::DisplayRole), ok);
}

//--

QString modelHeaderString(const QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, int role, bool &ok) {
  QVariant var = modelHeaderValue(model, column, orient, role, ok);
  if (! var.isValid()) return "";

  QString str;

  bool rc = CQChartsVariant::toString(var, str);
  assert(rc);

  return str;
}

QString modelHeaderString(const QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, bool &ok) {
  return modelHeaderString(model, column, orient, column.role(Qt::DisplayRole), ok);
}

QString modelHeaderString(const QAbstractItemModel *model, const CQChartsColumn &column,
                          int role, bool &ok) {
  return modelHeaderString(model, column, Qt::Horizontal, role, ok);
}

QString modelHeaderString(const QAbstractItemModel *model, const CQChartsColumn &column, bool &ok) {
  return modelHeaderString(model, column, Qt::Horizontal, column.role(Qt::DisplayRole), ok);
}

//--

bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         Qt::Orientation orient, const QVariant &var, int role) {
  if (column.type() != CQChartsColumn::Type::DATA &&
      column.type() != CQChartsColumn::Type::DATA_INDEX)
    return false;

  return model->setHeaderData(column.column(), orient, var, role);
}

bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         Qt::Orientation orientation, const QVariant &var) {
  return setModelHeaderValue(model, column, orientation, var, column.role(Qt::EditRole));
}

bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         const QVariant &var, int role) {
  return setModelHeaderValue(model, column, Qt::Horizontal, var, role);
}

bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         const QVariant &var) {
  return setModelHeaderValue(model, column, Qt::Horizontal, var, column.role(Qt::EditRole));
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

    CQBaseModelType    columnType;
    CQBaseModelType    columnBaseType;
    CQChartsNameValues nameValues;

    if (! columnValueType(charts, model, column.column(), columnType, columnBaseType, nameValues))
      return var;

    CQChartsColumnTypeMgr *columnTypeMgr = charts->columnTypeMgr();

    const CQChartsColumnType *typeData = columnTypeMgr->getType(columnType);

    return typeData->indexVar(var, column.index());
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
    QVariant var;

    ok = CQChartsEvalInst->evalExpr(row, column.expr(), var);

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

}

//------

namespace CQChartsModelUtil {

bool isValidModelColumn(const QAbstractItemModel *model, int column) {
  return (column >= 0 && column < model->columnCount());
}

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
            columns.push_back(c);
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

}

//------

namespace CQChartsModelUtil {

QString
replaceModelExprVars(const QString &expr, const QAbstractItemModel *model,
                     const QModelIndex &ind, int nr, int nc)
{
  auto quoteStr = [](const QString &str, bool doQuote) -> QString {
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

        int column1 = CQChartsUtil::toInt(str, ok);

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

}

//------

namespace CQChartsModelUtil {

void
exportModel(const QAbstractItemModel *model, CQBaseModelDataType type, bool hheader,
            bool vheader, std::ostream &os) {
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
  else {
    assert(false);
  }
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
