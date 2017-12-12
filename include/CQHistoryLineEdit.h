#ifndef CQHISTORY_LINE_EDIT_H
#define CQHISTORY_LINE_EDIT_H

#include <QLineEdit>
#include <CAutoPtr.h>

class CHistory;

class CQHistoryLineEdit : public QLineEdit {
  Q_OBJECT

  Q_PROPERTY(bool autoClear READ isAutoClear WRITE setAutoClear)

 public:
  CQHistoryLineEdit(QWidget *parent=0);
 ~CQHistoryLineEdit();

  bool isAutoClear() const { return autoClear_; }
  void setAutoClear(bool b) { autoClear_ = b; }

 private slots:
  void execSlot();

 signals:
  void exec(const QString &cmd);

 private:
  void keyPressEvent(QKeyEvent *event);

 private:
  CAutoPtr<CHistory> history_;
  bool               autoClear_ { true };
};

#endif
