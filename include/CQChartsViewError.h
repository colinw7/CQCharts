#ifndef CQChartsViewError_H
#define CQChartsViewError_H

#include <QFrame>
#include <map>

class CQChartsView;
class CQChartsPlot;
class CQTabSplit;
class QTextBrowser;

class CQChartsViewError : public QFrame {
  Q_OBJECT

 public:
  CQChartsViewError(CQChartsView *view);

  void updatePlots();

  QSize sizeHint() const override;

 private:
  using Texts = std::map<CQChartsPlot *, QTextBrowser *>;

  CQChartsView* view_ { nullptr };
  CQTabSplit*   tab_  { nullptr };
  Texts         texts_;
};

#endif
