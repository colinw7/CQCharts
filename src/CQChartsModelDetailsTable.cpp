#include <CQChartsModelDetailsTable.h>
#include <CQCharts.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>

#include <CQTableWidget.h>
#include <QVBoxLayout>
#include <QContextMenuEvent>
#include <QMenu>

// TODO: create model and use CQChartsModelView/CQModelView

CQChartsModelDetailsTable::
CQChartsModelDetailsTable(CQChartsModelData *modelData) :
 QFrame(nullptr)
{
  setObjectName("detailsTable");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  table_ = CQUtil::makeWidget<CQTableWidget>("table");

  table_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  table_->setSelectionMode(QAbstractItemView::ExtendedSelection);
  table_->setSelectionBehavior(QAbstractItemView::SelectItems);
  table_->setAlternatingRowColors(true);

  layout->addWidget(table_);

  setModelData(modelData);

  setContextMenuPolicy(Qt::DefaultContextMenu);
}

void
CQChartsModelDetailsTable::
setModelData(CQChartsModelData *modelData)
{
  if (modelData != modelData_) {
    if (modelData_) {
      auto *charts = modelData_->charts();

      disconnect(modelData_, SIGNAL(modelChanged()), this, SLOT(modelChangedSlot()));
      disconnect(charts, SIGNAL(modelTypeChanged(int)), this, SLOT(modelTypeChangedSlot(int)));
    }

    modelData_ = modelData;

    if (modelData_) {
      auto *charts = modelData_->charts();

      connect(modelData_, SIGNAL(modelChanged()), this, SLOT(modelChangedSlot()));
      connect(charts, SIGNAL(modelTypeChanged(int)), this, SLOT(modelTypeChangedSlot(int)));
    }

    if (isAutoUpdate())
      update();
  }
}

void
CQChartsModelDetailsTable::
setFlip(bool b)
{
  if (b != flip_) {
    flip_ = b;

    checkedUpdate(/*force*/true);
  }
}

void
CQChartsModelDetailsTable::
modelChangedSlot()
{
  checkedUpdate();
}

void
CQChartsModelDetailsTable::
modelTypeChangedSlot(int modelInd)
{
  if (modelData_ && modelData_->ind() == modelInd)
    checkedUpdate();
}

void
CQChartsModelDetailsTable::
checkedUpdate(bool force)
{
  if (force || isAutoUpdate())
    update();
  else {
    valid_ = false;

    table_->clear();
  }
}

void
CQChartsModelDetailsTable::
update()
{
  valid_ = true;

  //---

  table_->clear();

  //---

  if (! modelData_)
    return;

  auto *details = modelData_->details();

  int  nc     = details->numColumns    ();
//int  nr     = details->numRows       ();
//bool isHier = details->isHierarchical();

  //---

  table_->setSortingEnabled(false);

  QStringList valueNames;

  if (! isFlip())
    valueNames << "Column Index" << "Column Name";

  valueNames << (QStringList() <<
    "Type" << "Min" << "Max" << "Mean" << "StdDev" << "Monotonic" << "Num Unique" << "Num Null");

  int nv = valueNames.size();

  QStringList columnNames;

  if (isFlip()) {
    columnNames << "Value";

    for (int c = 0; c < nc; ++c) {
      const auto *columnDetails = details->columnDetails(CQChartsColumn(c));

      columnNames << columnDetails->headerName();
    }
  }

  if (! isFlip()) {
    table_->setColumnCount(nv);
    table_->setHorizontalHeaderLabels(valueNames);
    table_->setRowCount(nc);
  }
  else {
    table_->setColumnCount(columnNames.length());
    table_->setHorizontalHeaderLabels(columnNames);
    table_->setRowCount(nv);
  }

  auto columnDetails = [&](int c, QString &nameStr, QString &typeStr, QString &minStr,
                           QString &maxStr, QString &meanStr, QString &stdDevStr,
                           QString &monoStr, QString &uniqueStr, QString &nullStr) {
    const auto *columnDetails = details->columnDetails(CQChartsColumn(c));

    nameStr   = columnDetails->headerName();
    typeStr   = columnDetails->typeName();
    minStr    = columnDetails->dataName(columnDetails->minValue   ()).toString();
    maxStr    = columnDetails->dataName(columnDetails->maxValue   ()).toString();
    meanStr   = columnDetails->dataName(columnDetails->meanValue  ()).toString();
    stdDevStr = columnDetails->dataName(columnDetails->stdDevValue()).toString();

    if (columnDetails->isMonotonic())
      monoStr = (columnDetails->isIncreasing() ? "Increasing" : "Decreasing");
    else
      monoStr.clear();

    uniqueStr = QString::number(columnDetails->numUnique());
    nullStr   = QString::number(columnDetails->numNull());
  };

  auto addWidgetItem = [&](const QString &name, int r, int c) {
    auto *item = new QTableWidgetItem(name);

    item->setToolTip(name);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    table_->setItem(r, c, item);

    return item;
  };

  auto setTableRow = [&](int c) {
    QString nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr, monoStr, uniqueStr, nullStr;

    if (! isFlip()) {
      columnDetails(c, nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr,
                    monoStr, uniqueStr, nullStr);

      auto cstr = QString::number(c);

      int ic = 0;

      addWidgetItem(cstr     , c, ic); ++ic;
      addWidgetItem(nameStr  , c, ic); ++ic;
      addWidgetItem(typeStr  , c, ic); ++ic;
      addWidgetItem(minStr   , c, ic); ++ic;
      addWidgetItem(maxStr   , c, ic); ++ic;
      addWidgetItem(meanStr  , c, ic); ++ic;
      addWidgetItem(stdDevStr, c, ic); ++ic;
      addWidgetItem(monoStr  , c, ic); ++ic;
      addWidgetItem(uniqueStr, c, ic); ++ic;
      addWidgetItem(nullStr  , c, ic); ++ic;
    }
    else {
      if (c == 0) {
        int r = 0;

        for (const auto &v : valueNames)
          addWidgetItem(v, r++, c);
      }
      else {
        columnDetails(c - 1, nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr,
                      monoStr, uniqueStr, nullStr);

        addWidgetItem(typeStr  , 0, c);
        addWidgetItem(minStr   , 1, c);
        addWidgetItem(maxStr   , 2, c);
        addWidgetItem(meanStr  , 3, c);
        addWidgetItem(stdDevStr, 4, c);
        addWidgetItem(monoStr  , 5, c);
        addWidgetItem(uniqueStr, 6, c);
        addWidgetItem(nullStr  , 7, c);
      }
    }
  };

  //---

  if (! isFlip()) {
    for (int c = 0; c < nc; ++c)
      setTableRow(c); // per column stat as row
  }
  else {
    setTableRow(0); // value names

    for (int c = 0; c < nc; ++c)
      setTableRow(c + 1); // stats per column as row
  }

  //---

  table_->fitAll();

  //---

  table_->setSortingEnabled(true);
}

void
CQChartsModelDetailsTable::
contextMenuEvent(QContextMenuEvent *e)
{
  auto *menu = new QMenu;

  //---

  auto *flipAction = menu->addAction("Flip");

  flipAction->setCheckable(true);
  flipAction->setChecked  (isFlip());

  connect(flipAction, SIGNAL(triggered(bool)), this, SLOT(flipSlot(bool)));

  //---

  (void) menu->exec(e->globalPos());

  delete menu;
}

void
CQChartsModelDetailsTable::
flipSlot(bool b)
{
  setFlip(b);
}
