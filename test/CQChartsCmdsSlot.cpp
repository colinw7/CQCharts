#include <CQChartsCmdsSlot.h>
#include <CQChartsCmds.h>
#include <CQChartsView.h>
#include <CQChartsPlot.h>
#include <CQChartsAnnotation.h>
#include <CQChartsModelData.h>

#include <CQTclUtil.h>

CQChartsCmdsSlotMgr::
CQChartsCmdsSlotMgr()
{
}

CQChartsCmdsSlotMgr::
~CQChartsCmdsSlotMgr()
{
  for (auto ps : slots_)
    delete ps.second;
}

CQChartsCmdsSlot *
CQChartsCmdsSlotMgr::
createSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
           CQChartsAnnotation *annotation, CQChartsModelData *modelData,
           const QString &fromName, const QString &toName)
{
  auto id = encodeId(view, plot, annotation, modelData, fromName, toName);

  auto *slot = new CQChartsCmdsSlot(cmds, view, plot, annotation, modelData, toName);

  slot->setId(id);

  slots_[id] = slot;

  return slot;
}

CQChartsCmdsSlot *
CQChartsCmdsSlotMgr::
getSlot(CQChartsCmds * /*cmds*/, CQChartsView *view, CQChartsPlot *plot,
        CQChartsAnnotation *annotation, CQChartsModelData *modelData,
        const QString &fromName, const QString &toName) const
{
  auto id = encodeId(view, plot, annotation, modelData, fromName, toName);

  auto ps = slots_.find(id);
  if (ps == slots_.end()) return nullptr;

  return (*ps).second;
}

void
CQChartsCmdsSlotMgr::
deleteSlot(CQChartsCmdsSlot *slot)
{
  slots_.erase(slot->id());

  slot->deleteLater();
}

QString
CQChartsCmdsSlotMgr::
encodeId(CQChartsView *view, CQChartsPlot *plot,
         CQChartsAnnotation *annotation, CQChartsModelData *modelData,
         const QString &fromName, const QString &toName) const
{
  QString id;

  if      (plot)
    id += "plot:" + plot->id();
  else if (view)
    id += "view:" + view->id();
  else if (annotation)
    id += "annotation:" + annotation->id();
  else if (modelData)
    id += "model:" + modelData->id();

  id += ":" + fromName;
  id += ":" + toName;

  return id;
}

//---

CQChartsCmdsSlot::
CQChartsCmdsSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                 CQChartsAnnotation *annotation, CQChartsModelData *modelData,
                 const QString &procName) :
 QObject(cmds), cmds_(cmds), view_(view), plot_(plot), annotation_(annotation),
 modelData_(modelData), procName_(procName)
{
}

CQChartsCmdsSlot::
~CQChartsCmdsSlot()
{
  if (obj_)
    disconnect(obj_, signal_.toLatin1().constData(), this, method_.toLatin1().constData());
}

void
CQChartsCmdsSlot::
connectSlot(QObject *obj, const char *signalName, const char *methodName)
{
  QObject::connect(obj, signalName, this, methodName);

  obj_    = obj;
  signal_ = signalName;
  method_ = methodName;
}

void
CQChartsCmdsSlot::
objIdPressed(const QString &id)
{
  auto cmd = getTclIdCmd(id);

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
annotationIdPressed(const QString &id)
{
  auto cmd = getTclIdCmd(id);

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
plotObjsAdded()
{
  auto cmd = getTclCmd();

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
selectionChanged()
{
  auto cmd = getTclCmd();

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
selectPress(const CQChartsGeom::Point &p)
{
  auto cmd = getTclCmd();

  cmd += QString(" %1 %2").arg(p.x).arg(p.y);

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
animateStateChanged(bool b)
{
  auto cmd = getTclCmd();

  cmd += QString(" %1").arg(b);

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
currentModelChanged()
{
  auto cmd = getTclCmd();

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
modelChanged()
{
  auto cmd = getTclCmd();

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
themeChanged()
{
  auto cmd = getTclCmd();

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
interfaceThemeChanged()
{
  auto cmd = getTclCmd();

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
keyEventPress(const QString &text)
{
  if (! view_) return;

  auto cmd = getTclCmd();

  cmd += QString(" {%1}").arg(text);

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
viewResized()
{
  if (! view_) return;

  auto cmd = getTclCmd();

  evalCmd(cmd);
}

void
CQChartsCmdsSlot::
evalCmd(const QString &cmd)
{
  if (! enabled_) return;

  enabled_ = false;

  cmds_->cmdBase()->qtcl()->eval(cmd, /*showError*/true, /*showResult*/false);

  enabled_ = true;
}

QString
CQChartsCmdsSlot::
getTclCmd() const
{
  auto cmd = procName_;

  if (view_)
    cmd += " \"" + view_->id() + "\"";

  if (plot_)
    cmd += " \"" + plot_->pathId() + "\"";

  if (annotation_)
    cmd += " \"" + annotation_->id() + "\"";

  if (modelData_)
    cmd += " \"" + modelData_->id() + "\"";

  return cmd;
}

QString
CQChartsCmdsSlot::
getTclIdCmd(const QString &id) const
{
  auto cmd = procName_;

  if (view_)
    cmd += " \"" + view_->id() + "\"";

  if (plot_)
    cmd += " \"" + plot_->pathId() + "\"";

  if (annotation_)
    cmd += " \"" + annotation_->id() + "\"";

  if (modelData_)
    cmd += " \"" + modelData_->id() + "\"";

  cmd += " \"" + id + "\"";

  return cmd;
}
