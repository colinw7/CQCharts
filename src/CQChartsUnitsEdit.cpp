#include <CQChartsUnitsEdit.h>
#include <CQChartsUtil.h>

CQChartsUnitsEdit::
CQChartsUnitsEdit(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("units");

  addItems(CQChartsUtil::unitNames(/*includeNone*/true));

  connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged()));
}

const CQChartsUnits &
CQChartsUnitsEdit::
units() const
{
  return units_;
}

void
CQChartsUnitsEdit::
setUnits(const CQChartsUnits &units)
{
  units_ = units;

  QString ustr = CQChartsUtil::unitsString(units_);

  setCurrentIndex(findText(ustr, Qt::MatchExactly));

  int ind = currentIndex();

  setToolTip(CQChartsUtil::unitTipNames(/*includeNone*/true)[ind]);
}

void
CQChartsUnitsEdit::
indexChanged()
{
  QString ustr = currentText();

  (void) CQChartsUtil::decodeUnits(ustr, units_, units_);

  emit unitsChanged();
}
