#include <CQApp.h>
#include <CQCommandTest.h>
#include <CQCommand.h>
#include <CQUtil.h>
#include <QVBoxLayout>

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

#if defined(USE_PYTHON)
  pyMain_ = PyImport_AddModule("__main__");
  pyDict_ = PyModule_GetDict(pyMain_);
#endif
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

#if 1
  static std::string lastRes;

  auto *ret = PyRun_String(str.toLatin1().constData(), Py_file_input, pyDict_, pyDict_);
  PyRun_SimpleString("\n");

  if (ret) { // success
    auto* repr    = PyObject_Repr(ret);
    auto *reprStr = (repr ? PyUnicode_AsEncodedString(repr, "utf-8", "~E~") : nullptr);

    if (reprStr) {
      auto *reprChars = PyBytes_AS_STRING(reprStr);

      lastRes = reprChars;

      //std::cerr << reprChars << "\n";

      Py_XDECREF(reprStr);
    }

    Py_XDECREF(repr);
  }
  else {
    PyErr_Print();
  }
#else
  if (PyRun_SimpleString(str.toLatin1().constData()) == 0) {
  }
#endif

  std::string ostr;

  COSExec::readGrabbedOutput(ostr);

  instance_->command_->outputText(ostr.c_str());

  //COSExec::ungrabOutput();
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
