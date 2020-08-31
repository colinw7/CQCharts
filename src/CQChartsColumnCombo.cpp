#include <CQChartsColumnCombo.h>
#include <CQChartsVariant.h>

CQChartsColumnCombo::
CQChartsColumnCombo(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("columnCombo");

  setToolTip("Column Name");

  connect(this, SIGNAL(currentIndexChanged(int)),
          this, SIGNAL(columnChanged()));

  setModel(nullptr);
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
  QVariant var = itemData(currentIndex());

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
  int ind = findData(column.column());

  if (ind > 0)
    setCurrentIndex(ind);
  else
    setCurrentIndex(0);
}

void
CQChartsColumnCombo::
setModel(QAbstractItemModel *model)
{
  if (model_ == model)
    return;

  model_ = model;

  updateItems();
}

void
CQChartsColumnCombo::
updateItems()
{
  clear();

  if (isAllowNone())
    addItem("<none>", -1);

  if (! model_)
    return;

  int nc = model_->columnCount();

  for (int c = 0; c < nc; ++c) {
    QString name = model_->headerData(c, Qt::Horizontal).toString();

    QString label;

    if (! name.simplified().length())
      label = QString("%1 : <no name>").arg(c);
    else
      label = QString("%1 : %2").arg(c).arg(name);

    addItem(label, c);
  }
}
