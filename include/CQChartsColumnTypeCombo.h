#ifndef CQChartsColumnTypeCombo_H
#define CQChartsColumnTypeCombo_H

#include <QComboBox>

class CQCharts;
class CQChartsColumnType;

/*!
 * \brief Column Type Combo Chooser
 * \ingroup Charts
 */
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
