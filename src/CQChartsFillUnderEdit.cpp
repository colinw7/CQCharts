#include <CQChartsFillUnderEdit.h>

#include <CQWidgetMenu.h>
#include <CQPropertyView.h>
#include <CQRealSpin.h>
#include <CQChartsUtil.h>
#include <CQGroupBox.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStylePainter>

CQChartsFillUnderSideEdit::
CQChartsFillUnderSideEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("fillUnderSide");

  QHBoxLayout *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  combo_ = CQUtil::makeWidget<QComboBox>("combo");

  combo_->addItems(QStringList() << CQChartsFillUnderSide::sideNames());

  layout->addWidget(combo_);

  connectSlots(true);
}

void
CQChartsFillUnderSideEdit::
connectSlots(bool b)
{
  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      QObject::connect(w, from, this, to);
    else
      QObject::disconnect(w, from, this, to);
  };

  connectDisconnect(b, combo_, SIGNAL(currentIndexChanged(int)), SLOT(comboChanged()));
}

const CQChartsFillUnderSide &
CQChartsFillUnderSideEdit::
fillUnderSide() const
{
  return fillUnderSide_;
}

void
CQChartsFillUnderSideEdit::
setFillUnderSide(const CQChartsFillUnderSide &fillUnderSide)
{
  connectSlots(false);

  fillUnderSide_ = fillUnderSide;

  combo_->setCurrentIndex(combo_->findText(fillUnderSide_.toString()));

  connectSlots(true);
}

void
CQChartsFillUnderSideEdit::
comboChanged()
{
  connectSlots(false);

  fillUnderSide_ = CQChartsFillUnderSide(combo_->currentText());

  connectSlots(true);

  emit fillUnderSideChanged();
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsFillUnderSidePropertyViewType::
CQChartsFillUnderSidePropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsFillUnderSidePropertyViewType::
getEditor() const
{
  return new CQChartsFillUnderSidePropertyViewEditor;
}

bool
CQChartsFillUnderSidePropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsFillUnderSidePropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsFillUnderSide fillUnderSide = value.value<CQChartsFillUnderSide>();

  QString str = fillUnderSide.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsFillUnderSidePropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsFillUnderSide>().toString();

  return str;
}

//------

CQChartsFillUnderSidePropertyViewEditor::
CQChartsFillUnderSidePropertyViewEditor()
{
}

QWidget *
CQChartsFillUnderSidePropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsFillUnderSideEdit *edit = new CQChartsFillUnderSideEdit(parent);

  return edit;
}

void
CQChartsFillUnderSidePropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsFillUnderSideEdit *edit = qobject_cast<CQChartsFillUnderSideEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(fillUnderSideChanged()), obj, method);
}

QVariant
CQChartsFillUnderSidePropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsFillUnderSideEdit *edit = qobject_cast<CQChartsFillUnderSideEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->fillUnderSide());
}

void
CQChartsFillUnderSidePropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsFillUnderSideEdit *edit = qobject_cast<CQChartsFillUnderSideEdit *>(w);
  assert(edit);

  CQChartsFillUnderSide fillUnderSide = var.value<CQChartsFillUnderSide>();

  edit->setFillUnderSide(fillUnderSide);
}

//------

CQChartsFillUnderPosLineEdit::
CQChartsFillUnderPosLineEdit(QWidget *parent) :
 CQChartsLineEditBase(parent)
{
  setObjectName("fillUnderPosLineEdit");

  //---

  menuEdit_ = dataEdit_ = new CQChartsFillUnderPosEdit;

  menu_->setWidget(dataEdit_);

  //---

  connectSlots(true);
}

const CQChartsFillUnderPos &
CQChartsFillUnderPosLineEdit::
fillUnderPos() const
{
  return dataEdit_->fillUnderPos();
}

void
CQChartsFillUnderPosLineEdit::
setFillUnderPos(const CQChartsFillUnderPos &fillUnderPos)
{
  updateFillUnderPos(fillUnderPos, /*updateText*/true);
}

void
CQChartsFillUnderPosLineEdit::
updateFillUnderPos(const CQChartsFillUnderPos &fillUnderPos, bool updateText)
{
  connectSlots(false);

  dataEdit_->setFillUnderPos(fillUnderPos);

  connectSlots(true);

  if (updateText)
    fillUnderPosToWidgets();

  emit fillUnderPosChanged();
}

void
CQChartsFillUnderPosLineEdit::
textChanged()
{
  CQChartsFillUnderPos fillUnderPos(edit_->text());

  if (! fillUnderPos.isValid())
    return;

  updateFillUnderPos(fillUnderPos, /*updateText*/ false);
}

void
CQChartsFillUnderPosLineEdit::
fillUnderPosToWidgets()
{
  connectSlots(false);

  if (fillUnderPos().isValid())
    edit_->setText(fillUnderPos().toString());
  else
    edit_->setText("");

  setToolTip(fillUnderPos().toString());

  connectSlots(true);
}

void
CQChartsFillUnderPosLineEdit::
menuEditChanged()
{
  fillUnderPosToWidgets();

  emit fillUnderPosChanged();
}

void
CQChartsFillUnderPosLineEdit::
connectSlots(bool b)
{
  CQChartsLineEditBase::connectSlots(b);

  if (b)
    connect(dataEdit_, SIGNAL(fillUnderPosChanged()), this, SLOT(menuEditChanged()));
  else
    disconnect(dataEdit_, SIGNAL(fillUnderPosChanged()), this, SLOT(menuEditChanged()));
}

void
CQChartsFillUnderPosLineEdit::
drawPreview(QPainter *painter, const QRect &)
{
  QString str = (fillUnderPos().isValid() ? fillUnderPos().toString() : "<none>");

  drawCenteredText(painter, str);
}

//------

#include <CQPropertyViewItem.h>
#include <CQPropertyViewDelegate.h>

CQChartsFillUnderPosPropertyViewType::
CQChartsFillUnderPosPropertyViewType()
{
}

CQPropertyViewEditorFactory *
CQChartsFillUnderPosPropertyViewType::
getEditor() const
{
  return new CQChartsFillUnderPosPropertyViewEditor;
}

bool
CQChartsFillUnderPosPropertyViewType::
setEditorData(CQPropertyViewItem *item, const QVariant &value)
{
  return item->setData(value);
}

void
CQChartsFillUnderPosPropertyViewType::
draw(CQPropertyViewItem *, const CQPropertyViewDelegate *delegate, QPainter *painter,
     const QStyleOptionViewItem &option, const QModelIndex &ind,
     const QVariant &value, bool inside)
{
  delegate->drawBackground(painter, option, ind, inside);

  CQChartsFillUnderPos fillUnderPos = value.value<CQChartsFillUnderPos>();

  QString str = fillUnderPos.toString();

  QFontMetricsF fm(option.font);

  double w = fm.width(str);

  //---

  QStyleOptionViewItem option1 = option;

  option1.rect.setRight(option1.rect.left() + w + 8);

  delegate->drawString(painter, option1, str, ind, inside);
}

QString
CQChartsFillUnderPosPropertyViewType::
tip(const QVariant &value) const
{
  QString str = value.value<CQChartsFillUnderPos>().toString();

  return str;
}

//------

CQChartsFillUnderPosPropertyViewEditor::
CQChartsFillUnderPosPropertyViewEditor()
{
}

QWidget *
CQChartsFillUnderPosPropertyViewEditor::
createEdit(QWidget *parent)
{
  CQChartsFillUnderPosLineEdit *edit = new CQChartsFillUnderPosLineEdit(parent);

  return edit;
}

void
CQChartsFillUnderPosPropertyViewEditor::
connect(QWidget *w, QObject *obj, const char *method)
{
  CQChartsFillUnderPosLineEdit *edit = qobject_cast<CQChartsFillUnderPosLineEdit *>(w);
  assert(edit);

  QObject::connect(edit, SIGNAL(fillUnderPosChanged()), obj, method);
}

QVariant
CQChartsFillUnderPosPropertyViewEditor::
getValue(QWidget *w)
{
  CQChartsFillUnderPosLineEdit *edit = qobject_cast<CQChartsFillUnderPosLineEdit *>(w);
  assert(edit);

  return QVariant::fromValue(edit->fillUnderPos());
}

void
CQChartsFillUnderPosPropertyViewEditor::
setValue(QWidget *w, const QVariant &var)
{
  CQChartsFillUnderPosLineEdit *edit = qobject_cast<CQChartsFillUnderPosLineEdit *>(w);
  assert(edit);

  CQChartsFillUnderPos fillUnderPos = var.value<CQChartsFillUnderPos>();

  edit->setFillUnderPos(fillUnderPos);
}

//------

CQChartsFillUnderPosEdit::
CQChartsFillUnderPosEdit(QWidget *parent) :
 CQChartsEditBase(parent)
{
  setObjectName("fillUnderPos");

  QVBoxLayout *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //----

  CQGroupBox *xGroup = CQUtil::makeLabelWidget<CQGroupBox>("X", "xGroup");

  layout->addWidget(xGroup);

  QGridLayout *xlayout = CQUtil::makeLayout<QGridLayout>(xGroup, 2, 2);

  //--

  xtypeCombo_ = CQUtil::makeWidget<QComboBox>("xtypeCombo");

  xtypeCombo_->addItems(QStringList() << "None" << "Min" << "Max" << "Pos");

  connect(xtypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));

  QLabel *xtypeLabel = CQUtil::makeLabelWidget<QLabel>("Type", "xtypeLabel");

  xlayout->addWidget(xtypeLabel , 0, 0);
  xlayout->addWidget(xtypeCombo_, 0, 1);

  //---

  QLabel *xposLabel = CQUtil::makeLabelWidget<QLabel>("Pos", "xposLabel");

  xposEdit_ = CQUtil::makeWidget<CQRealSpin>("xposEdit");

  connect(xposEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));

  xlayout->addWidget(xposLabel, 1, 0);
  xlayout->addWidget(xposEdit_, 1, 1);

  //----

  CQGroupBox *yGroup = CQUtil::makeLabelWidget<CQGroupBox>("Y", "yGroup");

  layout->addWidget(yGroup);

  QGridLayout *ylayout = CQUtil::makeLayout<QGridLayout>(yGroup, 2, 2);

  //--

  ytypeCombo_ = CQUtil::makeWidget<QComboBox>("ytypeCombo");

  ytypeCombo_->addItems(QStringList() << "None" << "Min" << "Max" << "Pos");

  connect(ytypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(widgetsToFillUnderPos()));

  QLabel *ytypeLabel = CQUtil::makeLabelWidget<QLabel>("Type", "ytypeLabel");

  ylayout->addWidget(ytypeLabel , 0, 0);
  ylayout->addWidget(ytypeCombo_, 0, 1);

  //---

  QLabel *yposLabel = CQUtil::makeLabelWidget<QLabel>("Pos", "yposLabel");

  yposEdit_ = CQUtil::makeWidget<CQRealSpin>("yposEdit");

  connect(yposEdit_, SIGNAL(valueChanged(double)), this, SLOT(widgetsToFillUnderPos()));

  ylayout->addWidget(yposLabel, 1, 0);
  ylayout->addWidget(yposEdit_, 1, 1);
}

const CQChartsFillUnderPos &
CQChartsFillUnderPosEdit::
fillUnderPos() const
{
  return fillUnderPos_;
}

void
CQChartsFillUnderPosEdit::
setFillUnderPos(const CQChartsFillUnderPos &fillUnderPos)
{
  fillUnderPos_ = fillUnderPos;

  fillUnderPosToWidgets();

  emit fillUnderPosChanged();
}

void
CQChartsFillUnderPosEdit::
fillUnderPosToWidgets()
{
  connectSlots(false);

  if (fillUnderPos_.isValid()) {
    if      (fillUnderPos_.xtype() == CQChartsFillUnderPos::Type::NONE) {
      xtypeCombo_->setCurrentIndex(0);
    }
    else if (fillUnderPos_.xtype() == CQChartsFillUnderPos::Type::MIN) {
      xtypeCombo_->setCurrentIndex(1);
    }
    else if (fillUnderPos_.xtype() == CQChartsFillUnderPos::Type::MAX) {
      xtypeCombo_->setCurrentIndex(2);
    }
    else if (fillUnderPos_.xtype() == CQChartsFillUnderPos::Type::POS) {
      xtypeCombo_->setCurrentIndex(3);
    }

    xposEdit_->setValue(fillUnderPos_.xpos());

    if      (fillUnderPos_.ytype() == CQChartsFillUnderPos::Type::NONE) {
      ytypeCombo_->setCurrentIndex(0);
    }
    else if (fillUnderPos_.ytype() == CQChartsFillUnderPos::Type::MIN) {
      ytypeCombo_->setCurrentIndex(1);
    }
    else if (fillUnderPos_.ytype() == CQChartsFillUnderPos::Type::MAX) {
      ytypeCombo_->setCurrentIndex(2);
    }
    else if (fillUnderPos_.ytype() == CQChartsFillUnderPos::Type::POS) {
      ytypeCombo_->setCurrentIndex(3);
    }

    yposEdit_->setValue(fillUnderPos_.ypos());
  }
  else {
    xtypeCombo_->setCurrentIndex(0);
    xposEdit_->setValue(0.0);

    ytypeCombo_->setCurrentIndex(0);
    yposEdit_->setValue(0.0);
  }

  connectSlots(true);
}

void
CQChartsFillUnderPosEdit::
widgetsToFillUnderPos()
{
  CQChartsFillUnderPos::Type xtype = CQChartsFillUnderPos::Type::NONE;

  int xtypeInd = xtypeCombo_->currentIndex();

  if      (xtypeInd == 0) xtype = CQChartsFillUnderPos::Type::NONE;
  else if (xtypeInd == 1) xtype = CQChartsFillUnderPos::Type::MIN;
  else if (xtypeInd == 2) xtype = CQChartsFillUnderPos::Type::MAX;
  else if (xtypeInd == 3) xtype = CQChartsFillUnderPos::Type::POS;

  double xpos = xposEdit_->value();

  CQChartsFillUnderPos::Type ytype = CQChartsFillUnderPos::Type::NONE;

  int ytypeInd = ytypeCombo_->currentIndex();

  if      (ytypeInd == 0) ytype = CQChartsFillUnderPos::Type::NONE;
  else if (ytypeInd == 1) ytype = CQChartsFillUnderPos::Type::MIN;
  else if (ytypeInd == 2) ytype = CQChartsFillUnderPos::Type::MAX;
  else if (ytypeInd == 3) ytype = CQChartsFillUnderPos::Type::POS;

  double ypos = yposEdit_->value();

  fillUnderPos_ = CQChartsFillUnderPos(xtype, xpos, ytype, ypos);

  //---

  emit fillUnderPosChanged();
}

void
CQChartsFillUnderPosEdit::
connectSlots(bool b)
{
  auto connectDisconnect = [&](bool b, QWidget *w, const char *from, const char *to) {
    if (b)
      connect(w, from, this, to);
    else
      disconnect(w, from, this, to);
  };

  connectDisconnect(b, xtypeCombo_, SIGNAL(currentIndexChanged(int)),
                    SLOT(widgetsToFillUnderPos()));
  connectDisconnect(b, xposEdit_, SIGNAL(valueChanged(double)),
                    SLOT(widgetsToFillUnderPos()));
  connectDisconnect(b, ytypeCombo_, SIGNAL(currentIndexChanged(int)),
                    SLOT(widgetsToFillUnderPos()));
  connectDisconnect(b, yposEdit_, SIGNAL(valueChanged(double)),
                    SLOT(widgetsToFillUnderPos()));
}

void
CQChartsFillUnderPosEdit::
drawPreview(QPainter *painter, const QRect &)
{
  QString str = (fillUnderPos().isValid() ? fillUnderPos().toString() : "<none>");

  drawCenteredText(painter, str);
}
