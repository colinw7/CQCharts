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
setColumn(const CQChartsColumn &column)
{
  int ind = findData(column.column());

  if (ind > 0)
    setCurrentIndex(ind);
  else
    setCurrentIndex(0);
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
setModel(QAbstractItemModel *model)
{
  clear();

  addItem("<none>", -1);

  if (! model)
    return;

  int nc = model->columnCount();

  for (int c = 0; c < nc; ++c) {
    QString name = model->headerData(c, Qt::Horizontal).toString();

    QString label;

    if (! name.simplified().length())
      label = QString("%1 : <no name>").arg(c);
    else
      label = QString("%1 : %2").arg(c).arg(name);

    addItem(label, c);
  }
}
