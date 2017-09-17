#include <CQPropertyModel.h>
#include <CQPropertyItem.h>
#include <cassert>

CQPropertyModel::
CQPropertyModel() :
 QAbstractItemModel()
{
}

int
CQPropertyModel::
columnCount(const QModelIndex &) const
{
  return 2;
}

int
CQPropertyModel::
rowCount(const QModelIndex &parent) const
{
  CQPropertyItem *parentItem = this->item(parent);

  if (! parentItem)
    return 0;

  return parentItem->children().size();
}

QVariant
CQPropertyModel::
headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation != Qt::Horizontal)
    return QVariant();

  if (role == Qt::DisplayRole) {
    if      (section == 0)
      return "Name";
    else if (section == 1)
      return "Value";
    else
      return QVariant();
  }

  return QVariant();
}

QVariant
CQPropertyModel::
data(const QModelIndex &index, int role) const
{
  CQPropertyItem *item = this->item(index);

  if (! item)
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    if      (index.column() == 0)
      return item->name();
    else if (index.column() == 1)
      return item->data();
    else
      return QVariant();
  }

  return QVariant();
}

bool
CQPropertyModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  CQPropertyItem *item = this->item(index);

  if (! item)
    return false;

  if (role == Qt::DisplayRole || role == Qt::EditRole) {
    if      (index.column() == 0)
      return false;
    else if (index.column() == 1) {
      item->setData(value);

      emit dataChanged(index, index);

      return true;
    }
    else
      return false;
  }

  return false;
}

QModelIndex
CQPropertyModel::
index(int row, int column, const QModelIndex &parent) const
{
  CQPropertyItem *parentItem = this->item(parent);

  if (! parentItem)
    return QModelIndex();

  if (row < 0 || row >= parentItem->numChildren())
    return QModelIndex();

  if (column <0 || column >= 2)
    return QModelIndex();

  CQPropertyItem *childItem = parentItem->child(row);

  QModelIndex ind = createIndex(row, column, childItem);

  assert(this->item(ind) == childItem);

  return ind;
}

QModelIndex
CQPropertyModel::
parent(const QModelIndex &index) const
{
  CQPropertyItem *item = this->item(index);

  if (! item)
    return QModelIndex();

  CQPropertyItem *parent = item->parent();

  if (! parent || parent == root())
    return QModelIndex();

  CQPropertyItem *parentParent = parent->parent();

  if (! parentParent)
    parentParent = root();

  //---

  int i = 0;

  for (const auto &child : parentParent->children()) {
    if (child == parent) {
      QModelIndex ind = createIndex(i, 0, parent);

      assert(this->item(ind) == parent);

      return ind;
    }

    ++i;
  }

  return QModelIndex();
}

Qt::ItemFlags
CQPropertyModel::
flags(const QModelIndex &index) const
{
  CQPropertyItem *item = this->item(index);

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (item->isEditable())
    flags |= Qt::ItemIsEditable;

  return flags;
}

void
CQPropertyModel::
clear()
{
  delete root_;

  root_ = nullptr;
}

CQPropertyItem *
CQPropertyModel::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  beginResetModel();

  QStringList pathParts = path.split('/', QString::SkipEmptyParts);

  CQPropertyItem *parentItem = hierItem(pathParts, /*create*/true);

  CQPropertyItem *item = new CQPropertyItem(parentItem, object, name);

  connect(item, SIGNAL(valueChanged(QObject *, const QString &)),
          this, SIGNAL(valueChanged(QObject *, const QString &)));

  parentItem->addChild(item);

  if (alias != "")
    item->setAlias(alias);

  endResetModel();

  return item;
}

CQPropertyItem *
CQPropertyModel::
item(const QModelIndex &index) const
{
  if (! index.isValid())
    return root();

  bool ok;

  CQPropertyItem *item = this->item(index, ok);

  assert(ok);

  return item;
}

CQPropertyItem *
CQPropertyModel::
item(const QModelIndex &index, bool &ok) const
{
  ok = true;

  CQPropertyItem *item = static_cast<CQPropertyItem *>(index.internalPointer());

  if (item)
    ok = item->isValid();

  return item;
}

CQPropertyItem *
CQPropertyModel::
hierItem(const QStringList &pathParts, bool create)
{
  return hierItem(root(), pathParts, create);
}

CQPropertyItem *
CQPropertyModel::
hierItem(CQPropertyItem *parentItem, const QStringList &pathParts, bool create)
{
  if (pathParts.empty())
    return parentItem;

  const QString &path = pathParts[0];

  if (path.length() == 0)
    return nullptr;

  for (const auto &child : parentItem->children()) {
    if (! child->object() && child->name() == path) {
      if (pathParts.size() == 1)
        return child;

      return hierItem(child, pathParts.mid(1), create);
    }
  }

  if (! create)
    return nullptr;

  CQPropertyItem *item = new CQPropertyItem(parentItem, nullptr, path);

  parentItem->addChild(item);

  return hierItem(item, pathParts.mid(1), create);
}

CQPropertyItem *
CQPropertyModel::
root() const
{
  if (! root_) {
    CQPropertyModel *th = const_cast<CQPropertyModel *>(this);

    th->root_ = new CQPropertyItem(nullptr, nullptr, "");
  }

  return root_;
}
