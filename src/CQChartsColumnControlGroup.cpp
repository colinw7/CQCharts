#include <CQChartsColumnControlGroup.h>
#include <CQChartsWidgetUtil.h>
#include <CQChartsIconButton.h>

#include <CQGroupBox.h>
#include <CQUtil.h>

#include <QButtonGroup>
#include <QRadioButton>
#include <QCheckBox>
#include <QStackedWidget>
#include <QVBoxLayout>

#include <svg/legend_svg.h>
#include <svg/columns_svg.h>

CQChartsColumnControlGroup::
CQChartsColumnControlGroup(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("columnControlGroup");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  groupBox_ = CQUtil::makeWidget<CQGroupBox>("group");

  layout->addWidget(groupBox_);

  auto *groupLayout = CQUtil::makeLayout<QVBoxLayout>(groupBox_, 0, 0);

  //---

  // corner buttons
  auto *cornerControl = CQUtil::makeWidget<QFrame>("cornerControl");
  auto *cornerLayout  = CQUtil::makeLayout<QHBoxLayout>(cornerControl, 0, 0);

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

#if 0
  keyCheck_ = CQUtil::makeLabelWidget<QCheckBox>("Key", "keyCheck");

  connect(keyCheck_, SIGNAL(stateChanged(int)), this, SLOT(keyCheckSlot()));
#else
  keyCheck_ = CQUtil::makeWidget<CQChartsIconButton>("keyCheck");

  keyCheck_->setCheckable(true);

  keyCheck_->setIcon("LEGEND");
  keyCheck_->setToolTip("Display Key for column values");

  connect(keyCheck_, SIGNAL(clicked()), this, SLOT(keyCheckSlot()));
#endif

  cornerLayout->addWidget(keyCheck_);

  groupBox_->setCornerWidget(cornerControl);

  keyCheck_->setVisible(false);

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

void
CQChartsColumnControlGroup::
controlButtonClicked(QAbstractButton *button)
{
  int ind = (button->text() == "Global" ? 0 : 1);

  stack_->setCurrentIndex(ind);

  if (hasKey())
    keyCheck_->setVisible(ind == 1);

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

  disconnect(radioGroup_, SIGNAL(buttonClicked(QAbstractButton *)),
             this, SLOT(controlButtonClicked(QAbstractButton *)));

  radioGroup_->buttons().at(ind)->setChecked(true);

  if (hasKey())
    keyCheck_->setVisible(ind == 1);

  connect(radioGroup_, SIGNAL(buttonClicked(QAbstractButton *)),
          this, SLOT(controlButtonClicked(QAbstractButton *)));
}
