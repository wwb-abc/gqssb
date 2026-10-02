#include "ui/MainWindow.h"
#include "ui_MainWindow.h"

#include "models/Enums.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setupUiState();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUiState()
{
    setWindowTitle(QStringLiteral("ACM 错题分析与 AI 辅助系统"));

    // M0 自检：证明 models/Enums.h 已正确编译并链接进程序。
    // 这一步跑通，说明 INCLUDEPATH 与工程配置都没问题，
    // 之后出现的编译错误就一定是新代码自己的问题。
    const QString probe = QStringLiteral("VERDICT 映射自检：%1 -> %2")
                              .arg(acm::verdictToString(acm::Verdict::WrongAnswer),
                                   acm::verdictToString(acm::verdictFromString(
                                       QStringLiteral("TIME_LIMIT_EXCEEDED"))));

    ui->labelStatus->setText(probe);

    qInfo().noquote() << "[M0] Enums.h 已可用，Enums::Verdict 共"
                      << acm::allKnownVerdicts().size() << "个取值";
}
