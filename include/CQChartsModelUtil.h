#ifndef CQChartsModelUtil_H
#define CQChartsModelUtil_H

#include <CQChartsExprModel.h>
#include <CQChartsColumnType.h>
#include <CQChartsUtil.h>
#include <CQBaseModelTypes.h>

class CQDataModel;
class CQChartsColumn;
class QAbstractItemModel;

//---

namespace CQChartsModelUtil {

bool isHierarchical(QAbstractItemModel *model);

int hierRowCount(QAbstractItemModel *model);

QString parentPath(QAbstractItemModel *model, const QModelIndex &parent);

CQBaseModelType calcColumnType(QAbstractItemModel *model, int icolumn);

//------

bool columnValueType(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                     CQBaseModelType &columnType, CQBaseModelType &columnBaseType,
                     CQChartsNameValues &nameValues);

bool formatColumnTypeValue(CQCharts *charts, QAbstractItemModel *model,
                           const CQChartsColumn &column, const QString &typeStr,
                           double value, QString &str);

bool formatColumnValue(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                       double value, QString &str);

bool formatColumnTypeValue(CQCharts *charts, QAbstractItemModel *model,
                           const CQChartsColumn &column, CQChartsColumnType *typeData,
                           const CQChartsNameValues &nameValues, double value, QString &str);

QVariant columnDisplayData(CQCharts *charts, QAbstractItemModel *model,
                           const CQChartsColumn &column, const QVariant &var, bool &converted);

QVariant columnUserData(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                        const QVariant &var, bool &converted);

bool columnTypeStr(CQCharts *charts, QAbstractItemModel *model,
                   const CQChartsColumn &column, QString &typeStr);

bool setColumnTypeStrs(CQCharts *charts, QAbstractItemModel *model, const QString &columnTypes);

bool setColumnTypeStr(CQCharts *charts, QAbstractItemModel *model,
                      const CQChartsColumn &column, const QString &typeStr);

#if 0
void remapColumnTime(QAbstractItemModel *model, const CQChartsColumn &column,
                     CQChartsColumnType *typeData, const CQChartsNameValues &nameValues);
#endif

}

//------

namespace CQChartsModelUtil {

void processAddExpression(QAbstractItemModel *model, const QString &exprStr);

int processExpression(QAbstractItemModel *model, const QString &exprStr);

int processExpression(QAbstractItemModel *model, CQChartsExprModel::Function function,
                      const CQChartsColumn &column, const QString &expr);

CQChartsExprModel *getExprModel(QAbstractItemModel *model);

CQDataModel *getDataModel(QAbstractItemModel *model);

QVariant modelMetaValue(QAbstractItemModel *model, const QString &name);

}

//------

namespace CQChartsModelUtil {

QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orientation, int role, bool &ok);
QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orientation, bool &ok);

QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                          int role, bool &ok);
QVariant modelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column, bool &ok);

//--

QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, int role, bool &ok);
QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, bool &ok);

QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column,
                          int role, bool &ok);
QString modelHeaderString(QAbstractItemModel *model, const CQChartsColumn &column, bool &ok);

//--

bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         Qt::Orientation orient, const QVariant &var, int role);
bool setModelHeaderValue(QAbstractItemModel *model, const CQChartsColumn &column,
                         Qt::Orientation orientation, const QVariant &var);

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

QVariant modelValue(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
QVariant modelValue(QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

QVariant modelValue(CQCharts *charts, QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
QVariant modelValue(CQCharts *charts, QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

//---

QString modelString(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
QString modelString(QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

QString modelString(CQCharts *charts, QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
QString modelString(CQCharts *charts, QAbstractItemModel *model, int row,
                    const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

QString modelHierString(CQCharts *charts, QAbstractItemModel *model, int row,
                        const CQChartsColumn &column, const QModelIndex &parent,
                        int role, bool &ok);
QString modelHierString(CQCharts *charts, QAbstractItemModel *model, int row,
                        const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

//---

double modelReal(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
double modelReal(QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

double modelReal(CQCharts *charts, QAbstractItemModel *model, int row,
                 const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
double modelReal(CQCharts *charts, QAbstractItemModel *model, int row,
                 const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

double modelHierReal(CQCharts *charts, QAbstractItemModel *model, int row,
                     const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
double modelHierReal(CQCharts *charts, QAbstractItemModel *model, int row,
                     const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

//---

long modelInteger(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
long modelInteger(QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

long modelInteger(CQCharts *charts, QAbstractItemModel *model, int row,
                  const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
long modelInteger(CQCharts *charts, QAbstractItemModel *model, int row,
                  const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

long modelHierInteger(CQCharts *charts, QAbstractItemModel *model, int row,
                      const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
long modelHierInteger(CQCharts *charts, QAbstractItemModel *model, int row,
                      const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

//---

CQChartsColor variantToColor(const QVariant &var);

CQChartsColor modelColor(QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
CQChartsColor modelColor(QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

CQChartsColor modelColor(CQCharts *charts, QAbstractItemModel *model, int row,
                         const CQChartsColumn &column, const QModelIndex &parent,
                         int role, bool &ok);
CQChartsColor modelColor(CQCharts *charts, QAbstractItemModel *model, int row,
                         const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

}

//------

namespace CQChartsModelUtil {

bool isValidModelColumn(QAbstractItemModel *model, int column);

int modelColumnNameToInd(const QAbstractItemModel *model, const QString &name);

bool stringToColumn(const QAbstractItemModel *model, const QString &str, CQChartsColumn &column);

bool stringToColumns(const QAbstractItemModel *model, const QString &str,
                     std::vector<CQChartsColumn> &columns);

}

//------

namespace CQChartsModelUtil {

bool decodeModelFilterStr(QAbstractItemModel *model, const QString &filter,
                          QString &filter1, int &column);

}

//------

namespace CQChartsModelUtil {

QString replaceModelExprVars(const QString &expr, QAbstractItemModel *model,
                             const QModelIndex &ind, int nr, int nc);

}

//------

namespace CQChartsModelUtil {

void exportModel(QAbstractItemModel *model, CQBaseModelDataType type,
                 bool hheader=true, bool vheader=false,
                 std::ostream &os=std::cout);

}

#endif
