#ifndef CQChartsCmdsSlot_H
#define CQChartsCmdsSlot_H

#include <CQChartsGeom.h>

#include <QObject>

class CQChartsCmds;
class CQChartsView;
class CQChartsPlot;
class CQChartsAnnotation;

/*!
 * \brief Charts Tcl Command Callback Handler
 * \ingroup Charts
 */
class CQChartsCmdsSlot : public QObject {
  Q_OBJECT

 public:
  CQChartsCmdsSlot(CQChartsCmds *cmds, CQChartsView *view, CQChartsPlot *plot,
                   CQChartsAnnotation *annotation, const QString &procName);

 private:
  void evalCmd(const QString &cmd);

  QString getTclCmd() const;
  QString getTclIdCmd(const QString &id) const;

 public Q_SLOTS:
  void objIdPressed         (const QString &);
  void annotationIdPressed  (const QString &);
  void plotObjsAdded        ();
  void selectionChanged     ();
  void selectPress          (const CQChartsGeom::Point &);
  void themeChanged         ();
  void interfaceThemeChanged();
  void keyEventPress        (const QString &);

 private:
  CQChartsCmds*       cmds_       { nullptr };
  CQChartsView*       view_       { nullptr };
  CQChartsPlot*       plot_       { nullptr };
  CQChartsAnnotation* annotation_ { nullptr };
  QString             procName_;
};

#endif
