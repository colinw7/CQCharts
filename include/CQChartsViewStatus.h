#ifndef CQChartsViewStatus_H
#define CQChartsViewStatus_H

#include <QFrame>
#include <QLabel>

class CQChartsWindow;
class CQChartsViewStatusStatus;
class CQChartsViewStatusPos;
class CQChartsViewStatusSel;

/*!
 * \brief View status widget
 * \ingroup Charts
 */
class CQChartsViewStatus : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString statusText READ statusText WRITE setStatusText)
  Q_PROPERTY(QString posText    READ posText    WRITE setPosText   )
  Q_PROPERTY(QString selText    READ selText    WRITE setSelText   )

 public:
  CQChartsViewStatus(CQChartsWindow *window);

  CQChartsWindow *window() const { return window_; }

  QString statusText() const;
  void setStatusText(const QString &s);

  QString posText() const;
  void setPosText(const QString &s);

  QString selText() const;
  void setSelText(const QString &s);

  QSize sizeHint() const override;

 private:
  CQChartsWindow*           window_      { nullptr };
  CQChartsViewStatusStatus* statusLabel_ { nullptr };
  CQChartsViewStatusPos*    posLabel_    { nullptr };
  CQChartsViewStatusSel*    selLabel_    { nullptr };
};

//---

/*!
 * \brief View status label widget
 * \ingroup Charts
 */
class CQChartsViewStatusStatus : public QLabel {
  Q_OBJECT

 public:
  CQChartsViewStatusStatus(CQChartsViewStatus *status);

  const QString &text() const { return text_; }
  void setText(const QString &text);

  void contextMenuEvent(QContextMenuEvent *e) override;

 private slots:
  void activateSlot(bool);

 private:
  CQChartsViewStatus* status_ { nullptr };
  QString             text_;
  bool                active_ { true };
};

//---

/*!
 * \brief View status position widget
 * \ingroup Charts
 */
class CQChartsViewStatusPos : public QLabel {
  Q_OBJECT

 public:
  CQChartsViewStatusPos(CQChartsViewStatus *status);

  const QString &text() const { return text_; }
  void setText(const QString &text);

  void contextMenuEvent(QContextMenuEvent *e) override;

 private slots:
  void activateSlot(bool);
  void posTextTypeAction(QAction *action);

 private:
  CQChartsViewStatus* status_ { nullptr };
  QString             text_;
  bool                active_ { true };
};

//---

/*!
 * \brief View status selection widget
 * \ingroup Charts
 */
class CQChartsViewStatusSel : public QLabel {
  Q_OBJECT

 public:
  CQChartsViewStatusSel(CQChartsViewStatus *status);

  const QString &text() const { return text_; }
  void setText(const QString &text);

  void contextMenuEvent(QContextMenuEvent *e) override;

 private slots:
  void activateSlot(bool);

 private:
  CQChartsViewStatus* status_ { nullptr };
  QString             text_;
  bool                active_ { true };
};

#endif
