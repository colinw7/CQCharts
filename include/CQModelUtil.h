#ifndef CQModelUtil_H
#define CQModelUtil_H

#include <CQBaseModelTypes.h>
#include <QAbstractItemModel>

namespace CQModelUtil {

//! is model hierarchical (row has children)
bool isHierarchical(const QAbstractItemModel *model);

//---

//! get model header value for orientation and role
QVariant modelHeaderValue(const QAbstractItemModel *model, int c, Qt::Orientation orientation,
                          int role, bool &ok);

//! get model header value for orientation and role
QVariant modelHeaderValue(const QAbstractItemModel *model, int c, Qt::Orientation orientation,
                          CQBaseModelRole role, bool &ok);

//! get model header value for role
QVariant modelHeaderValue(const QAbstractItemModel *model, int c, int role, bool &ok);

//! get model header value for base model role
QVariant modelHeaderValue(const QAbstractItemModel *model, int c, CQBaseModelRole role, bool &ok);

//! get model header value as string
QString modelHeaderString(const QAbstractItemModel *model, int c, bool &ok);

//---

//! get model value from row, column and parent
QVariant modelValue(const QAbstractItemModel *model, int r, int c,
                    const QModelIndex &parent, bool &ok);

//! get model value from model index and role
QVariant modelValue(const QAbstractItemModel *model, const QModelIndex &ind, int role, bool &ok);

//! get model value from model index (edit role or display role)
QVariant modelValue(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

//! get model real value
double modelReal(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

//! get model integer value
long modelInteger(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

//! get model string value
QString modelString(const QAbstractItemModel *model, const QModelIndex &ind, bool &ok);

//---

//! get column value type (from base model)
bool columnValueType(const QAbstractItemModel *model, int c, CQBaseModelType &type);

//! calculate column value type (can be limited to maximum number of rows)
CQBaseModelType calcColumnType(const QAbstractItemModel *model, int icolumn, int maxRows=-1);

//---

//! get base model (no proxy model)
QAbstractItemModel *getBaseModel(QAbstractItemModel *model);

//---

//! get support base mode role names
const QStringList &roleNames();

//! name to base mode role
int nameToRole(const QString &name);

}

#endif
