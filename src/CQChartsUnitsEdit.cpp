#include <CQChartsUnitsEdit.h>
#include <CQChartsUtil.h>

CQChartsUnitsEdit::
CQChartsUnitsEdit(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("units");

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

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

QSize
CQChartsUnitsEdit::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("none") + 8, fm.height() + 4);
}
