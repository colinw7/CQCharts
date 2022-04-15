#ifndef CQChartsViewQuery_H
#define CQChartsViewQuery_H

#include <QFrame>

class CQChartsView;
class QTextEdit;

/*!
 * \brief Widget to show Query text
 * \ingroup Charts
 */
class CQChartsViewQuery : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QString text READ text WRITE setText)

 public:
  CQChartsViewQuery(CQChartsView *view);

  QString text() const;
  void setText(const QString &text);

  QSize sizeHint() const override;

 private:
  CQChartsView* view_ { nullptr };
  QTextEdit*    text_ { nullptr };
};

#endif
