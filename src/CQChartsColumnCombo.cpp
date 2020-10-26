#include <CQChartsColumnCombo.h>
#include <CQChartsModelData.h>
#include <CQChartsVariant.h>

CQChartsColumnCombo::
CQChartsColumnCombo(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("columnCombo");

  setToolTip("Column Name");

  connect(this, SIGNAL(currentIndexChanged(int)),
          this, SIGNAL(columnChanged()));

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
  auto var = itemData(currentIndex());

  bool ok;

  long icolumn = CQChartsVariant::toInt(var, ok);

  if (icolumn < 0)
    icolumn = -1;

  return CQChartsColumn(int(icolumn));
}

void
CQChartsColumnCombo::
setColumn(const CQChartsColumn &column)
{
  int icolumn = column.column();

  int ind = findData(QVariant(icolumn));

  if (ind > 0)
    setCurrentIndex(ind);
  else
    setCurrentIndex(0);
}

void
CQChartsColumnCombo::
setModelData(CQChartsModelData *modelData)
{
  if (modelData == modelData_)
    return;

  if (modelData_) {
    disconnect(modelData_, SIGNAL(dataChanged()), this, SLOT(updateItems()));
    disconnect(modelData_, SIGNAL(modelChanged()), this, SLOT(updateItems()));
    disconnect(modelData_, SIGNAL(currentModelChanged()), this, SLOT(updateItems()));
  }

  modelData_ = modelData;

  if (modelData_) {
    connect(modelData_, SIGNAL(dataChanged()), this, SLOT(updateItems()));
    connect(modelData_, SIGNAL(modelChanged()), this, SLOT(updateItems()));
    connect(modelData_, SIGNAL(currentModelChanged()), this, SLOT(updateItems()));
  }

  updateItems();
}

void
CQChartsColumnCombo::
updateItems()
{
  clear();

  if (isAllowNone())
    addItem("<none>", -1);

  if (! modelData_)
    return;

  int icolumn = modelData_->currentColumn();

  auto *model = modelData_->currentModel().data();

  int nc = model->columnCount();

  for (int c = 0; c < nc; ++c) {
    auto name = model->headerData(c, Qt::Horizontal).toString();

    QString label;

    if (! name.simplified().length())
      label = QString("%1 : <no name>").arg(c);
    else
      label = QString("%1 : %2").arg(c).arg(name);

    addItem(label, QVariant(c));
  }

  int ind = findData(icolumn);

  if (ind >= 0)
    setCurrentIndex(ind);
}
