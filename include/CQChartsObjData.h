#ifndef CQChartsObjData_H
#define CQChartsObjData_H

#include <CQChartsData.h>
#include <CQChartsInvalidator.h>
#include <CQChartsUtil.h>

#define CQCHARTS_LINE_DATA_PROPERTIES \
Q_PROPERTY(CQChartsLineData lineData READ lineData WRITE setLineData) \
\
Q_PROPERTY(bool             lines      READ isLines    WRITE setLines     ) \
Q_PROPERTY(CQChartsColor    linesColor READ linesColor WRITE setLinesColor) \
Q_PROPERTY(double           linesAlpha READ linesAlpha WRITE setLinesAlpha) \
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

  bool isLines() const { return lineData_.isVisible(); }
  void setLines(bool b) {
    if (b != lineData_.isVisible()) {
      lineData_.setVisible(b); lineDataInvalidate(true); }
  }

  const CQChartsColor &linesColor() const { return lineData_.color(); }
  void setLinesColor(const CQChartsColor &c) {
    if (c != lineData_.color()) {
      lineData_.setColor(c); lineDataInvalidate(); }
  }

#if 0
  QColor interpLinesColor(int i, int n) const {
    return CQChartsInterpolator(lineDataObj_).interpColor(linesColor(), i, n);
  }

  QColor interpLinesColor(double r) const {
    return CQChartsInterpolator(lineDataObj_).interpColor(linesColor(), r);
  }
#endif

  QColor interpLinesColor(const CQChartsUtil::ColorInd &ind) const {
    return CQChartsInterpolator(lineDataObj_).interpColor(linesColor(), ind);
  }

  double linesAlpha() const { return lineData_.alpha(); }
  void setLinesAlpha(double a) {
    if (a != lineData_.alpha()) {
      lineData_.setAlpha(a); lineDataInvalidate(); }
  }

  const CQChartsLength &linesWidth() const { return lineData_.width(); }
  void setLinesWidth(const CQChartsLength &l) {
    if (l != lineData_.width()) {
      lineData_.setWidth(l); lineDataInvalidate(/*reload*/true); }
   }

  const CQChartsLineDash &linesDash() const { return lineData_.dash(); }
  void setLinesDash(const CQChartsLineDash &d) {
    if (d != lineData_.dash()) {
      lineData_.setDash(d); lineDataInvalidate(); }
  }

#if 0
  void setLineDataPen(QPen &pen, int i, int n) const {
    QColor lc = interpLinesColor(i, n);

    lineDataObj_->setPen(pen, isLines(), lc, linesAlpha(), linesWidth(), linesDash());
  }

  void setLineDataPen(QPen &pen, double r) const {
    QColor lc = interpLinesColor(r);

    lineDataObj_->setPen(pen, isLines(), lc, linesAlpha(), linesWidth(), linesDash());
  }
#endif

  void setLineDataPen(QPen &pen, const CQChartsUtil::ColorInd &ind) const {
    QColor lc = interpLinesColor(ind);

    lineDataObj_->setPen(pen, isLines(), lc, linesAlpha(), linesWidth(), linesDash());
  }

  //---

  const CQChartsLineData &lineData() const { return lineData_; }

  void setLineData(const CQChartsLineData &data) {
    lineData_ = data; lineDataInvalidate(/*reload*/true);
  };

 private:
  void lineDataInvalidate(bool reload=false) {
    CQChartsInvalidator(lineDataObj_).invalidate(reload);
  }

 private:
  OBJ* lineDataObj_ { nullptr };

 protected:
  CQChartsLineData lineData_;
};

//------

#define CQCHARTS_NAMED_LINE_DATA_PROPERTIES(UNAME,LNAME) \
Q_PROPERTY(CQChartsLineData LNAME##LineData READ LNAME##LineData WRITE set##UNAME##LineData) \
\
Q_PROPERTY(bool             LNAME##Lines      READ is##UNAME##Lines  WRITE set##UNAME##Lines     ) \
Q_PROPERTY(CQChartsColor    LNAME##LinesColor READ LNAME##LinesColor WRITE set##UNAME##LinesColor) \
Q_PROPERTY(double           LNAME##LinesAlpha READ LNAME##LinesAlpha WRITE set##UNAME##LinesAlpha) \
Q_PROPERTY(CQChartsLength   LNAME##LinesWidth READ LNAME##LinesWidth WRITE set##UNAME##LinesWidth) \
Q_PROPERTY(CQChartsLineDash LNAME##LinesDash  READ LNAME##LinesDash  WRITE set##UNAME##LinesDash )

/*!
 * \brief Object named line data
 * \ingroup Charts
 */
#define CQCHARTS_NAMED_LINE_DATA(UNAME,LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##LineData { \
 public: \
  CQChartsObj##UNAME##LineData(OBJ *obj) : \
   LNAME##LineDataObj_(obj) { \
  } \
\
  bool is##UNAME##Lines() const { return LNAME##LineData_.isVisible(); } \
  void set##UNAME##Lines(bool b) { \
    if (b != LNAME##LineData_.isVisible()) { \
      LNAME##LineData_.setVisible(b); LNAME##LineDataInvalidate(true); } \
  } \
\
  const CQChartsColor &LNAME##LinesColor() const { return LNAME##LineData_.color(); } \
  void set##UNAME##LinesColor(const CQChartsColor &c) { \
    if (c != LNAME##LineData_.color()) { \
      LNAME##LineData_.setColor(c); LNAME##LineDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##LinesColor(int i, int n) const { \
    return CQChartsInterpolator(LNAME##LineDataObj_).interpColor(LNAME##LinesColor(), i, n); \
  } \
\
  QColor interp##UNAME##LinesColor(double r) const { \
    return CQChartsInterpolator(LNAME##LineDataObj_).interpColor(LNAME##LinesColor(), r); \
  } \
\
  QColor interp##UNAME##LinesColor(const CQChartsUtil::ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##LineDataObj_).interpColor(LNAME##LinesColor(), ind); \
  } \
\
  double LNAME##LinesAlpha() const { return LNAME##LineData_.alpha(); } \
  void set##UNAME##LinesAlpha(double a) { \
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
  void set##UNAME##LineDataPen(QPen &pen, int i, int n) const { \
    QColor lc = interp##UNAME##LinesColor(i, n); \
\
    LNAME##LineDataObj_->setPen(pen, is##UNAME##Lines(), lc, LNAME##LinesAlpha(), \
                                LNAME##LinesWidth(), LNAME##LinesDash()); \
  } \
\
  void set##UNAME##LineDataPen(QPen &pen, double r) const { \
    QColor lc = interp##UNAME##LinesColor(r); \
\
    LNAME##LineDataObj_->setPen(pen, is##UNAME##Lines(), lc, LNAME##LinesAlpha(), \
                                LNAME##LinesWidth(), LNAME##LinesDash()); \
  } \
\
  void set##UNAME##LineDataPen(QPen &pen, const CQChartsUtil::ColorInd &ind) const { \
    QColor lc = interp##UNAME##LinesColor(ind); \
\
    LNAME##LineDataObj_->setPen(pen, is##UNAME##Lines(), lc, LNAME##LinesAlpha(), \
                                LNAME##LinesWidth(), LNAME##LinesDash()); \
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
Q_PROPERTY(double              symbolStrokeAlpha \
           READ symbolStrokeAlpha WRITE setSymbolStrokeAlpha) \
Q_PROPERTY(CQChartsLength      symbolStrokeWidth \
           READ symbolStrokeWidth WRITE setSymbolStrokeWidth) \
Q_PROPERTY(CQChartsLineDash    symbolStrokeDash \
           READ symbolStrokeDash  WRITE setSymbolStrokeDash ) \
Q_PROPERTY(bool                symbolFilled \
           READ isSymbolFilled    WRITE setSymbolFilled     ) \
Q_PROPERTY(CQChartsColor       symbolFillColor \
           READ symbolFillColor   WRITE setSymbolFillColor  ) \
Q_PROPERTY(double              symbolFillAlpha \
           READ symbolFillAlpha   WRITE setSymbolFillAlpha  ) \
Q_PROPERTY(CQChartsFillPattern symbolFillPattern \
           READ symbolFillPattern WRITE setSymbolFillPattern) \

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

  bool isPoints() const { return pointData_.isVisible(); }
  void setPoints(bool b) {
    if (b != pointData_.isVisible()) {
      pointData_.setVisible(b); pointDataInvalidate(true); }
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

#if 0
  QColor interpSymbolStrokeColor(int i, int n) const {
    return CQChartsInterpolator(pointDataObj_).interpColor(symbolStrokeColor(), i, n);
  }

  QColor interpSymbolStrokeColor(double r) const {
    return CQChartsInterpolator(pointDataObj_).interpColor(symbolStrokeColor(), r);
  }
#endif

  QColor interpSymbolStrokeColor(const CQChartsUtil::ColorInd &ind) const {
    return CQChartsInterpolator(pointDataObj_).interpColor(symbolStrokeColor(), ind);
  }

  double symbolStrokeAlpha() const { return pointData_.stroke().alpha(); }
  void setSymbolStrokeAlpha(double a) {
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

#if 0
  QColor interpSymbolFillColor(int i, int n) const {
    return CQChartsInterpolator(pointDataObj_).interpColor(symbolFillColor(), i, n);
  }

  QColor interpSymbolFillColor(double r) const {
    return CQChartsInterpolator(pointDataObj_).interpColor(symbolFillColor(), r);
  }
#endif

  QColor interpSymbolFillColor(const CQChartsUtil::ColorInd &ind) const {
    return CQChartsInterpolator(pointDataObj_).interpColor(symbolFillColor(), ind);
  }

  double symbolFillAlpha() const { return pointData_.fill().alpha(); }
  void setSymbolFillAlpha(double a) {
    if (a != pointData_.fill().alpha()) {
      pointData_.fill().setAlpha(a); pointDataInvalidate(); }
  }

  const CQChartsFillPattern &symbolFillPattern() const { return pointData_.fill().pattern(); }
  void setSymbolFillPattern(const CQChartsFillPattern &p) {
    if (p != pointData_.fill().pattern()) {
      pointData_.fill().setPattern(p); pointDataInvalidate(); }
  }

  //---

#if 0
  void setSymbolPenBrush(QPen &pen, QBrush &brush, int i, int n) const {
    pointDataObj_->setPenBrush(pen, brush,
      isSymbolStroked(), interpSymbolStrokeColor(i, n), symbolStrokeAlpha(),
      symbolStrokeWidth(), symbolStrokeDash(),
      isSymbolFilled(), interpSymbolFillColor(i, n), symbolFillAlpha(), symbolFillPattern());
  }

  void setSymbolPenBrush(QPen &pen, QBrush &brush, double r) const {
    pointDataObj_->setPenBrush(pen, brush,
      isSymbolStroked(), interpSymbolStrokeColor(r), symbolStrokeAlpha(),
      symbolStrokeWidth(), symbolStrokeDash(),
      isSymbolFilled(), interpSymbolFillColor(r), symbolFillAlpha(), symbolFillPattern());
  }
#endif

  void setSymbolPenBrush(QPen &pen, QBrush &brush, const CQChartsUtil::ColorInd &ind) const {
    pointDataObj_->setPenBrush(pen, brush,
      isSymbolStroked(), interpSymbolStrokeColor(ind), symbolStrokeAlpha(),
      symbolStrokeWidth(), symbolStrokeDash(),
      isSymbolFilled(), interpSymbolFillColor(ind), symbolFillAlpha(), symbolFillPattern());
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
};

//------

#define CQCHARTS_NAMED_POINT_DATA_PROPERTIES(UNAME,LNAME) \
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
Q_PROPERTY(double              LNAME##SymbolStrokeAlpha \
           READ LNAME##SymbolStrokeAlpha WRITE set##UNAME##SymbolStrokeAlpha) \
Q_PROPERTY(CQChartsLength      LNAME##SymbolStrokeWidth \
           READ LNAME##SymbolStrokeWidth WRITE set##UNAME##SymbolStrokeWidth) \
Q_PROPERTY(CQChartsLineDash    LNAME##SymbolStrokeDash \
           READ LNAME##SymbolStrokeDash  WRITE set##UNAME##SymbolStrokeDash ) \
Q_PROPERTY(bool                LNAME##SymbolFilled \
           READ is##UNAME##SymbolFilled  WRITE set##UNAME##SymbolFilled     ) \
Q_PROPERTY(CQChartsColor       LNAME##SymbolFillColor \
           READ LNAME##SymbolFillColor   WRITE set##UNAME##SymbolFillColor  ) \
Q_PROPERTY(double              LNAME##SymbolFillAlpha \
           READ LNAME##SymbolFillAlpha   WRITE set##UNAME##SymbolFillAlpha  ) \
Q_PROPERTY(CQChartsFillPattern LNAME##SymbolFillPattern \
           READ LNAME##SymbolFillPattern WRITE set##UNAME##SymbolFillPattern)

/*!
 * \brief Object named point data
 * \ingroup Charts
 */
#define CQCHARTS_NAMED_POINT_DATA(UNAME,LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##PointData { \
 public: \
  CQChartsObj##UNAME##PointData(OBJ *obj) : \
   LNAME##PointDataObj_(obj) { \
  } \
\
  bool is##UNAME##Points() const { return LNAME##PointData_.isVisible(); } \
  void set##UNAME##Points(bool b) { \
    if (b != LNAME##PointData_.isVisible()) { \
      LNAME##PointData_.setVisible(b); LNAME##PointDataInvalidate(true); } \
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
  QColor interp##UNAME##SymbolStrokeColor(int i, int n) const { \
    return CQChartsInterpolator(LNAME##PointDataObj_). \
      interpColor(LNAME##SymbolStrokeColor(), i, n); \
  } \
\
  QColor interp##UNAME##SymbolStrokeColor(double r) const { \
    return CQChartsInterpolator(LNAME##PointDataObj_). \
      interpColor(LNAME##SymbolStrokeColor(), r); \
  } \
\
  QColor interp##UNAME##SymbolStrokeColor(const CQChartsUtil::ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##PointDataObj_). \
      interpColor(LNAME##SymbolStrokeColor(), ind); \
  } \
\
  double LNAME##SymbolStrokeAlpha() const { return LNAME##PointData_.stroke().alpha(); } \
  void set##UNAME##SymbolStrokeAlpha(double a) { \
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
      LNAME##PointData_.fill().setVisible(b); LNAME##PointDataInvalidate(true); } \
  } \
\
  const CQChartsColor &LNAME##SymbolFillColor() const { return LNAME##PointData_.fill().color(); } \
  void set##UNAME##SymbolFillColor(const CQChartsColor &c) { \
    if (c != LNAME##PointData_.fill().color()) { \
      LNAME##PointData_.fill().setColor(c); LNAME##PointDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##SymbolFillColor(int i, int n) const { \
    return CQChartsInterpolator(LNAME##PointDataObj_).interpColor(LNAME##SymbolFillColor(), i, n); \
  } \
\
  QColor interp##UNAME##SymbolFillColor(double r) const { \
    return CQChartsInterpolator(LNAME##PointDataObj_).interpColor(LNAME##SymbolFillColor(), r); \
  } \
\
  QColor interp##UNAME##SymbolFillColor(const CQChartsUtil::ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##PointDataObj_).interpColor(LNAME##SymbolFillColor(), ind); \
  } \
\
  double LNAME##SymbolFillAlpha() const { return LNAME##PointData_.fill().alpha(); } \
  void set##UNAME##SymbolFillAlpha(double a) { \
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
  void set##UNAME##SymbolPenBrush(QPen &pen, QBrush &brush, int i, int n) const { \
    LNAME##PointDataObj_->setPenBrush(pen, brush, \
      is##UNAME##SymbolStroked(), interp##UNAME##SymbolStrokeColor(i, n), \
      LNAME##SymbolStrokeAlpha(), LNAME##SymbolStrokeWidth(), LNAME##SymbolStrokeDash(), \
      is##UNAME##SymbolFilled(), interp##UNAME##SymbolFillColor(i, n), \
      LNAME##SymbolFillAlpha(), LNAME##SymbolFillPattern()); \
  } \
\
  void set##UNAME##SymbolPenBrush(QPen &pen, QBrush &brush, double r) const { \
    LNAME##PointDataObj_->setPenBrush(pen, brush, \
      is##UNAME##SymbolStroked(), interp##UNAME##SymbolStrokeColor(r), \
      LNAME##SymbolStrokeAlpha(), LNAME##SymbolStrokeWidth(), LNAME##SymbolStrokeDash(), \
      is##UNAME##SymbolFilled(), interp##UNAME##SymbolFillColor(r), \
      LNAME##SymbolFillAlpha(), LNAME##SymbolFillPattern()); \
  } \
\
  void set##UNAME##SymbolPenBrush(QPen &pen, QBrush &brush, \
                                  const CQChartsUtil::ColorInd &ind) const { \
    LNAME##PointDataObj_->setPenBrush(pen, brush, \
      is##UNAME##SymbolStroked(), interp##UNAME##SymbolStrokeColor(ind), \
      LNAME##SymbolStrokeAlpha(), LNAME##SymbolStrokeWidth(), LNAME##SymbolStrokeDash(), \
      is##UNAME##SymbolFilled(), interp##UNAME##SymbolFillColor(ind), \
      LNAME##SymbolFillAlpha(), LNAME##SymbolFillPattern()); \
  } \
\
  const CQChartsSymbolData &LNAME##SymbolData() const { return LNAME##PointData_; } \
\
  void set##UNAME##SymbolData(const CQChartsSymbolData &data) { \
    LNAME##PointData_ = data; LNAME##PointDataInvalidate(true); \
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
};

//------

#define CQCHARTS_NAMED_FILL_DATA_PROPERTIES(UNAME,LNAME) \
Q_PROPERTY(CQChartsFillData LNAME##FillData READ LNAME##FillData WRITE set##UNAME##FillData) \
\
Q_PROPERTY(bool                LNAME##Filled \
           READ is##UNAME##Filled  WRITE set##UNAME##Filled     ) \
Q_PROPERTY(CQChartsColor       LNAME##FillColor \
           READ LNAME##FillColor   WRITE set##UNAME##FillColor  ) \
Q_PROPERTY(double              LNAME##FillAlpha \
           READ LNAME##FillAlpha   WRITE set##UNAME##FillAlpha  ) \
Q_PROPERTY(CQChartsFillPattern LNAME##FillPattern \
           READ LNAME##FillPattern WRITE set##UNAME##FillPattern)

/*!
 * \brief Object named fill data
 * \ingroup Charts
 */
#define CQCHARTS_NAMED_FILL_DATA(UNAME,LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##FillData { \
 public: \
  CQChartsObj##UNAME##FillData(OBJ *obj) : \
   LNAME##FillDataObj_(obj) { \
  } \
\
  bool is##UNAME##Filled() const { return LNAME##FillData_.isVisible(); } \
  void set##UNAME##Filled(bool b) { \
    if (b != LNAME##FillData_.isVisible()) { \
      LNAME##FillData_.setVisible(b); LNAME##FillDataInvalidate(true); } \
  } \
\
  const CQChartsColor &LNAME##FillColor() const { return LNAME##FillData_.color(); } \
  void set##UNAME##FillColor(const CQChartsColor &c) { \
    if (c != LNAME##FillData_.color()) { \
      LNAME##FillData_.setColor(c); LNAME##FillDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##FillColor(int i, int n) const { \
    return CQChartsInterpolator(LNAME##FillDataObj_).interpColor(LNAME##FillColor(), i, n); \
  } \
\
  QColor interp##UNAME##FillColor(double r) const { \
    return CQChartsInterpolator(LNAME##FillDataObj_).interpColor(LNAME##FillColor(), r); \
  } \
\
  QColor interp##UNAME##FillColor(const CQChartsUtil::ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##FillDataObj_).interpColor(LNAME##FillColor(), ind); \
  } \
\
  double LNAME##FillAlpha() const { return LNAME##FillData_.alpha(); } \
  void set##UNAME##FillAlpha(double a) { \
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
    LNAME##FillData_ = data; LNAME##FillDataInvalidate(true); \
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
};

//------

#define CQCHARTS_TEXT_DATA_PROPERTIES \
Q_PROPERTY(CQChartsTextData textData READ textData WRITE setTextData) \
\
Q_PROPERTY(bool          textVisible   READ isTextVisible   WRITE setTextVisible  ) \
Q_PROPERTY(CQChartsColor textColor     READ textColor       WRITE setTextColor    ) \
Q_PROPERTY(double        textAlpha     READ textAlpha       WRITE setTextAlpha    ) \
Q_PROPERTY(CQChartsFont  textFont      READ textFont        WRITE setTextFont     ) \
Q_PROPERTY(double        textAngle     READ textAngle       WRITE setTextAngle    ) \
Q_PROPERTY(bool          textContrast  READ isTextContrast  WRITE setTextContrast ) \
Q_PROPERTY(Qt::Alignment textAlign     READ textAlign       WRITE setTextAlign    ) \
Q_PROPERTY(bool          textFormatted READ isTextFormatted WRITE setTextFormatted) \
Q_PROPERTY(bool          textScaled    READ isTextScaled    WRITE setTextScaled   ) \
Q_PROPERTY(bool          textHtml      READ isTextHtml      WRITE setTextHtml     )

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

  bool isTextVisible() const { return textData_.isVisible(); }
  void setTextVisible(bool b) {
    if (b != textData_.isVisible()) {
      textData_.setVisible(b); textDataInvalidate(true); }
  }

  const CQChartsColor &textColor() const { return textData_.color(); }
  void setTextColor(const CQChartsColor &c) {
    if (c != textData_.color()) {
      textData_.setColor(c); textDataInvalidate(); }
  }

  double textAlpha() const { return textData_.alpha(); }
  void setTextAlpha(double a) {
    if (a != textData_.alpha()) {
      textData_.setAlpha(a); textDataInvalidate(); }
  }

#if 0
  QColor interpTextColor(int i, int n) const {
    return CQChartsInterpolator(textDataObj_).interpColor(textColor(), i, n);
  }

  QColor interpTextColor(double r) const {
    return CQChartsInterpolator(textDataObj_).interpColor(textColor(), r);
  }
#endif

  QColor interpTextColor(const CQChartsUtil::ColorInd &ind) const {
    return CQChartsInterpolator(textDataObj_).interpColor(textColor(), ind);
  }

  const CQChartsFont &textFont() const { return textData_.font(); }
  void setTextFont(const CQChartsFont &f) {
    if (f != textData_.font()) {
      textData_.setFont(f); textDataInvalidate(); }
  }

  double textAngle() const { return textData_.angle(); }
  void setTextAngle(double a) {
    if (a != textData_.angle()) {
      textData_.setAngle(a); textDataInvalidate(); }
  }

  bool isTextContrast() const { return textData_.isContrast(); }
  void setTextContrast(bool b) {
    if (b != textData_.isContrast()) {
      textData_.setContrast(b); textDataInvalidate(); }
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
};

//------

#define CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(UNAME,LNAME) \
Q_PROPERTY(CQChartsTextData LNAME##TextData READ LNAME##TextData WRITE set##UNAME##TextData) \
\
Q_PROPERTY(bool          LNAME##TextVisible \
           READ is##UNAME##TextVisible   WRITE set##UNAME##TextVisible  ) \
Q_PROPERTY(CQChartsColor LNAME##TextColor \
           READ LNAME##TextColor         WRITE set##UNAME##TextColor    ) \
Q_PROPERTY(double        LNAME##TextAlpha \
           READ LNAME##TextAlpha         WRITE set##UNAME##TextAlpha    ) \
Q_PROPERTY(CQChartsFont  LNAME##TextFont \
           READ LNAME##TextFont          WRITE set##UNAME##TextFont     ) \
Q_PROPERTY(double        LNAME##TextAngle \
           READ LNAME##TextAngle         WRITE set##UNAME##TextAngle    ) \
Q_PROPERTY(bool          LNAME##TextContrast \
           READ is##UNAME##TextContrast  WRITE set##UNAME##TextContrast ) \
Q_PROPERTY(Qt::Alignment LNAME##TextAlign \
           READ LNAME##TextAlign         WRITE set##UNAME##TextAlign    ) \
Q_PROPERTY(bool          LNAME##TextFormatted \
           READ is##UNAME##TextFormatted WRITE set##UNAME##TextFormatted) \
Q_PROPERTY(bool          LNAME##TextScaled \
           READ is##UNAME##TextScaled    WRITE set##UNAME##TextScaled   ) \
Q_PROPERTY(bool          LNAME##TextHtml \
           READ is##UNAME##TextHtml      WRITE set##UNAME##TextHtml     )

/*!
 * \brief Object named text data
 * \ingroup Charts
 */
#define CQCHARTS_NAMED_TEXT_DATA(UNAME,LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##TextData { \
 public: \
  CQChartsObj##UNAME##TextData(OBJ *obj) : \
   LNAME##TextDataObj_(obj) { \
  } \
\
  bool is##UNAME##TextVisible() const { return LNAME##TextData_.isVisible(); } \
  void set##UNAME##TextVisible(bool b) { \
    if (b != LNAME##TextData_.isVisible()) { \
      LNAME##TextData_.setVisible(b); LNAME##TextDataInvalidate(true); } \
  } \
\
  const CQChartsColor &LNAME##TextColor() const { return LNAME##TextData_.color(); } \
  void set##UNAME##TextColor(const CQChartsColor &c) { \
    if (c != LNAME##TextData_.color()) { \
      LNAME##TextData_.setColor(c); LNAME##TextDataInvalidate(); } \
  } \
\
  double LNAME##TextAlpha() const { return LNAME##TextData_.alpha(); } \
  void set##UNAME##TextAlpha(double a) { \
    if (a != LNAME##TextData_.alpha()) { \
      LNAME##TextData_.setAlpha(a); LNAME##TextDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##TextColor(int i, int n) const { \
    return CQChartsInterpolator(LNAME##TextDataObj_).interpColor(LNAME##TextColor(), i, n); \
  } \
\
  QColor interp##UNAME##TextColor(double r) const { \
    return CQChartsInterpolator(LNAME##TextDataObj_).interpColor(LNAME##TextColor(), r); \
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
  double LNAME##TextAngle() const { return LNAME##TextData_.angle(); } \
  void set##UNAME##TextAngle(double a) { \
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
};

//------

#define CQCHARTS_STROKE_DATA_PROPERTIES \
Q_PROPERTY(CQChartsStrokeData strokeData READ strokeData WRITE setStrokeData) \
\
Q_PROPERTY(bool             stroked     READ isStroked   WRITE setStroked    ) \
Q_PROPERTY(CQChartsColor    strokeColor READ strokeColor WRITE setStrokeColor) \
Q_PROPERTY(double           strokeAlpha READ strokeAlpha WRITE setStrokeAlpha) \
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

  double strokeAlpha() const { return strokeData_.alpha(); }
  void setStrokeAlpha(double a) {
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

#if 0
  QColor interpStrokeColor(int i, int n) const {
    if (strokeDataObj_)
      return CQChartsInterpolator(strokeDataObj_).interpColor(strokeColor(), i, n);
    else
      return strokeColor().color();
  }

  QColor interpStrokeColor(double r) const {
    if (strokeDataObj_)
      return CQChartsInterpolator(strokeDataObj_).interpColor(strokeColor(), r);
    else
      return strokeColor().color();
  }
#endif

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
Q_PROPERTY(bool              stroked     READ isStroked   WRITE setStroked    ) \
Q_PROPERTY(CQChartsColor     strokeColor READ strokeColor WRITE setStrokeColor) \
Q_PROPERTY(double            strokeAlpha READ strokeAlpha WRITE setStrokeAlpha) \
Q_PROPERTY(CQChartsLength    strokeWidth READ strokeWidth WRITE setStrokeWidth) \
Q_PROPERTY(CQChartsLineDash  strokeDash  READ strokeDash  WRITE setStrokeDash ) \
Q_PROPERTY(CQChartsLength    cornerSize  READ cornerSize  WRITE setCornerSize ) \
\
Q_PROPERTY(bool                filled      READ isFilled    WRITE setFilled     ) \
Q_PROPERTY(CQChartsColor       fillColor   READ fillColor   WRITE setFillColor  ) \
Q_PROPERTY(double              fillAlpha   READ fillAlpha   WRITE setFillAlpha  ) \
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

  double strokeAlpha() const { return shapeData_.stroke().alpha(); }
  void setStrokeAlpha(double a) {
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

#if 0
  QColor interpStrokeColor(int i, int n) const {
    if (shapeDataObj_)
      return CQChartsInterpolator(shapeDataObj_).interpColor(strokeColor(), i, n);
    else
      return strokeColor().color();
  }

  QColor interpStrokeColor(double r) const {
    if (shapeDataObj_)
      return CQChartsInterpolator(shapeDataObj_).interpColor(strokeColor(), r);
    else
      return strokeColor().color();
  }
#endif

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

  double fillAlpha() const { return shapeData_.fill().alpha(); }
  void setFillAlpha(double a) {
    if (a != shapeData_.fill().alpha()) {
      shapeData_.fill().setAlpha(a); shapeDataInvalidate(); }
  }

  const CQChartsFillPattern &fillPattern() const { return shapeData_.fill().pattern(); }
  void setFillPattern(const CQChartsFillPattern &p) {
    if (p != shapeData_.fill().pattern()) {
      shapeData_.fill().setPattern(p); shapeDataInvalidate(); }
  }

#if 0
  QColor interpFillColor(int i, int n) const {
    if (shapeDataObj_)
      return CQChartsInterpolator(shapeDataObj_).interpColor(fillColor(), i, n);
    else
      return fillColor().color();
  }

  QColor interpFillColor(double r) const {
    if (shapeDataObj_)
      return CQChartsInterpolator(shapeDataObj_).interpColor(fillColor(), r);
    else
      return fillColor().color();
  }
#endif

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

#define CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(UNAME,LNAME) \
Q_PROPERTY(CQChartsShapeData LNAME##ShapeData \
           READ LNAME##ShapeData WRITE set##UNAME##ShapeData) \
\
Q_PROPERTY(bool             LNAME##Stroked \
           READ is##UNAME##Stroked WRITE set##UNAME##Stroked    ) \
Q_PROPERTY(CQChartsColor    LNAME##StrokeColor \
           READ LNAME##StrokeColor WRITE set##UNAME##StrokeColor) \
Q_PROPERTY(double           LNAME##StrokeAlpha \
           READ LNAME##StrokeAlpha WRITE set##UNAME##StrokeAlpha) \
Q_PROPERTY(CQChartsLength   LNAME##StrokeWidth \
           READ LNAME##StrokeWidth WRITE set##UNAME##StrokeWidth) \
Q_PROPERTY(CQChartsLineDash LNAME##StrokeDash \
           READ LNAME##StrokeDash  WRITE set##UNAME##StrokeDash ) \
Q_PROPERTY(CQChartsLength   LNAME##CornerSize \
           READ LNAME##CornerSize  WRITE set##UNAME##CornerSize ) \
\
Q_PROPERTY(bool                LNAME##Filled \
           READ is##UNAME##Filled  WRITE set##UNAME##Filled     ) \
Q_PROPERTY(CQChartsColor       LNAME##FillColor \
           READ LNAME##FillColor   WRITE set##UNAME##FillColor  ) \
Q_PROPERTY(double              LNAME##FillAlpha \
           READ LNAME##FillAlpha   WRITE set##UNAME##FillAlpha  ) \
Q_PROPERTY(CQChartsFillPattern LNAME##FillPattern \
           READ LNAME##FillPattern WRITE set##UNAME##FillPattern)

/*!
 * \brief Object named shape data
 * \ingroup Charts
 */
#define CQCHARTS_NAMED_SHAPE_DATA(UNAME,LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##ShapeData { \
 public: \
  CQChartsObj##UNAME##ShapeData(OBJ *obj) : \
   LNAME##ShapeDataObj_(obj) { \
  } \
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
  double LNAME##StrokeAlpha() const { return LNAME##ShapeData_.stroke().alpha(); } \
  void set##UNAME##StrokeAlpha(double a) { \
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
  QColor interp##UNAME##StrokeColor(int i, int n) const { \
    return CQChartsInterpolator(LNAME##ShapeDataObj_).interpColor(LNAME##StrokeColor(), i, n); \
  } \
\
  QColor interp##UNAME##StrokeColor(double r) const { \
    return CQChartsInterpolator(LNAME##ShapeDataObj_).interpColor(LNAME##StrokeColor(), r); \
  } \
\
  QColor interp##UNAME##StrokeColor(const CQChartsUtil::ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##ShapeDataObj_).interpColor(LNAME##StrokeColor(), ind); \
  } \
\
  bool is##UNAME##Filled() const { return LNAME##ShapeData_.fill().isVisible(); } \
  void set##UNAME##Filled(bool b) { \
    if (b != LNAME##ShapeData_.fill().isVisible()) { \
      LNAME##ShapeData_.fill().setVisible(b); LNAME##ShapeDataInvalidate(true); } \
  } \
\
  const CQChartsColor &LNAME##FillColor() const { return LNAME##ShapeData_.fill().color(); } \
  void set##UNAME##FillColor(const CQChartsColor &c) { \
    if (c != LNAME##ShapeData_.fill().color()) { \
      LNAME##ShapeData_.fill().setColor(c); LNAME##ShapeDataInvalidate(); } \
  } \
\
  double LNAME##FillAlpha() const { return LNAME##ShapeData_.fill().alpha(); } \
  void set##UNAME##FillAlpha(double a) { \
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
  QColor interp##UNAME##FillColor(int i, int n) const { \
    return CQChartsInterpolator(LNAME##ShapeDataObj_).interpColor(LNAME##FillColor(), i, n); \
  } \
\
  QColor interp##UNAME##FillColor(double r) const { \
    return CQChartsInterpolator(LNAME##ShapeDataObj_).interpColor(LNAME##FillColor(), r); \
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
};

//------

#define CQCHARTS_BOX_DATA_PROPERTIES \
Q_PROPERTY(CQChartsBoxData boxData READ boxData WRITE setBoxData) \
\
Q_PROPERTY(bool   visible READ isVisible WRITE setVisible) \
Q_PROPERTY(double margin  READ margin    WRITE setMargin ) \
Q_PROPERTY(double padding READ padding   WRITE setPadding) \
\
Q_PROPERTY(bool              stroked     READ isStroked   WRITE setStroked    ) \
Q_PROPERTY(CQChartsColor     strokeColor READ strokeColor WRITE setStrokeColor) \
Q_PROPERTY(double            strokeAlpha READ strokeAlpha WRITE setStrokeAlpha) \
Q_PROPERTY(CQChartsLength    strokeWidth READ strokeWidth WRITE setStrokeWidth) \
Q_PROPERTY(CQChartsLineDash  strokeDash  READ strokeDash  WRITE setStrokeDash ) \
Q_PROPERTY(CQChartsLength    cornerSize  READ cornerSize  WRITE setCornerSize ) \
\
Q_PROPERTY(bool                filled      READ isFilled    WRITE setFilled     ) \
Q_PROPERTY(CQChartsColor       fillColor   READ fillColor   WRITE setFillColor  ) \
Q_PROPERTY(double              fillAlpha   READ fillAlpha   WRITE setFillAlpha  ) \
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

  bool isVisible() const { return boxData_.isVisible(); }
  void setVisible(bool b) { boxData_.setVisible(b); boxDataInvalidate(); }

  double margin() const { return boxData_.margin(); }
  void setMargin(double r) { boxData_.setMargin(r); boxDataInvalidate(); }

  double padding() const { return boxData_.padding(); }
  void setPadding(double r) { boxData_.setPadding(r); boxDataInvalidate(); }

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

  double strokeAlpha() const { return boxData_.shape().stroke().alpha(); }
  void setStrokeAlpha(double a) {
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

#if 0
  QColor interpStrokeColor(int i, int n) const {
    if (boxDataObj_)
      return CQChartsInterpolator(boxDataObj_).interpColor(strokeColor(), i, n);
    else
      return strokeColor().color();
  }

  QColor interpStrokeColor(double r) const {
    if (boxDataObj_)
      return CQChartsInterpolator(boxDataObj_).interpColor(strokeColor(), r);
    else
      return strokeColor().color();
  }
#endif

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

  double fillAlpha() const { return boxData_.shape().fill().alpha(); }
  void setFillAlpha(double a) {
    if (a != boxData_.shape().fill().alpha()) {
      boxData_.shape().fill().setAlpha(a); boxDataInvalidate(); }
  }

  const CQChartsFillPattern &fillPattern() const { return boxData_.shape().fill().pattern(); }
  void setFillPattern(const CQChartsFillPattern &p) {
    if (p != boxData_.shape().fill().pattern()) {
      boxData_.shape().fill().setPattern(p); boxDataInvalidate(); }
  }

#if 0
  QColor interpFillColor(int i, int n) const {
    if (boxDataObj_)
      return CQChartsInterpolator(boxDataObj_).interpColor(fillColor(), i, n);
    else
      return fillColor().color();
  }

  QColor interpFillColor(double r) const {
    if (boxDataObj_)
      return CQChartsInterpolator(boxDataObj_).interpColor(fillColor(), r);
    else
      return fillColor().color();
  }
#endif

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

CQCHARTS_NAMED_FILL_DATA (Background,background)
CQCHARTS_NAMED_SHAPE_DATA(Bar,bar)
CQCHARTS_NAMED_TEXT_DATA (Header,header)
CQCHARTS_NAMED_SHAPE_DATA(BestFit,bestFit)
CQCHARTS_NAMED_LINE_DATA (Stats,stats)

#endif
