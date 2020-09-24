#include <CQChartsGeomBBoxEdit.h>
#include <CQChartsVariant.h>
#include <CQChartsLineEdit.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsRegionMgr.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>

#include <QHBoxLayout>

CQChartsGeomBBoxEdit::
CQChartsGeomBBoxEdit(QWidget *parent, const BBox &value) :
 QFrame(parent)
{
  init(value);
}

CQChartsGeomBBoxEdit::
CQChartsGeomBBoxEdit(const BBox &value) :
 QFrame(nullptr)
{
  init(value);
}

void
CQChartsGeomBBoxEdit::
init(const BBox &value)
{
  setObjectName("edit");

  setFrameStyle(QFrame::NoFrame | QFrame::Plain);

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  //---

  edit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

  edit_->setToolTip("Bounding Box (x1 y1 x2 y2)");
  edit_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  connect(edit_, SIGNAL(editingFinished()), this, SLOT(editingFinishedI()));

  layout->addWidget(edit_);

  //---

  regionButton_ = CQUtil::makeWidget<CQChartsRegionButton>("region");

  regionButton_->setMode(CQChartsRegionButton::Mode::RECT);

  connect(regionButton_, SIGNAL(rectRegionSet(const CQChartsGeom::BBox &)),
          this, SLOT(regionSlot(const CQChartsGeom::BBox &)));

  layout->addWidget(regionButton_);

  //---

  setFocusProxy(edit_);

  setValue(value);
}

void
CQChartsGeomBBoxEdit::
setValue(const BBox &bbox)
{
  bbox_ = bbox;

  bboxToWidget();
}

void
CQChartsGeomBBoxEdit::
setView(View *view)
{
  view_ = view;
  plot_ = nullptr;

  regionButton_->setView(view_);
  regionButton_->setVisible(view_);
}

void
CQChartsGeomBBoxEdit::
setPlot(Plot *plot)
{
  plot_ = plot;
  view_ = nullptr;

  regionButton_->setView(plot_ ? plot_->view() : nullptr);
  regionButton_->setVisible(plot_);
}

void
CQChartsGeomBBoxEdit::
editingFinishedI()
{
  if (disableSignals_) return;

  if (widgetToBBox())
    emit valueChanged();
}

void
CQChartsGeomBBoxEdit::
regionSlot(const CQChartsGeom::BBox &bbox)
{
  BBox pbbox;

  if      (plot())
    pbbox = plot()->viewToWindow(bbox);
  else if (view())
    pbbox = bbox;

  setRegion(pbbox);
}

void
CQChartsGeomBBoxEdit::
setRegion(const CQChartsGeom::BBox &bbox)
{
  setValue(bbox);

  emit valueChanged();
  emit regionChanged();
}

void
CQChartsGeomBBoxEdit::
bboxToWidget()
{
  disableSignals_ = true;

  auto str = bbox_.toString();

  edit_->setText(str);

  disableSignals_ = false;
}

bool
CQChartsGeomBBoxEdit::
widgetToBBox()
{
  BBox bbox;

  if (! bbox.fromString(edit_->text()))
    return false;

  bbox_ = bbox;

  return true;
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsGeomBBoxPropertyViewType::
CQChartsGeomBBoxPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsGeomBBoxPropertyViewType::
getEditor() const
{
  return new CQChartsGeomBBoxPropertyViewEditor;
}

bool
CQChartsGeomBBoxPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsGeomBBoxPropertyViewType::
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

  QFontMetrics fm(font);

  int w = fm.width(str);

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 2*margin());

  option1.font = font;

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsGeomBBoxPropertyViewType::
tip(const QVariant &value) const
{
  bool ok;

  QString str = valueString(nullptr, value, ok);

  return str;
}

QString
CQChartsGeomBBoxPropertyViewType::
valueString(CQPropertyViewItem *, const QVariant &value, bool &ok) const
{
  auto bbox = CQChartsVariant::toBBox(value, ok);

  QString str;

  if (ok)
    str = bbox.toString();
  else
    str = "Undefined";

  return str;
}

//------

CQChartsGeomBBoxPropertyViewEditor::
CQChartsGeomBBoxPropertyViewEditor()
{
}

QWidget *
CQChartsGeomBBoxPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsGeomBBoxEdit(parent);

  return edit;
}

void
CQChartsGeomBBoxPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsGeomBBoxEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(valueChanged()), obj, method);
}

QVariant
CQChartsGeomBBoxPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsGeomBBoxEdit *>(w);
  assert(edit);

  return CQChartsVariant::fromBBox(edit->getValue());
}

void
CQChartsGeomBBoxPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsGeomBBoxEdit *>(w);
  assert(edit);

  bool ok;
  auto bbox = CQChartsVariant::toBBox(var, ok);
  if (! ok) return;

  edit->setValue(bbox);
}
