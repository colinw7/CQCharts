#include <CQChartsFilterEdit.h>
#include <CQHistoryLineEdit.h>
#include <CQIconCombo.h>

#include <QLineEdit>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QButtonGroup>

#include <svg/filter_light_svg.h>
#include <svg/filter_dark_svg.h>
#include <svg/search_light_svg.h>
#include <svg/search_dark_svg.h>

CQChartsFilterEdit::
CQChartsFilterEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("filterEdit");

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  edit_ = new CQHistoryLineEdit;

  edit_->setObjectName("edit");

  edit_->setAutoClear(false);
  edit_->setClearButtonEnabled(true);

  connect(edit_, SIGNAL(exec(const QString &)), this, SLOT(acceptSlot(const QString &)));

  layout->addWidget(edit_);

  combo_ = new CQIconCombo;

  combo_->setObjectName("combo");

  combo_->addItem(CQPixmapCacheInst->getIcon("FILTER_LIGHT", "FILTER_DARK"), "Filter");
  combo_->addItem(CQPixmapCacheInst->getIcon("SEARCH_LIGHT", "SEARCH_DARK"), "Search");

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboSlot(int)));

  layout->addWidget(combo_);

  QFrame *opFrame = new QFrame;

  opFrame->setObjectName("opFrame");

  QHBoxLayout *opLayout = new QHBoxLayout(opFrame);
  opLayout->setMargin(0); opLayout->setSpacing(2);

  QButtonGroup *opButtonGroup = new QButtonGroup(this);

  replaceButton_ = new QRadioButton("Replace");
  addButton_     = new QRadioButton("Add");

  replaceButton_->setObjectName("replace");
  addButton_    ->setObjectName("add");

  opLayout->addWidget(replaceButton_);
  opLayout->addWidget(addButton_);

  replaceButton_->setChecked(true);

  opButtonGroup->addButton(replaceButton_);
  opButtonGroup->addButton(addButton_);

  layout->addWidget(opFrame);
}

void
CQChartsFilterEdit::
comboSlot(int /*ind*/)
{
  edit_->setText("");
}

void
CQChartsFilterEdit::
acceptSlot(const QString &text)
{
  if (combo_->currentIndex() == 0) {
    if (replaceButton_->isChecked())
      emit replaceFilter(text);
    else
      emit addFilter(text);
  }
  else {
    if (replaceButton_->isChecked())
      emit replaceSearch(text);
    else
      emit addSearch(text);
  }
}
