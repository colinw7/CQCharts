#include <CQChartsFillPatternEdit.h>
#include <CQChartsView.h>
#include <CQChartsPaletteNameEdit.h>
#include <CQChartsColorEdit.h>
#include <CQChartsAlphaEdit.h>
#include <CQChartsAngleEdit.h>
#include <CQChartsImageEdit.h>
#include <CQChartsVariant.h>
#include <CQChartsObjUtil.h>
#include <CQChartsWidgetUtil.h>

#include <CQPropertyView.h>

#include <CQRealSpin.h>
#include <CQWidgetMenu.h>

#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QPainter>

CQChartsFillPatternLineEdit::
CQChartsFillPatternLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("fillPatternLineEdit");

  setToolTip("Fill Pattern");

  //---

  menuEdit_ = dataEdit_ = new CQChartsFillPatternEdit;

  dataEdit_->setNoFocus();

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);

  fillPatternToWidgets();
}

void
CQChartsFillPatternLineEdit::
setCharts(CQCharts *charts)
{
  dataEdit_->setCharts(charts);
}

const CQChartsFillPattern &
CQChartsFillPatternLineEdit::
fillPattern() const
{
  return dataEdit_->fillPattern();
}

void
CQChartsFillPatternLineEdit::
setFillPattern(const CQChartsFillPattern &fillPattern)
{
  updateFillPattern(fillPattern, /*updateText*/ true);
}

void
CQChartsFillPatternLineEdit::
setNoFocus()
{
  dataEdit_->setNoFocus();
}

void
CQChartsFillPatternLineEdit::
updateFillPattern(const CQChartsFillPattern &fillPattern, bool updateText)
{
  connectSlots(false);

  dataEdit_->setFillPattern(fillPattern);

  connectSlots(true);

  if (updateText)
    fillPatternToWidgets();

  emit fillPatternChanged();
}

void
CQChartsFillPatternLineEdit::
textChanged()
{
  CQChartsFillPattern fillPattern(edit_->text());

  if (! fillPattern.isValid())
    return;

  updateFillPattern(fillPattern, /*updateText*/ false);
}

void
CQChartsFillPatternLineEdit::
fillPatternToWidgets()
{
  connectSlots(false);

  if (fillPattern().isValid())
    edit_->setText(fillPattern().toString());
  else
    edit_->setText("");

  auto tip = QString("%1 (%2)").arg(toolTip()).arg(fillPattern().toString());

  edit_->setToolTip(tip);

  connectSlots(true);
}

void
CQChartsFillPatternLineEdit::
menuEditChanged()
{
  fillPatternToWidgets();

  emit fillPatternChanged();
}

void
CQChartsFillPatternLineEdit::
connectSlots(bool b)
{
  connectBaseSlots(b);

  CQChartsWidgetUtil::connectDisconnect(b,
    dataEdit_, SIGNAL(fillPatternChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsFillPatternLineEdit::
drawPreview(QPainter *painter, const QRect &rect)
{
  // draw pattern
  auto bg = QColor(Qt::black);
  auto fg = QColor(Qt::white);

  QPen   pen;
  QBrush brush;

  CQChartsUtil::setPen(pen, true, fg);
  CQChartsUtil::setBrush(brush, true, bg, CQChartsAlpha(), fillPattern());

  painter->setPen(pen);

  painter->fillRect(rect, brush);

  //---

  // draw text
  CQChartsUtil::setPen(pen, true, QColor(Qt::black));

  painter->setPen(pen);

  auto str = (fillPattern().isValid() ? fillPattern().toString() : "<none>");

  drawCenteredText(painter, str);
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsFillPatternPropertyViewType::
CQChartsFillPatternPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsFillPatternPropertyViewType::
getEditor() const
{
  return new CQChartsFillPatternPropertyViewEditor;
}

bool
CQChartsFillPatternPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsFillPatternPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, const ItemState &itemState)
{
  delegate->drawBackground(painter, option, ind, itemState);

  auto fillPattern = CQChartsFillPattern::fromVariant(value);

  int x = option.rect.left();

  //---

  // draw fillPattern if can be directly determined
  auto rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  //---

  // draw pattern
  auto bg = QColor(Qt::black);
  auto fg = QColor(Qt::white);

  QPen   pen;
  QBrush brush;

  CQChartsUtil::setPen(pen, true, fg);
  CQChartsUtil::setBrush(brush, true, bg, CQChartsAlpha(), fillPattern);

  painter->setPen(pen);

  painter->fillRect(rect, brush);

  //---

  // draw border (TODO: config color)
  CQChartsUtil::setPen(pen, true, QColor(200, 200, 200));

  painter->setPen(pen);

  painter->drawRect(rect);

  x = rect.right() + 2;

  //---

  auto str = fillPattern.toString();

  QFontMetrics fm(option.font);

  int w = fm.horizontalAdvance(str);

  auto option1 = option;

  option1.rect = QRect(x, option1.rect.top(), w + 2*margin(), option1.rect.height());

  delegate->drawString(painter, option1, str, ind, itemState);
}

QString
CQChartsFillPatternPropertyViewType::
tip(const QVariant &value) const
{
  auto fillPattern = CQChartsFillPattern::fromVariant(value);

  return fillPattern.toString();
}

//------

CQChartsFillPatternPropertyViewEditor::
CQChartsFillPatternPropertyViewEditor()
{
}

QWidget *
CQChartsFillPatternPropertyViewEditor::
createEdit(QWidget *parent)
{
  auto *item = CQPropertyViewMgrInst->editItem();

  auto *obj = (item ? item->object() : nullptr);

  CQChartsObjUtil::ObjData objData;

  CQChartsObjUtil::getObjData(obj, objData);

  //---

  auto *edit = new CQChartsFillPatternLineEdit(parent);

  if (objData.charts)
    edit->setCharts(objData.charts);

  return edit;
}

void
CQChartsFillPatternPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  auto *edit = qobject_cast<CQChartsFillPatternLineEdit *>(w);
  assert(edit);

  // TODO: why do we need direct connection for plot object to work ?
  QObject::connect(edit, SIGNAL(fillPatternChanged()), obj, method, Qt::DirectConnection);
}

QVariant
CQChartsFillPatternPropertyViewEditor::
getValue(QWidget *w)
{
  auto *edit = qobject_cast<CQChartsFillPatternLineEdit *>(w);
  assert(edit);

  return CQChartsFillPattern::toVariant(edit->fillPattern());
}

void
CQChartsFillPatternPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsFillPatternLineEdit *>(w);
  assert(edit);

  auto fillPattern = CQChartsFillPattern::fromVariant(var);

  edit->setFillPattern(fillPattern);
}

//------

CQChartsFillPatternEdit::
CQChartsFillPatternEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("fillPatternEdit");

  //---

  auto *layout = CQUtil::makeLayout<QGridLayout>(this, 2, 2);

  //---

  int row = 0;

  auto addLabelWidget = [&](const QString &label, QWidget *edit) {
    auto *labelW = CQUtil::makeLabelWidget<QLabel>(label, "label");

    layout->addWidget(labelW, row, 0);
    layout->addWidget(edit  , row, 1);

    widgetLabels_[edit] = labelW;

    ++row;
  };

  //---

  // type
  typeCombo_ = CQUtil::makeWidget<QComboBox>("typeCombo");

  typeCombo_->addItems(QStringList() << fillPattern_.enumNames());

  typeCombo_->setToolTip("Fill pattern type");

  addLabelWidget("Type", typeCombo_);

  //---

  // scale
  scaleEdit_ = CQUtil::makeWidget<CQRealSpin>("scaleEdit");

  scaleEdit_->setToolTip("Palette scale factor");

  addLabelWidget("Scale", scaleEdit_);

  //---

  // palette
  paletteEdit_ = CQUtil::makeWidget<CQChartsPaletteNameEdit>("paletteEdit");

  paletteEdit_->setToolTip("Palette name");

  addLabelWidget("Palette", paletteEdit_);

  //---

  // image
  imageEdit_ = CQUtil::makeWidget<CQChartsImageEdit>("imageEdit");

  imageEdit_->setToolTip("Image name");

  addLabelWidget("Image", imageEdit_);

  //---

  // angle
  angleEdit_ = CQUtil::makeWidget<CQChartsAngleEdit>("angleEdit");

  angleEdit_->setToolTip("Palette angle");

  addLabelWidget("Angle", angleEdit_);

  //---

  // alt color
  altColorEdit_ = CQUtil::makeWidget<CQChartsColorLineEdit>("altColor");

  altColorEdit_->setToolTip("Alternate color");

  addLabelWidget("Alt Color", altColorEdit_);

  //---

  // alt alpha
  altAlphaEdit_ = CQUtil::makeWidget<CQChartsAlphaEdit>("altAlpha");

  altAlphaEdit_->setToolTip("Alternate color alpha");

  addLabelWidget("Alt Alpha", altAlphaEdit_);

  //---

  layout->setRowStretch(row, 1);

  layout->setColumnStretch(2, 1);

  //---

  connectSlots(true);

  setFixedHeight(CQChartsFillPatternEdit::minimumSizeHint().height());

  updateState();
}

void
CQChartsFillPatternEdit::
setCharts(CQCharts *charts)
{
  paletteEdit_->setCharts(charts);
}

void
CQChartsFillPatternEdit::
setFillPattern(const CQChartsFillPattern &fillPattern)
{
  fillPattern_ = fillPattern;

  fillPatternToWidgets();

  updateState();

  emit fillPatternChanged();
}

void
CQChartsFillPatternEdit::
setNoFocus()
{
  typeCombo_   ->setFocusPolicy(Qt::NoFocus);
//scaleEdit_   ->setFocusPolicy(Qt::NoFocus);
//paletteEdit_ ->setFocusPolicy(Qt::NoFocus);
//imageEdit_   ->setFocusPolicy(Qt::NoFocus);
//angleEdit_   ->setFocusPolicy(Qt::NoFocus);
//altColorEdit_->setFocusPolicy(Qt::NoFocus);
//altAlphaEdit_->setFocusPolicy(Qt::NoFocus);
}

void
CQChartsFillPatternEdit::
connectSlots(bool b)
{
  assert(b != connected_);

  connected_ = b;

  //---

  auto connectDisconnect = [&](QWidget *w, const char *from, const char *to) {
    CQChartsWidgetUtil::connectDisconnect(connected_, w, from, this, to);
  };

  connectDisconnect(typeCombo_   , SIGNAL(currentIndexChanged(int)),
                    SLOT(widgetsToFillPattern()));
  connectDisconnect(scaleEdit_ , SIGNAL(valueChanged(double)),
                    SLOT(widgetsToFillPattern()));
  connectDisconnect(paletteEdit_ , SIGNAL(nameChanged()),
                    SLOT(widgetsToFillPattern()));
  connectDisconnect(imageEdit_ , SIGNAL(imageChanged()),
                    SLOT(widgetsToFillPattern()));
  connectDisconnect(angleEdit_   , SIGNAL(angleChanged()),
                    SLOT(widgetsToFillPattern()));
  connectDisconnect(altColorEdit_, SIGNAL(colorChanged()),
                    SLOT(widgetsToFillPattern()));
  connectDisconnect(altAlphaEdit_, SIGNAL(alphaChanged()),
                    SLOT(widgetsToFillPattern()));
}

void
CQChartsFillPatternEdit::
fillPatternToWidgets()
{
  connectSlots(false);

  if (fillPattern_.isValid()) {
    // set type
    auto names = fillPattern_.enumNames();

    for (int i = 0; i < names.length(); ++i) {
      if (fillPattern_.type() ==  fillPattern_.stringToType(names[i]))
        typeCombo_->setCurrentIndex(i);
    }

    //---

    scaleEdit_->setValue(fillPattern_.scale());

    if (fillPattern_.type() == CQChartsFillPattern::Type::PALETTE)
      paletteEdit_->setPaletteName(fillPattern_.palette());

    imageEdit_   ->setImage(fillPattern_.image());
    angleEdit_   ->setAngle(fillPattern_.angle());
    altColorEdit_->setColor(fillPattern_.altColor());
    altAlphaEdit_->setAlpha(fillPattern_.altAlpha());
  }
  else {
    typeCombo_->setCurrentIndex(0);
  }

  connectSlots(true);
}

void
CQChartsFillPatternEdit::
widgetsToFillPattern()
{
  // get type
  auto typeName = typeCombo_->currentText();

  auto type = fillPattern_.stringToType(typeName);

  auto fillPattern = CQChartsFillPattern(type);

  //---

  fillPattern.setScale(scaleEdit_->value());

  if (fillPattern.type() == CQChartsFillPattern::Type::PALETTE)
    fillPattern.setPalette(paletteEdit_->paletteName());

  fillPattern.setImage   (imageEdit_   ->image());
  fillPattern.setAngle   (angleEdit_   ->angle());
  fillPattern.setAltColor(altColorEdit_->color());
  fillPattern.setAltAlpha(altAlphaEdit_->alpha());

  fillPattern_ = fillPattern;

  //---

  updateState();

  emit fillPatternChanged();
}

void
CQChartsFillPatternEdit::
updateState()
{
  auto setEditVisible = [&](QWidget *w, bool visible) {
    w->setVisible(visible);

    widgetLabels_[w]->setVisible(visible);
  };

//setEditVisible(scaleEdit_   , false);
  setEditVisible(paletteEdit_ , false);
  setEditVisible(imageEdit_   , false);
//setEditVisible(angleEdit_   , false);
//setEditVisible(altColorEdit_, false);
//setEditVisible(altAlphaEdit_, false);

  if      (fillPattern_.type() == CQChartsFillPattern::Type::PALETTE) {
    setEditVisible(paletteEdit_, true);
  }
  else if (fillPattern_.type() == CQChartsFillPattern::Type::IMAGE ||
           fillPattern_.type() == CQChartsFillPattern::Type::TEXTURE) {
    setEditVisible(imageEdit_, true);
  }
}

QSize
CQChartsFillPatternEdit::
sizeHint() const
{
  auto s1 = CQChartsEditBase::sizeHint();
  auto s2 = minimumSizeHint();

  return QSize(s1.width(), s2.height());
}

QSize
CQChartsFillPatternEdit::
minimumSizeHint() const
{
  QFontMetrics fm(font());

  int eh = fm.height() + 4;

  return QSize(0, eh*8);
}
