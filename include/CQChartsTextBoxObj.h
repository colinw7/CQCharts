#ifndef CQChartsTextBoxObj_H
#define CQChartsTextBoxObj_H

#include <CQChartsBoxObj.h>
#include <CQChartsGeom.h>
#include <CQCharts.h>
#include <QFont>
#include <QRectF>

/*!
 * \brief Charts Text Box Object
 */
class CQChartsTextBoxObj : public CQChartsBoxObj,
 public CQChartsObjTextData<CQChartsTextBoxObj> {
  Q_OBJECT

  Q_PROPERTY(QString textStr    READ textStr WRITE setTextStr)
  Q_PROPERTY(QString textString READ textStr WRITE setTextStr)

  CQCHARTS_TEXT_DATA_PROPERTIES

 public:
  CQChartsTextBoxObj(CQChartsView *view);
  CQChartsTextBoxObj(CQChartsPlot *plot);

  virtual ~CQChartsTextBoxObj() { }

  //---

  const QString &textStr() const { return textStr_; }
  void setTextStr(const QString &s) { textStr_ = s; textBoxDataInvalidate(); }

  //---

#if 0
  void setTextFontSize(double s) {
    QFont f = textData_.font(); f.setPointSizeF(s); textData_.setFont(f); boxDataInvalidate(); }
#endif

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void addTextDataProperties(CQPropertyViewModel *model, const QString &path);

  //---

  void draw(QPainter *painter, const QRectF &rect) const;
  void draw(QPainter *painter, const QPolygonF &poly) const;

  //---

  void drawText(QPainter *painter, const QRectF &rect, const QString &text) const;

  //---

  virtual void textBoxDataInvalidate();

 protected:
  void textDataInvalidate(bool) override {
    textBoxDataInvalidate();
  }

 protected:
  QString textStr_; //! text
};

#endif
