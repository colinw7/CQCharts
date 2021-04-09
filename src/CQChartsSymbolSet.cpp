#include <CQChartsSymbolSet.h>

CQChartsSymbolSetMgr::
CQChartsSymbolSetMgr(CQCharts *charts) :
 charts_(charts)
{
}

CQChartsSymbolSetMgr::
~CQChartsSymbolSetMgr()
{
  for (auto &symbolSet : symbolSets_)
    delete symbolSet;
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

bool
CQChartsSymbolSetMgr::
hasSymbolSet(const QString &name) const
{
  return symbolSet(name);
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

QStringList
CQChartsSymbolSetMgr::
symbolSetNames() const
{
  QStringList names;

  for (const auto &symbolSet : symbolSets_)
    names.push_back(symbolSet->name());

  return names;
}

//---

CQChartsSymbolSet::
CQChartsSymbolSet(const QString &name) :
 name_(name)
{
}

void
CQChartsSymbolSet::
addSymbol(const CQChartsSymbol &symbol)
{
  assert(symbol.isValid());

  symbols_.push_back(SymbolData(symbol));
}

int
CQChartsSymbolSet::
numSymbols() const
{
  return symbols_.size();
}

const CQChartsSymbolSet::SymbolData &
CQChartsSymbolSet::
symbolData(int i) const
{
  assert(i >= 0 && i < numSymbols());

  return symbols_[i];
}

CQChartsSymbolSet::SymbolData
CQChartsSymbolSet::
interpI(int i) const
{
  return interpI(i, 0, numSymbols() - 1);
}

CQChartsSymbolSet::SymbolData
CQChartsSymbolSet::
interpI(int i, int imin, int imax) const
{
  int i1 = interpIRange(i, imin, imax);

  return symbolData(i1);
}

int
CQChartsSymbolSet::
interpIRange(int i, int imin, int imax)
{
  int ii = i - imin;
  int ni = imax - imin + 1;

  assert(ni > 0);

  int i1;

  if (ii < 0) {
    int i2 = -ii - 1;
    int i3 = (i2 % ni);

    i1 = ni - i3 - 1;
  }
  else {
    i1 = (ii % ni);
  }

  return i1 + imin;
}

CQChartsSymbol
CQChartsSymbolSet::
symbol(int i) const
{
  if (i < 0 || i >= numSymbols())
    return CQChartsSymbol();

  return symbols_[i].symbol;
}

QStringList
CQChartsSymbolSet::
symbolNames() const
{
  QStringList names;

  for (const auto &symbolData : symbols_)
    names.push_back(symbolData.symbol.toString());

  return names;
}

bool
CQChartsSymbolSet::
moveUp(int i)
{
  if (i <= 0 || i >= numSymbols())
    return false;

  std::swap(symbols_[i - 1], symbols_[i]);

  return true;
}

bool
CQChartsSymbolSet::
moveDown(int i)
{
  if (i < 0 || i >= numSymbols() - 1)
    return false;

  std::swap(symbols_[i], symbols_[i + 1]);

  return true;
}
