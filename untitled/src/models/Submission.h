#ifndef SUBMISSION_H
#define SUBMISSION_H

// ============================================================================
//  Submission.h —— 一次提交记录（对应 /api/user.status 的一条 result）
//
//  这是整个项目最核心、数据量最大的表（几千到几万行），
//  所有错题统计、知识点掌握度、推荐都建立在它之上。
// ============================================================================

#include "models/Enums.h"
#include "models/Problem.h"

#include <QMetaType>
#include <QString>
#include <QtGlobal>

namespace acm {

struct Submission
{
    /// API 的全局唯一提交号，直接用作主键
    qint64 id = 0;

    QString handle;                 // 谁的提交（为将来支持多账号预留）

    qint64 contestId = 0;
    QString problemIndex;           // 与 Problem::index 对应
    QString problemName;            // 冗余存一份，避免列表查询时 join

    Verdict verdict = Verdict::Unknown;

    /// ⚠️ 必须是 "TESTS"。
    /// 初次同步时务必核对这个字段的实际取值：
    ///   非 "TESTS" 的记录（PRE_TESTS / CHALLENGES 等）必须被过滤掉，
    ///   否则"是否通过"会判错。
    QString testset;

    int passedTestCount = 0;
    qint64 timeConsumedMillis = 0;
    qint64 memoryConsumedBytes = 0;

    QString programmingLanguage;

    /// Unix 时间戳（秒）。增量同步就靠它做水位线
    qint64 creationTimeSeconds = 0;

    ParticipantType participantType = ParticipantType::Unknown;

    /// 时间顺序的比较：用于"同一题取最后一次提交"
    bool isNewerThan(const Submission &other) const
    {
        return creationTimeSeconds > other.creationTimeSeconds;
    }

    /// 提交时题目的难度分（同步时从 problems 表补全；可能为 0）
    int problemRating = 0;
    bool hasProblemRating = false;

    bool isAccepted() const { return acm::isAccepted(verdict); }

    /// 题目身份，用于按题分组
    QString problemKey() const
    {
        return QString::number(contestId) + QLatin1Char('-') + problemIndex;
    }
};

} // namespace acm

Q_DECLARE_METATYPE(acm::Submission)

#endif // SUBMISSION_H
