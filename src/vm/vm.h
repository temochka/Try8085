/*
  Copyright (C) 2009, 2016  Artem Chistyakov <chistyakov.artem@gmail.com>
  Copyright (C) 2009  Daniil Zapyatoy <zapper.kb@gmail.com>
  Copyright (C) 2008, 2009  Ilya Beda <ir4y.ix@gmail.com>

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
#ifndef TRY8085_VM_H
#define TRY8085_VM_H

#define MEMSIZE 0xffff
#define INTSIZE 10
#define PORTSIZE 0xff
#define Default_SP 0xf000
#define Default_IP 0x0000
#include <QSharedMemory>
#include <exception>

class VMException : public std::exception
{
  const char* what() const throw() { return "Try8085 VM generic exception"; }
};

class VMMemoryException : public VMException
{
  const char* what() const throw() { return "Try8085 VM memory exception"; }
};

struct RGFLAG
{
  unsigned char C, P, Z, S, I;
};

class VM
{
public:
  unsigned char A, B, C, D, E, H, L;
  RGFLAG flag;
  unsigned short IP;
  unsigned short SP;
  unsigned char* memory;
  unsigned char* interupts;
  unsigned char* port;
  VM();
  ~VM();
  int exec();
  void stop();
  void free_bpx();
  void add_bpx(unsigned short);
  int del_bpx(unsigned short);
  int execflag;
  void clear_flags();
  void clear_registers();
  void set_memory(unsigned char* m);
  void get_memory(unsigned char* m);

private:
  int ret_flag;
  int my_timer_count;
  void set_flags(int);

  QSharedMemory sharedMemory;
  QSharedMemory sharedPorts;
  QSharedMemory sharedInterrupts;

  struct bp
  {
    unsigned short addr;
    bp* next;
    int in_use;
  } * bpx;
};

#endif // TRY8085_VM_H
