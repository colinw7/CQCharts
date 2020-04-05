#include <CQChartsCmdsSlot.h>
#include <CQChartsCmds.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsAnnotation.h>

#include <CQTclUtil.h>

CQChartsCmdsSlot::
CQChartsCmdsSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                 CQChartsAnnotation *annotation, const QString &procName) :
 QObject(cmds), cmds_(cmds), view_(view), plot_(plot), annotation_(annotation),
 procName_(procName)
{
}

void
CQChartsCmdsSlot::
objIdPressed(const QString &id)
{
  if (view_)
    disconnect(view_, SIGNAL(objIdPressed(const QString &)),
               this, SLOT(objIdPressed(const QString &)));

  if (plot_)
    disconnect(plot_, SIGNAL(objIdPressed(const QString &)),
               this, SLOT(objIdPressed(const QString &)));

  QString cmd = getTclIdCmd(id);

  evalCmd(cmd);

  if (view_)
    connect(view_, SIGNAL(objIdPressed(const QString &)),
            this, SLOT(objIdPressed(const QString &)));

  if (plot_)
    connect(plot_, SIGNAL(objIdPressed(const QString &)),
            this, SLOT(objIdPressed(const QString &)));
}

void
CQChartsCmdsSlot::
annotationIdPressed(const QString &id)
{
  if (view_)
    disconnect(view_, SIGNAL(annotationIdPressed(const QString &)),
               this, SLOT(annotationIdPressed(const QString &)));

  if (plot_)
    disconnect(plot_, SIGNAL(annotationIdPressed(const QString &)),
               this, SLOT(annotationIdPressed(const QString &)));

  if (annotation_)
    disconnect(annotation_, SIGNAL(pressed(const QString &)),
               this, SLOT(annotationIdPressed(const QString &)));

  QString cmd = getTclIdCmd(id);

  evalCmd(cmd);

  if (view_)
    connect(view_, SIGNAL(annotationIdPressed(const QString &)),
            this, SLOT(annotationIdPressed(const QString &)));

  if (plot_)
    connect(plot_, SIGNAL(annotationIdPressed(const QString &)),
            this, SLOT(annotationIdPressed(const QString &)));

  if (annotation_)
    connect(annotation_, SIGNAL(pressed(const QString &)),
            this, SLOT(annotationIdPressed(const QString &)));
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
  if (view_)
    disconnect(view_, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

  if (plot_)
    disconnect(plot_, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

  QString cmd = getTclCmd();

  evalCmd(cmd);

  if (view_)
    connect(view_, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));

  if (plot_)
    connect(plot_, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}

void
CQChartsCmdsSlot::
themeChanged()
{
  disconnect(cmds_->charts(), SIGNAL(themeChanged()), this, SLOT(themeChanged()));

  QString cmd = getTclCmd();

  evalCmd(cmd);

  connect(cmds_->charts(), SIGNAL(themeChanged()), this, SLOT(themeChanged()));
}

void
CQChartsCmdsSlot::
interfaceThemeChanged()
{
  disconnect(cmds_->charts(), SIGNAL(interfaceThemeChanged()), this, SLOT(interfaceThemeChanged()));

  QString cmd = getTclCmd();

  evalCmd(cmd);

  connect(cmds_->charts(), SIGNAL(interfaceThemeChanged()), this, SLOT(interfaceThemeChanged()));
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
  QString cmd = procName_;

  if (view_)
    cmd += " \"" + view_->id() + "\"";

  if (plot_)
    cmd += " \"" + plot_->pathId() + "\"";

  if (annotation_)
    cmd += " \"" + annotation_->id() + "\"";

  return cmd;
}

QString
CQChartsCmdsSlot::
getTclIdCmd(const QString &id) const
{
  QString cmd = procName_;

  if (view_)
    cmd += " \"" + view_->id() + "\"";

  if (plot_)
    cmd += " \"" + plot_->pathId() + "\"";

  if (annotation_)
    cmd += " \"" + annotation_->id() + "\"";

  cmd += " \"" + id + "\"";

  return cmd;
}
