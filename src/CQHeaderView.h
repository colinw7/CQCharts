#ifndef CQHeaderView_H
#define CQHeaderView_H

#include <QHeaderView>

class CQHeaderViewWidgetFactory;
class QTableWidget;

class CQHeaderView : public QHeaderView {
  Q_OBJECT

 public:
  CQHeaderView(QWidget *parent=0);

  void setWidgetFactory(CQHeaderViewWidgetFactory *factory);

  QWidget *widget(int i) const;

  void initWidgets();

  void fixWidgetPositions();

 private:
  void showEvent(QShowEvent *e);

 private slots:
  void handleSectionResized(int i);

  void handleSectionMoved(int logical, int oldVisualIndex, int newVisualIndex);

 private:
  typedef QList<QWidget *> Widgets;

  CQHeaderViewWidgetFactory *factory_ { 0 };
  Widgets                    widgets_;
};

class CQHeaderViewWidgetFactory {
 public:
  CQHeaderViewWidgetFactory() { }

  virtual ~CQHeaderViewWidgetFactory() { }

  virtual QWidget *createWidget(int i) = 0;
};

#endif
