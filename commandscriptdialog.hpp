#ifndef COMMANDSCRIPTDIALOG_HPP
#define COMMANDSCRIPTDIALOG_HPP

#include <QDialog>
#include <QItemSelection>
#include <QList>
#include <QStandardItemModel>

#include "GlobalDefinition.hpp"
#include "scriptconfig.hpp"

namespace Ui {
class CommandScriptDialog;
}

class CommandScriptDialog : public QDialog {
  Q_OBJECT

public:
  explicit CommandScriptDialog(
      QWidget *parent = nullptr,
      const QList<DatabaseCommand> commands = QList<DatabaseCommand>());
  ~CommandScriptDialog();

  void onCommandSelectionChanged(const QItemSelection &selected,
                                 const QItemSelection &deselected);

  void onConfirmButtonClicked();

  void onCancelButtonClicked();

  QList<ScriptConfig> getScriptConfigs() const;

private:
  Ui::CommandScriptDialog *ui;
  QStandardItemModel *commandItemModel;
};

#endif // COMMANDSCRIPTDIALOG_HPP
