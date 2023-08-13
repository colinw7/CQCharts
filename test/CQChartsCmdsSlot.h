#ifndef CQChartsCmdsSlot_H
#define CQChartsCmdsSlot_H

#include <CQChartsGeom.h>

#include <QObject>
#include <map>

class CQChartsCmds;
class CQChartsView;
class CQChartsPlot;
class CQChartsAnnotation;
class CQChartsModelData;

/*!
 * \brief Charts Tcl Command Callback Handler
 * \ingroup Charts
 */
class CQChartsCmdsSlot : public QObject {
  Q_OBJECT

 public:
  CQChartsCmdsSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                   CQChartsAnnotation *annotation, CQChartsModelData *modelData,
                   const QString &procName);

 ~CQChartsCmdsSlot();

  const QString &id() const { return id_; }
  void setId(const QString &s) { id_ = s; }

  void connectSlot(QObject *obj, const char *signalName, const char *methodName);

 private:
  void evalCmd(const QString &cmd);

  QString getTclCmd() const;
  QString getTclIdCmd(const QString &id) const;

 public Q_SLOTS:
  // view
  void keyEventPress(const QString &);
  void viewResized  ();

  // plot, view, model
  void objIdPressed    (const QString &);
  void selectionChanged();

  // plot, view, annotation
  void annotationIdPressed(const QString &);

  // plot
  void plotObjsAdded      ();
  void selectPress        (const CQChartsGeom::Point &);
  void animateStateChanged(bool);
  void currentModelChanged();

  // model
  void modelChanged();

  // global
  void themeChanged         ();
  void interfaceThemeChanged();

 private:
  CQChartsCmds*       cmds_       { nullptr };
  CQChartsView*       view_       { nullptr };
  CQChartsPlot*       plot_       { nullptr };
  CQChartsAnnotation* annotation_ { nullptr };
  CQChartsModelData*  modelData_  { nullptr };
  QString             procName_;
  QString             id_;

  QObject* obj_ { nullptr };
  QString  signal_;
  QString  method_;

  mutable bool enabled_ { true };
};

//---

#define CQChartsCmdsSlotMgrInst CQChartsCmdsSlotMgr::instance()

class CQChartsCmdsSlotMgr {
 public:
  static CQChartsCmdsSlotMgr *instance() {
    static CQChartsCmdsSlotMgr *inst;
    if (! inst)
      inst = new CQChartsCmdsSlotMgr;
    return inst;
  }

 ~CQChartsCmdsSlotMgr();

  CQChartsCmdsSlot *createSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                               CQChartsAnnotation *annotation, CQChartsModelData *modelData,
                               const QString &fromName, const QString &toName);

  CQChartsCmdsSlot *getSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                            CQChartsAnnotation *annotation, CQChartsModelData *modelData,
                            const QString &fromName, const QString &toName) const;

  void deleteSlot(CQChartsCmdsSlot *slot);

 private:
  CQChartsCmdsSlotMgr();

  QString encodeId(CQChartsView *view, CQChartsPlot *plot,
                   CQChartsAnnotation *annotation, CQChartsModelData *modelData,
                   const QString &fromName, const QString &toName) const;

 private:
  using Slots = std::map<QString, CQChartsCmdsSlot *>;

  Slots slots_;
};

#endif
