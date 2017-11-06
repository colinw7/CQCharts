#include <CQPropertyViewTree.h>
#include <CQPropertyViewFilter.h>
#include <CQPropertyViewModel.h>
#include <CQPropertyViewDelegate.h>
#include <CQPropertyViewItem.h>
#include <CQHeaderView.h>

#include <QSortFilterProxyModel>
#include <QHeaderView>
#include <QMouseEvent>
#include <QMenu>
#include <set>

CQPropertyViewTree::
CQPropertyViewTree(QWidget *parent, CQPropertyViewModel *model) :
 QTreeView(parent), model_(model)
{
  setObjectName("propertyView");

  //--

  filter_ = new CQPropertyViewFilter(this);

  connect(model_, SIGNAL(valueChanged(QObject *, const QString &)),
          this, SIGNAL(valueChanged(QObject *, const QString &)));
  connect(model_, SIGNAL(valueChanged(QObject *, const QString &)),
          this, SLOT(redraw()));

  filter_->setSourceModel(model_);

  setModel(filter_);

  //--

  setHeader(new CQHeaderView(this));

  header()->setStretchLastSection(true);
  //header()->setSectionResizeMode(QHeaderView::Interactive);
  //header()->setSectionResizeMode(QHeaderView::ResizeToContents);

  //--

  setSelectionMode(ExtendedSelection);

  setUniformRowHeights(true);

  setAlternatingRowColors(true);

  setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);

  //--

  CQPropertyViewDelegate *delegate = new CQPropertyViewDelegate(this);

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

CQPropertyViewTree::
~CQPropertyViewTree()
{
  delete filter_;
}

void
CQPropertyViewTree::
setMouseHighlight(bool b)
{
  mouseHighlight_ = b;

  setMouseTracking(mouseHighlight_);
}

void
CQPropertyViewTree::
setFilter(const QString &filter)
{
  filter_->setFilter(filter);
}

void
CQPropertyViewTree::
modelResetSlot()
{
  //std::cerr << "model reset" << std::endl;
}

void
CQPropertyViewTree::
redraw()
{
  viewport()->update();
}

void
CQPropertyViewTree::
clear()
{
  model_->clear();
}

void
CQPropertyViewTree::
addProperty(const QString &path, QObject *obj, const QString &name, const QString &alias)
{
  model_->addProperty(path, obj, name, alias);
}

bool
CQPropertyViewTree::
setProperty(QObject *object, const QString &path, const QVariant &value)
{
  bool rc = model_->setProperty(object, path, value);

  redraw();

  return rc;
}

bool
CQPropertyViewTree::
getProperty(QObject *object, const QString &path, QVariant &value)
{
  return model_->getProperty(object, path, value);
}

void
CQPropertyViewTree::
selectObject(const QObject *obj)
{
  CQPropertyViewItem *root = model_->root();

  for (int i = 0; i < root->numChildren(); ++i) {
    CQPropertyViewItem *item = root->child(i);

    if (selectObject(item, obj))
      return;
  }
}

bool
CQPropertyViewTree::
selectObject(CQPropertyViewItem *item, const QObject *obj)
{
  QObject *obj1 = item->object();

  if (obj1 == obj) {
    selectItem(item, true);
    return true;
  }

  for (int i = 0; i < item->numChildren(); ++i) {
    CQPropertyViewItem *item1 = item->child(i);

    if (selectObject(item1, obj))
      return true;
  }

  return false;
}

void
CQPropertyViewTree::
expandAll()
{
  CQPropertyViewItem *root = model_->root();

  expandAll(root);
}

void
CQPropertyViewTree::
expandAll(CQPropertyViewItem *item)
{
  expandItemTree(item);

  for (int i = 0; i < item->numChildren(); ++i) {
    CQPropertyViewItem *item1 = item->child(i);

    expandAll(item1);
  }
}

void
CQPropertyViewTree::
collapseAll()
{
  CQPropertyViewItem *root = model_->root();

  collapseAll(root);
}

void
CQPropertyViewTree::
collapseAll(CQPropertyViewItem *item)
{
  collapseItemTree(item);

  for (int i = 0; i < item->numChildren(); ++i) {
    CQPropertyViewItem *item1 = item->child(i);

    collapseAll(item1);
  }
}

void
CQPropertyViewTree::
expandSelected()
{
  QModelIndexList indices = this->selectedIndexes();

  for (int i = 0; i < indices.length(); ++i) {
    CQPropertyViewItem *item = getModelItem(indices[i]);

    expandItemTree(item);
  }

  resizeColumnToContents(0);
  resizeColumnToContents(1);

  for (int i = 0; i < indices.length(); ++i) {
    CQPropertyViewItem *item = getModelItem(indices[i]);

    scrollToItem(item);
  }
}

void
CQPropertyViewTree::
getSelectedObjects(std::vector<QObject *> &objs)
{
  QModelIndexList indices = this->selectedIndexes();

  for (int i = 0; i < indices.length(); ++i) {
    CQPropertyViewItem *item = getModelItem(indices[i]);

    QObject *obj;
    QString  path;

    getItemData(item, obj, path);

    objs.push_back(obj);
  }
}

void
CQPropertyViewTree::
search(const QString &text)
{
  QString searchStr = text;

  if (searchStr.length() && searchStr[searchStr.length() - 1] != '*')
    searchStr += "*";

  if (searchStr.length() && searchStr[0] != '*')
    searchStr = "*" + searchStr;

  QRegExp regexp(searchStr, Qt::CaseSensitive, QRegExp::Wildcard);

  CQPropertyViewItem *root = model_->root();

  // get matching items
  Items items;

  for (int i = 0; i < root->numChildren(); ++i) {
    CQPropertyViewItem *item = root->child(i);

    searchItemTree(item, regexp, items);
  }


  // ensure selection visible
  // select matching items
  QItemSelectionModel *sm = this->selectionModel();

  sm->clear();

  for (uint i = 0; i < items.size(); ++i) {
    CQPropertyViewItem *item = items[i];

    selectItem(item, true);
  }

  //---

  // ensure selection expanded
  for (uint i = 0; i < items.size(); ++i) {
    CQPropertyViewItem *item = items[i];

    expandItemTree(item);
  }

  //---

  // make item visible
  resizeColumnToContents(0);
  resizeColumnToContents(1);

  for (uint i = 0; i < items.size(); ++i) {
    CQPropertyViewItem *item = items[i];

    scrollToItem(item);
  }
}

void
CQPropertyViewTree::
searchItemTree(CQPropertyViewItem *item, const QRegExp &regexp, Items &items)
{
  QString itemText = item->aliasName();

  if (regexp.exactMatch(itemText))
    items.push_back(item);

  int n = item->numChildren();

  for (int i = 0; i < n; ++i) {
    CQPropertyViewItem *item1 = item->child(i);

    searchItemTree(item1, regexp, items);
  }
}

void
CQPropertyViewTree::
expandItemTree(CQPropertyViewItem *item)
{
  while (item) {
    expandItem(item);

    item = item->parent();
  }
}

void
CQPropertyViewTree::
collapseItemTree(CQPropertyViewItem *item)
{
  while (item) {
    collapseItem(item);

    item = item->parent();
  }
}

void
CQPropertyViewTree::
itemClickedSlot(const QModelIndex &index)
{
  CQPropertyViewItem *item = getModelItem(index);

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
CQPropertyViewTree::
itemSelectionSlot()
{
  // filter model indices
  QModelIndexList indices = this->selectedIndexes();
  if (indices.empty()) return;

  QModelIndex ind = indices[0];

  assert(ind.model() == filter_);

  CQPropertyViewItem *item = getModelItem(ind);

  QObject *obj;
  QString  path;

  getItemData(item, obj, path);

  emit itemSelected(obj, path);
}

CQPropertyViewItem *
CQPropertyViewTree::
getModelItem(const QModelIndex &ind, bool map) const
{
  if (map) {
    bool ok;

    CQPropertyViewItem *item = model_->item(ind, ok);

    if (! item)
      return nullptr;

    assert(! ok);

    QModelIndex ind1 = filter_->mapToSource(ind);

    CQPropertyViewItem *item1 = model_->item(ind1);

    return item1;
  }
  else {
    CQPropertyViewItem *item = model_->item(ind);

    return item;
  }
}

void
CQPropertyViewTree::
getItemData(CQPropertyViewItem *item, QObject* &obj, QString &path)
{
  path = item->path("/");

  //---

  // use object from first branch child
  CQPropertyViewItem *item1 = item;

  int n = item1->numChildren();

  while (n > 0) {
    item1 = item1->child(0);

    n = item1->numChildren();
  }

  obj = item1->object();
}

void
CQPropertyViewTree::
customContextMenuSlot(const QPoint &pos)
{
  // Map point to global from the viewport to account for the header.
  QPoint mpos = viewport()->mapToGlobal(pos);

  if (isItemMenu()) {
    CQPropertyViewItem *item = getModelItem(indexAt(pos));

    if (item) {
      QObject *obj;
      QString  path;

      getItemData(item, obj, path);

      if (obj) {
        showContextMenu(obj, mpos);

        return;
      }
    }
  }

  //---

  QMenu *menu = new QMenu;

  QAction *expandAction   = new QAction("Expand All"  , menu);
  QAction *collapseAction = new QAction("Collapse All", menu);

  connect(expandAction  , SIGNAL(triggered()), this, SLOT(expandAll()));
  connect(collapseAction, SIGNAL(triggered()), this, SLOT(collapseAll()));

  menu->addAction(expandAction);
  menu->addAction(collapseAction);

  menu->exec(mpos);

  delete menu;
}

void
CQPropertyViewTree::
showContextMenu(QObject *obj, const QPoint &globalPos)
{
  emit menuExec(obj, globalPos);
}

void
CQPropertyViewTree::
mouseMoveEvent(QMouseEvent *me)
{
  if (! isMouseHighlight())
    return;

  QModelIndex ind = indexAt(me->pos());

  if (ind.isValid()) {
    CQPropertyViewItem *item = getModelItem(ind);

    if (item) {
      if (! isMouseInd(ind)) {
        setMouseInd(ind);

        redraw();
      }

      return;
    }
  }

  if (! isMouseInd(QModelIndex())) {
    unsetMouseInd();

    redraw();
  }
}

void
CQPropertyViewTree::
leaveEvent(QEvent *)
{
  if (! isMouseHighlight()) return;

  unsetMouseInd();

  redraw();
}

void
CQPropertyViewTree::
scrollToItem(CQPropertyViewItem *item)
{
  QModelIndex ind = indexFromItem(item, 0, /*map*/true);

  scrollTo(ind);
}

void
CQPropertyViewTree::
selectItem(CQPropertyViewItem *item, bool selected)
{
  QItemSelectionModel *sm = this->selectionModel();

  if (selected) {
    QModelIndex ind = indexFromItem(item, 0, /*map*/true);

    sm->select(ind, QItemSelectionModel::Select);
  }

  //sm->select(ind, QItemSelectionModel::Deselect);
}

void
CQPropertyViewTree::
expandItem(CQPropertyViewItem *item)
{
  QModelIndex ind = indexFromItem(item, 0, /*map*/true);

  setExpanded(ind, true);
}

void
CQPropertyViewTree::
collapseItem(CQPropertyViewItem *item)
{
  QModelIndex ind = indexFromItem(item, 0, /*map*/true);

  setExpanded(ind, false);
}

QModelIndex
CQPropertyViewTree::
indexFromItem(CQPropertyViewItem *item, int column, bool map) const
{
  QModelIndex ind = model_->indexFromItem(item, column);

  if (! ind.isValid())
    return QModelIndex();

  if (map) {
    CQPropertyViewFilter *filterModel = this->filterModel();

    return filterModel->mapFromSource(ind);
  }

  return ind;
}

void
CQPropertyViewTree::
setMouseInd(const QModelIndex &i)
{
  assert(i.isValid());

  hasMouseInd_ = true;
  mouseInd_    = i;
}

void
CQPropertyViewTree::
unsetMouseInd()
{
  hasMouseInd_ = false;
  mouseInd_    = QModelIndex();
}

bool
CQPropertyViewTree::
isMouseInd(const QModelIndex &i)
{
  if (! isMouseHighlight())
    return false;

  if (i.isValid()) {
    if (! hasMouseInd_)
      return false;

    assert(i.model() == mouseInd_.model());

    if (mouseInd_.parent() != i.parent())
      return false;

    return (mouseInd_.row() == i.row());
  }
  else {
    return ! hasMouseInd_;
  }
}
