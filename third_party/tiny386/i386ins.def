// 0: X 1: R 2: W 3: RW

#ifdef I
I(C(0x00), EbGb, 3, ADDb)
I(C(0x01), EvGv, 3, ADD)
I(C(0x02), GbEb, 1, ADDb)
I(C(0x03), GvEv, 1, ADD)
I(C(0x04), ALIb, 0, ADDb)
I(C(0x05), AXIv, 0, ADD)
I(C(0x06), _,    0, PUSH_ES)
I(C(0x07), _,    0, POP_ES)
I(C(0x08), EbGb, 3, ORb)
I(C(0x09), EvGv, 3, OR)
I(C(0x0a), GbEb, 1, ORb)
I(C(0x0b), GvEv, 1, OR)
I(C(0x0c), ALIb, 0, ORb)
I(C(0x0d), AXIv, 0, OR)
I(C(0x0e), _,    0, PUSH_CS)
//  0x0f   2-byte escape

I(C(0x10), EbGb, 3, ADCb)
I(C(0x11), EvGv, 3, ADC)
I(C(0x12), GbEb, 1, ADCb)
I(C(0x13), GvEv, 1, ADC)
I(C(0x14), ALIb, 0, ADCb)
I(C(0x15), AXIv, 0, ADC)
I(C(0x16), _,    0, PUSH_SS)
I(C(0x17), _,    0, POP_SS)
I(C(0x18), EbGb, 3, SBBb)
I(C(0x19), EvGv, 3, SBB)
I(C(0x1a), GbEb, 1, SBBb)
I(C(0x1b), GvEv, 1, SBB)
I(C(0x1c), ALIb, 0, SBBb)
I(C(0x1d), AXIv, 0, SBB)
I(C(0x1e), _,    0, PUSH_DS)
I(C(0x1f), _,    0, POP_DS)

I(C(0x20), EbGb, 3, ANDb)
I(C(0x21), EvGv, 3, AND)
I(C(0x22), GbEb, 1, ANDb)
I(C(0x23), GvEv, 1, AND)
I(C(0x24), ALIb, 0, ANDb)
I(C(0x25), AXIv, 0, AND)
//  0x26   SEG ES
I(C(0x27), _,    0, DAA)
I(C(0x28), EbGb, 3, SUBb)
I(C(0x29), EvGv, 3, SUB)
I(C(0x2a), GbEb, 1, SUBb)
I(C(0x2b), GvEv, 1, SUB)
I(C(0x2c), ALIb, 0, SUBb)
I(C(0x2d), AXIv, 0, SUB)
//  0x2e   SEG CS
I(C(0x2f), _,    0, DAS)

I(C(0x30), EbGb, 3, XORb)
I(C(0x31), EvGv, 3, XOR)
I(C(0x32), GbEb, 1, XORb)
I(C(0x33), GvEv, 1, XOR)
I(C(0x34), ALIb, 0, XORb)
I(C(0x35), AXIv, 0, XOR)
//  0x36   SEG SS
I(C(0x37), _,    0, AAA)
I(C(0x38), EbGb, 1, CMPb)
I(C(0x39), EvGv, 1, CMP)
I(C(0x3a), GbEb, 1, CMPb)
I(C(0x3b), GvEv, 1, CMP)
I(C(0x3c), ALIb, 0, CMPb)
I(C(0x3d), AXIv, 0, CMP)
//  0x3e   SEG DS
I(C(0x3f), _,    0, AAS)

I(C(0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47), PlusRegv, 0, INC)
I(C(0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f), PlusRegv, 0, DEC)

I(C(0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57), PlusRegv, 0, PUSH)
I(C(0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f), PlusRegv, 0, POPReg)

I(C(0x60), _,      0, PUSHA)
I(C(0x61), _,      0, POPA)
I(C(0x62), GvMa,   0, BOUND)
I(C(0x63), PMEwGw,   0, ARPL)
//  0x64   SEG FS
//  0x65   SEG GS
//  0x66   Operand Size
//  0x67   Address Size
I(C(0x68), Iv,     0, PUSH)
I(C(0x69), GvEvIv, 1, IMUL2)
I(C(0x6a), Ib,     0, PUSHb)
I(C(0x6b), GvEvIb, 1, IMUL2)
I(C(0x6c), _,      0, INSb)
I(C(0x6d), _,      0, INS)
I(C(0x6e), _,      0, OUTSb)
I(C(0x6f), _,      0, OUTS)

I(C(0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
    0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f), Jb, 0, JCCb)

//  0x80   GRP1 EbIb
//  0x81   GRP1 EvIv
//  0x82   GRP1 EbIb (undocumented)
//  0x83   GRP1 EvIb
I(C(0x84), EbGb,   1, TESTb)
I(C(0x85), EvGv,   1, TEST)
I(C(0x86), EbGb,   3, XCHGb)
I(C(0x87), EvGv,   3, XCHG)
I(C(0x88), EbGb,   2, MOVb)
I(C(0x89), EvGv,   2, MOV)
I(C(0x8a), GbEb,   1, MOVb)
I(C(0x8b), GvEv,   1, MOV)
I(C(0x8c), EwSw,   2, MOVw)
I(C(0x8d), GvM,    0, LEA)
I(C(0x8e), SwEw,   1, MOVSeg)
I(C(0x8f), _,      0, POP)

I(C(0x90), _,      0, NOP)
I(C(0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97), _, 0, XCHGAX)
I(C(0x98), _,      0, CBW_CWDE)
I(C(0x99), _,      0, CWD_CDQ)
I(C(0x9a), Ap,     0, CALLFAR)
I(C(0x9b), _,      0, WAIT)
I(C(0x9c), _,      0, PUSHF)
I(C(0x9d), _,      0, POPF)
I(C(0x9e), _,      0, SAHF)
I(C(0x9f), _,      0, LAHF)

I(C(0xa0), ALOb,   1, MOVb)
I(C(0xa1), AXOv,   1, MOV)
I(C(0xa2), ObAL,   2, MOVb)
I(C(0xa3), OvAX,   2, MOV)
I(C(0xa4), _,      0, MOVSb)
I(C(0xa5), _,      0, MOVS)
I(C(0xa6), _,      0, CMPSb)
I(C(0xa7), _,      0, CMPS)
I(C(0xa8), ALIb,   0, TESTb)
I(C(0xa9), AXIv,   0, TEST)
I(C(0xaa), _,      0, STOSb)
I(C(0xab), _,      0, STOS)
I(C(0xac), _,      0, LODSb)
I(C(0xad), _,      0, LODS)
I(C(0xae), _,      0, SCASb)
I(C(0xaf), _,      0, SCAS)

I(C(0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7), PlusRegIb, 0, MOVb)
I(C(0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf), PlusRegIv, 0, MOV)

//  0xc0   GRP2 EbIb
//  0xc1   GRP2 EvIv
I(C(0xc2), Iw,     0, RETw)
I(C(0xc3), _,      0, RET)
I(C(0xc4), GvMp,   1, LES)
I(C(0xc5), GvMp,   1, LDS)
I(C(0xc6), EbIb,   2, MOVb)
I(C(0xc7), EvIv,   2, MOV)
I(C(0xc8), IwIb,   0, ENTER)
I(C(0xc9), _,   0, LEAVE)
I(C(0xca), Iw,     0, RETFARw)
I(C(0xcb), _,      0, RETFAR)
I(C(0xcc), _,      0, INT3)
I(C(0xcd), Ib,     0, INT)
I(C(0xce), _,      0, INTO)
I(C(0xcf), _,      0, IRET)

//  0xd0   GRP2 Eb1
//  0xd1   GRP2 Ev1
//  0xd2   GRP2 EbCL
//  0xd3   GRP2 EvCL
I(C(0xd4), Ib,     0, AAM)
I(C(0xd5), Ib,     0, AAD)
I(C(0xd6), _,      0, SALC) // undocumented
I(C(0xd7), _,      0, XLAT)
I(C(0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf), _, 0, ESC)

I(C(0xe0), Jb,     0, LOOPNEb)
I(C(0xe1), Jb,     0, LOOPEb)
I(C(0xe2), Jb,     0, LOOPb)
I(C(0xe3), Jb,     0, JCXZb)
I(C(0xe4), ALIb,   0, INb)
I(C(0xe5), AXIb,   0, IN)
I(C(0xe6), IbAL,   0, OUTb)
I(C(0xe7), IbAX,   0, OUT)
I(C(0xe8), Av,     0, CALL)
I(C(0xe9), Jv,     0, JMP)
I(C(0xea), Ap,     0, JMPFAR)
I(C(0xeb), Jb,     0, JMPb)
I(C(0xec), ALDX,   0, INb)
I(C(0xed), AXDX,   0, IN)
I(C(0xee), DXAL,   0, OUTb)
I(C(0xef), DXAX,   0, OUT)

//  0xf0   LOCK
//  0xf2   REPNE
//  0xf3   REP/REPE
I(C(0xf4), _,      0, HLT)
I(C(0xf5), _,      0, CMC)
//  0xf6   GRP3 Eb
//  0xf7   GRP3 Ev
I(C(0xf8), _,      0, CLC)
I(C(0xf9), _,      0, STC)
I(C(0xfa), _,      0, CLI)
I(C(0xfb), _,      0, STI)
I(C(0xfc), _,      0, CLD)
I(C(0xfd), _,      0, STD)
//  0xfe   GRP4
//  0xff   GRP5
#endif

#ifdef I2
//   0x00   GRP6
//   0x01   GRP7
I2(C(0x02), GvEw, 1, LAR)
I2(C(0x03), GvEw, 1, LSL)
I2(C(0x06), _, 0, CLTS)

I2(C(0x20), _, 0, MOVFC) // Move control register to r32
I2(C(0x21), _, 0, MOVFD) // Move debug register to r32
I2(C(0x22), _, 0, MOVTC) // Move r32 to control register
I2(C(0x23), _, 0, MOVTD) // Move r32 to debug register
I2(C(0x24), _, 0, MOVFT)
I2(C(0x26), _, 0, MOVTT)

I2(C(0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
     0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f), Jv, 0, JCC)
I2(C(0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97,
     0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f), Eb, 2, SETCCb)

I2(C(0xa0), _,      0, PUSH_FS)
I2(C(0xa1), _,      0, POP_FS)
I2(C(0xa3), BTEvGv, 1, BT)
I2(C(0xa4), EvGvIb, 3, SHLD)
I2(C(0xa5), EvGvCL, 3, SHLD)
I2(C(0xa8), _,      0, PUSH_GS)
I2(C(0xa9), _,      0, POP_GS)
I2(C(0xab), BTEvGv, 3, BTS)
I2(C(0xac), EvGvIb, 3, SHRD)
I2(C(0xad), EvGvCL, 3, SHRD)
I2(C(0xaf), GvEv,   1, IMUL2)

I2(C(0xb2), GvMp,   1, LSS)
I2(C(0xb3), BTEvGv, 3, BTR)
I2(C(0xb4), GvMp,   1, LFS)
I2(C(0xb5), GvMp,   1, LGS)
I2(C(0xb6), GvEb,   1, MOVZX)
I2(C(0xb7), GvEw,   1, MOVZX)
//   0xba   GRP8
I2(C(0xbb), BTEvGv, 3, BTC)
I2(C(0xbc), GvEv,   1, BSF)
I2(C(0xbd), GvEv,   1, BSR)
I2(C(0xbe), GvEb,   1, MOVSX)
I2(C(0xbf), GvEw,   1, MOVSX)
#endif

#ifdef IG1b
IG1b(C(0x00), EbIb, 3, ADDb)
IG1b(C(0x01), EbIb, 3, ORb)
IG1b(C(0x02), EbIb, 3, ADCb)
IG1b(C(0x03), EbIb, 3, SBBb)
IG1b(C(0x04), EbIb, 3, ANDb)
IG1b(C(0x05), EbIb, 3, SUBb)
IG1b(C(0x06), EbIb, 3, XORb)
IG1b(C(0x07), EbIb, 1, CMPb)
#endif

#ifdef IG1v
IG1v(C(0x00), EvIv, 3, ADD)
IG1v(C(0x01), EvIv, 3, OR)
IG1v(C(0x02), EvIv, 3, ADC)
IG1v(C(0x03), EvIv, 3, SBB)
IG1v(C(0x04), EvIv, 3, AND)
IG1v(C(0x05), EvIv, 3, SUB)
IG1v(C(0x06), EvIv, 3, XOR)
IG1v(C(0x07), EvIv, 1, CMP)
#endif

#ifdef IG1vIb
IG1vIb(C(0x00), EvIb, 3, ADD)
IG1vIb(C(0x01), EvIb, 3, OR)
IG1vIb(C(0x02), EvIb, 3, ADC)
IG1vIb(C(0x03), EvIb, 3, SBB)
IG1vIb(C(0x04), EvIb, 3, AND)
IG1vIb(C(0x05), EvIb, 3, SUB)
IG1vIb(C(0x06), EvIb, 3, XOR)
IG1vIb(C(0x07), EvIb, 1, CMP)
#endif

#ifdef IG2b
IG2b(C(0x00), EbIb, 3, ROLb)
IG2b(C(0x01), EbIb, 3, RORb)
IG2b(C(0x02), EbIb, 3, RCLb)
IG2b(C(0x03), EbIb, 3, RCRb)
IG2b(C(0x04, 0x06), EbIb, 3, SHLb)
IG2b(C(0x05), EbIb, 3, SHRb)
IG2b(C(0x07), EbIb, 3, SARb)
#endif

#ifdef IG2b1
IG2b1(C(0x00), Eb1, 3, ROLb)
IG2b1(C(0x01), Eb1, 3, RORb)
IG2b1(C(0x02), Eb1, 3, RCLb)
IG2b1(C(0x03), Eb1, 3, RCRb)
IG2b1(C(0x04, 0x06), Eb1, 3, SHLb)
IG2b1(C(0x05), Eb1, 3, SHRb)
IG2b1(C(0x07), Eb1, 3, SARb)
#endif

#ifdef IG2bC
IG2bC(C(0x00), EbCL, 3, ROLb)
IG2bC(C(0x01), EbCL, 3, RORb)
IG2bC(C(0x02), EbCL, 3, RCLb)
IG2bC(C(0x03), EbCL, 3, RCRb)
IG2bC(C(0x04, 0x06), EbCL, 3, SHLb)
IG2bC(C(0x05), EbCL, 3, SHRb)
IG2bC(C(0x07), EbCL, 3, SARb)
#endif

#ifdef IG2v
IG2v(C(0x00), EvIb, 3, ROL)
IG2v(C(0x01), EvIb, 3, ROR)
IG2v(C(0x02), EvIb, 3, RCL)
IG2v(C(0x03), EvIb, 3, RCR)
IG2v(C(0x04, 0x06), EvIb, 3, SHL)
IG2v(C(0x05), EvIb, 3, SHR)
IG2v(C(0x07), EvIb, 3, SAR)
#endif

#ifdef IG2v1
IG2v1(C(0x00), Ev1, 3, ROL)
IG2v1(C(0x01), Ev1, 3, ROR)
IG2v1(C(0x02), Ev1, 3, RCL)
IG2v1(C(0x03), Ev1, 3, RCR)
IG2v1(C(0x04, 0x06), Ev1, 3, SHL)
IG2v1(C(0x05), Ev1, 3, SHR)
IG2v1(C(0x07), Ev1, 3, SAR)
#endif

#ifdef IG2vC
IG2vC(C(0x00), EvCL, 3, ROL)
IG2vC(C(0x01), EvCL, 3, ROR)
IG2vC(C(0x02), EvCL, 3, RCL)
IG2vC(C(0x03), EvCL, 3, RCR)
IG2vC(C(0x04, 0x06), EvCL, 3, SHL)
IG2vC(C(0x05), EvCL, 3, SHR)
IG2vC(C(0x07), EvCL, 3, SAR)
#endif

#ifdef IG3b
IG3b(C(0x00, 0x01), EbIb, 1, TESTb)
IG3b(C(0x02), Eb, 3, NOTb)
IG3b(C(0x03), Eb, 3, NEGb)
IG3b(C(0x04), Eb, 1, MULb)
IG3b(C(0x05), Eb, 1, IMULb)
IG3b(C(0x06), Eb, 1, DIVb)
IG3b(C(0x07), Eb, 1, IDIVb)
#endif

#ifdef IG3v
IG3v(C(0x00, 0x01), EvIv, 1, TEST)
IG3v(C(0x02), Ev, 3, NOT)
IG3v(C(0x03), Ev, 3, NEG)
IG3v(C(0x04), Ev, 1, MUL)
IG3v(C(0x05), Ev, 1, IMUL)
IG3v(C(0x06), Ev, 1, DIV)
IG3v(C(0x07), Ev, 1, IDIV)
#endif

#ifdef IG4
IG4(C(0x00), Eb, 3, INCb)
IG4(C(0x01), Eb, 3, DECb)
#endif

#ifdef IG5
IG5(C(0x00), Ev, 3, INC)
IG5(C(0x01), Ev, 3, DEC)
IG5(C(0x02), Ev, 1, CALLABS)
IG5(C(0x03), Ep, 1, CALLFAR)
IG5(C(0x04), Ev, 1, JMPABS)
IG5(C(0x05), Ep, 1, JMPFAR)
IG5(C(0x06), Ev, 1, PUSH)
#endif

#ifdef IG6
IG6(C(0x00), Ew, 2, SLDT)
IG6(C(0x01), Ew, 2, STR)
IG6(C(0x02), Ew, 1, LLDT)
IG6(C(0x03), Ew, 1, LTR)
IG6(C(0x04), Ew, 1, VERR)
IG6(C(0x05), Ew, 1, VERW)
#endif

#ifdef IG7
IG7(C(0x00), Ms, 2, SGDT)
IG7(C(0x01), Ms, 2, SIDT)
IG7(C(0x02), Ms, 1, LGDT)
IG7(C(0x03), Ms, 1, LIDT)
IG7(C(0x04), Ew, 2, SMSW)
IG7(C(0x06), Ew, 1, LMSW)
#endif

#ifdef IG8
IG8(C(0x04), BTEvIb, 1, BT)
IG8(C(0x05), BTEvIb, 3, BTS)
IG8(C(0x06), BTEvIb, 3, BTR)
IG8(C(0x07), BTEvIb, 3, BTC)
#endif

#ifdef I2
I2(C(0xff), _,       0, UD0)
#endif

//486 (not in 386 ISA)
#ifdef I2
I2(C(0xb0), EbGb,    3, CMPXCHb)
I2(C(0xb1), EvGv,    3, CMPXCH)
I2(C(0xc0), EbGb,    3, XADDb)
I2(C(0xc1), EvGv,    3, XADD)
I2(C(0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf), PlusRegv, _, BSWAP)
I2(C(0x09), _,       0, WBINVD)
#endif
#ifdef IG7
IG7(C(0x07), Ms,     0, INVLPG)
#endif

//used by win98se
#ifdef I2
I2(C(0xa2), _,       0, CPUID) // CPUID
#endif

//used by winXP
#ifdef IG9
IG9(C(0x01), Mq,     0, CMPXCH8B)
#endif
#ifdef I2
I2(C(0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
    0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f), GvEv, 0, CMOV)
I2(C(0x31), _,       0, RDTSC)
#endif

#ifdef I2
I2(C(0x30), _,       0, WRMSR)
I2(C(0x32), _,       0, RDMSR)
I2(C(0x34), _,       0, SYSENTER)
I2(C(0x35), _,       0, SYSEXIT)
#endif

#if defined(I386_ENABLE_MMX) || defined(I386_ENABLE_SSE)
#define SIMD_i386ins_def
#include "simd.inc"
#undef SIMD_i386ins_def
#endif
