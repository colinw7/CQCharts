#ifndef CQFontListView_H
#define CQFontListView_H

#include <QListView>
#include <QStringListModel>

class CQFontListView : public QListView {
  Q_OBJECT

 public:
  CQFontListView(QWidget *parent=0);

  QStringListModel *model() const {
    return static_cast<QStringListModel *>(QListView::model());
  }

  void setCurrentItem(int item) {
     QListView::setCurrentIndex(static_cast<QAbstractListModel*>(model())->index(item));
  }

  int currentItem() const {
    return QListView::currentIndex().row();
  }

  int count() const {
    return model()->rowCount();
  }

  QString currentText() const {
    int row = QListView::currentIndex().row();
    return row < 0 ? QString() : model()->stringList().at(row);
  }

  void currentChanged(const QModelIndex &current, const QModelIndex &previous) Q_DECL_OVERRIDE {
    QListView::currentChanged(current, previous);
    if (current.isValid())
      emit highlighted(current.row());
  }

  QString text(int i) const {
    return model()->stringList().at(i);
  }

signals:
  void highlighted(int);
};

#endif
