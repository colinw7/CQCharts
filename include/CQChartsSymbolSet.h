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
  explicit CQChartsSymbolSet(const QString &name);

  //! get name
  const QString &name() const { return name_; }

  //! add symbol
  void addSymbol(const CQChartsSymbol &symbol, bool filled=false);

  int numSymbols() const;

  SymbolData interpI(int i) const;

  CQChartsSymbol symbol(int i) const;

  bool isFilled(int i) const;

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
