# C and header comment style

Apply this style to every generated project-owned `.c` and `.h` file unless the target repository explicitly overrides it.

## File header

```c
/**
 ******************************************************************************
 *@file               :   file_name.c
 *@brief              :   Brief description.
 *@version            :   V1.0
 *@note               :   1 tab == 4 spaces!
 ******************************************************************************
 */
```

## C source layout

Use these sections in order, retaining empty sections when they make the template easier to extend:

```c
/* Includes -----------------------------------------------------------------*/
/* define   -----------------------------------------------------------------*/
/* typedef ------------------------------------------------------------------*/
/* variables ----------------------------------------------------------------*/
/* Private  functions  ------------------------------------------------------*/
/* Exported functions -------------------------------------------------------*/
/* end of file --------------------------------------------------------------*/
```

## Header layout

Use a conventional include guard, C++ linkage block, and these sections:

```c
/* Define to prevent recursive inclusion ------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif
/* Includes -----------------------------------------------------------------*/
/* define -------------------------------------------------------------------*/
/* typedef ------------------------------------------------------------------*/
/* variables ----------------------------------------------------------------*/
/* function  ----------------------------------------------------------------*/
#ifdef __cplusplus
}
#endif
/* end of file --------------------------------------------------------------*/
```

Add the project's Doxygen-style function comment to public APIs and non-trivial
private functions. Use `<stdint.h>` fixed-width types directly at API
boundaries; do not create aliases such as `u8` or `plat_u16_t` that add no
domain meaning. Define custom types for semantics such as status, resource ID,
model, or state. Each public header must include the standard headers needed by
its own declarations rather than relying on transitive includes.
