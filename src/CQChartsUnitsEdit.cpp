#include <CQChartsUnitsEdit.h>
#include <CQChartsUtil.h>

CQChartsUnitsEdit::
CQChartsUnitsEdit(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("units");

  setToolTip("Units");

  //---

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  addItems(CQChartsUtil::unitNames(/*includeNone*/true));

  connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged()));

  updateTip();
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

  int ind = findText(ustr, Qt::MatchExactly);

  if (ind < 0)
    ind = 0;

  setCurrentIndex(ind);

  updateTip();
}

void
CQChartsUnitsEdit::
indexChanged()
{
  QString ustr = currentText();

  (void) CQChartsUtil::decodeUnits(ustr, units_, units_);

  updateTip();

  emit unitsChanged();
}

void
CQChartsUnitsEdit::
updateTip()
{
  int ind = std::max(currentIndex(), 0);

  setToolTip(QString("%1 units").arg(CQChartsUtil::unitTipNames(/*includeNone*/true)[ind]));
}

QSize
CQChartsUnitsEdit::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("none") + 8, fm.height() + 4);
}
