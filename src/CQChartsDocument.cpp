#include <CQChartsDocument.h>

CQChartsDocument::
CQChartsDocument(QWidget *parent) :
 QTextBrowser(parent)
{
  setObjectName("document");

  setOpenLinks(false);
  setOpenExternalLinks(false);

  auto imagePath = QString("/home/colinw/dev/progs/charts/doc/CQCharts");

  document()->setMetaInformation(QTextDocument::DocumentUrl, "file:" + imagePath + "/");

  connect(this, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(anchorSlot(const QUrl &)));
}

void
CQChartsDocument::
anchorSlot(const QUrl &url)
{
  QString chartsPrefix("charts://");

  auto str = url.toString();

  if (str.startsWith(chartsPrefix)) {
    auto str1 = str.mid(chartsPrefix.length());

    emit linkClicked(str1);
  }
}
