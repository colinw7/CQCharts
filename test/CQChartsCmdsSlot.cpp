#include <CQChartsCmdsSlot.h>
#include <CQChartsCmds.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>

#ifdef CQCharts_USE_TCL
#include <CQTclUtil.h>
#endif

CQChartsCmdsSlot::
CQChartsCmdsSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                 const QString &procName) :
 cmds_(cmds), view_(view), plot_(plot), procName_(procName)
{
}

void
CQChartsCmdsSlot::
objIdPressed(const QString &id)
{
#ifdef CQCharts_USE_TCL
  QString cmd = getTclIdCmd(id);

  cmds_->cmdBase()->qtcl()->eval(cmd, /*showError*/true, /*showResult*/false);
#else
  std::cerr << "objIdPressed: " << id.toStdString() << "\n";
#endif
}

void
CQChartsCmdsSlot::
annotationIdPressed(const QString &id)
{
#ifdef CQCharts_USE_TCL
  QString cmd = getTclIdCmd(id);

  cmds_->cmdBase()->qtcl()->eval(cmd, /*showError*/true, /*showResult*/false);
#else
  std::cerr << "annotationIdPressed: " << id.toStdString() << "\n";
#endif
}

void
CQChartsCmdsSlot::
plotObjsAdded()
{
  disconnect(plot_, SIGNAL(plotObjsAdded()),
             this, SLOT(plotObjsAdded()));

#ifdef CQCharts_USE_TCL
  QString cmd = getTclCmd();

  cmds_->cmdBase()->qtcl()->eval(cmd, /*showError*/true, /*showResult*/false);
#else
  std::cerr << "plotObjsAdded\n";
#endif

  connect(plot_, SIGNAL(plotObjsAdded()),
          this, SLOT(plotObjsAdded()));
}

QString
CQChartsCmdsSlot::
getTclCmd() const
{
  QString viewName = view_->id();

  QString cmd = procName_;

  cmd += " \"" + viewName + "\"";

  if (plot_)
    cmd += " \"" + plot_->pathId() + "\"";

  return cmd;
}

QString
CQChartsCmdsSlot::
getTclIdCmd(const QString &id) const
{
  QString viewName = view_->id();

  QString cmd = procName_;

  cmd += " \"" + viewName + "\"";

  if (plot_)
    cmd += " \"" + plot_->pathId() + "\"";

  cmd += " \"" + id + "\"";

  return cmd;
}
