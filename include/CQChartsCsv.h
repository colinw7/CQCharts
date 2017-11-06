#ifndef CQChartsCsv_H
#define CQChartsCsv_H

#include <QSortFilterProxyModel>

class CQCharts;
class CQCsvModel;
class CQExprModel;

class CQChartsCsv : public QSortFilterProxyModel {
  Q_OBJECT

 public:
  CQChartsCsv(CQCharts *charts);
 ~CQChartsCsv();

  CQCsvModel  *csvModel () const { return csvModel_ ; }
  CQExprModel *exprModel() const { return exprModel_; }

  void setCommentHeader(bool b);
  void setFirstLineHeader(bool b);

  bool load(const QString &filename);

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

  //---

  bool filterAcceptsRow(int row, const QModelIndex &parent) const override;

 private:
  CQCharts*    charts_    { nullptr };
  CQCsvModel*  csvModel_  { nullptr };
  CQExprModel* exprModel_ { nullptr };
};

#endif
