#ifndef CQChartsModelUtil_H
#define CQChartsModelUtil_H

#include <CQChartsModelTypes.h>
#include <CQChartsExprModel.h>
#include <CQChartsColumnType.h>
#include <CQChartsUtil.h>
#ifdef CQCHARTS_EXCEL
#include <CQExcelModel.h>
#endif

class CQDataModel;
class CQHierSepModel;
class CQChartsColumn;
//class CQChartsModelIndex;

class QSortFilterProxyModel;
class QAbstractItemModel;

//---

namespace CQChartsModelUtil {

using Column        = CQChartsColumn;
using Columns       = CQChartsColumns;
using ColumnType    = CQChartsColumnType;
using ModelTypeData = CQChartsModelTypeData;
using ModelType     = CQBaseModelType;
using NameValues    = CQChartsNameValues;

//---

bool isHierarchical(const QAbstractItemModel *model);

void hierData(CQCharts *charts, const QAbstractItemModel *model, int &nr, int &maxDepth);

int hierRowCount(CQCharts *charts, const QAbstractItemModel *model);

QString parentPath(const QAbstractItemModel *model, const QModelIndex &parent);

ModelType calcColumnType(const QAbstractItemModel *model, int icolumn, int maxRows=-1);

//------

bool columnValueType(CQCharts *charts, const QAbstractItemModel *model,
                     const Column &column, ModelTypeData &columnTypeData,
                     bool init=false);

bool formatColumnTypeValue(CQCharts *charts, const QAbstractItemModel *model,
                           const Column &column, const QString &typeStr,
                           const QVariant &value, QString &str);

bool formatColumnValue(CQCharts *charts, const QAbstractItemModel *model,
                       const Column &column, const QVariant &value, QString &str);

bool formatColumnTypeValue(CQCharts *charts, const QAbstractItemModel *model,
                           const Column &column, const ColumnType *columnType,
                           const ModelTypeData &typeData, const QVariant &value, QString &str);

QVariant columnDisplayData(CQCharts *charts, const QAbstractItemModel *model,
                           const Column &column, const QVariant &var, bool &converted);

QVariant columnUserData(CQCharts *charts, const QAbstractItemModel *model,
                        const Column &column, const QVariant &var, bool &converted);

QVariant columnHeaderUserData(CQCharts *charts, const QAbstractItemModel *model, int section,
                              const QVariant &var, bool &converted);

//---

bool columnTypeStr(CQCharts *charts, const QAbstractItemModel *model,
                   const Column &column, QString &typeStr);

bool setColumnTypeStrs(CQCharts *charts, QAbstractItemModel *model, const QString &columnTypes);

bool setColumnTypeIndexStr(CQCharts *charts, QAbstractItemModel *model,
                           int i, const QString &columnType);

bool setColumnTypeStr(CQCharts *charts, QAbstractItemModel *model,
                      const Column &column, const QString &typeStr);

// private
bool setColumnTypeStrI(CQCharts *charts, QAbstractItemModel *model, const Column &column,
                       const QString &typeName, const QString &typeStr, const QStringList &strs,
                       QString &errorMsg);

bool setColumnType(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                   const ModelType &type, const NameValues &nameValues=NameValues());

//---

bool setHeaderTypeStrs(CQCharts *charts, QAbstractItemModel *model, const QString &columnTypes);

bool setHeaderTypeIndexStr(CQCharts *charts, QAbstractItemModel *model,
                           int i, const QString &columnType);

bool setHeaderTypeStr(CQCharts *charts, QAbstractItemModel *model,
                      const Column &column, const QString &typeStr);

bool setHeaderTypeStrI(CQCharts *charts, QAbstractItemModel *model, const Column &column,
                       const QString &typeName, const QString &typeStr, const QStringList &strs,
                       QString &errorMsg);

//---

#if 0
void remapColumnTime(const QAbstractItemModel *model, const Column &column,
                     ColumnType *typeData, const CQChartsNameValues &nameValues);
#endif

}

//------

namespace CQChartsModelUtil {

using ExprModel = CQChartsExprModel;

#ifdef CQCHARTS_EXCEL
using ExcelModel = CQExcel::Model;
#endif

void processAddExpression(QAbstractItemModel *model, const QString &exprStr);

#if 0
int processExpression(QAbstractItemModel *model, const QString &exprStr);
#endif

int processExpression(QAbstractItemModel *model, ExprModel::Function function,
                      const Column &column, const QString &expr);

bool decodeExpression(QAbstractItemModel *model, const QString &exprStr,
                      CQChartsExprModel::Function &function, Column &column, QString &expr);

#ifdef CQCHARTS_EXCEL
const ExcelModel *getExcelModel(const QAbstractItemModel *model);
ExcelModel *getExcelModel(QAbstractItemModel *model);
#endif

const ExprModel *getExprModel(const QAbstractItemModel *model);
ExprModel *getExprModel(QAbstractItemModel *model);

const CQDataModel *getDataModel(const QAbstractItemModel *model);
CQDataModel *getDataModel(QAbstractItemModel *model);

CQHierSepModel *getHierSepModel(QAbstractItemModel *model);

QSortFilterProxyModel *getSortFilterProxyModel(QAbstractItemModel *model);

QAbstractItemModel *getBaseModel(QAbstractItemModel *model);

void getProxyModels(QAbstractItemModel *model, std::vector<QAbstractProxyModel *> &proxyModels,
                    QAbstractItemModel* &sourceModel);

//bool getProperty(const QAbstractItemModel *model, const QString &name, QVariant &value);
//bool setProperty(QAbstractItemModel *model, const QString &name, const QVariant &value);

//---

QVariant getModelMetaValue(const QAbstractItemModel *model, const QString &name,
                           const QString &key);

bool setModelMetaValue(QAbstractItemModel *model, const QString &name,
                       const QString &key, const QVariant &value);

QStringList modelMetaNames(QAbstractItemModel *model);

QStringList modelMetaNameKeys(QAbstractItemModel *model, const QString &name);

}

//------

namespace CQChartsModelUtil {

QVariant modelHeaderValueI(const QAbstractItemModel *model, const Column &column,
                           Qt::Orientation orient, int role, bool &ok);

QVariant modelHeaderValue(const QAbstractItemModel *model, int section,
                          Qt::Orientation orient, int role, bool &ok);
QVariant modelHeaderValue(const QAbstractItemModel *model, int section,
                          Qt::Orientation orient, bool &ok);
QVariant modelHeaderValue(const QAbstractItemModel *model, const Column &column,
                          int role, bool &ok);
QVariant modelHeaderValue(const QAbstractItemModel *model, const Column &column,
                          bool &ok);

//--

QString modelHeaderStringI(const QAbstractItemModel *model, const Column &column,
                          Qt::Orientation orient, int role, bool &ok);

QString modelHHeaderString(const QAbstractItemModel *model, const Column &column,
                           int role, bool &ok);
QString modelHHeaderString(const QAbstractItemModel *model, const Column &column,
                           bool &ok);

QString modelHHeaderString(const QAbstractItemModel *model, int section, int role, bool &ok);
QString modelHHeaderString(const QAbstractItemModel *model, int section, bool &ok);
QString modelVHeaderString(const QAbstractItemModel *model, int section, int role, bool &ok);
QString modelVHeaderString(const QAbstractItemModel *model, int section, bool &ok);

//--

bool setModelHeaderValueI(QAbstractItemModel *model, const Column &column,
                          Qt::Orientation orient, const QVariant &var, int role);

bool setModelHeaderValue(QAbstractItemModel *model, int section,
                         Qt::Orientation orient, const QVariant &var, int role);
bool setModelHeaderValue(QAbstractItemModel *model, int section,
                         Qt::Orientation orient, const QVariant &var);
bool setModelHeaderValue(QAbstractItemModel *model, const Column &column,
                         const QVariant &var, int role);
bool setModelHeaderValue(QAbstractItemModel *model, const Column &column,
                         const QVariant &var);

//--

bool setModelValue(QAbstractItemModel *model, int row, const Column &column,
                   const QModelIndex &parent, const QVariant &var, int role);
bool setModelValue(QAbstractItemModel *model, int row, const Column &column,
                   const QModelIndex &parent, const QVariant &var);

//---

QVariant modelValue(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
QVariant modelValue(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

QVariant modelValue(CQCharts *charts, const QAbstractItemModel *model, int row,
                    const Column &column, const QModelIndex &parent, int role, bool &ok);
QVariant modelValue(CQCharts *charts, const QAbstractItemModel *model, int row,
                    const Column &column, const QModelIndex &parent, bool &ok);

//---

QString modelString(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
QString modelString(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

QString modelString(CQCharts *charts, const QAbstractItemModel *model, int row,
                    const Column &column, const QModelIndex &parent, int role, bool &ok);
QString modelString(CQCharts *charts, const QAbstractItemModel *model, int row,
                    const Column &column, const QModelIndex &parent, bool &ok);

#if 0
QString modelString(CQCharts *charts, const QAbstractItemModel *model,
                    const CQChartsModelIndex &ind, bool &ok);
#endif

#if 0
QString modelHierString(CQCharts *charts, const QAbstractItemModel *model, int row,
                        const Column &column, const QModelIndex &parent,
                        int role, bool &ok);
QString modelHierString(CQCharts *charts, const QAbstractItemModel *model, int row,
                        const Column &column, const QModelIndex &parent, bool &ok);
#endif

//---

double modelReal(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
double modelReal(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

double modelReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                 const Column &column, const QModelIndex &parent, int role, bool &ok);
double modelReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                 const Column &column, const QModelIndex &parent, bool &ok);

#if 0
double modelHierReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                     const Column &column, const QModelIndex &parent, int role, bool &ok);
double modelHierReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                     const Column &column, const QModelIndex &parent, bool &ok);
#endif

//---

long modelInteger(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
long modelInteger(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

long modelInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                  const Column &column, const QModelIndex &parent, int role, bool &ok);
long modelInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                  const Column &column, const QModelIndex &parent, bool &ok);

#if 0
long modelHierInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                      const Column &column, const QModelIndex &parent, int role, bool &ok);
long modelHierInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                      const Column &column, const QModelIndex &parent, bool &ok);
#endif

//---

using Color = CQChartsColor;

Color variantToColor(const QVariant &var);

#if 0
Color modelColor(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
Color modelColor(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

Color modelColor(CQCharts *charts, const QAbstractItemModel *model, int row,
                 const Column &column, const QModelIndex &parent, int role, bool &ok);
Color modelColor(CQCharts *charts, const QAbstractItemModel *model, int row,
                 const Column &column, const QModelIndex &parent, bool &ok);
#endif

}

//------

namespace CQChartsModelUtil {

void setContext(const QAbstractItemModel *model);

bool stringToColumnProc(const QString &str, CQChartsColumn &column);

#if 0
bool isValidModelColumn(const QAbstractItemModel *model, int column);
#endif

int modelColumnNameToInd(const QAbstractItemModel *model, const QString &name);

bool stringToColumn(const QAbstractItemModel *model, const QString &str, Column &column);

bool stringToColumns(const QAbstractItemModel *model, const QString &str,
                     std::vector<Column> &columns);

QString columnsToString(const QAbstractItemModel *model, const Columns &columns, bool &ok);
QString columnToString(const QAbstractItemModel *model, const Column &column, bool &ok);

#if 0
bool stringToModelInd(const QAbstractItemModel *model, const QString &str,
                      CQChartsModelIndex &ind);
#endif

bool stringToModelInd(const QAbstractItemModel *model, const QString &str,
                      int &row, Column &column, std::vector<int> &prows);

}

//------

namespace CQChartsModelUtil {

//! filter column data
struct FilterColumn {
  QString filter;
  int     column { -1 };

  FilterColumn(const QString &filter, int column) :
   filter(filter), column(column) {
  }
};

using FilterColumns = std::vector<FilterColumn>;

bool decodeModelFilterStr(const QAbstractItemModel *model, const QString &filter,
                          QString &filter1, int &column);
bool decodeModelFilterStrs(const QAbstractItemModel *model, const QString &filter,
                           FilterColumns &filterColumns);

}

//------

namespace CQChartsModelUtil {

QString replaceModelExprVars(const QString &expr, const QAbstractItemModel *model,
                             const QModelIndex &ind, int nr, int nc);

}

//------

namespace CQChartsModelUtil {

bool exportModel(const QAbstractItemModel *model, CQBaseModelDataType type,
                 bool hheader=true, bool vheader=false,
                 std::ostream &os=std::cout);

}

//------

class CQChartsFilterModel;

namespace CQChartsModelUtil {

using FilterModel = CQChartsFilterModel;

enum class FlattenOp {
  NONE,
  SUM,
  MEAN,
  COUNT,
  UNIQUE,
  MAX,
  MEDIAN,
  MIN,
  RANGE
};

//! flattend data
struct FlattenData {
  using Column      = CQChartsColumn;
  using ColumnOpMap = std::map<Column, FlattenOp>;
  using ColumnOp    = std::pair<Column, FlattenOp>;
  using ColumnOps   = std::vector<ColumnOp>;

  Column      groupColumn;
  FlattenOp   defOp      { FlattenOp::NONE };
  ColumnOpMap columnOpMap;
  ColumnOps   columnOps;
};

FilterModel *flattenModel(CQCharts *charts, QAbstractItemModel *model,
                          const FlattenData &flattenData);

inline FlattenOp flattenStringToOp(const QString &str) {
  if      (str == "sum"   ) return FlattenOp::SUM;
  else if (str == "mean"  ) return FlattenOp::MEAN;
  else if (str == "avg"   ) return FlattenOp::MEAN;
  else if (str == "count" ) return FlattenOp::COUNT;
  else if (str == "unique") return FlattenOp::UNIQUE;
  else if (str == "max"   ) return FlattenOp::MAX;
  else if (str == "median") return FlattenOp::MEDIAN;
  else if (str == "min"   ) return FlattenOp::MIN;
  else if (str == "range" ) return FlattenOp::RANGE;
  else                      return FlattenOp::NONE;
}

}

//------

namespace CQChartsModelUtil {

enum class MatchType {
  EXACT,
  EXACT_SINGLE,
  GLOB,
  REGEXP
};

bool findRows(QAbstractItemModel *model, const Column &column,
              const QString &match, MatchType matchType, std::vector<int> &rows);

}

//------

namespace CQChartsModelUtil {

const QStringList &roleNames(QAbstractItemModel *model);

int nameToRole(QAbstractItemModel *model, const QString &name);

}

#endif
