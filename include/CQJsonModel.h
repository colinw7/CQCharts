#ifndef CQJsonModel_H
#define CQJsonModel_H

#include <QAbstractItemModel>
#include <CJson.h>

class CQJsonModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  CQJsonModel();

  bool load(const QString &filename);

  bool isHierarchical() const { return hier_; }

  bool applyMatch(const QString &match);

  int columnCount(const QModelIndex &parent=QModelIndex()) const override;

  int rowCount(const QModelIndex &parent=QModelIndex()) const override;

  QVariant headerData(int section, Qt::Orientation orientation=Qt::Horizontal,
                      int role=Qt::DisplayRole) const override;

  QVariant data(const QModelIndex &index, int role=Qt::DisplayRole) const override;

  QModelIndex index(int row, int column, const QModelIndex &parent=QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &index) const override;

 private:
  typedef std::map<int,QString> ColumnMap;

 private:
  bool isRootHierarchical(QString &hierName, QStringList &hierColumns) const;
  bool isArrayHierarchical(const std::string &name, CJson::Array *array, ColumnMap &columns) const;
  bool isObjHierarchical(const std::string &name, CJson::Object *obj, ColumnMap &columns) const;

 protected:
  typedef std::vector<QString>  Cells;
  typedef std::vector<Cells>    Data;

  QString              filename_;
  CJson*               json_      { nullptr };
  CJson::Value*        jsonValue_ { nullptr };
  QString              jsonMatch_;
  CJson::Array::Values jsonValues_;
  bool                 hier_      { false };
  QString              hierName_;
  QStringList          hierColumns_;
};

#endif
