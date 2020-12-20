#include <CQChartsCmdLine.h>
#include <CQCharts.h>
#include <CQChartsWidgetUtil.h>

#include <CQCommand.h>
#include <CQTclUtil.h>
#include <CTclParse.h>
#include <CQStrUtil.h>
#include <COSExec.h>
#include <COSProcess.h>

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

class CQChartsCmdWidget : public CQCommand::CommandWidget {
 public:
  CQChartsCmdWidget(CQChartsCmdLine *cmdLine);

  bool complete(const QString &text, int pos,
                QString &newText, CompleteMode completeMode) const override {
    bool interactive = (completeMode == CompleteMode::Interactive);

    return cmdLine_->complete(const_cast<CQChartsCmdWidget *>(this), text,
                              pos, newText, interactive);
  }

 private:
  CQChartsCmdLine *cmdLine_ { nullptr };
};

//---

class CQChartsCmdScrollArea : public CQCommand::ScrollArea {
 public:
  CQChartsCmdScrollArea(CQChartsCmdLine *cmdLine) :
   cmdLine_(cmdLine) {
  }

  CQCommand::CommandWidget *createCommandWidget() const override {
    return new CQChartsCmdWidget(cmdLine_);
  }

 private:
  CQChartsCmdLine *cmdLine_ { nullptr };
};

//---

CQChartsCmdWidget::
CQChartsCmdWidget(CQChartsCmdLine *cmdLine) :
 CQCommand::CommandWidget(nullptr), cmdLine_(cmdLine)
{
}

//---

CQChartsCmdLine::
CQChartsCmdLine(CQCharts *charts, QWidget *parent) :
 QFrame(parent), charts_(charts)
{
  setWindowTitle("Charts Command Line");

  setObjectName("cmdLine");

  //---

  auto *layout = new QVBoxLayout(this);

  command_ = new CQChartsCmdScrollArea(this);

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

bool
CQChartsCmdLine::
complete(CQChartsCmdWidget *widget, const QString &text, int pos,
         QString &newText, bool interactive) const
{
  auto resultToStrings = [&](const QVariant &var) {
    QStringList strs;

    if (var.type() == QVariant::List) {
      QList<QVariant> vars = var.toList();

      for (int i = 0; i < vars.length(); ++i) {
        QString str = vars[i].toString();

        strs.push_back(str);
      }
    }
    else {
      if (! CQTcl::splitList(var.toString(), strs))
        strs << var.toString();
    }

    return strs;
  };

  //---

  int len = text.length();

  if (pos >= len)
    pos = len - 1;

  //---

  CTclParse parse;

  CTclParse::Tokens tokens;

  parse.parseLine(text.toStdString(), tokens);

  auto *token = parse.getTokenForPos(tokens, pos);

  //---

  auto lhs = text.mid(0, token ? token->pos() : pos + 1);
  auto str = (token ? token->str() : "");
  auto rhs = text.mid(token ? token->endPos() + 1 : pos + 1);

  // complete command
  if      (token && token->type() == CTclToken::Type::COMMAND) {
    //std::cerr << "Command: " << str << "\n";

    const auto &cmds = qtcl_->commandNames();

    auto matchCmds = CQStrUtil::matchStrs(str.c_str(), cmds);

    QString matchStr;
    bool    exact = false;

    if (interactive && matchCmds.size() > 1) {
      matchStr = widget->showCompletionChooser(matchCmds);

      if (matchStr != "")
        exact = true;
    }

    //---

    newText = lhs;

    if (matchStr == "")
      newText += CQStrUtil::longestMatch(matchCmds, exact);
    else
      newText += matchStr;

    if (exact)
      newText += " ";

    newText += rhs;

    return (newText.length() > text.length());
  }
  // complete option
  else if (str[0] == '-') {
    // get previous command token for command name
    std::string command;

    for (int pos1 = pos - 1; pos1 >= 0; --pos1) {
      auto *token1 = parse.getTokenForPos(tokens, pos1);
      if (! token1) continue;

      if (token1->type() == CTclToken::Type::COMMAND) {
        command = token1->str();
        break;
      }
    }

    if (command == "")
      return false;

    auto option = str.substr(1);

    //---

    // get all options for interactive complete
    QString matchStr;

    if (interactive) {
      auto cmd = QString("complete -command {%1} -option {*} -all").
                         arg(command.c_str());

      QVariant res;

      (void) qtcl_->eval(cmd, res);

      QStringList strs = resultToStrings(res);

      auto matchStrs = CQStrUtil::matchStrs(option.c_str(), strs);

      if (matchStrs.size() > 1) {
        matchStr = widget->showCompletionChooser(matchStrs);

        if (matchStr != "")
          matchStr = "-" + matchStr + " ";
      }
    }

    //---

    if (matchStr == "") {
      // use complete command to complete command option
      auto cmd = QString("complete -command {%1} -option {%2} -exact_space").
                         arg(command.c_str()).arg(option.c_str());

      QVariant res;

      (void) qtcl_->eval(cmd, res);

      matchStr = res.toString();
    }

    newText = lhs + matchStr + rhs;

    return (newText.length() > text.length());
  }
  else {
    // get previous tokens for option name and command name
    using OptionValues = std::map<std::string, std::string>;

    std::string  command;
    int          commandPos { -1 };
    std::string  option;
    OptionValues optionValues;

    for (int pos1 = pos - 1; pos1 >= 0; --pos1) {
      auto *token1 = parse.getTokenForPos(tokens, pos1);
      if (! token1) continue;

      const auto &str = token1->str();
      if (str.empty()) continue;

      if      (token1->type() == CTclToken::Type::COMMAND) {
        command    = str;
        commandPos = token1->pos();
        break;
      }
      else if (str[0] == '-') {
        if (option.empty())
          option = str.substr(1);

        if (pos1 > token1->pos())
          pos1 = token1->pos(); // move to start
      }
    }

    if (command == "" || option == "")
      return false;

    // get option values to next command
    std::string lastOption;

    for (int pos1 = commandPos + command.length(); pos1 < text.length(); ++pos1) {
      auto *token1 = parse.getTokenForPos(tokens, pos1);
      if (! token1) continue;

      const auto &str = token1->str();
      if (str.empty()) continue;

      if      (token1->type() == CTclToken::Type::COMMAND) {
        break;
      }
      else if (str[0] == '-') {
        lastOption = str.substr(1);

        optionValues[lastOption] = "";

        pos1 = token1->pos() + token1->str().length(); // move to end
      }
      else {
        if (lastOption != "")
          optionValues[lastOption] = str;

        pos1 = token1->pos() + token1->str().length(); // move to end
      }
    }

    //---

    std::string nameValues;

    for (const auto &nv : optionValues) {
      if (! nameValues.empty())
        nameValues += " ";

      nameValues += "{{" + nv.first + "} {" + nv.second + "}}";
    }

    nameValues = "{" + nameValues + "}";

    //---

    // get all option values for interactive complete
    QString matchStr;

    if (interactive) {
      auto cmd =
        QString("complete -command {%1} -option {%2} -value {*} -name_values %3 -all").
                arg(command.c_str()).arg(option.c_str()).arg(nameValues.c_str());

      QVariant res;

      (void) qtcl_->eval(cmd, res);

      QStringList strs = resultToStrings(res);

      auto matchStrs = CQStrUtil::matchStrs(str.c_str(), strs);

      if (matchStrs.size() > 1) {
        matchStr = widget->showCompletionChooser(matchStrs);

        if (matchStr != "")
          matchStr = matchStr + " ";
      }
    }

    //---

    if (matchStr == "") {
      // use complete command to complete command option value
      QVariant res;

      auto cmd =
        QString("complete -command {%1} -option {%2} -value {%3} -name_values %4 -exact_space").
                arg(command.c_str()).arg(option.c_str()).arg(str.c_str()).arg(nameValues.c_str());

      qtcl_->eval(cmd, res);

      matchStr = res.toString();
    }

    newText = lhs + matchStr + rhs;

    return (newText.length() > text.length());
  }
}

void
CQChartsCmdLine::
executeCommand(const QString &line)
{
  auto line1 = line.trimmed();

  if (! line1.length())
    return;

  if (line1[0] == '!') {
    line1 = line.mid(1);

    std::string str;

    COSProcess::executeCommand(line1.toStdString(), str);

    command_->outputText(str.c_str());

    return;
  }

  COSExec::grabOutput();

  bool log = true;

  int rc = qtcl_->eval(line, /*showError*/true, /*showResult*/log);

  std::cout << std::flush;

  if (rc != TCL_OK)
    std::cerr << "Invalid line: '" << line.toStdString() + "'\n";

  std::string str;

  COSExec::readGrabbedOutput(str);

  COSExec::ungrabOutput();

  command_->outputText(str.c_str());
}

QSize
CQChartsCmdLine::
sizeHint() const
{
  QFontMetrics fm(font());

  return QSize(fm.width("X")*80, fm.height()*25);
}
