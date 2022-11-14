#ifndef CQChartsViewError_H
#define CQChartsViewError_H

#include <QFrame>
#include <map>

class CQChartsView;
class CQChartsPlot;
class CQTabSplit;
class QTextBrowser;

/*!
 * \brief Widget to show View/Plot errors
 * \ingroup Charts
 */
class CQChartsViewError : public QFrame {
  Q_OBJECT

 public:
  using View = CQChartsView;
  using Plot = CQChartsPlot;

 public:
  CQChartsViewError(View *view);

  void updatePlots();

  QSize sizeHint() const override;

 private:
  using Texts = std::map<Plot *, QTextBrowser *>;

  View*       view_ { nullptr };
  CQTabSplit* tab_  { nullptr };
  Texts       texts_;
};

#endif
