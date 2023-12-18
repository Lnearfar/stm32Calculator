#pragma once
#ifndef TINYEXPR_H
#define TINYEXPR_H


#ifdef __cplusplus
extern "C" {
#endif



    typedef struct te_expr {
        int type;
        union { double value; const double* bound; const void* function; };
        void* parameters[1];
    } te_expr;


    enum {
        TE_VARIABLE = 0,

        TE_FUNCTION0 = 8, TE_FUNCTION1, TE_FUNCTION2, TE_FUNCTION3,
        TE_FUNCTION4, TE_FUNCTION5, TE_FUNCTION6, TE_FUNCTION7,

        TE_CLOSURE0 = 16, TE_CLOSURE1, TE_CLOSURE2, TE_CLOSURE3,
        TE_CLOSURE4, TE_CLOSURE5, TE_CLOSURE6, TE_CLOSURE7,

        TE_FLAG_PURE = 32
    };

    typedef struct te_variable {
        const char* name;
        const void* address;
        int type;
        void* context;
    } te_variable;



    /* Parses the input expression, evaluates it, and frees it. */
    /* Returns NaN on error. */
    double te_interp(const char* expression, int* error);

    /* Parses the input expression and binds variables. */
    /* Returns NULL on error. */
    te_expr* te_compile(const char* expression, const te_variable* variables, int var_count, int* error);

    /* Evaluates the expression. */
    double te_eval(const te_expr* n);

    /* Prints debugging information on the syntax tree. */
    void te_print(const te_expr* n);

    /* Frees the expression. */
    /* This is safe to call on NULL pointers. */
    void te_free(te_expr* n);

    void replace_str(char* str, const char* old_str, const char* new_str);
#ifdef __cplusplus
}
#endif

#endif /*TINYEXPR_H*/#pragma once
