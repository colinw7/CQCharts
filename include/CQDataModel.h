#ifndef CQDataModel_H
#define CQDataModel_H

#include <CQBaseModel.h>
#include <vector>

class CQDataModel : public CQBaseModel {
  Q_OBJECT

  Q_PROPERTY(bool readOnly READ isReadOnly WRITE setReadOnly)

 public:
  CQDataModel();

  CQDataModel(int numCols, int numRows);

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

  //--

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role=Qt::DisplayRole) override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  bool setData(const QModelIndex &index, const QVariant &value,
               int role=Qt::DisplayRole) override;

  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const;

 protected:
  void init(int numCols, int numRows);

 protected:
  typedef std::vector<QVariant> Cells;
  typedef std::vector<Cells>    Data;

  bool readOnly_ { false };

  Cells header_;
  Data  data_;
};

#endif
