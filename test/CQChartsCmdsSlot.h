#ifndef CQChartsCmdsSlot_H
#define CQChartsCmdsSlot_H

#include <CQChartsGeom.h>

#include <QObject>

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
};

#endif
