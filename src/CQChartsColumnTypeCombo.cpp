#include <CQChartsColumnTypeCombo.h>

#include <CQCharts.h>
#include <CQChartsColumnType.h>

#include <CQBaseModel.h>

CQChartsColumnTypeCombo::
CQChartsColumnTypeCombo(QWidget *parent, CQCharts *charts) :
 QComboBox(parent), charts_(charts)
{
  setObjectName("columnType");

  init();
}

void
CQChartsColumnTypeCombo::
setCharts(CQCharts *charts)
{
  if (charts != charts_) {
    charts_ = charts;

    init();
  }
}

void
CQChartsColumnTypeCombo::
init()
{
  if (! charts_)
    return;

  // get type names
  auto *columnTypeMgr = charts_->columnTypeMgr();

  QStringList typeNames;

  columnTypeMgr->typeNames(typeNames);

  //---

  clear();

  addItems(typeNames);

  connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChangedSlot(int)));
}

void
CQChartsColumnTypeCombo::
indexChangedSlot(int)
{
  Q_EMIT typeChanged();
}

const CQChartsColumnType *
CQChartsColumnTypeCombo::
columnType() const
{
  if (! charts_)
    return nullptr;

  auto *columnTypeMgr = charts_->columnTypeMgr();

  auto typeStr = this->currentText();

  auto columnType = CQBaseModel::nameType(typeStr);

  return columnTypeMgr->getType(columnType);
}

void
CQChartsColumnTypeCombo::
setColumnType(const CQChartsColumnType *type)
{
  if (type) {
    int typeInd = this->findText(type->name());

    if (typeInd >= 0)
      this->setCurrentIndex(typeInd);
  }
  else
    this->setCurrentIndex(-1);
}
