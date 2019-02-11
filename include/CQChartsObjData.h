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

  QColor interpLinesColor(int i, int n) const {
    return lineDataObj_->charts()->interpColor(linesColor(), i, n);
  }

  double linesAlpha() const { return lineData_.alpha(); }
  void setLinesAlpha(double a) {
    if (a != lineData_.alpha()) {
      lineData_.setAlpha(a); lineDataInvalidate(); }
  }

  const CQChartsLength &linesWidth() const { return lineData_.width(); }
  void setLinesWidth(const CQChartsLength &l) {
    if (l != lineData_.width()) {
      lineData_.setWidth(l); lineDataInvalidate(); }
   }

  const CQChartsLineDash &linesDash() const { return lineData_.dash(); }
  void setLinesDash(const CQChartsLineDash &d) {
    if (d != lineData_.dash()) {
      lineData_.setDash(d); lineDataInvalidate(); }
  }

  void setLineDataPen(QPen &pen, int i, int n) const {
    QColor lc = interpLinesColor(i, n);

    lineDataObj_->setPen(pen, isLines(), lc, linesAlpha(), linesWidth(), linesDash());
  }

  //---

  const CQChartsLineData &lineData() const { return lineData_; }

  void setLineData(const CQChartsLineData &data) {
    lineData_ = data; lineDataInvalidate();
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
    return LNAME##LineDataObj_->charts()->interpColor(LNAME##LinesColor(), i, n); \
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
  const CQChartsLineData &LNAME##LineData() const { return LNAME##LineData_; } \
 \
  void set##UNAME##LineData(const CQChartsLineData &data) { \
    LNAME##LineData_ = data; LNAME##LineDataInvalidate(); \
  }; \
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
Q_PROPERTY(CQChartsSymbolData pointData READ pointData WRITE setPointData) \
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
           READ symbolFillPattern WRITE setSymbolFillPattern)

template<class OBJ>
class CQChartsObjPointData {
 public:
  CQChartsObjPointData(OBJ *obj) :
   pointDataObj_(obj) {
  }

  bool isPoints() const { return pointData_.visible; }
  void setPoints(bool b) {
    CQChartsUtil::testAndSet(pointData_.visible, b, [&]() { pointDataInvalidate(true); } );
  }

  const CQChartsSymbol &symbolType() const { return pointData_.type; }
  void setSymbolType(const CQChartsSymbol &t) {
    CQChartsUtil::testAndSet(pointData_.type, t, [&]() { pointDataInvalidate(true); } );
  }

  const CQChartsLength &symbolSize() const { return pointData_.size; }
  void setSymbolSize(const CQChartsLength &s) {
    CQChartsUtil::testAndSet(pointData_.size, s, [&]() { pointDataInvalidate(true); } );
  }

  bool isSymbolStroked() const { return pointData_.stroke.isVisible(); }
  void setSymbolStroked(bool b) {
    if (b != pointData_.stroke.isVisible()) {
      pointData_.stroke.setVisible(b); pointDataInvalidate(); }
  }

  const CQChartsColor &symbolStrokeColor() const { return pointData_.stroke.color(); }
  void setSymbolStrokeColor(const CQChartsColor &c) {
    if (c != pointData_.stroke.color()) {
      pointData_.stroke.setColor(c); pointDataInvalidate(); }
  }

  QColor interpSymbolStrokeColor(int i, int n) const {
    return pointDataObj_->charts()->interpColor(symbolStrokeColor(), i, n);
  }

  double symbolStrokeAlpha() const { return pointData_.stroke.alpha(); }
  void setSymbolStrokeAlpha(double a) {
    if (a != pointData_.stroke.alpha()) {
      pointData_.stroke.setAlpha(a); pointDataInvalidate(); }
  }

  const CQChartsLength &symbolStrokeWidth() const { return pointData_.stroke.width(); }
  void setSymbolStrokeWidth(const CQChartsLength &l) {
    if (l != pointData_.stroke.width()) {
      pointData_.stroke.setWidth(l); pointDataInvalidate(); }
  }

  const CQChartsLineDash &symbolStrokeDash() const { return pointData_.stroke.dash(); }
  void setSymbolStrokeDash(const CQChartsLineDash &d) {
    if (d != pointData_.stroke.dash()) {
      pointData_.stroke.setDash(d); pointDataInvalidate(); }
  }

  bool isSymbolFilled() const { return pointData_.fill.isVisible(); }
  void setSymbolFilled(bool b) {
    if (b != pointData_.fill.isVisible()) {
      pointData_.fill.setVisible(b); pointDataInvalidate(); }
  }

  const CQChartsColor &symbolFillColor() const { return pointData_.fill.color(); }
  void setSymbolFillColor(const CQChartsColor &c) {
    if (c != pointData_.fill.color()) {
      pointData_.fill.setColor(c); pointDataInvalidate(); }
  }

  QColor interpSymbolFillColor(double r) const {
    return pointDataObj_->charts()->interpColor(symbolFillColor(), r);
  }

  QColor interpSymbolFillColor(int i, int n) const {
    return pointDataObj_->charts()->interpColor(symbolFillColor(), i, n);
  }

  double symbolFillAlpha() const { return pointData_.fill.alpha(); }
  void setSymbolFillAlpha(double a) {
    if (a != pointData_.fill.alpha()) {
      pointData_.fill.setAlpha(a); pointDataInvalidate(); }
  }

  const CQChartsFillPattern &symbolFillPattern() const { return pointData_.fill.pattern(); }
  void setSymbolFillPattern(const CQChartsFillPattern &p) {
    if (p != pointData_.fill.pattern()) {
      pointData_.fill.setPattern(p); pointDataInvalidate(); }
  }

  void setSymbolPenBrush(QPen &pen, QBrush &brush, int i, int n) const {
    pointDataObj_->setPenBrush(pen, brush,
      isSymbolStroked(), interpSymbolStrokeColor(i, n), symbolStrokeAlpha(),
      symbolStrokeWidth(), symbolStrokeDash(),
      isSymbolFilled(), interpSymbolFillColor(i, n), symbolFillAlpha(), symbolFillPattern());
  }

  //---

  const CQChartsSymbolData &pointData() const { return pointData_; }

  void setPointData(const CQChartsSymbolData &data) {
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

#define CQCHARTS_NAMED_POINT_DATA(UNAME,LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##PointData { \
 public: \
  CQChartsObj##UNAME##PointData(OBJ *obj) : \
   LNAME##PointDataObj_(obj) { \
  } \
\
  bool is##UNAME##Points() const { return LNAME##PointData_.visible; } \
  void set##UNAME##Points(bool b) { \
    CQChartsUtil::testAndSet(LNAME##PointData_.visible, b, [&]() { \
      LNAME##PointDataInvalidate(true); } ); \
  } \
\
  const CQChartsSymbol &LNAME##SymbolType() const { return LNAME##PointData_.type; } \
  void set##UNAME##SymbolType(const CQChartsSymbol &t) { \
    CQChartsUtil::testAndSet(LNAME##PointData_.type, t, [&]() { \
      LNAME##PointDataInvalidate(); } ); \
  } \
\
  const CQChartsLength &LNAME##SymbolSize() const { return LNAME##PointData_.size; } \
  void set##UNAME##SymbolSize(const CQChartsLength &s) { \
    CQChartsUtil::testAndSet(LNAME##PointData_.size, s, [&]() { \
      LNAME##PointDataInvalidate(); } ); \
  } \
\
  bool is##UNAME##SymbolStroked() const { return LNAME##PointData_.stroke.isVisible(); } \
  void set##UNAME##SymbolStroked(bool b) { \
    if (b != LNAME##PointData_.stroke.isVisible()) { \
      LNAME##PointData_.stroke.setVisible(b); LNAME##PointDataInvalidate(); } \
  } \
\
  const CQChartsColor &LNAME##SymbolStrokeColor() const { \
    return LNAME##PointData_.stroke.color(); } \
  void set##UNAME##SymbolStrokeColor(const CQChartsColor &c) { \
    if (c != LNAME##PointData_.stroke.color()) { \
      LNAME##PointData_.stroke.setColor(c); LNAME##PointDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##SymbolStrokeColor(int i, int n) const { \
    return LNAME##PointDataObj_->charts()->interpColor(LNAME##SymbolStrokeColor(), i, n); \
  } \
\
  double LNAME##SymbolStrokeAlpha() const { return LNAME##PointData_.stroke.alpha(); } \
  void set##UNAME##SymbolStrokeAlpha(double a) { \
    if (a != LNAME##PointData_.stroke.alpha()) { \
      LNAME##PointData_.stroke.setAlpha(a); LNAME##PointDataInvalidate(); } \
  } \
\
  const CQChartsLength &LNAME##SymbolStrokeWidth() const { \
    return LNAME##PointData_.stroke.width(); } \
  void set##UNAME##SymbolStrokeWidth(const CQChartsLength &l) { \
    if (l != LNAME##PointData_.stroke.width()) { \
      LNAME##PointData_.stroke.setWidth(l); LNAME##PointDataInvalidate(); } \
  } \
\
  const CQChartsLineDash &LNAME##SymbolStrokeDash() const { \
    return LNAME##PointData_.stroke.dash(); } \
  void set##UNAME##SymbolStrokeDash(const CQChartsLineDash &d) { \
    if (d != LNAME##PointData_.stroke.dash()) { \
      LNAME##PointData_.stroke.setDash(d); LNAME##PointDataInvalidate(); } \
  } \
\
  bool is##UNAME##SymbolFilled() const { return LNAME##PointData_.fill.isVisible(); } \
  void set##UNAME##SymbolFilled(bool b) { \
    if (b != LNAME##PointData_.fill.isVisible()) { \
      LNAME##PointData_.fill.setVisible(b); LNAME##PointDataInvalidate(true); } \
  } \
\
  const CQChartsColor &LNAME##SymbolFillColor() const { return LNAME##PointData_.fill.color(); } \
  void set##UNAME##SymbolFillColor(const CQChartsColor &c) { \
    if (c != LNAME##PointData_.fill.color()) { \
      LNAME##PointData_.fill.setColor(c); LNAME##PointDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##SymbolFillColor(double r) const { \
    return LNAME##PointDataObj_->charts()->interpColor(LNAME##SymbolFillColor(), r); \
  } \
\
  QColor interp##UNAME##SymbolFillColor(int i, int n) const { \
    return LNAME##PointDataObj_->charts()->interpColor(LNAME##SymbolFillColor(), i, n); \
  } \
\
  double LNAME##SymbolFillAlpha() const { return LNAME##PointData_.fill.alpha(); } \
  void set##UNAME##SymbolFillAlpha(double a) { \
    if (a != LNAME##PointData_.fill.alpha()) { \
      LNAME##PointData_.fill.setAlpha(a); LNAME##PointDataInvalidate(); } \
  } \
\
  const CQChartsFillPattern &LNAME##SymbolFillPattern() const { \
    return LNAME##PointData_.fill.pattern(); } \
  void set##UNAME##SymbolFillPattern(const CQChartsFillPattern &p) { \
    if (p != LNAME##PointData_.fill.pattern()) { \
      LNAME##PointData_.fill.setPattern(p); LNAME##PointDataInvalidate(); } \
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
Q_PROPERTY(bool                LNAME##Filled \
           READ is##UNAME##Filled  WRITE set##UNAME##Filled     ) \
Q_PROPERTY(CQChartsColor       LNAME##FillColor \
           READ LNAME##FillColor   WRITE set##UNAME##FillColor  ) \
Q_PROPERTY(double              LNAME##FillAlpha \
           READ LNAME##FillAlpha   WRITE set##UNAME##FillAlpha  ) \
Q_PROPERTY(CQChartsFillPattern LNAME##FillPattern \
           READ LNAME##FillPattern WRITE set##UNAME##FillPattern)

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
    return LNAME##FillDataObj_->charts()->interpColor(LNAME##FillColor(), i, n); \
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
      LNAME##FillData_.setPattern(p); LNAME##FillDataInvalidate(true); } \
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
Q_PROPERTY(QFont         textFont      READ textFont        WRITE setTextFont     ) \
Q_PROPERTY(double        textAngle     READ textAngle       WRITE setTextAngle    ) \
Q_PROPERTY(bool          textContrast  READ isTextContrast  WRITE setTextContrast ) \
Q_PROPERTY(Qt::Alignment textAlign     READ textAlign       WRITE setTextAlign    ) \
Q_PROPERTY(bool          textFormatted READ isTextFormatted WRITE setTextFormatted) \
Q_PROPERTY(bool          textScaled    READ isTextScaled    WRITE setTextScaled   ) \
Q_PROPERTY(bool          textHtml      READ isTextHtml      WRITE setTextHtml     )

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

  QColor interpTextColor(int i, int n) const {
    return textDataObj_->charts()->interpColor(textColor(), i, n);
  }

  const QFont &textFont() const { return textData_.font(); }
  void setTextFont(const QFont &f) {
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
Q_PROPERTY(bool          LNAME##TextVisible \
           READ is##UNAME##TextVisible   WRITE set##UNAME##TextVisible  ) \
Q_PROPERTY(CQChartsColor LNAME##TextColor \
           READ LNAME##TextColor         WRITE set##UNAME##TextColor    ) \
Q_PROPERTY(double        LNAME##TextAlpha \
           READ LNAME##TextAlpha         WRITE set##UNAME##TextAlpha    ) \
Q_PROPERTY(QFont         LNAME##TextFont \
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
    return LNAME##TextDataObj_->charts()->interpColor(LNAME##TextColor(), i, n); \
  } \
\
  const QFont &LNAME##TextFont() const { return LNAME##TextData_.font(); } \
  void set##UNAME##TextFont(const QFont &f) { \
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
Q_PROPERTY(bool             border      READ isBorder    WRITE setBorder     ) \
Q_PROPERTY(CQChartsColor    borderColor READ borderColor WRITE setBorderColor) \
Q_PROPERTY(double           borderAlpha READ borderAlpha WRITE setBorderAlpha) \
Q_PROPERTY(CQChartsLength   borderWidth READ borderWidth WRITE setBorderWidth) \
Q_PROPERTY(CQChartsLineDash borderDash  READ borderDash  WRITE setBorderDash ) \
Q_PROPERTY(CQChartsLength   cornerSize  READ cornerSize  WRITE setCornerSize )

template<class OBJ>
class CQChartsObjStrokeData {
 public:
  CQChartsObjStrokeData(OBJ *obj) :
   strokeDataObj_(obj) {
  }

  //---

  bool isBorder() const { return strokeData_.isVisible(); }
  void setBorder(bool b) {
    if (b != strokeData_.isVisible()) {
      strokeData_.setVisible(b); strokeDataInvalidate(); }
  }

  const CQChartsColor &borderColor() const { return strokeData_.color(); }
  void setBorderColor(const CQChartsColor &c) {
    if (c != strokeData_.color()) {
      strokeData_.setColor(c); strokeDataInvalidate(); }
  }

  double borderAlpha() const { return strokeData_.alpha(); }
  void setBorderAlpha(double a) {
    if (a != strokeData_.alpha()) {
      strokeData_.setAlpha(a); strokeDataInvalidate(); }
  }

  const CQChartsLength &borderWidth() const { return strokeData_.width(); }
  void setBorderWidth(const CQChartsLength &l) {
    if (l != strokeData_.width()) {
      strokeData_.setWidth(l); strokeDataInvalidate(); }
  }

  const CQChartsLineDash &borderDash() const { return strokeData_.dash(); }
  void setBorderDash(const CQChartsLineDash &d) {
    if (d != strokeData_.dash()) {
      strokeData_.setDash(d); strokeDataInvalidate(); }
  }

  const CQChartsLength &cornerSize() const { return strokeData_.cornerSize(); }
  void setCornerSize(const CQChartsLength &l) {
    if (l != strokeData_.cornerSize()) {
      strokeData_.setCornerSize(l); strokeDataInvalidate(); }
  }

  QColor interpBorderColor(int i, int n) const {
    return strokeDataObj_->charts()->interpColor(borderColor(), i, n);
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
Q_PROPERTY(bool              border      READ isBorder    WRITE setBorder     ) \
Q_PROPERTY(CQChartsColor     borderColor READ borderColor WRITE setBorderColor) \
Q_PROPERTY(double            borderAlpha READ borderAlpha WRITE setBorderAlpha) \
Q_PROPERTY(CQChartsLength    borderWidth READ borderWidth WRITE setBorderWidth) \
Q_PROPERTY(CQChartsLineDash  borderDash  READ borderDash  WRITE setBorderDash ) \
Q_PROPERTY(CQChartsLength    cornerSize  READ cornerSize  WRITE setCornerSize ) \
\
Q_PROPERTY(bool                filled      READ isFilled    WRITE setFilled     ) \
Q_PROPERTY(CQChartsColor       fillColor   READ fillColor   WRITE setFillColor  ) \
Q_PROPERTY(double              fillAlpha   READ fillAlpha   WRITE setFillAlpha  ) \
Q_PROPERTY(CQChartsFillPattern fillPattern READ fillPattern WRITE setFillPattern)

template<class OBJ>
class CQChartsObjShapeData {
 public:
  CQChartsObjShapeData(OBJ *obj) :
   shapeDataObj_(obj) {
  }

  //---

  bool isBorder() const { return shapeData_.border().isVisible(); }
  void setBorder(bool b) {
    if (b != shapeData_.border().isVisible()) {
      shapeData_.border().setVisible(b); shapeDataInvalidate(); }
  }

  const CQChartsColor &borderColor() const { return shapeData_.border().color(); }
  void setBorderColor(const CQChartsColor &c) {
    if (c != shapeData_.border().color()) {
      shapeData_.border().setColor(c); shapeDataInvalidate(); }
  }

  double borderAlpha() const { return shapeData_.border().alpha(); }
  void setBorderAlpha(double a) {
    if (a != shapeData_.border().alpha()) {
      shapeData_.border().setAlpha(a); shapeDataInvalidate(); }
  }

  const CQChartsLength &borderWidth() const { return shapeData_.border().width(); }
  void setBorderWidth(const CQChartsLength &l) {
    if (l != shapeData_.border().width()) {
      shapeData_.border().setWidth(l); shapeDataInvalidate(); }
  }

  const CQChartsLineDash &borderDash() const { return shapeData_.border().dash(); }
  void setBorderDash(const CQChartsLineDash &d) {
    if (d != shapeData_.border().dash()) {
      shapeData_.border().setDash(d); shapeDataInvalidate(); }
  }

  const CQChartsLength &cornerSize() const { return shapeData_.border().cornerSize(); }
  void setCornerSize(const CQChartsLength &l) {
    if (l != shapeData_.border().cornerSize()) {
      shapeData_.border().setCornerSize(l); shapeDataInvalidate(); }
  }

  QColor interpBorderColor(int i, int n) const {
    return shapeDataObj_->charts()->interpColor(borderColor(), i, n);
  }

  //---

  bool isFilled() const { return shapeData_.background().isVisible(); }
  void setFilled(bool b) {
    if (b != shapeData_.background().isVisible()) {
      shapeData_.background().setVisible(b); shapeDataInvalidate(); }
  }

  const CQChartsColor &fillColor() const { return shapeData_.background().color(); }
  void setFillColor(const CQChartsColor &c) {
    if (c != shapeData_.background().color()) {
      shapeData_.background().setColor(c); shapeDataInvalidate(); }
  }

  double fillAlpha() const { return shapeData_.background().alpha(); }
  void setFillAlpha(double a) {
    if (a != shapeData_.background().alpha()) {
      shapeData_.background().setAlpha(a); shapeDataInvalidate(); }
  }

  const CQChartsFillPattern &fillPattern() const { return shapeData_.background().pattern(); }
  void setFillPattern(const CQChartsFillPattern &p) {
    if (p != shapeData_.background().pattern()) {
      shapeData_.background().setPattern(p); shapeDataInvalidate(); }
  }

  QColor interpFillColor(int i, int n) const {
    return shapeDataObj_->charts()->interpColor(fillColor(), i, n);
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
\
Q_PROPERTY(CQChartsShapeData LNAME##ShapeData \
           READ LNAME##ShapeData WRITE set##UNAME##ShapeData) \
\
Q_PROPERTY(bool             LNAME##Border \
           READ is##UNAME##Border  WRITE set##UNAME##Border     ) \
Q_PROPERTY(CQChartsColor    LNAME##BorderColor \
           READ LNAME##BorderColor WRITE set##UNAME##BorderColor) \
Q_PROPERTY(double           LNAME##BorderAlpha \
           READ LNAME##BorderAlpha WRITE set##UNAME##BorderAlpha) \
Q_PROPERTY(CQChartsLength   LNAME##BorderWidth \
           READ LNAME##BorderWidth WRITE set##UNAME##BorderWidth) \
Q_PROPERTY(CQChartsLineDash LNAME##BorderDash \
           READ LNAME##BorderDash  WRITE set##UNAME##BorderDash ) \
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

#define CQCHARTS_NAMED_SHAPE_DATA(UNAME,LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##ShapeData { \
 public: \
  CQChartsObj##UNAME##ShapeData(OBJ *obj) : \
   LNAME##ShapeDataObj_(obj) { \
  } \
\
  bool is##UNAME##Border() const { return LNAME##ShapeData_.border().isVisible(); } \
  void set##UNAME##Border(bool b) { \
    if (b != LNAME##ShapeData_.border().isVisible()) { \
      LNAME##ShapeData_.border().setVisible(b); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const CQChartsColor &LNAME##BorderColor() const { return LNAME##ShapeData_.border().color(); } \
  void set##UNAME##BorderColor(const CQChartsColor &c) { \
    if (c != LNAME##ShapeData_.border().color()) { \
      LNAME##ShapeData_.border().setColor(c); LNAME##ShapeDataInvalidate(); } \
  } \
\
  double LNAME##BorderAlpha() const { return LNAME##ShapeData_.border().alpha(); } \
  void set##UNAME##BorderAlpha(double a) { \
    if (a != LNAME##ShapeData_.border().alpha()) { \
      LNAME##ShapeData_.border().setAlpha(a); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const CQChartsLength &LNAME##BorderWidth() const { return LNAME##ShapeData_.border().width(); } \
  void set##UNAME##BorderWidth(const CQChartsLength &l) { \
    if (l != LNAME##ShapeData_.border().width()) { \
      LNAME##ShapeData_.border().setWidth(l); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const CQChartsLineDash &LNAME##BorderDash() const { return LNAME##ShapeData_.border().dash(); } \
  void set##UNAME##BorderDash(const CQChartsLineDash &d) { \
    if (d != LNAME##ShapeData_.border().dash()) { \
      LNAME##ShapeData_.border().setDash(d); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const CQChartsLength &LNAME##CornerSize() const { \
    return LNAME##ShapeData_.border().cornerSize(); } \
  void set##UNAME##CornerSize(const CQChartsLength &l) { \
    if (l != LNAME##ShapeData_.border().cornerSize()) { \
      LNAME##ShapeData_.border().setCornerSize(l); LNAME##ShapeDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##BorderColor(int i, int n) const { \
    return LNAME##ShapeDataObj_->charts()->interpColor(LNAME##BorderColor(), i, n); \
  } \
\
  bool is##UNAME##Filled() const { return LNAME##ShapeData_.background().isVisible(); } \
  void set##UNAME##Filled(bool b) { \
    if (b != LNAME##ShapeData_.background().isVisible()) { \
      LNAME##ShapeData_.background().setVisible(b); LNAME##ShapeDataInvalidate(true); } \
  } \
\
  const CQChartsColor &LNAME##FillColor() const { return LNAME##ShapeData_.background().color(); } \
  void set##UNAME##FillColor(const CQChartsColor &c) { \
    if (c != LNAME##ShapeData_.background().color()) { \
      LNAME##ShapeData_.background().setColor(c); LNAME##ShapeDataInvalidate(); } \
  } \
\
  double LNAME##FillAlpha() const { return LNAME##ShapeData_.background().alpha(); } \
  void set##UNAME##FillAlpha(double a) { \
    if (a != LNAME##ShapeData_.background().alpha()) { \
      LNAME##ShapeData_.background().setAlpha(a); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const CQChartsFillPattern &LNAME##FillPattern() const { \
    return LNAME##ShapeData_.background().pattern(); } \
  void set##UNAME##FillPattern(const CQChartsFillPattern &p) { \
    if (p != LNAME##ShapeData_.background().pattern()) { \
      LNAME##ShapeData_.background().setPattern(p); LNAME##ShapeDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##FillColor(int i, int n) const { \
    return LNAME##ShapeDataObj_->charts()->interpColor(LNAME##FillColor(), i, n); \
  } \
\
  const CQChartsShapeData &LNAME##ShapeData() const { return LNAME##ShapeData_; }; \
\
  void set##UNAME##ShapeData(const CQChartsShapeData &data) { \
    LNAME##ShapeData_ = data; LNAME##ShapeDataInvalidate(); \
  }; \
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

CQCHARTS_NAMED_FILL_DATA(Background,background)

#endif
