#ifndef CQChartsWidgetUtil_H
#define CQChartsWidgetUtil_H

#include <QFrame>
#include <QWidgetAction>

class QGridLayout;

namespace CQChartsWidgetUtil {

void addGridLabelWidget(QGridLayout *playout, const QString &label, QWidget *widget, int &row);

QFrame *createHSpacer(int width=1, const char *name="spacer");

bool isFixedPitch(const QFont &font);
QFont getMonospaceFont();

void setTextColor(QWidget *w, const QColor &c);

inline void connectDisconnect(bool b, QObject *from, const char *fromName,
                              QObject *to, const char *toName) {
  if (b)
    QObject::connect(from, fromName, to, toName);
  else
    QObject::disconnect(from, fromName, to, toName);
}

class AutoDisconnect {
 public:
  AutoDisconnect(QObject *from, const char *fromName, QObject *to, const char *toName) :
   from_(from), fromName_(fromName), to_(to), toName_(toName) {
    connectDisconnect(false, from_, fromName_, to_, toName_);
  }

 ~AutoDisconnect() {
    connectDisconnect(true, from_, fromName_, to_, toName_);
  }

 private:
  QObject    *from_     { nullptr };
  const char *fromName_ { nullptr };
  QObject    *to_       { nullptr };
  const char *toName_   { nullptr };
};

}

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

//---

class CQChartsWidgetAction : public QWidgetAction {
 public:
  CQChartsWidgetAction(QWidget *w) :
   QWidgetAction(nullptr), w_(w) {
  }

 ~CQChartsWidgetAction() {
    delete w_;
  }

  QWidget *widget() const { return w_; }

  QWidget *createWidget(QWidget *parent) {
    w_->setParent(parent);

    return w_;
  }

 private:
  QWidget *w_ { nullptr };
};

#endif
