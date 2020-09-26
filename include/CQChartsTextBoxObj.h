#ifndef CQChartsTextBoxObj_H
#define CQChartsTextBoxObj_H

#include <CQChartsBoxObj.h>
#include <CQChartsGeom.h>
#include <CQCharts.h>
#include <QFont>

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
   using View         = CQChartsView;
   using Plot         = CQChartsPlot;
   using PropertyView = CQPropertyViewModel;

 public:
  CQChartsTextBoxObj(View *view);
  CQChartsTextBoxObj(Plot *plot);

  virtual ~CQChartsTextBoxObj() { }

  //---

  void setVisible(bool b) override { CQChartsBoxObj::setVisible(b); textBoxDataInvalidate(); }

  //---

  const QString &textStr() const { return textStr_; }
  void setTextStr(const QString &s) { textStr_ = s; textBoxDataInvalidate(); }

  //---

#if 0
  void setTextFontSize(double s) {
    QFont f = textData_.font(); f.setPointSizeF(s); textData_.setFont(f); boxDataInvalidate(); }
#endif

  //---

  void addProperties(PropertyView *model, const QString &path,
                     const QString &desc) override;

  void addTextDataProperties(PropertyView *model, const QString &path,
                             const QString &desc, bool addVisible=false);

  //---

  virtual void textBoxDataInvalidate();

  //---

  void write(std::ostream &os, const QString &varName) const;

 protected:
  void init();

  void textDataInvalidate(bool) override {
    textBoxDataInvalidate();
  }

 protected:
  QString textStr_; //!< text
};

#endif
