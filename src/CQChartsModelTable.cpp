#include <CQChartsModelTable.h>
#include <CQChartsModelData.h>
#include <CQChartsVariant.h>
#include <CQCharts.h>

#include <QHeaderView>

CQChartsModelTable::
CQChartsModelTable(CQCharts *charts) :
 CQTableWidget()
{
  setObjectName("modelTable");

  horizontalHeader()->setStretchLastSection(true);

//setSelectionMode(ExtendedSelection);

  setSelectionBehavior(QAbstractItemView::SelectRows);

  setCharts(charts);
}

void
CQChartsModelTable::
setCharts(CQCharts *charts)
{
  if (charts_) {
    disconnect(charts_, SIGNAL(modelDataChanged()), this, SLOT(updateModels()));
    disconnect(charts_, SIGNAL(modelDataDataChanged()), this, SLOT(updateModels()));
  }

  charts_ = charts;

  if (charts_) {
    updateModels();

    connect(charts_, SIGNAL(modelDataChanged()), this, SLOT(updateModels()));
    connect(charts_, SIGNAL(modelDataDataChanged()), this, SLOT(updateModels()));
  }
}

void
CQChartsModelTable::
updateModels()
{
  headerNames_ = QStringList() << "Name" << "Index" << "Filename" << "Object Name";

  CQCharts::ModelDatas modelDatas;

  charts_->getModelDatas(modelDatas);

  clear();

  setColumnCount(4);
  setRowCount(int(modelDatas.size()));

  for (int i = 0; i < headerNames_.length(); ++i)
    setHorizontalHeaderItem(i, new QTableWidgetItem(headerNames_[i]));

  auto createItem = [&](const QString &name, int r, int c) {
    auto *item = new QTableWidgetItem(name);

    item->setToolTip(name);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    setItem(r, c, item);

    return item;
  };

  int i = 0;

  for (const auto &modelData : modelDatas) {
    auto *nameItem = createItem(modelData->id(), i, 0);

    nameItem->setData(Qt::UserRole, modelData->ind());

    (void) createItem(QString::number(modelData->ind()), i, 1);
    (void) createItem(modelData->filename()            , i, 2);
    (void) createItem(modelData->model()->objectName() , i, 3);

    ++i;
  }
}

int
CQChartsModelTable::
selectedModel() const
{
  auto items = selectedItems();

  for (int i = 0; i < items.length(); ++i) {
    auto *item = items[i];
    if (item->column() != 0) continue;

    bool ok;

    long ind = CQChartsVariant::toInt(item->data(Qt::UserRole), ok);
    if (! ok) return -1;

    return int(ind);
  }

  return -1;
}

QSize
CQChartsModelTable::
sizeHint() const
{
  //auto s = CQTableWidget::sizeHint();

  int nr = rowCount();

  QFontMetrics fm(font());

  int tw = 0;

  for (const auto &name : headerNames_)
    tw += fm.width(name) + 4;

  int rh = fm.height() + 4;
  int th = (nr + 1)*rh;

  return QSize(tw, th);
}
