#ifndef CQChartsViewStatus_H
#define CQChartsViewStatus_H

#include <QFrame>

class CQChartsView;

class CQChartsViewStatus : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString text READ text WRITE setText)

 public:
  CQChartsViewStatus(CQChartsView *view);

  const QString &text() const { return text_; }
  void setText(const QString &s);

  void paintEvent(QPaintEvent *);

  QSize sizeHint() const;

 private:
  CQChartsView *view_ { nullptr };
  QString       text_;
};

#endif
