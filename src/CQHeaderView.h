#ifndef CQHeaderView_H
#define CQHeaderView_H

#include <QHeaderView>

class CQHeaderViewWidgetFactory;
class QTableWidget;

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
