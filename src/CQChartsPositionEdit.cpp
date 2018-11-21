#include <CQChartsPositionEdit.h>
#include <CQPoint2DEdit.h>
#include <QComboBox>
#include <QHBoxLayout>

CQChartsPositionEdit::
CQChartsPositionEdit(QWidget *parent) :
 QFrame(parent)
{
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  pointEdit_  = new CQPoint2DEdit;
  unitsCombo_ = new QComboBox;

  unitsCombo_->addItems(QStringList() << "px" << "%" << "P" << "V");

  pointEdit_ ->setObjectName("point");
  unitsCombo_->setObjectName("units");

  layout->addWidget(pointEdit_);
  layout->addWidget(unitsCombo_);
}

void
CQChartsPositionEdit::
setPosition(const CQChartsPosition &pos)
{
  const QPointF                 &p     = pos.p();
  const CQChartsPosition::Units &units = pos.units();

  QString ustr = CQChartsPosition::unitsString(units);

  pointEdit_->setValue(CPoint2D(p.x(), p.y()));

  unitsCombo_->setCurrentIndex(unitsCombo_->findText(ustr, Qt::MatchExactly));
}

CQChartsPosition
CQChartsPositionEdit::
position() const
{
  CPoint2D p = pointEdit_->getValue();

  QString str = unitsCombo_->currentText();

  CQChartsPosition::Units units;

  CQChartsPosition::decodeUnits(str, units);

  return CQChartsPosition(QPointF(p.x, p.y), units);
}
