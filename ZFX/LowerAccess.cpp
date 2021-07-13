#include "IRVisitor.h"
#include "Stmts.h"

namespace zfx {

struct LowerAccess : Visitor<LowerAccess> {
    using visit_stmt_types = std::tuple
        < AssignStmt
        , UnaryOpStmt
        , BinaryOpStmt
        , LiterialStmt
        , SymbolStmt
        , TempSymbolStmt
        , Statement
        >;

    std::unique_ptr<IR> ir = std::make_unique<IR>();

    void visit(Statement *stmt) {
        error("unexpected statement type `%s`", typeid(*stmt).name());
    }

    struct RegInfo {
        int last_used = -1;
        int curr_stmtid = -1;
    };

    std::map<int, int> usages;
    int reg_top_id = 0;

    int store(int stmtid) {
        if (auto it = usages.find(stmtid); it != usages.end()) {
            return it->second;
        }
        int regid = reg_top_id++;
        usages[stmtid] = regid;
        return regid;
    }

    int load(int stmtid) {
        if (auto it = usages.find(stmtid); it != usages.end()) {
            return it->second;
        }
        error("statement $%d used before assignment", stmtid);
    }

    void visit(TempSymbolStmt *stmt) {
        if (stmt->symids.size() != 1) {
            error("scalar expected on temp load, got %d-D vector",
                stmt->symids.size());
        }
        // let the RegisterAllocation pass to spill it to local memory:
        store(stmt->id);
    }

    void visit(SymbolStmt *stmt) {
        if (stmt->symids.size() != 1) {
            error("scalar expected on load, got %d-D vector",
                stmt->symids.size());
        }
        ir->emplace_back<AsmGlobalLoadStmt>
            ( stmt->symids[0]
            , store(stmt->id)
            );
    }

    void visit(LiterialStmt *stmt) {
        ir->emplace_back<AsmLoadConstStmt>
            ( store(stmt->id)
            , stmt->value
            );
    }

    void visit(UnaryOpStmt *stmt) {
        ir->emplace_back<AsmUnaryOpStmt>
            ( stmt->op
            , store(stmt->id)
            , load(stmt->src->id)
            );
    }

    void visit(BinaryOpStmt *stmt) {
        ir->emplace_back<AsmBinaryOpStmt>
            ( stmt->op
            , store(stmt->id)
            , load(stmt->lhs->id)
            , load(stmt->rhs->id)
            );
    }

    void visit(AssignStmt *stmt) {
        if (auto dst = dynamic_cast<SymbolStmt *>(stmt->dst); dst) {
            if (dst->symids.size() != 1) {
                error("scalar expected on store, got %d-D vector",
                    dst->symids.size());
            }
            ir->emplace_back<AsmGlobalStoreStmt>
                ( dst->symids[0]
                , load(stmt->src->id)
                );
            return;
        }
        ir->emplace_back<AsmAssignStmt>
            ( store(stmt->dst->id)
            , load(stmt->src->id)
            );
    }
};

std::unique_ptr<IR> apply_lower_access(IR *ir) {
    LowerAccess visitor;
    visitor.apply(ir);
    return std::move(visitor.ir);
}

}