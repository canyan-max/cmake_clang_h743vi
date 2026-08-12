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

Add the project's Doxygen-style function comment to public APIs and non-trivial private functions. Use `<stdint.h>` fixed-width types at API boundaries.
