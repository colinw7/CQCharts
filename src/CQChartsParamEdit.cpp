#include <CQChartsParamEdit.h>

#include <CQChartsColorEdit.h>
#include <CQChartsLineEdit.h>

#include <CQIntegerSpin.h>
#include <CQCheckBox.h>
#include <CQUtil.h>

#include <QComboBox>
#include <QHBoxLayout>

CQChartsParamEdit::
CQChartsParamEdit(QWidget *parent) :
 QFrame(parent)
{
  layout_ = CQUtil::makeLayout<QHBoxLayout>(this, 0, 0);
}

void
CQChartsParamEdit::
setString(const QString &str)
{
  if (! edit_) {
    reset();

    edit_ = CQUtil::makeWidget<CQChartsLineEdit>("edit");

    layout_->addWidget(edit_);
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

    layout_->addWidget(check_);
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

    layout_->addWidget(ispin_);
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

    layout_->addWidget(combo_);
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

    layout_->addWidget(color_);
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

  return edit_->text().simplified();
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
