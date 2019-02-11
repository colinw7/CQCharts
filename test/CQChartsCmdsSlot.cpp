#include <CQChartsCmdsSlot.h>
#include <CQChartsCmds.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>

#include <CQTclUtil.h>

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
  QString cmd = getTclIdCmd(id);

  cmds_->cmdBase()->qtcl()->eval(cmd, /*showError*/true, /*showResult*/false);
}

void
CQChartsCmdsSlot::
annotationIdPressed(const QString &id)
{
  QString cmd = getTclIdCmd(id);

  cmds_->cmdBase()->qtcl()->eval(cmd, /*showError*/true, /*showResult*/false);
}

void
CQChartsCmdsSlot::
plotObjsAdded()
{
  disconnect(plot_, SIGNAL(plotObjsAdded()),
             this, SLOT(plotObjsAdded()));

  QString cmd = getTclCmd();

  cmds_->cmdBase()->qtcl()->eval(cmd, /*showError*/true, /*showResult*/false);

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
