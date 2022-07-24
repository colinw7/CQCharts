#include <CPython.h>
#include <CStrParse.h>

CPython::
CPython()
{
}

bool
CPython::
isCompleteLine(const std::string &line)
{
  startStringParse(line);

  bool rc = isCompleteLine1('\0');

  endParse();

  return rc;
}

bool
CPython::
isCompleteLine1(char endChar)
{
  while (! parse_->eof()) {
    if      (parse_->isChar('[')) {
      parse_->skipChar();

      if (! isCompleteLine1(']'))
        return false;

      if (! parse_->isChar(']'))
        return false;

      parse_->skipChar();
    }
    else if (parse_->isChar('{')) {
      parse_->skipChar();

      if (! isCompleteLine1('}'))
        return false;

      if (! parse_->isChar('}'))
        return false;

      parse_->skipChar();
    }
    else if (parse_->isChar(endChar)) {
      return true;
    }
    // TODO """
    else if (parse_->isChar('\"')) {
      parse_->skipChar();

      if (! isCompleteLine1('\"'))
        return false;

      if (! parse_->isChar('\"'))
        return false;

      parse_->skipChar();
    }
    // TODO '''
    else if (parse_->isChar('\'')) {
      parse_->skipChar();

      if (! isCompleteLine1('\''))
        return false;

      if (! parse_->isChar('\''))
        return false;

      parse_->skipChar();
    }
    else if (parse_->isChar('\\')) {
      parse_->skipChar();
      parse_->skipChar();
    }
    else
      parse_->skipChar();
  }

  return true;
}

void
CPython::
startStringParse(const std::string &str)
{
  parseStack_.push_back(parse_);

  parse_ = new CStrParse(str);
}

void
CPython::
endParse()
{
  delete parse_;

  parse_ = parseStack_.back();

  parseStack_.pop_back();
}
