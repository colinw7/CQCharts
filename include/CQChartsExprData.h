#ifndef CQChartsExprData_H
#define CQChartsExprData_H

#include <QSortFilterProxyModel>

class CQCharts;
class CQDataModel;
class CQExprModel;

class CQChartsExprData : public QSortFilterProxyModel {
  Q_OBJECT

 public:
  CQChartsExprData(CQCharts *charts, int nc=1, int nr=100);
 ~CQChartsExprData();

  CQDataModel *dataModel() const { return dataModel_; }
  CQExprModel *exprModel() const { return exprModel_; }

  //---

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                     int role=Qt::DisplayRole) override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const;

 private:
  CQCharts*    charts_    { nullptr };
  CQDataModel* dataModel_ { nullptr };
  CQExprModel* exprModel_ { nullptr };
};

#endif
