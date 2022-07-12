#ifndef CQChartsSymbolsList_H
#define CQChartsSymbolsList_H

#include <CQChartsSymbol.h>
#include <CQChartsWidgetIFace.h>

#include <QListWidget>
#include <QFrame>

class CQChartsSymbolsItemDelegate;
class CQChartsSymbolSet;
class CQCharts;

class CQChartsSymbolsList : public QListWidget, public CQChartsWidgetIFace {
  Q_OBJECT

  Q_PROPERTY(QString        setName READ setName WRITE setSetName)
  Q_PROPERTY(CQChartsSymbol symbol  READ symbol  WRITE setSymbol)

 public:
  using Symbol = CQChartsSymbol;

 public:
  CQChartsSymbolsList(CQCharts *charts=nullptr);

  CQCharts *charts() const { return charts_; }
  void setCharts(CQCharts *charts) override;

  const QString &setName() const { return setName_; }
  void setSetName(const QString &name);

  const Symbol &symbol() const { return symbol_; }
  void setSymbol(const Symbol &s);

  Symbol symbolAt(int ind) const;

  CQChartsSymbolSet *symbolSet() const;

  void moveCurrentUp();
  void moveCurrentDown();

  QListWidgetItem *currentItem() const;

  bool selectedSymbol(Symbol &symbol) const;

 public Q_SLOTS:
  void setSetNameSlot(const QString &s);

 Q_SIGNALS:
  void setNameChanged(const QString &name);
  void symbolChanged();

 private:
  void updateItems();

  void setCurrentSymbol();

 private Q_SLOTS:
  void selectionChangeSlot();

 private:
  CQCharts*                    charts_   { nullptr };
  QString                      setName_;
  Symbol                       symbol_;
  CQChartsSymbolsItemDelegate* delegate_ { nullptr };
};

//---

class CQChartsSymbolsListControl : public QFrame, public CQChartsWidgetIFace {
  Q_OBJECT

  Q_PROPERTY(QString        setName READ setName WRITE setSetName)
  Q_PROPERTY(CQChartsSymbol symbol READ symbol WRITE setSymbol)

 public:
  using Symbol = CQChartsSymbol;

 public:
  CQChartsSymbolsListControl(CQCharts *charts=nullptr);

  CQCharts *charts() const;
  void setCharts(CQCharts *charts) override;

  const QString &setName() const;
  void setSetName(const QString &name);

  const Symbol &symbol() const;
  void setSymbol(const Symbol &s);

 public Q_SLOTS:
  void setSetNameSlot(const QString &s);

 Q_SIGNALS:
  void setNameChanged(const QString &name);
  void symbolChanged();

 private Q_SLOTS:
  void symbolUpSlot();
  void symbolDownSlot();

 private:
  CQChartsSymbolsList *symbolsList_ { nullptr };
};

#endif
