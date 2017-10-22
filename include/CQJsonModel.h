#ifndef CQJsonModel_H
#define CQJsonModel_H

#include <CQBaseModel.h>
#include <CJson.h>
#include <QStringList>

class CQJsonModel : public CQBaseModel {
  Q_OBJECT

  Q_PROPERTY(bool hierarchical READ isHierarchical WRITE setHierarchical)
  Q_PROPERTY(bool flag         READ isFlat         WRITE setFlat        )

 public:
  CQJsonModel();

  bool load(const QString &filename);

  bool isHierarchical() const { return hier_; }
  void setHierarchical(bool b) { hier_ = b; };

  bool isFlat() const { return flat_; }
  void setFlat(bool b) { flat_ = b; }

  bool applyMatch(const QString &match);

  //---

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  bool setHeaderData(int section, Qt::Orientation orientation,
                     const QVariant &value, int role=Qt::DisplayRole) override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const;

 private:
  typedef std::map<int,QString> ColumnMap;

 private:
  bool isRootHierarchical(QString &hierName, QStringList &hierColumns) const;

  bool isArrayHierarchical(const std::string &name, CJson::Array *array, ColumnMap &columns) const;

  bool isObjHierarchical(const std::string &name, CJson::Object *obj, ColumnMap &columns) const;

  bool headerString(int section, QString &str) const;

  QString parentName(CJson::Value *value) const;

 protected:
  typedef std::vector<QString> Cells;
  typedef std::vector<Cells>   Data;

  QString              filename_;
  CJson*               json_      { nullptr };
  CJson::Value*        jsonValue_ { nullptr };
  QString              jsonMatch_;
  CJson::Array::Values jsonValues_;
  bool                 hier_      { false };
  bool                 flat_      { false };
  QString              hierName_;
  QStringList          hierColumns_;
};

#endif
