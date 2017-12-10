#ifndef CQChartsViewToolBar_H
#define CQChartsViewToolBar_H

#include <QFrame>

class CQChartsWindow;
class CQIconCombo;
class QToolButton;
class QStackedWidget;

class CQChartsViewToolBar : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewToolBar(CQChartsWindow *window);

  void updateMode();

  QSize sizeHint() const override;

 private slots:
  void modeSlot(int ind);

  void selectButtonClicked(int);
  void zoomSlot();

  void autoFitSlot();

  void leftSlot();
  void rightSlot();

 private:
  CQChartsWindow* window_        { nullptr };
  CQIconCombo*    modeCombo_     { nullptr };
  QToolButton*    autoFitButton_ { nullptr };
  QToolButton*    leftButton_    { nullptr };
  QToolButton*    rightButton_   { nullptr };
  QStackedWidget* controlsStack_ { nullptr };
};

#endif
