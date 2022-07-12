#include <QDialog>

#undef slots

namespace CQCommand {
class ScrollArea;
}

#ifdef USE_PYTHON
#include <Python.h>
#endif

class CQCommandTest : public QDialog {
  Q_OBJECT

 public:
  CQCommandTest();
 ~CQCommandTest();

  QSize sizeHint() const override;

 public Q_SLOTS:
  void executeCommand(const QString &);

 private:
  static void clOutputProc(const char *str, void *);

 private:
  static CQCommandTest *instance_;

  CQCommand::ScrollArea *command_ { nullptr };

#ifdef USE_PYTHON
  PyObject *pyMain_ { nullptr };
  PyObject *pyDict_ { nullptr };
#endif
};
