#include <CQChartsGeomPointEdit.h>
#include <CQChartsLineEdit.h>
#include <CQChartsVariant.h>

#include <CQPropertyView.h>

#include <QHBoxLayout>

CQChartsGeomPointEdit::
CQChartsGeomPointEdit(QWidget *parent, const Point &value) :
 QFrame(parent)
{
  init(value);
}

CQChartsGeomPointEdit::
CQChartsGeomPointEdit(const Point &value) :
 QFrame(0)
{
  init(value);
}

void
CQChartsGeomPointEdit::
init(const Point &value)
{
  setObjectName("edit");

  setFrameStyle(QFrame::NoFrame | QFrame::Plain);

  auto layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  //---

  edit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

  edit_->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  connect(edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));

  layout->addWidget(edit_);

  //---

  setFocusProxy(edit_);

  setValue(value);
}

void
CQChartsGeomPointEdit::
setValue(const Point &point)
{
  point_ = point;

  pointToWidget();
}

const CQChartsGeomPointEdit::Point &
CQChartsGeomPointEdit::
getValue() const
{
  return point_;
}

void
CQChartsGeomPointEdit::
editingFinishedI()
{
  if (disableSignals_) return;

  if (widgetToPoint())
    emit valueChanged();
}

void
CQChartsGeomPointEdit::
pointToWidget()
{
  disableSignals_ = true;

  QString str = point_.toString();

  edit_->setText(str);

  disableSignals_ = false;
}

bool
CQChartsGeomPointEdit::
widgetToPoint()
{
  Point point;

  if (! point.fromString(edit_->text()))
    return false;

  point_ = point;

  return true;
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsGeomPointPropertyViewType::
CQChartsGeomPointPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsGeomPointPropertyViewType::
getEditor() const
{
  return new CQChartsGeomPointPropertyViewEditor;
}

bool
CQChartsGeomPointPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsGeomPointPropertyViewType::
draw(CQPropertyViewItem *item, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  //---

  bool ok;

  QString str = valueString(item, value, ok);

  QFont font = option.font;

  if (! ok)
    font.setItalic(true);

  //---

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  option1.font = font;

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsGeomPointPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  QString str = valueString(nullptr, value, ok);

  return str;
}

QString
CQChartsGeomPointPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  CQChartsGeom::Point point = CQChartsVariant::toPoint(value, ok);

  QString str;

  if (ok)
    str = point.toString();
  else
    str = "Undefined";

  return str;
}

//------

CQChartsGeomPointPropertyViewEditor::
CQChartsGeomPointPropertyViewEditor()
{
}

QWidget *
CQChartsGeomPointPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsGeomPointEdit *edit = new CQChartsGeomPointEdit(parent);

  return edit;
}

void
CQChartsGeomPointPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto edit = qobject_cast<CQChartsGeomPointEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQChartsGeomPointPropertyViewEditor::
getValue(QWidget *w)
{
  auto edit = qobject_cast<CQChartsGeomPointEdit *>(w);
  assert(edit);

  return CQChartsVariant::fromPoint(edit->getValue());
}

void
CQChartsGeomPointPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto edit = qobject_cast<CQChartsGeomPointEdit *>(w);
  assert(edit);

  bool ok;
  CQChartsGeom::Point point = CQChartsVariant::toPoint(var, ok);
  if (! ok) return;

  edit->setValue(point);
}
