/*
* Descent 3 
* Copyright (C) 2024 Parallax Software
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.

--- HISTORICAL COMMENTS FOLLOW ---

 * $Logfile: /DescentIII/Main/lib/d3x_op.h $
 * $Revision: 20 $
 * $Date: 4/03/98 5:15p $
 * $Author: Samir $
 *
 * D3X Opcodes.
 *
 * $Log: /DescentIII/Main/lib/d3x_op.h $
 *
 * 20    4/03/98 5:15p Samir
 * Added event parameters and upped registers.
 *
 * 19    3/26/98 12:18p Samir
 * Updated D3X version.
 *
 * 18    3/19/98 9:41p Samir
 * updated script file to now store length of script names.  this wasn't
 * happening before, and was a major bug that seemed to go unnoticed.
 *
 * 17    3/11/98 11:22a Samir
 * Added new integer opcodes.
 *
 * 16    2/10/98 10:44a Samir
 * Added gamemode script type.
 *
 * 15    2/05/98 2:44p Samir
 * Added dynamic strings.
 *
 * 14    1/22/98 6:22p Samir
 * Moved script parameter info to d3x.h.
 *
 * 13    1/20/98 4:12p Samir
 * New script housekeeping system.
 *
 * 12    1/19/98 3:48p Samir
 * Added script parameter support.
 *
 * 11    1/19/98 2:44p Samir
 * Use one script per object and started parameter passing support.
 *
 * 10    9/22/97 5:59p Samir
 * Added a name field to scripts, so we create threads using name instead
 * of number
 *
 * 9     9/10/97 2:01p Samir
 * Added level type for scripts.
 *
 * 8     9/08/97 4:28p Samir
 * Added script type object support, for specialized trigger/object
 * scripts.  Needed to add type to map file.
 *
 * 7     9/02/97 6:11p Samir
 * Added constant for number of arguments to a script.
 *
 * 6     8/21/97 5:55p Samir
 * Updated map file to store memory usage per script.
 *
 * 5     8/12/97 3:27p Samir
 * Renamed registers to VF (Vector-float)
 *
 * 4     8/05/97 6:42p Samir
 * D3X Script map structure.
 *
 * 3     8/05/97 10:47a Samir
 * Added endscript processing.
 *
 * 2     8/04/97 4:21p Samir
 * Finalized opcodes for D3X processor.
 *
 * $NoKeywords: $
 */

#ifndef OPCODES_H
#define OPCODES_H

/*	This is a list of macro-opcodes */

/*
        Memory operations
                LOAD				rx, (ABSOLUTE)
                LOAD				rx, (ADx+IMM)
                LOAD				rx, IMM
                STORE				rx, (ABSOLUTE)
                STORE				rx, (ADx+IMM)

        Math functions
                ADD				rx, rx
                ADDI				rx, rx
                SUB				rx, rx
                MUL				rx, rx
                DIV				rx, rx
                AND				rx, rx
                MOD				rx, rx
                BAND				rx, rx
                BANDI				rx, rx
                OR					rx, rx
                BOR				rx, rx
                BORI				rx, rx
                EQU				rx, rx
                NEQ				rx, rx
                LT					rx, rx
                LTE				rx, rx
                GT					rx, rx
                GTE				rx, rx
                NEG				rx
                NOT				rx
                ABS				rx

        Math vector functions (Vector element extraction)
                VEX				rx, vx
                VEY				rx, vx
                VEY				rx, vx
                XEV				vx, rx
                YEV				vx, rx
                ZEV				vx, rx
                SCALEV			vx, i/fx

        Execution Buffer
                EPUSH				rx
                EPOP				rx
                PCALL				FN
                CALL				ABSOLUTE

        Call Stack Data Operations
                CPUSH				rx
                CPOP				rx
                TOCSP				ADx					Transfer ADx reg to CSP reg
                FROMCSP			ADx					Transger CSP reg to ADx reg
                RETURN									grabs address from current CSP
   (return from CALL)

        BRANCHING

                JUMP				ABSOLUTE				Branch uncoditonally
                JUMPN				ABSOLUTE, rx		Jump if rx == 0
*/

/*
        registers
                ad0, ad1									Address registers
                i0, i1									Integer registers
                f0, f1									Float registers
                v0, v1									Vector registers
*/

#define OP_BREAK 0
#define OP_LOAD_ABS 1
#define OP_LOAD_ADI 2
#define OP_LOAD_IMM 3
#define OP_LOAD_PARM 4
#define OP_STORE_ABS 5
#define OP_STORE_ADI 6
#define OP_STORE_PARM 7
#define OP_ADD 10
#define OP_SUB 11
#define OP_MUL 12
#define OP_DIV 13
#define OP_MOD 14
#define OP_AND 15
#define OP_OR 16
#define OP_NEG 17
#define OP_NOT 18
#define OP_ABS 19
#define OP_EQU 20
#define OP_NEQ 21
#define OP_LT 22
#define OP_LTE 23
#define OP_GT 24
#define OP_GTE 25
#define OP_BOR 26
#define OP_BAND 27
#define OP_VEX 28
#define OP_VEY 29
#define OP_VEZ 30
#define OP_XEV 31
#define OP_YEV 32
#define OP_ZEV 33
#define OP_ADDI 34
#define OP_BANDI 35
#define OP_BORI 36
#define OP_EPUSH 40
#define OP_EPOP 41
#define OP_PCALL 42
#define OP_CALL 43
#define OP_CPUSH 44
#define OP_CPOP 45
#define OP_TOCSP 46
#define OP_FROMCSP 47
#define OP_RET 48
#define OP_DEFER 49
#define OP_JUMP_ABS 50
#define OP_JUMP_NCOND 51

#define VFREG_START 0
#define VFREG_NUM 16
#define REG_VF0 0
#define REG_VF1 1
#define REG_VF2 2
#define REG_VF3 3
#define REG_VF4 4
#define REG_VF5 5
#define REG_VF6 6
#define REG_VF7 7

/*	Address registers allow indirect addressing.
        Ad0 typically accesses local variables from the stack.
*/
#define AREG_START VFREG_START + VFREG_NUM
#define AREG_NUM 2
#define REG_AD0 0
#define REG_AD1 1

#define MAX_D3X_REGS 32

struct tD3XInstruction {
  uint8_t opc; /* Opcode */
  union {
    struct /* Register and or immediate */
    {
      uint8_t d;
      union {
        int i;
        float f;
      } imm;
    } ri;
    struct /* register <- Address-Immediate indirect addressing */
    {
      uint8_t d, a;
      uint16_t imm;
    } aii;
    struct /* jump absolute <- register  or jump/call absolute (r = 0xff) */
    {
      uint16_t abs;
      uint8_t r;
    } ra;
    struct /* register <- Address-Immediate indirect addressing */
    {
      uint8_t d, s;
    } rr;
  } opr; /* Operand depends on Opcode */
};

/*	Values used for defer opcode */
#define DEFER_END 0
#define DEFER_DEFAULT 1

/*	Values defined for type of object the 'me' or 'it' variable will be */
#define REF_OBJTYPE 0
#define REF_TRIGTYPE 1
#define REF_LEVELTYPE 2
#define REF_GAMEMODE 3

/*	parameter types for scripts */
#define PARMTYPE_NUMBER 0
#define PARMTYPE_VECTOR 1
#define PARMTYPE_REF 2
#define PARMTYPE_STRREF 3

/*	Offsets into call stack which contain the passed arguments to a script
        SCRARG_NUM = number of arguments of script (including event arguments)
*/
#define SCRARG_NUM 4
#define SCRARG_EVENT 0
#define SCRARG_ME 1
#define SCRARG_ITTYPE 2
#define SCRARG_IT 3
#define EVTARG_NUM 8
#define SCRSTACK_START SCRARG_NUM + EVTARG_NUM

/*	Program map entry */
#define MAX_D3XID_NAME 32

struct tD3XPMap {
  char name[MAX_D3XID_NAME];
  uint16_t ip;
  uint16_t mem;
  uint16_t type;
  uint16_t parms;
};

#define D3X_TAG "D3X5"

#endif
