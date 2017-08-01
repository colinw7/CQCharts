#include <CQChartsTable.h>
#include <CQChartsHeader.h>

CQChartsTable::
CQChartsTable(QWidget *parent) :
 QTableView(parent)
{
  setObjectName("table");

  header_ = new CQChartsHeader(this);

  header_->setSectionsClickable(true);
  header_->setHighlightSections(true);

  setHorizontalHeader(header_);

  connect(header_, SIGNAL(sectionClicked(int)), this, SLOT(headerClickSlot(int)));
}

void
CQChartsTable::
setModel(QAbstractItemModel *model)
{
  QTableView::setModel(model);
}

void
CQChartsTable::
headerClickSlot(int section)
{
  emit columnClicked(section);
}
