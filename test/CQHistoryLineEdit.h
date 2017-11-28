#ifndef CQHISTORY_LINE_EDIT_H
#define CQHISTORY_LINE_EDIT_H

#include <QLineEdit>
#include <CAutoPtr.h>

class CHistory;

class CQHistoryLineEdit : public QLineEdit {
  Q_OBJECT

 public:
  CQHistoryLineEdit(QWidget *parent=0);
 ~CQHistoryLineEdit();

 private slots:
  void execSlot();

 signals:
  void exec(const QString &cmd);

 private:
  void keyPressEvent(QKeyEvent *event);

 private:
  CAutoPtr<CHistory> history_;
};

#endif
