#ifndef CQChartsViewQuery_H
#define CQChartsViewQuery_H

#include <QFrame>
#include <QPointer>

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
  using View = CQChartsView;

 public:
  CQChartsViewQuery(View *view);

  QString text() const;
  void setText(const QString &text);

  QSize sizeHint() const override;

 private:
  using ViewP = QPointer<View>;

  ViewP      view_;
  QTextEdit* text_ { nullptr };
};

#endif
