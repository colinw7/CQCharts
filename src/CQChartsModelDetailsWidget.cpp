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

  auto addWidgetItem = [&](const QString &name, int r, int c) {
    QTableWidgetItem *item = new QTableWidgetItem(name);

    item->setFlags(item->flags() & ~Qt::ItemIsEditable);

    detailsTable_->setItem(r, c, item);

    return item;
  };

  auto setTableRow = [&](int c) {
    QString nameStr, typeStr, minStr, maxStr, meanStr, stdDevStr, monoStr, uniqueStr, nullStr;

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
