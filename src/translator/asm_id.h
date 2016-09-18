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
 * This contains the opcode and pseudo information
 * Each is assigned an ID. if ID < 256 it is a opcode otherwise pseudo op
 *
 * R. Sridhar
 */

#ifndef ASM_ID_H
#define ASM_ID_H

#include <cctype>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#define ASM_DS_MAX_LINE_LENGTH 1024
#define ASM_DS_MAX_IDENTIFIER_LENGTH 32
#define ASM_DS_MAX_OPCODE_LENGTH 5
#define ASM_DS_MAX_OPERAND 100
#define NUM_KEYWORDS 12

typedef struct
{
  int op_num;
  char op_str[ASM_DS_MAX_OPCODE_LENGTH];
  int num_args;
  char arg1[4];
  char arg2[4];

  int user_args; /* 0 - no arg, 1 - byte, 2 - word */
  char* op_desc; /* describes the instructions */

} IdOpcode;

typedef struct
{
  int op_num; /* >= 256 */
  char op_str[ASM_DS_MAX_OPCODE_LENGTH];

  int user_args; /* 0 - no arg, n args seperated by comma */
} IdPseudo;

typedef enum {
  ID_PSEUDO_DB = 500,
  ID_PSEUDO_DS,

  ID_PSEUDO_EQU = 300
} IdPseudoId;

IdPseudo* asm_id_pseudo_lookup(char*);
bool asm_existing_command(char*);
IdOpcode* asm_id_opcode_lookup(char*, char*, char*);
IdOpcode* asm_id_opcode_lookup_by_code(int);
bool asm_util_parse_number(char*, int*);
bool asm_util_str_to_int(char*, int*, int);
bool asm_id_keywords_lookup(char*);
int asm_command_size(int);

#endif // ASM_ID_H
