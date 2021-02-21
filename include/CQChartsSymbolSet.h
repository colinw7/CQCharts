#ifndef CQChartsSymbolSet_H
#define CQChartsSymbolSet_H

#include <CQChartsSymbol.h>

class CQChartsSymbolSet;
class CQCharts;

class CQChartsSymbolSetMgr {
 public:
  CQChartsSymbolSetMgr(CQCharts *charts);

  CQCharts *charts() const { return charts_; }

  //! add symbol set
  void addSymbolSet(CQChartsSymbolSet *symbolSet);

  //! get symbol set
  CQChartsSymbolSet *symbolSet(const QString &name) const;

  //! get nth symbol set
  CQChartsSymbolSet *symbolSet(int i) const;

  int numSymbolSets() const;

 private:
  using SymbolSets      = std::vector<CQChartsSymbolSet *>;
  using NamedSymbolSets = std::map<QString, CQChartsSymbolSet *>;

  CQCharts*       charts_;
  SymbolSets      symbolSets_;
  NamedSymbolSets namedSymbolSets_;
};

//---

class CQChartsSymbolSet {
 public:
  struct SymbolData {
    CQChartsSymbol symbol;
    bool           filled { false };

    SymbolData() = default;

    SymbolData(const CQChartsSymbol &symbol, bool filled) :
     symbol(symbol), filled(filled) {
    }
  };

 public:
  CQChartsSymbolSet(const QString &name);

  //! get name
  const QString &name() const { return name_; }

  //! add symbol
  void addSymbol(const CQChartsSymbol &symbol, bool filled);

  //! get nth symbol
  const SymbolData &symbol(int i) const;

  int numSymbols() const;

  SymbolData interpI(int i) const;

 private:
  using Symbols = std::vector<SymbolData>;

  QString name_;
  Symbols symbols_;
};

#endif
