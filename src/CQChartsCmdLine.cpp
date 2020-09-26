#include <CQChartsCmdLine.h>
#include <CQCharts.h>

#include <CQCommand.h>
#include <CQTclUtil.h>
#include <COSExec.h>

#include <QVBoxLayout>

#include <iostream>

CQChartsCmdLineDlgMgr::
CQChartsCmdLineDlgMgr()
{
}

void
CQChartsCmdLineDlgMgr::
showDialog(CQCharts *charts)
{
  if (dlg_ && dlg_->charts() != charts) {
    delete dlg_;

    dlg_ = nullptr;
  }

  if (! dlg_)
    dlg_ = new CQChartsCmdLine(charts);

  dlg_->show();

  dlg_->raise();
}

//---

CQChartsCmdLine::
CQChartsCmdLine(CQCharts *charts, QWidget *parent) :
 QFrame(parent), charts_(charts)
{
  setObjectName("cmdLine");

  //---

  auto *layout = new QVBoxLayout(this);

  command_ = new CQScrolledCommand;

  command_->getCommand()->setMinLines(10);
  command_->getCommand()->setPrompt("> ");

  connect(command_, SIGNAL(executeCommand(const QString &)),
          this, SLOT(executeCommand(const QString &)));

  layout->addWidget(command_);

  //---

  qtcl_ = charts_->cmdTcl();
}

CQChartsCmdLine::
~CQChartsCmdLine()
{
}

void
CQChartsCmdLine::
executeCommand(const QString &line)
{
  COSExec::grabOutput();

  bool log = true;

  int rc = qtcl_->eval(line, /*showError*/true, /*showResult*/log);

  std::cout << std::flush;

  if (rc != TCL_OK)
    std::cerr << "Invalid line: '" << line.toStdString() + "'\n";

  std::string str;

  COSExec::readGrabbedOutput(str);

  COSExec::ungrabOutput();

  command_->outputText(str);
}

QSize
CQChartsCmdLine::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("X")*80, fm.height()*25);
}
