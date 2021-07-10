#pragma once

#include "Statement.h"
#include <map>

struct IR {
    std::vector<std::unique_ptr<Statement>> stmts;
    std::map<Statement *, Statement *> cloned;

    IR() = default;
    IR(IR const &ir) { *this = ir; }

    void clear() {
        stmts.clear();
    }

    size_t size() const {
        return stmts.size();
    }

    IR &operator=(IR const &ir) {
        clear();
        for (auto const &s: ir.stmts) {
            auto stmt = s.get();
            auto new_stmt = push_clone_back(stmt);
        }
        return *this;
    }

    template <class T, class ...Ts>
    T *emplace_back(Ts &&...ts) {
        auto id = stmts.size();
        auto stmt = std::make_unique<T>(id, std::forward<Ts>(ts)...);
        auto raw_ptr = stmt.get();
        stmts.push_back(std::move(stmt));
        return raw_ptr;
    }

    struct Hole {
        int id;
        IR *ir;

        Hole
            ( int id_
            , IR *ir_
            )
            : id(id_)
            , ir(ir_)
            {}

        template <class T, class ...Ts>
        T *place(Ts &&...ts) const {
            auto stmt = std::make_unique<T>(id, std::forward<Ts>(ts)...);
            auto raw_ptr = stmt.get();
            ir->stmts[id] = std::move(stmt);
            return raw_ptr;
        }
    };

    Hole make_hole_back() {
        int id = stmts.size();
        stmts.push_back(nullptr);
        return Hole(id, this);
    }

    Statement *push_clone_back(Statement const *stmt_) {
        auto stmt = const_cast<Statement *>(stmt_);
        if (auto it = cloned.find(stmt); it != cloned.end()) {
            return it->second;
        }
        // think: really necessary checking all dep stmts when clone?
        for (Statement *&field: stmt->fields()) {
            field = push_clone_back(field);
        }
        auto id = stmts.size();
        auto new_stmt = stmt->clone(id);
        auto raw_ptr = new_stmt.get();
        stmts.push_back(std::move(new_stmt));
        cloned[stmt] = raw_ptr;
        return raw_ptr;
    }

    void print() const {
        for (auto const &s: stmts) {
            if (s) {
                cout << s->print() << endl;
            }
        }
    }
};