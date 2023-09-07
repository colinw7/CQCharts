#include <CQChartsGeomMinMaxEdit.h>
#include <CQChartsLineEdit.h>
#include <CQChartsVariant.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQTclUtil.h>

#include <CQPropertyView.h>

#include <QHBoxLayout>

namespace {

bool stringToRange(const QString &str, double &min, double &max)
{
  QStringList strs;

  if (! CQTcl::splitList(str, strs) || strs.size() != 2)
    return false;

  if (! CQChartsUtil::toReal(strs[0], min) || ! CQChartsUtil::toReal(strs[1], max))
    return false;

  return true;
}

}

//---

CQChartsGeomMinMaxEdit::
CQChartsGeomMinMaxEdit(QWidget *parent, const MinMax &value) :
 QFrame(parent)
{
  init(value);
}

CQChartsGeomMinMaxEdit::
CQChartsGeomMinMaxEdit(const MinMax &value) :
 QFrame(nullptr)
{
  init(value);
}

CQChartsView *
CQChartsGeomMinMaxEdit::
view() const
{
  return view_.data();
}

CQChartsPlot *
CQChartsGeomMinMaxEdit::
plot() const
{
  return plot_.data();
}

void
CQChartsGeomMinMaxEdit::
init(const MinMax &value)
{
  setObjectName("edit");

  setFrameStyle(uint(QFrame::NoFrame) | uint(QFrame::Plain));

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  //---

  edit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

  edit_->setToolTip("Range (min max)");
  edit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  connect(edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));

  layout->addWidget(edit_);

  //---

  setFocusProxy(edit_);

  setValue(value);
}

void
CQChartsGeomMinMaxEdit::
setValue(const MinMax &range)
{
  range_ = range;

  rangeToWidget();
}

void
CQChartsGeomMinMaxEdit::
setView(View *view)
{
  view_ = view;
  plot_ = PlotP();
}

void
CQChartsGeomMinMaxEdit::
setPlot(Plot *plot)
{
  plot_ = plot;
  view_ = ViewP();
}

void
CQChartsGeomMinMaxEdit::
editingFinishedI()
{
  if (disableSignals_) return;

  if (widgetToRange())
    Q_EMIT valueChanged();
  else
    rangeToWidget();
}

void
CQChartsGeomMinMaxEdit::
rangeToWidget()
{
  disableSignals_ = true;

  auto str = QString("%1 %2").arg(range_.min()).arg(range_.max());

  edit_->setText(str);

  disableSignals_ = false;
}

bool
CQChartsGeomMinMaxEdit::
widgetToRange()
{
  double min, max;
  if (! stringToRange(edit_->text(), min, max))
    return false;

  range_ = MinMax(min, max);

  return true;
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsGeomMinMaxPropertyViewType::
CQChartsGeomMinMaxPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsGeomMinMaxPropertyViewType::
getEditor() const
{
  return new CQChartsGeomMinMaxPropertyViewEditor;
}

bool
CQChartsGeomMinMaxPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsGeomMinMaxPropertyViewType::
draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  //---

  bool ok;

  auto str  = valueString(item, value, ok);
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
CQChartsGeomMinMaxPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  QString str = valueString(nullptr, value, ok);

  return str;
}

QString
CQChartsGeomMinMaxPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  auto str = CQChartsVariant::toString(value, ok);

  double min, max;
  if (ok && ! stringToRange(str, min, max))
    ok = false;

  QString str1;

  if (ok)
    str1 = QString("%1 %2").arg(min).arg(max);
  else
    str1 = "Undefined";

  return str1;
}

//------

CQChartsGeomMinMaxPropertyViewEditor::
CQChartsGeomMinMaxPropertyViewEditor()
{
}

QWidget *
CQChartsGeomMinMaxPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsGeomMinMaxEdit(parent);

  return edit;
}

void
CQChartsGeomMinMaxPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsGeomMinMaxEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQChartsGeomMinMaxPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsGeomMinMaxEdit *>(w);
  assert(edit);

  auto range = edit->getValue();

  return QString("%1 %2").arg(range.min()).arg(range.max());
}

void
CQChartsGeomMinMaxPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsGeomMinMaxEdit *>(w);
  assert(edit);

  bool ok;
  auto str = CQChartsVariant::toString(var, ok);
  if (! ok) return;

  double min, max;
  if (! stringToRange(str, min, max))
    return;

  auto range = CQChartsGeom::MinMax(min, max);

  edit->setValue(range);
}
