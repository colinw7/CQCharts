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

bool isHierarchical(const QAbstractItemModel *model);

int hierRowCount(const QAbstractItemModel *model);

QString parentPath(const QAbstractItemModel *model, const QModelIndex &parent);

CQBaseModelType calcColumnType(const QAbstractItemModel *model, int icolumn);

//------

bool columnValueType(CQCharts *charts, const QAbstractItemModel *model,
                     const CQChartsColumn &column, CQBaseModelType &columnType,
                     CQBaseModelType &columnBaseType, CQChartsNameValues &nameValues);

bool formatColumnTypeValue(CQCharts *charts, const QAbstractItemModel *model,
                           const CQChartsColumn &column, const QString &typeStr,
                           double value, QString &str);

bool formatColumnValue(CQCharts *charts, const QAbstractItemModel *model,
                       const CQChartsColumn &column, double value, QString &str);

bool formatColumnTypeValue(CQCharts *charts, const QAbstractItemModel *model,
                           const CQChartsColumn &column, const CQChartsColumnType *typeData,
                           const CQChartsNameValues &nameValues, double value, QString &str);

QVariant columnDisplayData(CQCharts *charts, const QAbstractItemModel *model,
                           const CQChartsColumn &column, const QVariant &var, bool &converted);

QVariant columnUserData(CQCharts *charts, const QAbstractItemModel *model,
                        const CQChartsColumn &column, const QVariant &var, bool &converted);

bool columnTypeStr(CQCharts *charts, const QAbstractItemModel *model,
                   const CQChartsColumn &column, QString &typeStr);

bool setColumnTypeStrs(CQCharts *charts, QAbstractItemModel *model, const QString &columnTypes);

bool setColumnTypeIndexStr(CQCharts *charts, QAbstractItemModel *model,
                           int i, const QString &columnType);

bool setColumnTypeI(CQCharts *charts, QAbstractItemModel *model, const CQChartsColumn &column,
                    const QString &typeName, const QString &typeStr, const QStringList &strs,
                    QString &errorMsg);

bool setColumnTypeStr(CQCharts *charts, QAbstractItemModel *model,
                      const CQChartsColumn &column, const QString &typeStr);

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

QAbstractItemModel *getBaseModel(QAbstractItemModel *model);

using ModelNames = std::map<QAbstractItemModel*,QStringList>;

void getPropertyNames(const QAbstractItemModel *model, ModelNames &names);
void getPropertyNames(const QAbstractItemModel *model, QStringList &names);

bool getProperty(const QAbstractItemModel *model, const QString &name, QVariant &value);
bool setProperty(QAbstractItemModel *model, const QString &name, const QVariant &value);

QVariant getModelMetaValue(const QAbstractItemModel *model, const QString &name);
bool setModelMetaValue(QAbstractItemModel *model, const QString &name, const QVariant &value);

}

//------

namespace CQChartsModelUtil {

QVariant modelHeaderValue(const QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orientation, int role, bool &ok);
QVariant modelHeaderValue(const QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orientation, bool &ok);

QVariant modelHeaderValue(const QAbstractItemModel *model, const CQChartsColumn &column,
                          int role, bool &ok);
QVariant modelHeaderValue(const QAbstractItemModel *model, const CQChartsColumn &column, bool &ok);

//--

QString modelHeaderString(const QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, int role, bool &ok);
QString modelHeaderString(const QAbstractItemModel *model, const CQChartsColumn &column,
                          Qt::Orientation orient, bool &ok);

QString modelHeaderString(const QAbstractItemModel *model, const CQChartsColumn &column,
                          int role, bool &ok);
QString modelHeaderString(const QAbstractItemModel *model, const CQChartsColumn &column, bool &ok);

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

QString modelHierString(CQCharts *charts, const QAbstractItemModel *model, int row,
                        const CQChartsColumn &column, const QModelIndex &parent,
                        int role, bool &ok);
QString modelHierString(CQCharts *charts, const QAbstractItemModel *model, int row,
                        const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

//---

double modelReal(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
double modelReal(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

double modelReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                 const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
double modelReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                 const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

double modelHierReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                     const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
double modelHierReal(CQCharts *charts, const QAbstractItemModel *model, int row,
                     const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

//---

long modelInteger(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);
long modelInteger(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

long modelInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                  const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
long modelInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                  const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

long modelHierInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                      const CQChartsColumn &column, const QModelIndex &parent, int role, bool &ok);
long modelHierInteger(CQCharts *charts, const QAbstractItemModel *model, int row,
                      const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

//---

CQChartsColor variantToColor(const QVariant &var);

CQChartsColor modelColor(const QAbstractItemModel *model, const QModelIndex &ind,
                         int role, bool &ok);
CQChartsColor modelColor(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

CQChartsColor modelColor(CQCharts *charts, const QAbstractItemModel *model, int row,
                         const CQChartsColumn &column, const QModelIndex &parent,
                         int role, bool &ok);
CQChartsColor modelColor(CQCharts *charts, const QAbstractItemModel *model, int row,
                         const CQChartsColumn &column, const QModelIndex &parent, bool &ok);

}

//------

namespace CQChartsModelUtil {

bool isValidModelColumn(const QAbstractItemModel *model, int column);

int modelColumnNameToInd(const QAbstractItemModel *model, const QString &name);

bool stringToColumn(const QAbstractItemModel *model, const QString &str, CQChartsColumn &column);

bool stringToColumns(const QAbstractItemModel *model, const QString &str,
                     std::vector<CQChartsColumn> &columns);

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

void exportModel(const QAbstractItemModel *model, CQBaseModelDataType type,
                 bool hheader=true, bool vheader=false,
                 std::ostream &os=std::cout);

}

//------

namespace CQChartsModelUtil {

const QStringList &roleNames();

int nameToRole(const QString &name);

}

#endif
