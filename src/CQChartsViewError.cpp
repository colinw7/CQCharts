#include <CQChartsViewError.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>

#include <CQTabSplit.h>
#include <CQUtil.h>

#include <QTextBrowser>
#include <QAbstractTextDocumentLayout>
#include <QVBoxLayout>

CQChartsViewError::
CQChartsViewError(CQChartsView *view) :
 view_(view)
{
  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 2, 2);

  tab_ = CQUtil::makeWidget<CQTabSplit>("area");

  tab_->setState(CQTabSplit::State::TAB);

  layout->addWidget(tab_);
}

void
CQChartsViewError::
updatePlots()
{
  tab_->removeAllWidgets();

  texts_.clear();

  for (auto &plot : view_->plots()) {
    if (! plot->hasErrors())
      continue;

    auto *text = CQUtil::makeWidget<QTextBrowser>("text");

    tab_->addWidget(text, plot->id());

    plot->addErrorsToWidget(text);

    texts_[plot] = text;
  }

  resize(sizeHint());
}

QSize
CQChartsViewError::
sizeHint() const
{
  QFontMetrics fm(font());

  int tw = 80*fm.width("X");

  int w = 0, h = 0;

  for (const auto &p : texts_) {
    QTextBrowser *text = p.second;

    auto doc = text->document();

    if (text->width() < tw) {
      text->resize(tw, text->height());

      doc->markContentsDirty(0, doc->characterCount());
    }

    QAbstractTextDocumentLayout *layout = doc->documentLayout();

    auto s = layout->documentSize();

    w = std::max(w, int(s.width ()));
    h = std::max(h, int(s.height()));
  }

  w = std::max(w, 100);
  h = std::max(h, 100);

  return QSize(w + 8, h + 40);
}
