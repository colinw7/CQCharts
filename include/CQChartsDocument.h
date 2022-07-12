#ifndef CQChartsDocument_H
#define CQChartsDocument_H

#include <QTextBrowser>

/*!
 * \brief Document Class for Help
 * \ingroup Charts
 */
class CQChartsDocument : public QTextBrowser {
  Q_OBJECT

 public:
  CQChartsDocument(QWidget *parent=nullptr);

 Q_SIGNALS:
  void linkClicked(const QString &name);

 private Q_SLOTS:
  void anchorSlot(const QUrl &url);
};

#endif
