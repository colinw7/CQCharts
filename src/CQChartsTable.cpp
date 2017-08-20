#include <CQChartsTable.h>
#include <CQChartsHeader.h>
#include <QSortFilterProxyModel>

CQChartsTable::
CQChartsTable(QWidget *parent) :
 QTableView(parent)
{
  setObjectName("table");

  setAlternatingRowColors(true);

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
setFilter(const QString &filter)
{
  QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model_);

  if (proxyModel)
    proxyModel->setFilterWildcard(filter);
}

void
CQChartsTable::
headerClickSlot(int section)
{
  emit columnClicked(section);
}

QSize
CQChartsTable::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("X")*40, 20*fm.height());
}
