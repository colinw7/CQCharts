#include <CQChartsUnitsEdit.h>
#include <CQChartsUtil.h>

CQChartsUnitsEdit::
CQChartsUnitsEdit(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("units");

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  QStringList unitNames = CQChartsUtil::unitNames   (/*includeNone*/true);
  QStringList tipNames  = CQChartsUtil::unitTipNames(/*includeNone*/true);

  addItems(unitNames);

  assert(unitNames.size() == tipNames.size());

  for (int i = 0; i < count(); ++i)
    setItemData(i, tipNames[i], Qt::ToolTipRole);

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

  (void) CQChartsUtil::decodeUnits(ustr, units_, /*default*/units_);

  updateTip();

  emit unitsChanged();
}

void
CQChartsUnitsEdit::
updateTip()
{
  int ind = std::max(currentIndex(), 0);

  setToolTip(CQChartsUtil::unitTipNames(/*includeNone*/true)[ind]);
}

QSize
CQChartsUnitsEdit::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("none") + 8, fm.height() + 4);
}
