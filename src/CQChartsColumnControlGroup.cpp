#include <CQChartsColumnControlGroup.h>
#include <CQGroupBox.h>
#include <CQUtil.h>

#include <QButtonGroup>
#include <QRadioButton>
#include <QStackedWidget>
#include <QVBoxLayout>

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

  globalRadio->setChecked(true);

  radioGroup_->addButton(globalRadio);
  radioGroup_->addButton(columnRadio);

  connect(radioGroup_, SIGNAL(buttonClicked(QAbstractButton *)),
          this, SLOT(controlButtonClicked(QAbstractButton *)));

  cornerLayout->addWidget(globalRadio);
  cornerLayout->addWidget(columnRadio);
  cornerLayout->addStretch(1);

  groupBox_->setCornerWidget(cornerControl);

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
  if (button->text() == "Global")
    stack_->setCurrentIndex(0);
  else
    stack_->setCurrentIndex(1);

  emit groupChanged();
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

  connect(radioGroup_, SIGNAL(buttonClicked(QAbstractButton *)),
          this, SLOT(controlButtonClicked(QAbstractButton *)));
}
