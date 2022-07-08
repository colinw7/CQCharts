#include <QDialog>

namespace CQCommand {
class ScrollArea;
}

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
};
