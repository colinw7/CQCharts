#include <CQChartsPropertyViewTreeFilterEdit.h>
#include <CQChartsPropertyViewTree.h>
#include <CQChartsFilterEdit.h>

#include <CQUtil.h>

#include <QVBoxLayout>

CQChartsPropertyViewTreeFilterEdit::
CQChartsPropertyViewTreeFilterEdit(CQChartsPropertyViewTree *tree) :
 tree_(tree)
{
  setObjectName("filterEdit");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  filterEdit_ = new CQChartsFilterEdit;

  connect(filterEdit_, SIGNAL(replaceFilter(const QString &)),
          this, SLOT(replaceFilterSlot(const QString &)));
  connect(filterEdit_, SIGNAL(addFilter(const QString &)),
          this, SLOT(addFilterSlot(const QString &)));
  connect(filterEdit_, SIGNAL(escapePressed()), this, SLOT(hideFilterSlot()));

  connect(filterEdit_, SIGNAL(replaceSearch(const QString &)),
          this, SLOT(replaceSearchSlot(const QString &)));
  connect(filterEdit_, SIGNAL(addSearch(const QString &)),
          this, SLOT(addSearchSlot(const QString &)));

  layout->addWidget(filterEdit_);

  setFocusProxy(filterEdit_);
}

void
CQChartsPropertyViewTreeFilterEdit::
replaceFilterSlot(const QString &text)
{
  tree_->setFilter(text);
}

void
CQChartsPropertyViewTreeFilterEdit::
addFilterSlot(const QString &text)
{
//tree_->addFilter(text);
  tree_->setFilter(text);
}

void
CQChartsPropertyViewTreeFilterEdit::
replaceSearchSlot(const QString &text)
{
  tree_->search(text);
}

void
CQChartsPropertyViewTreeFilterEdit::
addSearchSlot(const QString &text)
{
//tree_->addSearch(text);
  tree_->search(text);
}

void
CQChartsPropertyViewTreeFilterEdit::
hideFilterSlot()
{
  tree_->setFilterDisplayed(false);
}
