AC_DEFUN([AX_CFI_DIRECTIVES],
  [AC_MSG_CHECKING([if the assembler supports CFI directives])
   AC_TRY_COMPILE([asm (".cfi_startproc\n\t.cfi_endproc");],,
                  [have_cfi_directives=yes],
                  [have_cfi_directives=no])

   AS_IF([test "x$have_cfi_directives" != xno],
     [AC_DEFINE(HAVE_CFI_DIRECTIVES, 1, [Define if the assembler supports CFI directives.])])
   AC_MSG_RESULT([$have_cfi_directives])])
