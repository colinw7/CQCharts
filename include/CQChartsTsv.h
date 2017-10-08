#ifndef CQChartsTsv_H
#define CQChartsTsv_H

#include <CQChartsModelColumn.h>
#include <QSortFilterProxyModel>

class CQCharts;
class CQTsvModel;

class CQChartsTsv : public QSortFilterProxyModel, public CQChartsModelColumn {
  Q_OBJECT

 public:
  CQChartsTsv(CQCharts *charts);
 ~CQChartsTsv();

  CQTsvModel *tsvModel() const { return tsvModel_; }

  void setCommentHeader(bool b);
  void setFirstLineHeader(bool b);

  bool load(const QString &filename);

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                     int role=Qt::DisplayRole) override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const;

 private:
  CQCharts*   charts_   { nullptr };
  CQTsvModel* tsvModel_ { nullptr };
};

#endif
