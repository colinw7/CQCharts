#ifndef CQChartsSymbolSetsList_H
#define CQChartsSymbolSetsList_H

#include <CQChartsWidgetIFace.h>
#include <QListWidget>

class CQCharts;

class CQChartsSymbolSetsList : public QListWidget, public CQChartsWidgetIFace {
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName)

 public:
  CQChartsSymbolSetsList(CQCharts *charts=nullptr);

  CQCharts *charts() const { return charts_; }
  void setCharts(CQCharts *charts) override;

  const QString &name() const { return name_; }
  void setName(const QString &name);

 private slots:
  void rowChanged(int row);

 signals:
  void nameChanged(const QString &name);

 private:
  CQCharts* charts_ { nullptr };
  QString   name_;
};

#endif
