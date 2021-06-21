#include <CQChartsColumnControlGroup.h>
#include <CQChartsWidgetUtil.h>

#include <CQGroupBox.h>
#include <CQIconButton.h>
#include <CQUtil.h>

//#include <QButtonGroup>
//#include <QRadioButton>
#include <QCheckBox>
#include <QStackedWidget>
#include <QVBoxLayout>

CQChartsColumnControlGroup::
CQChartsColumnControlGroup(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("columnControlGroup");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  groupBox_ = CQUtil::makeWidget<CQGroupBox>("group");

  groupBox_->setTitleScale(0.85);
  groupBox_->setTitleColored(true);

  layout->addWidget(groupBox_);

  auto *groupLayout = CQUtil::makeLayout<QVBoxLayout>(groupBox_, 0, 0);

  //---

  // corner buttons
  auto *cornerControl = CQUtil::makeWidget<QFrame>("cornerControl");
  auto *cornerLayout  = CQUtil::makeLayout<QHBoxLayout>(cornerControl, 0, 0);

  cornerControl->setAutoFillBackground(true);

#if 0
  radioGroup_ = new QButtonGroup(this);

  auto *globalRadio = CQUtil::makeLabelWidget<QRadioButton>("Global", "global");
  auto *columnRadio = CQUtil::makeLabelWidget<QRadioButton>("Column", "column");

  globalRadio->setToolTip("Apply value to all");
  columnRadio->setToolTip("Use column data for each value");

  globalRadio->setChecked(true);

  radioGroup_->addButton(globalRadio);
  radioGroup_->addButton(columnRadio);

  connect(radioGroup_, SIGNAL(buttonClicked(QAbstractButton *)),
          this, SLOT(controlButtonClicked(QAbstractButton *)));

  cornerLayout->addWidget(globalRadio);
  cornerLayout->addWidget(columnRadio);

  cornerLayout->addStretch(1);

  auto *keySpacer = CQChartsWidgetUtil::createHSpacer(2);
  cornerLayout->addWidget(keySpacer);
#else
  columnCheck_ = CQUtil::makeWidget<CQIconButton>("columnCheck");

  columnCheck_->setCheckable(true);
  columnCheck_->setSize(CQIconButton::Size::SMALL);
  columnCheck_->setIcon("COLUMNS");
  columnCheck_->setToolTip("Use column values");

  connect(columnCheck_, SIGNAL(clicked()), this, SLOT(columnCheckSlot()));

  cornerLayout->addWidget(columnCheck_);
#endif

#if 0
  keyCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Key", "keyCheck");

  connect(keyCheck_, SIGNAL(stateChanged(int)), this, SLOT(keyCheckSlot()));
#else
  keyCheck_ = CQUtil::makeWidget<CQIconButton>("keyCheck");

  keyCheck_->setCheckable(true);
  keyCheck_->setSize(CQIconButton::Size::SMALL);
  keyCheck_->setIcon("LEGEND");
  keyCheck_->setToolTip("Display Key for column values");

  connect(keyCheck_, SIGNAL(clicked()), this, SLOT(keyCheckSlot()));
#endif

  cornerLayout->addWidget(keyCheck_);

  groupBox_->setCornerWidget(cornerControl);

//keyCheck_->setVisible(false);
  keyCheck_->setEnabled(false);

  //---

  // stack
  stack_ = CQUtil::makeWidget<QStackedWidget>("stack");

  groupLayout->addWidget(stack_);

  globalWidget_ = CQUtil::makeWidget<QFrame>("globalFrame");
  columnWidget_ = CQUtil::makeWidget<QFrame>("columnFrame");

  CQUtil::makeLayout<QVBoxLayout>(globalWidget_, 2, 2);
  CQUtil::makeLayout<QVBoxLayout>(columnWidget_, 2, 2);

  stack_->addWidget(globalWidget_);
  stack_->addWidget(columnWidget_);
}

const QString &
CQChartsColumnControlGroup::
title() const
{
  return groupBox_->title();
}

void
CQChartsColumnControlGroup::
setTitle(const QString &s)
{
  groupBox_->setTitle(s);
}

#if 0
void
CQChartsColumnControlGroup::
controlButtonClicked(QAbstractButton *button)
{
  int ind = (button->text() == "Global" ? 0 : 1);

  setColumnStack(ind == 1);
}
#endif

void
CQChartsColumnControlGroup::
columnCheckSlot()
{
  setColumnStack(columnCheck_->isChecked());
}

void
CQChartsColumnControlGroup::
setColumnStack(bool b)
{
  stack_->setCurrentIndex(b ? 1 : 0);

  if (hasKey()) {
  //keyCheck_->setVisible(b);
    keyCheck_->setEnabled(b);
  }

  emit groupChanged();
}

void
CQChartsColumnControlGroup::
keyCheckSlot()
{
  emit showKey(isKeyVisible());
}

bool
CQChartsColumnControlGroup::
isKeyVisible() const
{
  return keyCheck_->isChecked();
}

void
CQChartsColumnControlGroup::
addFixedWidget(QWidget *w)
{
  auto *layout = qobject_cast<QVBoxLayout *>(globalWidget_->layout());

  layout->addWidget(w);
}

void
CQChartsColumnControlGroup::
addColumnWidget(QWidget *w)
{
  auto *layout = qobject_cast<QVBoxLayout *>(columnWidget_->layout());

  layout->addWidget(w);
}

bool
CQChartsColumnControlGroup::
isFixed() const
{
  return (stack_->currentIndex() == 0);
}

void
CQChartsColumnControlGroup::
setFixed()
{
  setCurrentIndex(0);
}

bool
CQChartsColumnControlGroup::
isColumn() const
{
  return (stack_->currentIndex() == 1);
}

void
CQChartsColumnControlGroup::
setColumn()
{
  setCurrentIndex(1);
}

void
CQChartsColumnControlGroup::
setCurrentIndex(int ind)
{
  stack_->setCurrentIndex(ind);

#if 0
  disconnect(radioGroup_, SIGNAL(buttonClicked(QAbstractButton *)),
             this, SLOT(controlButtonClicked(QAbstractButton *)));

  radioGroup_->buttons().at(ind)->setChecked(true);
#else
  disconnect(columnCheck_, SIGNAL(clicked()), this, SLOT(columnCheckSlot()));

  columnCheck_->setChecked(ind == 1);
#endif

  if (hasKey()) {
  //keyCheck_->setVisible(ind == 1);
    keyCheck_->setEnabled(ind == 1);
  }

#if 0
  connect(radioGroup_, SIGNAL(buttonClicked(QAbstractButton *)),
          this, SLOT(controlButtonClicked(QAbstractButton *)));
#else
  connect(columnCheck_, SIGNAL(clicked()), this, SLOT(columnCheckSlot()));
#endif
}
