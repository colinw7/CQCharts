#include <CQPropertyViewModel.h>
#include <CQPropertyViewItem.h>
#include <cassert>

CQPropertyViewModel::
CQPropertyViewModel() :
 QAbstractItemModel()
{
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

  if (column <0 || column >= 2)
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

  CQPropertyViewItem *parentItem = hierItem(pathParts, /*create*/true);

  CQPropertyViewItem *item = new CQPropertyViewItem(parentItem, object, name);

  connect(item, SIGNAL(valueChanged(QObject *, const QString &)),
          this, SIGNAL(valueChanged(QObject *, const QString &)));

  parentItem->addChild(item);

  if (alias != "")
    item->setAlias(alias);

  endResetModel();

  return item;
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
hierItem(const QStringList &pathParts, bool create)
{
  return hierItem(root(), pathParts, create);
}

CQPropertyViewItem *
CQPropertyViewModel::
hierItem(CQPropertyViewItem *parentItem, const QStringList &pathParts, bool create)
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

  CQPropertyViewItem *item = new CQPropertyViewItem(parentItem, nullptr, path);

  parentItem->addChild(item);

  return hierItem(item, pathParts.mid(1), create);
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

void
CQPropertyViewModel::
refresh()
{
  int nr = rowCount(QModelIndex());

  beginInsertRows(QModelIndex(), 0, nr);
  endInsertRows  ();
}
