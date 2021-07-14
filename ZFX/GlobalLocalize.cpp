#include "IRVisitor.h"
#include "Stmts.h"
#include <functional>
#include <map>

namespace zfx {

struct ReassignGlobals : Visitor<ReassignGlobals> {
    using visit_stmt_types = std::tuple
        < AsmGlobalLoadStmt
        , AsmGlobalStoreStmt
        >;

    std::map<int, int> globals;
    std::map<int, int> locals;

    void local(int &mem) {
        if (auto it = locals.find(mem); it != locals.end()) {
            mem = it->second;
            return;
        }
        int newmem = locals.size();
        locals[mem] = newmem;
        mem = newmem;
    }

    void global(int &mem) {
        if (auto it = globals.find(mem); it != globals.end()) {
            mem = it->second;
            return;
        }
        int newmem = globals.size();
        globals[mem] = newmem;
        mem = newmem;
    }

    void visit(AsmLocalStoreStmt *stmt) {
        local(stmt->mem);
    }

    void visit(AsmLocalLoadStmt *stmt) {
        local(stmt->mem);
    }

    void visit(AsmGlobalStoreStmt *stmt) {
        global(stmt->mem);
    }

    void visit(AsmGlobalLoadStmt *stmt) {
        global(stmt->mem);
    }
};

struct GlobalLocalize : Visitor<GlobalLocalize> {
    using visit_stmt_types = std::tuple
        < AsmLocalLoadStmt
        , AsmLocalStoreStmt
        , AsmGlobalLoadStmt
        , AsmGlobalStoreStmt
        , Statement
        >;

    std::unique_ptr<IR> ir = std::make_unique<IR>();

    int nglobals = 0;

    void visit(Statement *stmt) {
        ir->push_clone_back(stmt);
    }

    void visit(AsmLocalLoadStmt *stmt) {
        ir->emplace_back<AsmLocalLoadStmt>
            ( stmt->mem + nglobals
            , stmt->val
            );
    }

    void visit(AsmLocalStoreStmt *stmt) {
        ir->emplace_back<AsmLocalStoreStmt>
            ( stmt->mem + nglobals
            , stmt->val
            );
    }

    void visit(AsmGlobalLoadStmt *stmt) {
        ir->emplace_back<AsmLocalLoadStmt>
            ( stmt->mem
            , stmt->val
            );
    }

    void visit(AsmGlobalStoreStmt *stmt) {
        ir->emplace_back<AsmLocalStoreStmt>
            ( stmt->mem
            , stmt->val
            );
    }
};

std::map<int, int> apply_global_localize(IR *ir) {
    ReassignGlobals reassign;
    reassign.apply(ir);
    GlobalLocalize visitor;
    visitor.nglobals = reassign.globals.size();
    visitor.apply(ir);
    *ir = *visitor.ir;
    return reassign.globals;
}

}