#ifndef CQChartsViewToolBar_H
#define CQChartsViewToolBar_H

#include <QFrame>

class CQChartsView;
class QToolButton;
class QStackedWidget;

class CQChartsViewToolBar : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewToolBar(CQChartsView *view);

  void updateMode();

  QSize sizeHint() const;

 private slots:
  void selectSlot(bool b);
  void zoomSlot(bool b);
  void probeSlot(bool b);

  void autoFitSlot();

 private:
  CQChartsView*   view_          { nullptr };
  QToolButton*    selectButton_  { nullptr };
  QToolButton*    zoomButton_    { nullptr };
  QToolButton*    probeButton_   { nullptr };
  QToolButton*    autoFitButton_ { nullptr };
  QStackedWidget* controlsStack_ { nullptr };
};

#endif
