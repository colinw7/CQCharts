#ifndef CQChartsSymbolSet_H
#define CQChartsSymbolSet_H

#include <CQChartsSymbol.h>
#include <QObject>

class CQChartsSymbolSet;
class CQCharts;

class CQChartsSymbolSetMgr : public QObject {
  Q_OBJECT

 public:
  explicit CQChartsSymbolSetMgr(CQCharts *charts);

 ~CQChartsSymbolSetMgr();

  CQCharts *charts() const { return charts_; }

  //! add symbol set
  void addSymbolSet(CQChartsSymbolSet *symbolSet);

  //! has symbol set of name
  bool hasSymbolSet(const QString &name) const;

  //! get symbol set by name
  CQChartsSymbolSet *symbolSet(const QString &name) const;

  //! get nth symbol set
  CQChartsSymbolSet *symbolSet(int i) const;

  int numSymbolSets() const;

  QStringList symbolSetNames() const;

 private:
  using SymbolSets      = std::vector<CQChartsSymbolSet *>;
  using NamedSymbolSets = std::map<QString, CQChartsSymbolSet *>;

  CQCharts*       charts_ { nullptr };
  SymbolSets      symbolSets_;
  NamedSymbolSets namedSymbolSets_;
};

//---

class CQChartsSymbolSet {
 public:
  struct SymbolData {
    CQChartsSymbol symbol;

    SymbolData() = default;

    explicit SymbolData(const CQChartsSymbol &symbol) :
     symbol(symbol) {
    }
  };

 public:
  CQChartsSymbolSet() = default;

  explicit CQChartsSymbolSet(const QString &name);

  //! get name
  const QString &name() const { return name_; }

  //! add symbol
  CQChartsSymbol addSymbol(const CQChartsSymbol &symbol);

  int numSymbols() const;

  SymbolData interpI(int i) const;
  SymbolData interpI(int i, int imin, int imax) const;

  static int interpIRange(int i, int imin, int imax);

  CQChartsSymbol symbol(int i) const;

  //! get nth symbol
  const SymbolData &symbolData(int i) const;

  QStringList symbolNames() const;

  //---

  bool moveUp  (int i);
  bool moveDown(int i);

 private:
  using Symbols = std::vector<SymbolData>;

  QString name_;
  Symbols symbols_;
};

#endif
