#include <CQChartsTable.h>
#include <CQCharts.h>
#include <CQChartsColumn.h>

#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QMenu>
#include <QActionGroup>
#include <cassert>

class CQChartsTableSelectionModel : public QItemSelectionModel {
 public:
  CQChartsTableSelectionModel(CQChartsTable *table) :
   QItemSelectionModel(table->CQTableView::model()), table_(table) {
  }

  void select(const QModelIndex &index, SelectionFlags flags) {
    QItemSelectionModel::select(index, adjustFlags(flags));
  }

  void select(const QItemSelection &selection, SelectionFlags flags) {
    QItemSelectionModel::select(selection, adjustFlags(flags));
  }

 private:
  SelectionFlags adjustFlags(SelectionFlags flags) const {
    if     (table_->selectionBehavior() == QAbstractItemView::SelectRows)
      flags |= Rows;
    else if (table_->selectionBehavior() == QAbstractItemView::SelectColumns)
      flags |= Columns;

    return flags;
  }

 private:
  CQChartsTable *table_ { nullptr };
};

//------

CQChartsTable::
CQChartsTable(CQCharts *charts) :
 CQTableView(nullptr), charts_(charts)
{
  setObjectName("table");

  setSortingEnabled(true);

  horizontalHeader()->setSectionsClickable(true);
  //horizontalHeader()->setHighlightSections(true);

  verticalHeader()->setVisible(false);

  setSelectionBehavior(SelectRows);

  connect(horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerClickSlot(int)));
}

void
CQChartsTable::
addMenuActions(QMenu *menu)
{
  CQTableView::addMenuActions(menu);

  QActionGroup *actionGroup = new QActionGroup(menu);

  QAction *selectItems   = new QAction("Select Items"  , menu);
  QAction *selectRows    = new QAction("Select Rows"   , menu);
  QAction *selectColumns = new QAction("Select Columns", menu);

  selectItems  ->setCheckable(true);
  selectRows   ->setCheckable(true);
  selectColumns->setCheckable(true);

  selectItems  ->setChecked(selectionBehavior() == SelectItems);
  selectRows   ->setChecked(selectionBehavior() == SelectRows);
  selectColumns->setChecked(selectionBehavior() == SelectColumns);

  actionGroup->addAction(selectItems);
  actionGroup->addAction(selectRows);
  actionGroup->addAction(selectColumns);

  connect(actionGroup, SIGNAL(triggered(QAction *)),
          this, SLOT(selectionBehaviorSlot(QAction *)));

  menu->addActions(actionGroup->actions());
}

void
CQChartsTable::
setModel(const ModelP &model)
{
  if (sm_)
    disconnect(sm_, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
               this, SLOT(selectionSlot()));

  //---

  model_ = model;

  QTableView::setModel(model_.data());

  sm_ = new CQChartsTableSelectionModel(this);

  setSelectionModel(sm_);

  //---

  connect(sm_, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this, SLOT(selectionSlot()));
}

void
CQChartsTable::
setFilter(const QString &filter)
{
  if (! model_)
    return;

  QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model_.data());
  assert(proxyModel);

  QString filter1 = filter;

  if (filter.length()) {
    QStringList strs = filter.split(':', QString::KeepEmptyParts);

    if (strs.size() == 2) {
      int column = -1;

      QString name = strs[0];

      for (int i = 0; i < model_->columnCount(); ++i) {
        QString name1 = model_->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();

        if (name == name1) {
          column = i;
          break;
        }
      }

      if (column > 0)
        proxyModel->setFilterKeyColumn(column);

      filter1 = strs[1];
    }
  }

  proxyModel->setFilterWildcard(filter1);
}

void
CQChartsTable::
headerClickSlot(int section)
{
  emit columnClicked(section);
}

void
CQChartsTable::
selectionSlot()
{
  QModelIndexList indices = selectedIndexes();
  if (indices.empty()) return;

  scrollTo(indices.at(0), QAbstractItemView::EnsureVisible);
}

void
CQChartsTable::
selectionBehaviorSlot(QAction *action)
{
  if      (action->text() == "Select Items")
    setSelectionBehavior(SelectItems);
  else if (action->text() == "Select Rows")
    setSelectionBehavior(SelectRows);
  else if (action->text() == "Select Columns")
    setSelectionBehavior(SelectColumns);
}

void
CQChartsTable::
calcDetails(Details &details)
{
  details.numColumns = model_->columnCount();
  details.numRows    = model_->rowCount   ();

  CQChartsColumnTypeMgr *columnTypeMgr = charts_->columnTypeMgr();

  for (int c = 0; c < details.numColumns; ++c) {
    CQBaseModel::Type  type;
    CQChartsNameValues nameValues;

    QString  typeName;
    QVariant minValue;
    QVariant maxValue;

    if (columnTypeMgr->getModelColumnType(model_.data(), c, type, nameValues)) {
      CQChartsColumnType *columnType = columnTypeMgr->getType(type);

      typeName = columnType->name();

      if (type == CQBaseModel::Type::INTEGER) {
        long imin = 0; long imax = 0; bool iset = false;

        for (int r = 0; r < details.numRows; ++r) {
          bool ok;

          QVariant value = CQChartsUtil::modelValue(model_.data(), r, c, ok);
          if (! ok) continue;

          long i = CQChartsUtil::toInt(value, ok);
          if (! ok) continue;

          imin = (! iset ? i : std::min(imin, i));
          imax = (! iset ? i : std::max(imax, i));

          iset = true;
        }

        minValue = QVariant(int(imin));
        maxValue = QVariant(int(imax));
      }
    }

    details.columns.emplace_back(typeName, minValue, maxValue);
  }
}

QSize
CQChartsTable::
sizeHint() const
{
  QFontMetricsF fm(font());

  return QSize(fm.width("X")*40, 20*fm.height());
}
