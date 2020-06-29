#include <CQChartsFillPatternEdit.h>
#include <CQChartsView.h>
#include <CQChartsColorEdit.h>
#include <CQChartsVariant.h>
#include <CQChartsWidgetUtil.h>

#include <CQWidgetMenu.h>
#include <CQRealSpin.h>

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

  setToolTip(fillPattern().toString());

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
  QColor c = Qt::red;

  painter->fillRect(rect, QBrush(c));

  //---

  QString str = (fillPattern().isValid() ? fillPattern().toString() : "<none>");

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
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsFillPattern fillPattern = value.value<CQChartsFillPattern>();

  int x = option.rect.left();

  //---

  // draw fillPattern if can be directly determined
  QRect rect = option.rect;

  rect.setWidth(option.rect.height());

  rect.adjust(0, 1, -3, -2);

  QColor c = Qt::red;

  painter->fillRect(rect, QBrush(c));

  painter->setPen(Qt::black);

  painter->drawRect(rect);

  x = rect.right() + 2;

  //---

  QString str = fillPattern.toString();

  QFontMetrics fm(option.font);

  int w = fm.width(str);

  QStyleOptionViewItem option1 = option;

  option1.rect = QRect(x, option1.rect.top(), w + 8, option1.rect.height());

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsFillPatternPropertyViewType::
tip(const QVariant &value) const
{
  CQChartsFillPattern fillPattern = value.value<CQChartsFillPattern>();

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
  auto *edit = new CQChartsFillPatternLineEdit(parent);

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

  return QVariant::fromValue<CQChartsFillPattern>(edit->fillPattern());
}

void
CQChartsFillPatternPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  auto *edit = qobject_cast<CQChartsFillPatternLineEdit *>(w);
  assert(edit);

  CQChartsFillPattern fillPattern = var.value<CQChartsFillPattern>();

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

  typeCombo_ = CQUtil::makeWidget<QComboBox>("typeCombo");

  typeCombo_->addItems(QStringList() << fillPattern_.enumNames());

  typeCombo_->setToolTip("Fill pattern type");

  addLabelWidget("Type", typeCombo_);

  //---

  paletteEdit_ = CQUtil::makeWidget<CQChartsLineEdit>("paletteEdit");

  paletteEdit_->setToolTip("Palette name");

  addLabelWidget("Palette", paletteEdit_);

  //---

  angleEdit_ = CQUtil::makeWidget<CQRealSpin>("angleEdit");

  angleEdit_->setToolTip("Palette angle");
  angleEdit_->setRange(-360.0, 360.0);

  addLabelWidget("Angle", angleEdit_);

  //---

  altColorEdit_ = CQUtil::makeWidget<CQChartsColorLineEdit>("altColor");

  altColorEdit_->setToolTip("Alternate color");

  addLabelWidget("Alt Color", altColorEdit_);

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
//paletteEdit_ ->setFocusPolicy(Qt::NoFocus);
//angleEdit_   ->setFocusPolicy(Qt::NoFocus);
//altColorEdit_->setFocusPolicy(Qt::NoFocus);
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
  connectDisconnect(paletteEdit_ , SIGNAL(textChanged(const QString &)),
                    SLOT(widgetsToFillPattern()));
  connectDisconnect(angleEdit_   , SIGNAL(valueChanged(double)),
                    SLOT(widgetsToFillPattern()));
  connectDisconnect(altColorEdit_, SIGNAL(colorChanged()),
                    SLOT(widgetsToFillPattern()));
}

void
CQChartsFillPatternEdit::
fillPatternToWidgets()
{
  connectSlots(false);

  if (fillPattern_.isValid()) {
    QStringList names = fillPattern_.enumNames();

    for (int i = 0; i < names.length(); ++i) {
      if (fillPattern_.type() ==  fillPattern_.stringToType(names[i]))
        typeCombo_->setCurrentIndex(i);
    }

    if (fillPattern_.type() == CQChartsFillPattern::Type::PALETTE)
      paletteEdit_->setText(fillPattern_.palette());

    angleEdit_   ->setValue(fillPattern_.angle());
    altColorEdit_->setColor(fillPattern_.altColor());
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
  QString typeName = typeCombo_->currentText();

  CQChartsFillPattern::Type type = fillPattern_.stringToType(typeName);

  CQChartsFillPattern fillPattern = CQChartsFillPattern(type);

  if (fillPattern.type() == CQChartsFillPattern::Type::PALETTE)
    fillPattern.setPalette(paletteEdit_->text());

  fillPattern.setAngle   (angleEdit_   ->value());
  fillPattern.setAltColor(altColorEdit_->color());

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

  setEditVisible(paletteEdit_ , false);
//setEditVisible(angleEdit_   , false);
//setEditVisible(altColorEdit_, false);

  if (fillPattern_.type() == CQChartsFillPattern::Type::PALETTE) {
    setEditVisible(paletteEdit_, true);
  }
}

QSize
CQChartsFillPatternEdit::
sizeHint() const
{
  QSize s1 = CQChartsEditBase::sizeHint();
  QSize s2 = minimumSizeHint();

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
