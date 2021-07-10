#pragma once

#include "Statement.h"

struct UnaryOpStmt : Stmt<UnaryOpStmt> {
    std::string op;
    Statement *src;

    UnaryOpStmt
        ( int id_
        , std::string op_
        , Statement *src_
        )
        : Stmt(id_)
        , op(op_)
        , src(src_)
    {}

    virtual StmtFields fields() override {
        return {
            src,
            };
    }

    virtual std::string print() const override {
        return format(
            "$%d = UnaryOp [%s] $%d"
            , id
            , op.c_str()
            , src->id
            );
    }
};

struct BinaryOpStmt : Stmt<BinaryOpStmt> {
    std::string op;
    Statement *lhs;
    Statement *rhs;

    BinaryOpStmt
        ( int id_
        , std::string op_
        , Statement *lhs_
        , Statement *rhs_
        )
        : Stmt(id_)
        , op(op_)
        , lhs(lhs_)
        , rhs(rhs_)
    {}

    virtual StmtFields fields() override {
        return {
            lhs,
            rhs,
            };
    }

    virtual std::string print() const override {
        return format(
            "$%d = BinaryOp [%s] $%d $%d"
            , id
            , op.c_str()
            , lhs->id
            , rhs->id
            );
    }
};

struct AssignStmt : Stmt<AssignStmt> {
    Statement *dst;
    Statement *src;

    AssignStmt
        ( int id_
        , Statement *dst_
        , Statement *src_
        )
        : Stmt(id_)
        , dst(dst_)
        , src(src_)
    {}

    virtual StmtFields fields() override {
        return {
            dst,
            src,
            };
    }

    virtual std::string print() const override {
        return format(
            "$%d = Assign $%d $%d"
            , id
            , dst->id
            , src->id
            );
    }
};

struct SymbolStmt : Stmt<SymbolStmt> {
    std::string name;

    SymbolStmt
        ( int id_
        , std::string name_
        )
        : Stmt(id_)
        , name(name_)
    {}

    virtual StmtFields fields() override {
        return {
            };
    }

    virtual std::string print() const override {
        return format(
            "$%d = Symbol [%s]"
            , id
            , name.c_str()
            );
    }
};

struct LiterialStmt : Stmt<LiterialStmt> {
    std::string name;

    LiterialStmt
        ( int id_
        , std::string name_
        )
        : Stmt(id_)
        , name(name_)
    {}

    virtual StmtFields fields() override {
        return {
            };
    }

    virtual std::string print() const override {
        return format(
            "$%d = Literial [%s]"
            , id
            , name.c_str()
            );
    }
};

struct AsmAssignStmt : AsmStmt<AsmAssignStmt> {
    int dst;
    int src;

    AsmAssignStmt
        ( int id_
        , int dst_
        , int src_
        )
        : AsmStmt(id_)
        , dst(dst_)
        , src(src_)
    {}

    virtual std::string print() const override {
        return format(
            "$%d = AsmAssign r%d r%d"
            , id
            , dst
            , src
            );
    }
};

struct AsmLoadConstStmt : AsmStmt<AsmLoadConstStmt> {
    int dst;
    std::string name;

    AsmLoadConstStmt
        ( int id_
        , int dst_
        , std::string name_
        )
        : AsmStmt(id_)
        , dst(dst_)
        , name(name_)
    {}

    virtual std::string print() const override {
        return format(
            "$%d = AsmLoadConst r%d [%s]"
            , id
            , dst
            , name.c_str()
            );
    }
};


struct AsmLoadSymbolStmt : AsmStmt<AsmLoadSymbolStmt> {
    int dst;
    std::string name;

    AsmLoadSymbolStmt
        ( int id_
        , int dst_
        , std::string name_
        )
        : AsmStmt(id_)
        , dst(dst_)
        , name(name_)
    {}

    virtual std::string print() const override {
        return format(
            "$%d = AsmLoadSymbol r%d [%s]"
            , id
            , dst
            , name.c_str()
            );
    }
};

struct AsmBinaryOpStmt : AsmStmt<AsmBinaryOpStmt> {
    std::string op;
    int dst;
    int lhs;
    int rhs;

    AsmBinaryOpStmt
        ( int id_
        , std::string op_
        , int dst_
        , int lhs_
        , int rhs_
        )
        : AsmStmt(id_)
        , op(op_)
        , dst(dst_)
        , lhs(lhs_)
        , rhs(rhs_)
    {}

    virtual std::string print() const override {
        return format(
            "$%d = AsmBinaryOp (%s) r%d r%d r%d"
            , id
            , op.c_str()
            , dst
            , lhs
            , rhs
            );
    }
};

struct AsmUnaryOpStmt : AsmStmt<AsmUnaryOpStmt> {
    std::string op;
    int dst;
    int src;

    AsmUnaryOpStmt
        ( int id_
        , std::string op_
        , int dst_
        , int src_
        )
        : AsmStmt(id_)
        , op(op_)
        , dst(dst_)
        , src(src_)
    {}

    virtual std::string print() const override {
        return format(
            "$%d = AsmUnaryOp (%s) r%d r%d"
            , id
            , op.c_str()
            , dst
            , src
            );
    }
};

struct AsmMemoryStoreStmt : AsmStmt<AsmMemoryStoreStmt> {
    int mem;
    int val;

    AsmMemoryStoreStmt
        ( int id_
        , int mem_
        , int val_
        )
        : AsmStmt(id_)
        , mem(mem_)
        , val(val_)
    {}

    virtual std::string print() const override {
        return format(
            "$%d = AsmMemoryStore r%d [%d]"
            , id
            , val
            , mem
            );
    }
};

struct AsmMemoryLoadStmt : AsmStmt<AsmMemoryLoadStmt> {
    int mem;
    int val;

    AsmMemoryLoadStmt
        ( int id_
        , int mem_
        , int val_
        )
        : AsmStmt(id_)
        , mem(mem_)
        , val(val_)
    {}

    virtual std::string print() const override {
        return format(
            "$%d = AsmMemoryLoad r%d [%d]"
            , id
            , val
            , mem
            );
    }
};