#include <CQChartsTextDlg.h>
#include <CQChartsPropertyViewTree.h>
#include <CQPropertyViewModel.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <QHBoxLayout>

CQChartsTextDlg::
CQChartsTextDlg(QWidget *parent) :
 QDialog(parent)
{
  setWindowTitle("Text Dialog");

  setObjectName("textDlg");

  auto layout = CQUtil::makeLayout<QHBoxLayout>(this, 2, 2);

  canvas_ = new CQChartsTextCanvas;

  propertyModel_ = new CQPropertyViewModel;
  propertyTree_  = new CQPropertyViewTree(this, propertyModel_);

  layout->addWidget(canvas_);
  layout->addWidget(propertyTree_);

  propertyModel_->addProperty("", canvas_, "text"         );
  propertyModel_->addProperty("", canvas_, "color"        );
  propertyModel_->addProperty("", canvas_, "alpha"        );
  propertyModel_->addProperty("", canvas_, "font"         );
  propertyModel_->addProperty("", canvas_, "angle"        );
  propertyModel_->addProperty("", canvas_, "contrast"     );
  propertyModel_->addProperty("", canvas_, "contrastAlpha");
  propertyModel_->addProperty("", canvas_, "align"        );
  propertyModel_->addProperty("", canvas_, "formatted"    );
  propertyModel_->addProperty("", canvas_, "scaled"       );
  propertyModel_->addProperty("", canvas_, "html"         );
}

QSize
CQChartsTextDlg::
sizeHint() const
{
  return QSize(800, 600);
}

//---

CQChartsTextCanvas::
CQChartsTextCanvas(QWidget *parent) :
 QWidget(parent)
{
}

void
CQChartsTextCanvas::
paintEvent(QPaintEvent *)
{
  QPainter painter(this);

  painter.drawRect(rect_);

  CQChartsPixelPainter device(&painter);

  CQChartsTextOptions options;

  options.angle            = angle();
  options.contrast         = isContrast();
  options.contrastAlpha    = contrastAlpha();
  options.align            = align();
  options.formatted        = isFormatted();
  options.scaled           = isScaled();
  options.html             = isHtml();
  options.minScaleFontSize = 4.0;
  options.maxScaleFontSize = 400.0;

  CQChartsDrawUtil::drawTextInBox(&device, CQChartsGeom::BBox(rect_), text_, options);
}

QSize
CQChartsTextCanvas::
sizeHint() const
{
  return QSize(600, 600);
}
