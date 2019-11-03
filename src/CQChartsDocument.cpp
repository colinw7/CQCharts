#include <CQChartsDocument.h>

CQChartsDocument::
CQChartsDocument(QWidget *parent) :
 QTextBrowser(parent)
{
  setObjectName("document");

  setOpenLinks(false);
  setOpenExternalLinks(false);

  QString imagePath = "/home/colinw/dev/progs/charts/doc/CQCharts";

  document()->setMetaInformation(QTextDocument::DocumentUrl, "file:" + imagePath + "/");

  connect(this, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(anchorSlot(const QUrl &)));
}

void
CQChartsDocument::
anchorSlot(const QUrl &url)
{
  QString chartsPrefix("charts://");

  QString str = url.toString();

  if (str.startsWith(chartsPrefix)) {
    QString str1 = str.mid(chartsPrefix.length());

    emit linkClicked(str1);
  }
}
