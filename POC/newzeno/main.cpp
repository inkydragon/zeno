#include "Backend.h"
#include "Frontend.h"
#include "Helpers.h"
#include <iostream>


namespace {

int myadd(int x, int y) {
    auto z = x + y;
    return z;
}
ZENO_DEFINE_NODE(myadd, "int myadd(int x, int y)");

void printint(int x) {
    std::cout << "printint: " << x << std::endl;
}
ZENO_DEFINE_NODE(printint, "void printint(int x)");

void printtype(zeno::v2::container::any x) {
    // see also https://en.cppreference.com/w/cpp/utility/any/type
    std::cout << "printtype: " << x.type().name() << std::endl;
}
ZENO_DEFINE_NODE(printtype, "void printtype(zeno::v2::container::any x)");

}


int main() {
    zeno::v2::frontend::Graph graph;
    graph.nodes.push_back({"make_value", {}, 1, 21.34f});
    graph.nodes.push_back({"myadd", {{0, 0}, {0, 0}}, 1, nullptr});
    graph.nodes.push_back({"printint", {{1, 0}}, 0, nullptr});
    graph.nodes.push_back({"printtype", {{1, 0}}, 0, nullptr});

    zeno::v2::frontend::ForwardSorter sorter(graph);
    sorter.require(2);
    sorter.require(3);
    auto ir = sorter.linearize();

    for (auto const &stmt: ir->stmts) {
        std::cout << stmt->to_string() << std::endl;
    }

    auto scope = zeno::v2::backend::Session::get().makeScope();
    for (auto const &stmt: ir->stmts) {
        stmt->apply(scope.get());
    }

    return 0;
}
