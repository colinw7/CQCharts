#include <CQChartsSymbolSetsList.h>
#include <CQChartsSymbolSet.h>
#include <CQCharts.h>

CQChartsSymbolSetsList::
CQChartsSymbolSetsList(CQCharts *charts)
{
  setObjectName("symbolSetsList");

  setToolTip("Symbol Sets");

  setCharts(charts);
}

void
CQChartsSymbolSetsList::
setCharts(CQCharts *charts)
{
  if (charts_ != charts) {
    charts_ = charts;

    if (charts_) {
      auto *symbolSetMgr = charts_->symbolSetMgr();

      auto names = symbolSetMgr->symbolSetNames();

      addItems(names);

      name_ = names.at(0);

      connect(this, SIGNAL(currentRowChanged(int)), this, SLOT(rowChanged(int)));
    }
  }
}

void
CQChartsSymbolSetsList::
setName(const QString &name)
{
  auto items = findItems(name, Qt::MatchExactly);
  if (! items.length()) return;

  setCurrentItem(items.at(0));
}

void
CQChartsSymbolSetsList::
rowChanged(int row)
{
  auto *item = this->item(row);
  if (! item) return;

  name_ = item->text();

  Q_EMIT nameChanged(name_);
}
