#ifndef CQTabBarWidget_H
#define CQTabBarWidget_H

#include <QWidget>
#include <QIcon>

class CQTabBar;
class QStackedWidget;
class QMenu;

class CQTabBarWidget : public QWidget {
  Q_OBJECT

  Q_PROPERTY(Qt::Orientation     orientation  READ orientation  WRITE setOrientation)
  Q_PROPERTY(Qt::ToolButtonStyle buttonStyle  READ buttonStyle  WRITE setButtonStyle)
  Q_PROPERTY(int                 currentIndex READ currentIndex WRITE setCurrentIndex)
  Q_PROPERTY(bool                flowTabs     READ isFlowTabs   WRITE setFlowTabs)

 public:
  CQTabBarWidget(QWidget *parent = NULL);

  //! get/set tab orientation
  Qt::Orientation orientation() const { return orientation_; }
  void setOrientation(Qt::Orientation orient);

  //! get/set button style (icon and/or text)
  Qt::ToolButtonStyle buttonStyle() const;
  void setButtonStyle(const Qt::ToolButtonStyle &buttonStyle);

  //! get/set flow tabs
  bool isFlowTabs() const;
  void setFlowTabs(bool b);

  //---

  void clear();

  void addTab(QWidget *page, const QIcon &icon, const QString &text);
  void addTab(QWidget *page, const QString &text);

  int count() const;

  QString tabText(int ind) const;
  QIcon tabIcon(int ind) const;

  //---

  //! get/set current index
  int currentIndex() const;
  void setCurrentIndex(int tab);

  //---

  void contextMenuEvent(QContextMenuEvent *e) override;

  void resizeEvent(QResizeEvent *) override;

  //---

  QSize sizeHint() const override;

 private Q_SLOTS:
  void tabSlot(int);
  void moveTabSlot(int, int);

  void showTabMenu(const QPoint &gpos);

  void tabActionSlot();

 private:
  QMenu *createTabMenu() const;

 private:
  Qt::Orientation orientation_ { Qt::Horizontal };
  CQTabBar*       tabBar_      { nullptr };
  QStackedWidget* stack_       { nullptr };
};

#endif
