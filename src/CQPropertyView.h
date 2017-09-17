#ifndef CQPropertyView_H
#define CQPropertyView_H

#include <QTreeView>

class CQPropertyModel;
class CQPropertyItem;
class CQPropertyViewFilter;

class CQPropertyView : public QTreeView {
  Q_OBJECT

  Q_PROPERTY(bool mouseHighlight READ isMouseHighlight WRITE setMouseHighlight)

 public:
  typedef std::vector<CQPropertyItem *> Items;

 public:
  CQPropertyView(QWidget *parent=nullptr);

  CQPropertyModel *propertyModel() const { return model_; }

  CQPropertyViewFilter *filterModel() const { return filter_; }

  void setFilter(const QString &filter);

  void clear();

  void addProperty(const QString &path, QObject *obj,
                   const QString &name, const QString &alias="");

  void selectObject(const QObject *obj);

  void getSelectedObjects(std::vector<QObject *> &objs);

  bool isMouseHighlight() const { return mouseHighlight_; }
  void setMouseHighlight(bool b);

  void setMouseInd(const QModelIndex &i) { hasMouseInd_ = true; mouseInd_ = i; }
  void unsetMouseInd() { hasMouseInd_ = false; }
  bool isMouseInd(const QModelIndex &i);

  CQPropertyItem *getModelItem(const QModelIndex &index, bool map=true) const;

 signals:
  void valueChanged(QObject *, const QString &);

  void itemClicked( QObject *obj, const QString &path);
  void itemSelected(QObject *obj, const QString &path);

  void menuExec(QObject *obj, const QPoint &gpos);

 public slots:
  void expandSelected();

  void search(const QString &text);

 private slots:
  void modelResetSlot();

  void itemClickedSlot(const QModelIndex &index);

  void itemSelectionSlot();

  void customContextMenuSlot(const QPoint &pos);

 private:
  bool selectObject(CQPropertyItem *item, const QObject *obj);

  void getItemData(CQPropertyItem *item, QObject* &obj, QString &path);

  void searchItemTree(CQPropertyItem *item, const QRegExp &regexp, Items &items);

  void expandItemTree(CQPropertyItem *item);

  void showContextMenu(QObject *obj, const QPoint &globalPos);

  void mouseMoveEvent(QMouseEvent *);

  void leaveEvent(QEvent *);

  void scrollToItem(CQPropertyItem *item);

  void expandItem(CQPropertyItem *item);

  void selectItem(CQPropertyItem *item, bool selected);

  QModelIndex indexFromItem(CQPropertyItem *item, int column, bool map=false) const;

 private:
  CQPropertyModel*      model_          { nullptr };
  CQPropertyViewFilter* filter_         { nullptr };
  bool                  mouseHighlight_ { false };
  bool                  hasMouseInd_    { false };
  QModelIndex           mouseInd_;
};

#endif
