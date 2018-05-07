#ifndef CQFontDialog_H
#define CQFontDialog_H

#include <QDialog>
#include <QFontDatabase>

class CQFontListView;
class QLineEdit;
class QLabel;
class QCheckBox;
class QComboBox;
class QDialogButtonBox;

class CQFontDialog : public QDialog {
  Q_OBJECT

  Q_PROPERTY(QFont             currentFont READ currentFont WRITE setCurrentFont)
  Q_PROPERTY(FontDialogOptions options     READ options     WRITE setOptions    )

 public:
  enum FontDialogOption {
    NoButtons           = (1<<0),
    DontUseNativeDialog = (1<<1),
    ScalableFonts       = (1<<2),
    NonScalableFonts    = (1<<3),
    MonospacedFonts     = (1<<4),
    ProportionalFonts   = (1<<5)
  };

  Q_ENUM(FontDialogOption)

  Q_DECLARE_FLAGS(FontDialogOptions, FontDialogOption)

 public:
  static QFont getFont(bool *ok, QWidget *parent = nullptr);
  static QFont getFont(bool *ok, const QFont &initial, QWidget *parent = nullptr,
                       const QString &title="", FontDialogOptions options=FontDialogOptions());

 public:
  CQFontDialog(QWidget *parent=0);
  CQFontDialog(const QFont &font, QWidget *parent=0);

 ~CQFontDialog();

  void setCurrentFont(const QFont &font);
  QFont currentFont() const;

  void setWritingSystem(QFontDatabase::WritingSystem ws);

  QFont selectedFont() const;

  void setOption(FontDialogOption option, bool on = true);
  bool testOption(FontDialogOption option) const;
  void setOptions(FontDialogOptions options);
  FontDialogOptions options() const;

 signals:
  void currentFontChanged(const QFont &font);
  void fontSelected(const QFont &font);

 private:
  void init();

  void initWidgets();

  void done(int result) override;

  bool eventFilter(QObject *object, QEvent *event) override;

 private slots:
  void familyHighlighted(int);
  void writingSystemHighlighted(int);
  void styleHighlighted(int);
  void sizeHighlighted(int);
  void sizeChanged(const QString &);

  void updateSample();
  void updateFamilies();
  void updateStyles();
  void updateSizes();

  void updateSampleFont(const QFont &newFont);

 private:
  QLineEdit*        familyEdit_         { nullptr };
  CQFontListView*   familyList_         { nullptr };
  QLabel*           familyAccel_        { nullptr };
  QLineEdit*        styleEdit_          { nullptr };
  CQFontListView*   styleList_          { nullptr };
  QLabel*           styleAccel_         { nullptr };
  QLineEdit*        sizeEdit_           { nullptr };
  CQFontListView*   sizeList_           { nullptr };
  QLabel*           sizeAccel_          { nullptr };
  QCheckBox*        strikeout_          { nullptr };
  QCheckBox*        underline_          { nullptr };
  QLineEdit*        sampleEdit_         { nullptr };
  QComboBox*        writingSystemCombo_ { nullptr };
  QLabel*           writingSystemAccel_ { nullptr };
  QDialogButtonBox* buttonBox_          { nullptr };

  QFontDatabase                fdb_;
  QString                      family_;
  QFontDatabase::WritingSystem writingSystem_;
  QString                      style_;
  int                          size_               { 0 };
  bool                         smoothScalable_     { false };
  QFont                        selectedFont_;
  FontDialogOptions            options_;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(CQFontDialog::FontDialogOptions)

#endif
