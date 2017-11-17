#ifndef CQChartsViewToolBar_H
#define CQChartsViewToolBar_H

#include <QFrame>

class CQChartsWindow;
class QToolButton;
class QStackedWidget;

class CQChartsViewToolBar : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewToolBar(CQChartsWindow *window);

  void updateMode();

  QSize sizeHint() const override;

 private slots:
  void selectSlot(bool b);
  void zoomSlot(bool b);
  void probeSlot(bool b);

  void autoFitSlot();

  void leftSlot();
  void rightSlot();

 private:
  CQChartsWindow* window_        { nullptr };
  QToolButton*    selectButton_  { nullptr };
  QToolButton*    zoomButton_    { nullptr };
  QToolButton*    probeButton_   { nullptr };
  QToolButton*    autoFitButton_ { nullptr };
  QToolButton*    leftButton_    { nullptr };
  QToolButton*    rightButton_   { nullptr };
  QStackedWidget* controlsStack_ { nullptr };
};

#endif
