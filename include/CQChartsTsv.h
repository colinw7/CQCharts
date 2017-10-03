#ifndef CQChartsTsv_H
#define CQChartsTsv_H

#include <CQChartsModel.h>

class CQTsvModel;
class QSortFilterProxyModel;

class CQChartsTsv : public CQChartsModel {
  Q_OBJECT

 public:
  CQChartsTsv(CQCharts *charts);
 ~CQChartsTsv();

  CQTsvModel *tsvModel() const { return tsvModel_; }

  QSortFilterProxyModel *proxyModel() const { return proxyModel_; }

  void setCommentHeader(bool b);
  void setFirstLineHeader(bool b);

  bool load(const QString &filename);

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const;

 private:
  typedef std::map<int,QString> ColumnTypes;

  CQTsvModel*            tsvModel_   { nullptr };
  QSortFilterProxyModel* proxyModel_ { nullptr };
};

#endif
