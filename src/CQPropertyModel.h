#ifndef CQPropertyModel_H
#define CQPropertyModel_H

#include <QAbstractItemModel>
#include <vector>

class CQPropertyItem;

class CQPropertyModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  CQPropertyModel();

  CQPropertyItem *root() const;

  int columnCount(const QModelIndex &) const override;

  int rowCount(const QModelIndex &parent) const override;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  QVariant data(const QModelIndex &index, int role) const override;

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

  QModelIndex index(int row, int column, const QModelIndex &parent) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  void clear();

  CQPropertyItem *addProperty(const QString &path, QObject *object, const QString &name,
                                  const QString &alias="");

  CQPropertyItem *item(const QModelIndex &index, bool &ok) const;

  CQPropertyItem *item(const QModelIndex &index) const;

 private:
  CQPropertyItem *hierItem(const QStringList &pathPaths, bool create=false);

  CQPropertyItem *hierItem(CQPropertyItem *parentRow, const QStringList &pathPaths,
                           bool create=false);

 signals:
  void valueChanged(QObject *, const QString &);

 private:
  CQPropertyItem *root_ { nullptr };
};

#endif
