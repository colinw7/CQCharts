#include <CQChartsPositionEdit.h>
#include <CQChartsGeomPointEdit.h>
#include <CQChartsUnitsEdit.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

#include <QHBoxLayout>

CQChartsPositionEdit::
CQChartsPositionEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("positionEdit");

  //---

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  //---

  edit_ = CQUtil::makeWidget<CQChartsGeomPointEdit>("point");

  edit_->setToolTip("Position Point (X Y)");

  layout->addWidget(edit_);

  connect(edit_, SIGNAL(regionChanged()), this, SLOT(editRegionSlot()));

  //---

  unitsEdit_ = CQUtil::makeWidget<CQChartsUnitsEdit>("units");

  layout->addWidget(unitsEdit_);

  //---

  setFocusProxy(edit_);

  connectSlots(true);
}

void
CQChartsPositionEdit::
setPosition(const CQChartsPosition &position)
{
  position_ = position;

  positionToWidgets();
}

void
CQChartsPositionEdit::
setView(View *view)
{
  view_ = view;

  edit_->setView(view);
}

void
CQChartsPositionEdit::
setPlot(Plot *plot)
{
  plot_ = plot;

  edit_->setPlot(plot);
}

void
CQChartsPositionEdit::
editRegionSlot()
{
  setRegion(edit_->getValue());
}

void
CQChartsPositionEdit::
setRegion(const CQChartsGeom::Point &p)
{
  setPosition(CQChartsPosition::plot(p));

  Q_EMIT regionChanged();
}

void
CQChartsPositionEdit::
editChanged()
{
  auto        point = edit_->getValue();
  const auto &units = position_.units();

  CQChartsPosition position(point, units);

  if (! position.isValid())
    return;

  position_ = position;

  Q_EMIT positionChanged();
}

void
CQChartsPositionEdit::
unitsChanged()
{
  const auto &value = position_.p();
  auto        units = unitsEdit_->units();

  CQChartsPosition position(value, units);

  if (! position.isValid())
    return;

  position_ = position;

  Q_EMIT positionChanged();
}

void
CQChartsPositionEdit::
positionToWidgets()
{
  connectSlots(false);

  const auto &point = position_.p();
  const auto &units = position_.units();

  edit_->setValue(point);

  unitsEdit_->setUnits(units);

  connectSlots(true);
}

#if 0
void
CQChartsPositionEdit::
widgetsToPosition()
{
  auto point = edit_->getValue();
  auto units = unitsEdit_->units();

  CQChartsPosition position(point, units);

  if (! position.isValid())
    return;

  position_ = position;
}
#endif

void
CQChartsPositionEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(connected_, w, from, this, to);
  };

  connectDisconnect(edit_, SIGNAL(valueChanged()), SLOT(editChanged()));
  connectDisconnect(unitsEdit_, SIGNAL(unitsChanged()), SLOT(unitsChanged()));
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsPositionPropertyViewType::
CQChartsPositionPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsPositionPropertyViewType::
getEditor() const
{
  return new CQChartsPositionPropertyViewEditor;
}

bool
CQChartsPositionPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsPositionPropertyViewType::
draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  //---

  bool ok;

  auto str = valueString(item, value, ok);

  auto font = option.font;

  if (! ok)
    font.setItalic(true);

  //---

  QFontMetrics fm(option.font);

  int w = fm.horizontalAdvance(str);

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  option1.font = font;

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsPositionPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  auto str = valueString(nullptr, value, ok);

  return str;
}

QString
CQChartsPositionPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  auto position = CQChartsPosition::fromVariant(value);

  QString str;

  if (position.isValid()) {
    str = position.toString();
    ok  = true;
  }
  else {
    str = "Undefined";
    ok  = false;
  }

  return str;
}

//------

CQChartsPositionPropertyViewEditor::
CQChartsPositionPropertyViewEditor()
{
}

QWidget *
CQChartsPositionPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsPositionEdit(parent);

  return edit;
}

void
CQChartsPositionPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsPositionEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(positionChanged()), obj, method);
}

QVariant
CQChartsPositionPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsPositionEdit *>(w);
  assert(edit);

  return CQChartsPosition::toVariant(edit->position());
}

void
CQChartsPositionPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsPositionEdit *>(w);
  assert(edit);

  auto position = CQChartsPosition::fromVariant(var);

  edit->setPosition(position);
}
