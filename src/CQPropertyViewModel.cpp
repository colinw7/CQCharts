#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <cassert>

CQPropertyViewModel::
CQPropertyViewModel() :
 QAbstractItemModel()
{
}

CQPropertyViewModel::
~CQPropertyViewModel()
{
  clear();
}

int
CQPropertyViewModel::
columnCount(const QModelIndex &) const
{
  return 2;
}

int
CQPropertyViewModel::
rowCount(const QModelIndex &parent) const
{
  CQPropertyViewItem *parentItem = this->item(parent);

  if (! parentItem)
    return 0;

  return parentItem->children().size();
}

QVariant
CQPropertyViewModel::
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
CQPropertyViewModel::
data(const QModelIndex &index, int role) const
{
  CQPropertyViewItem *item = this->item(index);

  if (! item)
    return QVariant();

  if      (role == Qt::DisplayRole || role == Qt::EditRole) {
    if      (index.column() == 0)
      return item->name();
    else if (index.column() == 1)
      return item->data();
    else
      return QVariant();
  }
  else if (role == Qt::ToolTipRole) {
    if      (index.column() == 0)
      return item->path(".", /*alias*/true);
    else if (index.column() == 1)
      return item->tip();
  }

  return QVariant();
}

bool
CQPropertyViewModel::
setData(const QModelIndex &index, const QVariant &value, int role)
{
  CQPropertyViewItem *item = this->item(index);

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
CQPropertyViewModel::
index(int row, int column, const QModelIndex &parent) const
{
  CQPropertyViewItem *parentItem = this->item(parent);

  if (! parentItem)
    return QModelIndex();

  if (row < 0 || row >= parentItem->numChildren())
    return QModelIndex();

  if (column < 0 || column >= 2)
    return QModelIndex();

  CQPropertyViewItem *childItem = parentItem->child(row);

  QModelIndex ind = createIndex(row, column, childItem);

  assert(this->item(ind) == childItem);

  return ind;
}

QModelIndex
CQPropertyViewModel::
parent(const QModelIndex &index) const
{
  CQPropertyViewItem *item = this->item(index);

  if (! item)
    return QModelIndex();

  CQPropertyViewItem *parent = item->parent();

  if (! parent || parent == root())
    return QModelIndex();

  CQPropertyViewItem *parentParent = parent->parent();

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
CQPropertyViewModel::
flags(const QModelIndex &index) const
{
  CQPropertyViewItem *item = this->item(index);

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;

  if (item->isEditable())
    flags |= Qt::ItemIsEditable;

  return flags;
}

void
CQPropertyViewModel::
clear()
{
  delete root_;

  root_ = nullptr;
}

CQPropertyViewItem *
CQPropertyViewModel::
addProperty(const QString &path, QObject *object, const QString &name, const QString &alias)
{
  beginResetModel();

  QStringList pathParts = path.split('/', QString::SkipEmptyParts);

  CQPropertyViewItem *parentItem = hierItem(pathParts, /*create*/true, /*alias*/false);

  CQPropertyViewItem *item = new CQPropertyViewItem(parentItem, object, name);

  connect(item, SIGNAL(valueChanged(QObject *, const QString &)),
          this, SIGNAL(valueChanged(QObject *, const QString &)));

  parentItem->addChild(item);

  if (alias != "")
    item->setAlias(alias);

  endResetModel();

  return item;
}

bool
CQPropertyViewModel::
setProperty(QObject *object, const QString &path, const QVariant &value)
{
  CQPropertyViewItem *item = propertyItem(object, path, '.', /*create*/false, /*alias*/true);

  if (! item)
    return false;

  bool rc = item->setData(value);

  if (rc) {
    QModelIndex ind = indexFromItem(item, 1);

    emit dataChanged(ind, ind);
  }

  return rc;
}

bool
CQPropertyViewModel::
getProperty(QObject *object, const QString &path, QVariant &value)
{
  CQPropertyViewItem *item = propertyItem(object, path, '.', /*create*/false, /*alias*/true);

  if (! item)
    return false;

  value = item->data();

  return true;
}

void
CQPropertyViewModel::
removeProperties(const QString &path, QObject *)
{
  beginResetModel();

  QStringList pathParts = path.split('/', QString::SkipEmptyParts);

  CQPropertyViewItem *item = hierItem(pathParts, /*create*/false, /*alias*/false);

  if (item && item->parent())
    item->parent()->removeChild(item);

  endResetModel();
}

void
CQPropertyViewModel::
objectNames(QObject *object, QStringList &names) const
{
  CQPropertyViewItem *item = objectItem(object);
  if (! item) return;

  itemNames(item, object, item, names);
}

void
CQPropertyViewModel::
itemNames(CQPropertyViewItem *rootItem, QObject *object,
          CQPropertyViewItem *item, QStringList &names) const
{
  if (item->object() && item->object() != object)
    return;

  if (item->numChildren() > 0) {
    for (int i = 0; i < item->numChildren(); ++i) {
      CQPropertyViewItem *item1 = item->child(i);

      itemNames(rootItem, object, item1, names);
    }
  }
  else {
    QString name = item->path(".", /*alias*/true, rootItem);

    names.push_back(name);
  }
}

CQPropertyViewItem *
CQPropertyViewModel::
item(const QModelIndex &index) const
{
  if (! index.isValid())
    return root();

  bool ok;

  CQPropertyViewItem *item = this->item(index, ok);

  assert(ok);

  return item;
}

CQPropertyViewItem *
CQPropertyViewModel::
item(const QModelIndex &index, bool &ok) const
{
  ok = true;

  CQPropertyViewItem *item = static_cast<CQPropertyViewItem *>(index.internalPointer());

  if (item)
    ok = item->isValid();

  return item;
}

CQPropertyViewItem *
CQPropertyViewModel::
propertyItem(QObject *object, const QString &path)
{
  return propertyItem(object, path, '.', /*create*/false, /*alias*/true);
}

CQPropertyViewItem *
CQPropertyViewModel::
propertyItem(QObject *object, const QString &path, QChar splitChar, bool create, bool alias)
{
  CQPropertyViewItem *item = objectItem(object);

  if (! item)
    return nullptr;

  QStringList strs = path.split(splitChar);

  return hierItem(item, strs, create, alias);
}

CQPropertyViewItem *
CQPropertyViewModel::
hierItem(const QStringList &pathParts, bool create, bool alias)
{
  return hierItem(root(), pathParts, create, alias);
}

CQPropertyViewItem *
CQPropertyViewModel::
hierItem(CQPropertyViewItem *parentItem, const QStringList &pathParts, bool create, bool alias)
{
  if (pathParts.empty())
    return parentItem;

  const QString &path = pathParts[0];

  if (path.length() == 0)
    return nullptr;

  for (const auto &child : parentItem->children()) {
    if (! alias) {
      if (! child->object() && child->name() == path) {
        if (pathParts.size() == 1)
          return child;

        return hierItem(child, pathParts.mid(1), create, alias);
      }
    }
    else {
      if (child->name() == path || child->alias() == path) {
        if (pathParts.size() == 1)
          return child;

        return hierItem(child, pathParts.mid(1), create, alias);
      }
    }
  }

  if (! create)
    return nullptr;

  CQPropertyViewItem *item = new CQPropertyViewItem(parentItem, nullptr, path);

  parentItem->addChild(item);

  return hierItem(item, pathParts.mid(1), create, alias);
}

CQPropertyViewItem *
CQPropertyViewModel::
root() const
{
  if (! root_) {
    CQPropertyViewModel *th = const_cast<CQPropertyViewModel *>(this);

    th->root_ = new CQPropertyViewItem(nullptr, nullptr, "");
  }

  return root_;
}

CQPropertyViewItem *
CQPropertyViewModel::
objectItem(const QObject *obj) const
{
  CQPropertyViewItem *root = this->root();

  return objectItem(root, obj);
}

CQPropertyViewItem *
CQPropertyViewModel::
objectItem(CQPropertyViewItem *parent, const QObject *obj) const
{
  for (int i = 0; i < parent->numChildren(); ++i) {
    CQPropertyViewItem *item = parent->child(i);

    if (item->object() == obj)
      return parent;
  }

  for (int i = 0; i < parent->numChildren(); ++i) {
    CQPropertyViewItem *item = parent->child(i);

    CQPropertyViewItem *item1 = objectItem(item, obj);

    if (item1)
      return item1;
  }

  return nullptr;
}

QModelIndex
CQPropertyViewModel::
indexFromItem(CQPropertyViewItem *item, int column) const
{
  CQPropertyViewItem *root = this->root();

  if (item == root)
    return QModelIndex();

  CQPropertyViewItem *parentItem = item->parent();

  for (int i = 0; parentItem->numChildren(); ++i) {
    if (parentItem->child(i) == item) {
      QModelIndex parentInd = indexFromItem(parentItem, 0);

      QModelIndex ind = index(i, column, parentInd);

      return ind;
    }
  }

  return QModelIndex();
}

void
CQPropertyViewModel::
refresh()
{
  int nr = rowCount(QModelIndex());

  beginInsertRows(QModelIndex(), 0, nr);
  endInsertRows  ();
}
