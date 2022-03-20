#ifndef CQJsonModel_H
#define CQJsonModel_H

#include <CQBaseModel.h>
#include <CJson.h>
#include <QStringList>

class CQJsonModel : public CQBaseModel {
  Q_OBJECT

  Q_PROPERTY(bool hierarchical READ isHierarchical WRITE setHierarchical)
  Q_PROPERTY(bool flat         READ isFlat         WRITE setFlat        )
  Q_PROPERTY(bool readOnly     READ isReadOnly     WRITE setReadOnly    )

 private:
  using ColumnValues = std::vector<QVariantList>;

  struct ColumnArrayData {
    int          numRows { -1 };
    QStringList  headerNames;
    ColumnValues columnValues;
  };

 public:
  CQJsonModel();

 ~CQJsonModel();

  //---

  bool load(const QString &filename);

  void save(std::ostream &os);
  void save(QAbstractItemModel *model, std::ostream &os);

  //---

  const QString &filename() const { return filename_; }
  void setFilename(const QString &s) { filename_ = s; }

  bool isHierarchical() const { return hier_; }
  void setHierarchical(bool b) { hier_ = b; };

  bool isFlat() const { return flat_; }
  void setFlat(bool b) { flat_ = b; }

  bool isReadOnly() const { return readOnly_; }
  void setReadOnly(bool b) { readOnly_ = b; }

  //---

  bool applyMatch(const QString &match);

  //---

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role=Qt::DisplayRole) override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  bool setData(const QModelIndex &index, const QVariant &value, int role=Qt::DisplayRole) override;

  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

 private:
  typedef std::map<int,QString> ColumnMap;

 private:
  bool isRootHierarchical(QString &hierName, QStringList &hierColumns) const;

  bool isArrayHierarchical(const std::string &name, CJson::Array *array, ColumnMap &columns) const;

  bool isObjHierarchical(const std::string &name, CJson::Object *obj, ColumnMap &columns) const;

  bool headerString(int section, QString &str) const;

  QString parentName(CJson::Value *value) const;

  bool isColumnArray(ColumnArrayData &data) const;

 protected:
  using Cells = std::vector<QString>;
  using Data  = std::vector<Cells>;

  QString         filename_;
  CJson*          json_        { nullptr };
  CJson::ValueP   jsonValue_;
  QString         jsonMatch_;
  CJson::Values   jsonValues_;
  bool            hier_        { false };
  bool            flat_        { false };
  bool            readOnly_    { false };
  bool            columnArray_ { false };
  ColumnArrayData columnArrayData_;
  QString         hierName_;
  QStringList     hierColumns_;
};

#endif
