#ifndef CQChartsTextBoxObj_H
#define CQChartsTextBoxObj_H

#include <CQChartsBoxObj.h>
#include <CQChartsGeom.h>
#include <QFont>
#include <QRectF>

class CQChartsTextBoxObj : public CQChartsBoxObj {
  Q_OBJECT

  Q_PROPERTY(QString textStr READ textStr WRITE setTextStr)

  Q_PROPERTY(bool          textVisible   READ isTextVisible   WRITE setTextVisible  )
  Q_PROPERTY(QFont         textFont      READ textFont        WRITE setTextFont     )
  Q_PROPERTY(CQChartsColor textColor     READ textColor       WRITE setTextColor    )
  Q_PROPERTY(double        textAlpha     READ textAlpha       WRITE setTextAlpha    )
  Q_PROPERTY(double        textAngle     READ textAngle       WRITE setTextAngle    )
  Q_PROPERTY(bool          textContrast  READ isTextContrast  WRITE setTextContrast )
  Q_PROPERTY(Qt::Alignment textAlign     READ textAlign       WRITE setTextAlign    )
  Q_PROPERTY(bool          textFormatted READ isTextFormatted WRITE setTextFormatted)
  Q_PROPERTY(bool          textScaled    READ isTextScaled    WRITE setTextScaled   )

 public:
  CQChartsTextBoxObj(CQChartsView *view);
  CQChartsTextBoxObj(CQChartsPlot *plot);

  virtual ~CQChartsTextBoxObj() { }

  //---

  bool isTextVisible() const { return textData_.isVisible(); }
  void setTextVisible(bool b) { textData_.setVisible(b); redrawBoxObj(); }

  const QString &textStr() const { return textStr_; }
  void setTextStr(const QString &s) { textStr_ = s; redrawBoxObj(); }

  const QFont &textFont() const { return textData_.font(); }
  void setTextFont(const QFont &f) { textData_.setFont(f); redrawBoxObj(); }

  const CQChartsColor &textColor() const { return textData_.color(); }
  void setTextColor(const CQChartsColor &c) { textData_.setColor(c); redrawBoxObj(); }

  double textAlpha() const { return textData_.alpha(); }
  void setTextAlpha(double a) { textData_.setAlpha(a); redrawBoxObj(); }

  double textAngle() const { return textData_.angle(); }
  void setTextAngle(double a) { textData_.setAngle(a); redrawBoxObj(); }

  bool isTextContrast() const { return textData_.isContrast(); }
  void setTextContrast(bool b) { textData_.setContrast(b); redrawBoxObj(); }

  const Qt::Alignment &textAlign() const { return textData_.align(); }
  void setTextAlign(const Qt::Alignment &a) { textData_.setAlign(a); redrawBoxObj(); }

  bool isTextFormatted() const { return textData_.isFormatted(); }
  void setTextFormatted(bool b) { textData_.setFormatted(b); redrawBoxObj(); }

  bool isTextScaled() const { return textData_.isScaled(); }
  void setTextScaled(bool b) { textData_.setScaled(b); redrawBoxObj(); }

  bool isHtml() const { return textData_.isHtml(); }
  void setHtml(bool b) { textData_.setHtml(b); redrawBoxObj(); }

  //---

  const CQChartsTextData &textData() const { return textData_; }
  void setTextData(const CQChartsTextData &textData) { textData_ = textData; redrawBoxObj(); }

  //---

  void setTextFontSize(double s) {
    QFont f = textData_.font(); f.setPointSizeF(s); textData_.setFont(f); redrawBoxObj(); }

  QColor interpTextColor(int i, int n) const;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  void draw(QPainter *painter, const QRectF &rect) const;
  void draw(QPainter *painter, const QPolygonF &poly) const;

  //---

  void drawText(QPainter *painter, const QRectF &rect, const QString &text) const;

 protected:
  QString          textStr_;  // text
  CQChartsTextData textData_; // draw data
};

#endif
