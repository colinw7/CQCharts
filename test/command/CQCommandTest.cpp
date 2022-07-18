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

//---

class TestCmdWidget : public CQCommand::CommandWidget {
 public:
  TestCmdWidget(CQCommand::ScrollArea *scrollArea) :
   CQCommand::CommandWidget(scrollArea) {
  }

  bool complete(const QString &text, int pos,
                QString &newText, CompleteMode completeMode) const override {
    return CQCommand::CommandWidget::complete(text, pos, newText, completeMode);
  }

  bool isCompleteLine(const QString &str) const override {
#if defined(USE_PYTHON)
    auto len = size_t(str.length());

    if (len && str[int(len - 1)] == '\\')
      return false;

    size_t i = 0;

    bool in_str1 = false; QChar istr1 = '\0';
    bool in_str2 = false; QChar istr2 = '\0';
    bool in_str3 = false; QChar istr3 = '\0';

    int in_brackets1 = 0;
    int in_brackets2 = 0;
    int in_brackets3 = 0;

    int in_block = 0;

    std::vector<int> indents;

    while (i < len && str[int(i)].isSpace())
      ++i;

    int indent     = int(i);
    int nextIndent = -1;

    if (indent > 0)
      return true;

    while (i < len) {
      if (! in_str1 && ! in_str2 && ! in_str3) {
        if (str[int(i)].isSpace()) {
          bool newline = (str[int(i)] == '\n');

          if (newline)
            ++i;

          auto j = i;

          while (i < len && str[int(i)].isSpace())
            ++i;

          if (newline) {
            auto indent1 = i - j;

            if (nextIndent == -1) {
              if (int(indent1) <= indent)
                return true;

              nextIndent = int(indent1);
              indent     = nextIndent;
            }
            else {
              if      (int(indent1) < indent) {
                --in_block;

                if (in_block < 0)
                  return true;
              }
              else if (int(indent1) > indent) {
                return true;
              }
            }

            indents.push_back(indent);
          }

          if (i >= len)
            break;
        }

        if      (str[int(i)] == '\"' || str[int(i)] == '\'') {
          auto c1 = str[int(i)];

          if (i < len - 1 && str[int(i + 1)] == c1) {
            if (i < len - 2 && str[int(i + 2)] == c1) {
              in_str3 = true;
              istr3   = c1;

              i += 3;
            }
            else {
              in_str2 = true;
              istr2   = c1;

              i += 2;
            }
          }
          else {
            in_str1 = true;
            istr1   = c1;

            ++i;
          }
        }
        else if (str[int(i)] == '(') {
          ++in_brackets1;

          ++i;
        }
        else if (str[int(i)] == '[') {
          ++in_brackets2;

          ++i;
        }
        else if (str[int(i)] == '{') {
          ++in_brackets3;

          ++i;
        }
        else if (str[int(i)] == ')') {
          --in_brackets1;

          ++i;
        }
        else if (str[int(i)] == ']') {
          --in_brackets2;

          ++i;
        }
        else if (str[int(i)] == '}') {
          --in_brackets3;

          ++i;
        }
        else if (str[int(i)] == ':') {
          if (! in_brackets1 && ! in_brackets2 && ! in_brackets3) {
            ++in_block;

            nextIndent = -1;
          }

          ++i;
        }
        else {
          ++i;
        }
      }
      else if (in_str1) {
        if      (str[int(i)] == '\\' && i < len - 1)
          i += 2;
        else if (str[int(i)] == istr1) {
          in_str1 = false;
          ++i;
        }
        else
          ++i;
      }
      else if (in_str2) {
        if      (str[int(i)] == '\\' && i < len - 1)
          i += 2;
        else if (str[int(i)] == istr2 && i < len - 1 && str[int(i + 1)] == istr2) {
          in_str2 = false;
          i += 2;
        }
        else
          ++i;
      }
      else if (in_str3) {
        if      (str[int(i)] == '\\' && i < len - 1)
          i += 2;
        else if (str[int(i)] == istr3 && i < len - 2 &&
                 str[int(i + 1)] == istr3 && str[int(i + 2)] == istr3) {
          in_str3 = false;
          i += 3;
        }
        else
          ++i;
      }
      else {
        ++i;
      }
    }

    if (in_brackets1 > 0 || in_brackets2 > 0 || in_brackets3 > 0)
      return false;

    if (in_str3)
      return false;

    if (in_block)
      return false;

    return true;
#else
    return CQCommand::CommandWidget::isCompleteLine(str);
#endif
  }
};

//---

class TestCmdScroll : public CQCommand::ScrollArea {
 public:
  TestCmdScroll(QWidget *parent=nullptr) :
   CQCommand::ScrollArea(parent) {
  }

  CQCommand::CommandWidget *createCommandWidget() const override {
    auto *scrollArea = dynamic_cast<const CQCommand::ScrollArea *>(this);

    return new TestCmdWidget(const_cast<CQCommand::ScrollArea *>(scrollArea));
  }
};

//---

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

  command_ = new TestCmdScroll(this);

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
