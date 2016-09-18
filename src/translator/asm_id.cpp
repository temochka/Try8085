/*
  Copyright (C) 2009, 2016  Artem Chistyakov <chistyakov.artem@gmail.com>
  Copyright (C) 2009  Ilya Beda <ir4y.ix@gmail.com>
  Copyright (C) 2009  Daniil Zapyatoy <zapper.kb@gmail.com>
  Copyright (C) 2003  Sridhar Ratnakumar <sridhar.ratna@gmail.com>

  This file is part of Try8085.

  Try8085 is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Try8085 is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Try8085.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "asm_id.h"

static IdOpcode id_opcode[] = {
  /*
   * { 0x, "", 0, "X", "X" },
   */
  { 0xCE, "ACI", 0, "X", "X", 1,
    "Add immediate to accumulator with carry" } /* TODO - Add descriptions for
                                                   all the instructions */
  ,
  { 0x8F, "ADC", 1, "A", "X", 0, "Add register to accumulator with carry" },
  { 0x88, "ADC", 1, "B", "X", 0, "Add register to accumulator with carry" },
  { 0x89, "ADC", 1, "C", "X", 0, "Add register to accumulator with carry" },
  { 0x8A, "ADC", 1, "D", "X", 0, "Add register to accumulator with carry" },
  { 0x8B, "ADC", 1, "E", "X", 0, "Add register to accumulator with carry" },
  { 0x8C, "ADC", 1, "H", "X", 0, "Add register to accumulator with carry" },
  { 0x8D, "ADC", 1, "L", "X", 0, "Add register to accumulator with carry" },
  { 0x8E, "ADC", 1, "M", "X", 0, "Add register to accumulator with carry" },

  { 0x87, "ADD", 1, "A", "X", 0, "Add register or memory to accumulator" },
  { 0x80, "ADD", 1, "B", "X", 0, "Add register or memory to accumulator" },
  { 0x81, "ADD", 1, "C", "X", 0, "Add register or memory to accumulator" },
  { 0x82, "ADD", 1, "D", "X", 0, "Add register or memory to accumulator" },
  { 0x83, "ADD", 1, "E", "X", 0, "Add register or memory to accumulator" },
  { 0x84, "ADD", 1, "H", "X", 0, "Add register or memory to accumulator" },
  { 0x85, "ADD", 1, "L", "X", 0, "Add register or memory to accumulator" },
  { 0x86, "ADD", 1, "M", "X", 0, "Add register or memory to accumulator" },

  { 0xC6, "ADI", 0, "X", "X", 1, "Add immediate to accumulator" },

  { 0xA7, "ANA", 1, "A", "X", 0,
    "Logical AND register or memory with accumulator" },
  { 0xA0, "ANA", 1, "B", "X", 0,
    "Logical AND register or memory with accumulator" },
  { 0xA1, "ANA", 1, "C", "X", 0,
    "Logical AND register or memory with accumulator" },
  { 0xA2, "ANA", 1, "D", "X", 0,
    "Logical AND register or memory with accumulator" },
  { 0xA3, "ANA", 1, "E", "X", 0,
    "Logical AND register or memory with accumulator" },
  { 0xA4, "ANA", 1, "H", "X", 0,
    "Logical AND register or memory with accumulator" },
  { 0xA5, "ANA", 1, "L", "X", 0,
    "Logical AND register or memory with accumulator" },
  { 0xA6, "ANA", 1, "M", "X", 0,
    "Logical AND register or memory with accumulator" },

  { 0xE6, "ANI", 0, "X", "X", 1, "Logical AND immediate with accumulator" },

  { 0xCD, "CALL", 0, "X", "X", 2, "Subroutine Call" },
  { 0xDC, "CC", 0, "X", "X", 2, "Call on Carry" },
  { 0xFC, "CM", 0, "X", "X", 2, "Call on minus" },
  { 0x2F, "CMA", 0, "M", "X", 0, "Complement accumulator" },
  { 0x3F, "CMC", 0, "M", "X", 0, "Complement carry" },

  { 0xBF, "CMP", 1, "A", "X", 0,
    "Compare register or memory with accumulator" },
  { 0xB8, "CMP", 1, "B", "X", 0,
    "Compare register or memory with accumulator" },
  { 0xB9, "CMP", 1, "C", "X", 0,
    "Compare register or memory with accumulator" },
  { 0xBA, "CMP", 1, "D", "X", 0,
    "Compare register or memory with accumulator" },
  { 0xBB, "CMP", 1, "E", "X", 0,
    "Compare register or memory with accumulator" },
  { 0xBC, "CMP", 1, "H", "X", 0,
    "Compare register or memory with accumulator" },
  { 0xBD, "CMP", 1, "L", "X", 0,
    "Compare register or memory with accumulator" },
  { 0xBE, "CMP", 1, "M", "X", 0,
    "Compare register or memory with accumulator" },

  { 0xD4, "CNC", 0, "X", "X", 2, "Call on no Carry" },
  { 0xC4, "CNZ", 0, "X", "X", 2, "Call on no zero" },
  { 0xF4, "CP", 0, "X", "X", 2, "Call on positive" },
  { 0xEC, "CPE", 0, "X", "X", 2, "Call on parity even" },
  { 0xFE, "CPI", 0, "X", "X", 1, "Compare immediate with accumulator" },
  { 0xE4, "CPO", 0, "X", "X", 2, "Call on parity odd" },
  { 0xCC, "CZ", 0, "X", "X", 2, "Call on zero" },

  { 0x27, "DAA", 0, "M", "X", 0, "Decimal adjust accumulator" },

  { 0x09, "DAD", 1, "B", "X", 0, "Add register pair to H and L registers" },
  { 0x19, "DAD", 1, "D", "X", 0, "Add register pair to H and L registers" },
  { 0x29, "DAD", 1, "H", "X", 0, "Add register pair to H and L registers" },
  { 0x39, "DAD", 1, "SP", "X", 0, "Add register pair to H and L registers" },

  { 0x3D, "DCR", 1, "A", "X", 0, "Decrement register or memory by 1" },
  { 0x05, "DCR", 1, "B", "X", 0, "Decrement register or memory by 1" },
  { 0x0D, "DCR", 1, "C", "X", 0, "Decrement register or memory by 1" },
  { 0x15, "DCR", 1, "D", "X", 0, "Decrement register or memory by 1" },
  { 0x1D, "DCR", 1, "E", "X", 0, "Decrement register or memory by 1" },
  { 0x25, "DCR", 1, "H", "X", 0, "Decrement register or memory by 1" },
  { 0x2D, "DCR", 1, "L", "X", 0, "Decrement register or memory by 1" },
  { 0x35, "DCR", 1, "M", "X", 0, "Decrement register or memory by 1" },

  { 0x0B, "DCX", 1, "B", "X", 0, "Decrement register pair by 1" },
  { 0x1B, "DCX", 1, "D", "X", 0, "Decrement register pair by 1" },
  { 0x2B, "DCX", 1, "H", "X", 0, "Decrement register pair by 1" },
  { 0x3B, "DCX", 1, "SP", "X", 0, "Decrement register pair by 1" },

  { 0xF3, "DI", 0, "X", "X", 0, "Disable Interrupts" },
  { 0xFB, "EI", 0, "X", "X", 0, "Enable Interrupts" },
  { 0x76, "HLT", 0, "X", "X", 0, "Halt and enter wait state" },

  { 0xDB, "IN", 0, "X", "X", 1,
    "Input data to accumulator from a port with 8-bit address" },

  { 0x3C, "INR", 1, "A", "X", 0, "Increment register or memory by 1" },
  { 0x04, "INR", 1, "B", "X", 0, "Increment register or memory by 1" },
  { 0x0C, "INR", 1, "C", "X", 0, "Increment register or memory by 1" },
  { 0x14, "INR", 1, "D", "X", 0, "Increment register or memory by 1" },
  { 0x1C, "INR", 1, "E", "X", 0, "Increment register or memory by 1" },
  { 0x24, "INR", 1, "H", "X", 0, "Increment register or memory by 1" },
  { 0x2C, "INR", 1, "L", "X", 0, "Increment register or memory by 1" },
  { 0x34, "INR", 1, "M", "X", 0, "Increment register or memory by 1" },

  { 0x03, "INX", 1, "B", "X", 0, "Increment register pair by 1" },
  { 0x13, "INX", 1, "D", "X", 0, "Increment register pair by 1" },
  { 0x23, "INX", 1, "H", "X", 0, "Increment register pair by 1" },
  { 0x33, "INX", 1, "SP", "X", 0, "Increment register pair by 1" },

  { 0xDA, "JC", 0, "X", "X", 2, "Jump on Carry" },
  { 0xFA, "JM", 0, "X", "X", 2, "Jump on minus" },
  { 0xC3, "JMP", 0, "X", "X", 2, "Jump" },
  { 0xD2, "JNC", 0, "X", "X", 2, "Jump on no Carry" },
  { 0xC2, "JNZ", 0, "X", "X", 2, "Jump on no zero" },
  { 0xF2, "JP", 0, "X", "X", 2, "Jump on positive" },
  { 0xEA, "JPE", 0, "X", "X", 2, "Jump on parity even" },
  { 0xE2, "JPO", 0, "X", "X", 2, "Jump on parity odd" },
  { 0xCA, "JZ", 0, "X", "X", 2, "Jump on zero" },

  { 0x3A, "LDA", 0, "X", "X", 2, "Load accumulator" },

  { 0x0A, "LDAX", 1, "B", "X", 0, "Load accumulator indirect" },
  { 0x1A, "LDAX", 1, "D", "X", 0, "Load accumulator indirect" },

  { 0x2A, "LHLD", 0, "X", "X", 2, "Load H and L registers direct" },

  { 0x01, "LXI", 1, "B", "X", 2, "Load register pair immediate" },
  { 0x11, "LXI", 1, "D", "X", 2, "Load register pair immediate" },
  { 0x21, "LXI", 1, "H", "X", 2, "Load register pair immediate" },
  { 0x31, "LXI", 1, "SP", "X", 2, "Load register pair immediate" },

  { 0x7F, "MOV", 2, "A", "A", 0, "Copy from source to destination" },
  { 0x78, "MOV", 2, "A", "B", 0, "Copy from source to destination" },
  { 0x79, "MOV", 2, "A", "C", 0, "Copy from source to destination" },
  { 0x7A, "MOV", 2, "A", "D", 0, "Copy from source to destination" },
  { 0x7B, "MOV", 2, "A", "E", 0, "Copy from source to destination" },
  { 0x7C, "MOV", 2, "A", "H", 0, "Copy from source to destination" },
  { 0x7D, "MOV", 2, "A", "L", 0, "Copy from source to destination" },
  { 0x7E, "MOV", 2, "A", "M", 0, "Copy from source to destination" },

  { 0x47, "MOV", 2, "B", "A", 0, "Copy from source to destination" },
  { 0x40, "MOV", 2, "B", "B", 0, "Copy from source to destination" },
  { 0x41, "MOV", 2, "B", "C", 0, "Copy from source to destination" },
  { 0x42, "MOV", 2, "B", "D", 0, "Copy from source to destination" },
  { 0x43, "MOV", 2, "B", "E", 0, "Copy from source to destination" },
  { 0x44, "MOV", 2, "B", "H", 0, "Copy from source to destination" },
  { 0x45, "MOV", 2, "B", "L", 0, "Copy from source to destination" },
  { 0x46, "MOV", 2, "B", "M", 0, "Copy from source to destination" },

  { 0x4F, "MOV", 2, "C", "A", 0, "Copy from source to destination" },
  { 0x48, "MOV", 2, "C", "B", 0, "Copy from source to destination" },
  { 0x49, "MOV", 2, "C", "C", 0, "Copy from source to destination" },
  { 0x4A, "MOV", 2, "C", "D", 0, "Copy from source to destination" },
  { 0x4B, "MOV", 2, "C", "E", 0, "Copy from source to destination" },
  { 0x4C, "MOV", 2, "C", "H", 0, "Copy from source to destination" },
  { 0x4D, "MOV", 2, "C", "L", 0, "Copy from source to destination" },
  { 0x4E, "MOV", 2, "C", "M", 0, "Copy from source to destination" },

  { 0x57, "MOV", 2, "D", "A", 0, "Copy from source to destination" },
  { 0x50, "MOV", 2, "D", "B", 0, "Copy from source to destination" },
  { 0x51, "MOV", 2, "D", "C", 0, "Copy from source to destination" },
  { 0x52, "MOV", 2, "D", "D", 0, "Copy from source to destination" },
  { 0x53, "MOV", 2, "D", "E", 0, "Copy from source to destination" },
  { 0x54, "MOV", 2, "D", "H", 0, "Copy from source to destination" },
  { 0x55, "MOV", 2, "D", "L", 0, "Copy from source to destination" },
  { 0x56, "MOV", 2, "D", "M", 0, "Copy from source to destination" },

  { 0x5F, "MOV", 2, "E", "A", 0, "Copy from source to destination" },
  { 0x58, "MOV", 2, "E", "B", 0, "Copy from source to destination" },
  { 0x59, "MOV", 2, "E", "C", 0, "Copy from source to destination" },
  { 0x5A, "MOV", 2, "E", "D", 0, "Copy from source to destination" },
  { 0x5B, "MOV", 2, "E", "E", 0, "Copy from source to destination" },
  { 0x5C, "MOV", 2, "E", "H", 0, "Copy from source to destination" },
  { 0x5D, "MOV", 2, "E", "L", 0, "Copy from source to destination" },
  { 0x5E, "MOV", 2, "E", "M", 0, "Copy from source to destination" },

  { 0x67, "MOV", 2, "H", "A", 0, "Copy from source to destination" },
  { 0x60, "MOV", 2, "H", "B", 0, "Copy from source to destination" },
  { 0x61, "MOV", 2, "H", "C", 0, "Copy from source to destination" },
  { 0x62, "MOV", 2, "H", "D", 0, "Copy from source to destination" },
  { 0x63, "MOV", 2, "H", "E", 0, "Copy from source to destination" },
  { 0x64, "MOV", 2, "H", "H", 0, "Copy from source to destination" },
  { 0x65, "MOV", 2, "H", "L", 0, "Copy from source to destination" },
  { 0x66, "MOV", 2, "H", "M", 0, "Copy from source to destination" },

  { 0x6F, "MOV", 2, "L", "A", 0, "Copy from source to destination" },
  { 0x68, "MOV", 2, "L", "B", 0, "Copy from source to destination" },
  { 0x69, "MOV", 2, "L", "C", 0, "Copy from source to destination" },
  { 0x6A, "MOV", 2, "L", "D", 0, "Copy from source to destination" },
  { 0x6B, "MOV", 2, "L", "E", 0, "Copy from source to destination" },
  { 0x6C, "MOV", 2, "L", "H", 0, "Copy from source to destination" },
  { 0x6D, "MOV", 2, "L", "L", 0, "Copy from source to destination" },
  { 0x6E, "MOV", 2, "L", "M", 0, "Copy from source to destination" },

  { 0x77, "MOV", 2, "M", "A", 0, "Copy from source to destination" },
  { 0x70, "MOV", 2, "M", "B", 0, "Copy from source to destination" },
  { 0x71, "MOV", 2, "M", "C", 0, "Copy from source to destination" },
  { 0x72, "MOV", 2, "M", "D", 0, "Copy from source to destination" },
  { 0x73, "MOV", 2, "M", "E", 0, "Copy from source to destination" },
  { 0x74, "MOV", 2, "M", "H", 0, "Copy from source to destination" },
  { 0x75, "MOV", 2, "M", "L", 0, "Copy from source to destination" },

  { 0x3E, "MVI", 1, "A", "X", 1, "Move immediate 8-bit" },
  { 0x06, "MVI", 1, "B", "X", 1, "Move immediate 8-bit" },
  { 0x0E, "MVI", 1, "C", "X", 1, "Move immediate 8-bit" },
  { 0x16, "MVI", 1, "D", "X", 1, "Move immediate 8-bit" },
  { 0x1E, "MVI", 1, "E", "X", 1, "Move immediate 8-bit" },
  { 0x26, "MVI", 1, "H", "X", 1, "Move immediate 8-bit" },
  { 0x2E, "MVI", 1, "L", "X", 1, "Move immediate 8-bit" },
  { 0x36, "MVI", 1, "M", "X", 1, "Move immediate 8-bit" },

  { 0x00, "NOP", 0, "M", "X", 0, "No operation" },

  { 0xB7, "ORA", 1, "A", "X", 0,
    "Logical OR register or memory with accumulator" },
  { 0xB0, "ORA", 1, "B", "X", 0,
    "Logical OR register or memory with accumulator" },
  { 0xB1, "ORA", 1, "C", "X", 0,
    "Logical OR register or memory with accumulator" },
  { 0xB2, "ORA", 1, "D", "X", 0,
    "Logical OR register or memory with accumulator" },
  { 0xB3, "ORA", 1, "E", "X", 0,
    "Logical OR register or memory with accumulator" },
  { 0xB4, "ORA", 1, "H", "X", 0,
    "Logical OR register or memory with accumulator" },
  { 0xB5, "ORA", 1, "L", "X", 0,
    "Logical OR register or memory with accumulator" },
  { 0xB6, "ORA", 1, "M", "X", 0,
    "Logical OR register or memory with accumulator" },

  { 0xF6, "ORI", 0, "X", "X", 1, "Logical OR immediate with accumulator" },
  { 0xD3, "OUT", 0, "X", "X", 1,
    "Output data from accumulator to a port with 8-bit address" },

  { 0xE9, "PCHL", 0, "M", "X", 0, "Load program counter with HL contents" },

  { 0xC0, "RNZ", 0, "B", "X", 0, "Return on no zero" },
  { 0xC1, "POP", 1, "B", "X", 0, "Pop off stack to register pair" },
  { 0xD1, "POP", 1, "D", "X", 0, "Pop off stack to register pair" },
  { 0xE1, "POP", 1, "H", "X", 0, "Pop off stack to register pair" },
  { 0xF1, "POP", 1, "PSW", "X", 0, "Pop off stack to register pair" },

  { 0xC5, "PUSH", 1, "B", "X", 0, "Push register pair onto stack" },
  { 0xD5, "PUSH", 1, "D", "X", 0, "Push register pair onto stack" },
  { 0xE5, "PUSH", 1, "H", "X", 0, "Push register pair onto stack" },
  { 0xF5, "PUSH", 1, "PSW", "X", 0, "Push register pair onto stack" },

  { 0x17, "RAL", 0, "M", "X", 0, "Rotate accumulator left through carry" },
  { 0x1F, "RAR", 0, "M", "X", 0, "Rotate accumulator right through carry" },
  { 0xD8, "RC", 0, "M", "X", 0, "Return on Carry" },
  { 0xC9, "RET", 0, "M", "X", 0, "Return from subroutine" },
  { 0x20, "RIM", 0, "M", "X", 0, "Read interrupt mask" },
  { 0x07, "RLC", 0, "M", "X", 0, "Rotate accumulator left" },
  { 0xF8, "RM", 0, "M", "X", 0, "Return on minus" },
  { 0xD0, "RNC", 0, "M", "X", 0, "Return on no Carry" },
  { 0xF0, "RP", 0, "M", "X", 0, "Return on positive" },
  { 0xE8, "RPE", 0, "M", "X", 0, "Return on parity even" },
  { 0xE0, "RPO", 0, "M", "X", 0, "Return on parity odd" },
  { 0x0F, "RRC", 0, "M", "X", 0, "Rotate accumulator right" },

  { 0xC7, "RST", 1, "0", "X", 0, "Restart" },
  { 0xCF, "RST", 1, "1", "X", 0, "Restart" },
  { 0xD7, "RST", 1, "2", "X", 0, "Restart" },
  { 0xDF, "RST", 1, "3", "X", 0, "Restart" },
  { 0xE7, "RST", 1, "4", "X", 0, "Restart" },
  { 0xEF, "RST", 1, "5", "X", 0, "Restart" },
  { 0xF7, "RST", 1, "6", "X", 0, "Restart" },
  { 0xFF, "RST", 1, "7", "X", 0, "Restart" },

  { 0xC8, "RZ", 0, "X", "X", 0, "Return on zero" },

  { 0x9F, "SBB", 1, "A", "X", 0,
    "Subtract source and borrow from accumulator" },
  { 0x98, "SBB", 1, "B", "X", 0,
    "Subtract source and borrow from accumulator" },
  { 0x99, "SBB", 1, "C", "X", 0,
    "Subtract source and borrow from accumulator" },
  { 0x9A, "SBB", 1, "D", "X", 0,
    "Subtract source and borrow from accumulator" },
  { 0x9B, "SBB", 1, "E", "X", 0,
    "Subtract source and borrow from accumulator" },
  { 0x9C, "SBB", 1, "H", "X", 0,
    "Subtract source and borrow from accumulator" },
  { 0x9D, "SBB", 1, "L", "X", 0,
    "Subtract source and borrow from accumulator" },
  { 0x9E, "SBB", 1, "M", "X", 0,
    "Subtract source and borrow from accumulator" },

  { 0xDE, "SBI", 0, "X", "X", 1,
    "Subtract immediate from accumulator with borrow" },
  { 0x22, "SHLD", 0, "X", "X", 2, "Store H and L registers direct" },

  { 0x30, "SIM", 0, "M", "X", 0, "Set interrupt mask" },
  { 0xF9, "SPHL", 0, "M", "X", 0,
    "Copy H and L registers to the stack pointer" },

  { 0x32, "STA", 0, "X", "X", 2, "Store accumulator direct" },
  { 0x02, "STAX", 1, "B", "X", 0, "Store accumulator indirect (RegPair)" },
  { 0x12, "STAX", 1, "D", "X", 0, "Store accumulator indirect (RegPair)" },

  { 0x37, "STC", 0, "M", "X", 0, "Set Carry" }, // modified

  { 0x97, "SUB", 1, "A", "X", 0,
    "Subtract register or memory from accumulator" },
  { 0x90, "SUB", 1, "B", "X", 0,
    "Subtract register or memory from accumulator" },
  { 0x91, "SUB", 1, "C", "X", 0,
    "Subtract register or memory from accumulator" },
  { 0x92, "SUB", 1, "D", "X", 0,
    "Subtract register or memory from accumulator" },
  { 0x93, "SUB", 1, "E", "X", 0,
    "Subtract register or memory from accumulator" },
  { 0x94, "SUB", 1, "H", "X", 0,
    "Subtract register or memory from accumulator" },
  { 0x95, "SUB", 1, "L", "X", 0,
    "Subtract register or memory from accumulator" },
  { 0x96, "SUB", 1, "M", "X", 0,
    "Subtract register or memory from accumulator" },

  { 0xD6, "SUI", 0, "X", "X", 1, "Subtract immediate from accumulator" },

  { 0xEB, "XCHG", 0, "M", "X", 0, "Exchange H and L with D and E" },

  { 0xAF, "XRA", 1, "A", "X", 0,
    "Exclusive OR register or memory with accumulator" },
  { 0xA8, "XRA", 1, "B", "X", 0,
    "Exclusive OR register or memory with accumulator" },
  { 0xA9, "XRA", 1, "C", "X", 0,
    "Exclusive OR register or memory with accumulator" },
  { 0xAA, "XRA", 1, "D", "X", 0,
    "Exclusive OR register or memory with accumulator" },
  { 0xAB, "XRA", 1, "E", "X", 0,
    "Exclusive OR register or memory with accumulator" },
  { 0xAC, "XRA", 1, "H", "X", 0,
    "Exclusive OR register or memory with accumulator" },
  { 0xAD, "XRA", 1, "L", "X", 0,
    "Exclusive OR register or memory with accumulator" },
  { 0xAE, "XRA", 1, "M", "X", 0,
    "Exclusive OR register or memory with accumulator" },

  { 0xEE, "XRI", 0, "X", "X", 1, "Exclusive OR immediate with accumulator" },
  { 0xE3, "XTHL", 0, "M", "X", 0, "Exchange H and L with top of stack" }
};

static IdPseudo id_pseudo[] = {
  { ID_PSEUDO_DB, "DB", 1 },
};

static char* reserved_keywords[] = { "A", "B",  "C",  "D",   "E",  "H",
                                     "L", "SP", "PC", "PSW", "IP", "DB" };

IdPseudo*
asm_id_pseudo_lookup(char* op_name)
{
  int i;

  for (i = 0; i < sizeof(id_pseudo) / sizeof(id_pseudo[0]); i++) {
    if (strcmp(op_name, id_pseudo[i].op_str))
      continue;

    return &id_pseudo[i];
  }

  return NULL;
}

bool
asm_existing_command(char* cCmd)
{
  if (!asm_id_opcode_lookup(cCmd, NULL, NULL)) {
    if (!asm_id_pseudo_lookup(cCmd)) {
      return false;
    }
  }
  return true;
}

IdOpcode*
asm_id_opcode_lookup(char* op_name, char* arg1, char* arg2)
{
  int i, num_args;
  bool goodArg;

  for (i = 0; i < sizeof(id_opcode) / sizeof(id_opcode[0]); i++) {
    num_args = id_opcode[i].num_args + (id_opcode[i].user_args > 0);
    if (strcmp(op_name, id_opcode[i].op_str))
      continue;

    if (arg1) {
      goodArg = !strcmp(arg1, id_opcode[i].arg1) ? true : false;
      if (!goodArg &&
          !(num_args == 1 && id_opcode[i].user_args &&
            !strcmp(id_opcode[i].arg1, "X"))) {
        continue;
      }
    }

    if (arg2) {
      goodArg = !strcmp(arg2, id_opcode[i].arg2) ? true : false;
      if (!goodArg &&
          !(id_opcode[i].user_args && !strcmp(id_opcode[i].arg2, "X"))) {
        continue;
      }
    }

    return &id_opcode[i];
  }

  return NULL;
}

IdOpcode*
asm_id_opcode_lookup_by_code(int code)
{
  for (int i = 0; i < sizeof(id_opcode) / sizeof(id_opcode[0]); i++) {
    if (id_opcode[i].op_num == code) {
      return &id_opcode[i];
    }
  }
  return NULL;
}

bool
asm_util_parse_number(char* str, int* value)
{
  if (!isdigit(str[0]))
    return false;

  if (str[strlen(str) - 1] == 'h' || str[strlen(str) - 1] == 'H') {
    str[strlen(str) - 1] = '\0';

    return asm_util_str_to_int(str, value, 16);
  }
  if (str[strlen(str) - 1] == 'b' || str[strlen(str) - 1] == 'B') {
    str[strlen(str) - 1] = '\0';

    return asm_util_str_to_int(str, value, 2);
  }
  if (str[strlen(str) - 1] == 'o' || str[strlen(str) - 1] == 'O') {
    str[strlen(str) - 1] = '\0';

    return asm_util_str_to_int(str, value, 8);
  }

  return asm_util_str_to_int(str, value, 10);
}

bool
asm_util_str_to_int(char* str, int* value, int base)
{
  char* err = "";

  *value = strtol(str, &err, base);

  if (strlen(err))
    return false;

  return true;
}

bool
asm_id_keywords_lookup(char* as)
{
  if (asm_id_opcode_lookup(as, NULL, NULL)) {
    return true;
  }

  for (int i = 0; i < NUM_KEYWORDS; i++) {
    if (!strcmp(reserved_keywords[i], as)) {
      return true;
    }
  }
  return false;
}

int
asm_command_size(int op)
{
  switch (op) {
    case 0x0:
      return 1;
    case 0x1:
      return 3;
    case 0x2:
      return 1;
    case 0x3:
      return 1;
    case 0x4:
      return 1;
    case 0x5:
      return 1;
    case 0x6:
      return 2;
    case 0x7:
      return 1;
    case 0x8:
      return 1;
    case 0x9:
      return 1;
    case 0xA:
      return 1;
    case 0xB:
      return 1;
    case 0xC:
      return 1;
    case 0xD:
      return 1;
    case 0xE:
      return 2;
    case 0xF:
      return 1;
    case 0x10:
      return 1;
    case 0x11:
      return 3;
    case 0x12:
      return 1;
    case 0x13:
      return 1;
    case 0x14:
      return 1;
    case 0x15:
      return 1;
    case 0x16:
      return 2;
    case 0x17:
      return 1;
    case 0x18:
      return 1;
    case 0x19:
      return 1;
    case 0x1A:
      return 1;
    case 0x1B:
      return 1;
    case 0x1C:
      return 1;
    case 0x1D:
      return 1;
    case 0x1E:
      return 2;
    case 0x1F:
      return 1;
    case 0x20:
      return 1;
    case 0x21:
      return 3;
    case 0x22:
      return 3;
    case 0x23:
      return 1;
    case 0x24:
      return 1;
    case 0x25:
      return 1;
    case 0x26:
      return 2;
    case 0x27:
      return 1;
    case 0x28:
      return 1;
    case 0x29:
      return 1;
    case 0x2A:
      return 3;
    case 0x2B:
      return 1;
    case 0x2C:
      return 1;
    case 0x2D:
      return 1;
    case 0x2E:
      return 2;
    case 0x2F:
      return 1;
    case 0x30:
      return 1;
    case 0x31:
      return 3;
    case 0x32:
      return 3;
    case 0x33:
      return 1;
    case 0x34:
      return 1;
    case 0x35:
      return 1;
    case 0x36:
      return 2;
    case 0x37:
      return 1;
    case 0x38:
      return 1;
    case 0x39:
      return 1;
    case 0x3A:
      return 3;
    case 0x3B:
      return 1;
    case 0x3C:
      return 1;
    case 0x3D:
      return 1;
    case 0x3E:
      return 2;
    case 0x3F:
      return 1;
    case 0x40:
      return 1;
    case 0x41:
      return 1;
    case 0x42:
      return 1;
    case 0x43:
      return 1;
    case 0x44:
      return 1;
    case 0x45:
      return 1;
    case 0x46:
      return 1;
    case 0x47:
      return 1;
    case 0x48:
      return 1;
    case 0x49:
      return 1;
    case 0x4A:
      return 1;
    case 0x4B:
      return 1;
    case 0x4C:
      return 1;
    case 0x4D:
      return 1;
    case 0x4E:
      return 1;
    case 0x4F:
      return 1;
    case 0x50:
      return 1;
    case 0x51:
      return 1;
    case 0x52:
      return 1;
    case 0x53:
      return 1;
    case 0x54:
      return 1;
    case 0x55:
      return 1;
    case 0x56:
      return 1;
    case 0x57:
      return 1;
    case 0x58:
      return 1;
    case 0x59:
      return 1;
    case 0x5A:
      return 1;
    case 0x5B:
      return 1;
    case 0x5C:
      return 1;
    case 0x5D:
      return 1;
    case 0x5E:
      return 1;
    case 0x5F:
      return 1;
    case 0x60:
      return 1;
    case 0x61:
      return 1;
    case 0x62:
      return 1;
    case 0x63:
      return 1;
    case 0x64:
      return 1;
    case 0x65:
      return 1;
    case 0x66:
      return 1;
    case 0x67:
      return 1;
    case 0x68:
      return 1;
    case 0x69:
      return 1;
    case 0x6A:
      return 1;
    case 0x6B:
      return 1;
    case 0x6C:
      return 1;
    case 0x6D:
      return 1;
    case 0x6E:
      return 1;
    case 0x6F:
      return 1;
    case 0x70:
      return 1;
    case 0x71:
      return 1;
    case 0x72:
      return 1;
    case 0x73:
      return 1;
    case 0x74:
      return 1;
    case 0x75:
      return 1;
    case 0x76:
      return 1;
    case 0x77:
      return 1;
    case 0x78:
      return 1;
    case 0x79:
      return 1;
    case 0x7A:
      return 1;
    case 0x7B:
      return 1;
    case 0x7C:
      return 1;
    case 0x7D:
      return 1;
    case 0x7E:
      return 1;
    case 0x7F:
      return 1;
    case 0x80:
      return 1;
    case 0x81:
      return 1;
    case 0x82:
      return 1;
    case 0x83:
      return 1;
    case 0x84:
      return 1;
    case 0x85:
      return 1;
    case 0x86:
      return 1;
    case 0x87:
      return 1;
    case 0x88:
      return 1;
    case 0x89:
      return 1;
    case 0x8A:
      return 1;
    case 0x8B:
      return 1;
    case 0x8C:
      return 1;
    case 0x8D:
      return 1;
    case 0x8E:
      return 1;
    case 0x8F:
      return 1;
    case 0x90:
      return 1;
    case 0x91:
      return 1;
    case 0x92:
      return 1;
    case 0x93:
      return 1;
    case 0x94:
      return 1;
    case 0x95:
      return 1;
    case 0x96:
      return 1;
    case 0x97:
      return 1;
    case 0x98:
      return 1;
    case 0x99:
      return 1;
    case 0x9A:
      return 1;
    case 0x9B:
      return 1;
    case 0x9C:
      return 1;
    case 0x9D:
      return 1;
    case 0x9E:
      return 1;
    case 0x9F:
      return 1;
    case 0xA0:
      return 1;
    case 0xA1:
      return 1;
    case 0xA2:
      return 1;
    case 0xA3:
      return 1;
    case 0xA4:
      return 1;
    case 0xA5:
      return 1;
    case 0xA6:
      return 1;
    case 0xA7:
      return 1;
    case 0xA8:
      return 1;
    case 0xA9:
      return 1;
    case 0xAA:
      return 1;
    case 0xAB:
      return 1;
    case 0xAC:
      return 1;
    case 0xAD:
      return 1;
    case 0xAE:
      return 1;
    case 0xAF:
      return 1;
    case 0xB0:
      return 1;
    case 0xB1:
      return 1;
    case 0xB2:
      return 1;
    case 0xB3:
      return 1;
    case 0xB4:
      return 1;
    case 0xB5:
      return 1;
    case 0xB6:
      return 1;
    case 0xB7:
      return 1;
    case 0xB8:
      return 1;
    case 0xB9:
      return 1;
    case 0xBA:
      return 1;
    case 0xBB:
      return 1;
    case 0xBC:
      return 1;
    case 0xBD:
      return 1;
    case 0xBE:
      return 1;
    case 0xBF:
      return 1;
    case 0xC0:
      return 1;
    case 0xC1:
      return 1;
    case 0xC2:
      return 3;
    case 0xC3:
      return 3;
    case 0xC4:
      return 3;
    case 0xC5:
      return 1;
    case 0xC6:
      return 2;
    case 0xC7:
      return 1;
    case 0xC8:
      return 1;
    case 0xC9:
      return 1;
    case 0xCA:
      return 3;
    case 0xCB:
      return 1;
    case 0xCC:
      return 3;
    case 0xCD:
      return 3;
    case 0xCE:
      return 2;
    case 0xCF:
      return 1;
    case 0xD0:
      return 1;
    case 0xD1:
      return 1;
    case 0xD2:
      return 3;
    case 0xD3:
      return 2;
    case 0xD4:
      return 3;
    case 0xD5:
      return 1;
    case 0xD6:
      return 2;
    case 0xD7:
      return 1;
    case 0xD8:
      return 1;
    case 0xD9:
      return 1;
    case 0xDA:
      return 3;
    case 0xDB:
      return 2;
    case 0xDC:
      return 3;
    case 0xDD:
      return 1;
    case 0xDE:
      return 2;
    case 0xDF:
      return 1;
    case 0xE0:
      return 1;
    case 0xE1:
      return 1;
    case 0xE2:
      return 3;
    case 0xE3:
      return 1;
    case 0xE4:
      return 3;
    case 0xE5:
      return 1;
    case 0xE6:
      return 2;
    case 0xE7:
      return 1;
    case 0xE8:
      return 1;
    case 0xE9:
      return 1;
    case 0xEA:
      return 3;
    case 0xEB:
      return 1;
    case 0xEC:
      return 3;
    case 0xED:
      return 1;
    case 0xEE:
      return 2;
    case 0xEF:
      return 1;
    case 0xF0:
      return 1;
    case 0xF1:
      return 1;
    case 0xF2:
      return 3;
    case 0xF3:
      return 1;
    case 0xF4:
      return 3;
    case 0xF5:
      return 1;
    case 0xF6:
      return 2;
    case 0xF7:
      return 1;
    case 0xF8:
      return 1;
    case 0xF9:
      return 1;
    case 0xFA:
      return 3;
    case 0xFB:
      return 1;
    case 0xFC:
      return 3;
    case 0xFD:
      return 1;
    case 0xFE:
      return 2;
    case 0xFF:
      return 1;
    default:
      return -1;
  }
}
