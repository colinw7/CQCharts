#ifndef CQChartsJson_H
#define CQChartsJson_H

#include <CQChartsColumn.h>
#include <QSortFilterProxyModel>

class CQCharts;
class CQChartsColumnType;
class CQJsonModel;

class CQChartsJson : public QSortFilterProxyModel {
  Q_OBJECT

 public:
  CQChartsJson(CQCharts *charts);
 ~CQChartsJson();

  CQJsonModel *jsonModel() const { return jsonModel_; }

  bool load(const QString &filename);

  bool isHierarchical() const;

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value,
                     int role=Qt::DisplayRole) override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const;

  //---

  QString columnType(int col) const;
  bool setColumnType(int col, const QString &type);

 private:
  using Columns = std::vector<CQChartsColumn>;

  CQCharts*    charts_   { nullptr };
  CQJsonModel* jsonModel_ { nullptr };
  Columns      columns_;
};

#endif
