#ifndef CQChartsWidgetUtil_H
#define CQChartsWidgetUtil_H

#include <QFrame>

class QGridLayout;

namespace CQChartsWidgetUtil {

void addGridLabelWidget(QGridLayout *playout, const QString &label, QWidget *widget, int &row);

};

//------

class QPushButton;

//! \brief dialog OK, Apply, Cancel buttons
class CQChartsDialogButtons : public QFrame {
  Q_OBJECT

 public:
  CQChartsDialogButtons(QWidget *parent=nullptr);

  void connect(QWidget *w, const char *okSlot, const char *applySlot, const char *cancelSlot);

 protected:
  QPushButton* okButton_     { nullptr };
  QPushButton* applyButton_  { nullptr };
  QPushButton* cancelButton_ { nullptr };
};

#endif
