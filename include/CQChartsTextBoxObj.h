#ifndef CQChartsTextBoxObj_H
#define CQChartsTextBoxObj_H

#include <CQChartsBoxObj.h>
#include <CQChartsGeom.h>
#include <CQCharts.h>
#include <QFont>

/*!
 * \brief Charts Text Box Object
 * \ingroup Charts
 *
 * Extends CQChartsBoxObj with text properties
 */
class CQChartsTextBoxObj : public CQChartsBoxObj,
 public CQChartsObjTextData<CQChartsTextBoxObj> {
  Q_OBJECT

  Q_PROPERTY(QString textStr    READ textStr WRITE setTextStr)
  Q_PROPERTY(QString textString READ textStr WRITE setTextStr)

  CQCHARTS_TEXT_DATA_PROPERTIES

 public:
  enum PropertyType {
    VISIBLE = (1<<0),
    ANGLE   = (1<<1),
    ALIGN   = (1<<2),

    NONE        = 0,
    NOT_VISIBLE = ANGLE | ALIGN,
    ALL         = VISIBLE | ANGLE | ALIGN
  };

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

  void addProperties(PropertyView *model, const QString &path, const QString &desc) override;

  void addTypeProperties(PropertyView *model, const QString &path,
                         const QString &desc, uint addTypes=PropertyType::ALL);

  void addTextDataProperties(PropertyView *model, const QString &path,
                             const QString &desc, uint addTypes=PropertyType::ALL);

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
