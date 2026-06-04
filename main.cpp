#include "DebuggerApplication.hpp"

int main(int argc, char *argv[]) {
  // QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  // QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
  DebuggerApplication app(argc, argv);
  return app.exec();
}
