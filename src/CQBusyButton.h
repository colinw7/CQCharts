#ifndef CQBusyButton_H
#define CQBusyButton_H

#include <QToolButton>

class QTimer;

class CQBusyButton : public QToolButton {
  Q_OBJECT

  Q_PROPERTY(QString label READ label  WRITE setLabel)
  Q_PROPERTY(bool    busy  READ isBusy WRITE setBusy )

 public:
  CQBusyButton(QWidget *parent=nullptr);
 ~CQBusyButton();

  const QString &label() const { return label_; }
  void setLabel(const QString &s) { label_ = s; }

  bool isBusy() const { return busy_; }
  void setBusy(bool b);

  void paintEvent(QPaintEvent *) override;

  QSize sizeHint() const override;

 private Q_SLOTS:
  void timerSlot();
  void clickSlot();

 Q_SIGNALS:
  void busyStateChanged(bool);

 private:
  void drawBusy(QPainter *painter) const;

 private:
  QString label_;           //!< label
  bool    busy_  { false }; //!< is busy
  uint    ticks_ { 0 };     //!< ticks
  QTimer* timer_ { nullptr };
};

#endif
