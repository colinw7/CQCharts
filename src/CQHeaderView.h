#ifndef CQHeaderView_H
#define CQHeaderView_H

#include <QHeaderView>

class CQHeaderViewWidgetFactory;
class QTableView;
class QTreeView;

class CQHeaderView : public QHeaderView {
  Q_OBJECT

 public:
  CQHeaderView(QWidget *parent=nullptr);

  void setWidgetFactory(CQHeaderViewWidgetFactory *factory);

  void setSectionsMovable(bool movable);
  void setSectionsClickable(bool clickable);

  QWidget *widget(int i) const;

  void initWidgets();

  void fixWidgetPositions();

 private:
  void contextMenuEvent(QContextMenuEvent *event);

  void showEvent(QShowEvent *e);

 private slots:
  void handleSectionResized(int i);

  void handleSectionMoved(int logical, int oldVisualIndex, int newVisualIndex);

  void fitColumnSlot();
  void fitAllSlot();

  void stretchLastSlot(bool b);
  void sortIndicatorSlot(bool b);

 private:
  typedef std::map<int,int> ColumnWidths;

  void calcTableWidths(QTableView *table, ColumnWidths &columnWidths);

  void calcTreeWidths(QTreeView *tree, const QModelIndex &ind,
                      int depth, ColumnWidths &columnWidths);

 private:
  typedef QList<QWidget *> Widgets;

  CQHeaderViewWidgetFactory *factory_ { nullptr };
  Widgets                    widgets_;
  int                        menuSection_ { -1 };
};

//---

class CQHeaderViewWidgetFactory {
 public:
  CQHeaderViewWidgetFactory() { }

  virtual ~CQHeaderViewWidgetFactory() { }

  virtual QWidget *createWidget(int i) = 0;
};

#endif
