#ifndef ENUMS_H
#define ENUMS_H

// ============================================================================
//  Enums.h —— 全项目共享的枚举与互转函数
//
//  设计要点（M1 里程碑）：
//   1. 全部使用 enum class，避免不同枚举的隐式整数互转；
//   2. 每个枚举配一对 toString / fromString，且字符串严格对齐外部数据源
//      （Verdict / ParticipantType / ProblemType 必须与 Codeforces API 完全一致）；
//   3. fromString 一律返回 Unknown，绝不抛异常、绝不返回错误值——脏数据是常态；
//   4. 提供"哪些 verdict 算错"的判定函数，这是错题统计的唯一真相来源，
//      业务层不允许自己写 verdict 字符串比较。
// ============================================================================

#include <QDebug>
#include <QList>
#include <QString>

namespace acm {

// ---------------------------------------------------------------------------
//  提交判定结果（严格对齐 Codeforces API 的 verdict 字段）
//
//  取自 https://codeforces.com/apiHelp/objects#Submission 的合法取值。
//  ⚠️ 这些字符串必须逐字一致（大小写敏感），写错会导致错题统计静默出错。
//  ⚠️ 待与真实 API 响应核对一次（见 M4 的联调步骤），特别是 D 的拼写。
// ---------------------------------------------------------------------------
enum class Verdict {
    Unknown = 0,              // 解析失败 / 字段缺失（例如 TESTING 中的提交）
    Ok,                       // AC  通过
    WrongAnswer,              // WA  答案错误
    TimeLimitExceeded,        // TLE 超时
    MemoryLimitExceeded,      // MLE 超内存
    RuntimeError,             // RE  运行时错误
    CompilationError,         // CE  编译错误
    IdlenessLimitExceeded,    // ILE 空闲超限（交互题）
    Challenged,               // 被 hack（比赛中/赛后）
    Skipped,                  // 跳过
    Testing,                  // 评测中
    Rejected,                 // 拒绝（管理员判定）
    Failed,                   // 失败（历史遗留值）
    Partial                   // 部分分
};

inline QString verdictToString(Verdict v)
{
    switch (v) {
    case Verdict::Ok:                    return QStringLiteral("OK");
    case Verdict::WrongAnswer:           return QStringLiteral("WRONG_ANSWER");
    case Verdict::TimeLimitExceeded:     return QStringLiteral("TIME_LIMIT_EXCEEDED");
    case Verdict::MemoryLimitExceeded:   return QStringLiteral("MEMORY_LIMIT_EXCEEDED");
    case Verdict::RuntimeError:          return QStringLiteral("RUNTIME_ERROR");
    case Verdict::CompilationError:      return QStringLiteral("COMPILATION_ERROR");
    case Verdict::IdlenessLimitExceeded: return QStringLiteral("IDLENESS_LIMIT_EXCEEDED");
    case Verdict::Challenged:            return QStringLiteral("CHALLENGED");
    case Verdict::Skipped:               return QStringLiteral("SKIPPED");
    case Verdict::Testing:               return QStringLiteral("TESTING");
    case Verdict::Rejected:              return QStringLiteral("REJECTED");
    case Verdict::Failed:                return QStringLiteral("FAILED");
    case Verdict::Partial:               return QStringLiteral("PARTIAL");
    case Verdict::Unknown:               break;
    }
    return QStringLiteral("UNKNOWN");
}

inline Verdict verdictFromString(const QString &s)
{
    const QString v = s.trimmed().toUpper();
    if (v.isEmpty())
        return Verdict::Unknown;

    // 暴力遍历 14 个取值，比维护第二张查表更不容易写错
    const Verdict all[] = {
        Verdict::Ok, Verdict::WrongAnswer, Verdict::TimeLimitExceeded,
        Verdict::MemoryLimitExceeded, Verdict::RuntimeError,
        Verdict::CompilationError, Verdict::IdlenessLimitExceeded,
        Verdict::Challenged, Verdict::Skipped, Verdict::Testing,
        Verdict::Rejected, Verdict::Failed, Verdict::Partial
    };
    for (Verdict v2 : all) {
        if (verdictToString(v2) == v)
            return v2;
    }
    return Verdict::Unknown;
}

/// 是否通过（唯一正确的"算通过"判定入口）
inline bool isAccepted(Verdict v)
{
    return v == Verdict::Ok;
}

/// 是否算"我的代码有问题"——知识点分析要统计的就是这一类
/// 不含 CE：编译错误是语言/环境问题，不代表算法知识点薄弱
inline bool isLogicError(Verdict v)
{
    switch (v) {
    case Verdict::WrongAnswer:
    case Verdict::TimeLimitExceeded:
    case Verdict::MemoryLimitExceeded:
    case Verdict::RuntimeError:
    case Verdict::IdlenessLimitExceeded:
        return true;
    default:
        return false;
    }
}

/// 问题出在环境/流程，而非算法本身
inline bool isEnvironmentError(Verdict v)
{
    return v == Verdict::CompilationError || v == Verdict::Skipped;
}

/// 结果还未确定（评测中/被标记），统计时应排除，否则数字会来回跳
inline bool isPending(Verdict v)
{
    return v == Verdict::Unknown || v == Verdict::Testing;
}

// ---------------------------------------------------------------------------
//  题目类型（对齐 API 的 problem.type）
// ---------------------------------------------------------------------------
enum class ProblemType {
    Unknown = 0,
    Programming,
    Question
};

inline QString problemTypeToString(ProblemType t)
{
    switch (t) {
    case ProblemType::Programming: return QStringLiteral("PROGRAMMING");
    case ProblemType::Question:    return QStringLiteral("QUESTION");
    case ProblemType::Unknown:     break;
    }
    return QStringLiteral("UNKNOWN");
}

inline ProblemType problemTypeFromString(const QString &s)
{
    const QString v = s.trimmed().toUpper();
    if (v == QLatin1String("PROGRAMMING")) return ProblemType::Programming;
    if (v == QLatin1String("QUESTION"))    return ProblemType::Question;
    return ProblemType::Unknown;
}

// ---------------------------------------------------------------------------
//  参赛者类型（对齐 API 的 author.participantType）
// ---------------------------------------------------------------------------
enum class ParticipantType {
    Unknown = 0,
    Contestant,
    Practice,
    Virtual,
    Manager,
    OutOfCompetition
};

inline QString participantTypeToString(ParticipantType t)
{
    switch (t) {
    case ParticipantType::Contestant:       return QStringLiteral("CONTESTANT");
    case ParticipantType::Practice:         return QStringLiteral("PRACTICE");
    case ParticipantType::Virtual:          return QStringLiteral("VIRTUAL");
    case ParticipantType::Manager:          return QStringLiteral("MANAGER");
    case ParticipantType::OutOfCompetition: return QStringLiteral("OUT_OF_COMPETITION");
    case ParticipantType::Unknown:          break;
    }
    return QStringLiteral("UNKNOWN");
}

inline ParticipantType participantTypeFromString(const QString &s)
{
    const QString v = s.trimmed().toUpper();
    if (v == QLatin1String("CONTESTANT"))        return ParticipantType::Contestant;
    if (v == QLatin1String("PRACTICE"))          return ParticipantType::Practice;
    if (v == QLatin1String("VIRTUAL"))           return ParticipantType::Virtual;
    if (v == QLatin1String("MANAGER"))           return ParticipantType::Manager;
    if (v == QLatin1String("OUT_OF_COMPETITION"))return ParticipantType::OutOfCompetition;
    return ParticipantType::Unknown;
}

// ---------------------------------------------------------------------------
//  错题错误类型（用于 WrongNote 的人工归类，与 Verdict 解耦：
//  Verdict 是机器判定，ErrorType 是"我认为我错在哪"）
// ---------------------------------------------------------------------------
enum class ErrorType {
    Unknown = 0,
    WrongIdea,          // 思路就是错的
    MissedCase,         // 漏了边界 / 特殊情况
    Complexity,         // 复杂度估算错误（TLE/MLE 的根因）
    Implementation,     // 思路对但写挂了
    KnowledgeGap,       // 完全不会该知识点
    Careless            // 粗心
};

inline QString errorTypeToString(ErrorType t)
{
    switch (t) {
    case ErrorType::WrongIdea:      return QStringLiteral("思路错误");
    case ErrorType::MissedCase:     return QStringLiteral("漏边界");
    case ErrorType::Complexity:     return QStringLiteral("复杂度");
    case ErrorType::Implementation: return QStringLiteral("实现失误");
    case ErrorType::KnowledgeGap:   return QStringLiteral("知识点不会");
    case ErrorType::Careless:       return QStringLiteral("粗心");
    case ErrorType::Unknown:        break;
    }
    return QStringLiteral("未归类");
}

// ---------------------------------------------------------------------------
//  导入任务状态（给 ImportTask 用）
// ---------------------------------------------------------------------------
enum class TestTaskState {
    Pending = 0,
    Running,
    Succeeded,
    Failed,
    Cancelled
};

inline QString testTaskStateToString(TestTaskState s)
{
    switch (s) {
    case TestTaskState::Pending:   return QStringLiteral("等待中");
    case TestTaskState::Running:   return QStringLiteral("进行中");
    case TestTaskState::Succeeded: return QStringLiteral("成功");
    case TestTaskState::Failed:    return QStringLiteral("失败");
    case TestTaskState::Cancelled: return QStringLiteral("已取消");
    }
    return QStringLiteral("未知");
}

// ---------------------------------------------------------------------------
//  QDebug 支持：qDebug() << verdict，方便开发和排错
// ---------------------------------------------------------------------------
inline QDebug operator<<(QDebug dbg, Verdict v)
{
    dbg.noquote() << verdictToString(v);
    return dbg;
}

inline QDebug operator<<(QDebug dbg, ErrorType t)
{
    dbg.noquote() << errorTypeToString(t);
    return dbg;
}

/// 供测试与 UI 下拉框使用：全部合法 Verdict（不含 Unknown）
inline QList<Verdict> allKnownVerdicts()
{
    return {
        Verdict::Ok, Verdict::WrongAnswer, Verdict::TimeLimitExceeded,
        Verdict::MemoryLimitExceeded, Verdict::RuntimeError,
        Verdict::CompilationError, Verdict::IdlenessLimitExceeded,
        Verdict::Challenged, Verdict::Skipped, Verdict::Testing,
        Verdict::Rejected, Verdict::Failed, Verdict::Partial
    };
}

} // namespace acm

Q_DECLARE_METATYPE(acm::Verdict)
Q_DECLARE_METATYPE(acm::ProblemType)
Q_DECLARE_METATYPE(acm::ParticipantType)
Q_DECLARE_METATYPE(acm::ErrorType)
Q_DECLARE_METATYPE(acm::TestTaskState)

#endif // ENUMS_H
