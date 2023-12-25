#include "tinyexpr.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <limits.h>

#ifndef NAN
#define NAN (0.0 / 0.0)
#endif

#ifndef INFINITY
#define INFINITY (1.0 / 0.0)
#endif

typedef double (*te_fun2)(double, double);

enum
{
    TOK_NULL = TE_CLOSURE7 + 1,
    TOK_ERROR,
    TOK_END,
    TOK_SEP,
    TOK_OPEN,
    TOK_CLOSE,
    TOK_NUMBER,
    TOK_VARIABLE,
    TOK_INFIX
};

enum
{
    TE_CONSTANT = 1
};

typedef struct state
{
    const char *start;
    const char *next;
    int type;
    union
    {
        double value;
        const double *bound;
        const void *function;
    };
    void *context;

    const te_variable *lookup;
    int lookup_len;
} state;

#define TYPE_MASK(TYPE) ((TYPE) & 0x0000001F)

#define IS_PURE(TYPE) (((TYPE) & TE_FLAG_PURE) != 0)
#define IS_FUNCTION(TYPE) (((TYPE) & TE_FUNCTION0) != 0)
#define IS_CLOSURE(TYPE) (((TYPE) & TE_CLOSURE0) != 0)
#define ARITY(TYPE) (((TYPE) & (TE_FUNCTION0 | TE_CLOSURE0)) ? ((TYPE) & 0x00000007) : 0)
#define NEW_EXPR(type, ...) new_expr((type), (const te_expr *[]){__VA_ARGS__})
#define CHECK_NULL(ptr, ...) \
    if ((ptr) == NULL)       \
    {                        \
        __VA_ARGS__;         \
        return NULL;         \
    }

static te_expr *new_expr(const int type, const te_expr *parameters[])
{
    const int arity = ARITY(type);
    const int psize = sizeof(void *) * arity;
    const int size = (sizeof(te_expr) - sizeof(void *)) + psize + (IS_CLOSURE(type) ? sizeof(void *) : 0);
    te_expr *ret = malloc(size);
    CHECK_NULL(ret);

    memset(ret, 0, size);
    if (arity && parameters)
    {
        memcpy(ret->parameters, parameters, psize);
    }
    ret->type = type;
    ret->bound = 0;
    return ret;
}

void te_free_parameters(te_expr *n)
{
    if (!n)
        return;
    switch (TYPE_MASK(n->type))
    {
    case TE_FUNCTION7:
    case TE_CLOSURE7:
        te_free(n->parameters[6]); /* Falls through. */
    case TE_FUNCTION6:
    case TE_CLOSURE6:
        te_free(n->parameters[5]); /* Falls through. */
    case TE_FUNCTION5:
    case TE_CLOSURE5:
        te_free(n->parameters[4]); /* Falls through. */
    case TE_FUNCTION4:
    case TE_CLOSURE4:
        te_free(n->parameters[3]); /* Falls through. */
    case TE_FUNCTION3:
    case TE_CLOSURE3:
        te_free(n->parameters[2]); /* Falls through. */
    case TE_FUNCTION2:
    case TE_CLOSURE2:
        te_free(n->parameters[1]); /* Falls through. */
    case TE_FUNCTION1:
    case TE_CLOSURE1:
        te_free(n->parameters[0]);
    }
}

void te_free(te_expr *n)
{
    if (!n)
        return;
    te_free_parameters(n);
    free(n);
}

static double pi(void) { return 3.14159265358979323846; }
static double e(void) { return 2.71828182845904523536; }
static double fac(double a)
{ /* simplest version of fac */
    if (a < 0.0)
        return NAN;
    if (a > UINT_MAX)
        return INFINITY;
    unsigned int ua = (unsigned int)(a);
    unsigned long int result = 1, i;
    for (i = 1; i <= ua; i++)
    {
        if (i > ULONG_MAX / result)
            return INFINITY;
        result *= i;
    }
    return (double)result;
}
static double ncr(double n, double r)
{
    if (n < 0.0 || r < 0.0 || n < r)
        return NAN;
    if (n > UINT_MAX || r > UINT_MAX)
        return INFINITY;
    unsigned long int un = (unsigned int)(n), ur = (unsigned int)(r), i;
    unsigned long int result = 1;
    if (ur > un / 2)
        ur = un - ur;
    for (i = 1; i <= ur; i++)
    {
        if (result > ULONG_MAX / (un - ur + i))
            return INFINITY;
        result *= un - ur + i;
        result /= i;
    }
    return result;
}
static double npr(double n, double r) { return ncr(n, r) * fac(r); }

#ifdef _MSC_VER
#pragma function(ceil)
#pragma function(floor)
#endif

static const te_variable functions[] = {
    /* must be in alphabetical order */
    {"abs", fabs, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"acos", acos, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"asin", asin, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"atan", atan, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"atan2", atan2, TE_FUNCTION2 | TE_FLAG_PURE, 0},
    {"cbrt", cbrt, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"ceil", ceil, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"cos", cos, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"cosh", cosh, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"e", e, TE_FUNCTION0 | TE_FLAG_PURE, 0},
    {"exp", exp, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"fac", fac, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"floor", floor, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"ln", log, TE_FUNCTION1 | TE_FLAG_PURE, 0},
#ifdef TE_NAT_LOG
    {"log", log, TE_FUNCTION1 | TE_FLAG_PURE, 0},
#else
    {"log", log10, TE_FUNCTION1 | TE_FLAG_PURE, 0},
#endif
    {"log10", log10, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"ncr", ncr, TE_FUNCTION2 | TE_FLAG_PURE, 0},
    {"npr", npr, TE_FUNCTION2 | TE_FLAG_PURE, 0},
    {"pi", pi, TE_FUNCTION0 | TE_FLAG_PURE, 0},
    {"pow", pow, TE_FUNCTION2 | TE_FLAG_PURE, 0},
    {"sin", sin, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"sinh", sinh, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"sqrt", sqrt, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"tan", tan, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {"tanh", tanh, TE_FUNCTION1 | TE_FLAG_PURE, 0},
    {0, 0, 0, 0}};

static const te_variable *find_builtin(const char *name, int len)
{
    int imin = 0;
    int imax = sizeof(functions) / sizeof(te_variable) - 2;

    /*Binary search.*/
    while (imax >= imin)
    {
        const int i = (imin + ((imax - imin) / 2));
        int c = strncmp(name, functions[i].name, len);
        if (!c)
            c = '\0' - functions[i].name[len];
        if (c == 0)
        {
            return functions + i;
        }
        else if (c > 0)
        {
            imin = i + 1;
        }
        else
        {
            imax = i - 1;
        }
    }

    return 0;
}

static const te_variable *find_lookup(const state *s, const char *name, int len)
{
    int iters;
    const te_variable *var;
    if (!s->lookup)
        return 0;

    for (var = s->lookup, iters = s->lookup_len; iters; ++var, --iters)
    {
        if (strncmp(name, var->name, len) == 0 && var->name[len] == '\0')
        {
            return var;
        }
    }
    return 0;
}

static double add(double a, double b) { return a + b; }
static double sub(double a, double b) { return a - b; }
static double mul(double a, double b) { return a * b; }
static double divide(double a, double b) { return a / b; }
static double negate(double a) { return -a; }
static double comma(double a, double b)
{
    (void)a;
    return b;
}

// 自定义函数部分
// 字符串替换函数，将特殊符号替换成指定字符串
void replace_str(char *str, const char *old_str, const char *new_str)
{
    int old_len = strlen(old_str);
    int new_len = strlen(new_str);
    char *pos = str;

    while ((pos = strstr(pos, old_str)) != NULL)
    {
        // 将源字符串中子串后面的所有字符往后移动
        memmove(pos + new_len, pos + old_len, strlen(pos + old_len) + 1);
        // 将新的字符串插入到子串的位置上
        memcpy(pos, new_str, new_len);
        pos += new_len;
    }
}
void equation_x(char *str, char *ans) // 一元方程求解 str为方程式字符串 ans为计算结果字符串
{
    int pos_x1, pos_x2, len_x;
    int i_x = 0;
    int j_x = 0;
    strcpy(ans, "x=");
    // printf("The equation is %s\n", str);
    char a_x[10], b_x[10], c_x[100];
    char a1_x[] = "/(";
    char b1_x[] = "-(";
    char b2_x[] = "))";
    char xstr_x[10];
    len_x = strlen(str);
    for (i_x = 0; i_x < len_x; i_x++)
    {
        if (str[i_x] == 'x')
            pos_x1 = i_x;
        else if (str[i_x] == '=')
            pos_x2 = i_x;
    }
    for (i_x = 0; i_x < pos_x1; i_x++)
    {
        a_x[i_x] = str[i_x];
    }
    a_x[i_x] = ')';
    i_x++;
    a_x[i_x] = '\0';
    for (; i_x < pos_x2; i_x++)
    {
        b_x[j_x] = str[i_x];
        j_x++;
    }
    b_x[j_x] = '\0';
    i_x++;
    j_x = 1;
    c_x[0] = '(';
    for (; i_x < len_x; i_x++)
    {
        c_x[j_x] = str[i_x];
        j_x++;
    }
    c_x[j_x] = '\0';
    strcat(b1_x, b_x);
    strcat(b1_x, b2_x);
    strcat(a1_x, a_x);
    strcat(b1_x, a1_x);
    strcat(c_x, b1_x);
    // printf("%s\n", c);
    // printf("%s %s %s", a, b,c);
    // printf("%d %d", pos1, pos2);
    double r = te_interp(c_x, 0);
    // printf("The changed expression:\n\t%s\nevaluates to:\n\t%f\n", c, r);
    r = ((float)((int)((r + 0.005) * 100))) / 100; // 把运算结果保留小数点后两位
    sprintf(xstr_x, "%.2f", r);
    strcat(ans, xstr_x);
}

void insert(char s[], char t, int i)
{
    char str[100];
    strncpy(str, s, i);   // 将s数组的前i个字符复制到str中
    str[i] = t;           // 将t方到str后面
    str[i + 1] = '\0';    // 结束
    strcat(str, (s + i)); // 将s剩余的字符串连接到str
    strcpy(s, str);       // 将str复制到s中
}

void equation_xy(char *str, char *ans) // 二元方程求解 str为方程式字符串 ans为计算结果字符串
{
    int pos1_xy, pos2_xy, pos3_xy, pos4_xy, pos5_xy, len_xy;
    int i_xy = 0, j_xy = 0;
    int x_flag = 0, y_flag = 0, deng_flag = 0;
    strcpy(ans, "x=");
    len_xy = strlen(str);
    char a_xy[10], b_xy[10], c_xy[10], d_xy[10], m_xy[10], n_xy[10], ans_x[100], ans_y[100], xstr_xy[10], ystr_xy[10];
    for (i_xy = 0; i_xy < len_xy; i_xy++)
    {
        if (str[i_xy] == 'x')
        {
            if (x_flag == 0)
            {
                pos1_xy = i_xy;
                x_flag++;
            }
            else if (x_flag == 1)
                pos3_xy = i_xy;
        }
        else if (str[i_xy] == 'y')
        {
            if (y_flag == 0)
            {
                pos2_xy = i_xy;
                y_flag++;
            }
            else if (y_flag == 1)
                pos4_xy = i_xy;
        }
        else if (str[i_xy] == '#')
        {
            pos5_xy = i_xy;
        }
    }

    // printf("%d %d %d %d", pos1_xy, pos2_xy, pos3_xy, pos4_xy);

    a_xy[0] = '(';
    j_xy = 1;
    for (i_xy = 0; i_xy < pos1_xy; i_xy++)
    {
        a_xy[j_xy] = str[i_xy];
        j_xy++;
    }
    a_xy[j_xy] = ')';
    i_xy++;
    j_xy++;
    a_xy[j_xy] = '\0';
    b_xy[0] = '(';
    j_xy = 1;
    for (; i_xy < pos2_xy; i_xy++)
    {
        b_xy[j_xy] = str[i_xy];
        j_xy++;
    }
    b_xy[j_xy] = ')';
    i_xy += 2;
    j_xy++;
    b_xy[j_xy] = '\0';
    m_xy[0] = '(';
    j_xy = 1;
    for (; i_xy < pos5_xy; i_xy++)
    {
        m_xy[j_xy] = str[i_xy];
        j_xy++;
    }
    m_xy[j_xy] = ')';
    i_xy++;
    j_xy++;
    m_xy[j_xy] = '\0';
    c_xy[0] = '(';
    j_xy = 1;
    for (; i_xy < pos3_xy; i_xy++)
    {
        c_xy[j_xy] = str[i_xy];
        j_xy++;
    }
    c_xy[j_xy] = ')';
    i_xy++;
    j_xy++;
    c_xy[j_xy] = '\0';
    d_xy[0] = '(';
    j_xy = 1;
    for (; i_xy < pos4_xy; i_xy++)
    {
        d_xy[j_xy] = str[i_xy];
        j_xy++;
    }
    d_xy[j_xy] = ')';
    i_xy += 2;
    j_xy++;
    d_xy[j_xy] = '\0';
    n_xy[0] = '(';
    j_xy = 1;
    for (; i_xy < len_xy; i_xy++)
    {
        n_xy[j_xy] = str[i_xy];
        j_xy++;
    }
    n_xy[j_xy] = ')';
    i_xy++;
    j_xy++;
    n_xy[j_xy] = '\0';
    strcpy(ans_x, "(");
    strcpy(ans_y, "(");
    // printf("%s %s %s %s %s",a_xy, b_xy, c_xy, d_xy, m_xy, n_xy);

    char part_2[100] = "/(";
    strcat(part_2, a_xy);
    insert(part_2, '*', strlen(part_2));
    strcat(part_2, d_xy);
    insert(part_2, '-', strlen(part_2));
    strcat(part_2, b_xy);
    insert(part_2, '*', strlen(part_2));
    strcat(part_2, c_xy);
    insert(part_2, ')', strlen(part_2));

    strcat(ans_x, m_xy);
    insert(ans_x, '*', strlen(ans_x));
    strcat(ans_x, d_xy);
    insert(ans_x, '-', strlen(ans_x));
    strcat(ans_x, b_xy);
    insert(ans_x, '*', strlen(ans_x));
    strcat(ans_x, n_xy);
    insert(ans_x, ')', strlen(ans_x));
    strcat(ans_x, part_2);

    strcat(ans_y, n_xy);
    insert(ans_y, '*', strlen(ans_y));
    strcat(ans_y, a_xy);
    insert(ans_y, '-', strlen(ans_y));
    strcat(ans_y, m_xy);
    insert(ans_y, '*', strlen(ans_y));
    strcat(ans_y, c_xy);
    insert(ans_y, ')', strlen(ans_y));
    strcat(ans_y, part_2);

    double r_x = te_interp(ans_x, 0);
    // printf("The changed expression:\n\t%s\nevaluates to:\n\t%f\n", c, r);
    r_x = ((float)((int)((r_x + 0.005) * 100))) / 100; // 把运算结果保留小数点后两位
    sprintf(xstr_xy, "%.2f", r_x);

    double r_y = te_interp(ans_y, 0);
    // printf("The changed expression:\n\t%s\nevaluates to:\n\t%f\n", c, r);
    r_y = ((float)((int)((r_y + 0.005) * 100))) / 100; // 把运算结果保留小数点后两位
    sprintf(ystr_xy, "%.2f", r_y);

    strcat(ans, xstr_xy);
    insert(ans, ';', strlen(ans));
    char help_str[] = "y=";
    strcat(ans, help_str);
    strcat(ans, ystr_xy);
}
// 自定义函数部分结束

void next_token(state *s)
{
    s->type = TOK_NULL;

    do
    {

        if (!*s->next)
        {
            s->type = TOK_END;
            return;
        }

        /* Try reading a number. */
        if ((s->next[0] >= '0' && s->next[0] <= '9') || s->next[0] == '.')
        {
            s->value = strtod(s->next, (char **)&s->next);
            s->type = TOK_NUMBER;
        }
        else
        {
            /* Look for a variable or builtin function call. */
            if (isalpha(s->next[0]))
            {
                const char *start;
                start = s->next;
                while (isalpha(s->next[0]) || isdigit(s->next[0]) || (s->next[0] == '_'))
                    s->next++;

                const te_variable *var = find_lookup(s, start, s->next - start);
                if (!var)
                    var = find_builtin(start, s->next - start);

                if (!var)
                {
                    s->type = TOK_ERROR;
                }
                else
                {
                    switch (TYPE_MASK(var->type))
                    {
                    case TE_VARIABLE:
                        s->type = TOK_VARIABLE;
                        s->bound = var->address;
                        break;

                    case TE_CLOSURE0:
                    case TE_CLOSURE1:
                    case TE_CLOSURE2:
                    case TE_CLOSURE3: /* Falls through. */
                    case TE_CLOSURE4:
                    case TE_CLOSURE5:
                    case TE_CLOSURE6:
                    case TE_CLOSURE7:              /* Falls through. */
                        s->context = var->context; /* Falls through. */

                    case TE_FUNCTION0:
                    case TE_FUNCTION1:
                    case TE_FUNCTION2:
                    case TE_FUNCTION3: /* Falls through. */
                    case TE_FUNCTION4:
                    case TE_FUNCTION5:
                    case TE_FUNCTION6:
                    case TE_FUNCTION7: /* Falls through. */
                        s->type = var->type;
                        s->function = var->address;
                        break;
                    }
                }
            }
            else
            {
                /* Look for an operator or special character. */
                switch (s->next++[0])
                {
                case '+':
                    s->type = TOK_INFIX;
                    s->function = add;
                    break;
                case '-':
                    s->type = TOK_INFIX;
                    s->function = sub;
                    break;
                case '*':
                    s->type = TOK_INFIX;
                    s->function = mul;
                    break;
                case '/':
                    s->type = TOK_INFIX;
                    s->function = divide;
                    break;
                case '^':
                    s->type = TOK_INFIX;
                    s->function = pow;
                    break;
                case '%':
                    s->type = TOK_INFIX;
                    s->function = fmod;
                    break;
                case '(':
                    s->type = TOK_OPEN;
                    break;
                case ')':
                    s->type = TOK_CLOSE;
                    break;
                case ',':
                    s->type = TOK_SEP;
                    break;
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    break;
                default:
                    s->type = TOK_ERROR;
                    break;
                }
            }
        }
    } while (s->type == TOK_NULL);
}

static te_expr *list(state *s);
static te_expr *expr(state *s);
static te_expr *power(state *s);

static te_expr *base(state *s)
{
    /* <base>      =    <constant> | <variable> | <function-0> {"(" ")"} | <function-1> <power> | <function-X> "(" <expr> {"," <expr>} ")" | "(" <list> ")" */
    te_expr *ret;
    int arity;

    switch (TYPE_MASK(s->type))
    {
    case TOK_NUMBER:
        ret = new_expr(TE_CONSTANT, 0);
        CHECK_NULL(ret);

        ret->value = s->value;
        next_token(s);
        break;

    case TOK_VARIABLE:
        ret = new_expr(TE_VARIABLE, 0);
        CHECK_NULL(ret);

        ret->bound = s->bound;
        next_token(s);
        break;

    case TE_FUNCTION0:
    case TE_CLOSURE0:
        ret = new_expr(s->type, 0);
        CHECK_NULL(ret);

        ret->function = s->function;
        if (IS_CLOSURE(s->type))
            ret->parameters[0] = s->context;
        next_token(s);
        if (s->type == TOK_OPEN)
        {
            next_token(s);
            if (s->type != TOK_CLOSE)
            {
                s->type = TOK_ERROR;
            }
            else
            {
                next_token(s);
            }
        }
        break;

    case TE_FUNCTION1:
    case TE_CLOSURE1:
        ret = new_expr(s->type, 0);
        CHECK_NULL(ret);

        ret->function = s->function;
        if (IS_CLOSURE(s->type))
            ret->parameters[1] = s->context;
        next_token(s);
        ret->parameters[0] = power(s);
        CHECK_NULL(ret->parameters[0], te_free(ret));
        break;

    case TE_FUNCTION2:
    case TE_FUNCTION3:
    case TE_FUNCTION4:
    case TE_FUNCTION5:
    case TE_FUNCTION6:
    case TE_FUNCTION7:
    case TE_CLOSURE2:
    case TE_CLOSURE3:
    case TE_CLOSURE4:
    case TE_CLOSURE5:
    case TE_CLOSURE6:
    case TE_CLOSURE7:
        arity = ARITY(s->type);

        ret = new_expr(s->type, 0);
        CHECK_NULL(ret);

        ret->function = s->function;
        if (IS_CLOSURE(s->type))
            ret->parameters[arity] = s->context;
        next_token(s);

        if (s->type != TOK_OPEN)
        {
            s->type = TOK_ERROR;
        }
        else
        {
            int i;
            for (i = 0; i < arity; i++)
            {
                next_token(s);
                ret->parameters[i] = expr(s);
                CHECK_NULL(ret->parameters[i], te_free(ret));

                if (s->type != TOK_SEP)
                {
                    break;
                }
            }
            if (s->type != TOK_CLOSE || i != arity - 1)
            {
                s->type = TOK_ERROR;
            }
            else
            {
                next_token(s);
            }
        }

        break;

    case TOK_OPEN:
        next_token(s);
        ret = list(s);
        CHECK_NULL(ret);

        if (s->type != TOK_CLOSE)
        {
            s->type = TOK_ERROR;
        }
        else
        {
            next_token(s);
        }
        break;

    default:
        ret = new_expr(0, 0);
        CHECK_NULL(ret);

        s->type = TOK_ERROR;
        ret->value = NAN;
        break;
    }

    return ret;
}

static te_expr *power(state *s)
{
    /* <power>     =    {("-" | "+")} <base> */
    int sign = 1;
    while (s->type == TOK_INFIX && (s->function == add || s->function == sub))
    {
        if (s->function == sub)
            sign = -sign;
        next_token(s);
    }

    te_expr *ret;

    if (sign == 1)
    {
        ret = base(s);
    }
    else
    {
        te_expr *b = base(s);
        CHECK_NULL(b);

        ret = NEW_EXPR(TE_FUNCTION1 | TE_FLAG_PURE, b);
        CHECK_NULL(ret, te_free(b));

        ret->function = negate;
    }

    return ret;
}

#ifdef TE_POW_FROM_RIGHT
static te_expr *factor(state *s)
{
    /* <factor>    =    <power> {"^" <power>} */
    te_expr *ret = power(s);
    CHECK_NULL(ret);

    int neg = 0;

    if (ret->type == (TE_FUNCTION1 | TE_FLAG_PURE) && ret->function == negate)
    {
        te_expr *se = ret->parameters[0];
        free(ret);
        ret = se;
        neg = 1;
    }

    te_expr *insertion = 0;

    while (s->type == TOK_INFIX && (s->function == pow))
    {
        te_fun2 t = s->function;
        next_token(s);

        if (insertion)
        {
            /* Make exponentiation go right-to-left. */
            te_expr *p = power(s);
            CHECK_NULL(p, te_free(ret));

            te_expr *insert = NEW_EXPR(TE_FUNCTION2 | TE_FLAG_PURE, insertion->parameters[1], p);
            CHECK_NULL(insert, te_free(p), te_free(ret));

            insert->function = t;
            insertion->parameters[1] = insert;
            insertion = insert;
        }
        else
        {
            te_expr *p = power(s);
            CHECK_NULL(p, te_free(ret));

            te_expr *prev = ret;
            ret = NEW_EXPR(TE_FUNCTION2 | TE_FLAG_PURE, ret, p);
            CHECK_NULL(ret, te_free(p), te_free(prev));

            ret->function = t;
            insertion = ret;
        }
    }

    if (neg)
    {
        te_expr *prev = ret;
        ret = NEW_EXPR(TE_FUNCTION1 | TE_FLAG_PURE, ret);
        CHECK_NULL(ret, te_free(prev));

        ret->function = negate;
    }

    return ret;
}
#else
static te_expr *factor(state *s)
{
    /* <factor>    =    <power> {"^" <power>} */
    te_expr *ret = power(s);
    CHECK_NULL(ret);

    while (s->type == TOK_INFIX && (s->function == pow))
    {
        te_fun2 t = (te_fun2)s->function;
        next_token(s);
        te_expr *p = power(s);
        CHECK_NULL(p, te_free(ret));

        te_expr *prev = ret;
        ret = NEW_EXPR(TE_FUNCTION2 | TE_FLAG_PURE, ret, p);
        CHECK_NULL(ret, te_free(p), te_free(prev));

        ret->function = t;
    }

    return ret;
}
#endif

static te_expr *term(state *s)
{
    /* <term>      =    <factor> {("*" | "/" | "%") <factor>} */
    te_expr *ret = factor(s);
    CHECK_NULL(ret);

    while (s->type == TOK_INFIX && (s->function == mul || s->function == divide || s->function == fmod))
    {
        te_fun2 t = (te_fun2)s->function;
        next_token(s);
        te_expr *f = factor(s);
        CHECK_NULL(f, te_free(ret));

        te_expr *prev = ret;
        ret = NEW_EXPR(TE_FUNCTION2 | TE_FLAG_PURE, ret, f);
        CHECK_NULL(ret, te_free(f), te_free(prev));

        ret->function = t;
    }

    return ret;
}

static te_expr *expr(state *s)
{
    /* <expr>      =    <term> {("+" | "-") <term>} */
    te_expr *ret = term(s);
    CHECK_NULL(ret);

    while (s->type == TOK_INFIX && (s->function == add || s->function == sub))
    {
        te_fun2 t = (te_fun2)s->function;
        next_token(s);
        te_expr *te = term(s);
        CHECK_NULL(te, te_free(ret));

        te_expr *prev = ret;
        ret = NEW_EXPR(TE_FUNCTION2 | TE_FLAG_PURE, ret, te);
        CHECK_NULL(ret, te_free(te), te_free(prev));

        ret->function = t;
    }

    return ret;
}

static te_expr *list(state *s)
{
    /* <list>      =    <expr> {"," <expr>} */
    te_expr *ret = expr(s);
    CHECK_NULL(ret);

    while (s->type == TOK_SEP)
    {
        next_token(s);
        te_expr *e = expr(s);
        CHECK_NULL(e, te_free(ret));

        te_expr *prev = ret;
        ret = NEW_EXPR(TE_FUNCTION2 | TE_FLAG_PURE, ret, e);
        CHECK_NULL(ret, te_free(e), te_free(prev));

        ret->function = comma;
    }

    return ret;
}

#define TE_FUN(...) ((double (*)(__VA_ARGS__))n->function)
#define M(e) te_eval(n->parameters[e])

double te_eval(const te_expr *n)
{
    if (!n)
        return NAN;

    switch (TYPE_MASK(n->type))
    {
    case TE_CONSTANT:
        return n->value;
    case TE_VARIABLE:
        return *n->bound;

    case TE_FUNCTION0:
    case TE_FUNCTION1:
    case TE_FUNCTION2:
    case TE_FUNCTION3:
    case TE_FUNCTION4:
    case TE_FUNCTION5:
    case TE_FUNCTION6:
    case TE_FUNCTION7:
        switch (ARITY(n->type))
        {
        case 0:
            return TE_FUN(void)();
        case 1:
            return TE_FUN(double)(M(0));
        case 2:
            return TE_FUN(double, double)(M(0), M(1));
        case 3:
            return TE_FUN(double, double, double)(M(0), M(1), M(2));
        case 4:
            return TE_FUN(double, double, double, double)(M(0), M(1), M(2), M(3));
        case 5:
            return TE_FUN(double, double, double, double, double)(M(0), M(1), M(2), M(3), M(4));
        case 6:
            return TE_FUN(double, double, double, double, double, double)(M(0), M(1), M(2), M(3), M(4), M(5));
        case 7:
            return TE_FUN(double, double, double, double, double, double, double)(M(0), M(1), M(2), M(3), M(4), M(5), M(6));
        default:
            return NAN;
        }

    case TE_CLOSURE0:
    case TE_CLOSURE1:
    case TE_CLOSURE2:
    case TE_CLOSURE3:
    case TE_CLOSURE4:
    case TE_CLOSURE5:
    case TE_CLOSURE6:
    case TE_CLOSURE7:
        switch (ARITY(n->type))
        {
        case 0:
            return TE_FUN(void *)(n->parameters[0]);
        case 1:
            return TE_FUN(void *, double)(n->parameters[1], M(0));
        case 2:
            return TE_FUN(void *, double, double)(n->parameters[2], M(0), M(1));
        case 3:
            return TE_FUN(void *, double, double, double)(n->parameters[3], M(0), M(1), M(2));
        case 4:
            return TE_FUN(void *, double, double, double, double)(n->parameters[4], M(0), M(1), M(2), M(3));
        case 5:
            return TE_FUN(void *, double, double, double, double, double)(n->parameters[5], M(0), M(1), M(2), M(3), M(4));
        case 6:
            return TE_FUN(void *, double, double, double, double, double, double)(n->parameters[6], M(0), M(1), M(2), M(3), M(4), M(5));
        case 7:
            return TE_FUN(void *, double, double, double, double, double, double, double)(n->parameters[7], M(0), M(1), M(2), M(3), M(4), M(5), M(6));
        default:
            return NAN;
        }

    default:
        return NAN;
    }
}

#undef TE_FUN
#undef M

static void optimize(te_expr *n)
{
    /* Evaluates as much as possible. */
    if (n->type == TE_CONSTANT)
        return;
    if (n->type == TE_VARIABLE)
        return;

    /* Only optimize out functions flagged as pure. */
    if (IS_PURE(n->type))
    {
        const int arity = ARITY(n->type);
        int known = 1;
        int i;
        for (i = 0; i < arity; ++i)
        {
            optimize(n->parameters[i]);
            if (((te_expr *)(n->parameters[i]))->type != TE_CONSTANT)
            {
                known = 0;
            }
        }
        if (known)
        {
            const double value = te_eval(n);
            te_free_parameters(n);
            n->type = TE_CONSTANT;
            n->value = value;
        }
    }
}

te_expr *te_compile(const char *expression, const te_variable *variables, int var_count, int *error)
{
    state s;
    s.start = s.next = expression;
    s.lookup = variables;
    s.lookup_len = var_count;

    next_token(&s);
    te_expr *root = list(&s);
    if (root == NULL)
    {
        if (error)
            *error = -1;
        return NULL;
    }

    if (s.type != TOK_END)
    {
        te_free(root);
        if (error)
        {
            *error = (s.next - s.start);
            if (*error == 0)
                *error = 1;
        }
        return 0;
    }
    else
    {
        optimize(root);
        if (error)
            *error = 0;
        return root;
    }
}

double te_interp(const char *expression, int *error)
{
    te_expr *n = te_compile(expression, 0, 0, error);
    if (n == NULL)
    {
        return NAN;
    }

    double ret;
    if (n)
    {
        ret = te_eval(n);
        te_free(n);
    }
    else
    {
        ret = NAN;
    }
    return ret;
}

static void pn(const te_expr *n, int depth)
{
    int i, arity;
    printf("%*s", depth, "");

    switch (TYPE_MASK(n->type))
    {
    case TE_CONSTANT:
        printf("%f\n", n->value);
        break;
    case TE_VARIABLE:
        printf("bound %p\n", n->bound);
        break;

    case TE_FUNCTION0:
    case TE_FUNCTION1:
    case TE_FUNCTION2:
    case TE_FUNCTION3:
    case TE_FUNCTION4:
    case TE_FUNCTION5:
    case TE_FUNCTION6:
    case TE_FUNCTION7:
    case TE_CLOSURE0:
    case TE_CLOSURE1:
    case TE_CLOSURE2:
    case TE_CLOSURE3:
    case TE_CLOSURE4:
    case TE_CLOSURE5:
    case TE_CLOSURE6:
    case TE_CLOSURE7:
        arity = ARITY(n->type);
        printf("f%d", arity);
        for (i = 0; i < arity; i++)
        {
            printf(" %p", n->parameters[i]);
        }
        printf("\n");
        for (i = 0; i < arity; i++)
        {
            pn(n->parameters[i], depth + 1);
        }
        break;
    }
}

void te_print(const te_expr *n)
{
    pn(n, 0);
}
