#ifndef CQChartsObjData_H
#define CQChartsObjData_H

#include <CQChartsData.h>
#include <CQChartsInvalidator.h>
#include <CQChartsUtil.h>

#define CQCHARTS_LINE_DATA_PROPERTIES \
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

  bool isLines() const { return lineData_.visible; }
  void setLines(bool b) {
    CQChartsUtil::testAndSet(lineData_.visible, b, [&]() { lineDataInvalidate(true); } );
  }

  const CQChartsColor &linesColor() const { return lineData_.color; }
  void setLinesColor(const CQChartsColor &c) {
    CQChartsUtil::testAndSet(lineData_.color, c, [&]() { lineDataInvalidate(); } );
  }

  QColor interpLinesColor(int i, int n) const {
    return lineDataObj_->charts()->interpColor(linesColor(), i, n);
  }

  double linesAlpha() const { return lineData_.alpha; }
  void setLinesAlpha(double a) {
    CQChartsUtil::testAndSet(lineData_.alpha, a, [&]() { lineDataInvalidate(); } );
  }

  const CQChartsLength &linesWidth() const { return lineData_.width; }
  void setLinesWidth(const CQChartsLength &l) {
    CQChartsUtil::testAndSet(lineData_.width, l, [&]() { lineDataInvalidate(); } );
   }

  const CQChartsLineDash &linesDash() const { return lineData_.dash; }
  void setLinesDash(const CQChartsLineDash &d) {
    CQChartsUtil::testAndSet(lineData_.dash, d, [&]() { lineDataInvalidate(); } );
  }

  void setLineDataPen(QPen &pen, int i, int n) const {
    QColor lc = interpLinesColor(i, n);

    lineDataObj_->setPen(pen, isLines(), lc, linesAlpha(), linesWidth(), linesDash());
  }

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
  bool is##UNAME##Lines() const { return LNAME##LineData_.visible; } \
  void set##UNAME##Lines(bool b) { \
    CQChartsUtil::testAndSet(LNAME##LineData_.visible, b, [&]() { \
      LNAME##LineDataInvalidate(true); } ); \
  } \
\
  const CQChartsColor &LNAME##LinesColor() const { return LNAME##LineData_.color; } \
  void set##UNAME##LinesColor(const CQChartsColor &c) { \
    CQChartsUtil::testAndSet(LNAME##LineData_.color, c, [&]() { \
      LNAME##LineDataInvalidate(); } ); \
  } \
\
  QColor interp##UNAME##LinesColor(int i, int n) const { \
    return LNAME##LineDataObj_->charts()->interpColor(LNAME##LinesColor(), i, n); \
  } \
\
  double LNAME##LinesAlpha() const { return LNAME##LineData_.alpha; } \
  void set##UNAME##LinesAlpha(double a) { \
    CQChartsUtil::testAndSet(LNAME##LineData_.alpha, a, [&]() { \
      LNAME##LineDataInvalidate(); } ); \
  } \
\
  const CQChartsLength &LNAME##LinesWidth() const { return LNAME##LineData_.width; } \
  void set##UNAME##LinesWidth(const CQChartsLength &l) { \
    CQChartsUtil::testAndSet(LNAME##LineData_.width, l, [&]() { \
      LNAME##LineDataInvalidate(); } ); \
   } \
\
  const CQChartsLineDash &LNAME##LinesDash() const { return LNAME##LineData_.dash; } \
  void set##UNAME##LinesDash(const CQChartsLineDash &d) { \
    CQChartsUtil::testAndSet(LNAME##LineData_.dash, d, [&]() { \
      LNAME##LineDataInvalidate(); } ); \
  } \
\
  void set##UNAME##LineDataPen(QPen &pen, int i, int n) const { \
    QColor lc = interp##UNAME##LinesColor(i, n); \
\
    LNAME##LineDataObj_->setPen(pen, is##UNAME##Lines(), lc, LNAME##LinesAlpha(), \
                                LNAME##LinesWidth(), LNAME##LinesDash()); \
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

  bool isSymbolStroked() const { return pointData_.stroke.visible; }
  void setSymbolStroked(bool b) {
    CQChartsUtil::testAndSet(pointData_.stroke.visible, b, [&]() { pointDataInvalidate(); } );
  }

  const CQChartsColor &symbolStrokeColor() const { return pointData_.stroke.color; }
  void setSymbolStrokeColor(const CQChartsColor &c) {
    CQChartsUtil::testAndSet(pointData_.stroke.color, c, [&]() { pointDataInvalidate(); } );
  }

  QColor interpSymbolStrokeColor(int i, int n) const {
    return pointDataObj_->charts()->interpColor(symbolStrokeColor(), i, n);
  }

  double symbolStrokeAlpha() const { return pointData_.stroke.alpha; }
  void setSymbolStrokeAlpha(double a) {
    CQChartsUtil::testAndSet(pointData_.stroke.alpha, a, [&]() { pointDataInvalidate(); } );
  }

  const CQChartsLength &symbolStrokeWidth() const { return pointData_.stroke.width; }
  void setSymbolStrokeWidth(const CQChartsLength &l) {
    CQChartsUtil::testAndSet(pointData_.stroke.width, l, [&]() { pointDataInvalidate(); } );
  }

  const CQChartsLineDash &symbolStrokeDash() const { return pointData_.stroke.dash; }
  void setSymbolStrokeDash(const CQChartsLineDash &d) {
    CQChartsUtil::testAndSet(pointData_.stroke.dash, d, [&]() { pointDataInvalidate(); } );
  }

  bool isSymbolFilled() const { return pointData_.fill.visible; }
  void setSymbolFilled(bool b) {
    CQChartsUtil::testAndSet(pointData_.fill.visible, b, [&]() { pointDataInvalidate(); } );
  }

  const CQChartsColor &symbolFillColor() const { return pointData_.fill.color; }
  void setSymbolFillColor(const CQChartsColor &c) {
    CQChartsUtil::testAndSet(pointData_.fill.color, c, [&]() { pointDataInvalidate(); } );
  }

  QColor interpSymbolFillColor(double r) const {
    return pointDataObj_->charts()->interpColor(symbolFillColor(), r);
  }

  QColor interpSymbolFillColor(int i, int n) const {
    return pointDataObj_->charts()->interpColor(symbolFillColor(), i, n);
  }

  double symbolFillAlpha() const { return pointData_.fill.alpha; }
  void setSymbolFillAlpha(double a) {
    CQChartsUtil::testAndSet(pointData_.fill.alpha, a, [&]() { pointDataInvalidate(); } );
  }

  const CQChartsFillPattern &symbolFillPattern() const { return pointData_.fill.pattern; }
  void setSymbolFillPattern(const CQChartsFillPattern &p) {
    CQChartsUtil::testAndSet(pointData_.fill.pattern, p, [&]() { pointDataInvalidate(); } );
  }

  void setSymbolPenBrush(QPen &pen, QBrush &brush, int i, int n) const {
    pointDataObj_->setPenBrush(pen, brush,
      isSymbolStroked(), interpSymbolStrokeColor(i, n), symbolStrokeAlpha(),
      symbolStrokeWidth(), symbolStrokeDash(),
      isSymbolFilled(), interpSymbolFillColor(i, n), symbolFillAlpha(), symbolFillPattern());
  }

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
  bool is##UNAME##SymbolStroked() const { return LNAME##PointData_.stroke.visible; } \
  void set##UNAME##SymbolStroked(bool b) { \
    CQChartsUtil::testAndSet(LNAME##PointData_.stroke.visible, b, [&]() { \
      LNAME##PointDataInvalidate(); } ); \
  } \
\
  const CQChartsColor &LNAME##SymbolStrokeColor() const { return LNAME##PointData_.stroke.color; } \
  void set##UNAME##SymbolStrokeColor(const CQChartsColor &c) { \
    CQChartsUtil::testAndSet(LNAME##PointData_.stroke.color, c, [&]() { \
      LNAME##PointDataInvalidate(); } ); \
  } \
\
  QColor interp##UNAME##SymbolStrokeColor(int i, int n) const { \
    return LNAME##PointDataObj_->charts()->interpColor(LNAME##SymbolStrokeColor(), i, n); \
  } \
\
  double LNAME##SymbolStrokeAlpha() const { return LNAME##PointData_.stroke.alpha; } \
  void set##UNAME##SymbolStrokeAlpha(double a) { \
    CQChartsUtil::testAndSet(LNAME##PointData_.stroke.alpha, a, [&]() { \
      LNAME##PointDataInvalidate(); } ); \
  } \
\
  const CQChartsLength &LNAME##SymbolStrokeWidth() const { \
    return LNAME##PointData_.stroke.width; } \
  void set##UNAME##SymbolStrokeWidth(const CQChartsLength &l) { \
    CQChartsUtil::testAndSet(LNAME##PointData_.stroke.width, l, [&]() { \
      LNAME##PointDataInvalidate(); } ); \
  } \
\
  const CQChartsLineDash &LNAME##SymbolStrokeDash() const { \
    return LNAME##PointData_.stroke.dash; } \
  void set##UNAME##SymbolStrokeDash(const CQChartsLineDash &d) { \
    CQChartsUtil::testAndSet(LNAME##PointData_.stroke.dash, d, [&]() { \
      LNAME##PointDataInvalidate(); } ); \
  } \
\
  bool is##UNAME##SymbolFilled() const { return LNAME##PointData_.fill.visible; } \
  void set##UNAME##SymbolFilled(bool b) { \
    CQChartsUtil::testAndSet(LNAME##PointData_.fill.visible, b, [&]() { \
      LNAME##PointDataInvalidate(); } ); \
  } \
\
  const CQChartsColor &LNAME##SymbolFillColor() const { return LNAME##PointData_.fill.color; } \
  void set##UNAME##SymbolFillColor(const CQChartsColor &c) { \
    CQChartsUtil::testAndSet(LNAME##PointData_.fill.color, c, [&]() { \
      LNAME##PointDataInvalidate(); } ); \
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
  double LNAME##SymbolFillAlpha() const { return LNAME##PointData_.fill.alpha; } \
  void set##UNAME##SymbolFillAlpha(double a) { \
    CQChartsUtil::testAndSet(LNAME##PointData_.fill.alpha, a, [&]() { \
      LNAME##PointDataInvalidate(); } ); \
  } \
\
  const CQChartsFillPattern &LNAME##SymbolFillPattern() const { \
    return LNAME##PointData_.fill.pattern; } \
  void set##UNAME##SymbolFillPattern(const CQChartsFillPattern &p) { \
    CQChartsUtil::testAndSet(LNAME##PointData_.fill.pattern, p, [&]() { \
      LNAME##PointDataInvalidate(); } ); \
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
  bool is##UNAME##Filled() const { return LNAME##FillData_.visible; } \
  void set##UNAME##Filled(bool b) { \
    CQChartsUtil::testAndSet(LNAME##FillData_.visible, b, [&]() { \
      LNAME##FillDataInvalidate(true); } ); \
  } \
\
  const CQChartsColor &LNAME##FillColor() const { return LNAME##FillData_.color; } \
  void set##UNAME##FillColor(const CQChartsColor &c) { \
    CQChartsUtil::testAndSet(LNAME##FillData_.color, c, [&]() { \
      LNAME##FillDataInvalidate(); } ); \
  } \
\
  QColor interp##UNAME##FillColor(int i, int n) const { \
    return LNAME##FillDataObj_->charts()->interpColor(LNAME##FillColor(), i, n); \
  } \
\
  double LNAME##FillAlpha() const { return LNAME##FillData_.alpha; } \
  void set##UNAME##FillAlpha(double a) { \
    CQChartsUtil::testAndSet(LNAME##FillData_.alpha, a, [&]() { \
      LNAME##FillDataInvalidate(); } ); \
  } \
\
  const CQChartsFillPattern &LNAME##FillPattern() const { return LNAME##FillData_.pattern; } \
  void set##UNAME##FillPattern(const CQChartsFillPattern &p) { \
    CQChartsUtil::testAndSet(LNAME##FillData_.pattern, p, [&]() { \
      LNAME##FillDataInvalidate(); } ); \
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
Q_PROPERTY(bool          textVisible   READ isTextVisible   WRITE setTextVisible  ) \
Q_PROPERTY(QFont         textFont      READ textFont        WRITE setTextFont     ) \
Q_PROPERTY(CQChartsColor textColor     READ textColor       WRITE setTextColor    ) \
Q_PROPERTY(double        textAlpha     READ textAlpha       WRITE setTextAlpha    ) \
Q_PROPERTY(double        textAngle     READ textAngle       WRITE setTextAngle    ) \
Q_PROPERTY(bool          textContrast  READ isTextContrast  WRITE setTextContrast ) \
Q_PROPERTY(Qt::Alignment textAlign     READ textAlign       WRITE setTextAlign    ) \
Q_PROPERTY(bool          textFormatted READ isTextFormatted WRITE setTextFormatted) \
Q_PROPERTY(bool          textScaled    READ isTextScaled    WRITE setTextScaled   )

template<class OBJ>
class CQChartsObjTextData {
 public:
  CQChartsObjTextData(OBJ *obj) :
   textDataObj_(obj) {
  }

  bool isTextVisible() const { return textData_.visible; }
  void setTextVisible(bool b) {
    CQChartsUtil::testAndSet(textData_.visible, b, [&]() { textDataInvalidate(true); } );
  }

  const QFont &textFont() const { return textData_.font; }
  void setTextFont(const QFont &f) {
    CQChartsUtil::testAndSet(textData_.font, f, [&]() { textDataInvalidate(); } );
  }

  const CQChartsColor &textColor() const { return textData_.color; }
  void setTextColor(const CQChartsColor &c) {
    CQChartsUtil::testAndSet(textData_.color, c, [&]() { textDataInvalidate(); } );
  }

  double textAlpha() const { return textData_.alpha; }
  void setTextAlpha(double a) {
    CQChartsUtil::testAndSet(textData_.alpha, a, [&]() { textDataInvalidate(); } );
  }

  double textAngle() const { return textData_.angle; }
  void setTextAngle(double a) {
    CQChartsUtil::testAndSet(textData_.angle, a, [&]() { textDataInvalidate(); } );
  }

  QColor interpTextColor(int i, int n) const {
    return textDataObj_->charts()->interpColor(textColor(), i, n);
  }

  bool isTextContrast() const { return textData_.contrast; }
  void setTextContrast(bool b) {
    CQChartsUtil::testAndSet(textData_.contrast, b, [&]() { textDataInvalidate(); } );
  }

  const Qt::Alignment &textAlign() const { return textData_.align; }
  void setTextAlign(const Qt::Alignment &a) {
    CQChartsUtil::testAndSet(textData_.align, a, [&]() { textDataInvalidate(); } );
  }

  bool isTextFormatted() const { return textData_.formatted; }
  void setTextFormatted(bool b) {
    CQChartsUtil::testAndSet(textData_.formatted, b, [&]() { textDataInvalidate(); } );
  }

  bool isTextScaled() const { return textData_.scaled; }
  void setTextScaled(bool b) {
    CQChartsUtil::testAndSet(textData_.scaled, b, [&]() { textDataInvalidate(); } );
  }

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
Q_PROPERTY(QFont         LNAME##TextFont \
           READ LNAME##TextFont          WRITE set##UNAME##TextFont     ) \
Q_PROPERTY(CQChartsColor LNAME##TextColor \
           READ LNAME##TextColor         WRITE set##UNAME##TextColor    ) \
Q_PROPERTY(double        LNAME##TextAlpha \
           READ LNAME##TextAlpha         WRITE set##UNAME##TextAlpha    ) \
Q_PROPERTY(double        LNAME##TextAngle \
           READ LNAME##TextAngle         WRITE set##UNAME##TextAngle    ) \
Q_PROPERTY(bool          LNAME##TextContrast \
           READ is##UNAME##TextContrast  WRITE set##UNAME##TextContrast ) \
Q_PROPERTY(Qt::Alignment LNAME##TextAlign \
           READ LNAME##TextAlign         WRITE set##UNAME##TextAlign    ) \
Q_PROPERTY(bool          LNAME##TextFormatted \
           READ is##UNAME##TextFormatted WRITE set##UNAME##TextFormatted) \
Q_PROPERTY(bool          LNAME##TextScaled \
           READ is##UNAME##TextScaled    WRITE set##UNAME##TextScaled   )

#define CQCHARTS_NAMED_TEXT_DATA(UNAME,LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##TextData { \
 public: \
  CQChartsObj##UNAME##TextData(OBJ *obj) : \
   LNAME##TextDataObj_(obj) { \
  } \
\
  bool is##UNAME##TextVisible() const { return LNAME##TextData_.visible; } \
  void set##UNAME##TextVisible(bool b) { \
    CQChartsUtil::testAndSet(LNAME##TextData_.visible, b, [&]() { \
     LNAME##TextDataInvalidate(true); } ); \
  } \
\
  const QFont &LNAME##TextFont() const { return LNAME##TextData_.font; } \
  void set##UNAME##TextFont(const QFont &f) { \
    CQChartsUtil::testAndSet(LNAME##TextData_.font, f, [&]() { \
     LNAME##TextDataInvalidate(); } ); \
  } \
\
  const CQChartsColor &LNAME##TextColor() const { return LNAME##TextData_.color; } \
  void set##UNAME##TextColor(const CQChartsColor &c) { \
    CQChartsUtil::testAndSet(LNAME##TextData_.color, c, [&]() { \
     LNAME##TextDataInvalidate(); } ); \
  } \
\
  double LNAME##TextAlpha() const { return LNAME##TextData_.alpha; } \
  void set##UNAME##TextAlpha(double a) { \
    CQChartsUtil::testAndSet(LNAME##TextData_.alpha, a, [&]() { \
     LNAME##TextDataInvalidate(); } ); \
  } \
\
  double LNAME##TextAngle() const { return LNAME##TextData_.angle; } \
  void set##UNAME##TextAngle(double a) { \
    CQChartsUtil::testAndSet(LNAME##TextData_.angle, a, [&]() { \
     LNAME##TextDataInvalidate(); } ); \
  } \
\
  QColor interp##UNAME##TextColor(int i, int n) const { \
    return LNAME##TextDataObj_->charts()->interpColor(LNAME##TextColor(), i, n); \
  } \
\
  bool is##UNAME##TextContrast() const { return LNAME##TextData_.contrast; } \
  void set##UNAME##TextContrast(bool b) { \
    CQChartsUtil::testAndSet(LNAME##TextData_.contrast, b, [&]() { \
     LNAME##TextDataInvalidate(); } ); \
  } \
\
  const Qt::Alignment &LNAME##TextAlign() const { return LNAME##TextData_.align; } \
  void set##UNAME##TextAlign(const Qt::Alignment &a) { \
    CQChartsUtil::testAndSet(LNAME##TextData_.align, a, [&]() { \
     LNAME##TextDataInvalidate(); } ); \
  } \
\
  bool is##UNAME##TextFormatted() const { return LNAME##TextData_.formatted; } \
  void set##UNAME##TextFormatted(bool b) { \
    CQChartsUtil::testAndSet(LNAME##TextData_.formatted, b, [&]() { \
     LNAME##TextDataInvalidate(); } ); \
  } \
\
  bool is##UNAME##TextScaled() const { return LNAME##TextData_.scaled; } \
  void set##UNAME##TextScaled(bool b) { \
    CQChartsUtil::testAndSet(LNAME##TextData_.scaled, b, [&]() { \
     LNAME##TextDataInvalidate(); } ); \
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

  bool isBorder() const { return strokeData_.visible; }
  void setBorder(bool b) {
    CQChartsUtil::testAndSet(strokeData_.visible, b, [&]() { strokeDataInvalidate(); } );
  }

  const CQChartsColor &borderColor() const { return strokeData_.color; }
  void setBorderColor(const CQChartsColor &c) {
    CQChartsUtil::testAndSet(strokeData_.color, c, [&]() { strokeDataInvalidate(); } );
  }

  double borderAlpha() const { return strokeData_.alpha; }
  void setBorderAlpha(double a) {
    CQChartsUtil::testAndSet(strokeData_.alpha, a, [&]() { strokeDataInvalidate(); } );
  }

  const CQChartsLength &borderWidth() const { return strokeData_.width; }
  void setBorderWidth(const CQChartsLength &l) {
    CQChartsUtil::testAndSet(strokeData_.width, l, [&]() { strokeDataInvalidate(); } );
  }

  const CQChartsLineDash &borderDash() const { return strokeData_.dash; }
  void setBorderDash(const CQChartsLineDash &d) {
    CQChartsUtil::testAndSet(strokeData_.dash, d, [&]() { strokeDataInvalidate(); } );
  }

  const CQChartsLength &cornerSize() const { return strokeData_.cornerSize; }
  void setCornerSize(const CQChartsLength &l) {
    CQChartsUtil::testAndSet(strokeData_.cornerSize, l, [&]() { strokeDataInvalidate(); } );
  }

  QColor interpBorderColor(int i, int n) const {
    return strokeDataObj_->charts()->interpColor(borderColor(), i, n);
  }

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
Q_PROPERTY(bool             border      READ isBorder    WRITE setBorder     ) \
Q_PROPERTY(CQChartsColor    borderColor READ borderColor WRITE setBorderColor) \
Q_PROPERTY(double           borderAlpha READ borderAlpha WRITE setBorderAlpha) \
Q_PROPERTY(CQChartsLength   borderWidth READ borderWidth WRITE setBorderWidth) \
Q_PROPERTY(CQChartsLineDash borderDash  READ borderDash  WRITE setBorderDash ) \
Q_PROPERTY(CQChartsLength   cornerSize  READ cornerSize  WRITE setCornerSize ) \
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

  bool isBorder() const { return shapeData_.border.visible; }
  void setBorder(bool b) {
    CQChartsUtil::testAndSet(shapeData_.border.visible, b, [&]() { shapeDataInvalidate(); } );
  }

  const CQChartsColor &borderColor() const { return shapeData_.border.color; }
  void setBorderColor(const CQChartsColor &c) {
    CQChartsUtil::testAndSet(shapeData_.border.color, c, [&]() { shapeDataInvalidate(); } );
  }

  double borderAlpha() const { return shapeData_.border.alpha; }
  void setBorderAlpha(double a) {
    CQChartsUtil::testAndSet(shapeData_.border.alpha, a, [&]() { shapeDataInvalidate(); } );
  }

  const CQChartsLength &borderWidth() const { return shapeData_.border.width; }
  void setBorderWidth(const CQChartsLength &l) {
    CQChartsUtil::testAndSet(shapeData_.border.width, l, [&]() { shapeDataInvalidate(); } );
  }

  const CQChartsLineDash &borderDash() const { return shapeData_.border.dash; }
  void setBorderDash(const CQChartsLineDash &d) {
    CQChartsUtil::testAndSet(shapeData_.border.dash, d, [&]() { shapeDataInvalidate(); } );
  }

  const CQChartsLength &cornerSize() const { return shapeData_.border.cornerSize; }
  void setCornerSize(const CQChartsLength &l) {
    CQChartsUtil::testAndSet(shapeData_.border.cornerSize, l, [&]() { shapeDataInvalidate(); } );
  }

  QColor interpBorderColor(int i, int n) const {
    return shapeDataObj_->charts()->interpColor(borderColor(), i, n);
  }

  //---

  bool isFilled() const { return shapeData_.background.visible; }
  void setFilled(bool b) {
    CQChartsUtil::testAndSet(shapeData_.background.visible, b, [&]() { shapeDataInvalidate(); } );
  }

  const CQChartsColor &fillColor() const { return shapeData_.background.color; }
  void setFillColor(const CQChartsColor &c) {
    CQChartsUtil::testAndSet(shapeData_.background.color, c, [&]() { shapeDataInvalidate(); } );
  }

  double fillAlpha() const { return shapeData_.background.alpha; }
  void setFillAlpha(double a) {
    CQChartsUtil::testAndSet(shapeData_.background.alpha, a, [&]() { shapeDataInvalidate(); } );
  }

  const CQChartsFillPattern &fillPattern() const { return shapeData_.background.pattern; }
  void setFillPattern(const CQChartsFillPattern &p) {
    CQChartsUtil::testAndSet(shapeData_.background.pattern, p, [&]() { shapeDataInvalidate(); } );
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
  bool is##UNAME##Border() const { return LNAME##ShapeData_.border.visible; } \
  void set##UNAME##Border(bool b) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.border.visible, b, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  const CQChartsColor &LNAME##BorderColor() const { return LNAME##ShapeData_.border.color; } \
  void set##UNAME##BorderColor(const CQChartsColor &c) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.border.color, c, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  double LNAME##BorderAlpha() const { return LNAME##ShapeData_.border.alpha; } \
  void set##UNAME##BorderAlpha(double a) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.border.alpha, a, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  const CQChartsLength &LNAME##BorderWidth() const { return LNAME##ShapeData_.border.width; } \
  void set##UNAME##BorderWidth(const CQChartsLength &l) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.border.width, l, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  const CQChartsLineDash &LNAME##BorderDash() const { return LNAME##ShapeData_.border.dash; } \
  void set##UNAME##BorderDash(const CQChartsLineDash &d) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.border.dash, d, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  const CQChartsLength &LNAME##CornerSize() const { \
    return LNAME##ShapeData_.border.cornerSize; } \
  void set##UNAME##CornerSize(const CQChartsLength &l) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.border.cornerSize, l, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  QColor interp##UNAME##BorderColor(int i, int n) const { \
    return LNAME##ShapeDataObj_->charts()->interpColor(LNAME##BorderColor(), i, n); \
  } \
\
  bool is##UNAME##Filled() const { return LNAME##ShapeData_.background.visible; } \
  void set##UNAME##Filled(bool b) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.background.visible, b, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  const CQChartsColor &LNAME##FillColor() const { return LNAME##ShapeData_.background.color; } \
  void set##UNAME##FillColor(const CQChartsColor &c) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.background.color, c, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  double LNAME##FillAlpha() const { return LNAME##ShapeData_.background.alpha; } \
  void set##UNAME##FillAlpha(double a) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.background.alpha, a, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  const CQChartsFillPattern &LNAME##FillPattern() const { \
    return LNAME##ShapeData_.background.pattern; } \
  void set##UNAME##FillPattern(const CQChartsFillPattern &p) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.background.pattern, p, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  QColor interp##UNAME##FillColor(int i, int n) const { \
    return LNAME##ShapeDataObj_->charts()->interpColor(LNAME##FillColor(), i, n); \
  } \
\
  void setShapeData(const CQChartsShapeData &data) { \
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
