#ifndef __DEFINES_H
#define __DEFINES_H

#include <Windows.h>

// POINTER
#define PTR_ADD_OFFSET(base, offset) ((LPVOID)((ULONG_PTR)base + (offset)))
#define PTR_SUB_OFFSET(base, offset) ((LPVOID)((ULONG_PTR)base - (offset)))

// TRY FINALLY EXCEPT
#define TRY(return_type, return_value) return_type __try_var_value = (return_value); __try
#define FINALLY __finally
#define EXCEPT __except
#define LEAVE(return_value) __try_var_value = (return_value); __leave;
#define TRY_VALUE (__try_var_value)

// IF
#define DO_IF(expr, do_expr) if (expr) { do_expr; }
#define DO_ELSE(expr, do_expr) DO_IF(!(expr), do_expr)

// LOOP
#define CONTINUE continue;
#define CONTINUE_IF(expr) DO_IF(expr, CONTINUE)

#define BREAK break
#define BREAK_IF(expr) DO_IF(expr, BREAK)

#define RETURN return
#define RETURN_IF(expr) DO_IF((expr), RETURN)
#define RETURN_VALUE_IF(expr, value) DO_IF((expr), RETURN(value))

#endif
