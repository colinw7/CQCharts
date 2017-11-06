#ifndef CQChartsViewStatus_H
#define CQChartsViewStatus_H

#include <QFrame>

class CQChartsWindow;

class CQChartsViewStatus : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString text READ text WRITE setText)

 public:
  CQChartsViewStatus(CQChartsWindow *window);

  const QString &text() const { return text_; }
  void setText(const QString &s);

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private:
  CQChartsWindow *window_ { nullptr };
  QString         text_;
};

#endif
