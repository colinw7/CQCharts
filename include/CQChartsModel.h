#ifndef CQChartsModel_H
#define CQChartsModel_H

#include <CQChartsColumn.h>
#include <QAbstractItemModel>

class CQChartsModel : public QAbstractItemModel {
  Q_OBJECT

  Q_PROPERTY(bool columnHeaders READ hasColumnHeaders WRITE setColumnHeaders)

 public:
  CQChartsModel();

  bool hasColumnHeaders() const { return columnHeaders_; }
  void setColumnHeaders(bool b) { columnHeaders_ = b; }

  void addColumn(const QString &name);

  QString columnType(int col) const;
  void setColumnType(int col, const QString &type);

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

 protected:
  typedef std::vector<QString>        Cells;
  typedef std::vector<Cells>          Data;
  typedef std::vector<CQChartsColumn> Columns;

  bool    columnHeaders_ { false };
  int     numColumns_;
  Columns columns_;
  Data    data_;
};

#endif
