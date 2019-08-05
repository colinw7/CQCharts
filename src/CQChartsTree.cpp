#include <CQChartsTree.h>
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

  //delegate_ = new CQChartsTreeDelegate(this);

  //setItemDelegate(delegate_);

  //---

  connect(charts_, SIGNAL(modelTypeChanged(int)), this, SLOT(modelTypeChangedSlot(int)));
}

CQChartsTree::
~CQChartsTree()
{
  if (modelData_ && sm_)
    modelData_->removeSelectionModel(sm_);

//delete delegate_
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

  QMenu *selectMenu = new QMenu("Select");

  QActionGroup *selectActionGroup = new QActionGroup(menu);

  QAction *selectItems   = new QAction("Items"  , selectMenu);
  QAction *selectRows    = new QAction("Rows"   , selectMenu);
  QAction *selectColumns = new QAction("Columns", selectMenu);

  selectItems  ->setCheckable(true);
  selectRows   ->setCheckable(true);
  selectColumns->setCheckable(true);

  selectItems  ->setChecked(selectionBehavior() == SelectItems);
  selectRows   ->setChecked(selectionBehavior() == SelectRows);
  selectColumns->setChecked(selectionBehavior() == SelectColumns);

  selectActionGroup->addAction(selectItems);
  selectActionGroup->addAction(selectRows);
  selectActionGroup->addAction(selectColumns);

  connect(selectActionGroup, SIGNAL(triggered(QAction *)),
          this, SLOT(selectionBehaviorSlot(QAction *)));

  selectMenu->addActions(selectActionGroup->actions());

  menu->addMenu(selectMenu);

  //---

  QMenu *exportMenu = new QMenu("Export");

  QActionGroup *exportActionGroup = new QActionGroup(exportMenu);

  QAction *exportCSV = new QAction("CSV", exportMenu);
  QAction *exportTSV = new QAction("TSV", exportMenu);

  exportActionGroup->addAction(exportCSV);
  exportActionGroup->addAction(exportTSV);

  connect(exportActionGroup, SIGNAL(triggered(QAction *)),
          this, SLOT(exportSlot(QAction *)));

  exportMenu->addActions(exportActionGroup->actions());

  menu->addMenu(exportMenu);
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

  resetModelData();

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

  QString filter1;
  int     column { -1 };

  CQChartsModelUtil::decodeModelFilterStr(model_.data(), filter, filter1, column);

  if (column >= 0)
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
itemClickedSlot(const QModelIndex &)
{
  //delegate_->click(index);
}

void
CQChartsTree::
selectionSlot()
{
  QModelIndexList indices = selectedIndexes();
  if (indices.empty()) return;

  scrollTo(indices.at(0), QAbstractItemView::EnsureVisible);

  emit selectionChanged();
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

//delegate_->resetColumnData();
}

QSize
CQChartsTree::
sizeHint() const
{
  QFontMetricsF fm(font());

  return QSize(fm.width("X")*40, 20*fm.height());
}
