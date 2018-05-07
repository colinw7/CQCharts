#ifndef CQFontChooser_H
#define CQFontChooser_H

#include <QWidget>

class QLineEdit;
class QPushButton;
class QToolButton;
class QLabel;
class QFontComboBox;
class QComboBox;

class CQFontChooser : public QWidget {
  Q_OBJECT

  Q_PROPERTY(QFont   font        READ font        WRITE setFont       )
  Q_PROPERTY(QString fontName    READ fontName    WRITE setFontName   )
  Q_PROPERTY(bool    fixedWidth  READ fixedWidth  WRITE setFixedWidth )
  Q_PROPERTY(QString exampleText READ exampleText WRITE setExampleText)

 public:
  enum Style {
    FontNone,
    FontEdit,
    FontButton,
    FontLabel,
    FontDetailLabel,
    FontCombo
  };

  CQFontChooser(QWidget *parent=0);

  const QFont &font() const { return font_; }

  const QString &fontName() const { return fontName_; }

  bool fixedWidth() const { return fixedWidth_; }

  const QString &exampleText() const { return exampleText_; }

 public slots:
  void setStyle(Style style);

  void setFont(const QFont &font);

  void setFontName(const QString &fontName);

  void setFixedWidth(bool fixedWidth);

  void setExampleText(const QString &text);

 private:
  void updateWidgets();

  void updateCombos();

  void updateStyles();
  void updateSizes();

 private slots:
  void chooseFont();
  void editFont();
  void applyFont();
  void nameChanged();
  void styleChanged();
  void sizeChanged();

 signals:
  void fontChanged(const QFont &font);
  void fontChanged(const QString &fontName);
  void fontApplied(const QFont &font);
  void fontApplied(const QString &fontName);

 private:
  Style          style_;
  bool           fixedWidth_;
  QFont          font_;
  QString        fontName_;
  QString        exampleText_;
  QLineEdit     *cedit_;
  QToolButton   *cbutton_;
  QLabel        *clabel_;
  QToolButton   *button_;
  QFontComboBox *ncombo_;
  QComboBox     *scombo_;
  QComboBox     *zcombo_;
};

#endif
