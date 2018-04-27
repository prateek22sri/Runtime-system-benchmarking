/* -----------------------------------------------------------------------
   unix64.S - Copyright (c) 2002  Bo Thorsen <bo@suse.de>
	      Copyright (c) 2008  Red Hat, Inc

   x86-64 Foreign Function Interface 

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   ``Software''), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be included
   in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED ``AS IS'', WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
   HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
   DEALINGS IN THE SOFTWARE.


   PORT TO THE INTEL MIC ARCHITECTURE:
   EMILIO CASTILLO VILLAR
   CRISTOBAL CAMARERO COTERILLO

   UNIVERSITY OF CANTABRIA
   SPAIN
   ----------------------------------------------------------------------- */

/
#ifdef __x86_64__
#define LIBFFI_ASM	
#include <fficonfig.h>
#include <ffi.h>

.text

/* ffi_call_unix64 (void *args, unsigned long bytes, unsigned flags,
	            void *raddr, void (*fnaddr)(void));

   Bit o trickiness here -- ARGS+BYTES is the base of the stack frame
   for this function.  This has been allocated by ffi_call.  We also
   deallocate some of the stack that has been alloca'd.  */

	.align	2
	.globl	ffi_call_unix64
	.type	ffi_call_unix64,@function

ffi_call_unix64:
.LUW0:
	movq	(%rsp), %r10		/* Load return address.  */
	leaq	(%rdi, %rsi), %rax	/* Find local stack base.  */
	movq	%rdx, (%rax)		/* Save flags.  */
	movq	%rcx, 8(%rax)		/* Save raddr.  */
	movq	%rbp, 16(%rax)		/* Save old frame pointer.  */
	movq	%r10, 24(%rax)		/* Relocate return address.  */
	movq	%rax, %rbp		/* Finalize local stack frame.  */
.LUW1:
	movq	%rdi, %r10		/* Save a copy of the register area. */
	movq	%r8, %r11		/* Save a copy of the target fn.  */
	movl	%r9d, %eax		/* Set number of SSE registers.  */

	/* Load up all argument registers.  */
	movq	(%r10), %rdi
	movq	8(%r10), %rsi
	movq	16(%r10), %rdx
	movq	24(%r10), %rcx
	movq	32(%r10), %r8
	movq	40(%r10), %r9
	testl	%eax, %eax
	jnz	.Lload_sse
.Lret_from_load_sse:

	/* Deallocate the reg arg area.  */
	leaq	560(%r10), %rsp

	/* Call the user function.  */
	call	*%r11

	/* Deallocate stack arg area; local stack frame in redzone.  */
	leaq	24(%rbp), %rsp

	movq	0(%rbp), %rcx		/* Reload flags.  */
	movq	8(%rbp), %rdi		/* Reload raddr.  */
	movq	16(%rbp), %rbp		/* Reload old frame pointer.  */
.LUW2:

	/* The first byte of the flags contains the FFI_TYPE.  */
	movzbl	%cl, %r10d
	leaq	.Lstore_table(%rip), %r11
	movslq	(%r11, %r10, 4), %r10
	addq	%r11, %r10
	jmp	*%r10

.Lstore_table:
	.long	.Lst_void-.Lstore_table		/* FFI_TYPE_VOID */
	.long	.Lst_sint32-.Lstore_table	/* FFI_TYPE_INT */
	.long	.Lst_float-.Lstore_table	/* FFI_TYPE_FLOAT */
	.long	.Lst_double-.Lstore_table	/* FFI_TYPE_DOUBLE */
	.long	.Lst_ldouble-.Lstore_table	/* FFI_TYPE_LONGDOUBLE */
	.long	.Lst_uint8-.Lstore_table	/* FFI_TYPE_UINT8 */
	.long	.Lst_sint8-.Lstore_table	/* FFI_TYPE_SINT8 */
	.long	.Lst_uint16-.Lstore_table	/* FFI_TYPE_UINT16 */
	.long	.Lst_sint16-.Lstore_table	/* FFI_TYPE_SINT16 */
	.long	.Lst_uint32-.Lstore_table	/* FFI_TYPE_UINT32 */
	.long	.Lst_sint32-.Lstore_table	/* FFI_TYPE_SINT32 */
	.long	.Lst_int64-.Lstore_table	/* FFI_TYPE_UINT64 */
	.long	.Lst_int64-.Lstore_table	/* FFI_TYPE_SINT64 */
	.long	.Lst_struct-.Lstore_table	/* FFI_TYPE_STRUCT */
	.long	.Lst_int64-.Lstore_table	/* FFI_TYPE_POINTER */

	.align 2
.Lst_void:
	ret
	.align 2

.Lst_uint8:
	movzbq	%al, %rax
	movq	%rax, (%rdi)
	ret
	.align 2
.Lst_sint8:
	movsbq	%al, %rax
	movq	%rax, (%rdi)
	ret
	.align 2
.Lst_uint16:
	movzwq	%ax, %rax
	movq	%rax, (%rdi)
	.align 2
.Lst_sint16:
	movswq	%ax, %rax
	movq	%rax, (%rdi)
	ret
	.align 2
.Lst_uint32:
	movl	%eax, %eax
	movq	%rax, (%rdi)
	.align 2
.Lst_sint32:
	cltq
	movq	%rax, (%rdi)
	ret
	.align 2
.Lst_int64:
	movq	%rax, (%rdi)
	ret

	.align 2
.Lst_float:

	movl 	$1, %eax
	kmov	%eax, %k1
	vpackstorelps %zmm0, (%rdi){%k1}
	vpackstorehps %zmm0, 64(%rdi){%k1}
	/*movss	%xmm0, (%rdi)*/
	ret
	.align 2
.Lst_double:
	movl 	$1, %eax
	kmov	%eax, %k1
	vpackstorelpd %zmm0, (%rdi){%k1}
	vpackstorehpd %zmm0, 64(%rdi){%k1}
	/*movsd	%xmm0, (%rdi)*/
	ret
.Lst_ldouble:
	fstpt	(%rdi)
	ret

	.align 2
.Lst_struct:
	leaq	-20(%rsp), %rsi		/* Scratch area in redzone.  */

	/* We have to locate the values now, and since we don't want to
	   write too much data into the user's return value, we spill the
	   value to a 16 byte scratch area first.  Bits 8, 9, and 10
	   control where the values are located.  Only one of the three
	   bits will be set; see ffi_prep_cif_machdep for the pattern.  */


	movq 	%rax, %r10
	movl 	$1, %eax
	kmov	%eax, %k1
	movq	%r10, %rax

	vpackstorelpd %zmm0, -200(%rsp){%k1}
	vpackstorehpd %zmm0, -136(%rsp){%k1}
	movq 	-200(%rsp), %r10


	vpackstorelpd %zmm1, -200(%rsp){%k1}
	vpackstorehpd %zmm1, -136(%rsp){%k1}
	movq 	-200(%rsp), %r11

	/*movd	%zmm0, %r10
	movd	%zmm1, %r11*/
	testl	$0x100, %ecx
        jz .Lst_struct_n1
		movq	%rax, %rdx
		movq	%r10, %rax
.Lst_struct_n1:

	testl	$0x200, %ecx
	jz .Lst_struct_n2
		movq	%r10, %rdx
.Lst_struct_n2:
	testl	$0x400, %ecx
	jz .Lst_struct_n3
		movq	%r10, %rax
		movq	%r11, %rdx
.Lst_struct_n3:
	movq	%rax, (%rsi)
	movq	%rdx, 8(%rsi)

	/* Bits 12-31 contain the true size of the structure.  Copy from
	   the scratch area to the true destination.  */
	shrl	$12, %ecx
	rep movsb
	ret

	/* Many times we can avoid loading any SSE registers at all.
	   It's not worth an indirect jump to load the exact set of
	   SSE registers needed; zero or all is a good compromise.  */
	.align 2
.LUW3:
.Lload_sse:
	
	vloadunpacklq	48(%r10),  %zmm0
	vloadunpacklq	112(%r10), %zmm1
	vloadunpacklq	176(%r10), %zmm2
	vloadunpacklq	240(%r10), %zmm3
	vloadunpacklq	304(%r10), %zmm4
	vloadunpacklq	368(%r10), %zmm5
	vloadunpacklq	432(%r10), %zmm6
	vloadunpacklq	496(%r10), %zmm7

	vloadunpackhq	112(%r10), %zmm0
	vloadunpackhq	176(%r10), %zmm1
	vloadunpackhq	240(%r10), %zmm2
	vloadunpackhq	304(%r10), %zmm3
	vloadunpackhq	368(%r10), %zmm4
	vloadunpackhq	432(%r10), %zmm5
	vloadunpackhq	496(%r10), %zmm6
	vloadunpackhq	560(%r10), %zmm7

	/*vmovaps	48(%r10),  %zmm0
	vmovaps	112(%r10), %zmm1
	vmovaps	176(%r10), %zmm2
	vmovaps	240(%r10), %zmm3
	vmovaps	304(%r10), %zmm4
	vmovaps	368(%r10), %zmm5
	vmovaps	432(%r10), %zmm6
	vmovaps	496(%r10), %zmm7*/
	jmp	.Lret_from_load_sse

.LUW4:
	.size    ffi_call_unix64,.-ffi_call_unix64

	.align	2
	.globl ffi_closure_unix64
	.type	ffi_closure_unix64,@function

ffi_closure_unix64:
.LUW5:
	/* The carry flag is set by the trampoline iff SSE registers
	   are used.  Don't clobber it before the branch instruction.  */
	leaq    -584(%rsp), %rsp
.LUW6:
	movq	%rdi, (%rsp)
	movq    %rsi, 8(%rsp)
	movq    %rdx, 16(%rsp)
	movq    %rcx, 24(%rsp)
	movq    %r8, 32(%rsp)
	movq    %r9, 40(%rsp)
	jc      .Lsave_sse
.Lret_from_save_sse:

	movq	%r10, %rdi
	leaq	560(%rsp), %rsi
	movq	%rsp, %rdx
	leaq	592(%rsp), %rcx
	call	ffi_closure_unix64_inner@PLT

	/* Deallocate stack frame early; return value is now in redzone.  */
	addq	$584, %rsp
.LUW7:

	/* The first byte of the return value contains the FFI_TYPE.  */
	movzbl	%al, %r10d
	leaq	.Lload_table(%rip), %r11
	movslq	(%r11, %r10, 4), %r10
	addq	%r11, %r10
	jmp	*%r10

.Lload_table:
	.long	.Lld_void-.Lload_table		/* FFI_TYPE_VOID */
	.long	.Lld_int32-.Lload_table		/* FFI_TYPE_INT */
	.long	.Lld_float-.Lload_table		/* FFI_TYPE_FLOAT */
	.long	.Lld_double-.Lload_table	/* FFI_TYPE_DOUBLE */
	.long	.Lld_ldouble-.Lload_table	/* FFI_TYPE_LONGDOUBLE */
	.long	.Lld_int8-.Lload_table		/* FFI_TYPE_UINT8 */
	.long	.Lld_int8-.Lload_table		/* FFI_TYPE_SINT8 */
	.long	.Lld_int16-.Lload_table		/* FFI_TYPE_UINT16 */
	.long	.Lld_int16-.Lload_table		/* FFI_TYPE_SINT16 */
	.long	.Lld_int32-.Lload_table		/* FFI_TYPE_UINT32 */
	.long	.Lld_int32-.Lload_table		/* FFI_TYPE_SINT32 */
	.long	.Lld_int64-.Lload_table		/* FFI_TYPE_UINT64 */
	.long	.Lld_int64-.Lload_table		/* FFI_TYPE_SINT64 */
	.long	.Lld_struct-.Lload_table	/* FFI_TYPE_STRUCT */
	.long	.Lld_int64-.Lload_table		/* FFI_TYPE_POINTER */

	.align 2
.Lld_void:
	ret

	.align 2
.Lld_int8:
	movzbl	-24(%rsp), %eax
	ret
	.align 2
.Lld_int16:
	movzwl	-24(%rsp), %eax
	ret
	.align 2
.Lld_int32:
	movl	-24(%rsp), %eax
	ret
	.align 2
.Lld_int64:
	movq	-24(%rsp), %rax
	ret

	.align 2
.Lld_float:
	vbroadcastss	-24(%rsp), %zmm0
	/*movss	-24(%rsp), %xmm0*/
	ret
	.align 2
.Lld_double:
	vbroadcastsd	-24(%rsp), %zmm0
	/*movsd	-24(%rsp), %xmm0*/
	ret
	.align 2
.Lld_ldouble:
	fldt	-24(%rsp)
	ret

	.align 2
.Lld_struct:
	/* There are four possibilities here, %rax/%rdx, %xmm0/%rax,
	   %rax/%xmm0, %xmm0/%xmm1.  We collapse two by always loading
	   both rdx and xmm1 with the second word.  For the remaining,
	   bit 8 set means xmm0 gets the second word, and bit 9 means
	   that rax gets the second word.  */
	movq	-24(%rsp), %rcx
	movq	-16(%rsp), %rdx
	vbroadcastsd	-16(%rsp), %zmm1
	/*movq	-16(%rsp), %xmm1*/
	testl	$0x100, %eax
	jz .Lld_struct_1

	movq	%rdx, %rcx
.Lld_struct_1:
	subq	$8, %rsp
	movq	%rcx, (%rsp)
	addq	$8, %rsp
	vbroadcastss	(%rsp), %zmm0
	
	/*movd	%rcx, %zmm0*/
	movq	-24(%rsp), %rax
	testl	$0x200, %eax
	jz .Lld_struct_2
	movq	%rdx, %rax
.Lld_struct_2:
	ret

	/* See the comment above .Lload_sse; the same logic applies here.  */
	.align 2
.LUW8:
.Lsave_sse:
	vpackstorelq	%zmm0, 48(%rsp)
	vpackstorelq	%zmm1, 112(%rsp)
	vpackstorelq	%zmm2, 176(%rsp)
	vpackstorelq	%zmm3, 240(%rsp)
	vpackstorelq	%zmm4, 304(%rsp)
	vpackstorelq	%zmm5, 368(%rsp)
	vpackstorelq	%zmm6, 432(%rsp)
	vpackstorelq	%zmm7, 496(%rsp)
	
	vpackstorehq	%zmm0, 112(%rsp)
	vpackstorehq	%zmm1, 176(%rsp)
	vpackstorehq	%zmm2, 240(%rsp)
	vpackstorehq	%zmm3, 304(%rsp)
	vpackstorehq	%zmm4, 368(%rsp)
	vpackstorehq	%zmm5, 432(%rsp)
	vpackstorehq	%zmm6, 496(%rsp)
	vpackstorehq	%zmm7, 560(%rsp)
        /*vmovaps	%zmm0, 48(%rsp)
	vmovaps	%zmm1, 112(%rsp)
	vmovaps	%zmm2, 176(%rsp)
	vmovaps	%zmm3, 240(%rsp)
	vmovaps	%zmm4, 304(%rsp)
	vmovaps	%zmm5, 368(%rsp)
	vmovaps	%zmm6, 432(%rsp)
	vmovaps	%zmm7, 496(%rsp) */
	jmp	.Lret_from_save_sse

.LUW9:
	.size	ffi_closure_unix64,.-ffi_closure_unix64

	.section	.eh_frame,"a",@progbits
.Lframe1:
	.long	.LECIE1-.LSCIE1		/* CIE Length */
.LSCIE1:
	.long	0			/* CIE Identifier Tag */
	.byte	1			/* CIE Version */
	.ascii "zR\0"			/* CIE Augmentation */
	.uleb128 1			/* CIE Code Alignment Factor */
	.sleb128 -8			/* CIE Data Alignment Factor */
	.byte	0x10			/* CIE RA Column */
	.uleb128 1			/* Augmentation size */
	.byte	0x1b			/* FDE Encoding (pcrel sdata4) */
	.byte	0xc			/* DW_CFA_def_cfa, %rsp offset 8 */
	.uleb128 7
	.uleb128 8
	.byte	0x80+16			/* DW_CFA_offset, %rip offset 1*-8 */
	.uleb128 1
	.align 8
.LECIE1:
.LSFDE1:
	.long	.LEFDE1-.LASFDE1	/* FDE Length */
.LASFDE1:
	.long	.LASFDE1-.Lframe1	/* FDE CIE offset */
#if HAVE_AS_X86_PCREL
	.long	.LUW0-.			/* FDE initial location */
#else
	.long	.LUW0@rel
#endif
	.long	.LUW4-.LUW0		/* FDE address range */
	.uleb128 0x0			/* Augmentation size */

	.byte	0x4			/* DW_CFA_advance_loc4 */
	.long	.LUW1-.LUW0

	/* New stack frame based off rbp.  This is a itty bit of unwind
	   trickery in that the CFA *has* changed.  There is no easy way
	   to describe it correctly on entry to the function.  Fortunately,
	   it doesn't matter too much since at all points we can correctly
	   unwind back to ffi_call.  Note that the location to which we
	   moved the return address is (the new) CFA-8, so from the
	   perspective of the unwind info, it hasn't moved.  */
	.byte	0xc			/* DW_CFA_def_cfa, %rbp offset 32 */
	.uleb128 6
	.uleb128 32
	.byte	0x80+6			/* DW_CFA_offset, %rbp offset 2*-8 */
	.uleb128 2
	.byte	0xa			/* DW_CFA_remember_state */

	.byte	0x4			/* DW_CFA_advance_loc4 */
	.long	.LUW2-.LUW1
	.byte	0xc			/* DW_CFA_def_cfa, %rsp offset 8 */
	.uleb128 7
	.uleb128 8
	.byte	0xc0+6			/* DW_CFA_restore, %rbp */

	.byte	0x4			/* DW_CFA_advance_loc4 */
	.long	.LUW3-.LUW2
	.byte	0xb			/* DW_CFA_restore_state */

	.align 8
.LEFDE1:
.LSFDE3:
	.long	.LEFDE3-.LASFDE3	/* FDE Length */
.LASFDE3:
	.long	.LASFDE3-.Lframe1	/* FDE CIE offset */
#if HAVE_AS_X86_PCREL
	.long	.LUW5-.			/* FDE initial location */
#else
	.long	.LUW5@rel
#endif
	.long	.LUW9-.LUW5		/* FDE address range */
	.uleb128 0x0			/* Augmentation size */

	.byte	0x4			/* DW_CFA_advance_loc4 */
	.long	.LUW6-.LUW5
	.byte	0xe			/* DW_CFA_def_cfa_offset */
	.uleb128 208
	.byte	0xa			/* DW_CFA_remember_state */

	.byte	0x4			/* DW_CFA_advance_loc4 */
	.long	.LUW7-.LUW6
	.byte	0xe			/* DW_CFA_def_cfa_offset */
	.uleb128 8

	.byte	0x4			/* DW_CFA_advance_loc4 */
	.long	.LUW8-.LUW7
	.byte	0xb			/* DW_CFA_restore_state */

	.align 8
.LEFDE3:

#endif /* __x86_64__ */

#if defined __ELF__ && defined __linux__
	.section	.note.GNU-stack,"",@progbits
#endif
