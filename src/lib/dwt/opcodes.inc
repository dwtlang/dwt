#ifndef OP
#error "OP must be defined prior to including opcodes.inc"
#endif

// format: OP(<opcode>, <stack effect>, <operand bytes>)

OP(SKIP, 0, 0)
OP(LOOP, 0, 2)
OP(BRA, 0, 2)
OP(BRZ, -1, 2)
OP(BNZ, -1, 2)
OP(CALL, 0, 1)
OP(RET, 0, 0)
OP(SUPER, 0, 0)
OP(NIL, 1, 0)
OP(TRUE, 1, 0)
OP(FALSE, 1, 0)
OP(ZERO, 1, 0)
OP(ONE, 1, 0)
OP(TWO, 1, 0)
OP(POP, -1, 0)

// it is not possible to define the effect these have on the stack but that
// information is only relevant to the compiler and these ops are patched
// in by the optimiser after compilation so this does not present an issue.
OP(POPN, 0, 1)
OP(TAILCALL, 0, 1)

OP(GET, 1, 2)
OP(SET, 0, 2)
OP(CLOSE, -1, 0)
OP(UPVGET, 1, 2)
OP(UPVSET, 0, 2)
OP(MBRGET, -1, 2)
OP(MBRSET, 0, 2)
OP(KEYGET, -1, 0)
OP(KEYSET, 0, 0)
OP(PAIR, 0, 0)
OP(CLOSURE, 1, 2)
OP(GLOBAL, 1, 2)
OP(CONST, 1, 2)
OP(STORE, 0, 2)
OP(ADD, -1, 0)
OP(SUB, -1, 0)
OP(MUL, -1, 0)
OP(DIV, -1, 0)
OP(INC, 0, 0)
OP(DEC, 0, 0)
OP(NEG, 0, 0)
OP(LT, -1, 0)
OP(LTEQ, -1, 0)
OP(GT, -1, 0)
OP(GTEQ, -1, 0)
OP(EQ, -1, 0)
OP(NEQ, -1, 0)
OP(IS, -1, 0)
OP(AND, -1, 0)
OP(OR, -1, 0)
OP(XOR, -1, 0)
OP(MAP, 0, 0)
OP(PRINT, -1, 0)
OP(PRINTLN, -1, 0)
