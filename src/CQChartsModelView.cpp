#include <CQChartsModelView.h>
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

#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QMenu>
#include <QActionGroup>
#include <QFileDialog>
#include <QDir>

#include <cassert>

CQChartsModelView::
CQChartsModelView(CQCharts *charts, QWidget *parent) :
 CQModelView(parent), charts_(charts)
{
  setObjectName("modelView");

  setSortingEnabled(true);
  setStretchLastColumn(true);

  horizontalHeader()->setSortIndicator(0, Qt::AscendingOrder);

  //setSelectionBehavior(SelectRows);

  connect(horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(headerClickedSlot(int)));

  connect(this, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(itemClickedSlot(const QModelIndex &)));

  //---

  delegate_ = new CQChartsTableDelegate(this);

  setItemDelegate(delegate_);

  //---

  connect(charts_, SIGNAL(modelTypeChanged(int)), this, SLOT(modelTypeChangedSlot(int)));
}

CQChartsModelView::
~CQChartsModelView()
{
  if (modelData_ && sm_)
    modelData_->removeSelectionModel(sm_);

  delete delegate_;
  delete match_;
}

void
CQChartsModelView::
modelTypeChangedSlot(int modelId)
{
  auto *modelData = getModelData();

  if (modelData && modelData->ind() == modelId) {
    //delegate_->clearColumnTypes();
  }
}

void
CQChartsModelView::
addMenuActions(QMenu *menu)
{
  CQModelView::addMenuActions(menu);

  //---

  auto addMenu = [&](const QString &name) {
    auto *subMenu = new QMenu(name, menu);

    menu->addMenu(subMenu);

    return subMenu;
  };

  auto addActionGroup = [&](QMenu *menu, const char *slotName) {
    auto *actionGroup = new QActionGroup(menu);

    connect(actionGroup, SIGNAL(triggered(QAction *)), this, slotName);

    return actionGroup;
  };

  auto addAction = [&](const QString &name, const char *slotName) {
    auto *action = new QAction(name, menu);

    connect(action, SIGNAL(triggered()), this, slotName);

    menu->addAction(action);
  };

  //---

#if 0
  auto *selectMenu = addMenu("Select");

  auto *selectActionGroup = addActionGroup(selectMenu, SLOT(selectionBehaviorSlot(QAction *)));

  auto addSelectAction = [&](const QString &name, bool checked) {
    auto *action = new QAction(name, selectMenu);

    action->setCheckable(true);
    action->setChecked  (checked);

    selectActionGroup->addAction(action);
  };

  addSelectAction("Items"  , selectionBehavior() == SelectItems  );
  addSelectAction("Rows"   , selectionBehavior() == SelectRows   );
  addSelectAction("Columns", selectionBehavior() == SelectColumns);

  selectMenu->addActions(selectActionGroup->actions());
#endif

  //---

  auto *exportMenu = addMenu("Export");

  auto *exportActionGroup = addActionGroup(exportMenu, SLOT(exportSlot(QAction *)));

  auto addExportAction = [&](const QString &name) {
    auto *action = new QAction(name, exportMenu);

    exportActionGroup->addAction(action);
  };

  addExportAction("CSV");
  addExportAction("TSV");
  addExportAction("JSON");

  exportMenu->addActions(exportActionGroup->actions());

  //---

  addAction("Edit", SLOT(editSlot()));
}

void
CQChartsModelView::
setModelP(const ModelP &model)
{
  if (sm_)
    disconnect(sm_, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
               this, SLOT(selectionSlot()));

  //---

  model_ = model;

  CQModelView::setModel(model_.data());

  resetModelData();

  //--

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
CQChartsModelView::
setFilterAnd(bool b)
{
  auto *modelFilter = qobject_cast<CQChartsModelFilter *>(model_.data());

  if (modelFilter)
    modelFilter->setFilterCombine(b ? CQChartsModelFilter::Combine::AND :
                                      CQChartsModelFilter::Combine::OR);
}

void
CQChartsModelView::
setFilter(const QString &filter)
{
  addReplaceFilter(filter, /*add*/false);
}

void
CQChartsModelView::
addFilter(const QString &filter)
{
  addReplaceFilter(filter, /*add*/true);
}

void
CQChartsModelView::
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
    int     column { -1 };

    if (CQChartsModelUtil::decodeModelFilterStr(model, filter, filter1, column))
      proxyModel->setFilterKeyColumn(column);

    proxyModel->setFilterWildcard(filter1);
  }

  emit filterChanged();
}

QString
CQChartsModelView::
filterDetails() const
{
  auto *modelFilter = qobject_cast<CQChartsModelFilter *>(model_.data());

  if (modelFilter)
    return modelFilter->filterDetails();

  return "";
}

void
CQChartsModelView::
setSearch(const QString &text)
{
  addReplaceSearch(text, /*add*/false);
}

void
CQChartsModelView::
addSearch(const QString &text)
{
  addReplaceSearch(text, /*add*/true);
}

void
CQChartsModelView::
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
      RowVisitor(CQChartsModelView *view, const QString &text, int column, Rows &rows) :
       view_(view), regexp_(text), column_(column), rows_(rows) {
      }

      CQChartsModelView *view() const { return view_; }

      State visit(const QAbstractItemModel *model, const VisitData &data) override {
        QModelIndex ind = model->index(data.row, column_, data.parent);

        bool ok;

        QString str = CQChartsModelUtil::modelString(model, ind, ok);
        if (! ok) return State::SKIP;

        if (regexp_.match(str))
          rows_.push_back(ind);

        return State::OK;
      }

     private:
      CQChartsModelView* view_   { nullptr };
      CQChartsRegExp     regexp_;
      int                column_ { 0 };
      Rows&              rows_;
    };

    RowVisitor visitor(this, text1, keyColumn, rows);

    (void) CQChartsModelVisit::exec(charts_, model_.data(), visitor);
  }
  else {
    if (! match_)
      match_ = new CQChartsModelExprMatch;

    if (modelData_) {
      match_->setModelData(modelData_);
    }
    else {
      auto *model = proxyModel->sourceModel();

      match_->setModel(model);
    }

    match_->initColumns();

    class RowVisitor : public CQChartsModelVisitor {
     public:
      RowVisitor(CQChartsModelView *view, CQChartsModelExprMatch *match, const Matches &matches,
                 int column, Rows &rows) :
       view_(view), match_(match), matches_(matches), column_(column), rows_(rows) {
      }

      CQChartsModelView *view() const { return view_; }

      State visit(const QAbstractItemModel *model, const VisitData &data) override {
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
      CQChartsModelView*      view_   { nullptr };
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

  auto *sm = this->selectionModel();

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
CQChartsModelView::
headerClickedSlot(int section)
{
  emit columnClicked(section);
}

void
CQChartsModelView::
itemClickedSlot(const QModelIndex &index)
{
  delegate_->click(index);
}

void
CQChartsModelView::
selectionSlot()
{
  QModelIndexList indices = selectedIndexes();
  if (indices.empty()) return;

  scrollTo(indices.at(0), QAbstractItemView::EnsureVisible);

  emit selectionHasChanged();
}

void
CQChartsModelView::
scrollTo(const QModelIndex &index, ScrollHint hint)
{
  if (hint == QAbstractItemView::EnsureVisible)
    return;

  CQModelView::scrollTo(index, hint);
}

void
CQChartsModelView::
selectionBehaviorSlot(QAction *action)
{
  Q_UNUSED(action)

#if 0
  if      (action->text() == "Items")
    setSelectionBehavior(SelectItems);
  else if (action->text() == "Rows")
    setSelectionBehavior(SelectRows);
  else if (action->text() == "Columns")
    setSelectionBehavior(SelectColumns);
  else
    assert(false);
#endif
}

void
CQChartsModelView::
exportSlot(QAction *action)
{
  QString type = action->text();

  bool hheader = true;
  bool vheader = false;

  QString dir = QDir::current().dirName();
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

  QString fileName = QFileDialog::getSaveFileName(this, "Export Model", dir, pattern);
  if (! fileName.length()) return; // cancelled

  auto *modelData = getModelData();

  if      (type == "CSV")
    modelData->exportModel(fileName, CQBaseModelDataType::CSV, hheader, vheader);
  else if (type == "TSV")
    modelData->exportModel(fileName, CQBaseModelDataType::TSV, hheader, vheader);
  else if (type == "JSON")
    modelData->exportModel(fileName, CQBaseModelDataType::JSON, hheader, vheader);
  else
    assert(false);
}

void
CQChartsModelView::
editSlot()
{
  auto *modelData = getModelData();

  if (modelData)
    charts_->editModelDlg(modelData);
}

CQChartsModelData *
CQChartsModelView::
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
CQChartsModelView::
getDetails()
{
  auto *modelData = getModelData();

  return (modelData ? modelData->details() : nullptr);
}

void
CQChartsModelView::
resetModelData()
{
  if (modelData_)
    disconnect(modelData_, SIGNAL(modelChanged()), this, SLOT(resetModelData()));

  modelData_ = nullptr;

  delegate_->resetColumnData();
}

QSize
CQChartsModelView::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("X")*40, 20*fm.height());
}
