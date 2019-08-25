#include <CQChartsTree.h>
#include <CQChartsTableDelegate.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsSelectionModel.h>
#include <CQCharts.h>

#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QMenu>
#include <QActionGroup>
#include <cassert>

CQChartsTree::
CQChartsTree(CQCharts *charts, QWidget *parent) :
 CQTreeView(parent), charts_(charts)
{
  setObjectName("tree");

  setSortingEnabled(true);

  header()->setSectionsClickable(true);
//header()->setHighlightSections(true);
  header()->setSortIndicator(0, Qt::AscendingOrder);

  setSelectionBehavior(SelectRows);

  connect(header(), SIGNAL(sectionClicked(int)), this, SLOT(headerClickedSlot(int)));

  connect(this, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(itemClickedSlot(const QModelIndex &)));

  //---

  delegate_ = new CQChartsTableDelegate(this);

  setItemDelegate(delegate_);

  //---

  connect(charts_, SIGNAL(modelTypeChanged(int)), this, SLOT(modelTypeChangedSlot(int)));
}

CQChartsTree::
~CQChartsTree()
{
  if (modelData_ && sm_)
    modelData_->removeSelectionModel(sm_);

  delete delegate_;
}

void
CQChartsTree::
modelTypeChangedSlot(int modelId)
{
  CQChartsModelData *modelData = getModelData();

  if (modelData && modelData->ind() == modelId) {
    //delegate_->clearColumnTypes();
  }
}

void
CQChartsTree::
addMenuActions(QMenu *menu)
{
  CQTreeView::addMenuActions(menu);

  //---

  auto addMenu = [&](const QString &name) {
    QMenu *subMenu = new QMenu(name);

    menu->addMenu(subMenu);

    return subMenu;
  };

  auto addActionGroup = [&](QMenu *menu, const char *slotName) {
    QActionGroup *actionGroup = new QActionGroup(menu);

    connect(actionGroup, SIGNAL(triggered(QAction *)), this, slotName);

    return actionGroup;
  };

  //---

  QMenu *selectMenu = addMenu("Select");

  QActionGroup *selectActionGroup =
    addActionGroup(selectMenu, SLOT(selectionBehaviorSlot(QAction *)));

  auto addSelectAction = [&](const QString &name, bool checked) {
    QAction *action = new QAction(name, selectMenu);

    action->setCheckable(true);
    action->setChecked  (checked);

    selectActionGroup->addAction(action);
  };

  addSelectAction("Items"  , selectionBehavior() == SelectItems  );
  addSelectAction("Rows"   , selectionBehavior() == SelectRows   );
  addSelectAction("Columns", selectionBehavior() == SelectColumns);

  selectMenu->addActions(selectActionGroup->actions());

  //---

  QMenu *exportMenu = addMenu("Export");

  QActionGroup *exportActionGroup =
    addActionGroup(exportMenu, SLOT(exportSlot(QAction *)));

  auto addExportAction = [&](const QString &name) {
    QAction *action = new QAction(name, exportMenu);

    exportActionGroup->addAction(action);
  };

  addExportAction("CSV");
  addExportAction("TSV");

  exportMenu->addActions(exportActionGroup->actions());
}

void
CQChartsTree::
setModelP(const ModelP &model)
{
  if (sm_)
    disconnect(sm_, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
               this, SLOT(selectionSlot()));

  //---

  model_ = model;

  CQTreeView::setModel(model_.data());

  resetModelData();

  //--

  if (model_.data()) {
    CQChartsModelData *modelData = getModelData();

    if (modelData) {
      sm_ = new CQChartsSelectionModel(this, modelData);

      modelData->addSelectionModel(sm_);
    }
    else
      sm_ = new CQChartsSelectionModel(this, model_.data());

    setSelectionModel(sm_);
  }

  //---

  if (sm_)
    connect(sm_, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(selectionSlot()));
}

void
CQChartsTree::
setFilter(const QString &filter)
{
  if (! model_)
    return;

  QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model_.data());
  assert(proxyModel);

  QAbstractItemModel *model = proxyModel->sourceModel();
  assert(model);

  QString filter1;
  int     column { -1 };

  if (CQChartsModelUtil::decodeModelFilterStr(model, filter, filter1, column))
    proxyModel->setFilterKeyColumn(column);

  proxyModel->setFilterWildcard(filter1);

  emit filterChanged();
}

void
CQChartsTree::
headerClickedSlot(int section)
{
  emit columnClicked(section);
}

void
CQChartsTree::
itemClickedSlot(const QModelIndex &index)
{
  delegate_->click(index);
}

void
CQChartsTree::
selectionSlot()
{
  QModelIndexList indices = selectedIndexes();
  if (indices.empty()) return;

  scrollTo(indices.at(0), QAbstractItemView::EnsureVisible);

  emit selectionHasChanged();
}

void
CQChartsTree::
selectionBehaviorSlot(QAction *action)
{
  if      (action->text() == "Items")
    setSelectionBehavior(SelectItems);
  else if (action->text() == "Rows")
    setSelectionBehavior(SelectRows);
  else if (action->text() == "Columns")
    setSelectionBehavior(SelectColumns);
  else
    assert(false);
}

void
CQChartsTree::
exportSlot(QAction *action)
{
  QString type = action->text();

  if      (type == "CSV")
    CQChartsModelUtil::exportModel(modelP().data(), CQBaseModelDataType::CSV);
  else if (type == "TSV")
    CQChartsModelUtil::exportModel(modelP().data(), CQBaseModelDataType::TSV);
  else {
    std::cerr << "Invalid export type '" << type.toStdString() << "'\n";
  }
}

CQChartsModelData *
CQChartsTree::
getModelData()
{
  if (! modelData_) {
    modelData_ = charts_->getModelData(model_.data());

    if (modelData_)
      connect(modelData_, SIGNAL(modelChanged()), this, SLOT(resetModelData()));
  }

  return modelData_;
}

CQChartsModelDetails *
CQChartsTree::
getDetails()
{
  CQChartsModelData *modelData = getModelData();

  return (modelData ? modelData->details() : nullptr);
}

void
CQChartsTree::
resetModelData()
{
  if (modelData_)
    disconnect(modelData_, SIGNAL(modelChanged()), this, SLOT(resetModelData()));

  modelData_ = nullptr;

  delegate_->resetColumnData();
}

QSize
CQChartsTree::
sizeHint() const
{
  QFontMetricsF fm(font());

  return QSize(fm.width("X")*40, 20*fm.height());
}
