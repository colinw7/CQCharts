#include <CQChartsViewQuery.h>
#include <CQChartsView.h>

#include <QTextEdit>
#include <QVBoxLayout>

CQChartsViewQuery::
CQChartsViewQuery(CQChartsView *view) :
 view_(view)
{
  setObjectName("viewQuery");

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  text_ = CQUtil::makeWidget<QTextEdit>("text");

  layout->addWidget(text_);
}

QString
CQChartsViewQuery::
text() const
{
  return text_->toPlainText();
}

void
CQChartsViewQuery::
setText(const QString &text)
{
  text_->setText(text);
}

QSize
CQChartsViewQuery::
sizeHint() const
{
  return QFrame::sizeHint();
}
