   .global vectors
   .global _c_int00
   .global vector1
   .global vector2
   .global vector3
   .global interrupt4
   .global vector5
   .global vector6
   .global vector7
   .global int8_pb
   .global vector9
   .global vector10
   .global vector11
   .global vector12
   .global vector13
   .global vector14
   .global vector15

      .ref _c_int00				;entry address


; This is a macro that instantiates one entry in the interrupt service table.
VEC_ENTRY .macro addr
    STW   B0,*--B15
    MVKL  addr,B0
    MVKH  addr,B0
    B     B0
    LDW   *B15++,B0
    NOP   2
    NOP
    NOP
   .endm

; This is a dummy interrupt service routine used to initialize the IST.
vec_dummy:
  B    B3
  NOP  5

; This is the actual interrupt service table (IST).
 .sect ".vecs"
 .align 1024

vectors:
vector0:   VEC_ENTRY _c_int00      ;RESET
vector1:   VEC_ENTRY vec_dummy    ;NMI
vector2:   VEC_ENTRY vec_dummy    ;RSVD
vector3:   VEC_ENTRY vec_dummy    ;RSVD
vector4:   VEC_ENTRY interrupt4   ;Interrupt4 ISR
vector5:   VEC_ENTRY vec_dummy
vector6:   VEC_ENTRY vec_dummy
vector7:   VEC_ENTRY vec_dummy
vector8:   VEC_ENTRY int8_pb		; DLU PB interruption
vector9:   VEC_ENTRY vec_dummy
vector10:  VEC_ENTRY vec_dummy
vector11:  VEC_ENTRY vec_dummy
vector12:  VEC_ENTRY vec_dummy
vector13:  VEC_ENTRY vec_dummy
vector14:  VEC_ENTRY vec_dummy
vector15:  VEC_ENTRY vec_dummy


;* =============================================================================
;*   Automated Revision Information
;*   Changed: $Date: 2007-09-11 11:05:40 -0500 (Tue, 11 Sep 2007) $
;*   Revision: $Revision: 3960 $
;* =============================================================================


