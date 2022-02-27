#include <CQChartsUnitsEdit.h>
#include <CQChartsUtil.h>
#include <CQChartsVariant.h>

#include <CQPropertyView.h>

CQChartsUnitsEdit::
CQChartsUnitsEdit(QWidget *parent) :
 QComboBox(parent)
{
  setObjectName("units");

  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  auto unitNames = CQChartsUnits::unitNames   (/*includeNone*/true);
  auto tipNames  = CQChartsUnits::unitTipNames(/*includeNone*/true);

  addItems(unitNames);

  assert(unitNames.size() == tipNames.size());

  for (int i = 0; i < count(); ++i)
    setItemData(i, tipNames[i], Qt::ToolTipRole);

  connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged()));

  updateTip();
}

const CQChartsUnits::Type &
CQChartsUnitsEdit::
units() const
{
  return units_;
}

void
CQChartsUnitsEdit::
setUnits(const Units &units)
{
  units_ = units;

  auto ustr = CQChartsUnits::unitsString(units_);

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
  auto ustr = currentText();

  (void) CQChartsUnits::decodeUnits(ustr, units_, /*default*/units_);

  updateTip();

  emit unitsChanged();
}

void
CQChartsUnitsEdit::
updateTip()
{
  int ind = std::max(currentIndex(), 0);

  setToolTip(CQChartsUnits::unitTipNames(/*includeNone*/true)[ind]);
}

QSize
CQChartsUnitsEdit::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.horizontalAdvance("none") + 8, fm.height() + 4);
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsUnitsPropertyViewType::
CQChartsUnitsPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsUnitsPropertyViewType::
getEditor() const
{
  return new CQChartsUnitsPropertyViewEditor;
}

bool
CQChartsUnitsPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsUnitsPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  bool ok;
  auto units = CQChartsVariant::toUnits(value, ok);
  if (! ok) return;

  auto str = units.toString();

  QFontMetrics fm(option.font);

  int w = fm.horizontalAdvance(str);

  //---

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsUnitsPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;
  auto units = CQChartsVariant::toUnits(value, ok);
  if (! ok) return "";

  return units.toString();
}

//------

CQChartsUnitsPropertyViewEditor::
CQChartsUnitsPropertyViewEditor()
{
}

QWidget *
CQChartsUnitsPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsUnitsEdit(parent);

  return edit;
}

void
CQChartsUnitsPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsUnitsEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(unitsChanged()), obj, method);
}

QVariant
CQChartsUnitsPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsUnitsEdit *>(w);
  assert(edit);

  return CQChartsVariant::fromUnits(CQChartsUnits(edit->units()));
}

void
CQChartsUnitsPropertyViewEditor::
setValue(QWidget *w, const QVariant &value)
{
  auto *edit = qobject_cast<CQChartsUnitsEdit *>(w);
  assert(edit);

  bool ok;
  auto units = CQChartsVariant::toUnits(value, ok);
  if (! ok) return;

  edit->setUnits(units.type());
}
