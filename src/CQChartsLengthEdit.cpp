#include <CQChartsLengthEdit.h>
#include <CQRealSpin.h>
#include <QComboBox>
#include <QHBoxLayout>

CQChartsLengthEdit::
CQChartsLengthEdit(QWidget *parent) :
 QFrame(parent)
{
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  realEdit_   = new CQRealSpin;
  unitsCombo_ = new QComboBox;

  unitsCombo_->addItems(QStringList() << "px" << "%" << "P" << "V");

  realEdit_  ->setObjectName("real");
  unitsCombo_->setObjectName("units");

  layout->addWidget(realEdit_);
  layout->addWidget(unitsCombo_);
}

void
CQChartsLengthEdit::
setLength(const CQChartsLength &pos)
{
  double                       value = pos.value();
  const CQChartsLength::Units &units = pos.units();

  QString ustr = CQChartsLength::unitsString(units);

  realEdit_->setValue(value);

  unitsCombo_->setCurrentIndex(unitsCombo_->findText(ustr, Qt::MatchExactly));
}

CQChartsLength
CQChartsLengthEdit::
length() const
{
  double value = realEdit_->value();

  QString str = unitsCombo_->currentText();

  CQChartsLength::Units units;

  CQChartsLength::decodeUnits(str, units);

  return CQChartsLength(value, units);
}
