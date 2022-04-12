#include <CQChartsGlobalPropertiesWidget.h>
#include <CQChartsPropertyViewTree.h>
#include <CQChartsPropertyViewTreeFilterEdit.h>
#include <CQCharts.h>

#include <CQUtil.h>

#include <QVBoxLayout>

CQChartsGlobalPropertiesWidget::
CQChartsGlobalPropertiesWidget(CQCharts *charts)
{
  setObjectName("globalPropertiesWidget");

  CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  setCharts(charts);
}

void
CQChartsGlobalPropertiesWidget::
setCharts(CQCharts *charts)
{
  if (charts_ == charts)
    return;

  charts_ = charts;
  if (! charts_) return;

  //---

  auto *layout = qobject_cast<QVBoxLayout *>(this->layout());
  assert(layout);

  propertyTree_ = new CQChartsPropertyViewTree(this, charts->propertyModel());

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
CQChartsGlobalPropertiesWidget::
filterStateSlot(bool visible, bool focus)
{
  if (! filterEdit_) return;

  filterEdit_->setVisible(visible);

  if (focus)
    filterEdit_->setFocus();
}
