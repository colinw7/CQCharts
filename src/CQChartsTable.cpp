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
setModel(const ModelP &model)
{
  model_ = model;

  QTableView::setModel(model_.data());
}

void
CQChartsTable::
setFilter(const QString &filter)
{
  if (! model_)
    return;

  QSortFilterProxyModel *proxyModel = qobject_cast<QSortFilterProxyModel *>(model_.data());
  assert(proxyModel);

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
