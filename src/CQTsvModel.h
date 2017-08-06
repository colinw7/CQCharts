#ifndef CQTsvModel_H
#define CQTsvModel_H

#include <QAbstractItemModel>

class CQTsvModel : public QAbstractItemModel {
  Q_OBJECT

  Q_PROPERTY(bool columnHeaders READ hasColumnHeaders WRITE setColumnHeaders)

 public:
  CQTsvModel();

  bool hasColumnHeaders() const { return columnHeaders_; }
  void setColumnHeaders(bool b) { columnHeaders_ = b; }

  bool load(const QString &filename);

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

 protected:
  typedef std::vector<QString> Cells;
  typedef std::vector<Cells>   Data;

  QString filename_;
  bool    columnHeaders_ { false };
  int     numColumns_ { 0 };
  Cells   header_;
  Data    data_;
};

#endif
