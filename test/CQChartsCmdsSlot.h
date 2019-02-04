#ifndef CQChartsCmdsSlot_H
#define CQChartsCmdsSlot_H

#include <QObject>

class CQChartsCmds;
class CQChartsView;
class CQChartsPlot;

class CQChartsCmdsSlot : public QObject {
  Q_OBJECT

 public:
  CQChartsCmdsSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                   const QString &procName);

 private:
  QString getTclCmd() const;
  QString getTclIdCmd(const QString &id) const;

 public slots:
  void objIdPressed       (const QString &);
  void annotationIdPressed(const QString &);
  void plotObjsAdded      ();

 private:
  CQChartsCmds* cmds_ { nullptr };
  CQChartsView* view_ { nullptr };
  CQChartsPlot* plot_ { nullptr };
  QString       procName_;
};

#endif
