#include <CQChartsModelDetailsWidget.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQCharts.h>

#include <CQTableWidget.h>
#include <CQUtil.h>

#include <QVBoxLayout>
#include <QPushButton>
#include <QTextEdit>

CQChartsModelDetailsWidget::
CQChartsModelDetailsWidget(CQCharts *charts) :
 charts_(charts)
{
  setObjectName("modelDetailsWidget");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  //---

  QHBoxLayout *controlLayout = CQUtil::makeLayout<QHBoxLayout>(2, 2);

  updateButton_ = CQUtil::makeLabelWidget<QPushButton>("Update", "update");

  connect(updateButton_, SIGNAL(clicked()), this, SLOT(updateSlot()));

  controlLayout->addWidget(updateButton_);
  controlLayout->addStretch(1);

  layout->addLayout(controlLayout);

  //--

  detailsText_ = CQUtil::makeWidget<QTextEdit>("detailsText");

  detailsText_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  detailsText_->setReadOnly(true);

  layout->addWidget(detailsText_);

  //--

  detailsTable_ = CQUtil::makeWidget<CQTableWidget>("detailsText");

  detailsTable_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(detailsTable_);
}

void
CQChartsModelDetailsWidget::
setDetails(const CQChartsModelDetails *details)
{
  if (details_)
    disconnect(details_, SIGNAL(detailsReset()), this, SLOT(invalidateSlot()));

  details_ = const_cast<CQChartsModelDetails *>(details);

  if (details_)
    connect(details_, SIGNAL(detailsReset()), this, SLOT(invalidateSlot()));

  invalidate();
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
updateSlot()
{
  assert(details_);

  //---

  updateButton_->setEnabled(false);

  //---

  int nc = details_->numColumns();
  int nr = details_->numRows   ();

  //---

  detailsTable_->clear();

  QStringList columnNames = (QStringList() <<
    "Column" << "Type" << "Min" << "Max" << "Mean" << "StdDev" <<
    "Monotonic" << "Num Unique" << "Num Null");

  detailsTable_->setColumnCount(columnNames.length());

  detailsTable_->setHorizontalHeaderLabels(columnNames);

  detailsTable_->setRowCount(nc);

  auto columnDetails = [&](int c, QString &nameStr, QString &typeStr, QString &minStr,
                           QString &maxStr, QString &meanStr, QString &stdDevStr,
                           QString &monoStr, QString &uniqueStr, QString &nullStr) {
    const CQChartsModelColumnDetails *columnDetails = details_->columnDetails(c);

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

  auto setTableRow = [&](int c) {
    QString nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr, monoStr, uniqueStr, nullStr;

    columnDetails(c, nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr,
                  monoStr, uniqueStr, nullStr);

    QTableWidgetItem *item1 = new QTableWidgetItem(nameStr);
    QTableWidgetItem *item2 = new QTableWidgetItem(typeStr);
    QTableWidgetItem *item3 = new QTableWidgetItem(minStr);
    QTableWidgetItem *item4 = new QTableWidgetItem(maxStr);
    QTableWidgetItem *item5 = new QTableWidgetItem(meanStr);
    QTableWidgetItem *item6 = new QTableWidgetItem(stdDevStr);
    QTableWidgetItem *item7 = new QTableWidgetItem(monoStr);
    QTableWidgetItem *item8 = new QTableWidgetItem(uniqueStr);
    QTableWidgetItem *item9 = new QTableWidgetItem(nullStr);

    detailsTable_->setItem(c, 0, item1);
    detailsTable_->setItem(c, 1, item2);
    detailsTable_->setItem(c, 2, item3);
    detailsTable_->setItem(c, 3, item4);
    detailsTable_->setItem(c, 4, item5);
    detailsTable_->setItem(c, 5, item6);
    detailsTable_->setItem(c, 6, item7);
    detailsTable_->setItem(c, 7, item8);
    detailsTable_->setItem(c, 8, item9);
  };

  //---

  QString text = "<b></b>";

  text += "<table padding=\"4\">";
  text += QString("<tr><td>Columns</td><td>%1</td></tr>").arg(nc);
  text += QString("<tr><td>Rows</td><td>%1</td></tr>").arg(nr);
  text += "</table>";

  detailsText_->setHtml(text);

  //---

  detailsText_->setFixedHeight(detailsText_->document()->size().height() + 4);

  //---

  for (int c = 0; c < nc; ++c)
    setTableRow(c);
}
