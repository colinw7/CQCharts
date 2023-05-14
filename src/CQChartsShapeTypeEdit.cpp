#include <CQChartsShapeTypeEdit.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPixelPaintDevice.h>
#include <CQChartsVariant.h>

#include <CQPropertyView.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QHBoxLayout>

CQChartsShapeTypeEdit::
CQChartsShapeTypeEdit(QWidget *parent) :
 CQSwitchLineEdit(parent)
{
  setObjectName("shapeType");

  setToolTip("Shape Type");

  //---

  combo_ = CQUtil::makeWidget<QComboBox>("combo");

  combo_->addItem("<none>");
  combo_->addItems(CQChartsShapeType::typeNames());

  setAltEdit(combo_);

  //---

  connectSlots(true);
}

void
CQChartsShapeTypeEdit::
connectSlots(bool b)
{
  CQUtil::connectDisconnect(b,
    this, SIGNAL(editingFinished()), this, SLOT(textChangedSlot()));
  CQUtil::connectDisconnect(b,
    combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboChanged()));
}

const CQChartsShapeType &
CQChartsShapeTypeEdit::
shapeType() const
{
  return shapeType_;
}

void
CQChartsShapeTypeEdit::
setShapeType(const CQChartsShapeType &shapeType)
{
  if (shapeType != shapeType_) {
    shapeType_ = shapeType;

    updateWidgets();

    Q_EMIT shapeChanged();
  }
}

void
CQChartsShapeTypeEdit::
comboChanged()
{
  CQChartsShapeType shapeType;

  if (combo_->currentIndex() > 0)
    shapeType = CQChartsShapeType(combo_->currentText());
  else
    shapeType = CQChartsShapeType();

  setShapeType(shapeType);
}

void
CQChartsShapeTypeEdit::
textChangedSlot()
{
  CQChartsShapeType shapeType;

  if (shapeType.fromString(text()))
    setShapeType(shapeType);
}

void
CQChartsShapeTypeEdit::
updateWidgets()
{
  connectSlots(false);

  setText(shapeType_.toString());

  if (shapeType_.isValid())
    combo_->setCurrentIndex(combo_->findText(shapeType_.toString()));
  else
    combo_->setCurrentIndex(0);

  connectSlots(true);
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsShapeTypePropertyViewType::
CQChartsShapeTypePropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsShapeTypePropertyViewType::
getEditor() const
{
  return new CQChartsShapeTypePropertyViewEditor;
}

bool
CQChartsShapeTypePropertyViewType::
setEditorData(ViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsShapeTypePropertyViewType::
draw(CQPropertyViewItem *, const ViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  auto shape = CQChartsShapeType::fromVariant(value);

  //---

  // draw shape
  painter->save();

  int ss = std::max(option.rect.height()/2 - 2, 1);

  CQChartsGeom::BBox bbox1(option.rect.left()       , option.rect.center().y() - ss,
                           option.rect.left() + 2*ss, option.rect.center().y() + ss);

  painter->setClipRect(bbox1.adjusted(-1, -1, 1, 1).qrect());

  painter->setPen  (Qt::black);
  painter->setBrush(Qt::white);

  CQChartsPixelPaintDevice device(painter);

  if (shape.isValid()) {
    auto shapeData = CQChartsShapeTypeData(shape.type());

    CQChartsDrawUtil::drawShape(&device, shapeData, bbox1);
  }

  painter->restore();

  int x = int(bbox1.getXMax() + 2);

  //--

  // draw shape name
  auto str = shape.toString();

  QFontMetrics fm(option.font);

  int w = fm.horizontalAdvance(str);

  //---

  auto option1 = option;

  option1.rect = QRect(x, option1.rect.top(), w + 2*margin(), option1.rect.height());

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsShapeTypePropertyViewType::
tip(const QVariant &value) const
{
  auto shape = CQChartsShapeType::fromVariant(value);

  return shape.toString();
}

//------

CQChartsShapeTypePropertyViewEditor::
CQChartsShapeTypePropertyViewEditor()
{
}

QWidget *
CQChartsShapeTypePropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *edit = new CQChartsShapeTypeEdit(parent);

  return edit;
}

void
CQChartsShapeTypePropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsShapeTypeEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(shapeChanged()), obj, method);
}

QVariant
CQChartsShapeTypePropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsShapeTypeEdit *>(w);
  assert(edit);

  return CQChartsShapeType::toVariant(edit->shapeType());
}

void
CQChartsShapeTypePropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsShapeTypeEdit *>(w);
  assert(edit);

  auto shape = CQChartsShapeType::fromVariant(var);

  edit->setShapeType(shape);
}
