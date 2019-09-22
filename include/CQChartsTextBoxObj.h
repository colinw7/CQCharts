#ifndef CQChartsTextBoxObj_H
#define CQChartsTextBoxObj_H

#include <CQChartsBoxObj.h>
#include <CQChartsGeom.h>
#include <CQCharts.h>
#include <QFont>
#include <QRectF>

/*!
 * \brief Charts Text Box Object
 * \ingroup Charts
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

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const QString &desc) override;

  void addTextDataProperties(CQPropertyViewModel *model, const QString &path,
                             const QString &desc, bool addVisible=false);

  //---

  virtual void textBoxDataInvalidate();

  //---

  void write(std::ostream &os, const QString &varName) const;

 protected:
  void textDataInvalidate(bool) override {
    textBoxDataInvalidate();
  }

 protected:
  QString textStr_; //!< text
};

#endif
