#ifndef CQWidgetMenu_H
#define CQWidgetMenu_H

#include <QWidget>
#include <QWidgetAction>
#include <QMenu>

/*!
  \ingroup CQWidgetMenu

  \brief Creates a menu containing a widget

  The CQWidgetMenu class provides a popup menu which can hold a fixed sized widget.

  To use create a CQWidgetMenu widget and a contents widget and use the setWidget()
  method to store the widget in the menu. The menu uses the contents widget's
  sizeHint to determine the menu size.
*/

class CQWidgetMenuAction;
class CQWidgetMenuArea;

/*!
  \ingroup CQWidgetMenu

  \brief Widget menu

  Widget Menu class.

  To use:
    . create a CQWidgetMenu widget
    . create a custom contents widget
    . Call setWidget to passing the contents widget
*/
class CQWidgetMenu : public QMenu {
  Q_OBJECT

 public:
  //! create menu
  CQWidgetMenu(QWidget *parent=nullptr);

  //! get menu action
  CQWidgetMenuAction *getAction() const;

  //! set contents widget
  void setWidget(QWidget *widget);

  //! create menu action
  virtual CQWidgetMenuAction *createAction();

  //! create area widget
  virtual CQWidgetMenuArea *createArea() const;

  //! reset menu size
  void resetSize();

 signals:
  //! signal emitted before menu is shown
  void menuShown();

  //! signal emitted after menu is hidden
  void menuHidden();

 protected:
  CQWidgetMenuAction *action_ { nullptr }; // menu action widget
};

/*!
  \ingroup CQWidgetMenu

  \brief Widget menu area

  The widget menu area contains the area widget (to hold content widget)

  Internal class - DO NOT USE
*/
class CQWidgetMenuArea : public QWidget {
  Q_OBJECT

 public:
  //! create menu area
  CQWidgetMenuArea(QWidget *parent=nullptr);

  //! delete menu area
  virtual ~CQWidgetMenuArea() { }

  //! set contents widget
  virtual void setWidget(QWidget *w);

  //! get area widget
  QWidget *getWidget() const { return areaWidget_; }

  //! get menu action
  CQWidgetMenuAction *getAction() const { return action_; }

  //! set menu action
  void setAction(CQWidgetMenuAction *action);

  //! set menu size
  virtual void setSize(int w, int h);

  //! reset menu size
  virtual void resetSize();

  //! return size hint
  virtual QSize sizeHint() const;

  //! update widget placement in menu
  virtual void updateLayout();

  //! handle menu shown
  virtual void menuShow();

  //! handle menu hidden
  virtual void menuHide();

 private:
  //! handle resize event
  void resizeEvent(QResizeEvent *);

 private slots:
  //! handle menu shown signal
  void menuShowSlot();

  //! handle menu hidden signal
  void menuHideSlot();

 signals:
  //! notify menu shown
  void menuShown();

  //! notify menu hidden
  void menuHidden();

 protected:
  CQWidgetMenuAction *action_     { nullptr }; // menu action widget
  int                 w_ { 0 }, h_ { 0 };      // current width/height
  QWidget            *area_       { nullptr }; // container widget for content widget
  QWidget            *areaWidget_ { nullptr }; // content widget
};

/*!
  \ingroup CQWidgetMenu

  \brief Menu action widget

  Widget action to interface between widget contents and menu API

  Internal class - DO NOT USE
*/
class CQWidgetMenuAction : public QWidgetAction {
 public:
  CQWidgetMenuAction(CQWidgetMenu *menu);

 ~CQWidgetMenuAction();

  //! get parent menu
  CQWidgetMenu *getMenu() const { return menu_; }

  //! get menu contents area
  CQWidgetMenuArea *getArea() const;

  //! get menu content widget (required implementation for QWidgetAction)
  QWidget *getWidget() const;

 private:
  //! create menu widget (required implementation for QWidgetAction)
  QWidget *createWidget(QWidget *parent);

 protected:
  CQWidgetMenu     *menu_ { nullptr }; //! parent menu
  CQWidgetMenuArea *area_ { nullptr }; //! menu contents area
};

#endif
