#include <CQChartsGeomBBoxEdit.h>
#include <CQChartsVariant.h>
#include <CQChartsLineEdit.h>
#include <CQChartsPlot.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>

#include <QToolButton>
#include <QHBoxLayout>

#include <svg/region_light_svg.h>
#include <svg/region_dark_svg.h>

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

  regionButton_ = CQUtil::makeWidget<QToolButton>("region");

  regionButton_->setCheckable(true);
  regionButton_->setIcon(CQPixmapCacheInst->getIcon("REGION_LIGHT", "REGION_DARK"));

  connect(regionButton_, SIGNAL(clicked(bool)), this, SLOT(regionSlot(bool)));

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
setPlot(CQChartsPlot *plot)
{
  plot_ = plot;

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
regionSlot(bool b)
{
  if (! plot())
    return;

  CQChartsWidgetUtil::connectDisconnect(b,
    plot_->view(), SIGNAL(regionRectRelease(const CQChartsGeom::BBox &)),
    this, SLOT(regionReleaseSlot(const CQChartsGeom::BBox &)));
}

void
CQChartsGeomBBoxEdit::
regionReleaseSlot(const CQChartsGeom::BBox &bbox)
{
  if (! plot_)
    return;

  auto pbbox = plot_->viewToWindow(bbox);

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
