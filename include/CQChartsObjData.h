#ifndef CQChartsObjData_H
#define CQChartsObjData_H

#include <CQChartsData.h>
#include <CQChartsInvalidator.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsUtil.h>

#define CQCHARTS_LINE_DATA_PROPERTIES \
Q_PROPERTY(CQChartsLineData lineData READ lineData WRITE setLineData) \
\
Q_PROPERTY(bool             lines      READ isLines    WRITE setLines     ) \
Q_PROPERTY(CQChartsColor    linesColor READ linesColor WRITE setLinesColor) \
Q_PROPERTY(CQChartsAlpha    linesAlpha READ linesAlpha WRITE setLinesAlpha) \
Q_PROPERTY(CQChartsLength   linesWidth READ linesWidth WRITE setLinesWidth) \
Q_PROPERTY(CQChartsLineDash linesDash  READ linesDash  WRITE setLinesDash )

/*!
 * \brief Object line data
 * \ingroup Charts
 */
template<class OBJ>
class CQChartsObjLineData {
 public:
  CQChartsObjLineData(OBJ *obj) :
   lineDataObj_(obj) {
  }

  bool isReloadObj() const { return reloadObj_; }
  void setReloadObj(bool b) { reloadObj_ = b; }

  bool isLines() const { return lineData_.isVisible(); }
  void setLines(bool b) {
    if (b != lineData_.isVisible()) {
      lineData_.setVisible(b); lineDataInvalidate(/*reload*/isReloadObj()); }
  }

  const CQChartsColor &linesColor() const { return lineData_.color(); }
  void setLinesColor(const CQChartsColor &c) {
    if (c != lineData_.color()) {
      lineData_.setColor(c); lineDataInvalidate(); }
  }

  QColor interpLinesColor(const CQChartsUtil::ColorInd &ind) const {
    return CQChartsInterpolator(lineDataObj_).interpColor(linesColor(), ind);
  }

  const CQChartsAlpha &linesAlpha() const { return lineData_.alpha(); }
  void setLinesAlpha(const CQChartsAlpha &a) {
    if (a != lineData_.alpha()) {
      lineData_.setAlpha(a); lineDataInvalidate(); }
  }

  const CQChartsLength &linesWidth() const { return lineData_.width(); }
  void setLinesWidth(const CQChartsLength &l) {
    if (l != lineData_.width()) {
      lineData_.setWidth(l); lineDataInvalidate(/*reload*/isReloadObj()); }
   }

  const CQChartsLineDash &linesDash() const { return lineData_.dash(); }
  void setLinesDash(const CQChartsLineDash &d) {
    if (d != lineData_.dash()) {
      lineData_.setDash(d); lineDataInvalidate(); }
  }

  void setLineDataPen(QPen &pen, const CQChartsUtil::ColorInd &ind) const {
    QColor lc = interpLinesColor(ind);

    CQChartsPenBrush penBrush;
    lineDataObj_->setPen(penBrush,
      CQChartsPenData(isLines(), lc, linesAlpha(), linesWidth(), linesDash()));
    pen = penBrush.pen;
  }

  //---

  const CQChartsLineData &lineData() const { return lineData_; }

  void setLineData(const CQChartsLineData &data) {
    lineData_ = data; lineDataInvalidate(/*reload*/isReloadObj());
  };

 private:
  void lineDataInvalidate(bool reload=false) {
    CQChartsInvalidator(lineDataObj_).invalidate(reload);
  }

 private:
  OBJ* lineDataObj_ { nullptr };
  bool reloadObj_   { true };

 protected:
  CQChartsLineData lineData_;
};

//------

#define CQCHARTS_NAMED_LINE_DATA_PROPERTIES(UNAME, LNAME) \
Q_PROPERTY(CQChartsLineData LNAME##LineData READ LNAME##LineData WRITE set##UNAME##LineData) \
\
Q_PROPERTY(bool             LNAME##Lines      READ is##UNAME##Lines  WRITE set##UNAME##Lines     ) \
Q_PROPERTY(CQChartsColor    LNAME##LinesColor READ LNAME##LinesColor WRITE set##UNAME##LinesColor) \
Q_PROPERTY(CQChartsAlpha    LNAME##LinesAlpha READ LNAME##LinesAlpha WRITE set##UNAME##LinesAlpha) \
Q_PROPERTY(CQChartsLength   LNAME##LinesWidth READ LNAME##LinesWidth WRITE set##UNAME##LinesWidth) \
Q_PROPERTY(CQChartsLineDash LNAME##LinesDash  READ LNAME##LinesDash  WRITE set##UNAME##LinesDash )

/*!
 * \brief Object named line data
 * \ingroup Charts
 */
#define CQCHARTS_NAMED_LINE_DATA(UNAME, LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##LineData { \
 public: \
  CQChartsObj##UNAME##LineData(OBJ *obj) : \
   LNAME##LineDataObj_(obj) { \
  } \
\
  bool is##UNAME##ReloadObj() const { return LNAME##ReloadObj_; } \
  void set##UNAME##ReloadObj(bool b) { LNAME##ReloadObj_ = b; } \
\
  bool is##UNAME##Lines() const { return LNAME##LineData_.isVisible(); } \
  void set##UNAME##Lines(bool b) { \
    if (b != LNAME##LineData_.isVisible()) { \
      LNAME##LineData_.setVisible(b); LNAME##LineDataInvalidate(is##UNAME##ReloadObj()); } \
  } \
\
  const CQChartsColor &LNAME##LinesColor() const { return LNAME##LineData_.color(); } \
  void set##UNAME##LinesColor(const CQChartsColor &c) { \
    if (c != LNAME##LineData_.color()) { \
      LNAME##LineData_.setColor(c); LNAME##LineDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##LinesColor(const CQChartsUtil::ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##LineDataObj_).interpColor(LNAME##LinesColor(), ind); \
  } \
\
  const CQChartsAlpha &LNAME##LinesAlpha() const { return LNAME##LineData_.alpha(); } \
  void set##UNAME##LinesAlpha(const CQChartsAlpha &a) { \
    if (a != LNAME##LineData_.alpha()) { \
      LNAME##LineData_.setAlpha(a); LNAME##LineDataInvalidate(); } \
  } \
\
  const CQChartsLength &LNAME##LinesWidth() const { return LNAME##LineData_.width(); } \
  void set##UNAME##LinesWidth(const CQChartsLength &l) { \
    if (l != LNAME##LineData_.width()) { \
      LNAME##LineData_.setWidth(l); LNAME##LineDataInvalidate(); } \
   } \
\
  const CQChartsLineDash &LNAME##LinesDash() const { return LNAME##LineData_.dash(); } \
  void set##UNAME##LinesDash(const CQChartsLineDash &d) { \
    if (d != LNAME##LineData_.dash()) { \
      LNAME##LineData_.setDash(d); LNAME##LineDataInvalidate(); } \
  } \
\
  void set##UNAME##LineDataPen(QPen &pen, const CQChartsUtil::ColorInd &ind) const { \
    QColor lc = interp##UNAME##LinesColor(ind); \
\
    CQChartsPenBrush penBrush; \
    LNAME##LineDataObj_->setPen(penBrush, \
      CQChartsPenData(is##UNAME##Lines(), lc, LNAME##LinesAlpha(), \
                      LNAME##LinesWidth(), LNAME##LinesDash())); \
    pen = penBrush.pen; \
  } \
\
  const CQChartsLineData &LNAME##LineData() const { return LNAME##LineData_; } \
\
  void set##UNAME##LineData(const CQChartsLineData &data) { \
    LNAME##LineData_ = data; LNAME##LineDataInvalidate(); \
  } \
\
 private: \
  void LNAME##LineDataInvalidate(bool reload=false) { \
    CQChartsInvalidator(LNAME##LineDataObj_).invalidate(reload); \
  } \
\
 private: \
  OBJ* LNAME##LineDataObj_ { nullptr }; \
\
 protected: \
  CQChartsLineData LNAME##LineData_; \
  bool             LNAME##ReloadObj_ { true }; \
};

//------

#define CQCHARTS_POINT_DATA_PROPERTIES \
Q_PROPERTY(CQChartsSymbolData symbolData READ symbolData WRITE setSymbolData) \
\
Q_PROPERTY(bool                points \
           READ isPoints          WRITE setPoints           ) \
Q_PROPERTY(CQChartsSymbol      symbolType \
           READ symbolType        WRITE setSymbolType       ) \
Q_PROPERTY(CQChartsLength      symbolSize \
           READ symbolSize        WRITE setSymbolSize       ) \
Q_PROPERTY(bool                symbolStroked \
           READ isSymbolStroked   WRITE setSymbolStroked    ) \
Q_PROPERTY(CQChartsColor       symbolStrokeColor \
           READ symbolStrokeColor WRITE setSymbolStrokeColor) \
Q_PROPERTY(CQChartsAlpha       symbolStrokeAlpha \
           READ symbolStrokeAlpha WRITE setSymbolStrokeAlpha) \
Q_PROPERTY(CQChartsLength      symbolStrokeWidth \
           READ symbolStrokeWidth WRITE setSymbolStrokeWidth) \
Q_PROPERTY(CQChartsLineDash    symbolStrokeDash \
           READ symbolStrokeDash  WRITE setSymbolStrokeDash ) \
Q_PROPERTY(bool                symbolFilled \
           READ isSymbolFilled    WRITE setSymbolFilled     ) \
Q_PROPERTY(CQChartsColor       symbolFillColor \
           READ symbolFillColor   WRITE setSymbolFillColor  ) \
Q_PROPERTY(CQChartsAlpha       symbolFillAlpha \
           READ symbolFillAlpha   WRITE setSymbolFillAlpha  ) \
Q_PROPERTY(CQChartsFillPattern symbolFillPattern \
           READ symbolFillPattern WRITE setSymbolFillPattern)

/*!
 * \brief Object point data
 * \ingroup Charts
 */
template<class OBJ>
class CQChartsObjPointData {
 public:
  CQChartsObjPointData(OBJ *obj) :
   pointDataObj_(obj) {
  }

  bool isReloadObj() const { return reloadObj_; }
  void setReloadObj(bool b) { reloadObj_ = b; }

  bool isPoints() const { return pointData_.isVisible(); }
  void setPoints(bool b) {
    if (b != pointData_.isVisible()) {
      pointData_.setVisible(b); pointDataInvalidate(isReloadObj()); }
  }

  const CQChartsSymbol &symbolType() const { return pointData_.type(); }
  void setSymbolType(const CQChartsSymbol &t) {
    if (t != pointData_.type()) {
      pointData_.setType(t); pointDataInvalidate(); }
  }

  const CQChartsLength &symbolSize() const { return pointData_.size(); }
  void setSymbolSize(const CQChartsLength &s) {
    if (s != pointData_.size()) {
      pointData_.setSize(s); pointDataInvalidate(); }
  }

  bool isSymbolStroked() const { return pointData_.stroke().isVisible(); }
  void setSymbolStroked(bool b) {
    if (b != pointData_.stroke().isVisible()) {
      pointData_.stroke().setVisible(b); pointDataInvalidate(); }
  }

  const CQChartsColor &symbolStrokeColor() const { return pointData_.stroke().color(); }
  void setSymbolStrokeColor(const CQChartsColor &c) {
    if (c != pointData_.stroke().color()) {
      pointData_.stroke().setColor(c); pointDataInvalidate(); }
  }

  QColor interpSymbolStrokeColor(const CQChartsUtil::ColorInd &ind) const {
    return CQChartsInterpolator(pointDataObj_).interpColor(symbolStrokeColor(), ind);
  }

  const CQChartsAlpha &symbolStrokeAlpha() const { return pointData_.stroke().alpha(); }
  void setSymbolStrokeAlpha(const CQChartsAlpha &a) {
    if (a != pointData_.stroke().alpha()) {
      pointData_.stroke().setAlpha(a); pointDataInvalidate(); }
  }

  const CQChartsLength &symbolStrokeWidth() const { return pointData_.stroke().width(); }
  void setSymbolStrokeWidth(const CQChartsLength &l) {
    if (l != pointData_.stroke().width()) {
      pointData_.stroke().setWidth(l); pointDataInvalidate(); }
  }

  const CQChartsLineDash &symbolStrokeDash() const { return pointData_.stroke().dash(); }
  void setSymbolStrokeDash(const CQChartsLineDash &d) {
    if (d != pointData_.stroke().dash()) {
      pointData_.stroke().setDash(d); pointDataInvalidate(); }
  }

  bool isSymbolFilled() const { return pointData_.fill().isVisible(); }
  void setSymbolFilled(bool b) {
    if (b != pointData_.fill().isVisible()) {
      pointData_.fill().setVisible(b); pointDataInvalidate(); }
  }

  const CQChartsColor &symbolFillColor() const { return pointData_.fill().color(); }
  void setSymbolFillColor(const CQChartsColor &c) {
    if (c != pointData_.fill().color()) {
      pointData_.fill().setColor(c); pointDataInvalidate(); }
  }

  QColor interpSymbolFillColor(const CQChartsUtil::ColorInd &ind) const {
    return CQChartsInterpolator(pointDataObj_).interpColor(symbolFillColor(), ind);
  }

  const CQChartsAlpha &symbolFillAlpha() const { return pointData_.fill().alpha(); }
  void setSymbolFillAlpha(const CQChartsAlpha &a) {
    if (a != pointData_.fill().alpha()) {
      pointData_.fill().setAlpha(a); pointDataInvalidate(); }
  }

  const CQChartsFillPattern &symbolFillPattern() const { return pointData_.fill().pattern(); }
  void setSymbolFillPattern(const CQChartsFillPattern &p) {
    if (p != pointData_.fill().pattern()) {
      pointData_.fill().setPattern(p); pointDataInvalidate(); }
  }

  //---

  void setSymbolPenBrush(CQChartsPenBrush &penBrush, const CQChartsUtil::ColorInd &ind) const {
    pointDataObj_->setPenBrush(penBrush,
      CQChartsPenData(isSymbolStroked(), interpSymbolStrokeColor(ind), symbolStrokeAlpha(),
                      symbolStrokeWidth(), symbolStrokeDash()),
      CQChartsBrushData(isSymbolFilled(), interpSymbolFillColor(ind), symbolFillAlpha(),
                        symbolFillPattern()));
  }

  //---

  const CQChartsSymbolData &symbolData() const { return pointData_; }

  void setSymbolData(const CQChartsSymbolData &data) {
    pointData_ = data; pointDataInvalidate();
  };

 private:
  void pointDataInvalidate(bool reload=false) {
    CQChartsInvalidator(pointDataObj_).invalidate(reload);
  }

 private:
  OBJ* pointDataObj_ { nullptr };

 protected:
  CQChartsSymbolData pointData_;
  bool               reloadObj_ { true };
};

//------

#define CQCHARTS_NAMED_POINT_DATA_PROPERTIES(UNAME, LNAME) \
Q_PROPERTY(CQChartsSymbolData LNAME##SymbolData \
           READ LNAME##SymbolData WRITE set##UNAME##SymbolData) \
\
Q_PROPERTY(bool                LNAME##Points \
           READ is##UNAME##Points        WRITE set##UNAME##Points           ) \
Q_PROPERTY(CQChartsSymbol      LNAME##SymbolType \
           READ LNAME##SymbolType        WRITE set##UNAME##SymbolType       ) \
Q_PROPERTY(CQChartsLength      LNAME##SymbolSize \
           READ LNAME##SymbolSize        WRITE set##UNAME##SymbolSize       ) \
Q_PROPERTY(bool                LNAME##SymbolStroked \
           READ is##UNAME##SymbolStroked WRITE set##UNAME##SymbolStroked    ) \
Q_PROPERTY(CQChartsColor       LNAME##SymbolStrokeColor \
           READ LNAME##SymbolStrokeColor WRITE set##UNAME##SymbolStrokeColor) \
Q_PROPERTY(CQChartsAlpha       LNAME##SymbolStrokeAlpha \
           READ LNAME##SymbolStrokeAlpha WRITE set##UNAME##SymbolStrokeAlpha) \
Q_PROPERTY(CQChartsLength      LNAME##SymbolStrokeWidth \
           READ LNAME##SymbolStrokeWidth WRITE set##UNAME##SymbolStrokeWidth) \
Q_PROPERTY(CQChartsLineDash    LNAME##SymbolStrokeDash \
           READ LNAME##SymbolStrokeDash  WRITE set##UNAME##SymbolStrokeDash ) \
Q_PROPERTY(bool                LNAME##SymbolFilled \
           READ is##UNAME##SymbolFilled  WRITE set##UNAME##SymbolFilled     ) \
Q_PROPERTY(CQChartsColor       LNAME##SymbolFillColor \
           READ LNAME##SymbolFillColor   WRITE set##UNAME##SymbolFillColor  ) \
Q_PROPERTY(CQChartsAlpha       LNAME##SymbolFillAlpha \
           READ LNAME##SymbolFillAlpha   WRITE set##UNAME##SymbolFillAlpha  ) \
Q_PROPERTY(CQChartsFillPattern LNAME##SymbolFillPattern \
           READ LNAME##SymbolFillPattern WRITE set##UNAME##SymbolFillPattern)

/*!
 * \brief Object named point data
 * \ingroup Charts
 */
#define CQCHARTS_NAMED_POINT_DATA(UNAME, LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##PointData { \
 public: \
  CQChartsObj##UNAME##PointData(OBJ *obj) : \
   LNAME##PointDataObj_(obj) { \
  } \
\
  bool is##UNAME##ReloadObj() const { return LNAME##ReloadObj_; } \
  void set##UNAME##ReloadObj(bool b) { LNAME##ReloadObj_ = b; } \
\
  bool is##UNAME##Points() const { return LNAME##PointData_.isVisible(); } \
  void set##UNAME##Points(bool b) { \
    if (b != LNAME##PointData_.isVisible()) { \
      LNAME##PointData_.setVisible(b); LNAME##PointDataInvalidate(is##UNAME##ReloadObj()); } \
  } \
\
  const CQChartsSymbol &LNAME##SymbolType() const { return LNAME##PointData_.type(); } \
  void set##UNAME##SymbolType(const CQChartsSymbol &t) { \
    if (t != LNAME##PointData_.type()) { \
      LNAME##PointData_.setType(t); LNAME##PointDataInvalidate(); } \
  } \
\
  const CQChartsLength &LNAME##SymbolSize() const { return LNAME##PointData_.size(); } \
  void set##UNAME##SymbolSize(const CQChartsLength &s) { \
    if (s != LNAME##PointData_.size()) { \
      LNAME##PointData_.setSize(s); LNAME##PointDataInvalidate(); } \
  } \
\
  bool is##UNAME##SymbolStroked() const { return LNAME##PointData_.stroke().isVisible(); } \
  void set##UNAME##SymbolStroked(bool b) { \
    if (b != LNAME##PointData_.stroke().isVisible()) { \
      LNAME##PointData_.stroke().setVisible(b); LNAME##PointDataInvalidate(); } \
  } \
\
  const CQChartsColor &LNAME##SymbolStrokeColor() const { \
    return LNAME##PointData_.stroke().color(); } \
  void set##UNAME##SymbolStrokeColor(const CQChartsColor &c) { \
    if (c != LNAME##PointData_.stroke().color()) { \
      LNAME##PointData_.stroke().setColor(c); LNAME##PointDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##SymbolStrokeColor(const CQChartsUtil::ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##PointDataObj_). \
      interpColor(LNAME##SymbolStrokeColor(), ind); \
  } \
\
  const CQChartsAlpha &LNAME##SymbolStrokeAlpha() const { \
    return LNAME##PointData_.stroke().alpha(); } \
  void set##UNAME##SymbolStrokeAlpha(const CQChartsAlpha &a) { \
    if (a != LNAME##PointData_.stroke().alpha()) { \
      LNAME##PointData_.stroke().setAlpha(a); LNAME##PointDataInvalidate(); } \
  } \
\
  const CQChartsLength &LNAME##SymbolStrokeWidth() const { \
    return LNAME##PointData_.stroke().width(); } \
  void set##UNAME##SymbolStrokeWidth(const CQChartsLength &l) { \
    if (l != LNAME##PointData_.stroke().width()) { \
      LNAME##PointData_.stroke().setWidth(l); LNAME##PointDataInvalidate(); } \
  } \
\
  const CQChartsLineDash &LNAME##SymbolStrokeDash() const { \
    return LNAME##PointData_.stroke().dash(); } \
  void set##UNAME##SymbolStrokeDash(const CQChartsLineDash &d) { \
    if (d != LNAME##PointData_.stroke().dash()) { \
      LNAME##PointData_.stroke().setDash(d); LNAME##PointDataInvalidate(); } \
  } \
\
  bool is##UNAME##SymbolFilled() const { return LNAME##PointData_.fill().isVisible(); } \
  void set##UNAME##SymbolFilled(bool b) { \
    if (b != LNAME##PointData_.fill().isVisible()) { \
      LNAME##PointData_.fill().setVisible(b); \
      LNAME##PointDataInvalidate(is##UNAME##ReloadObj()); } \
  } \
\
  const CQChartsColor &LNAME##SymbolFillColor() const { \
    return LNAME##PointData_.fill().color(); } \
  void set##UNAME##SymbolFillColor(const CQChartsColor &c) { \
    if (c != LNAME##PointData_.fill().color()) { \
      LNAME##PointData_.fill().setColor(c); LNAME##PointDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##SymbolFillColor(const CQChartsUtil::ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##PointDataObj_).interpColor(LNAME##SymbolFillColor(), ind); \
  } \
\
  const CQChartsAlpha &LNAME##SymbolFillAlpha() const { \
    return LNAME##PointData_.fill().alpha(); } \
  void set##UNAME##SymbolFillAlpha(const CQChartsAlpha &a) { \
    if (a != LNAME##PointData_.fill().alpha()) { \
      LNAME##PointData_.fill().setAlpha(a); LNAME##PointDataInvalidate(); } \
  } \
\
  const CQChartsFillPattern &LNAME##SymbolFillPattern() const { \
    return LNAME##PointData_.fill().pattern(); } \
  void set##UNAME##SymbolFillPattern(const CQChartsFillPattern &p) { \
    if (p != LNAME##PointData_.fill().pattern()) { \
      LNAME##PointData_.fill().setPattern(p); LNAME##PointDataInvalidate(); } \
  } \
\
  void set##UNAME##SymbolPenBrush(CQChartsPenBrush &penBrush, \
                                  const CQChartsUtil::ColorInd &ind) const { \
    LNAME##PointDataObj_->setPenBrush(penBrush, \
      CQChartsPenData(is##UNAME##SymbolStroked(), interp##UNAME##SymbolStrokeColor(ind), \
                      LNAME##SymbolStrokeAlpha(), LNAME##SymbolStrokeWidth(), \
                      LNAME##SymbolStrokeDash()), \
      CQChartsBrushData(is##UNAME##SymbolFilled(), interp##UNAME##SymbolFillColor(ind), \
                        LNAME##SymbolFillAlpha(), LNAME##SymbolFillPattern())); \
  } \
\
  const CQChartsSymbolData &LNAME##SymbolData() const { return LNAME##PointData_; } \
\
  void set##UNAME##SymbolData(const CQChartsSymbolData &data) { \
    LNAME##PointData_ = data; LNAME##PointDataInvalidate(is##UNAME##ReloadObj()); \
  } \
\
 private: \
  void LNAME##PointDataInvalidate(bool reload=false) { \
    CQChartsInvalidator(LNAME##PointDataObj_).invalidate(reload); \
  } \
\
 private: \
  OBJ* LNAME##PointDataObj_ { nullptr }; \
\
 protected: \
  CQChartsSymbolData LNAME##PointData_; \
  bool               LNAME##ReloadObj_ { true }; \
};

//------

#define CQCHARTS_NAMED_FILL_DATA_PROPERTIES(UNAME, LNAME) \
Q_PROPERTY(CQChartsFillData LNAME##FillData READ LNAME##FillData WRITE set##UNAME##FillData) \
\
Q_PROPERTY(bool                LNAME##Filled \
           READ is##UNAME##Filled  WRITE set##UNAME##Filled     ) \
Q_PROPERTY(CQChartsColor       LNAME##FillColor \
           READ LNAME##FillColor   WRITE set##UNAME##FillColor  ) \
Q_PROPERTY(CQChartsAlpha       LNAME##FillAlpha \
           READ LNAME##FillAlpha   WRITE set##UNAME##FillAlpha  ) \
Q_PROPERTY(CQChartsFillPattern LNAME##FillPattern \
           READ LNAME##FillPattern WRITE set##UNAME##FillPattern)

/*!
 * \brief Object named fill data
 * \ingroup Charts
 */
#define CQCHARTS_NAMED_FILL_DATA(UNAME, LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##FillData { \
 public: \
  CQChartsObj##UNAME##FillData(OBJ *obj) : \
   LNAME##FillDataObj_(obj) { \
  } \
\
  bool is##UNAME##ReloadObj() const { return LNAME##ReloadObj_; } \
  void set##UNAME##ReloadObj(bool b) { LNAME##ReloadObj_ = b; } \
\
  bool is##UNAME##Filled() const { return LNAME##FillData_.isVisible(); } \
  void set##UNAME##Filled(bool b) { \
    if (b != LNAME##FillData_.isVisible()) { \
      LNAME##FillData_.setVisible(b); LNAME##FillDataInvalidate(is##UNAME##ReloadObj()); } \
  } \
\
  const CQChartsColor &LNAME##FillColor() const { return LNAME##FillData_.color(); } \
  void set##UNAME##FillColor(const CQChartsColor &c) { \
    if (c != LNAME##FillData_.color()) { \
      LNAME##FillData_.setColor(c); LNAME##FillDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##FillColor(const CQChartsUtil::ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##FillDataObj_).interpColor(LNAME##FillColor(), ind); \
  } \
\
  const CQChartsAlpha &LNAME##FillAlpha() const { return LNAME##FillData_.alpha(); } \
  void set##UNAME##FillAlpha(const CQChartsAlpha &a) { \
    if (a != LNAME##FillData_.alpha()) { \
      LNAME##FillData_.setAlpha(a); LNAME##FillDataInvalidate(); } \
  } \
\
  const CQChartsFillPattern &LNAME##FillPattern() const { return LNAME##FillData_.pattern(); } \
  void set##UNAME##FillPattern(const CQChartsFillPattern &p) { \
    if (p != LNAME##FillData_.pattern()) { \
      LNAME##FillData_.setPattern(p); LNAME##FillDataInvalidate(); } \
  } \
\
  const CQChartsFillData &LNAME##FillData() const { return LNAME##FillData_; } \
\
  void set##UNAME##FillData(const CQChartsFillData &data) { \
    LNAME##FillData_ = data; LNAME##FillDataInvalidate(is##UNAME##ReloadObj()); \
  } \
\
 private: \
  void LNAME##FillDataInvalidate(bool reload=false) { \
    CQChartsInvalidator(LNAME##FillDataObj_).invalidate(reload); \
  } \
\
 private: \
  OBJ* LNAME##FillDataObj_ { nullptr }; \
\
 protected: \
  CQChartsFillData LNAME##FillData_; \
  bool             LNAME##ReloadObj_ { true }; \
};

//------

#define CQCHARTS_TEXT_DATA_PROPERTIES \
Q_PROPERTY(CQChartsTextData textData READ textData WRITE setTextData) \
\
Q_PROPERTY(bool              textVisible       READ isTextVisible     WRITE setTextVisible      ) \
Q_PROPERTY(CQChartsColor     textColor         READ textColor         WRITE setTextColor        ) \
Q_PROPERTY(CQChartsAlpha     textAlpha         READ textAlpha         WRITE setTextAlpha        ) \
Q_PROPERTY(CQChartsFont      textFont          READ textFont          WRITE setTextFont         ) \
Q_PROPERTY(CQChartsAngle     textAngle         READ textAngle         WRITE setTextAngle        ) \
Q_PROPERTY(bool              textContrast      READ isTextContrast    WRITE setTextContrast     ) \
Q_PROPERTY(CQChartsAlpha     textContrastAlpha READ textContrastAlpha WRITE setTextContrastAlpha) \
Q_PROPERTY(Qt::Alignment     textAlign         READ textAlign         WRITE setTextAlign        ) \
Q_PROPERTY(bool              textFormatted     READ isTextFormatted   WRITE setTextFormatted    ) \
Q_PROPERTY(bool              textScaled        READ isTextScaled      WRITE setTextScaled       ) \
Q_PROPERTY(bool              textHtml          READ isTextHtml        WRITE setTextHtml         ) \
Q_PROPERTY(CQChartsLength    textClipLength    READ textClipLength    WRITE setTextClipLength   ) \
Q_PROPERTY(Qt::TextElideMode textClipElide     READ textClipElide     WRITE setTextClipElide    )

/*!
 * \brief Object text data
 * \ingroup Charts
 */
template<class OBJ>
class CQChartsObjTextData {
 public:
  CQChartsObjTextData(OBJ *obj) :
   textDataObj_(obj) {
  }

  bool isReloadObj() const { return reloadObj_; }
  void setReloadObj(bool b) { reloadObj_ = b; }

  bool isTextVisible() const { return textData_.isVisible(); }
  void setTextVisible(bool b) {
    if (b != textData_.isVisible()) {
      textData_.setVisible(b); textDataInvalidate(isReloadObj()); }
  }

  const CQChartsColor &textColor() const { return textData_.color(); }
  void setTextColor(const CQChartsColor &c) {
    if (c != textData_.color()) {
      textData_.setColor(c); textDataInvalidate(); }
  }

  const CQChartsAlpha& textAlpha() const { return textData_.alpha(); }
  void setTextAlpha(const CQChartsAlpha &a) {
    if (a != textData_.alpha()) {
      textData_.setAlpha(a); textDataInvalidate(); }
  }

  QColor interpTextColor(const CQChartsUtil::ColorInd &ind) const {
    return CQChartsInterpolator(textDataObj_).interpColor(textColor(), ind);
  }

  const CQChartsFont &textFont() const { return textData_.font(); }
  void setTextFont(const CQChartsFont &f) {
    if (f != textData_.font()) {
      textData_.setFont(f); textDataInvalidate(); }
  }

  const CQChartsAngle &textAngle() const { return textData_.angle(); }
  void setTextAngle(const CQChartsAngle &a) {
    if (a != textData_.angle()) {
      textData_.setAngle(a); textDataInvalidate(); }
  }

  bool isTextContrast() const { return textData_.isContrast(); }
  void setTextContrast(bool b) {
    if (b != textData_.isContrast()) {
      textData_.setContrast(b); textDataInvalidate(); }
  }

  const CQChartsAlpha& textContrastAlpha() const { return textData_.contrastAlpha(); }
  void setTextContrastAlpha(const CQChartsAlpha &a) {
    if (a != textData_.contrastAlpha()) {
      textData_.setContrastAlpha(a); textDataInvalidate(); }
  }

  const Qt::Alignment &textAlign() const { return textData_.align(); }
  void setTextAlign(const Qt::Alignment &a) {
    if (a != textData_.align()) {
      textData_.setAlign(a); textDataInvalidate(); }
  }

  bool isTextFormatted() const { return textData_.isFormatted(); }
  void setTextFormatted(bool b) {
    if (b != textData_.isFormatted()) {
      textData_.setFormatted(b); textDataInvalidate(); }
  }

  bool isTextScaled() const { return textData_.isScaled(); }
  void setTextScaled(bool b) {
    if (b != textData_.isScaled()) {
      textData_.setScaled(b); textDataInvalidate(); }
  }

  bool isTextHtml() const { return textData_.isHtml(); }
  void setTextHtml(bool b) {
    if (b != textData_.isHtml()) {
      textData_.setHtml(b); textDataInvalidate(); }
  }

  const CQChartsLength &textClipLength() const { return textData_.clipLength(); }
  void setTextClipLength(const CQChartsLength &l) {
    if (l != textData_.clipLength()) {
      textData_.setClipLength(l); textDataInvalidate(); }
  }

  const Qt::TextElideMode &textClipElide() const { return textData_.clipElide(); }
  void setTextClipElide(const Qt::TextElideMode &l) {
    if (l != textData_.clipElide()) {
      textData_.setClipElide(l); textDataInvalidate(); }
  }

  //---

  const CQChartsTextData &textData() const { return textData_; }

  void setTextData(const CQChartsTextData &data) {
    textData_ = data; textDataInvalidate();
  };

 protected:
  virtual void textDataInvalidate(bool reload=false) {
    CQChartsInvalidator(textDataObj_).invalidate(reload);
  }

 private:
  OBJ* textDataObj_ { nullptr };

 protected:
  CQChartsTextData textData_;
  bool             reloadObj_ { true };
};

//------

#define CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(UNAME, LNAME) \
Q_PROPERTY(CQChartsTextData LNAME##TextData READ LNAME##TextData WRITE set##UNAME##TextData) \
\
Q_PROPERTY(bool           LNAME##TextVisible \
           READ is##UNAME##TextVisible    WRITE set##UNAME##TextVisible      ) \
Q_PROPERTY(CQChartsColor  LNAME##TextColor \
           READ LNAME##TextColor          WRITE set##UNAME##TextColor        ) \
Q_PROPERTY(CQChartsAlpha  LNAME##TextAlpha \
           READ LNAME##TextAlpha          WRITE set##UNAME##TextAlpha        ) \
Q_PROPERTY(CQChartsFont   LNAME##TextFont \
           READ LNAME##TextFont           WRITE set##UNAME##TextFont         ) \
Q_PROPERTY(CQChartsAngle  LNAME##TextAngle \
           READ LNAME##TextAngle          WRITE set##UNAME##TextAngle        ) \
Q_PROPERTY(bool           LNAME##TextContrast \
           READ is##UNAME##TextContrast   WRITE set##UNAME##TextContrast     ) \
Q_PROPERTY(CQChartsAlpha  LNAME##TextContrastAlpha \
           READ LNAME##TextContrastAlpha  WRITE set##UNAME##TextContrastAlpha) \
Q_PROPERTY(Qt::Alignment  LNAME##TextAlign \
           READ LNAME##TextAlign          WRITE set##UNAME##TextAlign        ) \
Q_PROPERTY(bool           LNAME##TextFormatted \
           READ is##UNAME##TextFormatted  WRITE set##UNAME##TextFormatted    ) \
Q_PROPERTY(bool           LNAME##TextScaled \
           READ is##UNAME##TextScaled     WRITE set##UNAME##TextScaled       ) \
Q_PROPERTY(bool           LNAME##TextHtml \
           READ is##UNAME##TextHtml       WRITE set##UNAME##TextHtml         ) \
Q_PROPERTY(CQChartsLength LNAME##TextClipLength \
           READ LNAME##TextClipLength WRITE set##UNAME##TextClipLength       ) \
Q_PROPERTY(Qt::TextElideMode LNAME##TextClipElide \
           READ LNAME##TextClipElide WRITE set##UNAME##TextClipElide         )

/*!
 * \brief Object named text data
 * \ingroup Charts
 */
#define CQCHARTS_NAMED_TEXT_DATA(UNAME, LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##TextData { \
 public: \
  CQChartsObj##UNAME##TextData(OBJ *obj) : \
   LNAME##TextDataObj_(obj) { \
  } \
\
  bool is##UNAME##ReloadObj() const { return LNAME##ReloadObj_; } \
  void set##UNAME##ReloadObj(bool b) { LNAME##ReloadObj_ = b; } \
\
  bool is##UNAME##TextVisible() const { return LNAME##TextData_.isVisible(); } \
  void set##UNAME##TextVisible(bool b) { \
    if (b != LNAME##TextData_.isVisible()) { \
      LNAME##TextData_.setVisible(b); LNAME##TextDataInvalidate(is##UNAME##ReloadObj()); } \
  } \
\
  const CQChartsColor &LNAME##TextColor() const { return LNAME##TextData_.color(); } \
  void set##UNAME##TextColor(const CQChartsColor &c) { \
    if (c != LNAME##TextData_.color()) { \
      LNAME##TextData_.setColor(c); LNAME##TextDataInvalidate(); } \
  } \
\
  const CQChartsAlpha &LNAME##TextAlpha() const { return LNAME##TextData_.alpha(); } \
  void set##UNAME##TextAlpha(const CQChartsAlpha &a) { \
    if (a != LNAME##TextData_.alpha()) { \
      LNAME##TextData_.setAlpha(a); LNAME##TextDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##TextColor(const CQChartsUtil::ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##TextDataObj_).interpColor(LNAME##TextColor(), ind); \
  } \
\
  const CQChartsFont &LNAME##TextFont() const { return LNAME##TextData_.font(); } \
  void set##UNAME##TextFont(const CQChartsFont &f) { \
    if (f != LNAME##TextData_.font()) { \
      LNAME##TextData_.setFont(f); LNAME##TextDataInvalidate(); } \
  } \
\
  const CQChartsAngle &LNAME##TextAngle() const { return LNAME##TextData_.angle(); } \
  void set##UNAME##TextAngle(const CQChartsAngle &a) { \
    if (a != LNAME##TextData_.angle()) { \
      LNAME##TextData_.setAngle(a); LNAME##TextDataInvalidate(); } \
  } \
\
  bool is##UNAME##TextContrast() const { return LNAME##TextData_.isContrast(); } \
  void set##UNAME##TextContrast(bool b) { \
    if (b != LNAME##TextData_.isContrast()) { \
      LNAME##TextData_.setContrast(b); LNAME##TextDataInvalidate(); } \
  } \
\
  const CQChartsAlpha &LNAME##TextContrastAlpha() const { \
    return LNAME##TextData_.contrastAlpha(); } \
  void set##UNAME##TextContrastAlpha(const CQChartsAlpha &a) { \
    if (a != LNAME##TextData_.contrastAlpha()) { \
      LNAME##TextData_.setContrastAlpha(a); LNAME##TextDataInvalidate(); } \
  } \
\
  const Qt::Alignment &LNAME##TextAlign() const { return LNAME##TextData_.align(); } \
  void set##UNAME##TextAlign(const Qt::Alignment &a) { \
    if (a != LNAME##TextData_.align()) { \
      LNAME##TextData_.setAlign(a); LNAME##TextDataInvalidate(); } \
  } \
\
  bool is##UNAME##TextFormatted() const { return LNAME##TextData_.isFormatted(); } \
  void set##UNAME##TextFormatted(bool b) { \
    if (b != LNAME##TextData_.isFormatted()) { \
      LNAME##TextData_.setFormatted(b); LNAME##TextDataInvalidate(); } \
  } \
\
  bool is##UNAME##TextScaled() const { return LNAME##TextData_.isScaled(); } \
  void set##UNAME##TextScaled(bool b) { \
    if (b != LNAME##TextData_.isScaled()) { \
      LNAME##TextData_.setScaled(b); LNAME##TextDataInvalidate(); } \
  } \
\
  bool is##UNAME##TextHtml() const { return LNAME##TextData_.isHtml(); } \
  void set##UNAME##TextHtml(bool b) { \
    if (b != LNAME##TextData_.isHtml()) { \
      LNAME##TextData_.setHtml(b); LNAME##TextDataInvalidate(); } \
  } \
\
  const CQChartsLength &LNAME##TextClipLength() const { return LNAME##TextData_.clipLength(); } \
  void set##UNAME##TextClipLength(const CQChartsLength &l) { \
    if (l != LNAME##TextData_.clipLength()) { \
      LNAME##TextData_.setClipLength(l); LNAME##TextDataInvalidate(); } \
  } \
\
  const Qt::TextElideMode &LNAME##TextClipElide() const { return LNAME##TextData_.clipElide(); } \
  void set##UNAME##TextClipElide(const Qt::TextElideMode &l) { \
    if (l != LNAME##TextData_.clipElide()) { \
      LNAME##TextData_.setClipElide(l); LNAME##TextDataInvalidate(); } \
  } \
\
  const CQChartsTextData &LNAME##TextData() const { return LNAME##TextData_; } \
\
  void set##UNAME##TextData(const CQChartsTextData &data) { \
    LNAME##TextData_ = data; LNAME##TextDataInvalidate(); \
  } \
\
 private: \
  void LNAME##TextDataInvalidate(bool reload=false) { \
    CQChartsInvalidator(LNAME##TextDataObj_).invalidate(reload); \
  } \
\
 private: \
  OBJ* LNAME##TextDataObj_ { nullptr }; \
\
 protected: \
  CQChartsTextData LNAME##TextData_; \
  bool             LNAME##ReloadObj_ { true }; \
};

//------

#define CQCHARTS_STROKE_DATA_PROPERTIES \
Q_PROPERTY(CQChartsStrokeData strokeData READ strokeData WRITE setStrokeData) \
\
Q_PROPERTY(bool             stroked     READ isStroked   WRITE setStroked    ) \
Q_PROPERTY(CQChartsColor    strokeColor READ strokeColor WRITE setStrokeColor) \
Q_PROPERTY(CQChartsAlpha    strokeAlpha READ strokeAlpha WRITE setStrokeAlpha) \
Q_PROPERTY(CQChartsLength   strokeWidth READ strokeWidth WRITE setStrokeWidth) \
Q_PROPERTY(CQChartsLineDash strokeDash  READ strokeDash  WRITE setStrokeDash ) \
Q_PROPERTY(CQChartsLength   cornerSize  READ cornerSize  WRITE setCornerSize )

/*!
 * \brief Object stroke data
 * \ingroup Charts
 */
template<class OBJ>
class CQChartsObjStrokeData {
 public:
  CQChartsObjStrokeData(OBJ *obj) :
   strokeDataObj_(obj) {
  }

  //---

  bool isStroked() const { return strokeData_.isVisible(); }
  void setStroked(bool b) {
    if (b != strokeData_.isVisible()) {
      strokeData_.setVisible(b); strokeDataInvalidate(); }
  }

  const CQChartsColor &strokeColor() const { return strokeData_.color(); }
  void setStrokeColor(const CQChartsColor &c) {
    if (c != strokeData_.color()) {
      strokeData_.setColor(c); strokeDataInvalidate(); }
  }

  const CQChartsAlpha &strokeAlpha() const { return strokeData_.alpha(); }
  void setStrokeAlpha(const CQChartsAlpha &a) {
    if (a != strokeData_.alpha()) {
      strokeData_.setAlpha(a); strokeDataInvalidate(); }
  }

  const CQChartsLength &strokeWidth() const { return strokeData_.width(); }
  void setStrokeWidth(const CQChartsLength &l) {
    if (l != strokeData_.width()) {
      strokeData_.setWidth(l); strokeDataInvalidate(); }
  }

  const CQChartsLineDash &strokeDash() const { return strokeData_.dash(); }
  void setStrokeDash(const CQChartsLineDash &d) {
    if (d != strokeData_.dash()) {
      strokeData_.setDash(d); strokeDataInvalidate(); }
  }

  const CQChartsLength &cornerSize() const { return strokeData_.cornerSize(); }
  void setCornerSize(const CQChartsLength &l) {
    if (l != strokeData_.cornerSize()) {
      strokeData_.setCornerSize(l); strokeDataInvalidate(); }
  }

  QColor interpStrokeColor(const CQChartsUtil::ColorInd &ind) const {
    if (strokeDataObj_)
      return CQChartsInterpolator(strokeDataObj_).interpColor(strokeColor(), ind);
    else
      return strokeColor().color();
  }

  //---

  const CQChartsStrokeData &strokeData() const { return strokeData_; }

  void setStrokeData(const CQChartsStrokeData &data) {
    strokeData_ = data; strokeDataInvalidate();
  };

  //---

 private:
  void strokeDataInvalidate(bool reload=false) {
    CQChartsInvalidator(strokeDataObj_).invalidate(reload);
  }

 private:
  OBJ* strokeDataObj_ { nullptr };

 protected:
  CQChartsStrokeData strokeData_;
};

//------

#define CQCHARTS_SHAPE_DATA_PROPERTIES \
Q_PROPERTY(CQChartsShapeData shapeData READ shapeData WRITE setShapeData) \
\
Q_PROPERTY(bool             stroked     READ isStroked   WRITE setStroked    ) \
Q_PROPERTY(CQChartsColor    strokeColor READ strokeColor WRITE setStrokeColor) \
Q_PROPERTY(CQChartsAlpha    strokeAlpha READ strokeAlpha WRITE setStrokeAlpha) \
Q_PROPERTY(CQChartsLength   strokeWidth READ strokeWidth WRITE setStrokeWidth) \
Q_PROPERTY(CQChartsLineDash strokeDash  READ strokeDash  WRITE setStrokeDash ) \
Q_PROPERTY(CQChartsLength   cornerSize  READ cornerSize  WRITE setCornerSize ) \
\
Q_PROPERTY(bool                filled      READ isFilled    WRITE setFilled     ) \
Q_PROPERTY(CQChartsColor       fillColor   READ fillColor   WRITE setFillColor  ) \
Q_PROPERTY(CQChartsAlpha       fillAlpha   READ fillAlpha   WRITE setFillAlpha  ) \
Q_PROPERTY(CQChartsFillPattern fillPattern READ fillPattern WRITE setFillPattern)

/*!
 * \brief Object shape data
 * \ingroup Charts
 */
template<class OBJ>
class CQChartsObjShapeData {
 public:
  CQChartsObjShapeData(OBJ *obj) :
   shapeDataObj_(obj) {
  }

  //---

  bool isStroked() const { return shapeData_.stroke().isVisible(); }
  void setStroked(bool b) {
    if (b != shapeData_.stroke().isVisible()) {
      shapeData_.stroke().setVisible(b); shapeDataInvalidate(); }
  }

  const CQChartsColor &strokeColor() const { return shapeData_.stroke().color(); }
  void setStrokeColor(const CQChartsColor &c) {
    if (c != shapeData_.stroke().color()) {
      shapeData_.stroke().setColor(c); shapeDataInvalidate(); }
  }

  const CQChartsAlpha &strokeAlpha() const { return shapeData_.stroke().alpha(); }
  void setStrokeAlpha(const CQChartsAlpha &a) {
    if (a != shapeData_.stroke().alpha()) {
      shapeData_.stroke().setAlpha(a); shapeDataInvalidate(); }
  }

  const CQChartsLength &strokeWidth() const { return shapeData_.stroke().width(); }
  void setStrokeWidth(const CQChartsLength &l) {
    if (l != shapeData_.stroke().width()) {
      shapeData_.stroke().setWidth(l); shapeDataInvalidate(); }
  }

  const CQChartsLineDash &strokeDash() const { return shapeData_.stroke().dash(); }
  void setStrokeDash(const CQChartsLineDash &d) {
    if (d != shapeData_.stroke().dash()) {
      shapeData_.stroke().setDash(d); shapeDataInvalidate(); }
  }

  const CQChartsLength &cornerSize() const { return shapeData_.stroke().cornerSize(); }
  void setCornerSize(const CQChartsLength &l) {
    if (l != shapeData_.stroke().cornerSize()) {
      shapeData_.stroke().setCornerSize(l); shapeDataInvalidate(); }
  }

  QColor interpStrokeColor(const CQChartsUtil::ColorInd &ind) const {
    if (shapeDataObj_)
      return CQChartsInterpolator(shapeDataObj_).interpColor(strokeColor(), ind);
    else
      return strokeColor().color();
  }

  //---

  bool isFilled() const { return shapeData_.fill().isVisible(); }
  void setFilled(bool b) {
    if (b != shapeData_.fill().isVisible()) {
      shapeData_.fill().setVisible(b); shapeDataInvalidate(); }
  }

  const CQChartsColor &fillColor() const { return shapeData_.fill().color(); }
  void setFillColor(const CQChartsColor &c) {
    if (c != shapeData_.fill().color()) {
      shapeData_.fill().setColor(c); shapeDataInvalidate(); }
  }

  const CQChartsAlpha &fillAlpha() const { return shapeData_.fill().alpha(); }
  void setFillAlpha(const CQChartsAlpha &a) {
    if (a != shapeData_.fill().alpha()) {
      shapeData_.fill().setAlpha(a); shapeDataInvalidate(); }
  }

  const CQChartsFillPattern &fillPattern() const { return shapeData_.fill().pattern(); }
  void setFillPattern(const CQChartsFillPattern &p) {
    if (p != shapeData_.fill().pattern()) {
      shapeData_.fill().setPattern(p); shapeDataInvalidate(); }
  }

  QColor interpFillColor(const CQChartsUtil::ColorInd &ind) const {
    if (shapeDataObj_)
      return CQChartsInterpolator(shapeDataObj_).interpColor(fillColor(), ind);
    else
      return fillColor().color();
  }

  //---

  const CQChartsShapeData &shapeData() const { return shapeData_; }

  void setShapeData(const CQChartsShapeData &data) {
    shapeData_ = data; shapeDataInvalidate();
  };

 private:
  void shapeDataInvalidate(bool reload=false) {
    CQChartsInvalidator(shapeDataObj_).invalidate(reload);
  }

 private:
  OBJ* shapeDataObj_ { nullptr };

 protected:
  CQChartsShapeData shapeData_;
};

//------

#define CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(UNAME, LNAME) \
Q_PROPERTY(CQChartsShapeData LNAME##ShapeData \
           READ LNAME##ShapeData WRITE set##UNAME##ShapeData) \
\
Q_PROPERTY(bool             LNAME##Stroked \
           READ is##UNAME##Stroked WRITE set##UNAME##Stroked    ) \
Q_PROPERTY(CQChartsColor    LNAME##StrokeColor \
           READ LNAME##StrokeColor WRITE set##UNAME##StrokeColor) \
Q_PROPERTY(CQChartsAlpha    LNAME##StrokeAlpha \
           READ LNAME##StrokeAlpha WRITE set##UNAME##StrokeAlpha) \
Q_PROPERTY(CQChartsLength   LNAME##StrokeWidth \
           READ LNAME##StrokeWidth WRITE set##UNAME##StrokeWidth) \
Q_PROPERTY(CQChartsLineDash LNAME##StrokeDash \
           READ LNAME##StrokeDash  WRITE set##UNAME##StrokeDash ) \
Q_PROPERTY(CQChartsLength   LNAME##CornerSize \
           READ LNAME##CornerSize  WRITE set##UNAME##CornerSize ) \
\
Q_PROPERTY(bool                LNAME##Filled \
           READ is##UNAME##Filled  WRITE set##UNAME##Filled      ) \
Q_PROPERTY(CQChartsColor       LNAME##FillColor \
           READ LNAME##FillColor   WRITE set##UNAME##FillColor   ) \
Q_PROPERTY(CQChartsAlpha       LNAME##FillAlpha \
           READ LNAME##FillAlpha   WRITE set##UNAME##FillAlpha   ) \
Q_PROPERTY(CQChartsFillPattern LNAME##FillPattern \
           READ LNAME##FillPattern WRITE set##UNAME##FillPattern )

/*!
 * \brief Object named shape data
 * \ingroup Charts
 */
#define CQCHARTS_NAMED_SHAPE_DATA(UNAME, LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##ShapeData { \
 public: \
  CQChartsObj##UNAME##ShapeData(OBJ *obj) : \
   LNAME##ShapeDataObj_(obj) { \
  } \
\
  bool is##UNAME##ReloadObj() const { return LNAME##ReloadObj_; } \
  void set##UNAME##ReloadObj(bool b) { LNAME##ReloadObj_ = b; } \
\
  bool is##UNAME##Stroked() const { return LNAME##ShapeData_.stroke().isVisible(); } \
  void set##UNAME##Stroked(bool b) { \
    if (b != LNAME##ShapeData_.stroke().isVisible()) { \
      LNAME##ShapeData_.stroke().setVisible(b); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const CQChartsColor &LNAME##StrokeColor() const { return LNAME##ShapeData_.stroke().color(); } \
  void set##UNAME##StrokeColor(const CQChartsColor &c) { \
    if (c != LNAME##ShapeData_.stroke().color()) { \
      LNAME##ShapeData_.stroke().setColor(c); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const CQChartsAlpha &LNAME##StrokeAlpha() const { return LNAME##ShapeData_.stroke().alpha(); } \
  void set##UNAME##StrokeAlpha(const CQChartsAlpha &a) { \
    if (a != LNAME##ShapeData_.stroke().alpha()) { \
      LNAME##ShapeData_.stroke().setAlpha(a); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const CQChartsLength &LNAME##StrokeWidth() const { return LNAME##ShapeData_.stroke().width(); } \
  void set##UNAME##StrokeWidth(const CQChartsLength &l) { \
    if (l != LNAME##ShapeData_.stroke().width()) { \
      LNAME##ShapeData_.stroke().setWidth(l); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const CQChartsLineDash &LNAME##StrokeDash() const { return LNAME##ShapeData_.stroke().dash(); } \
  void set##UNAME##StrokeDash(const CQChartsLineDash &d) { \
    if (d != LNAME##ShapeData_.stroke().dash()) { \
      LNAME##ShapeData_.stroke().setDash(d); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const CQChartsLength &LNAME##CornerSize() const { \
    return LNAME##ShapeData_.stroke().cornerSize(); } \
  void set##UNAME##CornerSize(const CQChartsLength &l) { \
    if (l != LNAME##ShapeData_.stroke().cornerSize()) { \
      LNAME##ShapeData_.stroke().setCornerSize(l); LNAME##ShapeDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##StrokeColor(const CQChartsUtil::ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##ShapeDataObj_).interpColor(LNAME##StrokeColor(), ind); \
  } \
\
  bool is##UNAME##Filled() const { return LNAME##ShapeData_.fill().isVisible(); } \
  void set##UNAME##Filled(bool b) { \
    if (b != LNAME##ShapeData_.fill().isVisible()) { \
      LNAME##ShapeData_.fill().setVisible(b); \
      LNAME##ShapeDataInvalidate(is##UNAME##ReloadObj()); } \
  } \
\
  const CQChartsColor &LNAME##FillColor() const { return LNAME##ShapeData_.fill().color(); } \
  void set##UNAME##FillColor(const CQChartsColor &c) { \
    if (c != LNAME##ShapeData_.fill().color()) { \
      LNAME##ShapeData_.fill().setColor(c); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const CQChartsAlpha &LNAME##FillAlpha() const { return LNAME##ShapeData_.fill().alpha(); } \
  void set##UNAME##FillAlpha(const CQChartsAlpha &a) { \
    if (a != LNAME##ShapeData_.fill().alpha()) { \
      LNAME##ShapeData_.fill().setAlpha(a); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const CQChartsFillPattern &LNAME##FillPattern() const { \
    return LNAME##ShapeData_.fill().pattern(); } \
  void set##UNAME##FillPattern(const CQChartsFillPattern &p) { \
    if (p != LNAME##ShapeData_.fill().pattern()) { \
      LNAME##ShapeData_.fill().setPattern(p); LNAME##ShapeDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##FillColor(const CQChartsUtil::ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##ShapeDataObj_).interpColor(LNAME##FillColor(), ind); \
  } \
\
  const CQChartsShapeData &LNAME##ShapeData() const { return LNAME##ShapeData_; } \
\
  void set##UNAME##ShapeData(const CQChartsShapeData &data) { \
    LNAME##ShapeData_ = data; LNAME##ShapeDataInvalidate(); \
  } \
\
  CQChartsPenData LNAME##PenData(const CQChartsUtil::ColorInd &colorInd) const { \
    return CQChartsPenData(is##UNAME##Stroked(), interp##UNAME##StrokeColor(colorInd), \
                           LNAME##StrokeAlpha(), LNAME##StrokeWidth(), LNAME##StrokeDash()); \
  } \
\
  CQChartsBrushData LNAME##BrushData(const CQChartsUtil::ColorInd &colorInd) const { \
    return CQChartsBrushData(is##UNAME##Filled(), interp##UNAME##FillColor(colorInd), \
                             LNAME##FillAlpha(), LNAME##FillPattern()); \
  } \
\
 private: \
  void LNAME##ShapeDataInvalidate(bool reload=false) { \
    CQChartsInvalidator(LNAME##ShapeDataObj_).invalidate(reload); \
  } \
\
 private: \
  OBJ* LNAME##ShapeDataObj_ { nullptr }; \
\
 protected: \
  CQChartsShapeData LNAME##ShapeData_; \
  bool              LNAME##ReloadObj_ { true }; \
};

//------

#define CQCHARTS_BOX_DATA_PROPERTIES \
Q_PROPERTY(CQChartsBoxData boxData READ boxData WRITE setBoxData) \
\
Q_PROPERTY(bool           visible READ isVisible WRITE setVisible) \
Q_PROPERTY(CQChartsMargin margin  READ margin    WRITE setMargin ) \
Q_PROPERTY(CQChartsMargin padding READ padding   WRITE setPadding) \
\
Q_PROPERTY(bool             stroked     READ isStroked   WRITE setStroked    ) \
Q_PROPERTY(CQChartsColor    strokeColor READ strokeColor WRITE setStrokeColor) \
Q_PROPERTY(CQChartsAlpha    strokeAlpha READ strokeAlpha WRITE setStrokeAlpha) \
Q_PROPERTY(CQChartsLength   strokeWidth READ strokeWidth WRITE setStrokeWidth) \
Q_PROPERTY(CQChartsLineDash strokeDash  READ strokeDash  WRITE setStrokeDash ) \
Q_PROPERTY(CQChartsLength   cornerSize  READ cornerSize  WRITE setCornerSize ) \
\
Q_PROPERTY(bool                filled      READ isFilled    WRITE setFilled     ) \
Q_PROPERTY(CQChartsColor       fillColor   READ fillColor   WRITE setFillColor  ) \
Q_PROPERTY(CQChartsAlpha       fillAlpha   READ fillAlpha   WRITE setFillAlpha  ) \
Q_PROPERTY(CQChartsFillPattern fillPattern READ fillPattern WRITE setFillPattern) \
\
Q_PROPERTY(CQChartsSides borderSides READ borderSides WRITE setBorderSides)

/*!
 * \brief Object box data
 * \ingroup Charts
 */
template<class OBJ>
class CQChartsObjBoxData {
 public:
  CQChartsObjBoxData(OBJ *obj) :
   boxDataObj_(obj) {
  }

  //---

  const CQChartsMargin &margin() const { return boxData_.margin(); }
  void setMargin(const CQChartsMargin &m) { boxData_.setMargin(m); boxDataInvalidate(); }

  const CQChartsMargin &padding() const { return boxData_.padding(); }
  void setPadding(const CQChartsMargin &m) { boxData_.setPadding(m); boxDataInvalidate(); }

  //---

  const CQChartsSides &borderSides() const { return boxData_.borderSides(); }
  void setBorderSides(const CQChartsSides &s) { boxData_.setBorderSides(s); boxDataInvalidate(); }

  //---

  bool isStroked() const { return boxData_.shape().stroke().isVisible(); }
  void setStroked(bool b) {
    if (b != boxData_.shape().stroke().isVisible()) {
      boxData_.shape().stroke().setVisible(b); boxDataInvalidate(); }
  }

  const CQChartsColor &strokeColor() const { return boxData_.shape().stroke().color(); }
  void setStrokeColor(const CQChartsColor &c) {
    if (c != boxData_.shape().stroke().color()) {
      boxData_.shape().stroke().setColor(c); boxDataInvalidate(); }
  }

  const CQChartsAlpha &strokeAlpha() const { return boxData_.shape().stroke().alpha(); }
  void setStrokeAlpha(const CQChartsAlpha &a) {
    if (a != boxData_.shape().stroke().alpha()) {
      boxData_.shape().stroke().setAlpha(a); boxDataInvalidate(); }
  }

  const CQChartsLength &strokeWidth() const { return boxData_.shape().stroke().width(); }
  void setStrokeWidth(const CQChartsLength &l) {
    if (l != boxData_.shape().stroke().width()) {
      boxData_.shape().stroke().setWidth(l); boxDataInvalidate(); }
  }

  const CQChartsLineDash &strokeDash() const { return boxData_.shape().stroke().dash(); }
  void setStrokeDash(const CQChartsLineDash &d) {
    if (d != boxData_.shape().stroke().dash()) {
      boxData_.shape().stroke().setDash(d); boxDataInvalidate(); }
  }

  const CQChartsLength &cornerSize() const { return boxData_.shape().stroke().cornerSize(); }
  void setCornerSize(const CQChartsLength &l) {
    if (l != boxData_.shape().stroke().cornerSize()) {
      boxData_.shape().stroke().setCornerSize(l); boxDataInvalidate(); }
  }

  QColor interpStrokeColor(const CQChartsUtil::ColorInd &ind) const {
    if (boxDataObj_)
      return CQChartsInterpolator(boxDataObj_).interpColor(strokeColor(), ind);
    else
      return strokeColor().color();
  }

  //---

  bool isFilled() const { return boxData_.shape().fill().isVisible(); }
  void setFilled(bool b) {
    if (b != boxData_.shape().fill().isVisible()) {
      boxData_.shape().fill().setVisible(b); boxDataInvalidate(); }
  }

  const CQChartsColor &fillColor() const { return boxData_.shape().fill().color(); }
  void setFillColor(const CQChartsColor &c) {
    if (c != boxData_.shape().fill().color()) {
      boxData_.shape().fill().setColor(c); boxDataInvalidate(); }
  }

  const CQChartsAlpha &fillAlpha() const { return boxData_.shape().fill().alpha(); }
  void setFillAlpha(const CQChartsAlpha &a) {
    if (a != boxData_.shape().fill().alpha()) {
      boxData_.shape().fill().setAlpha(a); boxDataInvalidate(); }
  }

  const CQChartsFillPattern &fillPattern() const { return boxData_.shape().fill().pattern(); }
  void setFillPattern(const CQChartsFillPattern &p) {
    if (p != boxData_.shape().fill().pattern()) {
      boxData_.shape().fill().setPattern(p); boxDataInvalidate(); }
  }

  QColor interpFillColor(const CQChartsUtil::ColorInd &ind) const {
    if (boxDataObj_)
      return CQChartsInterpolator(boxDataObj_).interpColor(fillColor(), ind);
    else
      return fillColor().color();
  }

  //---

  const CQChartsBoxData &boxData() const { return boxData_; }

  void setBoxData(const CQChartsBoxData &data) {
    boxData_ = data; boxDataInvalidate();
  };

 private:
  void boxDataInvalidate(bool reload=false) {
    CQChartsInvalidator(boxDataObj_).invalidate(reload);
  }

 private:
  OBJ* boxDataObj_ { nullptr };

 protected:
  CQChartsBoxData boxData_;
};

//------

CQCHARTS_NAMED_FILL_DATA (Background, background)
CQCHARTS_NAMED_SHAPE_DATA(Bar, bar)
CQCHARTS_NAMED_TEXT_DATA (Header, header)
CQCHARTS_NAMED_SHAPE_DATA(BestFit, bestFit)
CQCHARTS_NAMED_LINE_DATA (Stats, stats)

CQCHARTS_NAMED_SHAPE_DATA(Cell, cell)
CQCHARTS_NAMED_TEXT_DATA(CellLabel, cellLabel)
CQCHARTS_NAMED_TEXT_DATA(XLabel, xLabel)
CQCHARTS_NAMED_TEXT_DATA(YLabel, yLabel)

#endif
