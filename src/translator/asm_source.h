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

/*
 * Assembler Input table
 * This is very imp. ds.
 * It represents source file in table form
 * The fields in this data structure will be descriptive
 *
 * R. Sridhar
 */
#ifndef ASM_SOURCE_H
#define ASM_SOURCE_H

#include "asm_id.h"
#include <QString>

struct IdMacro
{
  IdMacro(QString name)
    : name(name)
  {
  }
  QString name;
};

typedef struct
{
  /* generated during first scan */
  char s_id[ASM_DS_MAX_IDENTIFIER_LENGTH]; /* identifier in first column */
  int s_op; /* < 256 - opcode, otherwise pseudo op */
  IdOpcode* s_op_id_opcode;
  IdPseudo* s_op_id_pseudo;
  IdMacro* s_op_id_macro;
  int s_opnd_size;                 /* 0, 1, 2 */
  char s_opnd[ASM_DS_MAX_OPERAND]; /* = one of s_id (or) number */

  /* generated after final assembling - binary */
  unsigned int b_op;

  bool label_parsed;

  /* address in memory */
  int address;
  int line_number;
} AsmSourceEntry;

#endif // ASM_SOURCE_H
