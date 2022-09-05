#include <CQChartsTable.h>
#include <CQChartsTableDelegate.h>
#include <CQChartsModelFilter.h>
#include <CQChartsModelExprMatch.h>
#include <CQChartsModelData.h>
#include <CQChartsModelUtil.h>
#include <CQChartsRegExp.h>
#include <CQChartsVariant.h>
#include <CQChartsModelVisitor.h>
#include <CQChartsSelectionModel.h>
#include <CQCharts.h>

#include <CQCsvModel.h>
#include <CQTsvModel.h>

#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QFileDialog>
#include <QDir>

#include <cassert>

CQChartsTable::
CQChartsTable(CQCharts *charts, QWidget *parent) :
 CQTableView(parent), charts_(charts)
{
  setObjectName("table");

  setSortingEnabled(true);

  auto *hheader = this->horizontalHeader();
  auto *vheader = this->verticalHeader();

  hheader->setSectionsClickable(true);
//hheader->setHighlightSections(true);
  hheader->setSortIndicator(0, Qt::AscendingOrder);

  vheader->setVisible(false);

  setSelectionBehavior(SelectRows);

  connect(hheader, SIGNAL(sectionClicked(int)), this, SLOT(headerClickedSlot(int)));

  connect(this, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(itemClickedSlot(const QModelIndex &)));

  //---

  delegate_ = new CQChartsTableDelegate(this);

  setItemDelegate(delegate_);

  //---

  connect(charts_, SIGNAL(modelTypeChanged(int)), this, SLOT(modelTypeChangedSlot(int)));
}

CQChartsTable::
~CQChartsTable()
{
  if (modelData_ && sm_)
    modelData_->removeSelectionModel(sm_);

  delete delegate_;
  delete match_;
}

void
CQChartsTable::
modelTypeChangedSlot(int modelId)
{
  auto *modelData = getModelData();

  if (modelData && modelData->isInd(modelId)) {
    //delegate_->clearColumnTypes();
  }
}

void
CQChartsTable::
addMenuActions(QMenu *menu)
{
  CQTableView::addMenuActions(menu);

  //---

  auto addMenu = [&](const QString &name) {
    auto *subMenu = new QMenu(name, menu);

    menu->addMenu(subMenu);

    return subMenu;
  };

  auto addActionGroup = [&](QMenu *menu, const char *slotName) {
    return CQUtil::createActionGroup(menu, this, slotName);
  };

  auto addAction = [&](const QString &name, const char *slotName) {
    return CQUtil::addAction(menu, name, this, slotName);
  };

  //---

  auto *selectMenu = addMenu("Select");

  auto *selectActionGroup = addActionGroup(selectMenu, SLOT(selectionBehaviorSlot(QAction *)));

  auto addSelectAction = [&](const QString &name, bool checked) {
    auto *action = CQUtil::addCheckedAction(selectMenu, name, checked);

    selectActionGroup->addAction(action);
  };

  addSelectAction("Items"  , selectionBehavior() == SelectItems  );
  addSelectAction("Rows"   , selectionBehavior() == SelectRows   );
  addSelectAction("Columns", selectionBehavior() == SelectColumns);

  CQUtil::addActionGroupToMenu(selectActionGroup);

  //---

  auto *exportMenu = addMenu("Export");

  auto *exportActionGroup = addActionGroup(exportMenu, SLOT(exportSlot(QAction *)));

  auto addExportAction = [&](const QString &name) {
    auto *action = CQUtil::addAction(exportMenu, name);

    exportActionGroup->addAction(action);
  };

  addExportAction("CSV");
  addExportAction("TSV");
  addExportAction("JSON");

  CQUtil::addActionGroupToMenu(exportActionGroup);

  //---

  addAction("Edit", SLOT(editSlot()));
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

  resetModelData();

  //---

  if (model_.data()) {
    auto *modelData = getModelData();

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
CQChartsTable::
setFilterAnd(bool b)
{
  auto *modelFilter = qobject_cast<CQChartsModelFilter *>(model_.data());

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

  auto *modelFilter = qobject_cast<CQChartsModelFilter *>(model_.data());

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
    auto *proxyModel = qobject_cast<QSortFilterProxyModel *>(model_.data());
    assert(proxyModel);

    auto *model = proxyModel->sourceModel();
    assert(model);

    QString filter1;
    int     column = -1;

    if (CQChartsModelUtil::decodeModelFilterStr(model, filter, filter1, column))
      proxyModel->setFilterKeyColumn(column);

    proxyModel->setFilterWildcard(filter1);
  }

  Q_EMIT filterChanged();
}

QString
CQChartsTable::
filterDetails() const
{
  auto *modelFilter = qobject_cast<CQChartsModelFilter *>(model_.data());

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

  auto *proxyModel = qobject_cast<QSortFilterProxyModel *>(model_.data());
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

      State visit(const QAbstractItemModel *model, const VisitData &data) override {
        auto ind = model->index(data.row, column_, data.parent);

        bool ok;

        auto str = CQChartsModelUtil::modelString(model, ind, ok);
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

    auto *model = proxyModel->sourceModel();

    match_->setModel(model);

    match_->initColumns();

    class RowVisitor : public CQChartsModelVisitor {
     public:
      RowVisitor(CQChartsTable *table, CQChartsModelExprMatch *match, const Matches &matches,
                 int column, Rows &rows) :
       table_(table), match_(match), matches_(matches), column_(column), rows_(rows) {
      }

      CQChartsTable *table() const { return table_; }

      State visit(const QAbstractItemModel *model, const VisitData &data) override {
        auto ind = model->index(data.row, column_, data.parent);

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
    auto ind = model_->index(r.row(), keyColumn, r.parent());

    sel.select(ind, ind);
  }

  auto *sm = this->selectionModel();

  sm->clear();

  sm->select(sel, QItemSelectionModel::Select);

  //---

  // make item visible
  for (auto &r : rows) {
    auto ind = model_->index(r.row(), keyColumn, r.parent());

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
  Q_EMIT columnClicked(section);
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
  auto indices = selectedIndexes();
  if (indices.empty()) return;

  scrollTo(indices.at(0), QAbstractItemView::EnsureVisible);

  Q_EMIT selectionHasChanged();
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
  auto type = action->text();

  bool hheader = true;
  bool vheader = false;

  auto dir = QDir::current().dirName();
  QString pattern;

  if      (type == "CSV") {
    dir     += "/model.csv";
    pattern  = "Files (*.csv)";
  }
  else if (type == "TSV") {
    dir     += "/model.tsv";
    pattern  = "Files (*.tsv)";
  }
  else if (type == "JSON") {
    dir     += "/model.json";
    pattern  = "Files (*.json)";
  }
  else {
    std::cerr << "Invalid export type '" << type.toStdString() << "'\n";
    return;
  }

  auto filename = QFileDialog::getSaveFileName(this, "Export Model", dir, pattern);
  if (! filename.length()) return; // cancelled

  auto *modelData = getModelData();

  if      (type == "CSV")
    modelData->exportModel(filename, CQBaseModelDataType::CSV, hheader, vheader);
  else if (type == "TSV")
    modelData->exportModel(filename, CQBaseModelDataType::TSV, hheader, vheader);
  else if (type == "JSON")
    modelData->exportModel(filename, CQBaseModelDataType::JSON, hheader, vheader);
  else
    assert(false);
}

void
CQChartsTable::
editSlot()
{
  auto *modelData = getModelData();

  if (modelData)
    charts_->editModelDlg(modelData);
}

CQChartsModelData *
CQChartsTable::
getModelData()
{
  if (! modelData_) {
    modelData_ = charts_->getModelData(model_);

    if (modelData_)
      connect(modelData_, SIGNAL(modelChanged()), this, SLOT(resetModelData()));
  }

  return modelData_;
}

CQChartsModelDetails *
CQChartsTable::
getDetails()
{
  auto *modelData = getModelData();

  return (modelData ? modelData->details() : nullptr);
}

void
CQChartsTable::
resetModelData()
{
  if (modelData_)
    disconnect(modelData_, SIGNAL(modelChanged()), this, SLOT(resetModelData()));

  modelData_ = nullptr;

  delegate_->resetColumnData();
}

QSize
CQChartsTable::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.horizontalAdvance("X")*40, 20*fm.height());
}
