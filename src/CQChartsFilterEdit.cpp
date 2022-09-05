#include <CQChartsFilterEdit.h>

#include <CQIconCombo.h>
#include <CQSwitch.h>
#include <CQUtil.h>
#include <CQPixmapCache.h>

#include <QHBoxLayout>
#include <QKeyEvent>

CQChartsFilterEdit::
CQChartsFilterEdit(QWidget *parent) :
 QFrame(parent)
{
  setObjectName("filterEdit");

  setToolTip("Filter");

  //---

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 0, 2);

  edit_ = new CQChartsFilterEditEdit;

  connect(edit_, SIGNAL(returnPressed()), this, SLOT(acceptSlot()));
  connect(edit_, SIGNAL(escapePressed()), this, SLOT(escapeSlot()));

  layout->addWidget(edit_);

  combo_ = CQUtil::makeWidget<CQIconCombo>("combo");

  combo_->addItem(CQPixmapCacheInst->getLightDarkIcon("FILTER"), "Filter");
  combo_->addItem(CQPixmapCacheInst->getLightDarkIcon("SEARCH"), "Search");
  combo_->setFocusPolicy(Qt::NoFocus);

  connect(combo_, SIGNAL(currentIndexChanged(int)), this, SLOT(comboSlot(int)));

  layout->addWidget(combo_);

  auto *opFrame  = CQUtil::makeWidget<QFrame>("opFrame");
  auto *opLayout = CQUtil::makeLayout<QHBoxLayout>(opFrame, 0, 2);

  addReplaceSwitch_ = new CQSwitch("Replace", "Add");

  addReplaceSwitch_->setObjectName("add_replace");
  addReplaceSwitch_->setChecked(true);
  addReplaceSwitch_->setHighlightOn(false);
  addReplaceSwitch_->setFocusPolicy(Qt::NoFocus);
  addReplaceSwitch_->setToolTip("Add To or Replace Filter");

  opLayout->addWidget(addReplaceSwitch_);

  andOrSwitch_ = new CQSwitch("And", "Or");

  andOrSwitch_->setObjectName("and");
  andOrSwitch_->setChecked(true);
  andOrSwitch_->setHighlightOn(false);
  andOrSwitch_->setFocusPolicy(Qt::NoFocus);
  andOrSwitch_->setToolTip("Combine with And or Or");

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

  Q_EMIT filterAnd(b);
}

void
CQChartsFilterEdit::
acceptSlot()
{
  auto text = edit_->text();

  if (combo_->currentIndex() == 0) {
    if (text != filterText_) {
      filterText_ = text;

      filterDetails_.clear();

      if (addReplaceSwitch_->isChecked())
        Q_EMIT replaceFilter(filterText_);
      else
        Q_EMIT addFilter(filterText_);
    }
  }
  else {
    if (text != searchText_) {
      searchText_ = text;

      if (addReplaceSwitch_->isChecked())
        Q_EMIT replaceSearch(searchText_);
      else
        Q_EMIT addSearch(searchText_);
    }
  }
}

void
CQChartsFilterEdit::
escapeSlot()
{
  Q_EMIT escapePressed();
}

//------

CQChartsFilterEditEdit::
CQChartsFilterEditEdit(QWidget *parent) :
 CQChartsLineEdit(parent)
{
  setObjectName("edit");

  setToolTip("Filter");
}

void
CQChartsFilterEditEdit::
keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Escape) {
    Q_EMIT escapePressed();
    return;
  }

  CQChartsLineEdit::keyPressEvent(e);
}
