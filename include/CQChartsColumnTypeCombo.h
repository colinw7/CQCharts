#ifndef CQChartsColumnTypeCombo_H
#define CQChartsColumnTypeCombo_H

#include <QComboBox>

class CQCharts;
class CQChartsColumnType;

class CQChartsColumnTypeCombo : public QComboBox {
  Q_OBJECT

 public:
  CQChartsColumnTypeCombo(QWidget *parent=nullptr, CQCharts *charts=nullptr);

  const CQCharts *charts() const { return charts_; }
  void setCharts(CQCharts *charts);

  const CQChartsColumnType *columnType() const;
  void setColumnType(const CQChartsColumnType *type);

 signals:
  void typeChanged();

 private:
  void init();

 private slots:
  void indexChangedSlot(int);

 private:
  CQCharts *charts_ { nullptr };
};

#endif
