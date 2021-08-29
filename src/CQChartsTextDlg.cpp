#include <CQChartsTextDlg.h>
#include <CQChartsPropertyViewTree.h>
#include <CQPropertyViewModel.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsViewPlotPaintDevice.h>
#include <CQCharts.h>
#include <QHBoxLayout>

CQChartsTextDlg::
CQChartsTextDlg(QWidget *parent) :
 QDialog(parent)
{
  setWindowTitle("Text Dialog");

  setObjectName("textDlg");

  auto *layout = CQUtil::makeLayout<QHBoxLayout>(this, 2, 2);

  canvas_ = new CQChartsTextCanvas;

  propertyModel_ = new CQPropertyViewModel;
  propertyTree_  = new CQPropertyViewTree(this, propertyModel_);

  layout->addWidget(canvas_);
  layout->addWidget(propertyTree_);

  auto addProp = [&](const QString &name) {
    return propertyModel_->addProperty("", canvas_, name);
  };

  auto addStyleProp = [&](const QString &name) {
    auto *item = addProp(name);
    CQCharts::setItemIsStyle(item);
    return item;
  };

  addProp("text");

  addStyleProp("color"        );
  addStyleProp("alpha"        );
  addStyleProp("font"         );
  addStyleProp("angle"        );
  addStyleProp("contrast"     );
  addStyleProp("contrastAlpha");
  addStyleProp("align"        );
  addStyleProp("formatted"    );
  addStyleProp("scaled"       );
  addStyleProp("html"         );
  addStyleProp("clipLength"   );
  addStyleProp("clipElide"    );
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

  CQChartsPixelPaintDevice device(&painter);

  CQChartsTextOptions textOptions;

  textOptions.angle            = angle();
  textOptions.contrast         = isContrast();
  textOptions.contrastAlpha    = contrastAlpha();
  textOptions.align            = align();
  textOptions.formatted        = isFormatted();
  textOptions.scaled           = isScaled();
  textOptions.html             = isHtml();
  textOptions.clipLength       = device.lengthPixelWidth(clipLength());
  textOptions.clipElide        = clipElide();
  textOptions.minScaleFontSize = 4.0;
  textOptions.maxScaleFontSize = 400.0;

  CQChartsDrawUtil::drawTextInBox(&device, CQChartsGeom::BBox(rect_), text_, textOptions);
}

QSize
CQChartsTextCanvas::
sizeHint() const
{
  return QSize(600, 600);
}
