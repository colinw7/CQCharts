#ifndef CQPropertyViewModel_H
#define CQPropertyViewModel_H

#include <QAbstractItemModel>
#include <vector>

class CQPropertyViewItem;

class CQPropertyViewModel : public QAbstractItemModel {
  Q_OBJECT

 public:
  CQPropertyViewModel();

  CQPropertyViewItem *root() const;

  int columnCount(const QModelIndex &) const override;

  int rowCount(const QModelIndex &parent) const override;

  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

  QVariant data(const QModelIndex &index, int role) const override;

  bool setData(const QModelIndex &index, const QVariant &value, int role) override;

  QModelIndex index(int row, int column, const QModelIndex &parent) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  void clear();

  CQPropertyViewItem *addProperty(const QString &path, QObject *object, const QString &name,
                                  const QString &alias="");

  CQPropertyViewItem *item(const QModelIndex &index, bool &ok) const;

  CQPropertyViewItem *item(const QModelIndex &index) const;

  void refresh();

 private:
  CQPropertyViewItem *hierItem(const QStringList &pathPaths, bool create=false);

  CQPropertyViewItem *hierItem(CQPropertyViewItem *parentRow, const QStringList &pathPaths,
                           bool create=false);

 signals:
  void valueChanged(QObject *, const QString &);

 private:
  CQPropertyViewItem *root_ { nullptr };
};

#endif
