#include <CQChartsModelDetailsWidget.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQCharts.h>

#include <CQTableWidget.h>
#include <CQUtil.h>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QCheckBox>
#include <QLabel>

CQChartsModelDetailsWidget::
CQChartsModelDetailsWidget(CQCharts *charts) :
 charts_(charts)
{
  setObjectName("modelDetailsWidget");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //---

  // update button
  auto *controlLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  modelLabel_   = CQUtil::makeLabelWidget<QLabel>("");
  updateButton_ = CQUtil::makeLabelWidget<QPushButton>("Update", "update");

  updateButton_->setToolTip("Update details from model data");

  connect(updateButton_, SIGNAL(clicked()), this, SLOT(updateSlot()));

  controlLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("<b>Model:</b> "));
  controlLayout->addWidget(modelLabel_);
  controlLayout->addStretch(1);
  controlLayout->addWidget(updateButton_);

  layout->addLayout(controlLayout);

  //---

  // sumary labels

  auto *summaryLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  numColumnsLabel_ = CQUtil::makeLabelWidget<QLabel>("0");
  numRowsLabel_    = CQUtil::makeLabelWidget<QLabel>("0");
  hierLabel_       = CQUtil::makeLabelWidget<QLabel>("No");

  flipCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Flip");

  flipCheck_->setToolTip("Flip orientation of details table");

  connect(flipCheck_, SIGNAL(stateChanged(int)), this, SLOT(flipSlot(int)));

  summaryLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("<b>Columns:</b> "));
  summaryLayout->addWidget(numColumnsLabel_);
  summaryLayout->addWidget(CQUtil::makeLabelWidget<QLabel>(" "));
  summaryLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("<b>Rows:</b> "));
  summaryLayout->addWidget(numRowsLabel_);
  summaryLayout->addWidget(CQUtil::makeLabelWidget<QLabel>("<b>Hier:</b> "));
  summaryLayout->addWidget(hierLabel_);
  summaryLayout->addStretch(1);
  summaryLayout->addWidget(flipCheck_);

  layout->addLayout(summaryLayout);

  //--

  detailsTable_ = CQUtil::makeWidget<CQTableWidget>("detailsText");

  detailsTable_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(detailsTable_);
}

void
CQChartsModelDetailsWidget::
setFlip(bool b)
{
  if (b != flip_) {
    flip_ = b;

    updateSlot();
  }
}

void
CQChartsModelDetailsWidget::
setDetails(const CQChartsModelDetails *details, bool invalidate)
{
  if (details != details_) {
    if (details_)
      disconnect(details_, SIGNAL(detailsReset()), this, SLOT(invalidateSlot()));

    details_ = const_cast<CQChartsModelDetails *>(details);

    detailsTable_->clear();

    if (details_)
      connect(details_, SIGNAL(detailsReset()), this, SLOT(invalidateSlot()));
  }

  if (invalidate)
    this->invalidate();
}

void
CQChartsModelDetailsWidget::
invalidateSlot()
{
  invalidate();
}

void
CQChartsModelDetailsWidget::
invalidate()
{
  updateButton_->setEnabled(details_);
}

void
CQChartsModelDetailsWidget::
updateDetails(const CQChartsModelDetails *details)
{
  setDetails(details);

  updateSlot();
}

void
CQChartsModelDetailsWidget::
flipSlot(int state)
{
  setFlip(state);
}

void
CQChartsModelDetailsWidget::
updateSlot()
{
  assert(details_);

  //---

  modelLabel_->setText(QString("%1").arg(details_->data()->ind()));

  updateButton_->setEnabled(false);

  //---

  int  nc     = details_->numColumns    ();
  int  nr     = details_->numRows       ();
  bool isHier = details_->isHierarchical();

  //---

  detailsTable_->clear();

  QStringList valueNames;

  if (! isFlip())
    valueNames << "Column";

  valueNames << (QStringList() <<
    "Type" << "Min" << "Max" << "Mean" << "StdDev" << "Monotonic" << "Num Unique" << "Num Null");

  int nv = valueNames.size();

  QStringList columnNames;

  if (isFlip()) {
    columnNames << "Value";

    for (int c = 0; c < nc; ++c) {
      const auto *columnDetails = details_->columnDetails(CQChartsColumn(c));

      columnNames << columnDetails->headerName();
    }
  }

  if (! isFlip()) {
    detailsTable_->setColumnCount(nv);
    detailsTable_->setHorizontalHeaderLabels(valueNames);
    detailsTable_->setRowCount(nc);
  }
  else {
    detailsTable_->setColumnCount(columnNames.length());
    detailsTable_->setHorizontalHeaderLabels(columnNames);
    detailsTable_->setRowCount(nv);
  }

  auto columnDetails = [&](int c, QString &nameStr, QString &typeStr, QString &minStr,
                           QString &maxStr, QString &meanStr, QString &stdDevStr,
                           QString &monoStr, QString &uniqueStr, QString &nullStr) {
    const auto *columnDetails = details_->columnDetails(CQChartsColumn(c));

    nameStr   = columnDetails->headerName();
    typeStr   = columnDetails->typeName();
    minStr    = columnDetails->dataName(columnDetails->minValue   ()).toString();
    maxStr    = columnDetails->dataName(columnDetails->maxValue   ()).toString();
    meanStr   = columnDetails->dataName(columnDetails->meanValue  ()).toString();
    stdDevStr = columnDetails->dataName(columnDetails->stdDevValue()).toString();

    if (columnDetails->isMonotonic())
      monoStr = (columnDetails->isIncreasing() ? "Increasing" : "Decreasing");
    else
      monoStr = "";

    uniqueStr = QString("%1").arg(columnDetails->numUnique());
    nullStr   = QString("%1").arg(columnDetails->numNull());
  };

  auto addWidgetItem = [&](const QString &name, int r, int c) {
    QTableWidgetItem *item = new QTableWidgetItem(name);

    item->setToolTip(name);
    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    detailsTable_->setItem(r, c, item);

    return item;
  };

  auto setTableRow = [&](int c) {
    QString nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr, monoStr, uniqueStr, nullStr;

    if (! isFlip()) {
      columnDetails(c, nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr,
                    monoStr, uniqueStr, nullStr);

      addWidgetItem(nameStr  , c, 0);
      addWidgetItem(typeStr  , c, 1);
      addWidgetItem(minStr   , c, 2);
      addWidgetItem(maxStr   , c, 3);
      addWidgetItem(meanStr  , c, 4);
      addWidgetItem(stdDevStr, c, 5);
      addWidgetItem(monoStr  , c, 6);
      addWidgetItem(uniqueStr, c, 7);
      addWidgetItem(nullStr  , c, 8);
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

  numColumnsLabel_->setText(QString("%1").arg(nc));
  numRowsLabel_   ->setText(QString("%1").arg(nr));
  hierLabel_      ->setText(QString("%1").arg(isHier ? "Yes" : "No"));

  //---

  if (! isFlip()) {
    for (int c = 0; c < nc; ++c)
      setTableRow(c);
  }
  else {
    for (int c = 0; c < nc + 1; ++c)
      setTableRow(c);
  }

  //---

  detailsTable_->fitAll();
}
