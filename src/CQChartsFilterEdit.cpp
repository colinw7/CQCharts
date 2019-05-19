#include <CQChartsFilterEdit.h>
#include <CQIconCombo.h>
#include <CQSwitch.h>
#include <CQUtil.h>

#include <QHBoxLayout>
#include <QButtonGroup>
#include <QKeyEvent>

#include <svg/filter_light_svg.h>
#include <svg/filter_dark_svg.h>
#include <svg/search_light_svg.h>
#include <svg/search_dark_svg.h>

CQChartsFilterEdit::
CQChartsFilterEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("filterEdit");

  QHBoxLayout *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  edit_ = new CQChartsFilterEditEdit;

  connect(edit_, SIGNAL(returnPressed()), this, SLOT(acceptSlot()));
  connect(edit_, SIGNAL(escapePressed()), this, SLOT(escapeSlot()));

  layout->addWidget(edit_);

  combo_ = CQUtil::makeWidget<CQIconCombo>("combo");

  combo_->addItem(CQPixmapCacheInst->getIcon("FILTER_LIGHT", "FILTER_DARK"), "Filter");
  combo_->addItem(CQPixmapCacheInst->getIcon("SEARCH_LIGHT", "SEARCH_DARK"), "Search");
  combo_->setFocusPolicy(Qt::NoFocus);

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboSlot(int)));

  layout->addWidget(combo_);

  QFrame *opFrame = CQUtil::makeWidget<QFrame>("opFrame");

  QHBoxLayout *opLayout = CQUtil::makeLayout<QHBoxLayout>(opFrame, 0, 2);

  addReplaceSwitch_ = new CQSwitch("Replace", "Add");

  addReplaceSwitch_->setObjectName("add_replace");
  addReplaceSwitch_->setChecked(true);
  addReplaceSwitch_->setHighlightOn(false);
  addReplaceSwitch_->setFocusPolicy(Qt::NoFocus);

  opLayout->addWidget(addReplaceSwitch_);

  andOrSwitch_ = new CQSwitch("And", "Or");

  andOrSwitch_->setObjectName("and");
  andOrSwitch_->setChecked(true);
  andOrSwitch_->setHighlightOn(false);
  andOrSwitch_->setFocusPolicy(Qt::NoFocus);

  connect(andOrSwitch_, SIGNAL(toggled(bool)), this, SLOT(andSlot()));

  opLayout->addWidget(andOrSwitch_);

  layout->addWidget(opFrame);

  setFocusProxy(edit_);
}

void
CQChartsFilterEdit::
setFilterDetails(const QString &str)
{
  filterDetails_ = str;

  edit_->setToolTip(filterDetails_);
}

void
CQChartsFilterEdit::
setSearchDetails(const QString &str)
{
  searchDetails_ = str;

  edit_->setToolTip(searchDetails_);
}

void
CQChartsFilterEdit::
comboSlot(int /*ind*/)
{
  if (combo_->currentIndex() == 0) {
    edit_->setText(filterText_);

    edit_->setToolTip(filterDetails_);
  }
  else {
    edit_->setText(searchText_);

    edit_->setToolTip(searchDetails_);
  }
}

void
CQChartsFilterEdit::
andSlot()
{
  bool b = andOrSwitch_->isChecked();

  emit filterAnd(b);
}

void
CQChartsFilterEdit::
acceptSlot()
{
  QString text = edit_->text();

  if (combo_->currentIndex() == 0) {
    if (text != filterText_) {
      filterText_    = text;
      filterDetails_ = "";

      if (addReplaceSwitch_->isChecked())
        emit replaceFilter(filterText_);
      else
        emit addFilter(filterText_);
    }
  }
  else {
    if (text != searchText_) {
      searchText_ = text;

      if (addReplaceSwitch_->isChecked())
        emit replaceSearch(searchText_);
      else
        emit addSearch(searchText_);
    }
  }
}

void
CQChartsFilterEdit::
escapeSlot()
{
  emit escapePressed();
}

//------

CQChartsFilterEditEdit::
CQChartsFilterEditEdit(QWidget *parent) :
 CQLineEdit(parent)
{
  setObjectName("edit");
}

void
CQChartsFilterEditEdit::
keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Escape) {
    emit escapePressed();
    return;
  }

  CQLineEdit::keyPressEvent(e);
}
