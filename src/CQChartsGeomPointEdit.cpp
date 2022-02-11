#include <CQChartsGeomPointEdit.h>
#include <CQChartsLineEdit.h>
#include <CQChartsVariant.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsRegionMgr.h>
#include <CQChartsWidgetUtil.h>

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
 QFrame(nullptr)
{
  init(value);
}

void
CQChartsGeomPointEdit::
init(const Point &value)
{
  setObjectName("edit");

  setFrameStyle(uint(QFrame::NoFrame) | uint(QFrame::Plain));

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  //---

  edit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

  edit_->setToolTip("Point (x y)");
  edit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  connect(edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));

  layout->addWidget(edit_);

  //---

  regionButton_ = CQUtil::makeWidget<CQChartsRegionButton>("region");

  regionButton_->setMode(CQChartsRegionButton::Mode::POINT);

  connect(regionButton_, SIGNAL(pointRegionSet(const CQChartsGeom::Point &)),
          this, SLOT(regionSlot(const CQChartsGeom::Point &)));

  layout->addWidget(regionButton_);

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

void
CQChartsGeomPointEdit::
setView(View *view)
{
  view_ = view;
  plot_ = nullptr;

  regionButton_->setView(view_);
  regionButton_->setVisible(view_);
}

void
CQChartsGeomPointEdit::
setPlot(Plot *plot)
{
  plot_ = plot;
  view_ = nullptr;

  regionButton_->setView(plot_ ? plot_->view() : nullptr);
  regionButton_->setVisible(plot_);

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
regionSlot(const CQChartsGeom::Point &p)
{
  Point pp;

  if      (plot())
    pp = plot()->viewToWindow(p);
  else if (view())
    pp = p;

  setRegion(pp);
}

void
CQChartsGeomPointEdit::
setRegion(const CQChartsGeom::Point &p)
{
  setValue(p);

  emit valueChanged();
  emit regionChanged();
}

void
CQChartsGeomPointEdit::
pointToWidget()
{
  disableSignals_ = true;

  auto str = point_.toString();

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

  int w = fm.width(str);

  auto option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  option1.font = font;

  delegate->drawString(painter, option1, str, ind, itemState);
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
  auto point = CQChartsVariant::toPoint(value, ok);

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
  auto *edit = new CQChartsGeomPointEdit(parent);

  return edit;
}

void
CQChartsGeomPointPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsGeomPointEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQChartsGeomPointPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsGeomPointEdit *>(w);
  assert(edit);

  return CQChartsVariant::fromPoint(edit->getValue());
}

void
CQChartsGeomPointPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsGeomPointEdit *>(w);
  assert(edit);

  bool ok;
  auto point = CQChartsVariant::toPoint(var, ok);
  if (! ok) return;

  edit->setValue(point);
}
