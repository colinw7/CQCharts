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
CQChartsModelDetailsTable(ModelData *modelData) :
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
setCharts(CQCharts *charts)
{
  charts_ = charts;
}

int
CQChartsModelDetailsTable::
modelInd() const
{
  return (modelData() ? modelData()->ind() : -1);
}

void
CQChartsModelDetailsTable::
setModelInd(int ind)
{
  auto *modelData = (charts_ ? charts_->getModelDataByInd(ind) : nullptr);
  if (! modelData) return;

  setModelData(modelData);
}

void
CQChartsModelDetailsTable::
setModelData(ModelData *modelData)
{
  if (modelData != modelData_) {
    if (modelData_) {
      disconnect(modelData_, SIGNAL(deleted()), this, SLOT(modelDeletedSlot()));
      disconnect(modelData_, SIGNAL(modelChanged()), this, SLOT(modelChangedSlot()));
    }

    if (charts_)
      disconnect(charts_, SIGNAL(modelTypeChanged(int)), this, SLOT(modelTypeChangedSlot(int)));

    modelData_ = const_cast<ModelData *>(modelData);
    charts_    = (modelData_ ? modelData_->charts() : nullptr);

    if (modelData_) {
      connect(modelData_, SIGNAL(deleted()), this, SLOT(modelDeletedSlot()));
      connect(modelData_, SIGNAL(modelChanged()), this, SLOT(modelChangedSlot()));
    }

    if (charts_)
      connect(charts_, SIGNAL(modelTypeChanged(int)), this, SLOT(modelTypeChangedSlot(int)));

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
modelDeletedSlot()
{
  setModelData(nullptr);
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
  if (modelData() && modelData()->isInd(modelInd))
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

  auto *details = (modelData() ? modelData()->details() : nullptr);
  if (! details) return;

  int  nc     = details->numColumns    ();
//int  nr     = details->numRows       ();
//bool isHier = details->isHierarchical();

  //---

  table_->setSortingEnabled(false);

  QStringList valueNames;

  if (! isFlip())
    valueNames << "Column Index" << "Column Name";

  valueNames << (QStringList() <<
    "Type" << "Min" << "Max" << "Mean" << "StdDev" << "Monotonic" << "Num Unique" << "Num Null" <<
    "Median" << "Lower Median" << "Upper Median");

  auto nv = valueNames.size();

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

  struct ColumnDetailsData {
    QString nameStr;
    QString typeStr;
    QString minStr;
    QString maxStr;
    QString meanStr;
    QString stdDevStr;
    QString monoStr;
    QString uniqueStr;
    QString nullStr;
    QString medianStr;
    QString lowerMedianStr;
    QString upperMedianStr;
  };

  auto columnDetails = [&](int c, ColumnDetailsData &data) {
    const auto *columnDetails = details->columnDetails(CQChartsColumn(c));

    data.nameStr   = columnDetails->headerName();
    data.typeStr   = columnDetails->typeName();
    data.minStr    = columnDetails->dataName(columnDetails->minValue   ()).toString();
    data.maxStr    = columnDetails->dataName(columnDetails->maxValue   ()).toString();
    data.meanStr   = columnDetails->dataName(columnDetails->meanValue  ()).toString();
    data.stdDevStr = columnDetails->dataName(columnDetails->stdDevValue()).toString();

    if (columnDetails->isMonotonic())
      data.monoStr = (columnDetails->isIncreasing() ? "Increasing" : "Decreasing");
    else
      data.monoStr.clear();

    data.uniqueStr = QString::number(columnDetails->numUnique());
    data.nullStr   = QString::number(columnDetails->numNull());

    data.medianStr      = columnDetails->medianValue     ().toString();
    data.lowerMedianStr = columnDetails->lowerMedianValue().toString();
    data.upperMedianStr = columnDetails->upperMedianValue().toString();
  };

  auto addWidgetItem = [&](const QString &name, int r, int c) {
    auto *item = new QTableWidgetItem(name);

    item->setToolTip(name);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    table_->setItem(r, c, item);

    return item;
  };

  auto setTableRow = [&](int c) {
    ColumnDetailsData data;

    if (! isFlip()) {
      columnDetails(c, data);

      auto cstr = QString::number(c);

      int ic = 0;

      addWidgetItem(cstr               , c, ic); ++ic;
      addWidgetItem(data.nameStr       , c, ic); ++ic;
      addWidgetItem(data.typeStr       , c, ic); ++ic;
      addWidgetItem(data.minStr        , c, ic); ++ic;
      addWidgetItem(data.maxStr        , c, ic); ++ic;
      addWidgetItem(data.meanStr       , c, ic); ++ic;
      addWidgetItem(data.stdDevStr     , c, ic); ++ic;
      addWidgetItem(data.monoStr       , c, ic); ++ic;
      addWidgetItem(data.uniqueStr     , c, ic); ++ic;
      addWidgetItem(data.nullStr       , c, ic); ++ic;
      addWidgetItem(data.medianStr     , c, ic); ++ic;
      addWidgetItem(data.lowerMedianStr, c, ic); ++ic;
      addWidgetItem(data.upperMedianStr, c, ic); ++ic;
    }
    else {
      if (c == 0) {
        int r = 0;

        for (const auto &v : valueNames)
          addWidgetItem(v, r++, c);
      }
      else {
        columnDetails(c - 1, data);

        int ic = 0;

        addWidgetItem(data.typeStr       , ic, c); ++ic;
        addWidgetItem(data.minStr        , ic, c); ++ic;
        addWidgetItem(data.maxStr        , ic, c); ++ic;
        addWidgetItem(data.meanStr       , ic, c); ++ic;
        addWidgetItem(data.stdDevStr     , ic, c); ++ic;
        addWidgetItem(data.monoStr       , ic, c); ++ic;
        addWidgetItem(data.uniqueStr     , ic, c); ++ic;
        addWidgetItem(data.nullStr       , ic, c); ++ic;
        addWidgetItem(data.medianStr     , ic, c); ++ic;
        addWidgetItem(data.lowerMedianStr, ic, c); ++ic;
        addWidgetItem(data.upperMedianStr, ic, c); ++ic;
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
