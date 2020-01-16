#ifndef CQChartsViewError_H
#define CQChartsViewError_H

#include <CQWinWidget.h>
#include <map>

class CQChartsView;
class CQChartsPlot;
class CQTabSplit;
class QTextBrowser;

class CQChartsViewError : public CQWinWidget {
  Q_OBJECT

 public:
  CQChartsViewError(CQChartsView *view);

  void updatePlots();

  QSize sizeHint() const;

 private:
  using Texts = std::map<CQChartsPlot *, QTextBrowser *>;

  CQChartsView* view_ { nullptr };
  CQTabSplit*   tab_  { nullptr };
  Texts         texts_;
};

#endif
