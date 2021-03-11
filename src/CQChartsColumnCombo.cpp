#include <CQChartsColumnCombo.h>
#include <CQChartsModelData.h>
#include <CQChartsModelDetails.h>
#include <CQChartsVariant.h>
#include <CQChartsWidgetUtil.h>

CQChartsColumnCombo::
CQChartsColumnCombo(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("columnCombo");

  setToolTip("Column Name");

  connect(this, SIGNAL(currentIndexChanged(int)), this, SIGNAL(columnChanged()));

  setModelData(nullptr);
}

void
CQChartsColumnCombo::
setAllowNone(bool b)
{
  if (b == allowNone_)
    return;

  allowNone_ = b;

  updateItems();
}

CQChartsColumn
CQChartsColumnCombo::
getColumn() const
{
  CQChartsColumn c;

  if (! modelData_)
    return c;

  auto var = itemData(currentIndex());

  bool ok;

  long icolumn = CQChartsVariant::toInt(var, ok);

  if (! ok || icolumn < 0)
    icolumn = -1;

  c = CQChartsColumn(int(icolumn));

  return c;
}

void
CQChartsColumnCombo::
setColumn(const CQChartsColumn &column)
{
  assert(modelData_);

  int icolumn = column.column();

  int ind = findData(QVariant(icolumn));

  if (ind > 0)
    setCurrentIndex(ind);
  else
    setCurrentIndex(0);
}

void
CQChartsColumnCombo::
setModelData(const CQChartsModelData *modelData)
{
  if (modelData == modelData_)
    return;

  connectSlots(false);

  modelData_ = modelData;

  connectSlots(true);

  updateItems();
}

void
CQChartsColumnCombo::
connectSlots(bool b)
{
  if (modelData_) {
    auto *modelData = const_cast<CQChartsModelData *>(modelData_);

    CQChartsWidgetUtil::connectDisconnect(b,
      modelData, SIGNAL(dataChanged()), this, SLOT(updateItems()));
    CQChartsWidgetUtil::connectDisconnect(b,
      modelData, SIGNAL(modelChanged()), this, SLOT(updateItems()));
    CQChartsWidgetUtil::connectDisconnect(b,
      modelData, SIGNAL(currentModelChanged()), this, SLOT(updateItems()));
  }
}

void
CQChartsColumnCombo::
setModelColumn(CQChartsModelData *modelData, const Column &column)
{
  setModelData(modelData);

  setColumn(column);
}

void
CQChartsColumnCombo::
setNumericOnly(bool b)
{
  numericOnly_ = b;

  updateItems();
}

void
CQChartsColumnCombo::
updateItems()
{
  if (! modelData_) {
    clear();
    return;
  }

  int icolumn = getColumn().column();

  if (icolumn < 0)
    icolumn = modelData_->currentColumn();

  auto *model = modelData_->currentModel().data();

  int nc = model->columnCount();

  const CQChartsModelDetails *details = nullptr;

  if (isNumericOnly())
    details = modelData_->details();

  clear();

  if (isAllowNone())
    addItem("<none>", -1);

  for (int c = 0; c < nc; ++c) {
    if (isNumericOnly()) {
      const auto *columnDetails = (details ? details->columnDetails(Column(c)) : nullptr);

      if (columnDetails && ! columnDetails->isNumeric())
        continue;
    }

    auto name = model->headerData(c, Qt::Horizontal).toString();

    QString label;

    if (! name.trimmed().length())
      label = QString("%1 : <no name>").arg(c);
    else
      label = QString("%1 : %2").arg(c).arg(name);

    addItem(label, QVariant(c));
  }

  int ind = findData(icolumn);

  if (ind >= 0)
    setCurrentIndex(ind);
}
