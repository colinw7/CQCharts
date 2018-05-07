#include <CQFontDialog.h>
#include <CQFontListView.h>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QListView>
#include <QStringListModel>
#include <QFontDatabase>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFontDatabase>
#include <QApplication>
#include <QKeyEvent>

namespace {

void parseFontName(const QString &name, QString &foundry, QString &family) {
  int i  = name.indexOf    (QLatin1Char('['));
  int li = name.lastIndexOf(QLatin1Char(']'));

  if (i >= 0 && li >= 0 && i < li) {
    foundry = name.mid(i + 1, li - i - 1);

    if (i > 0 && name[i - 1] == QLatin1Char(' '))
      i--;

    family = name.left(i);
  }
  else {
    foundry.clear();
    family = name;
  }

  // capitalize the family/foundry names
  bool space = true;

  QChar *s   = family.data();
  int    len = family.length();

  while (len--) {
    if (space) *s = s->toUpper();

    space = s->isSpace();

    ++s;
  }

  space = true;

  s   = foundry.data();
  len = foundry.length();

  while (len--) {
    if (space) *s = s->toUpper();

    space = s->isSpace();

    ++s;
  }
}

}

//------

QFont
CQFontDialog::
getFont(bool *ok, QWidget *parent)
{
  QFont initial;

  return getFont(ok, initial, parent, QString(), 0);
}

QFont
CQFontDialog::
getFont(bool *ok, const QFont &initial, QWidget *parent, const QString &title,
        FontDialogOptions options)
{
  CQFontDialog dlg(parent);

  dlg.setOptions(options);
  dlg.setCurrentFont(initial);

  if (! title.isEmpty())
    dlg.setWindowTitle(title);

  int ret = (dlg.exec() || (options & CQFontDialog::NoButtons));

  if (ok)
    *ok = !!ret;

  if (! ret)
    return initial;

  return dlg.selectedFont();
}

//------

CQFontDialog::
CQFontDialog(QWidget *parent) :
 QDialog(parent)
{
  init();
}

CQFontDialog::
CQFontDialog(const QFont &initial, QWidget *parent) :
 QDialog(parent)
{
  init();

  setCurrentFont(initial);
}

CQFontDialog::
~CQFontDialog()
{
}

void
CQFontDialog::
init()
{
  writingSystem_ = QFontDatabase::Any;

  initWidgets();
}

void
CQFontDialog::
initWidgets()
{
  setObjectName("fontDialog");

  setWindowTitle("Select Font");

  //setSizeGripEnabled(true);

  // grid layout
  QGridLayout *mainGrid = new QGridLayout(this);
  mainGrid->setMargin(2); mainGrid->setSpacing(2);

  // font family
  familyEdit_ = new QLineEdit;
  familyEdit_->setObjectName("familyEdit");
  familyEdit_->setReadOnly(true);

  familyList_ = new CQFontListView;
  familyList_->setObjectName("familyList");
  familyEdit_->setFocusProxy(familyList_);

  familyAccel_ = new QLabel("&Font");
  familyAccel_->setObjectName("familyAccel");
  familyAccel_->setBuddy(familyList_);
  familyAccel_->setIndent(2);

  mainGrid->addWidget(familyAccel_, 0, 0);
  mainGrid->addWidget(familyEdit_ , 1, 0);
  mainGrid->addWidget(familyList_ , 2, 0);

  connect(familyList_, SIGNAL(highlighted(int)), this, SLOT(familyHighlighted(int)));

  //---

  // font style
  styleEdit_ = new QLineEdit;
  styleEdit_->setReadOnly(true);

  styleList_ = new CQFontListView;
  styleEdit_->setFocusProxy(styleList_);

  styleAccel_ = new QLabel("Font st&yle");
  styleAccel_->setBuddy(styleList_);
  styleAccel_->setIndent(2);

  mainGrid->addWidget(styleAccel_, 0, 2);
  mainGrid->addWidget(styleEdit_ , 1, 2);
  mainGrid->addWidget(styleList_ , 2, 2);

  connect(styleList_, SIGNAL(highlighted(int)), this, SLOT(styleHighlighted(int)));

  //---

  // font size
  sizeEdit_ = new QLineEdit;
  sizeEdit_->setFocusPolicy(Qt::ClickFocus);

  QIntValidator *validator = new QIntValidator(1, 512, this);
  sizeEdit_->setValidator(validator);

  sizeList_ = new CQFontListView;
  sizeEdit_->setFocusProxy(sizeList_);

  sizeAccel_ = new QLabel("&Size");
  sizeAccel_->setBuddy(sizeEdit_);
  sizeAccel_->setIndent(2);

  mainGrid->addWidget(sizeAccel_, 0, 4);
  mainGrid->addWidget(sizeEdit_ , 1, 4);
  mainGrid->addWidget(sizeList_ , 2, 4);

  connect(sizeList_, SIGNAL(highlighted(int)), this, SLOT(sizeHighlighted(int)));
  connect(sizeEdit_, SIGNAL(textChanged(QString)), this, SLOT(sizeChanged(QString)));

  //---

  // font effects box
  QGroupBox *effectsGroup = new QGroupBox("Effects");

  QVBoxLayout *effectsLayout = new QVBoxLayout(effectsGroup);

  strikeout_ = new QCheckBox("Stri&keout");
  underline_ = new QCheckBox("&Underline");

  effectsLayout->addWidget(strikeout_);
  effectsLayout->addWidget(underline_);

  connect(strikeout_, SIGNAL(clicked()), this, SLOT(updateSample()));
  connect(underline_, SIGNAL(clicked()), this, SLOT(updateSample()));

  mainGrid->addWidget(effectsGroup, 4, 0);

  //---

  // font sample
  QGroupBox *sampleGroup = new QGroupBox("Sample");

  QHBoxLayout *sampleLayout = new QHBoxLayout(sampleGroup);

  sampleEdit_ = new QLineEdit;
  sampleEdit_->setObjectName("sampleEdit");
  sampleEdit_->setSizePolicy(QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
  sampleEdit_->setAlignment(Qt::AlignCenter);
  sampleEdit_->setText("AaBbYyZz");

  sampleLayout->addWidget(sampleEdit_);

  mainGrid->addWidget(sampleGroup, 4, 2, 4, 3);

  //---

  // font writing system
  writingSystemCombo_ = new QComboBox;

  writingSystemAccel_ = new QLabel("Wr&iting System");
  writingSystemAccel_->setBuddy(writingSystemCombo_);
  writingSystemAccel_->setIndent(2);

  connect(writingSystemCombo_, SIGNAL(activated(int)),
          this, SLOT(writingSystemHighlighted(int)));

  //---

  size_           = 0;
  smoothScalable_ = false;

  for (int i = 0; i < QFontDatabase::WritingSystemsCount; ++i) {
    QFontDatabase::WritingSystem ws = QFontDatabase::WritingSystem(i);

    QString writingSystemName = QFontDatabase::writingSystemName(ws);

    if (writingSystemName.isEmpty())
      break;

    writingSystemCombo_->addItem(writingSystemName);
  }

  //---

  updateFamilies();

  if (familyList_->count() != 0) {
    familyList_->setCurrentItem(0);
    sizeList_  ->setCurrentItem(0);
  }

  mainGrid->addWidget(writingSystemAccel_, 5, 0);
  mainGrid->addWidget(writingSystemCombo_, 7, 0);

  buttonBox_ = new QDialogButtonBox;

  mainGrid->addWidget(buttonBox_, 9, 0, 1, 5);

  QPushButton *button = static_cast<QPushButton *>(buttonBox_->addButton(QDialogButtonBox::Ok));

  connect(buttonBox_, SIGNAL(accepted()), this, SLOT(accept()));

  button->setDefault(true);

  buttonBox_->addButton(QDialogButtonBox::Cancel);

  QObject::connect(buttonBox_, SIGNAL(rejected()), this, SLOT(reject()));

  //---

  mainGrid->setColumnStretch(0, 38);
  mainGrid->setColumnStretch(2, 24);
  mainGrid->setColumnStretch(4, 10);

  //---

  sizeEdit_  ->installEventFilter(this);
  familyList_->installEventFilter(this);
  styleList_ ->installEventFilter(this);
  sizeList_  ->installEventFilter(this);

  familyList_->setFocus();
}

void
CQFontDialog::
setOption(FontDialogOption option, bool on)
{
  const FontDialogOptions previousOptions = this->options();

  if (! (previousOptions & option) != !on)
    setOptions(previousOptions ^ option);
}

bool
CQFontDialog::
testOption(FontDialogOption option) const
{
  return (this->options_ & option);
}

void
CQFontDialog::
setOptions(FontDialogOptions options)
{
  if (this->options() == options)
    return;

  options_ = options;

  buttonBox_->setVisible(! (options & NoButtons));
}

CQFontDialog::FontDialogOptions
CQFontDialog::
options() const
{
  return options_;
}

bool
CQFontDialog::
eventFilter(QObject *o , QEvent *e)
{
  if (e->type() == QEvent::KeyPress) {
    QKeyEvent *k = (QKeyEvent *)e;

    if      (o == sizeEdit_ &&
             (k->key() == Qt::Key_Up     || k->key() == Qt::Key_Down ||
              k->key() == Qt::Key_PageUp || k->key() == Qt::Key_PageDown)) {
      int ci = sizeList_->currentItem();

      (void)QApplication::sendEvent(sizeList_, k);

      if (ci != sizeList_->currentItem() &&
          style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, this))
        sizeEdit_->selectAll();
      return true;
    }
    else if ((o == familyList_ || o == styleList_) &&
             (k->key() == Qt::Key_Return || k->key() == Qt::Key_Enter)) {
      k->accept();

      accept();

      return true;
    }
  }
  else if (e->type() == QEvent::FocusIn &&
           style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, this)) {
    if      (o == familyList_)
      familyEdit_->selectAll();
    else if (o == styleList_)
      styleEdit_->selectAll();
    else if (o == sizeList_)
      sizeEdit_->selectAll();
  }
  else if (e->type() == QEvent::MouseButtonPress && o == sizeList_) {
    sizeEdit_->setFocus();
  }

  return QDialog::eventFilter(o, e);
}

void
CQFontDialog::
updateFamilies()
{
  enum match_t { MATCH_NONE = 0, MATCH_LAST_RESORT = 1, MATCH_APP = 2, MATCH_FAMILY = 3 };

  const CQFontDialog::FontDialogOptions scalableMask =
    (CQFontDialog::ScalableFonts | CQFontDialog::NonScalableFonts);
  const CQFontDialog::FontDialogOptions spacingMask =
    (CQFontDialog::ProportionalFonts | CQFontDialog::MonospacedFonts);
  const CQFontDialog::FontDialogOptions options = this->options();

  QFontDatabase fdb;

  QStringList familyNames;

  const auto families = fdb.families(writingSystem_);

  for (const QString &family : families) {
    if (fdb.isPrivateFamily(family))
      continue;

    if ((options & scalableMask) && (options & scalableMask) != scalableMask) {
      if (bool(options & CQFontDialog::ScalableFonts) != fdb.isSmoothlyScalable(family))
        continue;
    }

    if ((options & spacingMask) && (options & spacingMask) != spacingMask) {
      if (bool(options & CQFontDialog::MonospacedFonts) != fdb.isFixedPitch(family))
        continue;
    }

    familyNames << family;
  }

  familyList_->model()->setStringList(familyNames);

  QString foundryName1, familyName1, foundryName2, familyName2;

  int     bestFamilyMatch = -1;
  match_t bestFamilyType  = MATCH_NONE;

  QFont f;

  // ##### do the right thing for a list of family names in the font.
  parseFontName(family_, foundryName1, familyName1);

  int i = 0;

  for (QStringList::const_iterator it = familyNames.constBegin();
        it != familyNames.constEnd(); ++it, ++i) {
    parseFontName(*it, foundryName2, familyName2);

    //try to match...
    if (familyName1 == familyName2) {
      bestFamilyType = MATCH_FAMILY;

      if (foundryName1 == foundryName2) {
        bestFamilyMatch = i;
        break;
      }

      if (bestFamilyMatch < MATCH_FAMILY)
        bestFamilyMatch = i;
    }

//and try some fall backs
    match_t type = MATCH_NONE;

    if (bestFamilyType <= MATCH_NONE && familyName2 == f.lastResortFamily())
      type = MATCH_LAST_RESORT;

    if (bestFamilyType <= MATCH_LAST_RESORT && familyName2 == f.family())
      type = MATCH_APP;

    // ### add fallback for writingSystem
    if (type != MATCH_NONE) {
      bestFamilyType  = type;
      bestFamilyMatch = i;
    }
  }

  if (i != -1 && bestFamilyType != MATCH_NONE)
    familyList_->setCurrentItem(bestFamilyMatch);
  else
    familyList_->setCurrentItem(0);

  familyEdit_->setText(familyList_->currentText());

  if (style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, this) &&
      familyList_->hasFocus())
    familyEdit_->selectAll();

  updateStyles();
}

/*
    Updates the contents of the "font style" list box. This
    function can be reimplemented if you have special requirements.
*/
void
CQFontDialog::
updateStyles()
{
  QStringList styles = fdb_.styles(familyList_->currentText());

  styleList_->model()->setStringList(styles);

  if (styles.isEmpty()) {
    styleEdit_->clear();

    smoothScalable_ = false;
  }
  else {
    if (! style_.isEmpty()) {
      bool found = false;
      bool first = true;

      QString cstyle = style_;

    redo:
      for (int i = 0; i < (int)styleList_->count(); i++) {
        if (cstyle == styleList_->text(i)) {
          styleList_->setCurrentItem(i);
          found = true;
          break;
        }
      }
      if (!found && first) {
        if (cstyle.contains(QLatin1String("Italic"))) {
          cstyle.replace(QLatin1String("Italic"), QLatin1String("Oblique"));
          first = false;
          goto redo;
        } else if (cstyle.contains(QLatin1String("Oblique"))) {
          cstyle.replace(QLatin1String("Oblique"), QLatin1String("Italic"));
          first = false;
          goto redo;
        }
      }
      if (!found)
        styleList_->setCurrentItem(0);
    }
    else {
      styleList_->setCurrentItem(0);
    }

    styleEdit_->setText(styleList_->currentText());
    if (style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, this) &&
        styleList_->hasFocus())
      styleEdit_->selectAll();

    smoothScalable_ =
      fdb_.isSmoothlyScalable(familyList_->currentText(), styleList_->currentText());
  }

  updateSizes();
}

/*!
  \internal
  Updates the contents of the "font size" list box. This
  function can be reimplemented if you have special requirements.
*/

void
CQFontDialog::
updateSizes()
{
  if (! familyList_->currentText().isEmpty()) {
    QList<int> sizes = fdb_.pointSizes(familyList_->currentText(), styleList_->currentText());

    int i       = 0;
    int current = -1;

    QStringList str_sizes;
    str_sizes.reserve(sizes.size());

    for (QList<int>::const_iterator it = sizes.constBegin(); it != sizes.constEnd(); ++it) {
      str_sizes.append(QString::number(*it));

      if (current == -1 && *it == size_)
        current = i;

      ++i;
    }

    sizeList_->model()->setStringList(str_sizes);

    if (current != -1)
      sizeList_->setCurrentItem(current);

    const QSignalBlocker blocker(sizeEdit_);

    sizeEdit_->setText((smoothScalable_ ? QString::number(size_) : sizeList_->currentText()));

    if (style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, this) &&
        sizeList_->hasFocus())
      sizeEdit_->selectAll();
  }
  else {
    sizeEdit_->clear();
  }

  updateSample();
}

void
CQFontDialog::
updateSample()
{
  // compute new font
  int pSize = sizeEdit_->text().toInt();

  QFont newFont(fdb_.font(familyList_->currentText(), style_, pSize));

  newFont.setStrikeOut(strikeout_->isChecked());
  newFont.setUnderline(underline_->isChecked());

  if (familyList_->currentText().isEmpty())
    sampleEdit_->clear();

  updateSampleFont(newFont);
}

void
CQFontDialog::
updateSampleFont(const QFont &newFont)
{
  if (newFont != sampleEdit_->font()) {
    sampleEdit_->setFont(newFont);

    emit currentFontChanged(newFont);
  }
}

void
CQFontDialog::
setWritingSystem(QFontDatabase::WritingSystem ws)
{
  writingSystem_ = ws;

  if (writingSystemCombo_->currentIndex() != int(writingSystem_))
    writingSystemCombo_->setCurrentIndex(int(writingSystem_));

  sampleEdit_->setText(fdb_.writingSystemSample(writingSystem_));

  updateFamilies();
}

void
CQFontDialog::
writingSystemHighlighted(int index)
{
  setWritingSystem(QFontDatabase::WritingSystem(index));
}

void
CQFontDialog::
familyHighlighted(int i)
{
  family_ = familyList_->text(i);

  familyEdit_->setText(family_);

  if (style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, this) &&
      familyList_->hasFocus())
    familyEdit_->selectAll();

  updateStyles();
}

void
CQFontDialog::
styleHighlighted(int index)
{
  QString s = styleList_->text(index);

  styleEdit_->setText(s);

  if (style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, this) &&
      styleList_->hasFocus())
    styleEdit_->selectAll();

  style_ = s;

  updateSizes();
}

void
CQFontDialog::
sizeHighlighted(int index)
{
  QString s = sizeList_->text(index);

  sizeEdit_->setText(s);

  if (style()->styleHint(QStyle::SH_FontDialog_SelectAssociatedText, 0, this) &&
      sizeEdit_->hasFocus())
    sizeEdit_->selectAll();

  size_ = s.toInt();

  updateSample();
}

void
CQFontDialog::
sizeChanged(const QString &s)
{
  // no need to check if the conversion is valid, since we have an QIntValidator in the size edit
  int size = s.toInt();

  if (this->size_ == size)
    return;

  this->size_ = size;

  if (sizeList_->count() != 0) {
    int i;

    for (i = 0; i < sizeList_->count() - 1; i++) {
      if (sizeList_->text(i).toInt() >= this->size_)
        break;
    }

    const QSignalBlocker blocker(sizeList_);

    if (sizeList_->text(i).toInt() == this->size_)
      sizeList_->setCurrentItem(i);
    else
      sizeList_->clearSelection();
  }

  updateSample();
}

void
CQFontDialog::
setCurrentFont(const QFont &font)
{
  family_ = font.family();
  style_  = fdb_.styleString(font);
  size_   = font.pointSize();

  if (size_ == -1) {
    QFontInfo fi(font);

    size_ = fi.pointSize();
  }

  strikeout_->setChecked(font.strikeOut());
  underline_->setChecked(font.underline());

  updateFamilies();
}

QFont
CQFontDialog::
currentFont() const
{
  return sampleEdit_->font();
}

QFont
CQFontDialog::
selectedFont() const
{
  return selectedFont_;
}

void
CQFontDialog::
done(int result)
{
  QDialog::done(result);

  if (result == Accepted) {
    // We check if this is the same font we had before, if so we emit currentFontChanged
    QFont selectedFont = currentFont();

    if (selectedFont != this->selectedFont_)
      emit currentFontChanged(selectedFont);

    selectedFont_ = selectedFont;

    emit fontSelected(selectedFont_);
  }
  else
    selectedFont_ = QFont();
}
