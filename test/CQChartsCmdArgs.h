#ifndef CQChartsCmdArgs_H
#define CQChartsCmdArgs_H

#include <CQChartsMargin.h>
#include <CQChartsPosition.h>
#include <CQChartsRect.h>
#include <CQChartsColor.h>
#include <CQChartsAlpha.h>
#include <CQChartsFillPattern.h>
#include <CQChartsFont.h>
#include <CQChartsLineDash.h>
#include <CQChartsSides.h>
#include <CQChartsPoints.h>
#include <CQChartsReals.h>
#include <CQChartsAngle.h>

#include <CQTclCmd.h>
//#include <CQTclUtil.h>
#include <CQAlignEdit.h>
#include <CQStrParse.h>

#include <tcl.h>

#include <boost/optional.hpp>

#include <set>

//------

/*!
 * \brief base class for handling command arguments
 * \ingroup Charts
 */
class CQChartsCmdBaseArgs : public CQTclCmd::CmdArgs {
 public:
  using Polygon = CQChartsGeom::Polygon;
  using Point   = CQChartsGeom::Point;

  //---

  //! types
  enum class Type {
    None     = int(CQTclCmd::CmdArg::Type::None),
    Boolean  = int(CQTclCmd::CmdArg::Type::Boolean),
    Integer  = int(CQTclCmd::CmdArg::Type::Integer),
    Real     = int(CQTclCmd::CmdArg::Type::Real),
    String   = int(CQTclCmd::CmdArg::Type::String),
    SBool    = int(CQTclCmd::CmdArg::Type::SBool),
    Enum     = int(CQTclCmd::CmdArg::Type::Enum),
    Color    = int(CQTclCmd::CmdArg::Type::Extra) + 1,
    Font     = int(CQTclCmd::CmdArg::Type::Extra) + 2,
    LineDash = int(CQTclCmd::CmdArg::Type::Extra) + 3,
    Length   = int(CQTclCmd::CmdArg::Type::Extra) + 4,
    Position = int(CQTclCmd::CmdArg::Type::Extra) + 5,
    Rect     = int(CQTclCmd::CmdArg::Type::Extra) + 6,
    Polygon  = int(CQTclCmd::CmdArg::Type::Extra) + 7,
    Align    = int(CQTclCmd::CmdArg::Type::Extra) + 8,
    Sides    = int(CQTclCmd::CmdArg::Type::Extra) + 9,
    Column   = int(CQTclCmd::CmdArg::Type::Extra) + 10,
    Row      = int(CQTclCmd::CmdArg::Type::Extra) + 11,
    Reals    = int(CQTclCmd::CmdArg::Type::Extra) + 12
  };

 public:
  CQChartsCmdBaseArgs(const QString &cmdName, const Args &argv);

  //---

  using CQTclCmd::CmdArgs::getOptValue;

  // get alignment value of current option
  bool getOptValue(Qt::Alignment &a);

  // get font value of current option
  //bool getOptValue(QFont &f);

  // get font value of current option
  bool getOptValue(CQChartsFont &f);

  // get length value of current option
  bool getOptValue(CQChartsLength &l);

  // get position value of current option
  bool getOptValue(CQChartsPosition &p);

  // get rect value of current option
  bool getOptValue(CQChartsRect &r);

  // get color value of current option
  bool getOptValue(CQChartsColor &c);

  // get line dash value of current option
  bool getOptValue(CQChartsLineDash &d);

  // get polygon value of current option
  bool getOptValue(Polygon &poly);

  // get reals value of current option
  bool getOptValue(CQChartsReals &r);

  //---

  bool handleParseArg(CQTclCmd::CmdArg *cmdArg, const QString &opt) override;

  // get parsed align value for option
  Qt::Alignment getParseAlign(const QString &name, Qt::Alignment def=Qt::AlignCenter) const;

  // get parsed polygon value for option
  Polygon getParsePoly(const QString &name, const Polygon &def=Polygon()) const;

  //---

  // get parsed font for option (default returned if not found)
  CQChartsFont getParseFont(const QString &name, const CQChartsFont &def=CQChartsFont()) const;

  // get parsed font for option (default returned if not found)
  //QFont getParseFont(const QString &name, const QFont &def=QFont()) const;

  // get parsed color for option (default returned if not found)
  CQChartsColor getParseColor(const QString &name, const CQChartsColor &def=CQChartsColor()) const;

  // get parsed alpha for option (default returned if not found)
  CQChartsAlpha getParseAlpha(const QString &name, const CQChartsAlpha &def=CQChartsAlpha()) const;

  // get parsed pattern for option (default returned if not found)
  CQChartsFillPattern getParsePattern(const QString &name,
                                      const CQChartsFillPattern &def=CQChartsFillPattern()) const;

  // get parsed angle for option (default returned if not found)
  CQChartsAngle getParseAngle(const QString &name, const CQChartsAngle &def=CQChartsAngle()) const;

  // get parsed line dash for option (default returned if not found)
  CQChartsLineDash getParseLineDash(const QString &name,
                                    const CQChartsLineDash &def=CQChartsLineDash()) const;

  // get parsed line side for option (default returned if not found)
  CQChartsSides getParseSides(const QString &name, const CQChartsSides &def=CQChartsSides()) const;

  //---

 protected:
  // string to polygon
  Polygon stringToPolygon(const QString &str) const;

  // parse polygon from string
  bool parsePoly(const QString &str, Polygon &poly) const;

  // parse polygon at parse position
  bool parsePoly(CQStrParse &parse, Polygon &poly) const;

  // parse point from string
  bool parsePoint(const QString &str, Point &pos) const;

  // parse point at parse position
  bool parsePoint(CQStrParse &parse, Point &pos) const;
};

//------

#include <CQChartsPlot.h>
#include <CQChartsModelUtil.h>
#include <CQChartsColumn.h>
#include <CQChartsRow.h>

namespace CQChartsCmdUtil {

template<typename T>
T viewPlotStringToValue(const QString &, CQChartsView *, CQChartsPlot *) {
  assert(false);
}

template<typename T>
T plotStringToValue(const QString &, CQChartsPlot *) {
  assert(false);
}

template<>
inline CQChartsLength
viewPlotStringToValue(const QString &str, CQChartsView *view, CQChartsPlot *) {
  return CQChartsLength(str, (view ? CQChartsUnits::Type::VIEW : CQChartsUnits::Type::PLOT));
}

template<>
inline CQChartsPosition
viewPlotStringToValue(const QString &str, CQChartsView *view, CQChartsPlot *) {
  return CQChartsPosition(str, (view ? CQChartsUnits::Type::VIEW : CQChartsUnits::Type::PLOT));
}

template<>
inline CQChartsRect
viewPlotStringToValue(const QString &str, CQChartsView *view, CQChartsPlot *) {
  return CQChartsRect(str, (view ? CQChartsUnits::Type::VIEW : CQChartsUnits::Type::PLOT));
}

template<>
inline CQChartsRow
plotStringToValue(const QString &str, CQChartsPlot *plot) {
  QString rowName = str;

  bool ok;

  long irow = CQChartsUtil::toInt(rowName, ok);

  if (! ok) {
    if (plot)
      irow = plot->getRowForId(rowName);
    else
      irow = -1;
  }

  return CQChartsRow(int(irow));
}

template<>
inline CQChartsPoints
viewPlotStringToValue(const QString &str, CQChartsView *view, CQChartsPlot *) {
  return CQChartsPoints(str, (view ? CQChartsUnits::Type::VIEW : CQChartsUnits::Type::PLOT));
}

//---

template<typename T>
T modelStringToValue(const QString &, QAbstractItemModel *) {
  assert(false);
}

template<>
inline CQChartsColumn
modelStringToValue(const QString &str, QAbstractItemModel *model) {
  CQChartsColumn column;

  if (! CQChartsModelUtil::stringToColumn(model, str, column))
    column = CQChartsColumn();

  return column;
}

}

//---

/*!
 * \brief derived class for handling command arguments (adds extra classes)
 * \ingroup Charts
 */
class CQChartsCmdArgs : public CQChartsCmdBaseArgs {
 public:
  CQChartsCmdArgs(const QString &cmdName, const Args &argv);

  // get parsed generic value for option of view/plot command
  template<typename T>
  T getParseValue(CQChartsView *view, CQChartsPlot *plot, const QString &name,
                  const T &def=T()) const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return def;

    return CQChartsCmdUtil::viewPlotStringToValue<T>((*p).second[0], view, plot);
  }

  CQChartsLength getParseLength(CQChartsView *view, CQChartsPlot *plot, const QString &name,
                                const CQChartsLength &def=CQChartsLength()) const;

  CQChartsPosition getParsePosition(CQChartsView *view, CQChartsPlot *plot, const QString &name,
                                    const CQChartsPosition &def=CQChartsPosition()) const;

  CQChartsRect getParseRect(CQChartsView *view, CQChartsPlot *plot, const QString &name,
                            const CQChartsRect &def=CQChartsRect()) const;

  CQChartsMargin getParseMargin(CQChartsView *view, CQChartsPlot *plot, const QString &name,
                                const CQChartsMargin &def=CQChartsMargin()) const;

  CQChartsPoints getParsePoints(CQChartsView *view, CQChartsPlot *plot, const QString &name,
                                const CQChartsPoints &def=CQChartsPoints()) const;

  //---

  // get parsed generic value for option of model command
  template<typename T>
  T getParseValue(QAbstractItemModel *model, const QString &name, const T &def=T()) const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return def;

    return CQChartsCmdUtil::modelStringToValue<T>((*p).second[0], model);
  }

  CQChartsColumn getParseColumn(const QString &name, QAbstractItemModel *model,
                                const CQChartsColumn &def=CQChartsColumn()) const;

#if 0
  CQChartsColumn getParseColumn(const QString &name, QAbstractItemModel *model) const;
#endif

  // get parsed generic value for option of view/plot command
  template<typename T>
  T getParseValue(CQChartsPlot *plot, const QString &name, const T &def=T()) const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return def;

    return CQChartsCmdUtil::plotStringToValue<T>((*p).second[0], plot);
  }

  // get parsed generic value for option
  template<typename T>
  T getParseValue(const QString &name, const T &def=T()) const {
    auto p = parseStr_.find(name);
    if (p == parseStr_.end()) return def;

    return T((*p).second[0]);
  }

  CQChartsRow getParseRow(const QString &name, CQChartsPlot *plot=nullptr,
                          const CQChartsRow &def=CQChartsRow()) const;

#if 0
  CQChartsRow getParseRow(const QString &name, CQChartsPlot *plot=nullptr) const;
#endif

  CQChartsReals getParseReals(const QString &name, const CQChartsReals &def=CQChartsReals()) const;
};

#endif
