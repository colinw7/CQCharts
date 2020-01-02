#ifndef CQChartsTextDlg_H
#define CQChartsTextDlg_H

#include <CQChartsColor.h>
#include <CQChartsAlpha.h>
#include <CQChartsFont.h>
#include <CQChartsAngle.h>

#include <QDialog>

class CQChartsTextCanvas;
class CQPropertyViewModel;
class CQPropertyViewTree;

class CQChartsTextDlg : public QDialog {
  Q_OBJECT

 public:
  CQChartsTextDlg(QWidget *parent=nullptr);

  QSize sizeHint() const override;

 private:
  CQChartsTextCanvas*  canvas_        { nullptr };
  CQPropertyViewModel* propertyModel_ { nullptr };
  CQPropertyViewTree*  propertyTree_  { nullptr };
};

class CQChartsTextCanvas : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QString       text          READ text          WRITE setText         )
  Q_PROPERTY(CQChartsColor color         READ color         WRITE setColor        )
  Q_PROPERTY(CQChartsAlpha alpha         READ alpha         WRITE setAlpha        )
  Q_PROPERTY(CQChartsFont  font          READ font          WRITE setFont         )
  Q_PROPERTY(CQChartsAngle angle         READ angle         WRITE setAngle        )
  Q_PROPERTY(bool          contrast      READ isContrast    WRITE setContrast     )
  Q_PROPERTY(CQChartsAlpha contrastAlpha READ contrastAlpha WRITE setContrastAlpha)
  Q_PROPERTY(Qt::Alignment align         READ align         WRITE setAlign        )
  Q_PROPERTY(bool          formatted     READ isFormatted   WRITE setFormatted    )
  Q_PROPERTY(bool          scaled        READ isScaled      WRITE setScaled       )
  Q_PROPERTY(bool          html          READ isHtml        WRITE setHtml         )

 public:
  CQChartsTextCanvas(QWidget *parent=nullptr);

  void paintEvent(QPaintEvent *) override;

  const QString &text() const { return text_; }
  void setText(const QString &v) { text_ = v; }

  const CQChartsColor &color() const { return color_; }
  void setColor(const CQChartsColor &c) { if (c != color_) { color_ = c; invalidate(); } }

  const CQChartsAlpha &alpha() const { return alpha_; }
  void setAlpha(const CQChartsAlpha &a) { if (a != alpha_) { alpha_ = a; invalidate(); } }

  const CQChartsFont &font() const { return font_; }
  void setFont(const CQChartsFont &f) { if (f != font_) { font_ = f; invalidate(); } }

  const CQChartsAngle &angle() const { return angle_; }
  void setAngle(const CQChartsAngle &a) { if (a != angle_) { angle_ = a; invalidate(); } }

  bool isContrast() const { return contrast_; }
  void setContrast(bool b) { if (b != contrast_) { contrast_ = b; invalidate(); } }

  const CQChartsAlpha &contrastAlpha() const { return contrastAlpha_; }
  void setContrastAlpha(const CQChartsAlpha &a) {
    if (a != contrastAlpha_) { contrastAlpha_ = a; invalidate(); } }

  const Qt::Alignment &align() const { return align_; }
  void setAlign(const Qt::Alignment &a) { if (a != align_) { align_ = a; invalidate(); } }

  bool isFormatted() const { return formatted_; }
  void setFormatted(bool b) { if (b != formatted_) { formatted_ = b; invalidate(); } }

  bool isScaled() const { return scaled_; }
  void setScaled(bool b) { if (b != scaled_) { scaled_ = b; invalidate(); } }

  bool isHtml() const { return html_; }
  void setHtml(bool b) { if (b != html_) { html_ = b; invalidate(); } }

  void invalidate() { update(); }

  QSize sizeHint() const override;

 private:
  QString       text_          { "Abc" };
  QRect         rect_          { 100, 100, 400, 400 };
  CQChartsColor color_;
  CQChartsAlpha alpha_;
  CQChartsFont  font_;
  CQChartsAngle angle_         { 0.0 };
  bool          contrast_      { false };
  CQChartsAlpha contrastAlpha_ { 0.5 };
  Qt::Alignment align_         { Qt::AlignHCenter | Qt::AlignVCenter };
  bool          formatted_     { false };
  bool          scaled_        { false };
  bool          html_          { false };
};

#endif
