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
    VISIBLE        = (1<<0),
    TEXT           = (1<<1),
    COLOR          = (1<<2),
    ALPHA          = (1<<3),
    FONT           = (1<<4),
    ANGLE          = (1<<5),
    CONTRAST       = (1<<6),
    CONTRAST_ALPHA = (1<<7),
    HTML           = (1<<8),
    ALIGN          = (1<<9),
    FORMATTED      = (1<<10),
    SCALED         = (1<<11),
    CLIP_LENGTH    = (1<<12),
    CLIP_ELIDE     = (1<<13),

    NONE        = 0,
    BASIC       = COLOR | ALPHA | FONT | ANGLE | CONTRAST | CONTRAST_ALPHA,
    ALL         = 0xFFFF,
    NOT_VISIBLE = (ALL & ~VISIBLE)
  };

  using View         = CQChartsView;
  using Plot         = CQChartsPlot;
  using Color        = CQChartsColor;
  using PropertyView = CQPropertyViewModel;

 public:
  CQChartsTextBoxObj(View *view);
  CQChartsTextBoxObj(Plot *plot);

  virtual ~CQChartsTextBoxObj() { }

  //---

  void setVisible(bool b) override { CQChartsBoxObj::setVisible(b); textBoxObjInvalidate(); }

  //---

  const QString &textStr() const { return textStr_; }
  void setTextStr(const QString &s) { textStr_ = s; textBoxObjInvalidate(); }

  //---

#if 0
  void setTextFontSize(double s) {
    QFont f = textData_.font(); f.setPointSizeF(s); textData_.setFont(f); textBoxObjInvalidate(); }
#endif

  //---

  void addProperties(PropertyView *model, const QString &path, const QString &desc) override;

  void addTypeProperties(PropertyView *model, const QString &path,
                         const QString &desc, uint addTypes=PropertyType::ALL);

  void addTextDataProperties(PropertyView *model, const QString &path,
                             const QString &desc, uint addTypes=PropertyType::ALL);

  //---

  CQChartsTextOptions textOptions() const;

  //---

  virtual void textBoxObjInvalidate();

  //---

  void write(std::ostream &os, const QString &varName) const;

 Q_SIGNALS:
  void textBoxObjInvalidated();

 protected:
  void init();

  void textDataInvalidate(bool) override {
    textBoxObjInvalidate();

    textBoxObjInvalidated();
  }

 protected:
  QString textStr_; //!< text
};

#endif
