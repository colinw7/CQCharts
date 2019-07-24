#include <CQChartsCmdsSlot.h>
#include <CQChartsCmds.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>

#include <CQTclUtil.h>

CQChartsCmdsSlot::
CQChartsCmdsSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                 const QString &procName) :
 QObject(cmds), cmds_(cmds), view_(view), plot_(plot), procName_(procName)
{
}

void
CQChartsCmdsSlot::
objIdPressed(const QString &id)
{
  QString cmd = getTclIdCmd(id);

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
annotationIdPressed(const QString &id)
{
  QString cmd = getTclIdCmd(id);

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
plotObjsAdded()
{
  if (plot_)
    disconnect(plot_, SIGNAL(plotObjsAdded()), this, SLOT(plotObjsAdded()));

  QString cmd = getTclCmd();

  evalCmd(cmd);

  if (plot_)
    connect(plot_, SIGNAL(plotObjsAdded()), this, SLOT(plotObjsAdded()));
}

void
CQChartsCmdsSlot::
selectionChanged()
{
  if (plot_)
    disconnect(plot_, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
  else
    disconnect(view_, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

  QString cmd = getTclCmd();

  evalCmd(cmd);

  if (plot_)
    connect(plot_, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
  else
    connect(view_, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}

void
CQChartsCmdsSlot::
evalCmd(const QString &cmd)
{
  cmds_->cmdBase()->qtcl()->eval(cmd, /*showError*/true, /*showResult*/false);
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
