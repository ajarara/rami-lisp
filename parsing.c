#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpc.h"

#include <readline/readline.h>
#include <readline/history.h>
/* #include <editline/history.h> */



enum { LVAL_NUM, LVAL_ERR, LVAL_SYM, LVAL_SEXPR };
enum { LERR_DIV_ZERO, LERR_BAD_OP, LERR_BAD_NUM };

/* notice the initial reference to lval. What does that do? */
typedef struct lval {
  int type;
  long num;
  char* err;
  char* sym;
  /* not reference counting, just the length of the list. */
  int count;
  /* is the struct keyword necessary here? */
  struct lval** cell;
} lval;

lval* lval_num(long x) {
  lval* v = malloc(sizeof(lval));
  /* notice we're no longer using the dot notation. Why? */
  v->type = LVAL_NUM;
  v->num = x;
  return v;
}

lval* lval_err(char* m) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_ERR;
  v->err = malloc(strlen(m) + 1);
  /* is the reason we don't alias the pointers to make it conform with v->err? */
  strcpy(v->err, m);
  return v;
}


lval* lval_sym(char* s) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SYM;
  v->sym = malloc(strlen(s) + 1);
  strcpy(v->sym, s);
  return v;
}

lval* lval_sexpr(void) {
  lval* v = malloc(sizeof(lval));
  v->type = LVAL_SEXPR;
  v->count = 0;
  /* Why bother initializing an sexpr's cell? */
  v->cell = NULL;
  return v;
}


void lval_del(lval* v) {
  switch (v->type) {
  case LVAL_NUM:
    break;
  case LVAL_ERR:
    free(v->err); break;
  case LVAL_SYM:
    free(v->sym); break;
  case LVAL_SEXPR:
    for (int i = 0; i > v->count; i++) {
      lval_del(v->cell[i]);
    }
    free(v->cell);
    break;
  }

  free(v);
}


lval* lval_read_num(mpc_ast_t* t) {
  errno = 0;
  long x = strtol(t->contents, NULL, 10);
  return errno != ERANGE ?
    lval_num(x) : lval_err("invalid number");
}


/* what's with the third line? */
lval* lval_add(lval* v, lval* x) {
  v->count++;
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  v->cell[v->count-1] = x;
  return v;
}


lval* lval_read(mpc_ast_t* t) {
  if (strstr(t->tag, "number")) { return lval_read_num(t); }
  if (strstr(t->tag, "symbol")) { return lval_sym(t->contents); }

  lval* x = NULL;

  /* root? like the prompt? kinda hacky */
  if (strcmp(t->tag, ">") == 0) { x = lval_sexpr(); }
  if (strstr(t->tag, "sexpr")) { x = lval_sexpr(); }
  
  /* 
     For the rest of the ast, recurse on the children, excepting parens.
     What is this regex tag?
   */
  
  for (int i = 0; i < t->children_num; i++) {
    if (strcmp(t->children[i]->contents, "(") == 0) { continue; }
    if (strcmp(t->children[i]->contents, ")") == 0) { continue; }
    if (strcmp(t->children[i]->tag, "regex") == 0) { continue; }
    x = lval_add(x, lval_read(t->children[i]));
  }
  return x;
}

void lval_print(lval* v);

void lval_expr_print(lval* v, char open, char close) {
  putchar(open);
  for (int i = 0; i < v->count; i++) {
    lval_print(v->cell[i]);
    if (i != (v->count-1)) {
      putchar(' ');
    }
  }
  putchar(close);
}

void lval_print(lval* v) {
  switch(v->type) {
  case LVAL_NUM:
    printf("%li", v->num);
    break;
  case LVAL_ERR:
    printf("Error: %s", v->err);
    break;
  case LVAL_SYM:
    printf("%s", v->sym);
    break;
  case LVAL_SEXPR:
    /* recurse */
    lval_expr_print(v, '(', ')');
    break;
  }
}


void lval_println(lval* v) {
  lval_print(v);
  putchar('\n');
}


lval* lval_pop(lval* v, int i) {
  /* get out the item at position i */
  lval* x = v->cell[i];
  /* shift memory after the... what?! */
  /* figured out the problem: I was writing to &v->cell which is a pointer (since **lval */
  memmove(&v->cell[i], &v->cell[i+1],
          sizeof(lval*) * (v->count-i-1));
  v->count--;
  /* realloc? I need to figure out what all this is doing. */
  v->cell = realloc(v->cell, sizeof(lval*) * v->count);
  return x;
}

lval* lval_take(lval* v, int i) {
  lval* x = lval_pop(v, i);
  lval_del(v);
  return x;
}

lval* builtin_op(lval* a, char* op) {
  /*  */
  for (int i = 0; i < a->count; i++) {
    if (a->cell[i]->type != LVAL_NUM) {
      lval_del(a);
      return lval_err("One of the arguments given is not a number!");
    }
  }

  lval* x = lval_pop(a, 0);

  /* this is intended to handle these cases:
   (+ 5) => 5
   (- 5) => -5
   (/ 5) => 1/5 which _might_ round to 0, which will make me sad
   (* 5) => 5
  */
  if (a->count == 0) {
    
    /* no reason to perform this check if it's a no op */
    /* will this be compiled out? */
    if (strcmp(op, "+") == 0) {
    }
    if (strcmp(op, "-") == 0) {
      /* negate me! */
      x->num = -x->num;
    }
    /* also no op */
    if (strcmp(op, "*") == 0) {
    }
    if(strcmp(op, "/") == 0) {
      /* initially I took this check out into its own void function
         but the problem is that this behavior is fundamentally different 
         so I kept it inlined. */
      if (x->num == 0) {
        lval_del(x);
        x = lval_err("Division By Zero!");
      } else {
        x->num = 1 / x->num;
      }
    }
  }

  /* hmm why not put this in an else block? */
  while (a->count > 0) {
    lval* y = lval_pop(a, 0);
    
    if (strcmp(op, "+") == 0) { x->num += y->num; }
    if (strcmp(op, "-") == 0) { x->num -= y->num; }
    if (strcmp(op, "*") == 0) { x->num *= y->num; }
    if (strcmp(op, "/") == 0) {
      if (y->num == 0 ) {
        lval_del(x);
        x = lval_err("Division By Zero!");
      } else {
        x->num /= y->num;
      }
    }
    lval_del(y);
  }
  lval_del(a);
  return x;
}

lval* lval_eval(lval* v);
    
lval* lval_eval_expr(lval* v) {

  /* Evaluate the children. Give them a grade. 
     Tell their parents they've been skipping classes. */
  for (int i = 0; i < v->count; i++) {
    v->cell[i] = lval_eval(v->cell[i]);
  }

  /* if any yield an error, pass the first error to lval_take */
  for (int i =0; i < v->count; i++) {
    if (v->cell[i]->type == LVAL_ERR) {
      return lval_take(v, i);
    }
  }

  /* Empty expression */
  if (v->count == 0) { return v; }

  /* Hmm so lval_take isn't a error handling function but a repr function */
  if (v->count == 1) { return lval_take(v, 0); }

  /* 
     inspect first child and assert it's a symbol
     this is probably where we would put in quoting later on. 
  */
  lval* f = lval_pop(v, 0);
  if (f->type != LVAL_SYM) {
    lval_del(f);
    lval_del(v);
    return lval_err("S-expression does not start with symbol!");
  }

  /* okay, return the result. */
  lval* result = builtin_op(v, f->sym);
  lval_del(f);
  return result;
}

lval* lval_eval(lval* v) {
  if (v->type == LVAL_SEXPR) {
    return lval_eval_expr(v);
  }
  return v;
}

int main(int argc, char** argv) {
  mpc_parser_t* Number    = mpc_new("number");
  mpc_parser_t* Symbol    = mpc_new("symbol");
  mpc_parser_t* Sexpr     = mpc_new("sexpr");
  mpc_parser_t* Expr      = mpc_new("expr");
  mpc_parser_t* Lispy     = mpc_new("lispy");
  
  mpca_lang(MPCA_LANG_DEFAULT,
            "                                          \
    number : /-?[0-9]+/ ;                    \
    symbol : '+' | '-' | '*' | '/' ;         \
    sexpr  : '(' <expr>* ')' ;               \
    expr   : <number> | <symbol> | <sexpr> ; \
    lispy  : /^/ <expr>* /$/ ;               \
  ", Number, Symbol, Sexpr, Expr, Lispy);
  puts("Lispy Version 0.0.0.2");
  puts("Press C-c to exit");

  while (1) {
    char* input = readline("🙀> ");

    add_history(input);

    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Lispy, &r)) {
      lval* x = lval_eval(lval_read(r.output));
      lval_println(x);
      lval_del(x);
      
      mpc_ast_delete(r.output);
    } else {
      /* Otherwise Print the Error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    free(input);
  }
  mpc_cleanup(5, Number, Symbol, Sexpr, Expr, Lispy);

  return 0;
}
