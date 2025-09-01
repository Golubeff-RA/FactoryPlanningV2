#include <catch2/catch_test_macros.hpp>
#include "operation.h"

TEST_CASE("Operation creation") {
    Operation op{777, false};
    CHECK(op.id() == 777);
    CHECK(op.stoppable() == false);
    CHECK(op.HasDependencies() == false);
    CHECK(op.ptr_to_work() == nullptr);
    CHECK(op.cnt_deps() == 0);
    CHECK(op.Appointed() == false);

    auto st_end = op.GetStEndTimes();
    CHECK(st_end.first == START_TIME_POINT);
    CHECK(st_end.second == START_TIME_POINT);
}

TEST_CASE("Operation dependencies") {
    // master -> slave
    Operation master(666, true);
    Operation slave(777, true);

    CHECK(master.AddDepended(slave.id()) == true);
    CHECK(master.AddDepended(slave.id()) == false);
    CHECK(slave.AddDependency(master.id()) == true);
    CHECK(master.depended().size() == 1);
    CHECK(*master.depended().begin() == 777);
    CHECK(slave.dependencies().size() == 1);
    CHECK(*slave.dependencies().begin() == 666);
}

TEST_CASE("Possible tools") {
    Operation op(888, false);
    op.AddPossibleTool(12);
    op.AddPossibleTool(12);
    CHECK(op.possible_tools().size() == 1);
}

TEST_CASE("Work and operation") {
    Work work(ch::system_clock::now(), ch::system_clock::now(), 0.5, 888);
    std::vector<Operation> operations;
    operations.reserve(100);
    Operation master_op{0, true};
    operations.push_back(master_op);
    work.AddOperation(master_op.id());
    for (size_t i = 1; i < operations.capacity(); ++i) {
        Operation operation {i, true};
        operation.SetWorkPtr(&work);
        master_op.AddDepended(operation.id());
        operation.AddDependency(master_op.id());
        operations.push_back(std::move(operation));
    }

    CHECK(work.operation_ids().size() == 100);
    CHECK(operations.back().ptr_to_work() == &work);

    CHECK(master_op.depended().size() == 99);
    CHECK(master_op.dependencies().size() == 0);
    CHECK(operations.back().cnt_deps() == 1);
    CHECK(operations.back().depended().size() == 0);

    auto now = ch::system_clock::now();
    master_op.SetTimes(now, now, now - now, operations);
    CHECK(operations.back().cnt_deps() == 0);
}