#ifndef IMPORTTASK_H
#define IMPORTTASK_H

// ============================================================================
//  ImportTask.h —— 一次同步/导入任务的进度与结果
//
//  用途：SyncService 在后台线程跑，通过信号把 ImportTask 发回主线程更新进度条。
//  ⚠️ 这个结构体会跨线程传递，所以必须可拷贝、不含任何指针/QObject。
// ============================================================================

#include "models/Enums.h"

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <QtGlobal>

namespace acm {

struct ImportTask
{
    qint64 id = 0;
    QString handle;

    TestTaskState state = TestTaskState::Pending;

    int totalCount = 0;             // 预计要拉多少条（0 = 未知，进度条转圈）
    int processedCount = 0;         // 已处理多少条
    int insertedCount = 0;          // 实际新增（增量同步时可能远小于 processed）
    int skippedCount = 0;           // 已存在而跳过

    QString errorMessage;

    QDateTime startedAt;
    QDateTime finishedAt;

    bool isRunning() const { return state == TestTaskState::Running; }
    bool isFinished() const
    {
        return state == TestTaskState::Succeeded
            || state == TestTaskState::Failed
            || state == TestTaskState::Cancelled;
    }

    /// 0~100；totalCount 未知时返回 -1，让进度条显示为"不确定"状态
    int percent() const
    {
        if (totalCount <= 0)
            return -1;
        const int p = processedCount * 100 / totalCount;
        return qBound(0, p, 100);
    }

    QString stateText() const { return testTaskStateToString(state); }
};

} // namespace acm

Q_DECLARE_METATYPE(acm::ImportTask)

#endif // IMPORTTASK_H
