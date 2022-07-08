#include <CQApp.h>
#include <CQCommandTest.h>
#include <CQCommand.h>
#include <CQUtil.h>
#include <QVBoxLayout>

//#define USE_CEIL 1
#define USE_PYTHON 1

#if defined(USE_CEIL)
#include <CCeilL.h>
#endif

#if defined(USE_PYTHON)
#undef slots
#include <COSExec.h>
#include <Python.h>
#endif

int
main(int argc, char **argv)
{
  CQApp app(argc, argv);

  auto *test = new CQCommandTest;

  test->show();

  return app.exec();
}

CQCommandTest *CQCommandTest::instance_;

CQCommandTest::
CQCommandTest()
{
  instance_ = this;

#if defined(USE_CEIL)
  ClLanguageMgrInst->init(nullptr, nullptr);

  ClLanguageMgrInst->setOutputProc(clOutputProc, nullptr);
#endif

#if defined(USE_PYTHON)
  wchar_t *program = Py_DecodeLocale("CQCommandTest", NULL);
  Py_SetProgramName(program);

  Py_Initialize();

  //PyRun_SimpleString("import sys; sys.path.insert(0, '.')" );
#endif

  auto *layout = CQUtil::makeLayout<QVBoxLayout>(this, 0, 0);

  command_ = new CQCommand::ScrollArea(this);

  QString prompt;

#if defined(USE_CEIL)
  prompt = QString::fromStdString(ClLanguageMgrInst->getPrompt()) + " ";
#else
  prompt = "> ";
#endif

  command_->getCommand()->setPrompt(prompt);

  connect(command_, SIGNAL(executeCommand(const QString &)),
          this, SLOT(executeCommand(const QString &)));

  layout->addWidget(command_);
}

CQCommandTest::
~CQCommandTest()
{
#if defined(USE_PYTHON)
  Py_Finalize();
#endif
}

void
CQCommandTest::
executeCommand(const QString &str)
{
#if defined(USE_CEIL)
  if (ClLanguageMgrInst->runCommand(str.toStdString())) {
    ClLanguageMgrInst->term();

    exit(ClLanguageMgrInst->getExitCode());
  }
#elif defined(USE_PYTHON)
  COSExec::grabOutput(true, true);

  if (PyRun_SimpleString(str.toLatin1().constData()) == 0) {
  }

  std::string ostr;

  COSExec::readGrabbedOutput(ostr);

  COSExec::ungrabOutput();

  instance_->command_->outputText(ostr.c_str());
#else
  std::cerr << str.toStdString() << "\n";
#endif

#if defined(USE_CEIL)
  QString prompt;

  prompt = QString::fromStdString(ClLanguageMgrInst->getPrompt()) + " ";

  command_->getCommand()->setPrompt(prompt);
#endif
}

void
CQCommandTest::
clOutputProc(const char *str, void *)
{
  instance_->command_->outputText(str);
}

QSize
CQCommandTest::
sizeHint() const
{
  return QSize(1200, 1600);
}
