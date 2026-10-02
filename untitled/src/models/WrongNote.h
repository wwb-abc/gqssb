#ifndef WRONGNOTE_H
#define WRONGNOTE_H

// ============================================================================
//  WrongNote.h —— 错题本里的一条"人写的"记录
//
//  与 Submission 的区别：
//    Submission = 机器抓到的客观事实（不可变）
//    WrongNote  = 我自己写的复盘（可变、可删、可反复修改）
//
//  ⚠️ 错题"是否未通过"由 Submission 统计出来，不要存进 WrongNote 里当真相。
//     WrongNote 只保存主观判断（我错在哪、我学到什么、复习到哪一步）。
// ============================================================================

#include "models/Enums.h"

#include <QDateTime>
#include <QMetaType>
#include <QString>
#include <QtGlobal>

namespace acm {

struct WrongNote
{
    qint64 id = 0;                  // 数据库自增主键；0 表示尚未入库

    qint64 contestId = 0;
    QString problemIndex;
    QString problemName;

    // ---- 主观内容 ----
    QString content;                // 复盘正文（我为什么错、正确思路是什么）
    ErrorType errorType = ErrorType::Unknown;
    QStringList keyPoints;          // 这题教给我的关键点

    // ---- 艾宾浩斯复习排程 ----
    int reviewCount = 0;            // 已复习次数
    QDateTime lastReviewAt;         // 上次复习时间（无效 = 从未复习）
    QDateTime nextReviewAt;         // 下次该复习的时间（无效 = 需要初始化）

    /// 自评掌握程度 0~5。和算法算出来的 mastery 分开存，不互相覆盖
    int masteryLevel = 0;

    QDateTime createdAt;
    QDateTime updatedAt;

    static constexpr int MaxMasteryLevel = 5;

    QString problemKey() const
    {
        return QString::number(contestId) + QLatin1Char('-') + problemIndex;
    }

    /// 是否到期待复习
    bool isDue(const QDateTime &now = QDateTime::currentDateTime()) const
    {
        return nextReviewAt.isValid() && nextReviewAt <= now;
    }

    /// 是否还没排过期
    bool needsScheduling() const { return !nextReviewAt.isValid(); }

    bool isValid() const { return contestId != 0 && !problemIndex.isEmpty(); }
};

} // namespace acm

Q_DECLARE_METATYPE(acm::WrongNote)

#endif // WRONGNOTE_H
