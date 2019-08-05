#ifndef CQChartsWidgetUtil_H
#define CQChartsWidgetUtil_H

#include <QFrame>

class QGridLayout;

namespace CQChartsWidgetUtil {

void addGridLabelWidget(QGridLayout *playout, const QString &label, QWidget *widget, int &row);

};

//------

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

  void connect(QWidget *w, const char *okSlot, const char *applySlot, const char *cancelSlot);

  void setToolTips(const QString &okTip, const QString &applyTip, const QString &cancelTip);

 protected:
  QPushButton* okButton_     { nullptr };
  QPushButton* applyButton_  { nullptr };
  QPushButton* cancelButton_ { nullptr };
};

#endif
