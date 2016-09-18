/*
  Copyright (C) 2009, 2016  Artem Chistyakov <chistyakov.artem@gmail.com>
  Copyright (C) 2009  Ilya Beda <ir4y.ix@gmail.com>
  Copyright (C) 2009  Daniil Zapyatoy <zapper.kb@gmail.com>

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
#include "arithmetic.h"

struct stack
{
  int key;
  stack* next;
};

static void
push(stack*& top, int k)
{
  stack* p;
  p = new stack;
  p->key = k;
  p->next = top;
  top = p;
}

static void
pop(stack*& top, int& k)
{
  if (top != NULL) {
    stack* p;
    p = top;
    k = top->key;
    top = top->next;
    delete p;
  } else
    k = 0;
}

static int
prior(char k)
{
  switch (k) {
    case '(':
      return 0;
    case ')':
      return 1;
    case '+':
      return 2;
    case '-':
      return 2;
    case '*':
      return 3;
    case '/':
      return 3;
    // case '^' : return 4;
    case 'm':
      return 5;
    default:
      return 0;
  }
}

int
get_result(char* inf)
{
  stack *op = NULL, *st = NULL;
  int i, j = 0, t, a, b, c;
  char* pst;
  pst = new char[255];
  for (i = 0; i < strlen(inf); i++) {
    if (inf[i] == '(')
      push(op, '(');

    if (isdigit(inf[i]))
      pst[j++] = inf[i];

    if (inf[i] == '-' && (inf[i - 1] == '(' || i == 0)) {
      push(op, 'm');
      continue;
    }

    if (strchr(zn, inf[i])) {
      pop(op, t);
      while (prior(t) >= prior(inf[i])) {
        pst[j++] = ' ';
        pst[j++] = t;
        pop(op, t);
      }
      push(op, t);
      push(op, inf[i]);
      pst[j++] = ' ';
    }

    if (inf[i] == ')') {
      pop(op, t);
      while (t != '(') {
        pst[j++] = ' ';
        pst[j++] = t;
        pop(op, t);
      }
    }
  }

  while (op != NULL) {
    pop(op, t);
    pst[j++] = ' ';
    pst[j++] = t;
  }
  pst[j] = NULL;
  i = 0;

  while (pst[i] != 0x0) {
    if (strchr(zn, pst[i])) {
      pop(st, b);
      pop(st, a);
      switch (pst[i]) {
        case '+':
          c = a + b;
          break;
        case '-':
          c = a - b;
          break;
        case '*':
          c = a * b;
          break;
        case '/':
          c = a / b;
          break;
          // case '^' : c=pow(a,b);break;
      }
      push(st, c);
      i++;
    }

    if (pst[i] == 'm') {
      pop(st, a);
      c = -a;
      push(st, c);
    }
    if (isdigit(pst[i])) {
      c = 0;
      while (isdigit(pst[i]))
        c = c * 10 + (pst[i++] - '0');
      push(st, c);
    }
    if (pst[i] == ' ')
      i++;
  }
  pop(st, t);
  return t;
}
