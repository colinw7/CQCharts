#ifndef CQDataModel_H
#define CQDataModel_H

#include <CQBaseModel.h>
#include <vector>

class CQDataModel : public CQBaseModel {
  Q_OBJECT

  Q_PROPERTY(bool    readOnly READ isReadOnly WRITE setReadOnly)
  Q_PROPERTY(QString filter   READ filter     WRITE setFilter  )

 public:
  CQDataModel();

  CQDataModel(int numCols, int numRows);

  virtual ~CQDataModel();

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

  //---

  const QString &filter() const { return filter_; }
  void setFilter(const QString &filter) { filter_ = filter; setFilterInited(false); }

  virtual bool hasFilter() const { return filter_.length(); }

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
  typedef std::vector<QVariant> Cells;
  typedef std::vector<Cells>    Data;

 protected:
  void init(int numCols, int numRows);

  //---

  virtual void initFilter();

  virtual bool isFilterInited() const { return filterInited_; }
  virtual void setFilterInited(bool b) { filterInited_ = b; }

  virtual bool acceptsRow(const Cells &cells) const;

 protected:
  struct FilterData {
    int     column { -1 };
    QRegExp regexp;
    bool    valid  { false };
  };

  typedef std::vector<FilterData> FilterDatas;

  bool readOnly_ { false };

  Cells       hheader_;                // horizontal header values
  Cells       vheader_;                // vertical header values
  Data        data_;                   // row values
  QString     filter_;                 // filter text
  bool        filterInited_ { false }; // filter initialized
  FilterDatas filterDatas_;            // filter datas
};

#endif