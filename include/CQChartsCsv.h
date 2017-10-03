#ifndef CQChartsCsv_H
#define CQChartsCsv_H

#include <CQChartsModel.h>

class CQCsvModel;
class QSortFilterProxyModel;

class CQChartsCsv : public CQChartsModel {
  Q_OBJECT

 public:
  CQChartsCsv(CQCharts *charts);
 ~CQChartsCsv();

  CQCsvModel *csvModel() const { return csvModel_; }

  QSortFilterProxyModel *proxyModel() const { return proxyModel_; }

  void setCommentHeader(bool b);
  void setFirstLineHeader(bool b);

  bool load(const QString &filename);

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role) override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const;

 private:
  typedef std::map<int,QString> ColumnTypes;

  CQCsvModel*            csvModel_   { nullptr };
  QSortFilterProxyModel* proxyModel_ { nullptr };
};

#endif
