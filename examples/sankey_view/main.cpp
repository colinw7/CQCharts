#include <CQChartsSankeyView.h>
#include <QApplication>

int
main(int argc, char **argv)
{
  QApplication app(argc, argv);

  auto *mw = new MainWindow;

  mw->show();

  return app.exec();
}
