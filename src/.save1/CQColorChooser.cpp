#include <CQColorChooser.h>
#include <CQAlphaButton.h>
#include <CQPixmapCache.h>

#include <QLineEdit>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QColorDialog>

#include <svg/color_dialog_svg.h>

CQColorChooser::
CQColorChooser(QWidget *parent) :
 QWidget(parent), styles_(Text | ColorButton), editable_(true)
{
  init();
}

CQColorChooser::
CQColorChooser(uint styles, QWidget *parent) :
 QWidget(parent), styles_(styles), editable_(true)
{
  init();
}

void
CQColorChooser::
init()
{
  setObjectName("colorChooser");

  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  color_     = QColor(1,2,3);
  alpha_     = 0.456;
  colorName_ = "abcdef";

  //-----

  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setMargin(0); layout->setSpacing(2);

  cedit_   = new QLineEdit    (this); cedit_  ->setObjectName("cedit"  ); // text color
  cbutton_ = new QToolButton  (this); cbutton_->setObjectName("cbutton"); // click color button
  clabel_  = new QLabel       (this); clabel_ ->setObjectName("clabel" ); // static color button
  button_  = new QToolButton  (this); button_ ->setObjectName("button" ); // image click button
  alphab_  = new CQAlphaButton(this); alphab_ ->setObjectName("alphab" ); // alpha button

  //cbutton_->setFixedSize(QSize(24,24));
  //clabel_ ->setFixedSize(QSize(24,24));
  //button_ ->setFixedSize(QSize(24,24));

  button_->setIcon(CQPixmapCacheInst->getIcon("COLOR_DIALOG"));

  layout->addWidget(cedit_  );
  layout->addWidget(cbutton_);
  layout->addWidget(clabel_ );
  layout->addWidget(alphab_ );
  layout->addWidget(button_ );

// TODO: add stretch widget and hide if any of the stretchable widgets are visible
//layout->addStretch();

  cedit_  ->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  cbutton_->setSizePolicy(QSizePolicy::Fixed    , QSizePolicy::Fixed);
  clabel_ ->setSizePolicy(QSizePolicy::Fixed    , QSizePolicy::Fixed);
  button_ ->setSizePolicy(QSizePolicy::Fixed    , QSizePolicy::Fixed);

  connect(cedit_  , SIGNAL(editingFinished()), this, SLOT(textColorChanged()));
  connect(cbutton_, SIGNAL(clicked()), this, SLOT(colorButtonClicked()));
  connect(button_ , SIGNAL(clicked()), this, SLOT(imageButtonClicked()));
  connect(alphab_ , SIGNAL(valueChanged()), this, SLOT(alphaButtonChanged()));

  update();

  setColor(color_, alpha_);

  setFocusProxy(cedit_);
}

void
CQColorChooser::
setEditable(bool editable)
{
  editable_ = editable;

  update();
}

void
CQColorChooser::
setStyles(uint styles)
{
  styles_ = styles;

  update();
}

void
CQColorChooser::
update()
{
  cedit_  ->setVisible(styles_ & Text       );
  cbutton_->setVisible(styles_ & ColorButton);
  clabel_ ->setVisible(styles_ & ColorLabel );
  button_ ->setVisible(styles_ & ImageButton);
  alphab_ ->setVisible(styles_ & AlphaButton);

  cedit_->setReadOnly(! editable_);
}

void
CQColorChooser::
setColor(const QColor &color, double alpha)
{
  if (! color.isValid()) {
    if (! color_.isValid())
      return;

    color_     = color;
    colorName_ = "";
  }
  else {
    QColor color1(color);

    color1.setAlpha(255);

    if (color_ == color1 && alpha_ == alpha_)
      return;

    color_     = color1;
    colorName_ = color_.name();
  }

  cedit_->setText(colorName_);

  QPixmap pixmap(24, 24);

  pixmap.fill(color_);

  cbutton_->setIcon(QIcon(QPixmap(pixmap)));

  if (color_.isValid())
    setWidgetBackground(clabel_, color_);

  alpha_ = alpha;

  alphab_->setColor(color_);
  alphab_->setAlpha(alpha_);
}

void
CQColorChooser::
setColorName(const QString &colorName, double alpha)
{
  QColor color;

  if (colorName.length()) {
    color = QColor(colorName);

    if (! color.isValid())
      return;
  }

  QColor color1(color);

  color1.setAlpha(255);

  if (color_ == color1 && alpha_ == alpha_)
    return;

  color_     = color1;
  colorName_ = colorName;

  cedit_->setText(colorName_);

  QPixmap pixmap(24, 24);

  pixmap.fill(color_);

  cbutton_->setIcon(QIcon(QPixmap(pixmap)));

  setWidgetBackground(clabel_, color_);

  alpha_ = alpha;

  alphab_->setAlpha(alpha_);
}

const QColor &
CQColorChooser::
color() const
{
  return color_;
}

QString
CQColorChooser::
colorName() const
{
  return colorName_;
}

void
CQColorChooser::
setAlpha(double alpha)
{
  alpha_ = alpha;

  alphab_->setAlpha(alpha_);
}

double
CQColorChooser::
getAlpha() const
{
  return alpha_;
}

void
CQColorChooser::
textColorChanged()
{
  setColorName(cedit_->text(), alpha_);

  changeColor();
}

void
CQColorChooser::
imageButtonClicked()
{
  QColor color = QColorDialog::getColor(color_, this);

  setColor(color, alpha_);

  changeColor();
}

void
CQColorChooser::
colorButtonClicked()
{
  QColor color = QColorDialog::getColor(color_, this);

  setColor(color, alpha_);

  changeColor();
}

void
CQColorChooser::
alphaButtonChanged()
{
  setAlpha(alphab_->getAlpha());

  changeColor();
}

void
CQColorChooser::
changeColor()
{
  QColor color(color_);

  color_.setAlpha(255*alpha_);

  emit colorChanged(color);
  emit colorChanged(colorName_);
}

void
CQColorChooser::
applyColor()
{
  QColor color(color_);

  color_.setAlpha(255*alpha_);

  emit colorApplied(color);
  emit colorApplied(colorName_);
}

void
CQColorChooser::
setWidgetBackground(QWidget *widget, const QColor &color)
{
  QPalette palette = widget->palette();

  palette.setColor(widget->backgroundRole(), color);

  widget->setPalette(palette);

  widget->setAutoFillBackground(true);
}
