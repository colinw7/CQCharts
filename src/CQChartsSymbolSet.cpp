#include <CQChartsSymbolSet.h>

CQChartsSymbolSetMgr::
CQChartsSymbolSetMgr(CQCharts *charts) :
 charts_(charts)
{
}

void
CQChartsSymbolSetMgr::
addSymbolSet(CQChartsSymbolSet *symbolSet)
{
  auto p = namedSymbolSets_.find(symbolSet->name());
  assert(p == namedSymbolSets_.end());

  symbolSets_     .push_back(symbolSet);
  namedSymbolSets_.insert(p, NamedSymbolSets::value_type(symbolSet->name(), symbolSet));
}

int
CQChartsSymbolSetMgr::
numSymbolSets() const
{
  return symbolSets_.size();
}

CQChartsSymbolSet *
CQChartsSymbolSetMgr::
symbolSet(const QString &name) const
{
  auto p = namedSymbolSets_.find(name);
  if (p == namedSymbolSets_.end()) return nullptr;

  return (*p).second;
}

CQChartsSymbolSet *
CQChartsSymbolSetMgr::
symbolSet(int i) const
{
  assert(i >= 0 && i < numSymbolSets());

  return symbolSets_[i];
}

//---

CQChartsSymbolSet::
CQChartsSymbolSet(const QString &name) :
 name_(name)
{
}

void
CQChartsSymbolSet::
addSymbol(const CQChartsSymbol &symbol, bool filled)
{
  symbols_.push_back(SymbolData(symbol, filled));
}

int
CQChartsSymbolSet::
numSymbols() const
{
  return symbols_.size();
}

const CQChartsSymbolSet::SymbolData &
CQChartsSymbolSet::
symbol(int i) const
{
  assert(i >= 0 && i < numSymbols());

  return symbols_[i];
}

CQChartsSymbolSet::SymbolData
CQChartsSymbolSet::
interpI(int i) const
{
  int i1;

  if (i < 0) {
    int i2 = -i - 1;
    int i3 = (i2 % numSymbols());

    i1 = numSymbols() - i3 - 1;
  }
  else {
    i1 = (i % numSymbols());
  }

  return symbol(i1);
}
