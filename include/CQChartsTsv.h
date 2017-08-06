#ifndef CQChartsTsv_H
#define CQChartsTsv_H

#include <CQChartsModel.h>

class CQTsvModel;

class CQChartsTsv : public CQChartsModel {
  Q_OBJECT

 public:
  CQChartsTsv();

  bool load(const QString &filename);

  QString columnType(int col) const override;
  void setColumnType(int col, const QString &type) override;

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

 private:
  typedef std::map<int,QString> ColumnTypes;

  CQTsvModel* model_ { nullptr };
  ColumnTypes columnTypes_;
};

#endif
