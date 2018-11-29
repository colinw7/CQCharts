#include <CQChartsTable.h>
#include <CQChartsTableDelegate.h>
#include <CQChartsModelFilter.h>
#include <CQChartsModelExprMatch.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsRegExp.h>
#include <CQChartsVariant.h>
#include <CQChartsModelVisitor.h>
#include <CQCharts.h>

#include <CQCsvModel.h>
#include <CQTsvModel.h>

#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QItemDelegate>
#include <QMenu>
#include <QActionGroup>
#include <cassert>

class CQChartsTableSelectionModel : public QItemSelectionModel {
 public:
  CQChartsTableSelectionModel(CQChartsTable *table) :
   QItemSelectionModel(table->CQTableView::model()), table_(table) {
    setObjectName("tableSelectionModel");
  }

  void select(const QModelIndex &ind, SelectionFlags flags) {
    QItemSelectionModel::select(ind, adjustFlags(flags));
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
CQChartsTable(CQCharts *charts, QWidget *parent) :
 CQTableView(parent), charts_(charts)
{
  setObjectName("table");

  setSortingEnabled(true);

  horizontalHeader()->setSectionsClickable(true);
//horizontalHeader()->setHighlightSections(true);
  horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

  verticalHeader()->setVisible(false);

  setSelectionBehavior(SelectRows);

  connect(horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerClickedSlot(int)));

  connect(this, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(itemClickedSlot(const QModelIndex &)));

  delegate_ = new CQChartsTableDelegate(this);

  setItemDelegate(delegate_);

  connect(charts_, SIGNAL(modelTypeChanged(int)), this, SLOT(modelTypeChangedSlot(int)));
}

CQChartsTable::
~CQChartsTable()
{
  delete delegate_;
  delete match_;
}

void
CQChartsTable::
modelTypeChangedSlot(int modelId)
{
  CQChartsModelData *modelData = charts_->getModelData(model_.data());

  if (modelData && modelData->ind() == modelId)
    delegate_->clearColumnTypes();
}

void
CQChartsTable::
addMenuActions(QMenu *menu)
{
  CQTableView::addMenuActions(menu);

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
CQChartsTable::
setModelP(const ModelP &model)
{
  if (sm_)
    disconnect(sm_, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
               this, SLOT(selectionSlot()));

  //---

  model_ = model;

  CQTableView::setModel(model_.data());

  if (model_.data()) {
    sm_ = new CQChartsTableSelectionModel(this);

    setSelectionModel(sm_);
  }

  //---

  if (sm_)
    connect(sm_, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
            this, SLOT(selectionSlot()));
}

void
CQChartsTable::
setFilterAnd(bool b)
{
  CQChartsModelFilter *modelFilter = qobject_cast<CQChartsModelFilter *>(model_.data());

  if (modelFilter)
    modelFilter->setFilterCombine(b ? CQChartsModelFilter::Combine::AND :
                                      CQChartsModelFilter::Combine::OR);
}

void
CQChartsTable::
setFilter(const QString &filter)
{
  addReplaceFilter(filter, /*add*/false);
}

void
CQChartsTable::
addFilter(const QString &filter)
{
  addReplaceFilter(filter, /*add*/true);
}

void
CQChartsTable::
addReplaceFilter(const QString &filter, bool add)
{
  if (! model_)
    return;

  CQChartsModelFilter *modelFilter = qobject_cast<CQChartsModelFilter *>(model_.data());

  if (modelFilter) {
    if (sm_)
      modelFilter->setSelectionModel(sm_);

    if (add)
      modelFilter->pushFilterData();
    else
      modelFilter->resetFilterData();

    if      (filter == "selected" || filter == "non-selected") {
      bool invert = (filter == "non-selected");

      modelFilter->setSelectionFilter(invert);
    }
    else if (isExprFilter()) {
      modelFilter->setExpressionFilter(filter);
    }
    else {
      modelFilter->setRegExpFilter(filter);
    }
  }
  else {
    QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model_.data());
    assert(proxyModel);

    QAbstractItemModel *model = proxyModel->sourceModel();
    assert(model);

    QString filter1;
    int     column = -1;

    if (CQChartsModelUtil::decodeModelFilterStr(model, filter, filter1, column))
      proxyModel->setFilterKeyColumn(column);

    proxyModel->setFilterWildcard(filter1);
  }

  emit filterChanged();
}

QString
CQChartsTable::
filterDetails() const
{
  CQChartsModelFilter *modelFilter = qobject_cast<CQChartsModelFilter *>(model_.data());

  if (modelFilter)
    return modelFilter->filterDetails();

  return "";
}

void
CQChartsTable::
setSearch(const QString &text)
{
  addReplaceSearch(text, /*add*/false);
}

void
CQChartsTable::
addSearch(const QString &text)
{
  addReplaceSearch(text, /*add*/true);
}

void
CQChartsTable::
addReplaceSearch(const QString &text, bool add)
{
  if (! add)
    matches_.clear();

  matches_.push_back(text);

  //---

  if (! model_)
    return;

  QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model_.data());
  assert(proxyModel);

  //---

  int oldKeyColumn = proxyModel->filterKeyColumn();

  int keyColumn = oldKeyColumn;

  // get matching items
  using Rows = std::vector<QModelIndex>;

  Rows rows;

  if (! isExprFilter()) {
    QString text1;
    int     column = -1;

    if (CQChartsModelUtil::decodeModelFilterStr(model_.data(), text, text1, column))
      proxyModel->setFilterKeyColumn(column);

    keyColumn = proxyModel->filterKeyColumn();

    class RowVisitor : public CQChartsModelVisitor {
     public:
      RowVisitor(CQChartsTable *table, const QString &text, int column, Rows &rows) :
       table_(table), regexp_(text), column_(column), rows_(rows) {
      }

      CQChartsTable *table() const { return table_; }

      State visit(QAbstractItemModel *model, const VisitData &data) override {
        QModelIndex ind = model->index(data.row, column_, data.parent);

        bool ok;

        QString str = CQChartsModelUtil::modelString(model, ind, ok);
        if (! ok) return State::SKIP;

        if (regexp_.match(str))
          rows_.push_back(ind);

        return State::OK;
      }

     private:
      CQChartsTable* table_  { nullptr };
      CQChartsRegExp regexp_;
      int            column_ { 0 };
      Rows&          rows_;
    };

    RowVisitor visitor(this, text1, keyColumn, rows);

    (void) CQChartsModelVisit::exec(charts_, model_.data(), visitor);
  }
  else {
    if (! match_)
      match_ = new CQChartsModelExprMatch;

    QAbstractItemModel *model = proxyModel->sourceModel();

    match_->setModel(model);

    match_->initColumns();

    class RowVisitor : public CQChartsModelVisitor {
     public:
      RowVisitor(CQChartsTable *table, CQChartsModelExprMatch *match, const Matches &matches,
                 int column, Rows &rows) :
       table_(table), match_(match), matches_(matches), column_(column), rows_(rows) {
      }

      CQChartsTable *table() const { return table_; }

      State visit(QAbstractItemModel *model, const VisitData &data) override {
        QModelIndex ind = model->index(data.row, column_, data.parent);

        bool isMatch = false;

        for (const auto &matchText : matches_) {
          match_->initMatch(matchText); // TODO: eval once

          bool ok;

          if (match_->match(ind, ok) && ok) {
            isMatch = true;
            break;
          }
        }

        if (isMatch)
          rows_.push_back(ind);

        return State::OK;
      }

     private:
      CQChartsTable*          table_  { nullptr };
      CQChartsModelExprMatch* match_  { nullptr };
      const Matches&          matches_;
      int                     column_ { 0 };
      Rows&                   rows_;
    };

    RowVisitor visitor(this, match_, matches_, keyColumn, rows);

    (void) CQChartsModelVisit::exec(charts_, model_.data(), visitor);
  }

  //---

  // select matching items
  QItemSelection sel;

  for (auto &r : rows) {
    QModelIndex ind = model_->index(r.row(), keyColumn, r.parent());

    sel.select(ind, ind);
  }

  QItemSelectionModel *sm = this->selectionModel();

  sm->clear();

  sm->select(sel, QItemSelectionModel::Select);

  //---

  // make item visible
  for (auto &r : rows) {
    QModelIndex ind = model_->index(r.row(), keyColumn, r.parent());

    scrollTo(ind);

    break;
  }

  //---

  // reset key column (if changed)
  if (oldKeyColumn != keyColumn)
    proxyModel->setFilterKeyColumn(oldKeyColumn);
}

void
CQChartsTable::
headerClickedSlot(int section)
{
  emit columnClicked(section);
}

void
CQChartsTable::
itemClickedSlot(const QModelIndex &index)
{
  delegate_->click(index);
}

void
CQChartsTable::
selectionSlot()
{
  QModelIndexList indices = selectedIndexes();
  if (indices.empty()) return;

  scrollTo(indices.at(0), QAbstractItemView::EnsureVisible);

  emit selectionChanged();
}

void
CQChartsTable::
scrollTo(const QModelIndex &index, ScrollHint hint)
{
  if (hint == QAbstractItemView::EnsureVisible)
    return;

  CQTableView::scrollTo(index, hint);
}

void
CQChartsTable::
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
CQChartsTable::
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

CQChartsModelDetails *
CQChartsTable::
getDetails()
{
  CQChartsModelData *modelData = charts_->getModelData(model_.data());
  assert(modelData);

  return modelData->details();
}

QSize
CQChartsTable::
sizeHint() const
{
  QFontMetricsF fm(font());

  return QSize(fm.width("X")*40, 20*fm.height());
}
