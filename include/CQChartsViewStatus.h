#ifndef CQChartsViewStatus_H
#define CQChartsViewStatus_H

#include <QFrame>

class CQChartsWindow;
class QLabel;

class CQChartsViewStatus : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString statusText READ statusText WRITE setStatusText)
  Q_PROPERTY(QString posText    READ posText    WRITE setPosText   )
  Q_PROPERTY(QString selText    READ selText    WRITE setSelText   )

 public:
  CQChartsViewStatus(CQChartsWindow *window);

  QString statusText() const;
  void setStatusText(const QString &s);

  QString posText() const;
  void setPosText(const QString &s);

  QString selText() const;
  void setSelText(const QString &s);

  QSize sizeHint() const override;

 private:
  CQChartsWindow* window_      { nullptr };
  QLabel*         statusLabel_ { nullptr };
  QLabel*         posLabel_    { nullptr };
  QLabel*         selLabel_    { nullptr };
};

#endif
