#ifndef CQPropertyViewTree_H
#define CQPropertyViewTree_H

#include <QTreeView>

class CQPropertyViewModel;
class CQPropertyViewItem;
class CQPropertyViewFilter;

class CQPropertyViewTree : public QTreeView {
  Q_OBJECT

  Q_PROPERTY(bool itemMenu       READ isItemMenu       WRITE setItemMenu      )
  Q_PROPERTY(bool mouseHighlight READ isMouseHighlight WRITE setMouseHighlight)

 public:
  typedef std::vector<CQPropertyViewItem *> Items;

 public:
  CQPropertyViewTree(QWidget *parent=nullptr);
 ~CQPropertyViewTree();

  CQPropertyViewModel *propertyModel() const { return model_; }

  CQPropertyViewFilter *filterModel() const { return filter_; }

  void setFilter(const QString &filter);

  void redraw();

  void clear();

  void addProperty(const QString &path, QObject *obj,
                   const QString &name, const QString &alias="");

  bool setProperty(QObject *object, const QString &path, const QVariant &value);
  bool getProperty(QObject *object, const QString &path, QVariant &value);

  void selectObject(const QObject *obj);

  void getSelectedObjects(std::vector<QObject *> &objs);

  bool isItemMenu() const { return itemMenu_; }
  void setItemMenu(bool b) { itemMenu_ = b; }

  bool isMouseHighlight() const { return mouseHighlight_; }
  void setMouseHighlight(bool b);

  void setMouseInd(const QModelIndex &i);
  void unsetMouseInd();
  bool isMouseInd(const QModelIndex &i);

  CQPropertyViewItem *getModelItem(const QModelIndex &index, bool map=true) const;

  void expandAll(CQPropertyViewItem *item);
  void collapseAll(CQPropertyViewItem *item);

 signals:
  void valueChanged(QObject *, const QString &);

  void itemClicked( QObject *obj, const QString &path);
  void itemSelected(QObject *obj, const QString &path);

  void menuExec(QObject *obj, const QPoint &gpos);

 public slots:
  void expandAll();
  void collapseAll();

  void expandSelected();

  void search(const QString &text);

 private slots:
  void modelResetSlot();

  void itemClickedSlot(const QModelIndex &index);

  void itemSelectionSlot();

  void customContextMenuSlot(const QPoint &pos);

 private:
  CQPropertyViewItem *objectItem(const QObject *obj) const;

  CQPropertyViewItem *objectItem(CQPropertyViewItem *parent, const QObject *obj) const;

  bool selectObject(CQPropertyViewItem *item, const QObject *obj);

  void getItemData(CQPropertyViewItem *item, QObject* &obj, QString &path);

  void searchItemTree(CQPropertyViewItem *item, const QRegExp &regexp, Items &items);

  void expandItemTree(CQPropertyViewItem *item);
  void collapseItemTree(CQPropertyViewItem *item);

  void showContextMenu(QObject *obj, const QPoint &globalPos);

  void mouseMoveEvent(QMouseEvent *);

  void leaveEvent(QEvent *);

  void scrollToItem(CQPropertyViewItem *item);

  void expandItem(CQPropertyViewItem *item);
  void collapseItem(CQPropertyViewItem *item);

  void selectItem(CQPropertyViewItem *item, bool selected);

  QModelIndex indexFromItem(CQPropertyViewItem *item, int column, bool map=false) const;

 private:
  CQPropertyViewModel*  model_          { nullptr };
  CQPropertyViewFilter* filter_         { nullptr };
  bool                  itemMenu_       { false };
  bool                  mouseHighlight_ { false };
  bool                  hasMouseInd_    { false };
  QModelIndex           mouseInd_;
};

#endif
