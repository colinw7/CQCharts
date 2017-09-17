#include <CQPropertyView.h>
#include <CQPropertyViewFilter.h>
#include <CQPropertyModel.h>
#include <CQPropertyDelegate.h>
#include <CQPropertyItem.h>

#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QMouseEvent>
#include <set>

CQPropertyView::
CQPropertyView(QWidget *parent) :
 QTreeView(parent)
{
  setObjectName("propertyView");

  //--

  filter_ = new CQPropertyViewFilter(this);

  model_ = new CQPropertyModel;

  connect(model_, SIGNAL(valueChanged(QObject *, const QString &)),
          this, SIGNAL(valueChanged(QObject *, const QString &)));

  filter_->setSourceModel(model_);

  setModel(filter_);

  //--

  header()->setStretchLastSection(true);

  header()->setSectionResizeMode(QHeaderView::ResizeToContents);

  //--

  setUniformRowHeights(true);

  setAlternatingRowColors(true);

  setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

  //--

  CQPropertyDelegate *delegate = new CQPropertyDelegate(this);

  setItemDelegate(delegate);

  //--

  connect(this, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(itemClickedSlot(const QModelIndex &)));

  QItemSelectionModel *sm = this->selectionModel();

  connect(sm, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this, SLOT(itemSelectionSlot()));

  //---

  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, SIGNAL(customContextMenuRequested(const QPoint&)),
          this, SLOT(customContextMenuSlot(const QPoint&)));
}

void
CQPropertyView::
setMouseHighlight(bool b)
{
  mouseHighlight_ = b;

  setMouseTracking(mouseHighlight_);
}

void
CQPropertyView::
setFilter(const QString &filter)
{
  filter_->setFilter(filter);
}

void
CQPropertyView::
modelResetSlot()
{
  std::cerr << "model reset" << std::endl;
}

void
CQPropertyView::
clear()
{
  model_->clear();
}

void
CQPropertyView::
addProperty(const QString &path, QObject *obj, const QString &name, const QString &alias)
{
  model_->addProperty(path, obj, name, alias);
}

void
CQPropertyView::
selectObject(const QObject *obj)
{
  CQPropertyItem *root = model_->root();

  for (int i = 0; i < root->numChildren(); ++i) {
    CQPropertyItem *item = root->child(i);

    if (selectObject(item, obj))
      return;
  }
}

bool
CQPropertyView::
selectObject(CQPropertyItem *item, const QObject *obj)
{
  QObject *obj1 = item->object();

  if (obj1 == obj) {
    selectItem(item, true);
    return true;
  }

  for (int i = 0; i < item->numChildren(); ++i) {
    CQPropertyItem *item1 = item->child(i);

    if (selectObject(item1, obj))
      return true;
  }

  return false;
}

void
CQPropertyView::
expandSelected()
{
  QModelIndexList indices = this->selectedIndexes();

  for (int i = 0; i < indices.length(); ++i) {
    CQPropertyItem *item = getModelItem(indices[i]);

    expandItemTree(item);
  }

  resizeColumnToContents(0);
  resizeColumnToContents(1);

  for (int i = 0; i < indices.length(); ++i) {
    CQPropertyItem *item = getModelItem(indices[i]);

    scrollToItem(item);
  }
}

void
CQPropertyView::
getSelectedObjects(std::vector<QObject *> &objs)
{
  QModelIndexList indices = this->selectedIndexes();

  for (int i = 0; i < indices.length(); ++i) {
    CQPropertyItem *item = getModelItem(indices[i]);

    QObject *obj;
    QString  path;

    getItemData(item, obj, path);

    objs.push_back(obj);
  }
}

void
CQPropertyView::
search(const QString &text)
{
  QRegExp regexp(text, Qt::CaseSensitive, QRegExp::Wildcard);

  CQPropertyItem *root = model_->root();

  Items items;

  for (int i = 0; i < root->numChildren(); ++i) {
    CQPropertyItem *item = root->child(i);

    searchItemTree(item, regexp, items);
  }

  for (uint i = 0; i < items.size(); ++i) {
    CQPropertyItem *item = items[i];

    expandItemTree(item);
  }

  resizeColumnToContents(0);
  resizeColumnToContents(1);

  for (uint i = 0; i < items.size(); ++i) {
    CQPropertyItem *item = items[i];

    scrollToItem(item);
  }
}

void
CQPropertyView::
searchItemTree(CQPropertyItem *item, const QRegExp &regexp, Items &items)
{
  QString itemText = item->name();

  if (regexp.exactMatch(itemText)) {
    selectItem(item, true);

    items.push_back(item);
  }
  else
    selectItem(item, false);

  int n = item->numChildren();

  for (int i = 0; i < n; ++i) {
    CQPropertyItem *item1 = item->child(i);

    searchItemTree(item1, regexp, items);
  }
}

void
CQPropertyView::
expandItemTree(CQPropertyItem *item)
{
  while (item) {
    expandItem(item);

    item = item->parent();
  }
}

void
CQPropertyView::
itemClickedSlot(const QModelIndex &index)
{
  CQPropertyItem *item = getModelItem(index);

  if (item && index.column() == 1) {
    if (item->click()) {
      update(index);
    }
  }

  //---

  QObject *obj;
  QString  path;

  getItemData(item, obj, path);

  emit itemClicked(obj, path);
}

void
CQPropertyView::
itemSelectionSlot()
{
  // filter model indices
  QModelIndexList indices = this->selectedIndexes();
  if (indices.empty()) return;

  QModelIndex ind = indices[0];

  assert(ind.model() == filter_);

  CQPropertyItem *item = getModelItem(ind);

  QObject *obj;
  QString  path;

  getItemData(item, obj, path);

  emit itemSelected(obj, path);
}

CQPropertyItem *
CQPropertyView::
getModelItem(const QModelIndex &ind, bool map) const
{
  if (map) {
    bool ok;

    CQPropertyItem *item = model_->item(ind, ok);

    if (! item)
      return nullptr;

    assert(! ok);

    QModelIndex ind1 = filter_->mapToSource(ind);

    CQPropertyItem *item1 = model_->item(ind1);

    return item1;
  }
  else {
    CQPropertyItem *item = model_->item(ind);

    return item;
  }
}

void
CQPropertyView::
getItemData(CQPropertyItem *item, QObject* &obj, QString &path)
{
  CQPropertyItem *item1 = item;

  while (item1) {
    if (path.length())
      path = item1->name() + "/" + path;
    else
      path = item1->name();

    item1 = item1->parent();
  }

  item1 = item;

  int n = item1->numChildren();

  while (n > 0) {
    item1 = item1->child(0);

    n = item1->numChildren();
  }

  obj = item1->object();
}

void
CQPropertyView::
customContextMenuSlot(const QPoint &pos)
{
  CQPropertyItem *item = getModelItem(indexAt(pos));
  if (! item) return;

  QObject *obj;
  QString  path;

  getItemData(item, obj, path);

  if (! obj)
    return;

  // Map point to global from the viewport to account for the header.
  showContextMenu(obj, viewport()->mapToGlobal(pos));
}

void
CQPropertyView::
showContextMenu(QObject *obj, const QPoint &globalPos)
{
  emit menuExec(obj, globalPos);
}

void
CQPropertyView::
mouseMoveEvent(QMouseEvent *me)
{
  if (! isMouseHighlight()) return;

  CQPropertyItem *item = getModelItem(indexAt(me->pos()));

  if (item) {
    QModelIndex ind = indexFromItem(item, 0, /*map*/true);

    setMouseInd(ind);
  }
  else
    unsetMouseInd();

  update();
}

void
CQPropertyView::
leaveEvent(QEvent *)
{
  if (! isMouseHighlight()) return;

  unsetMouseInd();

  update();
}

void
CQPropertyView::
scrollToItem(CQPropertyItem *item)
{
  QModelIndex ind = indexFromItem(item, 0, /*map*/true);

  scrollTo(ind);
}

void
CQPropertyView::
selectItem(CQPropertyItem *item, bool selected)
{
  QItemSelectionModel *sm = this->selectionModel();

  if (selected) {
    QModelIndex ind = indexFromItem(item, 0, /*map*/true);

    sm->select(ind, QItemSelectionModel::SelectCurrent);
  }

  //sm->select(ind, QItemSelectionModel::Deselect);
}

void
CQPropertyView::
expandItem(CQPropertyItem *item)
{
  QModelIndex ind = indexFromItem(item, 0, /*map*/true);

  setExpanded(ind, true);
}

QModelIndex
CQPropertyView::
indexFromItem(CQPropertyItem *item, int column, bool map) const
{
  CQPropertyItem *root = model_->root();

  if (item == root)
    return QModelIndex();

  CQPropertyItem *parentItem = item->parent();

  for (int i = 0; parentItem->numChildren(); ++i) {
    if (parentItem->child(i) == item) {
      QModelIndex parentInd = indexFromItem(parentItem, 0, false);

      QModelIndex ind = model_->index(i, column, parentInd);

      if (map) {
        CQPropertyViewFilter *filterModel = this->filterModel();

        return filterModel->mapFromSource(ind);
      }
      else
        return ind;
    }
  }

  return QModelIndex();
}

bool
CQPropertyView::
isMouseInd(const QModelIndex &i)
{
  if (! isMouseHighlight())
    return false;

  if (! hasMouseInd_)
    return false;

  if (mouseInd_.parent() != i.parent())
    return false;

  return (mouseInd_.row() == i.row());
}
