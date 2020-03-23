#ifndef CQChartsModelUtil_H
#define CQChartsModelUtil_H

#include <CQChartsModelTypes.h>
#include <CQChartsExprModel.h>
#include <CQChartsColumnType.h>
#include <CQChartsUtil.h>

class CQDataModel;
class CQHierSepModel;
class CQChartsColumn;

class QSortFilterProxyModel;
class QAbstractItemModel;

//---

namespace CQChartsModelUtil {

bool isHierarchical(const QAbstractItemModel *model);

void hierData(CQCharts *charts, const QAbstractItemModel *model, int &nr, int &maxDepth);

int hierRowCount(CQCharts *charts, const QAbstractItemModel *model);

QString parentPath(const QAbstractItemModel *model, const QModelIndex &parent);

CQBaseModelType calcColumnType(const QAbstractItemModel *model, int icolumn);

//------

bool columnValueType(CQCharts *charts, const QAbstractItemModel *model,
                     const CQChartsColumn &column, CQChartsModelTypeData &columnTypeData);

bool formatColumnTypeValue(CQCharts *charts, const QAbstractItemModel *model,
                           const CQChartsColumn &column, const QString &typeStr,
                           double value, QString &str);

bool formatColumnValue(CQCharts *charts, const QAbstractItemModel *model,
                       const CQChartsColumn &column, double value, QString &str);

bool formatColumnTypeValue(CQCharts *charts, const QAbstractItemModel *model,
                           const CQChartsColumn &column, const CQChartsColumnType *columnType,
                           const CQChartsModelTypeData &typeData, double value, QString &str);

QVariant columnDisplayData(CQCharts *charts, const QAbstractItemModel *model,
                           const CQChartsColumn &column, const QVariant &var, bool &converted);

QVariant columnUserData(CQCharts *charts, const QAbstractItemModel *model,
                        const CQChartsColumn &column, const QVariant &var, bool &converted);

QVariant columnHeaderUserData(CQCharts *charts, const QAbstractItemModel *model, int section,
                              const QVariant &var, bool &converted);

//---

bool columnTypeStr(CQCharts *charts, const QAbstractItemModel *model,
                   const CQChartsColumn &column, QString &typeStr);

bool setColumnTypeStrs(CQCharts *charts, QAbstractItemModel *model, const QString &columnTypes);

bool setColumnTypeIndexStr(CQCharts *charts, QAbstractItemModel *model,
                           int i, const QString &columnType);

bool setColumnTypeStr(CQCharts *charts, QAbstractItemModel *model,
                      const CQChartsColumn &column, const QString &typeStr);

bool setColumnTypeStrI(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                       const QString &typeName, const QString &typeStr, const QStringList &strs,
                       QString &errorMsg);

//---

bool setHeaderTypeStrs(CQCharts *charts, QAbstractItemModel *model, const QString &columnTypes);

bool setHeaderTypeIndexStr(CQCharts *charts, QAbstractItemModel *model,
                           int i, const QString &columnType);

bool setHeaderTypeStr(CQCharts *charts, QAbstractItemModel *model,
                      const CQChartsColumn &column, const QString &typeStr);

bool setHeaderTypeStrI(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                       const QString &typeName, const QString &typeStr, const QStringList &strs,
                       QString &errorMsg);

//---

#if 0
void remapColumnTime(const QAbstractItemModel *model, const CQChartsColumn &column,
                     CQChartsColumnType *typeData, const CQChartsNameValues &nameValues);
#endif

}

//------

namespace CQChartsModelUtil {

void processAddExpression(QAbstractItemModel *model, const QString &exprStr);

int processExpression(QAbstractItemModel *model, const QString &exprStr);

int processExpression(QAbstractItemModel *model, CQChartsExprModel::Function function,
                      const CQChartsColumn &column, const QString &expr);

const CQChartsExprModel *getExprModel(const QAbstractItemModel *model);
CQChartsExprModel *getExprModel(QAbstractItemModel *model);

const CQDataModel *getDataModel(const QAbstractItemModel *model);
CQDataModel *getDataModel(QAbstractItemModel *model);

CQHierSepModel *getHierSepModel(QAbstractItemModel *model);

QSortFilterProxyModel *getSortFilterProxyModel(QAbstractItemModel *model);

QAbstractItemModel *getBaseModel(QAbstractItemModel *model);

//bool getProperty(const QAbstractItemModel *model, const QString &name, QVariant &value);
//bool setProperty(QAbstractItemModel *model, const QString &name, const QVariant &value);

QVariant getModelMetaValue(const QAbstractItemModel *model, const QString &name);
bool setModelMetaValue(QAbstractItemModel *model, const QString &name, const QVariant &value);

}

//------

namespace CQChartsModelUtil {

QVariant modelHeaderValueI(const QAbstractItemModel *model, const CQChartsColumn &column,
                           Qt::Orientation orient, int role, bool &ok);

QVariant modelHeaderValue(const QAbstractItemModel *model, int section,
                          Qt::Orientation orient, int role, bool &ok);
QVariant modelHeaderValue(const QAbstractItemModel *model, int section,
                          Qt::Orientation orient, bool &ok);
QVariant modelHeaderValue(const QAbstractItemModel *model, const CQChartsColumn &column,
                          int role, bool &ok);
QVariant modelHeaderValue(const QAbstractItemModel *model, const CQChartsColumn &column,
                          bool &ok);

//--

QString modelHeaderStringI(const QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, int role, bool &ok);

QString modelHeaderString(const QAbstractItemModel *model, int section,
                          Qt::Orientation orient, int role, bool &ok);
QString modelHeaderString(const QAbstractItemModel *model, int section,
                          Qt::Orientation orient, bool &ok);
QString modelHeaderString(const QAbstractItemModel *model, const CQChartsColumn &column,
                          int role, bool &ok);
QString modelHeaderString(const QAbstractItemModel *model, const CQChartsColumn &column,
                          bool &ok);

//--

bool setModelHeaderValueI(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, const QVariant &var, int role);

bool setModelHeaderValue(QAbstractItemModel *model, int section,
                         Qt::Orientation orient, const QVariant &var, int role);
bool setModelHeaderValue(QAbstractItemModel *model, int section,
                         Qt::Orientation orient, const QVariant &var);
bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         const QVariant &var, int role);
bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         const QVariant &var);

//--

bool setModelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                   const QVariant &var, int role);
bool setModelValue(QAbstractItemModel *model, int row, const CQChartsColumn &column,
                   const QVariant &var);

//---

QVariant modelValue(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
QVariant modelValue(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

QVariant modelValue(CQCharts *charts, const QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
QVariant modelValue(CQCharts *charts, const QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

//---

QString modelString(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
QString modelString(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

QString modelString(CQCharts *charts, const QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
QString modelString(CQCharts *charts, const QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

QString modelString(CQCharts *charts, const QAbstractItemModel *model,
                    const CQChartsModelIndex &ind, bool &ok);

#if 0
QString modelHierString(CQCharts *charts, const QAbstractItemModel *model, int row,
                        const CQChartsColumn &column, const QModelIndex &parent,
                        int role, bool &ok);
QString modelHierString(CQCharts *charts, const QAbstractItemModel *model, int row,
                        const CQChartsColumn &column, const QModelIndex &parent, bool &ok);
#endif

//---

double modelReal(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
double modelReal(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

double modelReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                 const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
double modelReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                 const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

#if 0
double modelHierReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                     const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
double modelHierReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                     const CQChartsColumn &column, const QModelIndex &parent, bool &ok);
#endif

//---

long modelInteger(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
long modelInteger(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

long modelInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                  const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
long modelInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                  const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

#if 0
long modelHierInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                      const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
long modelHierInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                      const CQChartsColumn &column, const QModelIndex &parent, bool &ok);
#endif

//---

CQChartsColor variantToColor(const QVariant &var);

#if 0
CQChartsColor modelColor(const QAbstractItemModel *model, const QModelIndex &ind,
                         int role, bool &ok);
CQChartsColor modelColor(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

CQChartsColor modelColor(CQCharts *charts, const QAbstractItemModel *model, int row,
                         const CQChartsColumn &column, const QModelIndex &parent,
                         int role, bool &ok);
CQChartsColor modelColor(CQCharts *charts, const QAbstractItemModel *model, int row,
                         const CQChartsColumn &column, const QModelIndex &parent, bool &ok);
#endif

}

//------

namespace CQChartsModelUtil {

#if 0
bool isValidModelColumn(const QAbstractItemModel *model, int column);
#endif

int modelColumnNameToInd(const QAbstractItemModel *model, const QString &name);

bool stringToColumn(const QAbstractItemModel *model, const QString &str, CQChartsColumn &column);

bool stringToColumns(const QAbstractItemModel *model, const QString &str,
                     std::vector<CQChartsColumn> &columns);

bool stringToModelInd(const QAbstractItemModel *model, const QString &str,
                      CQChartsModelIndex &ind);

}

//------

namespace CQChartsModelUtil {

bool decodeModelFilterStr(const QAbstractItemModel *model, const QString &filter,
                          QString &filter1, int &column);

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

enum class FlattenOp {
  NONE,
  SUM,
  MEAN
};

struct FlattenData {
  using ColumnOp = std::map<CQChartsColumn, FlattenOp>;

  CQChartsColumn groupColumn;
  FlattenOp      defOp { FlattenOp::NONE };
  ColumnOp       columnOp;
};

CQChartsFilterModel *flattenModel(CQCharts *charts, QAbstractItemModel *model,
                                  const FlattenData &flattenData);

}

//------

namespace CQChartsModelUtil {

const QStringList &roleNames();

int nameToRole(const QString &name);

}

#endif
