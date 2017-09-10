#ifndef CQChartsModel_H
#define CQChartsModel_H

#include <CQChartsColumn.h>
#include <QAbstractItemModel>

class CQChartsColumnType;

class CQChartsModel : public QAbstractItemModel {
  Q_OBJECT

  Q_PROPERTY(bool columnHeaders READ hasColumnHeaders WRITE setColumnHeaders)

 public:
  CQChartsModel();

  virtual ~CQChartsModel() { }

  bool hasColumnHeaders() const { return columnHeaders_; }
  void setColumnHeaders(bool b) { columnHeaders_ = b; }

  void addColumn(const QString &name);

  virtual QString columnType(int col) const;
  virtual bool setColumnType(int col, const QString &type);

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  CQChartsColumnType *columnTypeData(int column, CQChartsNameValues &nameValues) const;

  bool isValidColumnType(const QString &type) const;

 protected:
  typedef std::vector<QString>        Cells;
  typedef std::vector<Cells>          Data;
  typedef std::vector<CQChartsColumn> Columns;

  bool    columnHeaders_ { false };
  Columns columns_;
  Data    data_;
};

#endif
