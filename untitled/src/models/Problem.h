#ifndef PROBLEM_H
#define PROBLEM_H

// ============================================================================
//  Problem.h —— 题目（对应 Codeforces 的 Problem 对象 + 我们自建的题库记录）
//
//  ⚠️ 最重要的一个设计决定：rating 必须允许"不存在"。
//     CF 上大量题目（尤其老题、非 rated 比赛题）没有 rating 字段。
//     如果用 int rating 并用 0 表示缺失，你会分不清 "rating=0" 和 "没有 rating"，
//     推荐算法会把它们全部当成最简单的题。
// ============================================================================

#include "models/Enums.h"

#include <QList>
#include <QMetaType>
#include <QString>
#include <QStringList>
#include <QtGlobal>

namespace acm {

struct Problem
{
    /// 题目身份 = (contestId, index)，例如 (1920, "C")
    /// 注意 contestId 用 qint64：gym / 特殊比赛的编号可能超出 int 范围
    qint64 contestId = 0;
    QString index;                  // "A" / "B1" / "C2"，不是整数！
    QString name;
    ProblemType type = ProblemType::Unknown;

    /// 官方难度分。rating == 0 表示"API 未提供"，不是"难度为 0"
    int rating = 0;
    bool hasRating = false;

    /// Codeforces 官方打的标签，是知识点分析的数据源（"dp" / "dsu" / ...）
    QStringList tags;

    /// 所属比赛名（由 Contest 表补全，便于列表显示）
    QString contestName;

    /// 组合主键的字符串形式，用于日志、SQL 拼接、去重
    QString key() const
    {
        return QString::number(contestId) + QLatin1Char('-') + index;
    }

    /// 界面上通常显示成 "1920C - The Game"
    QString displayName() const
    {
        return QString::number(contestId) + index + QStringLiteral(" - ") + name;
    }

    bool isValid() const
    {
        return contestId != 0 && !index.isEmpty();
    }

    /// rating 缺失时用于排序的兜底值
    int effectiveRating() const
    {
        return hasRating ? rating : 0;
    }
};

} // namespace acm

Q_DECLARE_METATYPE(acm::Problem)

#endif // PROBLEM_H
