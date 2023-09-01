#include <CQChartsParamEdit.h>

#include <CQChartsColorEdit.h>
#include <CQChartsLineEdit.h>

#include <CQIntegerSpin.h>
#include <CQCheckBox.h>
#include <CQIconButton.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QHBoxLayout>

CQChartsParamEdit::
CQChartsParamEdit(QWidget *parent) :
 QFrame(parent)
{
  layout_ = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);

  editFrame_  = CQUtil::makeWidget<QFrame>("editFrame");
  editLayout_ = CQUtil::makeLayout<QHBoxLayout>(editFrame_, 0, 0);

  layout_->addWidget(editFrame_);

  clearButton_ = CQUtil::makeWidget<CQIconButton>("clearButton");

  clearButton_->setIcon("CLEAR_BUTTON");

  connect(clearButton_, SIGNAL(clicked()), this, SLOT(clearSlot()));

  layout_->addWidget(clearButton_);
}

void
CQChartsParamEdit::
setString(const QString &str)
{
  if (! edit_) {
    reset();

    edit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

    editLayout_->addWidget(edit_);
  }

  edit_->setText(str);

  type_ = CQBaseModelType::STRING;
}

void
CQChartsParamEdit::
setBool(bool b)
{
  if (! check_) {
    reset();

    check_ = CQUtil::makeWidget<CQCheckBox>("edit");

    editLayout_->addWidget(check_);
  }

  check_->setChecked(b);

  type_ = CQBaseModelType::BOOLEAN;
}

void
CQChartsParamEdit::
setInteger(int i)
{
  if (! ispin_) {
    reset();

    ispin_ = CQUtil::makeWidget<CQIntegerSpin>("edit");

    editLayout_->addWidget(ispin_);
  }

  ispin_->setValue(i);

  type_ = CQBaseModelType::INTEGER;
}

void
CQChartsParamEdit::
setEnum(const QString &str, const QStringList &values)
{
  if (! combo_) {
    reset();

    combo_ = CQUtil::makeWidget<QComboBox>("edit");

    editLayout_->addWidget(combo_);
  }

  // make optional
  QStringList values1;
  values1 << "";
  values1 << values;

  combo_->clear();
  combo_->addItems(values1);

  int pos = combo_->findText(str);
  if (pos < 0) pos = 0;

  combo_->setCurrentIndex(pos);

  type_ = CQBaseModelType::ENUM;
}

void
CQChartsParamEdit::
setColor(const QString &str)
{
  if (! color_) {
    reset();

    color_ = CQUtil::makeWidget<CQChartsColorLineEdit>("edit");

    editLayout_->addWidget(color_);
  }

  CQChartsColor c(str);

  color_->setColor(c);

  type_ = CQBaseModelType::COLOR;
}

QString
CQChartsParamEdit::
getString() const
{
  assert(edit_);

  return edit_->text().trimmed();
}

bool
CQChartsParamEdit::
getBool() const
{
  assert(check_);

  return check_->isChecked();
}

int
CQChartsParamEdit::
getInteger() const
{
  assert(ispin_);

  return ispin_->value();
}

QString
CQChartsParamEdit::
getEnum() const
{
  assert(combo_);

  return combo_->currentText();
}

QString
CQChartsParamEdit::
getColor() const
{
  assert(color_);

  return color_->color().toString();
}

void
CQChartsParamEdit::
updateTip(const QString &tip)
{
  setToolTip(tip);

  if (color_) {
    color_->setToolTip(tip);

    color_->updateTip();
  }
}

void
CQChartsParamEdit::
reset()
{
  delete edit_;
  delete ispin_;
  delete check_;
  delete combo_;
  delete color_;

  edit_  = nullptr;
  ispin_ = nullptr;
  check_ = nullptr;
  combo_ = nullptr;
  color_ = nullptr;
}

void
CQChartsParamEdit::
clearSlot()
{
  setString("");
}
