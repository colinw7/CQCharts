#ifndef CQChartsWidgetUtil_H
#define CQChartsWidgetUtil_H

#include <QWidget>

class QFrame;
class QGridLayout;

namespace CQChartsWidgetUtil {

void addGridLabelWidget(QGridLayout *playout, const QString &label, QWidget *widget, int &row);

QFrame *createHSpacer(int width=1, const char *name="spacer");

QFrame *createHStretch(const char *name="spacer");

void setTextColor(QWidget *w, const QColor &c);

//---

#if 0
inline void optConnect(QObject *from, const char *fromName, QObject *to, const char *toName) {
  if (! from || ! to) return;

  QObject::connect(from, fromName, to, toName);
}

inline void optDisconnect(QObject *from, const char *fromName, QObject *to, const char *toName) {
  if (! from || ! to) return;

  QObject::disconnect(from, fromName, to, toName);
}

inline void connectDisconnect(bool b, QObject *from, const char *fromName,
                              QObject *to, const char *toName) {
  if (b)
    QObject::connect(from, fromName, to, toName);
  else
    QObject::disconnect(from, fromName, to, toName);
}

inline void optConnectDisconnect(bool b, QObject *from, const char *fromName,
                                 QObject *to, const char *toName) {
  if (! from || ! to) return;

  connectDisconnect(b, from, fromName, to, toName);
}
#endif

//---

#if 0
/*!
 * \brief RAII Class to Auto Connect/Disconnect to signal/slot
 * \ingroup Charts
 */
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
#endif

}

#endif
