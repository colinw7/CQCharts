#include <CQChartsEnv.h>
#include <cstdlib>
#include <cstring>
#include <cassert>

namespace CQChartsEnv {

QString getString(const char *name, const QString &def) {
  auto *env = getenv(name);
  if (! env) return def;

  return QString(env);
}

bool getBool(const char *name, bool def) {
  auto *env = getenv(name);
  if (! env) return def;

  if      (strcmp(env, "0") == 0 || strcmp(env, "false") == 0 || strcmp(env, "no" ) == 0)
    return false;
  else if (strcmp(env, "1") == 0 || strcmp(env, "true" ) == 0 || strcmp(env, "yes") == 0)
    return true;
  else
    return def;

  return true;
}

int getInt(const char *name, int def) {
  auto *env = getenv(name);
  if (! env) return def;

  return atoi(env);
}

}
