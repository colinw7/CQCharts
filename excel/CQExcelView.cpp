#include <CQExcelView.h>
#include <CQExcelFrame.h>
#include <CQExcelModel.h>
#include <CQExcelDelegate.h>

#include <CQUtil.h>

#include <QSortFilterProxyModel>
#include <QLabel>
#include <QMenu>

namespace CQExcel {

View::
View(Frame *frame, int nr, int nc) :
 CQModelView(frame), frame_(frame), nr_(nr), nc_(nc)
{
  setObjectName("excelView");

  model_ = new Model(this, nr_, nc_);
  proxy_ = new QSortFilterProxyModel;

  proxy_->setObjectName("proxy");
  proxy_->setSourceModel(excelModel());

  CQModelView::setModel(proxy_);

  connect(model_, SIGNAL(currentIndexChanged(const QModelIndex &)),
          this, SLOT(updateCurrentIndex()));

  auto *delegate = new CQExcelDelegate(this);

  setItemDelegate(delegate);

  auto *sm = selectionModel();

  connect(sm, SIGNAL(selectionChanged(const QItemSelection &, const QItemSelection &)),
          this, SLOT(selectionSlot()));

  connect(this, SIGNAL(clicked(const QModelIndex &)),
          this, SLOT(viewClickSlot(const QModelIndex &)));
}

void
View::
setExcelModel(QAbstractItemModel *model)
{
  if (model_)
    disconnect(model_, SIGNAL(currentIndexChanged(const QModelIndex &)),
               this, SLOT(updateCurrentIndex()));

  delete model_;
  delete proxy_;

  auto *proxyModel = qobject_cast<QAbstractProxyModel *>(model);

  Model *excelModel;

  if (proxyModel)
    excelModel = qobject_cast<Model *>(proxyModel->sourceModel());
  else
    excelModel = qobject_cast<Model *>(model);

  assert(excelModel);

  model_ = excelModel;
  proxy_ = proxyModel;

  if (! proxy_) {
    proxy_ = new QSortFilterProxyModel;

    proxy_->setObjectName("proxy");
    proxy_->setSourceModel(excelModel);
  }

  CQModelView::setModel(proxy_);

  if (model_)
    connect(model_, SIGNAL(currentIndexChanged(const QModelIndex &)),
            this, SLOT(updateCurrentIndex()));
}

void
View::
updateCurrentIndex()
{
  viewport()->update();
  update();
}

void
View::
selectionSlot()
{
  Q_EMIT currentSelectionChanged();
}

QModelIndexList
View::
getSelectedIndexes() const
{
  auto indices = this->selectedIndexes();

  QModelIndexList inds;

  for (const auto &ind : indices) {
    inds.push_back(proxy_->mapToSource(ind));
  }

  return inds;
}

void
View::
getSelectionDetails(QString &cellName)
{
  auto *model = this->excelModel();

  auto indices = this->getSelectedIndexes();

  Model::IndicesData indicesData;

  model->setIndicesData(indices, indicesData);

  // one row, multiple columns
  if      (indicesData.rowCols.size() == 1) {
    int row  = indicesData.rowCols.begin()->first;
    int col1 = *indicesData.rowCols[row].begin();
    int col2 = *indicesData.rowCols[row].rbegin();

    auto cellName1 = model->cellName(row, col1);
    auto cellName2 = model->cellName(row, col2);

    if (col1 != col2)
      cellName = QString("%1:%2").arg(cellName1).arg(cellName2);
    else
      cellName = cellName1;
  }
  // one column, multiple rows
  else if (indicesData.colRows.size() == 1) {
    int col  = indicesData.colRows.begin()->first;
    int row1 = *indicesData.colRows[col].begin();
    int row2 = *indicesData.colRows[col].rbegin();

    auto cellName1 = model->cellName(row1, col);
    auto cellName2 = model->cellName(row2, col);

    if (row1 != row2)
      cellName = QString("%1:%2").arg(cellName1).arg(cellName2);
    else
      cellName = cellName1;
  }
  // multiple rows and columns
  else {
    int row1 = -1;
    int col1 = -1;
    int row2 = -1;
    int col2 = -1;

    for (const auto &pr : indicesData.rowCols) {
      int row = pr.first;

      if (row1 < 0 || row < row1) row1 = row;
      if (row2 < 0 || row > row2) row2 = row;

      for (const auto &col : pr.second) {
        if (col1 < 0 || col < col1) col1 = col;
        if (col2 < 0 || col > col2) col2 = col;
      }

      auto cellName1 = model->cellName(row1, col1);
      auto cellName2 = model->cellName(row2, col2);

      if (cellName1 != cellName2)
        cellName = QString("%1:%2").arg(cellName1).arg(cellName2);
      else
        cellName = cellName1;
    }
  }
}

void
View::
viewClickSlot(const QModelIndex &ind)
{
  setCurrentIndex(proxy_->mapToSource(ind));
}

void
View::
applyFunction(const QString &text)
{
  auto indices = getSelectedIndexes();

  auto *model = this->excelModel();

  if (indices.length()) {
    for (const auto &ind : indices)
      model->setData(ind, text, Qt::EditRole);
  }
  else {
    if (currentIndex_.isValid())
      model->setData(currentIndex_, text, Qt::EditRole);
  }
}

void
View::
setCurrentIndex(const QModelIndex &ind)
{
  currentIndex_ = ind;

  Q_EMIT currentIndexChanged();
}

void
View::
getIndexDetails(const QModelIndex &ind, QString &cellName, QString &functionText)
{
  auto *model = this->excelModel();

  cellName = model->cellName(ind.row(), ind.column());

  functionText = model->cellExpression(ind);

  if (functionText == "") {
    auto var = model->data(ind, Qt::DisplayRole);

    functionText = var.toString();
  }
}

void
View::
addMenuActions(QMenu *menu)
{
  auto addMenu = [&](const QString &name) {
    auto *subMenu = CQUtil::makeLabelWidget<QMenu>(name, "menu");

    menu->addMenu(subMenu);

    return subMenu;
  };

  auto addAction = [&](QMenu *menu, const QString &name, const char *slotName=nullptr) {
    auto *action = menu->addAction(name);

    if (slotName)
      connect(action, SIGNAL(triggered()), this, slotName);

    return action;
  };

  addAction(menu, "Add Row"   , SLOT(addRowSlot()));
  addAction(menu, "Add Column", SLOT(addColumnSlot()));

  addAction(menu, "Set Current", SLOT(setCurrentlot()));

  addAction(menu, "Copy" , SLOT(copySlot()));
  addAction(menu, "Paste", SLOT(pasteSlot()));

  auto *functionsMenu = addMenu("Functions");

  addAction(functionsMenu, "Sum"    , SLOT(sumSlot()));
  addAction(functionsMenu, "Average", SLOT(averageSlot()));
  addAction(functionsMenu, "Min"    , SLOT(minSlot()));
  addAction(functionsMenu, "Max"    , SLOT(maxSlot()));

  auto *styleMenu = addMenu("Style");

  addAction(styleMenu, "Bad"  , SLOT(badStyleSlot()));
  addAction(styleMenu, "Error", SLOT(errorStyleSlot()));

  menu->addSeparator();

  CQModelView::addMenuActions(menu);
}

void
View::
addRowSlot()
{
  auto *model = this->excelModel();

  model->addRow();
}

void
View::
addColumnSlot()
{
  auto *model = this->excelModel();

  model->addColumn();
}

void
View::
setCurrentlot()
{
  auto *model = this->excelModel();

  model->setCurrentIndex(currentIndex());
}

void
View::
copySlot()
{
  copyIndices_ = this->getSelectedIndexes();
}

void
View::
pasteSlot()
{
  if (! currentIndex().isValid())
    return;

  if (copyIndices_.length() == 0)
    return;

  auto *model = this->excelModel();

  Model::IndicesData indicesData;

  model->setIndicesData(copyIndices_, indicesData);

  int row = currentIndex().row();
  int col = currentIndex().column();

  // one row, multiple columns
  if      (indicesData.rowCols.size() == 1) {
    for (const auto &ind : copyIndices_) {
      auto var = model->data(ind, Qt::DisplayRole);

      auto ind1 = model->index(row, col, QModelIndex());

      if (ind1.isValid())
        model->setData(ind1, var, Qt::DisplayRole);

      ++col;
    }
  }
  // one column, multiple rows
  else if (indicesData.colRows.size() == 1) {
    for (const auto &ind : copyIndices_) {
      auto var = model->data(ind, Qt::DisplayRole);

      auto ind1 = model->index(row, col, QModelIndex());

      if (ind1.isValid())
        model->setData(ind1, var, Qt::DisplayRole);

      ++row;
    }
  }
}

void
View::
sumSlot()
{
  addRangeFunction("sum");
}

void
View::
averageSlot()
{
  addRangeFunction("average");
}

void
View::
minSlot()
{
  addRangeFunction("min");
}

void
View::
maxSlot()
{
  addRangeFunction("max");
}

void
View::
addRangeFunction(const QString &fnName)
{
  auto indices = this->getSelectedIndexes();

  if (indices.length() == 0)
    return;

  auto *model = this->excelModel();

  Model::IndicesData indicesData;

  model->setIndicesData(indices, indicesData);

  // one row, multiple columns
  if      (indicesData.rowCols.size() == 1) {
    int row  = indicesData.rowCols.begin()->first;
    int col1 = *indicesData.rowCols[row].begin();
    int col2 = *indicesData.rowCols[row].rbegin();

    if (col1 == col2) return;

    auto cellName1 = model->cellName(row, col1);
    auto cellName2 = model->cellName(row, col2);

    model->addColumn();

    int nc = model->columnCount();

    auto ind = model->index(row, nc - 1, QModelIndex());

    auto cmd = QString("=%1(\"%2:%3\")").arg(fnName).arg(cellName1).arg(cellName2);

    model->setData(ind, cmd, Qt::DisplayRole);
  }
  // one column, multiple rows
  else if (indicesData.colRows.size() == 1) {
    int col  = indicesData.colRows.begin()->first;
    int row1 = *indicesData.colRows[col].begin();
    int row2 = *indicesData.colRows[col].rbegin();

    if (row1 == row2) return;

    auto cellName1 = model->cellName(row1, col);
    auto cellName2 = model->cellName(row2, col);

    model->addRow();

    int nr = model->rowCount();

    auto ind = model->index(nr - 1, col, QModelIndex());

    auto cmd = QString("=%1(\"%2:%3\")").arg(fnName).arg(cellName1).arg(cellName2);

    model->setData(ind, cmd, Qt::DisplayRole);
  }
}

void
View::
badStyleSlot()
{
  auto *model = this->excelModel();

  auto indices = this->getSelectedIndexes();

  for (const auto &ind : indices) {
    Model::Style style;

    if (model->hasCellStyle(ind))
      style = model->cellStyle(ind);

    style.brush = palette().window(); // bg

    style.pen.setColor(Qt::red); // fg

    model->setCellStyle(ind, style);
  }
}

void
View::
errorStyleSlot()
{
  auto *model = this->excelModel();

  auto indices = this->getSelectedIndexes();

  for (const auto &ind : indices) {
    Model::Style style;

    if (model->hasCellStyle(ind))
      style = model->cellStyle(ind);

    style.brush.setColor(Qt::red); // bg

    style.pen = palette().text(); // fg

    model->setCellStyle(ind, style);
  }
}

}
