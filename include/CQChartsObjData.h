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
Q_PROPERTY(CQChartsLineDash linesDash  READ linesDash  WRITE setLinesDash ) \
Q_PROPERTY(CQChartsLineCap  linesCap   READ linesCap   WRITE setLinesCap  ) \
Q_PROPERTY(CQChartsLineJoin linesJoin  READ linesJoin  WRITE setLinesJoin )

/*!
 * \brief Object line data
 * \ingroup Charts
 */
template<class OBJ>
class CQChartsObjLineData {
 public:
  using Color     = CQChartsColor;
  using Alpha     = CQChartsAlpha;
  using Length    = CQChartsLength;
  using LineDash  = CQChartsLineDash;
  using LineCap   = CQChartsLineCap;
  using LineJoin  = CQChartsLineJoin;
  using PenBrush  = CQChartsPenBrush;
  using PenData   = CQChartsPenData;
  using BrushData = CQChartsBrushData;
  using ColorInd  = CQChartsUtil::ColorInd;
  using LineData  = CQChartsLineData;

 public:
  using Invalidator = CQChartsInvalidator;

 public:
  CQChartsObjLineData(OBJ *obj) :
   lineDataObj_(obj), invalidator_(obj) {
  }

  CQChartsObjLineData(OBJ *obj, Invalidator *invalidator) :
   lineDataObj_(obj), invalidator_(obj), pinvalidator_(invalidator) {
  }

  //---

  bool isReloadObj() const { return reloadObj_; }
  void setReloadObj(bool b) { reloadObj_ = b; }

  bool isLines() const { return lineData_.isVisible(); }
  void setLines(bool b) {
    if (b != lineData_.isVisible()) {
      lineData_.setVisible(b); lineDataInvalidate(/*reload*/isReloadObj()); }
  }

  const Color &linesColor() const { return lineData_.color(); }
  void setLinesColor(const Color &c) {
    if (c != lineData_.color()) {
      lineData_.setColor(c); lineDataInvalidate(); }
  }

  QColor interpLinesColor(const ColorInd &ind) const {
    return CQChartsInterpolator(lineDataObj_).interpColor(linesColor(), ind);
  }

  const Alpha &linesAlpha() const { return lineData_.alpha(); }
  void setLinesAlpha(const Alpha &a) {
    if (a != lineData_.alpha()) {
      lineData_.setAlpha(a); lineDataInvalidate(); }
  }

  const Length &linesWidth() const { return lineData_.width(); }
  void setLinesWidth(const Length &l) {
    if (l != lineData_.width()) {
      lineData_.setWidth(l); lineDataInvalidate(/*reload*/isReloadObj()); }
   }

  const LineDash &linesDash() const { return lineData_.dash(); }
  void setLinesDash(const LineDash &d) {
    if (d != lineData_.dash()) {
      lineData_.setDash(d); lineDataInvalidate(); }
  }

  const LineCap &linesCap() const { return lineData_.lineCap(); }
  void setLinesCap(const LineCap &c) {
    if (c != lineData_.lineCap()) {
      lineData_.setLineCap(c); lineDataInvalidate(); }
  }

  const LineJoin &linesJoin() const { return lineData_.lineJoin(); }
  void setLinesJoin(const LineJoin &j) {
    if (j != lineData_.lineJoin()) {
      lineData_.setLineJoin(j); lineDataInvalidate(); }
  }

  void setLineDataPen(QPen &pen, const ColorInd &ind) const {
    QColor lc = interpLinesColor(ind);

    PenBrush penBrush;
    lineDataObj_->setPen(penBrush, lineDataPenData(lc));
    pen = penBrush.pen;
  }

  PenData lineDataPenData(const QColor &lc, const Alpha &alpha=Alpha()) const {
    auto strokeAlpha = (! alpha.isSet() ? linesAlpha() : alpha);
    return PenData(isLines(), lc, strokeAlpha, linesWidth(), linesDash(),
                   linesCap(), linesJoin());
  }

  BrushData lineDataBrushData(const QColor &lc, const Alpha &alpha=Alpha()) const {
    auto fillAlpha = (! alpha.isSet() ? linesAlpha() : alpha);
    return BrushData(true, lc, fillAlpha);
  }

  //---

  const LineData &lineData() const { return lineData_; }

  void setLineData(const LineData &data) {
    lineData_ = data; lineDataInvalidate(/*reload*/isReloadObj());
  };

 private:
  virtual void lineDataInvalidate(bool reload=false) {
    pinvalidator_ ? pinvalidator_->invalidate(reload) : invalidator_.invalidate(reload);
  }

 private:
  OBJ* lineDataObj_ { nullptr };
  bool reloadObj_   { true };

 protected:
  LineData     lineData_;
  Invalidator  invalidator_;
  Invalidator* pinvalidator_ { nullptr };
};

//------

#define CQCHARTS_NAMED_LINE_DATA_PROPERTIES(UNAME, LNAME) \
Q_PROPERTY(CQChartsLineData LNAME##LineData READ LNAME##LineData WRITE set##UNAME##LineData) \
\
Q_PROPERTY(bool             LNAME##Lines      READ is##UNAME##Lines  WRITE set##UNAME##Lines     ) \
Q_PROPERTY(CQChartsColor    LNAME##LinesColor READ LNAME##LinesColor WRITE set##UNAME##LinesColor) \
Q_PROPERTY(CQChartsAlpha    LNAME##LinesAlpha READ LNAME##LinesAlpha WRITE set##UNAME##LinesAlpha) \
Q_PROPERTY(CQChartsLength   LNAME##LinesWidth READ LNAME##LinesWidth WRITE set##UNAME##LinesWidth) \
Q_PROPERTY(CQChartsLineDash LNAME##LinesDash  READ LNAME##LinesDash  WRITE set##UNAME##LinesDash ) \
Q_PROPERTY(CQChartsLineCap  LNAME##LinesCap   READ LNAME##LinesCap   WRITE set##UNAME##LinesCap  ) \
Q_PROPERTY(CQChartsLineJoin LNAME##LinesJoin  READ LNAME##LinesJoin  WRITE set##UNAME##LinesJoin )

/*!
 * \brief Object named line data
 * \ingroup Charts
 */
#define CQCHARTS_NAMED_LINE_DATA(UNAME, LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##LineData { \
 public: \
  using Color     = CQChartsColor; \
  using Alpha     = CQChartsAlpha; \
  using Length    = CQChartsLength; \
  using LineDash  = CQChartsLineDash; \
  using LineCap   = CQChartsLineCap; \
  using LineJoin  = CQChartsLineJoin; \
  using PenBrush  = CQChartsPenBrush; \
  using PenData   = CQChartsPenData; \
  using BrushData = CQChartsBrushData; \
  using ColorInd  = CQChartsUtil::ColorInd; \
  using LineData  = CQChartsLineData; \
\
 public: \
  using Invalidator = CQChartsInvalidator; \
\
 public: \
  CQChartsObj##UNAME##LineData(OBJ *obj) : \
   LNAME##LineDataObj_(obj), LNAME##Invalidator_(obj) { \
  } \
\
  CQChartsObj##UNAME##LineData(OBJ *obj, Invalidator *invalidator) : \
   LNAME##LineDataObj_(obj), LNAME##Invalidator_(obj), LNAME##PInvalidator_(invalidator) { \
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
  const Color &LNAME##LinesColor() const { return LNAME##LineData_.color(); } \
  void set##UNAME##LinesColor(const Color &c) { \
    if (c != LNAME##LineData_.color()) { \
      LNAME##LineData_.setColor(c); LNAME##LineDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##LinesColor(const ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##LineDataObj_).interpColor(LNAME##LinesColor(), ind); \
  } \
\
  const Alpha &LNAME##LinesAlpha() const { return LNAME##LineData_.alpha(); } \
  void set##UNAME##LinesAlpha(const Alpha &a) { \
    if (a != LNAME##LineData_.alpha()) { \
      LNAME##LineData_.setAlpha(a); LNAME##LineDataInvalidate(); } \
  } \
\
  const Length &LNAME##LinesWidth() const { return LNAME##LineData_.width(); } \
  void set##UNAME##LinesWidth(const Length &l) { \
    if (l != LNAME##LineData_.width()) { \
      LNAME##LineData_.setWidth(l); LNAME##LineDataInvalidate(); } \
   } \
\
  const LineDash &LNAME##LinesDash() const { return LNAME##LineData_.dash(); } \
  void set##UNAME##LinesDash(const LineDash &d) { \
    if (d != LNAME##LineData_.dash()) { \
      LNAME##LineData_.setDash(d); LNAME##LineDataInvalidate(); } \
  } \
\
  const LineCap &LNAME##LinesCap() const { return LNAME##LineData_.lineCap(); } \
  void set##UNAME##LinesCap(const LineCap &c) { \
    if (c != LNAME##LineData_.lineCap()) { \
      LNAME##LineData_.setLineCap(c); LNAME##LineDataInvalidate(); } \
  } \
\
  const LineJoin &LNAME##LinesJoin() const { return LNAME##LineData_.lineJoin(); } \
  void set##UNAME##LinesJoin(const LineJoin &j) { \
    if (j != LNAME##LineData_.lineJoin()) { \
      LNAME##LineData_.setLineJoin(j); LNAME##LineDataInvalidate(); } \
  } \
\
  void set##UNAME##LineDataPen(QPen &pen, const ColorInd &ind) const { \
    QColor lc = interp##UNAME##LinesColor(ind); \
\
    PenBrush penBrush; \
    LNAME##LineDataObj_->setPen(penBrush, LNAME##LineDataPenData(lc)); \
    pen = penBrush.pen; \
  } \
\
  PenData LNAME##LineDataPenData(const QColor &lc, const Alpha &alpha=Alpha()) const { \
    auto strokeAlpha = (! alpha.isSet() ? LNAME##LinesAlpha() : alpha); \
    return PenData(is##UNAME##Lines(), lc, strokeAlpha, LNAME##LinesWidth(), \
                   LNAME##LinesDash(), LNAME##LinesCap(), LNAME##LinesJoin()); \
  } \
\
  BrushData LNAME##LineDataBrushData(const QColor &lc, const Alpha &alpha=Alpha()) const { \
    auto fillAlpha = (! alpha.isSet() ? LNAME##LinesAlpha() : alpha); \
    return BrushData(true, lc, fillAlpha); \
  } \
\
  const LineData &LNAME##LineData() const { return LNAME##LineData_; } \
\
  void set##UNAME##LineData(const LineData &data) { \
    LNAME##LineData_ = data; LNAME##LineDataInvalidate(); \
  } \
\
 private: \
  virtual void LNAME##LineDataInvalidate(bool reload=false) { \
    LNAME##PInvalidator_ ? LNAME##PInvalidator_->invalidate(reload) : \
                           LNAME##Invalidator_.invalidate(reload); \
  } \
\
 private: \
  OBJ* LNAME##LineDataObj_ { nullptr }; \
\
 protected: \
  LineData     LNAME##LineData_; \
  bool         LNAME##ReloadObj_ { true }; \
  Invalidator  LNAME##Invalidator_; \
  Invalidator* LNAME##PInvalidator_ { nullptr }; \
};

//------

#define CQCHARTS_POINT_DATA_PROPERTIES \
Q_PROPERTY(CQChartsSymbolData symbolData READ symbolData WRITE setSymbolData) \
\
Q_PROPERTY(bool                points \
           READ isPoints          WRITE setPoints           ) \
Q_PROPERTY(CQChartsSymbol      symbol \
           READ symbol            WRITE setSymbol           ) \
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
Q_PROPERTY(CQChartsLineCap     symbolStrokeCap \
           READ symbolStrokeCap   WRITE setSymbolStrokeCap  ) \
Q_PROPERTY(CQChartsLineJoin    symbolStrokeJoin \
           READ symbolStrokeJoin  WRITE setSymbolStrokeJoin ) \
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
  using Symbol      = CQChartsSymbol;
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using Length      = CQChartsLength;
  using LineDash    = CQChartsLineDash;
  using LineCap     = CQChartsLineCap;
  using LineJoin    = CQChartsLineJoin;
  using FillPattern = CQChartsFillPattern;
  using PenBrush    = CQChartsPenBrush;
  using PenData     = CQChartsPenData;
  using BrushData   = CQChartsBrushData;
  using ColorInd    = CQChartsUtil::ColorInd;
  using SymbolData  = CQChartsSymbolData;

 public:
  using Invalidator = CQChartsInvalidator;

 public:
  CQChartsObjPointData(OBJ *obj) :
   pointDataObj_(obj), invalidator_(obj) {
  }

  CQChartsObjPointData(OBJ *obj, Invalidator *invalidator) :
   pointDataObj_(obj), invalidator_(obj), pinvalidator_(invalidator) {
  }

  //---

  bool isReloadObj() const { return reloadObj_; }
  void setReloadObj(bool b) { reloadObj_ = b; }

  bool isPoints() const { return pointData_.isVisible(); }
  void setPoints(bool b) {
    if (b != pointData_.isVisible()) {
      pointData_.setVisible(b); pointDataInvalidate(isReloadObj()); }
  }

  const Symbol &symbol() const { return pointData_.symbol(); }
  void setSymbol(const Symbol &s) {
    if (s != pointData_.symbol()) {
      pointData_.setSymbol(s); pointDataInvalidate(); }
  }

  const Length &symbolSize() const { return pointData_.size(); }
  void setSymbolSize(const Length &l) {
    if (l != pointData_.size()) {
      pointData_.setSize(l); pointDataInvalidate(); }
  }

  bool isSymbolStroked() const { return pointData_.stroke().isVisible(); }
  void setSymbolStroked(bool b) {
    if (b != pointData_.stroke().isVisible()) {
      pointData_.stroke().setVisible(b); pointDataInvalidate(); }
  }

  const Color &symbolStrokeColor() const { return pointData_.stroke().color(); }
  void setSymbolStrokeColor(const Color &c) {
    if (c != pointData_.stroke().color()) {
      pointData_.stroke().setColor(c); pointDataInvalidate(); }
  }

  QColor interpSymbolStrokeColor(const ColorInd &ind) const {
    return CQChartsInterpolator(pointDataObj_).interpColor(symbolStrokeColor(), ind);
  }

  const Alpha &symbolStrokeAlpha() const { return pointData_.stroke().alpha(); }
  void setSymbolStrokeAlpha(const Alpha &a) {
    if (a != pointData_.stroke().alpha()) {
      pointData_.stroke().setAlpha(a); pointDataInvalidate(); }
  }

  const Length &symbolStrokeWidth() const { return pointData_.stroke().width(); }
  void setSymbolStrokeWidth(const Length &l) {
    if (l != pointData_.stroke().width()) {
      pointData_.stroke().setWidth(l); pointDataInvalidate(); }
  }

  const LineDash &symbolStrokeDash() const { return pointData_.stroke().dash(); }
  void setSymbolStrokeDash(const LineDash &d) {
    if (d != pointData_.stroke().dash()) {
      pointData_.stroke().setDash(d); pointDataInvalidate(); }
  }

  const LineCap &symbolStrokeCap() const { return pointData_.stroke().lineCap(); }
  void setSymbolStrokeCap(const LineCap &c) {
    if (c != pointData_.stroke().lineCap()) {
      pointData_.stroke().setLineCap(c); pointDataInvalidate(); }
  }

  const LineJoin &symbolStrokeJoin() const { return pointData_.stroke().lineJoin(); }
  void setSymbolStrokeJoin(const LineJoin &j) {
    if (j != pointData_.stroke().lineJoin()) {
      pointData_.stroke().setLineJoin(j); pointDataInvalidate(); }
  }

  bool isSymbolFilled() const { return pointData_.fill().isVisible(); }
  void setSymbolFilled(bool b) {
    if (b != pointData_.fill().isVisible()) {
      pointData_.fill().setVisible(b); pointDataInvalidate(); }
  }

  const Color &symbolFillColor() const { return pointData_.fill().color(); }
  void setSymbolFillColor(const Color &c) {
    if (c != pointData_.fill().color()) {
      pointData_.fill().setColor(c); pointDataInvalidate(); }
  }

  QColor interpSymbolFillColor(const ColorInd &ind) const {
    return CQChartsInterpolator(pointDataObj_).interpColor(symbolFillColor(), ind);
  }

  const Alpha &symbolFillAlpha() const { return pointData_.fill().alpha(); }
  void setSymbolFillAlpha(const Alpha &a) {
    if (a != pointData_.fill().alpha()) {
      pointData_.fill().setAlpha(a); pointDataInvalidate(); }
  }

  const FillPattern &symbolFillPattern() const { return pointData_.fill().pattern(); }
  void setSymbolFillPattern(const FillPattern &p) {
    if (p != pointData_.fill().pattern()) {
      pointData_.fill().setPattern(p); pointDataInvalidate(); }
  }

  //---

  void setSymbolPenBrush(PenBrush &penBrush, const ColorInd &ind) const {
    pointDataObj_->setPenBrush(penBrush,
      symbolPenData  (interpSymbolStrokeColor(ind)),
      symbolBrushData(interpSymbolFillColor  (ind)));
  }

  PenData symbolPenData(const QColor &c, const Alpha &alpha=Alpha()) const {
    auto strokeAlpha = (! alpha.isSet() ? symbolStrokeAlpha() : alpha);
    return PenData(isSymbolStroked(), c, strokeAlpha, symbolStrokeWidth(), symbolStrokeDash(),
                   symbolStrokeCap(), symbolStrokeJoin());
  }

  BrushData symbolBrushData(const QColor &c, const Alpha &alpha=Alpha()) const {
    auto fillAlpha = (! alpha.isSet() ? symbolFillAlpha() : alpha);
    return BrushData(isSymbolFilled(), c, fillAlpha, symbolFillPattern());
  }

  //---

  const SymbolData &symbolData() const { return pointData_; }

  void setSymbolData(const SymbolData &data) {
    pointData_ = data; pointDataInvalidate();
  };

 private:
  virtual void pointDataInvalidate(bool reload=false) {
    pinvalidator_ ? pinvalidator_->invalidate(reload) : invalidator_.invalidate(reload);
  }

 private:
  OBJ* pointDataObj_ { nullptr };

 protected:
  SymbolData   pointData_;
  bool         reloadObj_ { true };
  Invalidator  invalidator_;
  Invalidator* pinvalidator_ { nullptr };
};

//------

#define CQCHARTS_NAMED_POINT_DATA_PROPERTIES(UNAME, LNAME) \
Q_PROPERTY(CQChartsSymbolData LNAME##SymbolData \
           READ LNAME##SymbolData WRITE set##UNAME##SymbolData) \
\
Q_PROPERTY(bool                LNAME##Points \
           READ is##UNAME##Points        WRITE set##UNAME##Points           ) \
Q_PROPERTY(CQChartsSymbol      LNAME##Symbol \
           READ LNAME##Symbol            WRITE set##UNAME##Symbol           ) \
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
Q_PROPERTY(CQChartsLineCap     LNAME##SymbolStrokeCap \
           READ LNAME##SymbolStrokeCap   WRITE set##UNAME##SymbolStrokeCap  ) \
Q_PROPERTY(CQChartsLineJoin    LNAME##SymbolStrokeJoin \
           READ LNAME##SymbolStrokeJoin  WRITE set##UNAME##SymbolStrokeJoin ) \
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
  using Symbol      = CQChartsSymbol; \
  using Color       = CQChartsColor; \
  using Alpha       = CQChartsAlpha; \
  using Length      = CQChartsLength; \
  using LineDash    = CQChartsLineDash; \
  using LineCap     = CQChartsLineCap; \
  using LineJoin    = CQChartsLineJoin; \
  using FillPattern = CQChartsFillPattern; \
  using PenBrush    = CQChartsPenBrush; \
  using PenData     = CQChartsPenData; \
  using BrushData   = CQChartsBrushData; \
  using ColorInd    = CQChartsUtil::ColorInd; \
  using SymbolData  = CQChartsSymbolData; \
\
 public: \
  using Invalidator = CQChartsInvalidator; \
\
 public: \
  CQChartsObj##UNAME##PointData(OBJ *obj) : \
   LNAME##PointDataObj_(obj), LNAME##Invalidator_(obj) { \
  } \
\
  CQChartsObj##UNAME##PointData(OBJ *obj, Invalidator *invalidator) : \
   LNAME##PointDataObj_(obj), LNAME##Invalidator_(obj), LNAME##PInvalidator_(invalidator) { \
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
  const Symbol &LNAME##Symbol() const { return LNAME##PointData_.symbol(); } \
  void set##UNAME##Symbol(const Symbol &s) { \
    if (s != LNAME##PointData_.symbol()) { \
      LNAME##PointData_.setSymbol(s); LNAME##PointDataInvalidate(); } \
  } \
\
  const Length &LNAME##SymbolSize() const { return LNAME##PointData_.size(); } \
  void set##UNAME##SymbolSize(const Length &s) { \
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
  const Color &LNAME##SymbolStrokeColor() const { return LNAME##PointData_.stroke().color(); } \
  void set##UNAME##SymbolStrokeColor(const Color &c) { \
    if (c != LNAME##PointData_.stroke().color()) { \
      LNAME##PointData_.stroke().setColor(c); LNAME##PointDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##SymbolStrokeColor(const ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##PointDataObj_). \
      interpColor(LNAME##SymbolStrokeColor(), ind); \
  } \
\
  const Alpha &LNAME##SymbolStrokeAlpha() const { return LNAME##PointData_.stroke().alpha(); } \
  void set##UNAME##SymbolStrokeAlpha(const Alpha &a) { \
    if (a != LNAME##PointData_.stroke().alpha()) { \
      LNAME##PointData_.stroke().setAlpha(a); LNAME##PointDataInvalidate(); } \
  } \
\
  const Length &LNAME##SymbolStrokeWidth() const { return LNAME##PointData_.stroke().width(); } \
  void set##UNAME##SymbolStrokeWidth(const Length &l) { \
    if (l != LNAME##PointData_.stroke().width()) { \
      LNAME##PointData_.stroke().setWidth(l); LNAME##PointDataInvalidate(); } \
  } \
\
  const LineDash &LNAME##SymbolStrokeDash() const { return LNAME##PointData_.stroke().dash(); } \
  void set##UNAME##SymbolStrokeDash(const LineDash &d) { \
    if (d != LNAME##PointData_.stroke().dash()) { \
      LNAME##PointData_.stroke().setDash(d); LNAME##PointDataInvalidate(); } \
  } \
\
  const LineCap &LNAME##SymbolStrokeCap() const { return LNAME##PointData_.stroke().lineCap(); } \
  void set##UNAME##SymbolStrokeCap(const LineCap &c) { \
    if (c != LNAME##PointData_.stroke().lineCap()) { \
      LNAME##PointData_.stroke().setLineCap(c); LNAME##PointDataInvalidate(); } \
  } \
\
  const LineJoin &LNAME##SymbolStrokeJoin() const { \
    return LNAME##PointData_.stroke().lineJoin(); } \
  void set##UNAME##SymbolStrokeJoin(const LineJoin &j) { \
    if (j != LNAME##PointData_.stroke().lineJoin()) { \
      LNAME##PointData_.stroke().setLineJoin(j); LNAME##PointDataInvalidate(); } \
  } \
\
  bool is##UNAME##SymbolFilled() const { return LNAME##PointData_.fill().isVisible(); } \
  void set##UNAME##SymbolFilled(bool b) { \
    if (b != LNAME##PointData_.fill().isVisible()) { \
      LNAME##PointData_.fill().setVisible(b); \
      LNAME##PointDataInvalidate(is##UNAME##ReloadObj()); } \
  } \
\
  const Color &LNAME##SymbolFillColor() const { return LNAME##PointData_.fill().color(); } \
  void set##UNAME##SymbolFillColor(const Color &c) { \
    if (c != LNAME##PointData_.fill().color()) { \
      LNAME##PointData_.fill().setColor(c); LNAME##PointDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##SymbolFillColor(const ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##PointDataObj_).interpColor(LNAME##SymbolFillColor(), ind); \
  } \
\
  const Alpha &LNAME##SymbolFillAlpha() const { return LNAME##PointData_.fill().alpha(); } \
  void set##UNAME##SymbolFillAlpha(const Alpha &a) { \
    if (a != LNAME##PointData_.fill().alpha()) { \
      LNAME##PointData_.fill().setAlpha(a); LNAME##PointDataInvalidate(); } \
  } \
\
  const FillPattern &LNAME##SymbolFillPattern() const { \
    return LNAME##PointData_.fill().pattern(); } \
  void set##UNAME##SymbolFillPattern(const FillPattern &p) { \
    if (p != LNAME##PointData_.fill().pattern()) { \
      LNAME##PointData_.fill().setPattern(p); LNAME##PointDataInvalidate(); } \
  } \
\
  void set##UNAME##SymbolPenBrush(PenBrush &penBrush, const ColorInd &ind) const { \
    LNAME##PointDataObj_->setPenBrush(penBrush, \
      PenData(is##UNAME##SymbolStroked(), interp##UNAME##SymbolStrokeColor(ind), \
              LNAME##SymbolStrokeAlpha(), LNAME##SymbolStrokeWidth(), LNAME##SymbolStrokeDash(), \
              LNAME##SymbolStrokeCap(), LNAME##SymbolStrokeJoin()), \
      BrushData(is##UNAME##SymbolFilled(), interp##UNAME##SymbolFillColor(ind), \
                LNAME##SymbolFillAlpha(), LNAME##SymbolFillPattern())); \
  } \
\
  const SymbolData &LNAME##SymbolData() const { return LNAME##PointData_; } \
\
  void set##UNAME##SymbolData(const SymbolData &data) { \
    LNAME##PointData_ = data; LNAME##PointDataInvalidate(is##UNAME##ReloadObj()); \
  } \
\
 private: \
  virtual void LNAME##PointDataInvalidate(bool reload=false) { \
    LNAME##PInvalidator_ ? LNAME##PInvalidator_->invalidate(reload) : \
                           LNAME##Invalidator_.invalidate(reload); \
  } \
\
 private: \
  OBJ* LNAME##PointDataObj_ { nullptr }; \
\
 protected: \
  SymbolData   LNAME##PointData_; \
  bool         LNAME##ReloadObj_ { true }; \
  Invalidator  LNAME##Invalidator_; \
  Invalidator* LNAME##PInvalidator_ { nullptr }; \
};

//------

enum class CQChartsFillDataTypes {
  VISIBLE = (1<<0),
  COLOR   = (1<<1),
  ALPHA   = (1<<2),
  PATTERN = (1<<3),

  NONE     = 0,
  STANDARD = (COLOR | ALPHA | PATTERN),
  NO_COLOR = (ALPHA | PATTERN),
  ALL      = (VISIBLE | COLOR | ALPHA | PATTERN)
};

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
  using Color       = CQChartsColor; \
  using Alpha       = CQChartsAlpha; \
  using FillPattern = CQChartsFillPattern; \
  using ColorInd    = CQChartsUtil::ColorInd; \
  using FillData    = CQChartsFillData; \
\
 public: \
  using Invalidator = CQChartsInvalidator; \
\
 public: \
  CQChartsObj##UNAME##FillData(OBJ *obj) : \
   LNAME##FillDataObj_(obj), LNAME##Invalidator_(obj) { \
  } \
\
  CQChartsObj##UNAME##FillData(OBJ *obj, Invalidator *invalidator) : \
   LNAME##FillDataObj_(obj), LNAME##Invalidator_(obj), LNAME##PInvalidator_(invalidator) { \
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
  const Color &LNAME##FillColor() const { return LNAME##FillData_.color(); } \
  void set##UNAME##FillColor(const Color &c) { \
    if (c != LNAME##FillData_.color()) { \
      LNAME##FillData_.setColor(c); LNAME##FillDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##FillColor(const ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##FillDataObj_).interpColor(LNAME##FillColor(), ind); \
  } \
\
  const Alpha &LNAME##FillAlpha() const { return LNAME##FillData_.alpha(); } \
  void set##UNAME##FillAlpha(const Alpha &a) { \
    if (a != LNAME##FillData_.alpha()) { \
      LNAME##FillData_.setAlpha(a); LNAME##FillDataInvalidate(); } \
  } \
\
  const FillPattern &LNAME##FillPattern() const { return LNAME##FillData_.pattern(); } \
  void set##UNAME##FillPattern(const FillPattern &p) { \
    if (p != LNAME##FillData_.pattern()) { \
      LNAME##FillData_.setPattern(p); LNAME##FillDataInvalidate(); } \
  } \
\
  const FillData &LNAME##FillData() const { return LNAME##FillData_; } \
\
  void set##UNAME##FillData(const FillData &data) { \
    LNAME##FillData_ = data; LNAME##FillDataInvalidate(is##UNAME##ReloadObj()); \
  } \
\
 private: \
  virtual void LNAME##FillDataInvalidate(bool reload=false) { \
    LNAME##PInvalidator_ ? LNAME##PInvalidator_->invalidate(reload) : \
                           LNAME##Invalidator_.invalidate(reload); \
  } \
\
 private: \
  OBJ* LNAME##FillDataObj_ { nullptr }; \
\
 protected: \
  FillData     LNAME##FillData_; \
  bool         LNAME##ReloadObj_ { true }; \
  Invalidator  LNAME##Invalidator_; \
  Invalidator* LNAME##PInvalidator_ { nullptr }; \
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
  using Color    = CQChartsColor;
  using Alpha    = CQChartsAlpha;
  using Font     = CQChartsFont;
  using Angle    = CQChartsAngle;
  using Length   = CQChartsLength;
  using ColorInd = CQChartsUtil::ColorInd;
  using TextData = CQChartsTextData;

 public:
  using Invalidator = CQChartsInvalidator;

 public:
  CQChartsObjTextData(OBJ *obj) :
   textDataObj_(obj), invalidator_(obj) {
  }

  CQChartsObjTextData(OBJ *obj, Invalidator *invalidator) :
   textDataObj_(obj), invalidator_(obj), pinvalidator_(invalidator) {
  }

  //---

  bool isReloadObj() const { return reloadObj_; }
  void setReloadObj(bool b) { reloadObj_ = b; }

  bool isTextVisible() const { return textData_.isVisible(); }
  void setTextVisible(bool b) {
    if (b != textData_.isVisible()) {
      textData_.setVisible(b); textDataInvalidate(isReloadObj()); }
  }

  const Color &textColor() const { return textData_.color(); }
  void setTextColor(const Color &c) {
    if (c != textData_.color()) {
      textData_.setColor(c); textDataInvalidate(); }
  }

  const Alpha& textAlpha() const { return textData_.alpha(); }
  void setTextAlpha(const Alpha &a) {
    if (a != textData_.alpha()) {
      textData_.setAlpha(a); textDataInvalidate(); }
  }

  QColor interpTextColor(const ColorInd &ind) const {
    return CQChartsInterpolator(textDataObj_).interpColor(textColor(), ind);
  }

  const Font &textFont() const { return textData_.font(); }
  void setTextFont(const Font &f) {
    if (f != textData_.font()) {
      textData_.setFont(f); textDataInvalidate(); }
  }

  const Angle &textAngle() const { return textData_.angle(); }
  void setTextAngle(const Angle &a) {
    if (a != textData_.angle()) {
      textData_.setAngle(a); textDataInvalidate(); }
  }

  bool isTextContrast() const { return textData_.isContrast(); }
  void setTextContrast(bool b) {
    if (b != textData_.isContrast()) {
      textData_.setContrast(b); textDataInvalidate(); }
  }

  const Alpha& textContrastAlpha() const { return textData_.contrastAlpha(); }
  void setTextContrastAlpha(const Alpha &a) {
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

  const Length &textClipLength() const { return textData_.clipLength(); }
  void setTextClipLength(const Length &l) {
    if (l != textData_.clipLength()) {
      textData_.setClipLength(l); textDataInvalidate(); }
  }

  const Qt::TextElideMode &textClipElide() const { return textData_.clipElide(); }
  void setTextClipElide(const Qt::TextElideMode &l) {
    if (l != textData_.clipElide()) {
      textData_.setClipElide(l); textDataInvalidate(); }
  }

  //---

  const TextData &textData() const { return textData_; }

  void setTextData(const TextData &data) {
    textData_ = data; textDataInvalidate();
  };

  CQChartsTextOptions textOptions(CQChartsPaintDevice *device=nullptr) const {
    CQChartsTextOptions textOptions;

    textOptions.angle         = textAngle();
    textOptions.contrast      = isTextContrast();
    textOptions.contrastAlpha = textContrastAlpha();
    textOptions.align         = textAlign();
    textOptions.formatted     = isTextFormatted();
    textOptions.scaled        = isTextScaled();
    textOptions.html          = isTextHtml();
    textOptions.clipElide     = textClipElide();

    if (device)
      textOptions.clipLength = CQChartsDrawUtil::lengthPixelWidth(device, textClipLength());

    return textOptions;
  }

 protected:
  virtual void textDataInvalidate(bool reload=false) {
    pinvalidator_ ? pinvalidator_->invalidate(reload) : invalidator_.invalidate(reload);
  }

 private:
  OBJ* textDataObj_ { nullptr };

 protected:
  TextData     textData_;
  bool         reloadObj_ { true };
  Invalidator  invalidator_;
  Invalidator* pinvalidator_ { nullptr };
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
  using Color    = CQChartsColor; \
  using Alpha    = CQChartsAlpha; \
  using Font     = CQChartsFont; \
  using Angle    = CQChartsAngle; \
  using Length   = CQChartsLength; \
  using ColorInd = CQChartsUtil::ColorInd; \
  using TextData = CQChartsTextData; \
\
 public: \
  using Invalidator = CQChartsInvalidator; \
\
 public: \
  CQChartsObj##UNAME##TextData(OBJ *obj) : \
   LNAME##TextDataObj_(obj), LNAME##Invalidator_(obj) { \
  } \
\
  CQChartsObj##UNAME##TextData(OBJ *obj, Invalidator *invalidator) : \
   LNAME##TextDataObj_(obj), LNAME##Invalidator_(obj), LNAME##PInvalidator_(invalidator) { \
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
  const Color &LNAME##TextColor() const { return LNAME##TextData_.color(); } \
  void set##UNAME##TextColor(const Color &c) { \
    if (c != LNAME##TextData_.color()) { \
      LNAME##TextData_.setColor(c); LNAME##TextDataInvalidate(); } \
  } \
\
  const Alpha &LNAME##TextAlpha() const { return LNAME##TextData_.alpha(); } \
  void set##UNAME##TextAlpha(const Alpha &a) { \
    if (a != LNAME##TextData_.alpha()) { \
      LNAME##TextData_.setAlpha(a); LNAME##TextDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##TextColor(const ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##TextDataObj_).interpColor(LNAME##TextColor(), ind); \
  } \
\
  const Font &LNAME##TextFont() const { return LNAME##TextData_.font(); } \
  void set##UNAME##TextFont(const Font &f) { \
    if (f != LNAME##TextData_.font()) { \
      LNAME##TextData_.setFont(f); LNAME##TextDataInvalidate(); } \
  } \
\
  const Angle &LNAME##TextAngle() const { return LNAME##TextData_.angle(); } \
  void set##UNAME##TextAngle(const Angle &a) { \
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
  const Alpha &LNAME##TextContrastAlpha() const { \
    return LNAME##TextData_.contrastAlpha(); } \
  void set##UNAME##TextContrastAlpha(const Alpha &a) { \
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
  const Length &LNAME##TextClipLength() const { return LNAME##TextData_.clipLength(); } \
  void set##UNAME##TextClipLength(const Length &l) { \
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
  const TextData &LNAME##TextData() const { return LNAME##TextData_; } \
\
  void set##UNAME##TextData(const TextData &data) { \
    LNAME##TextData_ = data; LNAME##TextDataInvalidate(); \
  } \
\
  CQChartsTextOptions LNAME##TextOptions(CQChartsPaintDevice *device=nullptr) const { \
    CQChartsTextOptions textOptions; \
\
    textOptions.angle         = LNAME##TextAngle(); \
    textOptions.contrast      = is##UNAME##TextContrast(); \
    textOptions.contrastAlpha = LNAME##TextContrastAlpha(); \
    textOptions.align         = LNAME##TextAlign(); \
    textOptions.formatted     = is##UNAME##TextFormatted(); \
    textOptions.scaled        = is##UNAME##TextScaled(); \
    textOptions.html          = is##UNAME##TextHtml(); \
    textOptions.clipElide     = LNAME##TextClipElide(); \
\
    if (device) \
      textOptions.clipLength = \
        CQChartsDrawUtil::lengthPixelWidth(device, LNAME##TextClipLength()); \
\
    return textOptions; \
  } \
\
 private: \
  virtual void LNAME##TextDataInvalidate(bool reload=false) { \
    LNAME##PInvalidator_ ? LNAME##PInvalidator_->invalidate(reload) : \
                           LNAME##Invalidator_.invalidate(reload); \
  } \
\
 private: \
  OBJ* LNAME##TextDataObj_ { nullptr }; \
\
 protected: \
  TextData     LNAME##TextData_; \
  bool         LNAME##ReloadObj_ { true }; \
  Invalidator  LNAME##Invalidator_; \
  Invalidator* LNAME##PInvalidator_ { nullptr }; \
};

//------

enum class CQChartsStrokeDataTypes {
  VISIBLE = (1<<0),
  COLOR   = (1<<1),
  ALPHA   = (1<<2),
  WIDTH   = (1<<3),
  DASH    = (1<<4),
  CAP     = (1<<5),
  JOIN    = (1<<6),

  NONE     = 0,
  STANDARD = (COLOR | ALPHA | WIDTH | DASH | CAP),
  NO_COLOR = (ALPHA | WIDTH | DASH | CAP),
  ALL      = (VISIBLE | COLOR | ALPHA | WIDTH | DASH | CAP | JOIN)
};

#define CQCHARTS_STROKE_DATA_PROPERTIES \
Q_PROPERTY(CQChartsStrokeData strokeData READ strokeData WRITE setStrokeData) \
\
Q_PROPERTY(bool             stroked     READ isStroked   WRITE setStroked    ) \
Q_PROPERTY(CQChartsColor    strokeColor READ strokeColor WRITE setStrokeColor) \
Q_PROPERTY(CQChartsAlpha    strokeAlpha READ strokeAlpha WRITE setStrokeAlpha) \
Q_PROPERTY(CQChartsLength   strokeWidth READ strokeWidth WRITE setStrokeWidth) \
Q_PROPERTY(CQChartsLineDash strokeDash  READ strokeDash  WRITE setStrokeDash ) \
Q_PROPERTY(CQChartsLineCap  strokeCap   READ strokeCap   WRITE setStrokeCap  ) \
Q_PROPERTY(CQChartsLineJoin strokeJoin  READ strokeJoin  WRITE setStrokeJoin ) \
Q_PROPERTY(CQChartsLength   cornerSize  READ cornerSize  WRITE setCornerSize )

/*!
 * \brief Object stroke data
 * \ingroup Charts
 */
template<class OBJ>
class CQChartsObjStrokeData {
 public:
  using Color      = CQChartsColor;
  using Alpha      = CQChartsAlpha;
  using Length     = CQChartsLength;
  using LineDash   = CQChartsLineDash;
  using LineCap    = CQChartsLineCap;
  using LineJoin   = CQChartsLineJoin;
  using ColorInd   = CQChartsUtil::ColorInd;
  using StrokeData = CQChartsStrokeData;

 public:
  using Invalidator = CQChartsInvalidator;

 public:
  CQChartsObjStrokeData(OBJ *obj) :
   strokeDataObj_(obj), invalidator_(obj) {
  }

  CQChartsObjStrokeData(OBJ *obj, Invalidator *invalidator) :
   strokeDataObj_(obj), invalidator_(obj), pinvalidator_(invalidator) {
  }

  //---

  bool isStroked() const { return strokeData_.isVisible(); }
  void setStroked(bool b) {
    if (b != strokeData_.isVisible()) {
      strokeData_.setVisible(b); strokeDataInvalidate(); }
  }

  const Color &strokeColor() const { return strokeData_.color(); }
  void setStrokeColor(const Color &c) {
    if (c != strokeData_.color()) {
      strokeData_.setColor(c); strokeDataInvalidate(); }
  }

  const Alpha &strokeAlpha() const { return strokeData_.alpha(); }
  void setStrokeAlpha(const Alpha &a) {
    if (a != strokeData_.alpha()) {
      strokeData_.setAlpha(a); strokeDataInvalidate(); }
  }

  const Length &strokeWidth() const { return strokeData_.width(); }
  void setStrokeWidth(const Length &l) {
    if (l != strokeData_.width()) {
      strokeData_.setWidth(l); strokeDataInvalidate(); }
  }

  const LineDash &strokeDash() const { return strokeData_.dash(); }
  void setStrokeDash(const LineDash &d) {
    if (d != strokeData_.dash()) {
      strokeData_.setDash(d); strokeDataInvalidate(); }
  }

  const LineCap &strokeCap() const { return strokeData_.lineCap(); }
  void setStrokeCap(const LineCap &c) {
    if (c != strokeData_.lineCap()) {
      strokeData_.setLineCap(c); strokeDataInvalidate(); }
  }

  const LineJoin &strokeJoin() const { return strokeData_.lineJoin(); }
  void setStrokeJoin(const LineJoin &j) {
    if (j != strokeData_.lineJoin()) {
      strokeData_.setLineJoin(j); strokeDataInvalidate(); }
  }

  const Length &cornerSize() const { return strokeData_.cornerSize(); }
  void setCornerSize(const Length &l) {
    if (l != strokeData_.cornerSize()) {
      strokeData_.setCornerSize(l); strokeDataInvalidate(); }
  }

  QColor interpStrokeColor(const ColorInd &ind) const {
    if (strokeDataObj_)
      return CQChartsInterpolator(strokeDataObj_).interpColor(strokeColor(), ind);
    else
      return strokeColor().color();
  }

  //---

  const StrokeData &strokeData() const { return strokeData_; }

  void setStrokeData(const StrokeData &data) {
    strokeData_ = data; strokeDataInvalidate();
  };

  //---

 private:
  virtual void strokeDataInvalidate(bool reload=false) {
    pinvalidator_ ? pinvalidator_->invalidate(reload) : invalidator_.invalidate(reload);
  }

 private:
  OBJ* strokeDataObj_ { nullptr };

 protected:
  StrokeData   strokeData_;
  Invalidator  invalidator_;
  Invalidator* pinvalidator_ { nullptr };
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
Q_PROPERTY(CQChartsLineCap  strokeCap   READ strokeCap   WRITE setStrokeCap  ) \
Q_PROPERTY(CQChartsLineJoin strokeJoin  READ strokeJoin  WRITE setStrokeJoin ) \
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
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using Length      = CQChartsLength;
  using LineDash    = CQChartsLineDash;
  using LineCap     = CQChartsLineCap;
  using LineJoin    = CQChartsLineJoin;
  using FillPattern = CQChartsFillPattern;
  using PenData     = CQChartsPenData;
  using BrushData   = CQChartsBrushData;
  using ColorInd    = CQChartsUtil::ColorInd;
  using ShapeData   = CQChartsShapeData;

 public:
  using Invalidator = CQChartsInvalidator;

 public:
  CQChartsObjShapeData(OBJ *obj) :
   shapeDataObj_(obj), invalidator_(obj) {
  }

  CQChartsObjShapeData(OBJ *obj, Invalidator *invalidator) :
   shapeDataObj_(obj), invalidator_(obj), pinvalidator_(invalidator) {
  }

  //---

  bool isStroked() const { return shapeData_.stroke().isVisible(); }
  void setStroked(bool b) {
    if (b != shapeData_.stroke().isVisible()) {
      shapeData_.stroke().setVisible(b); shapeDataInvalidate(); }
  }

  const Color &strokeColor() const { return shapeData_.stroke().color(); }
  void setStrokeColor(const Color &c) {
    if (c != shapeData_.stroke().color()) {
      shapeData_.stroke().setColor(c); shapeDataInvalidate(); }
  }

  const Alpha &strokeAlpha() const { return shapeData_.stroke().alpha(); }
  void setStrokeAlpha(const Alpha &a) {
    if (a != shapeData_.stroke().alpha()) {
      shapeData_.stroke().setAlpha(a); shapeDataInvalidate(); }
  }

  const Length &strokeWidth() const { return shapeData_.stroke().width(); }
  void setStrokeWidth(const Length &l) {
    if (l != shapeData_.stroke().width()) {
      shapeData_.stroke().setWidth(l); shapeDataInvalidate(); }
  }

  const LineDash &strokeDash() const { return shapeData_.stroke().dash(); }
  void setStrokeDash(const LineDash &d) {
    if (d != shapeData_.stroke().dash()) {
      shapeData_.stroke().setDash(d); shapeDataInvalidate(); }
  }

  const LineCap &strokeCap() const { return shapeData_.stroke().lineCap(); }
  void setStrokeCap(const LineCap &c) {
    if (c != shapeData_.stroke().lineCap()) {
      shapeData_.stroke().setLineCap(c); shapeDataInvalidate(); }
  }

  const LineJoin &strokeJoin() const { return shapeData_.stroke().lineJoin(); }
  void setStrokeJoin(const LineJoin &j) {
    if (j != shapeData_.stroke().lineJoin()) {
      shapeData_.stroke().setLineJoin(j); shapeDataInvalidate(); }
  }

  const Length &cornerSize() const { return shapeData_.stroke().cornerSize(); }
  void setCornerSize(const Length &l) {
    if (l != shapeData_.stroke().cornerSize()) {
      shapeData_.stroke().setCornerSize(l); shapeDataInvalidate(); }
  }

  QColor interpStrokeColor(const ColorInd &ind) const {
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

  const Color &fillColor() const { return shapeData_.fill().color(); }
  void setFillColor(const Color &c) {
    if (c != shapeData_.fill().color()) {
      shapeData_.fill().setColor(c); shapeDataInvalidate(); }
  }

  const Alpha &fillAlpha() const { return shapeData_.fill().alpha(); }
  void setFillAlpha(const Alpha &a) {
    if (a != shapeData_.fill().alpha()) {
      shapeData_.fill().setAlpha(a); shapeDataInvalidate(); }
  }

  const FillPattern &fillPattern() const { return shapeData_.fill().pattern(); }
  void setFillPattern(const FillPattern &p) {
    if (p != shapeData_.fill().pattern()) {
      shapeData_.fill().setPattern(p); shapeDataInvalidate(); }
  }

  QColor interpFillColor(const ColorInd &ind) const {
    if (shapeDataObj_)
      return CQChartsInterpolator(shapeDataObj_).interpColor(fillColor(), ind);
    else
      return fillColor().color();
  }

  //---

  PenData penData(const QColor &c, const Alpha &alpha=Alpha()) const {
    auto strokeAlpha = (! alpha.isSet() ? this->strokeAlpha() : alpha);
    return PenData(isStroked(), c, strokeAlpha, strokeWidth(), strokeDash(),
                   strokeCap(), strokeJoin());
  }

  BrushData brushData(const QColor &c, const Alpha &alpha=Alpha()) const {
    auto fillAlpha = (! alpha.isSet() ? this->fillAlpha() : alpha);
    return BrushData(isFilled(), c, fillAlpha, fillPattern());
  }

  //---

  const ShapeData &shapeData() const { return shapeData_; }

  void setShapeData(const ShapeData &data) {
    shapeData_ = data; shapeDataInvalidate();
  };

 private:
  virtual void shapeDataInvalidate(bool reload=false) {
    pinvalidator_ ? pinvalidator_->invalidate(reload) : invalidator_.invalidate(reload);
  }

 private:
  OBJ* shapeDataObj_ { nullptr };

 protected:
  ShapeData    shapeData_;
  Invalidator  invalidator_;
  Invalidator* pinvalidator_ { nullptr };
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
Q_PROPERTY(CQChartsLineCap  LNAME##StrokeCap \
           READ LNAME##StrokeCap   WRITE set##UNAME##StrokeCap  ) \
Q_PROPERTY(CQChartsLineJoin LNAME##StrokeJoin \
           READ LNAME##StrokeJoin  WRITE set##UNAME##StrokeJoin ) \
Q_PROPERTY(CQChartsLength   LNAME##CornerSize \
           READ LNAME##CornerSize  WRITE set##UNAME##CornerSize ) \
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
 * \brief Object named shape data
 * \ingroup Charts
 */
#define CQCHARTS_NAMED_SHAPE_DATA(UNAME, LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##ShapeData { \
 public: \
  using Color       = CQChartsColor; \
  using Alpha       = CQChartsAlpha; \
  using Length      = CQChartsLength; \
  using LineDash    = CQChartsLineDash; \
  using LineCap     = CQChartsLineCap; \
  using LineJoin    = CQChartsLineJoin; \
  using FillPattern = CQChartsFillPattern; \
  using PenData     = CQChartsPenData; \
  using BrushData   = CQChartsBrushData; \
  using ColorInd    = CQChartsUtil::ColorInd; \
  using ShapeData   = CQChartsShapeData; \
\
 public: \
  using Invalidator = CQChartsInvalidator; \
\
 public: \
  CQChartsObj##UNAME##ShapeData(OBJ *obj) : \
   LNAME##ShapeDataObj_(obj), LNAME##Invalidator_(obj) { \
  } \
\
  CQChartsObj##UNAME##ShapeData(OBJ *obj, Invalidator *invalidator) : \
   LNAME##ShapeDataObj_(obj), LNAME##Invalidator_(obj), LNAME##PInvalidator_(invalidator) { \
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
  const Color &LNAME##StrokeColor() const { return LNAME##ShapeData_.stroke().color(); } \
  void set##UNAME##StrokeColor(const Color &c) { \
    if (c != LNAME##ShapeData_.stroke().color()) { \
      LNAME##ShapeData_.stroke().setColor(c); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const Alpha &LNAME##StrokeAlpha() const { return LNAME##ShapeData_.stroke().alpha(); } \
  void set##UNAME##StrokeAlpha(const Alpha &a) { \
    if (a != LNAME##ShapeData_.stroke().alpha()) { \
      LNAME##ShapeData_.stroke().setAlpha(a); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const Length &LNAME##StrokeWidth() const { return LNAME##ShapeData_.stroke().width(); } \
  void set##UNAME##StrokeWidth(const Length &l) { \
    if (l != LNAME##ShapeData_.stroke().width()) { \
      LNAME##ShapeData_.stroke().setWidth(l); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const LineDash &LNAME##StrokeDash() const { return LNAME##ShapeData_.stroke().dash(); } \
  void set##UNAME##StrokeDash(const LineDash &d) { \
    if (d != LNAME##ShapeData_.stroke().dash()) { \
      LNAME##ShapeData_.stroke().setDash(d); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const LineCap &LNAME##StrokeCap() const { return LNAME##ShapeData_.stroke().lineCap(); } \
  void set##UNAME##StrokeCap(const LineCap &c) { \
    if (c != LNAME##ShapeData_.stroke().lineCap()) { \
      LNAME##ShapeData_.stroke().setLineCap(c); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const LineJoin &LNAME##StrokeJoin() const { return LNAME##ShapeData_.stroke().lineJoin(); } \
  void set##UNAME##StrokeJoin(const LineJoin &j) { \
    if (j != LNAME##ShapeData_.stroke().lineJoin()) { \
      LNAME##ShapeData_.stroke().setLineJoin(j); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const Length &LNAME##CornerSize() const { return LNAME##ShapeData_.stroke().cornerSize(); } \
  void set##UNAME##CornerSize(const Length &l) { \
    if (l != LNAME##ShapeData_.stroke().cornerSize()) { \
      LNAME##ShapeData_.stroke().setCornerSize(l); LNAME##ShapeDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##StrokeColor(const ColorInd &ind) const { \
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
  const Color &LNAME##FillColor() const { return LNAME##ShapeData_.fill().color(); } \
  void set##UNAME##FillColor(const Color &c) { \
    if (c != LNAME##ShapeData_.fill().color()) { \
      LNAME##ShapeData_.fill().setColor(c); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const Alpha &LNAME##FillAlpha() const { return LNAME##ShapeData_.fill().alpha(); } \
  void set##UNAME##FillAlpha(const Alpha &a) { \
    if (a != LNAME##ShapeData_.fill().alpha()) { \
      LNAME##ShapeData_.fill().setAlpha(a); LNAME##ShapeDataInvalidate(); } \
  } \
\
  const FillPattern &LNAME##FillPattern() const { \
    return LNAME##ShapeData_.fill().pattern(); } \
  void set##UNAME##FillPattern(const FillPattern &p) { \
    if (p != LNAME##ShapeData_.fill().pattern()) { \
      LNAME##ShapeData_.fill().setPattern(p); LNAME##ShapeDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##FillColor(const ColorInd &ind) const { \
    return CQChartsInterpolator(LNAME##ShapeDataObj_).interpColor(LNAME##FillColor(), ind); \
  } \
\
  const ShapeData &LNAME##ShapeData() const { return LNAME##ShapeData_; } \
\
  void set##UNAME##ShapeData(const ShapeData &data) { \
    LNAME##ShapeData_ = data; LNAME##ShapeDataInvalidate(); \
  } \
\
  PenData LNAME##PenData(const ColorInd &colorInd) const { \
    return PenData(is##UNAME##Stroked(), interp##UNAME##StrokeColor(colorInd), \
                   LNAME##StrokeAlpha(), LNAME##StrokeWidth(), LNAME##StrokeDash(), \
                   LNAME##StrokeCap(), LNAME##StrokeJoin()); \
  } \
\
  PenData LNAME##PenData(const QColor &c, const Alpha &alpha=Alpha(), \
                         const Length &width=Length(), const LineDash &dash=LineDash()) const { \
    auto strokeAlpha = (! alpha.isSet() ? LNAME##StrokeAlpha() : alpha); \
    auto strokeWidth = (! width.isSet() ? LNAME##StrokeWidth() : width); \
    auto strokeDash  = (! dash .isSet() ? LNAME##StrokeDash () : dash ); \
    return PenData(is##UNAME##Stroked(), c, strokeAlpha, strokeWidth, strokeDash); \
  } \
\
  BrushData LNAME##BrushData(const ColorInd &colorInd) const { \
    return BrushData(is##UNAME##Filled(), interp##UNAME##FillColor(colorInd), \
                     LNAME##FillAlpha(), LNAME##FillPattern()); \
  } \
\
  BrushData LNAME##BrushData(const QColor &c, const Alpha &alpha=Alpha(), \
                             const FillPattern &pattern=FillPattern::makeSolid()) const { \
    auto fillAlpha   = (! alpha  .isSet  () ? LNAME##FillAlpha  () : alpha); \
    auto fillPattern = (! pattern.isValid() ? LNAME##FillPattern() : pattern); \
    return BrushData(is##UNAME##Filled(), c, fillAlpha, fillPattern); \
  } \
\
 private: \
  virtual void LNAME##ShapeDataInvalidate(bool reload=false) { \
    LNAME##PInvalidator_ ? LNAME##PInvalidator_->invalidate(reload) : \
                           LNAME##Invalidator_.invalidate(reload); \
  } \
\
 private: \
  OBJ* LNAME##ShapeDataObj_ { nullptr }; \
\
 protected: \
  ShapeData    LNAME##ShapeData_; \
  bool         LNAME##ReloadObj_ { true }; \
  Invalidator  LNAME##Invalidator_; \
  Invalidator* LNAME##PInvalidator_ { nullptr }; \
};

//------

#define CQCHARTS_BOX_DATA_PROPERTIES \
Q_PROPERTY(CQChartsBoxData boxData READ boxData WRITE setBoxData) \
\
Q_PROPERTY(CQChartsMargin margin  READ margin  WRITE setMargin ) \
Q_PROPERTY(CQChartsMargin padding READ padding WRITE setPadding) \
\
Q_PROPERTY(bool             stroked     READ isStroked   WRITE setStroked    ) \
Q_PROPERTY(CQChartsColor    strokeColor READ strokeColor WRITE setStrokeColor) \
Q_PROPERTY(CQChartsAlpha    strokeAlpha READ strokeAlpha WRITE setStrokeAlpha) \
Q_PROPERTY(CQChartsLength   strokeWidth READ strokeWidth WRITE setStrokeWidth) \
Q_PROPERTY(CQChartsLineDash strokeDash  READ strokeDash  WRITE setStrokeDash ) \
Q_PROPERTY(CQChartsLineCap  strokeCap   READ strokeCap   WRITE setStrokeCap  ) \
Q_PROPERTY(CQChartsLineJoin strokeJoin  READ strokeJoin  WRITE setStrokeJoin ) \
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
  using Margin      = CQChartsMargin;
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using Length      = CQChartsLength;
  using LineDash    = CQChartsLineDash;
  using LineCap     = CQChartsLineCap;
  using LineJoin    = CQChartsLineJoin;
  using FillPattern = CQChartsFillPattern;
  using Sides       = CQChartsSides;
  using ColorInd    = CQChartsUtil::ColorInd;
  using BoxData     = CQChartsBoxData;

 public:
  using Invalidator = CQChartsInvalidator;

 public:
  CQChartsObjBoxData(OBJ *obj) :
   boxDataObj_(obj), invalidator_(obj) {
  }

  CQChartsObjBoxData(OBJ *obj, Invalidator *invalidator) :
   boxDataObj_(obj), invalidator_(obj), pinvalidator_(invalidator) {
  }

  //---

  const Margin &margin() const { return boxData_.margin(); }
  void setMargin(const Margin &m) { boxData_.setMargin(m); boxDataInvalidate(); }

  const Margin &padding() const { return boxData_.padding(); }
  void setPadding(const Margin &m) { boxData_.setPadding(m); boxDataInvalidate(); }

  //---

  const Sides &borderSides() const { return boxData_.borderSides(); }
  void setBorderSides(const Sides &s) { boxData_.setBorderSides(s); boxDataInvalidate(); }

  //---

  bool isStroked() const { return boxData_.shape().stroke().isVisible(); }
  void setStroked(bool b) {
    if (b != boxData_.shape().stroke().isVisible()) {
      boxData_.shape().stroke().setVisible(b); boxDataInvalidate(); }
  }

  const Color &strokeColor() const { return boxData_.shape().stroke().color(); }
  void setStrokeColor(const Color &c) {
    if (c != boxData_.shape().stroke().color()) {
      boxData_.shape().stroke().setColor(c); boxDataInvalidate(); }
  }

  const Alpha &strokeAlpha() const { return boxData_.shape().stroke().alpha(); }
  void setStrokeAlpha(const Alpha &a) {
    if (a != boxData_.shape().stroke().alpha()) {
      boxData_.shape().stroke().setAlpha(a); boxDataInvalidate(); }
  }

  const Length &strokeWidth() const { return boxData_.shape().stroke().width(); }
  void setStrokeWidth(const Length &l) {
    if (l != boxData_.shape().stroke().width()) {
      boxData_.shape().stroke().setWidth(l); boxDataInvalidate(); }
  }

  const LineDash &strokeDash() const { return boxData_.shape().stroke().dash(); }
  void setStrokeDash(const LineDash &d) {
    if (d != boxData_.shape().stroke().dash()) {
      boxData_.shape().stroke().setDash(d); boxDataInvalidate(); }
  }

  const LineCap &strokeCap() const { return boxData_.shape().stroke().lineCap(); }
  void setStrokeCap(const LineCap &c) {
    if (c != boxData_.shape().stroke().lineCap()) {
      boxData_.shape().stroke().setLineCap(c); boxDataInvalidate(); }
  }

  const LineJoin &strokeJoin() const { return boxData_.shape().stroke().lineJoin(); }
  void setStrokeJoin(const LineJoin &j) {
    if (j != boxData_.shape().stroke().lineJoin()) {
      boxData_.shape().stroke().setLineJoin(j); boxDataInvalidate(); }
  }

  const Length &cornerSize() const { return boxData_.shape().stroke().cornerSize(); }
  void setCornerSize(const Length &l) {
    if (l != boxData_.shape().stroke().cornerSize()) {
      boxData_.shape().stroke().setCornerSize(l); boxDataInvalidate(); }
  }

  QColor interpStrokeColor(const ColorInd &ind) const {
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

  const Color &fillColor() const { return boxData_.shape().fill().color(); }
  void setFillColor(const Color &c) {
    if (c != boxData_.shape().fill().color()) {
      boxData_.shape().fill().setColor(c); boxDataInvalidate(); }
  }

  const Alpha &fillAlpha() const { return boxData_.shape().fill().alpha(); }
  void setFillAlpha(const Alpha &a) {
    if (a != boxData_.shape().fill().alpha()) {
      boxData_.shape().fill().setAlpha(a); boxDataInvalidate(); }
  }

  const FillPattern &fillPattern() const { return boxData_.shape().fill().pattern(); }
  void setFillPattern(const FillPattern &p) {
    if (p != boxData_.shape().fill().pattern()) {
      boxData_.shape().fill().setPattern(p); boxDataInvalidate(); }
  }

  QColor interpFillColor(const ColorInd &ind) const {
    if (boxDataObj_)
      return CQChartsInterpolator(boxDataObj_).interpColor(fillColor(), ind);
    else
      return fillColor().color();
  }

  //---

  const BoxData &boxData() const { return boxData_; }

  void setBoxData(const BoxData &data) {
    boxData_ = data; boxDataInvalidate();
  };

 private:
  virtual void boxDataInvalidate(bool reload=false) {
    pinvalidator_ ? pinvalidator_->invalidate(reload) : invalidator_.invalidate(reload);
  }

 private:
  OBJ* boxDataObj_ { nullptr };

 protected:
  BoxData      boxData_;
  Invalidator  invalidator_;
  Invalidator* pinvalidator_ { nullptr };
};

//------

#define CQCHARTS_NAMED_BOX_DATA_PROPERTIES(UNAME, LNAME) \
Q_PROPERTY(CQChartsBoxData LNAME##BoxData READ LNAME##BoxData WRITE set##UNAME##BoxData) \
\
Q_PROPERTY(CQChartsMargin LNAME##Margin  READ LNAME##Margin  WRITE set##UNAME##Margin ) \
Q_PROPERTY(CQChartsMargin LNAME##Padding READ LNAME##Padding WRITE set##UNAME##Padding) \
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
Q_PROPERTY(CQChartsLineCap  LNAME##StrokeCap \
           READ LNAME##StrokeCap   WRITE set##UNAME##StrokeCap  ) \
Q_PROPERTY(CQChartsLineJoin LNAME##StrokeJoin \
           READ LNAME##StrokeJoin  WRITE set##UNAME##StrokeJoin ) \
Q_PROPERTY(CQChartsLength   LNAME##CornerSize \
           READ LNAME##CornerSize  WRITE set##UNAME##CornerSize ) \
\
Q_PROPERTY(bool                LNAME##Filled \
           READ is##UNAME##Filled  WRITE set##UNAME##Filled     ) \
Q_PROPERTY(CQChartsColor       LNAME##FillColor \
           READ LNAME##FillColor   WRITE set##UNAME##FillColor  ) \
Q_PROPERTY(CQChartsAlpha       LNAME##FillAlpha \
           READ LNAME##FillAlpha   WRITE set##UNAME##FillAlpha  ) \
Q_PROPERTY(CQChartsFillPattern LNAME##FillPattern \
           READ LNAME##FillPattern WRITE set##UNAME##FillPattern) \
\
Q_PROPERTY(CQChartsSides LNAME##BorderSides \
           READ LNAME##BorderSides WRITE set##UNAME##BorderSides)

/*!
 * \brief Object named box data
 * \ingroup Charts
 */
#define CQCHARTS_NAMED_BOX_DATA(UNAME, LNAME) \
template<class OBJ> \
class CQChartsObj##UNAME##BoxData { \
 public: \
  using Margin      = CQChartsMargin; \
  using Color       = CQChartsColor; \
  using Alpha       = CQChartsAlpha; \
  using Length      = CQChartsLength; \
  using LineDash    = CQChartsLineDash; \
  using LineCap     = CQChartsLineCap; \
  using LineJoin    = CQChartsLineJoin; \
  using FillPattern = CQChartsFillPattern; \
  using Sides       = CQChartsSides; \
  using PenBrush    = CQChartsPenBrush; \
  using PenData     = CQChartsPenData; \
  using BrushData   = CQChartsBrushData; \
  using ColorInd    = CQChartsUtil::ColorInd; \
  using BoxData     = CQChartsBoxData; \
\
 public: \
  using Invalidator = CQChartsInvalidator; \
\
 public: \
  CQChartsObj##UNAME##BoxData(OBJ *obj) : \
   LNAME##BoxDataObj_(obj), LNAME##Invalidator_(obj) { \
  } \
\
  CQChartsObj##UNAME##BoxData(OBJ *obj, Invalidator *invalidator) : \
   LNAME##BoxDataObj_(obj), LNAME##Invalidator_(obj), LNAME##PInvalidator_(invalidator) { \
  } \
\
  const Margin &LNAME##Margin() const { return LNAME##BoxData_.margin(); } \
  void set##UNAME##Margin(const Margin &m) { \
    LNAME##BoxData_.setMargin(m); LNAME##BoxDataInvalidate(); } \
\
  const Margin &LNAME##Padding() const { return LNAME##BoxData_.padding(); } \
  void set##UNAME##Padding(const Margin &m) { \
    LNAME##BoxData_.setPadding(m); LNAME##BoxDataInvalidate(); } \
\
  const Sides &LNAME##BorderSides() const { return LNAME##BoxData_.borderSides(); } \
  void set##UNAME##BorderSides(const Sides &s) { \
    LNAME##BoxData_.setBorderSides(s); LNAME##BoxDataInvalidate(); } \
\
  bool is##UNAME##Stroked() const { return LNAME##BoxData_.shape().stroke().isVisible(); } \
  void set##UNAME##Stroked(bool b) { \
    if (b != LNAME##BoxData_.shape().stroke().isVisible()) { \
      LNAME##BoxData_.shape().stroke().setVisible(b); LNAME##BoxDataInvalidate(); } \
  } \
\
  const Color &LNAME##StrokeColor() const { return LNAME##BoxData_.shape().stroke().color(); } \
  void set##UNAME##StrokeColor(const Color &c) { \
    if (c != LNAME##BoxData_.shape().stroke().color()) { \
      LNAME##BoxData_.shape().stroke().setColor(c); LNAME##BoxDataInvalidate(); } \
  } \
\
  const Alpha &LNAME##StrokeAlpha() const { return LNAME##BoxData_.shape().stroke().alpha(); } \
  void set##UNAME##StrokeAlpha(const Alpha &a) { \
    if (a != LNAME##BoxData_.shape().stroke().alpha()) { \
      LNAME##BoxData_.shape().stroke().setAlpha(a); LNAME##BoxDataInvalidate(); } \
  } \
\
  const Length &LNAME##StrokeWidth() const { return LNAME##BoxData_.shape().stroke().width(); } \
  void set##UNAME##StrokeWidth(const Length &l) { \
    if (l != LNAME##BoxData_.shape().stroke().width()) { \
      LNAME##BoxData_.shape().stroke().setWidth(l); LNAME##BoxDataInvalidate(); } \
  } \
\
  const LineDash &LNAME##StrokeDash() const { return LNAME##BoxData_.shape().stroke().dash(); } \
  void set##UNAME##StrokeDash(const LineDash &d) { \
    if (d != LNAME##BoxData_.shape().stroke().dash()) { \
      LNAME##BoxData_.shape().stroke().setDash(d); LNAME##BoxDataInvalidate(); } \
  } \
\
  const LineCap &LNAME##StrokeCap() const { return LNAME##BoxData_.shape().stroke().lineCap(); } \
  void set##UNAME##StrokeCap(const LineCap &c) { \
    if (c != LNAME##BoxData_.shape().stroke().lineCap()) { \
      LNAME##BoxData_.shape().stroke().setLineCap(c); LNAME##BoxDataInvalidate(); } \
  } \
\
  const LineJoin &LNAME##StrokeJoin() const { \
    return LNAME##BoxData_.shape().stroke().lineJoin(); } \
  void set##UNAME##StrokeJoin(const LineJoin &j) { \
    if (j != LNAME##BoxData_.shape().stroke().lineJoin()) { \
      LNAME##BoxData_.shape().stroke().setLineJoin(j); LNAME##BoxDataInvalidate(); } \
  } \
\
  const Length &LNAME##CornerSize() const { \
    return LNAME##BoxData_.shape().stroke().cornerSize(); } \
  void set##UNAME##CornerSize(const Length &l) { \
    if (l != LNAME##BoxData_.shape().stroke().cornerSize()) { \
      LNAME##BoxData_.shape().stroke().setCornerSize(l); LNAME##BoxDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##StrokeColor(const ColorInd &ind) const { \
    if (LNAME##BoxDataObj_) \
      return CQChartsInterpolator(LNAME##BoxDataObj_).interpColor(LNAME##StrokeColor(), ind); \
    else \
      return LNAME##StrokeColor().color(); \
  } \
\
  bool is##UNAME##Filled() const { return LNAME##BoxData_.shape().fill().isVisible(); } \
  void set##UNAME##Filled(bool b) { \
    if (b != LNAME##BoxData_.shape().fill().isVisible()) { \
      LNAME##BoxData_.shape().fill().setVisible(b); LNAME##BoxDataInvalidate(); } \
  } \
\
  const Color &LNAME##FillColor() const { return LNAME##BoxData_.shape().fill().color(); } \
  void set##UNAME##FillColor(const Color &c) { \
    if (c != LNAME##BoxData_.shape().fill().color()) { \
      LNAME##BoxData_.shape().fill().setColor(c); LNAME##BoxDataInvalidate(); } \
  } \
\
  const Alpha &LNAME##FillAlpha() const { return LNAME##BoxData_.shape().fill().alpha(); } \
  void set##UNAME##FillAlpha(const Alpha &a) { \
    if (a != LNAME##BoxData_.shape().fill().alpha()) { \
      LNAME##BoxData_.shape().fill().setAlpha(a); LNAME##BoxDataInvalidate(); } \
  } \
\
  const FillPattern &LNAME##FillPattern() const { \
    return LNAME##BoxData_.shape().fill().pattern(); } \
  void set##UNAME##FillPattern(const FillPattern &p) { \
    if (p != LNAME##BoxData_.shape().fill().pattern()) { \
      LNAME##BoxData_.shape().fill().setPattern(p); LNAME##BoxDataInvalidate(); } \
  } \
\
  QColor interp##UNAME##FillColor(const ColorInd &ind) const { \
    if (LNAME##BoxDataObj_) \
      return CQChartsInterpolator(LNAME##BoxDataObj_).interpColor(LNAME##FillColor(), ind); \
    else \
      return LNAME##FillColor().color(); \
  } \
\
  const BoxData &LNAME##BoxData() const { return LNAME##BoxData_; } \
\
  void set##UNAME##BoxData(const BoxData &data) { \
    LNAME##BoxData_ = data; LNAME##BoxDataInvalidate(); \
  }; \
\
  void set##UNAME##BoxDataPenBrush(PenBrush &penBrush, const ColorInd &ind) const { \
    LNAME##BoxDataObj_->setPenBrush(penBrush, \
      PenData(is##UNAME##Stroked(), interp##UNAME##StrokeColor(ind), \
              LNAME##StrokeAlpha(), LNAME##StrokeWidth(), LNAME##StrokeDash(), \
              LNAME##StrokeCap(), LNAME##StrokeJoin()), \
      BrushData(is##UNAME##Filled(), interp##UNAME##FillColor(ind), \
                LNAME##FillAlpha(), LNAME##FillPattern())); \
  } \
\
 private: \
  virtual void LNAME##BoxDataInvalidate(bool reload=false) { \
    LNAME##PInvalidator_ ? LNAME##PInvalidator_->invalidate(reload) : \
                           LNAME##Invalidator_.invalidate(reload); \
  } \
\
 private: \
  OBJ* LNAME##BoxDataObj_ { nullptr }; \
\
 protected: \
  BoxData      LNAME##BoxData_; \
  Invalidator  LNAME##Invalidator_; \
  Invalidator* LNAME##PInvalidator_ { nullptr }; \
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
