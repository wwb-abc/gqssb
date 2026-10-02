// ============================================================================
//  tst_models.cpp —— models 层单元测试
//
//  为什么 M1 就要写测试：
//   verdict 字符串映射错了不会报错、不会崩溃，只会让错题统计静默算错。
//   这类 bug 靠肉眼看界面永远发现不了，只能靠测试锁死。
//
//  运行方式：
//   1) Qt Creator 打开 tests/tests.pro，点"运行"；
//   2) 或命令行：tests\build\out\tst_models.exe
// ============================================================================

#include "models/Enums.h"
#include "models/ImportTask.h"
#include "models/Problem.h"
#include "models/Submission.h"
#include "models/WrongNote.h"

#include <QtTest>

using namespace acm;

class TestModels : public QObject
{
    Q_OBJECT

private slots:
    // ---------------- Enums：Verdict ----------------

    void verdict_stringIsExactlyCodeforcesValue()
    {
        // 这些字符串大小写必须和 CF 完全一致，逐字锁定
        QCOMPARE(verdictToString(Verdict::Ok),                    QStringLiteral("OK"));
        QCOMPARE(verdictToString(Verdict::WrongAnswer),           QStringLiteral("WRONG_ANSWER"));
        QCOMPARE(verdictToString(Verdict::TimeLimitExceeded),     QStringLiteral("TIME_LIMIT_EXCEEDED"));
        QCOMPARE(verdictToString(Verdict::MemoryLimitExceeded),   QStringLiteral("MEMORY_LIMIT_EXCEEDED"));
        QCOMPARE(verdictToString(Verdict::RuntimeError),          QStringLiteral("RUNTIME_ERROR"));
        QCOMPARE(verdictToString(Verdict::CompilationError),      QStringLiteral("COMPILATION_ERROR"));
        QCOMPARE(verdictToString(Verdict::IdlenessLimitExceeded), QStringLiteral("IDLENESS_LIMIT_EXCEEDED"));
        QCOMPARE(verdictToString(Verdict::Challenged),            QStringLiteral("CHALLENGED"));
        QCOMPARE(verdictToString(Verdict::Skipped),               QStringLiteral("SKIPPED"));
        QCOMPARE(verdictToString(Verdict::Testing),               QStringLiteral("TESTING"));
        QCOMPARE(verdictToString(Verdict::Rejected),              QStringLiteral("REJECTED"));
        QCOMPARE(verdictToString(Verdict::Failed),                QStringLiteral("FAILED"));
        QCOMPARE(verdictToString(Verdict::Partial),               QStringLiteral("PARTIAL"));
    }

    void verdict_roundTripThroughString()
    {
        // 反向解析必须回到原值——这是整条数据链路的正确性基础
        for (Verdict v : allKnownVerdicts()) {
            QCOMPARE(verdictFromString(verdictToString(v)), v);
        }
    }

    void verdict_noTwoValuesShareTheSameString()
    {
        // 防御"复制粘贴漏改"导致的重复映射
        QSet<QString> seen;
        for (Verdict v : allKnownVerdicts()) {
            const QString s = verdictToString(v);
            QVERIFY2(!seen.contains(s), qPrintable(QStringLiteral("重复的 verdict 字符串: ") + s));
            seen.insert(s);
        }
        QCOMPARE(seen.size(), allKnownVerdicts().size());
    }

    void verdict_fromStringIsLenientAboutWhitespaceAndCase()
    {
        QCOMPARE(verdictFromString(QStringLiteral("  wrong_answer ")), Verdict::WrongAnswer);
        QCOMPARE(verdictFromString(QStringLiteral("Ok")),              Verdict::Ok);
    }

    void verdict_fromString_unknownInputReturnsUnknownInsteadOfGarbage()
    {
        // 脏数据是常态：新 verdict、字段缺失、拼写变动，都不能崩、不能误判
        QCOMPARE(verdictFromString(QStringLiteral("SOMETHING_NEW")), Verdict::Unknown);
        QCOMPARE(verdictFromString(QString()),                       Verdict::Unknown);
        QCOMPARE(verdictFromString(QStringLiteral("   ")),           Verdict::Unknown);
    }

    void verdict_classificationIsCorrect()
    {
        QVERIFY(isAccepted(Verdict::Ok));
        QVERIFY(!isAccepted(Verdict::WrongAnswer));
        QVERIFY(!isAccepted(Verdict::Partial));   // 部分分不等于通过
        QVERIFY(!isAccepted(Verdict::Unknown));

        QVERIFY(isLogicError(Verdict::WrongAnswer));
        QVERIFY(isLogicError(Verdict::TimeLimitExceeded));
        QVERIFY(isLogicError(Verdict::RuntimeError));
        QVERIFY(!isLogicError(Verdict::Ok));
        QVERIFY(!isLogicError(Verdict::CompilationError));
        QVERIFY(!isLogicError(Verdict::Skipped));

        QVERIFY(isEnvironmentError(Verdict::CompilationError));
        QVERIFY(isEnvironmentError(Verdict::Skipped));

        QVERIFY(isPending(Verdict::Testing));
        QVERIFY(isPending(Verdict::Unknown));
        QVERIFY(!isPending(Verdict::Ok));
        QVERIFY(!isPending(Verdict::WrongAnswer));

        // 三个分类互斥（Testing/Unknown 之外，每个 verdict 只能落在一类里）
        QVERIFY(!(isLogicError(Verdict::CompilationError)
                  && isEnvironmentError(Verdict::CompilationError)));
    }

    // ---------------- Enums：其余枚举 ----------------

    void problemType_roundTrip()
    {
        QCOMPARE(problemTypeToString(ProblemType::Programming), QStringLiteral("PROGRAMMING"));
        QCOMPARE(problemTypeToString(ProblemType::Question),    QStringLiteral("QUESTION"));
        QCOMPARE(problemTypeFromString(QStringLiteral("PROGRAMMING")), ProblemType::Programming);
        QCOMPARE(problemTypeFromString(QStringLiteral("nonsense")),    ProblemType::Unknown);
    }

    void participantType_roundTrip()
    {
        QCOMPARE(participantTypeToString(ParticipantType::Contestant),
                 QStringLiteral("CONTESTANT"));
        QCOMPARE(participantTypeToString(ParticipantType::OutOfCompetition),
                 QStringLiteral("OUT_OF_COMPETITION"));
        QCOMPARE(participantTypeFromString(QStringLiteral("VIRTUAL")), ParticipantType::Virtual);
        QCOMPARE(participantTypeFromString(QStringLiteral("nonsense")), ParticipantType::Unknown);
    }

    // ---------------- Problem ----------------

    void problem_ratingAbsenceIsDistinguishableFromZero()
    {
        Problem p;
        p.contestId = 1920;
        p.index = QStringLiteral("C");
        p.name = QStringLiteral("The Game");

        // 默认：没有 rating
        QVERIFY(!p.hasRating);
        QCOMPARE(p.rating, 0);
        QCOMPARE(p.effectiveRating(), 0);

        // 明确赋值 0 分是另一回事（虽然现实中不存在，但语义必须分得开）
        p.rating = 0;
        p.hasRating = true;
        QVERIFY(p.hasRating);
    }

    void problem_keyAndDisplayName()
    {
        Problem p;
        p.contestId = 1920;
        p.index = QStringLiteral("C");
        p.name = QStringLiteral("The Game");

        QCOMPARE(p.key(),         QStringLiteral("1920-C"));
        QCOMPARE(p.displayName(), QStringLiteral("1920C - The Game"));
        QVERIFY(p.isValid());

        Problem empty;
        QVERIFY(!empty.isValid());
    }

    void problem_contestIdSurvivesLargeValues()
    {
        // gym 题目编号可能超出 32 位 int
        Problem p;
        p.contestId = 3000000000LL;
        p.index = QStringLiteral("A");
        QCOMPARE(p.key(), QStringLiteral("3000000000-A"));
    }

    // ---------------- Submission ----------------

    void submission_isNewerThanUsesTimestamp()
    {
        Submission older;
        older.creationTimeSeconds = 1000;
        Submission newer;
        newer.creationTimeSeconds = 2000;

        QVERIFY(newer.isNewerThan(older));
        QVERIFY(!older.isNewerThan(newer));
        QVERIFY(!newer.isNewerThan(newer));
    }

    void submission_isAcceptedDelegatesToEnum()
    {
        Submission s;
        s.verdict = Verdict::Ok;
        QVERIFY(s.isAccepted());

        s.verdict = Verdict::WrongAnswer;
        QVERIFY(!s.isAccepted());
    }

    void submission_problemKeyMatchesProblemKeyFormat()
    {
        Submission s;
        s.contestId = 1920;
        s.problemIndex = QStringLiteral("C");
        QCOMPARE(s.problemKey(), QStringLiteral("1920-C"));
    }

    // ---------------- ImportTask ----------------

    void importTask_percentHandlesUnknownTotal()
    {
        ImportTask t;
        QCOMPARE(t.percent(), -1);          // 总数未知 -> 进度条走"不确定"模式

        t.totalCount = 200;
        t.processedCount = 50;
        QCOMPARE(t.percent(), 25);

        t.processedCount = 999;             // 超出也不许 >100
        QCOMPARE(t.percent(), 100);

        t.processedCount = -5;              // 负数也不许 <0
        QCOMPARE(t.percent(), 0);
    }

    void importTask_stateTransitions()
    {
        ImportTask t;
        QVERIFY(!t.isRunning());
        QVERIFY(!t.isFinished());

        t.state = TestTaskState::Running;
        QVERIFY(t.isRunning());
        QVERIFY(!t.isFinished());

        t.state = TestTaskState::Failed;
        QVERIFY(!t.isRunning());
        QVERIFY(t.isFinished());
        QCOMPARE(t.stateText(), QStringLiteral("失败"));
    }

    // ---------------- WrongNote ----------------

    void wrongNote_dueLogic()
    {
        WrongNote n;
        n.contestId = 1920;
        n.problemIndex = QStringLiteral("C");
        QVERIFY(n.isValid());

        // 从未排程
        QVERIFY(n.needsScheduling());
        QVERIFY(!n.isDue());

        // 已过期
        n.nextReviewAt = QDateTime::currentDateTime().addDays(-1);
        QVERIFY(!n.needsScheduling());
        QVERIFY(n.isDue());

        // 还没到
        n.nextReviewAt = QDateTime::currentDateTime().addDays(1);
        QVERIFY(!n.isDue());
    }
};

QTEST_MAIN(TestModels)
#include "tst_models.moc"
