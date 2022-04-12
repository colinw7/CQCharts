#include <CQChartsObjectPropertiesWidget.h>
#include <CQChartsPropertyViewTree.h>
#include <CQChartsPropertyViewTreeFilterEdit.h>

#include <CQUtil.h>

#include <QVBoxLayout>

CQChartsObjectPropertiesWidget::
CQChartsObjectPropertiesWidget()
{
  setObjectName("objectPropertiesWidget");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  //--

  propertyTree_ = new CQChartsPropertyViewTree(this, nullptr);

  propertyTree_->setObjectName("propertyTree");

  connect(propertyTree_, SIGNAL(itemSelected(QObject *, const QString &)),
          this, SIGNAL(propertyItemSelected(QObject *, const QString &)));

  connect(propertyTree_, SIGNAL(filterStateChanged(bool, bool)),
          this, SLOT(filterStateSlot(bool, bool)));

  //--

  filterEdit_ = new CQChartsPropertyViewTreeFilterEdit(propertyTree_);

  filterEdit_->setVisible(propertyTree_->isFilterDisplayed());

  //--

  layout->addWidget(filterEdit_);
  layout->addWidget(propertyTree_);
}

void
CQChartsObjectPropertiesWidget::
filterStateSlot(bool visible, bool focus)
{
  filterEdit_->setVisible(visible);

  if (focus)
    filterEdit_->setFocus();
}
