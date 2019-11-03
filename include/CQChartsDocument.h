#ifndef CQChartsDocument_H
#define CQChartsDocument_H

#include <QTextBrowser>

class CQChartsDocument : public QTextBrowser {
  Q_OBJECT

 public:
  CQChartsDocument(QWidget *parent=nullptr);

 signals:
  void linkClicked(const QString &name);

 private slots:
  void anchorSlot(const QUrl &url);
};

#endif
