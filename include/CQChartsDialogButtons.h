#ifndef CQChartsDialogButtons_H
#define CQChartsDialogButtons_H

#include <QFrame>

class QPushButton;

/*!
 * \brief dialog OK, Apply, Cancel buttons
 * \ingroup Charts
 */
class CQChartsDialogButtons : public QFrame {
  Q_OBJECT

 public:
  CQChartsDialogButtons(QWidget *parent=nullptr);

  QPushButton* okButton    () const { return okButton_    ; }
  QPushButton* applyButton () const { return applyButton_ ; }
  QPushButton* cancelButton() const { return cancelButton_; }
  QPushButton* helpButton  () const { return helpButton_  ; }

  void connect(QWidget *w, const char *okSlot, const char *applySlot, const char *cancelSlot,
               const char *helpSlot=nullptr);

  void setToolTips(const QString &okTip, const QString &applyTip, const QString &cancelTip);

 private:
  QPushButton* okButton_     { nullptr };
  QPushButton* applyButton_  { nullptr };
  QPushButton* cancelButton_ { nullptr };
  QPushButton* helpButton_   { nullptr };
};

#endif
