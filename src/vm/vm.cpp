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
#include "vm.h"

int
VM::del_bpx(unsigned short addr)
{
  bp* current;
  current = bpx;
  if (current->addr == addr) {
    bpx = current->next;
    delete current;
    return 0;
  }

  while (current->next != NULL) {
    if (current->next->addr == addr) {
      bp* var;
      var = current->next;
      current->next = current->next->next;
      delete var;
      return 0;
    }
    current = current->next;
  }
  return -1;
}

void
VM::add_bpx(unsigned short addr)
{
  bp* current;
  if (bpx == NULL) {
    bpx = new bp;
    bpx->addr = addr;
    bpx->in_use = 0;
    bpx->next = NULL;
  } else {
    current = bpx;
    while (current != NULL)
      current = current->next;
    current = new bp;
    current->addr = addr;
    current->in_use = 0;
    current->next = NULL;
  }
}

void
VM::free_bpx()
{
  bp* current;
  current = bpx;
  while (current != NULL) {
    bpx = current;
    current = current->next;
    delete bpx;
  }
  bpx = NULL;
}

void
VM::set_flags(int number)
{
  if ((number & 0x100) == 0x100)
    flag.C = 1; // check xxxxxxx1xxxxxxxx
  else
    flag.C = 0;
  number &= 0xFF;

  if (number < 0x80)
    flag.S = 0;
  else
    flag.S = 1;

  if (number == 0)
    flag.Z = 1;
  else
    flag.Z = 0;

  flag.P = 1 - number % 2;
}

void
VM::clear_flags()
{
  this->flag.C = 0;
  this->flag.I = 0;
  this->flag.P = 0;
  this->flag.S = 0;
  this->flag.Z = 0;
  this->execflag = 0;
  this->bpx = NULL;
  this->ret_flag = 0;
  this->my_timer_count = -1;
}

void
VM::clear_registers()
{
  this->A = 0;
  this->B = 0;
  this->C = 0;
  this->D = 0;
  this->E = 0;
  this->H = 0;
  this->L = 0;
  this->IP = Default_IP;
  this->SP = Default_SP;
  this->sharedInterrupts.lock();
  memset(this->interupts, 0, INTSIZE);
  this->sharedInterrupts.unlock();

  this->sharedPorts.lock();
  memset(this->port, 0, PORTSIZE);
  this->sharedPorts.unlock();
}

VM::VM()
{
  sharedMemory.setKey("try8085_memory");
  sharedMemory.create(MEMSIZE, QSharedMemory::ReadWrite);
  sharedMemory.attach();
  sharedMemory.lock();
  this->memory = (unsigned char*)this->sharedMemory.data();
  sharedMemory.unlock();

  if (memory == NULL) {
    throw VMMemoryException();
  }

  sharedMemory.lock();
  memset(this->memory, 0, MEMSIZE);
  sharedMemory.unlock();

  this->sharedPorts.setKey("try8085_ports");
  sharedPorts.create(PORTSIZE, QSharedMemory::ReadWrite);
  this->sharedPorts.attach();
  this->sharedPorts.lock();
  this->port = (unsigned char*)this->sharedPorts.data();
  this->sharedPorts.unlock();
  if (port == NULL) {
    throw VMMemoryException();
  }

  this->sharedPorts.lock();
  memset(this->port, 0, PORTSIZE);
  this->sharedPorts.unlock();

  this->sharedInterrupts.setKey("try8085_interrupts");
  this->sharedInterrupts.create(INTSIZE, QSharedMemory::ReadWrite);

  this->sharedInterrupts.attach();
  this->sharedInterrupts.lock();
  this->interupts = (unsigned char*)this->sharedInterrupts.data();
  this->sharedInterrupts.unlock();
  if (interupts == NULL) {
    throw VMMemoryException();
  }

  this->sharedInterrupts.lock();
  memset(this->interupts, 0, INTSIZE);
  this->sharedInterrupts.unlock();

  this->clear_flags();
  this->clear_registers();
}

void
VM::stop()
{
  return;
  int index = 0x0fff;
  unsigned short old_IP;
  char buf;
  while (execflag)
    ;
  execflag = 1;
  buf = memory[index];
  memory[index] = 0x76;
  old_IP = IP;
  IP = index;
  execflag = 0;
  while (execflag)
    ;
  IP = old_IP;
  memory[index] = buf;
}

VM::~VM()
{
  if (this->sharedInterrupts.isAttached()) {
    this->sharedInterrupts.detach();
  }
  if (this->sharedMemory.isAttached()) {
    this->sharedMemory.detach();
  }
  if (this->sharedPorts.isAttached()) {
    this->sharedPorts.detach();
  }
}

int
VM::exec()
{
  while (execflag)
    ;
  execflag = 1;

  this->sharedInterrupts.lock();
  this->sharedMemory.lock();
  if (interupts[0] == 1 && flag.I) {
    interupts[0] = 0;
    SP -= 2;
    *((unsigned short*)(memory + SP)) = IP;
    IP = 0x002c;
  } else if (interupts[1] == 1 && flag.I) {
    interupts[1] = 0;
    SP -= 2;
    *((unsigned short*)(memory + SP)) = IP;
    IP = 0x0034;
  } else if (interupts[2] == 1 && flag.I) {
    interupts[2] = 0;
    SP -= 2;
    *((unsigned short*)(memory + SP)) = IP;
    IP = 0x003c;
  } else if (interupts[3] && flag.I) {
    SP -= 2;
    *((unsigned short*)(memory + SP)) = IP;
    IP = *((unsigned short*)(interupts + 3));
    interupts[3] = 0;
    interupts[4] = 0;
  }
  this->sharedMemory.unlock();
  this->sharedInterrupts.unlock();

  this->sharedPorts.lock();
  if ((my_timer_count - 1) == *((unsigned short*)(port + 0x90)) - 1) {
    interupts[0] = 1;
    my_timer_count = -1;
  }
  this->sharedPorts.unlock();

  if (flag.I)
    my_timer_count++;

  if (ret_flag) {
    ret_flag = 0;
    flag.I = 1;
  }

  unsigned short ptr;
  void* buf;
  unsigned char _flag;
  int retVal;
  this->sharedMemory.lock();
  this->sharedPorts.lock();
  switch (memory[IP]) {
    // nop
    case 0x00:
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // lxi b,&
    case 0x01:
      B = memory[IP + 2];
      C = memory[IP + 1];
      IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    // stax b
    case 0x02:
      buf = &ptr;
      *(char*)buf = C;
      *((char*)buf + 1) = B;
      memory[ptr] = A;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // inx b
    case 0x03:
      if (!++C)
        B++;

      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // inr b
    case 0x04:
      if (!++B)
        flag.C = 1;
      else
        flag.C = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;

    // dcr b
    case 0x05:
      if (B-- == 0)
        flag.C = 1;
      else
        flag.C = 0;

      if (!B)
        flag.Z = 1;
      else
        flag.Z = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mvi b
    case 0x06:
      B = memory[IP + 1];
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // rlc
    case 0x07:
      flag.C = A & 0x80;
      flag.C >>= 7;
      A <<= 1;
      if (flag.C)
        A |= 0x01;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    //**no command*/
    case 0x08:
      execflag = 0;
      retVal = 0;
      break;
    // dad b
    case 0x09:
      break;

    // ldax b
    case 0x0A:
      buf = &ptr;
      *(char*)buf = C;
      *((char*)buf + 1) = B;
      A = memory[ptr];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // dcx B
    case 0x0B:
      if (!--C)
        B--;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // inr c
    case 0x0C:
      if (!++C)
        flag.C = 1;
      else
        flag.C = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // dcr c
    case 0x0D:
      if (C-- == 0)
        flag.C = 1;
      else
        flag.C = 0;

      if (!C)
        flag.Z = 1;
      else
        flag.Z = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mvi c
    case 0x0E:
      C = memory[IP + 1];
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // rrc
    case 0x0F:
      flag.C = A & 0x01;
      A >>= 1;
      if (flag.C)
        A |= 0x80;
      IP++;
      retVal = 1;
      break;
    //*****no such command!!!******
    case 0x10:
      execflag = 0;
      retVal = 0;
      break;
    // lxi d
    case 0x11:
      D = memory[IP + 2];
      E = memory[IP + 1];
      IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    // stax d
    case 0x12:
      buf = &ptr;
      *(char*)buf = E;
      *((char*)buf + 1) = D;
      memory[ptr] = A;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // inx d
    case 0x13:
      if (!++E)
        D++;
      IP++;
      execflag = 0;
      retVal = 1;
      break;

    // inr d
    case 0x14:
      if (!++D)
        flag.C = 1;
      else
        flag.C = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // dcr d
    case 0x15:
      if (D-- == 0)
        flag.C = 1;
      else
        flag.C = 0;

      if (!D)
        flag.Z = 1;
      else
        flag.Z = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mvi d
    case 0x16:
      D = memory[IP + 1];
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // ral
    case 0x17:
      _flag = flag.C;
      flag.C = A & 0x80;
      flag.C >>= 7;
      A <<= 1;
      if (_flag)
        A |= 0x01;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    //*****no such command!!!*****
    case 0x18:
      execflag = 0;
      retVal = 0;
      break;

    // dad d
    case 0x19:
      ptr = L + E;
      L += E;
      if ((ptr & 0x0100) == 0x0100) {
        ptr = H + D + 1;
        H = H + D + 1;
      } else {
        ptr = H + D;
        H += D;
      }
      if ((ptr & 0x0100) == 0x0100)
        flag.C = 1;
      else
        flag.C = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;

    // ldax d
    case 0x1A:
      buf = &ptr;
      *(char*)buf = E;
      *((char*)buf + 1) = D;
      A = memory[ptr];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // dcx d
    case 0x1B:
      if (!E--)
        D--;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // inr e
    case 0x1C:
      if (!++E)
        flag.C = 1;
      else
        flag.C = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // dcr e
    case 0x1D:
      if (E-- == 0)
        flag.C = 1;
      else
        flag.C = 0;

      if (!E)
        flag.Z = 1;
      else
        flag.Z = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mvi e
    case 0x1E:
      E = memory[IP + 1];
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // rar
    case 0x1F:
      _flag = flag.C;
      flag.C = A & 0x01;
      A >>= 1;
      if (_flag)
        A |= 0x80;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    //*****no such command!!!*****
    case 0x20:
      execflag = 0;
      retVal = 0;
      break;
    // lxi h
    case 0x21:
      H = memory[IP + 2];
      L = memory[IP + 1];
      IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    // shld
    case 0x22:
      *(unsigned short*)(memory + IP + 1) = L;
      *(unsigned short*)(memory + IP + 2) = H;
      IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    // inx h
    case 0x23:
      if (++L == 0)
        H++;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // inr h
    case 0x24:
      if (!++H)
        flag.C = 1;
      else
        flag.C = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // dcr h
    case 0x25:
      if (!H--)
        flag.C = 1;
      else
        flag.C = 0;

      if (!H)
        flag.Z = 1;
      else
        flag.Z = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mvi h
    case 0x26:
      H = memory[IP + 1];
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // command DAA is ignored!!!
    case 0x27:
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    //*****no such command!!!*****
    case 0x28:
      execflag = 0;
      retVal = 0;
      break;
    // dad H
    case 0x29:
      _flag = L + L;
      L += L;
      if ((_flag & 0x0100) == 0x0100) {
        _flag = H + H + 1;
        H = H + H + 1;
      } else {
        _flag = H + H;
        H += H;
      }
      if ((_flag & 0x0100) == 0x0100)
        flag.C = 1;
      else
        flag.C = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // lhld
    case 0x2A:
      //	ptr=*(unsigned short*)(memory+IP+1);
      L = *(memory + *(unsigned short*)(memory + IP + 1));
      H = *(memory + *(unsigned short*)(memory + IP + 1) + 1);
      IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    // dcx Hz
    case 0x2B:
      if (!--L)
        H--;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // inr l
    case 0x2C:
      L++;
      if (L == 0)
        flag.C = 1;
      else
        flag.C = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // dcr l
    case 0x2D:
      L--;
      if (L == 0) {
        flag.C = 1;
        flag.Z = 1;
      } else {
        flag.C = 0;
        flag.Z = 0;
      }
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mvi l
    case 0x2E:
      L = memory[IP + 1];
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // cma
    case 0x2F:
      A ^= 0xFF;
      IP++;
      execflag = 0;
      retVal = 1;
      break;

    //*****no such command!!!*****
    case 0x30:
      execflag = 0;
      retVal = 0;
      break;
    // lxi sp
    case 0x31:
      SP = *((unsigned short*)(memory + IP + 1));
      IP += 3;
      execflag = 0;
      retVal = 1;
      break;

    // STA
    case 0x32:
      *(memory + *(unsigned short*)(memory + IP + 1)) = A;
      IP += 3;
      execflag = 0;
      retVal = 1;
      break;

    // inx sp
    case 0x33:
      SP++;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // inr m
    case 0x34:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      memory[ptr]++;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // dcr m
    case 0x35:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      memory[ptr]--;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mvi m
    case 0x36:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      memory[ptr] = memory[IP + 1];
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // stc
    case 0x37:
      flag.C = 1;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    //*****no such command!!!*****
    case 0x38:
      execflag = 0;
      retVal = 0;
      break;
    // DAD SP
    case 0x39:
      SP += SP + H * 0xff + L;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // lda
    case 0x3A:
      A = *(memory + *(unsigned short*)(memory + IP + 1));
      IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    // dcx sp
    case 0x3B:
      SP--;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // inr a
    case 0x3C:
      if (!++A)
        flag.C = 1;
      else
        flag.C = 0;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // dcr a
    case 0x3D:
      if (!--A) {
        flag.C = 1;
        flag.Z = 1;
      } else {
        flag.C = 0;
        flag.Z = 0;
      }
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mvi A
    case 0x3E:
      A = memory[IP + 1];
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // cmc
    case 0x3F:
      flag.C = 1 - flag.C;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x40:
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x41:
      B = C;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x42:
      B = D;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x43:
      B = E;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x44:
      B = H;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x45:
      B = L;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x46:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      B = memory[ptr];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x47:
      B = A;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x48:
      C = B;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x49:
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x4A:
      C = D;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x4B:
      C = E;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x4C:
      C = H;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x4D:
      C = L;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x4E:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      C = memory[ptr];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x4F:
      C = A;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x50:
      D = B;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x51:
      D = C;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x52:
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x53:
      D = E;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x54:
      D = H;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x55:
      D = L;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x56:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      D = memory[ptr];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x57:
      D = A;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x58:
      E = B;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x59:
      E = C;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x5A:
      E = D;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x5B:
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x5C:
      E = H;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x5D:
      E = L;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x5E:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      E = memory[ptr];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x5F:
      E = A;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x60:
      H = B;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x61:
      H = C;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x62:
      H = D;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x63:
      H = E;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x64:
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x65:
      H = L;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x66:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      H = memory[ptr];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x67:
      H = A;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x68:
      L = B;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x69:
      L = C;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x6A:
      L = D;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x6B:
      L = E;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x6C:
      L = H;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x6D:
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x6E:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      L = memory[ptr];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x6F:
      L = A;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x70:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      memory[ptr] = B;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x71:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      memory[ptr] = C;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x72:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      memory[ptr] = D;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x73:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      memory[ptr] = E;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x74:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      memory[ptr] = H;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x75:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      memory[ptr] = L;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // HLT
    case 0x76:
      execflag = 0;
      retVal = 0;
      break;
    // mov
    case 0x77:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      memory[ptr] = A;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x78:
      A = B;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x79:
      A = C;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x7A:
      A = D;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x7B:
      A = E;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x7C:
      A = H;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x7D:
      A = L;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x7E:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      A = memory[ptr];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // mov
    case 0x7F:
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADD B
    case 0x80:
      set_flags(A + B);
      A += B;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADD C
    case 0x81:
      set_flags(A + C);
      A += C;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADD D
    case 0x82:
      set_flags(A + D);
      A += D;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADD E
    case 0x83:
      set_flags(A + E);
      A += E;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADD H
    case 0x84:
      set_flags(A + H);
      A += H;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADD L
    case 0x85:
      set_flags(A + L);
      A += L;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADD M
    case 0x86:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      set_flags(A + memory[ptr]);
      A += memory[ptr];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADD A
    case 0x87:
      set_flags(A + A);
      A += A;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADC B
    case 0x88:
      _flag = flag.C;
      set_flags(A + B + _flag);
      A = A + B + _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADC C
    case 0x89:
      _flag = flag.C;
      set_flags(A + C + _flag);
      A = A + C + _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADC D
    case 0x8A:
      _flag = flag.C;
      set_flags(A + D + _flag);
      A = A + D + _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADC E
    case 0x8B:
      _flag = flag.C;
      set_flags(A + E + _flag);
      A = A + E + _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADC H
    case 0x8C:
      _flag = flag.C;
      set_flags(A + H + _flag);
      A = A + H + _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADC L
    case 0x8D:
      _flag = flag.C;
      set_flags(A + L + _flag);
      A = A + L + _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADC M
    case 0x8E:
      _flag = flag.C;
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      set_flags(A + flag.C + memory[ptr]);
      A += memory[ptr] + flag.C;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ADC A
    case 0x8F:
      _flag = flag.C;
      set_flags(A + A + _flag);
      A = A + A + _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // sub b
    case 0x90:
      set_flags(A - B);
      A -= B;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // sub c
    case 0x91:
      set_flags(A - C);
      A -= C;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // sub d
    case 0x92:
      set_flags(A - D);
      A -= D;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // sub e
    case 0x93:
      set_flags(A - E);
      A -= E;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // sub h
    case 0x94:
      set_flags(A - H);
      A -= H;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // sub l
    case 0x95:
      set_flags(A - L);
      A -= L;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // sub m
    case 0x96:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      set_flags(A - memory[ptr]);
      A -= memory[ptr];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // sub a
    case 0x97:
      set_flags(A - A);
      A -= A;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // SBB B
    case 0x98:
      _flag = flag.C;
      set_flags(A - B - _flag);
      A = A - B - _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // SBB C
    case 0x99:
      _flag = flag.C;
      set_flags(A - C - _flag);
      A = A - C - _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // SBB D
    case 0x9A:
      _flag = flag.C;
      set_flags(A - D - _flag);
      A = A - D - _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // SBB E
    case 0x9B:
      _flag = flag.C;
      set_flags(A - E - _flag);
      A = A - E - _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // SBB H
    case 0x9C:
      _flag = flag.C;
      set_flags(A - H - _flag);
      A = A - H - _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // SBB L
    case 0x9D:
      _flag = flag.C;
      set_flags(A - L - _flag);
      A = A - L - _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // SBB M
    case 0x9E:
      _flag = flag.C;
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      set_flags(A - memory[ptr] - _flag);
      A = A - memory[ptr] - _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // SBB A
    case 0x9F:
      _flag = flag.C;
      set_flags(A - B - _flag);
      A = A - B - _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ANA B
    case 0xA0:
      A &= B;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ANA C
    case 0xA1:
      A &= C;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ANA D
    case 0xA2:
      A &= D;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ANA E
    case 0xA3:
      A &= E;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ANA H
    case 0xA4:
      A &= H;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ANA L
    case 0xA5:
      A &= L;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ANA M
    case 0xA6:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      A &= memory[ptr];
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ANA A
    case 0xA7:
      A &= A;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // XRA B
    case 0xA8:
      A ^= B;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // XRA C
    case 0xA9:
      A ^= C;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // XRA D
    case 0xAA:
      A ^= D;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // XRA E
    case 0xAB:
      A ^= E;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // XRA H
    case 0xAC:
      A ^= H;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // XRA L
    case 0xAD:
      A ^= L;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // XRA M
    case 0xAE:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      A ^= memory[ptr];
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // XRA A
    case 0xAF:
      A ^= A;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ORA B
    case 0xB0:
      A |= B;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ORA C
    case 0xB1:
      A |= C;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ORA D
    case 0xB2:
      A |= D;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ORA E
    case 0xB3:
      A |= E;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ORA H
    case 0xB4:
      A |= H;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ORA L
    case 0xB5:
      A |= L;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ORA M
    case 0xB6:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      A |= memory[ptr];
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ORA A
    case 0xB7:
      A |= A;
      set_flags(A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // CMP B
    case 0xB8:
      set_flags(A - B);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // CMP C
    case 0xB9:
      set_flags(A - C);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // CMP D
    case 0xBA:
      set_flags(A - D);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // CMP E
    case 0xBB:
      set_flags(A - E);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // CMP H
    case 0xBC:
      set_flags(A - H);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // CMP L
    case 0xBD:
      set_flags(A - L);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // CMP M
    case 0xBE:
      buf = &ptr;
      *(char*)buf = L;
      *((char*)buf + 1) = H;
      set_flags(A - memory[ptr]);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // CMP A
    case 0xBF:
      set_flags(A - A);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // RNZ
    case 0xC0:
      if (!flag.Z) {
        IP = *(unsigned short*)(memory + SP);
        SP += 2;
      } else
        IP += 1;
      execflag = 0;
      retVal = 1;
      break;
    // POP b
    case 0xC1:
      C = memory[SP++];
      B = memory[SP++];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // jnz
    case 0xC2:
      if (!flag.Z)
        IP = *(unsigned short*)(memory + IP + 1);
      else
        IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    // jmp
    case 0xC3:
      IP = *(unsigned short*)(memory + IP + 1);
      execflag = 0;
      retVal = 1;
      break;
    // cnz
    case 0xC4:
      if (!flag.Z) {
        SP -= 2;
        *(unsigned short*)(memory + SP) = IP + 3;
        IP = *(unsigned short*)(memory + IP + 1);
      } else
        IP += 3;
      execflag = 0;
      retVal = 3;
      break;
    // push b
    case 0xC5:
      memory[--SP] = B;
      memory[--SP] = C;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // a
    case 0xC6:
      A += memory[IP + 1];
      set_flags((int)A);
      IP += 2;
      execflag = 0;
      retVal = 2;
      break;
    // rst 0
    case 0xC7:
      SP -= 2;
      *((unsigned short*)(memory + SP)) = IP;
      IP = 0x0000;
      execflag = 0;
      retVal = 1;
      break;
    // rz
    case 0xC8:
      if (flag.Z) {
        IP = *(unsigned short*)(memory + SP);
        SP += 2;
      } else
        IP += 1;
      execflag = 0;
      retVal = 1;
      break;
    // ret
    case 0xC9:
      IP = *(unsigned short*)(memory + SP);
      SP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // jz
    case 0xCA:
      if (flag.Z)
        IP = *(unsigned short*)(memory + IP + 1);
      else
        IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    //*****no such command!!!*****
    case 0xCB:
      execflag = 0;
      retVal = 0;
      break;
    // cz
    case 0xCC:
      if (flag.Z) {
        SP -= 2;
        *(unsigned short*)(memory + SP) = IP + 3;
        IP = *(unsigned short*)(memory + IP + 1);
      } else
        IP += 3;
      execflag = 0;
      retVal = 3;
      break;
    // call
    case 0xCD:
      SP -= 2;
      *(unsigned short*)(memory + SP) = IP + 3;
      IP = *(unsigned short*)(memory + IP + 1);
      execflag = 0;
      retVal = 3;
      break;
    // aci
    case 0xCE:
      _flag = flag.C;
      set_flags(A + memory[IP + 1] + _flag);
      A = A + memory[IP + 1] + _flag;
      IP += 2;
      execflag = 0;
      retVal = 2;
      break;
    // rst 8
    case 0xCF:
      SP -= 2;
      *((unsigned short*)(memory + SP)) = IP;
      IP = 0x0080;
      execflag = 0;
      retVal = 1;
      break;
    // rnc
    case 0xD0:
      if (!flag.C) {
        IP = *(unsigned short*)(memory + SP);
        SP += 2;
      } else
        IP += 1;
      execflag = 0;
      retVal = 1;
      break;
    // pop d
    case 0xD1:
      E = memory[SP++];
      D = memory[SP++];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // jnc
    case 0xD2:
      if (!flag.C)
        IP = *(unsigned short*)(memory + IP + 1);
      else
        IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    // out
    case 0xD3:
      port[memory[IP + 1]] = A;
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // cnc
    case 0xD4:
      if (!flag.C) {
        SP -= 2;
        *(unsigned short*)(memory + SP) = IP + 3;
        IP = *(unsigned short*)(memory + IP + 1);
      } else
        IP += 3;
      execflag = 0;
      retVal = 3;
      break;
    // push d
    case 0xD5:
      memory[--SP] = D;
      memory[--SP] = E;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // sui
    case 0xD6:
      A -= memory[IP + 1];
      set_flags((int)A);
      IP += 2;
      execflag = 0;
      retVal = 2;
      break;
    // rst 16
    case 0xD7:
      SP -= 2;
      *((unsigned short*)(memory + SP)) = IP;
      IP = 0x0160;
      execflag = 0;
      retVal = 1;
      break;
    // rc
    case 0xD8:
      if (flag.C) {
        IP = *(unsigned short*)(memory + SP);
        SP += 2;
      } else
        IP += 1;
      execflag = 0;
      retVal = 1;
      break;
    //*****no such command!!!*****
    case 0xD9:
      execflag = 0;
      retVal = 0;
      break;
    // jc
    case 0xDA:
      if (flag.C)
        IP = *(unsigned short*)(memory + IP + 1);
      else
        IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    // in
    case 0xDB:
      A = port[memory[IP + 1]];
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // cc
    case 0xDC:
      if (flag.C) {
        SP -= 2;
        *(unsigned short*)(memory + SP) = IP + 3;
        IP = *(unsigned short*)(memory + IP + 1);
      } else
        IP += 3;
      execflag = 0;
      retVal = 3;
      break;
    //*****no such command!!!*****
    case 0xDD:
      execflag = 0;
      retVal = 0;
      break;
    // sbi
    case 0xDE:
      _flag = flag.C;
      set_flags(A - memory[IP + 1] - _flag);
      A = A - memory[IP + 1] - _flag;
      IP += 2;
      execflag = 0;
      retVal = 2;
      break;
    // rst 24
    case 0xDF:
      SP -= 2;
      *((unsigned short*)(memory + SP)) = IP;
      IP = 0x0240;
      execflag = 0;
      retVal = 1;
      break;
    // rpo
    case 0xE0:
      if (!flag.P) {
        IP = *(unsigned short*)(memory + SP);
        SP += 2;
      } else
        IP += 1;
      execflag = 0;
      retVal = 1;
      break;
    // pop h
    case 0xE1:
      L = memory[SP++];
      H = memory[SP++];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // jpo
    case 0xE2:
      if (!flag.P)
        IP = *(unsigned short*)(memory + IP + 1);
      else
        IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    // xthl
    case 0xE3:
      _flag = H;
      H = memory[SP + 1];
      memory[SP + 1] = _flag;
      _flag = L;
      L = memory[SP];
      memory[SP] = _flag;
      IP++;
    // cpo
    case 0xE4:
      if (!flag.P) {
        SP -= 2;
        *(unsigned short*)(memory + SP) = IP + 3;
        IP = *(unsigned short*)(memory + IP + 1);
      } else
        IP += 3;
      execflag = 0;
      retVal = 3;
      break;
    // push h
    case 0xE5:
      memory[--SP] = H;
      memory[--SP] = L;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ani
    case 0xE6:
      A &= memory[IP + 1];
      set_flags((int)A);
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // rst 32
    case 0xE7:
      SP -= 2;
      *((unsigned short*)(memory + SP)) = IP;
      IP = 0x0320;
      execflag = 0;
      retVal = 1;
      break;
    // rpe
    case 0xE8:
      if (flag.P) {
        IP = *(unsigned short*)(memory + SP);
        SP += 2;
      } else
        IP += 1;
      execflag = 0;
      retVal = 1;
      break;
    // pchl
    case 0xE9:
      IP = (H << 8) + L;
      execflag = 0;
      retVal = 1;
      break;
    // jpe
    case 0xEA:
      if (flag.P)
        IP = *(unsigned short*)(memory + IP + 1);
      else
        IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    // xchg
    case 0xEB:
      _flag = H;
      H = D;
      D = _flag;
      _flag = L;
      L = E;
      E = _flag;
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // cpe
    case 0xEC:
      if (flag.C) {
        SP -= 2;
        *(unsigned short*)(memory + SP) = IP + 3;
        IP = *(unsigned short*)(memory + IP + 1);
      } else
        IP += 3;
      execflag = 0;
      retVal = 3;
      break;
    //*****no such command!!!*****
    case 0xED:
      execflag = 0;
      retVal = 0;
      break;
    // xri
    case 0xEE:
      A ^= memory[IP + 1];
      set_flags((int)A);
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // rst 40
    case 0xEF:
      SP -= 2;
      *((unsigned short*)(memory + SP)) = IP;
      IP = 0x0400;
      execflag = 0;
      retVal = 1;
      break;
    // rp
    case 0xF0:
      if (!flag.S) {
        IP = *(unsigned short*)(memory + SP);
        SP += 2;
      } else
        IP += 1;
      execflag = 0;
      retVal = 1;
      break;
    // pop psw
    case 0xF1:
      if (memory[SP] & 1)
        flag.C = 1;
      else
        flag.C = 0;
      if (memory[SP] & 4)
        flag.P = 1;
      else
        flag.P = 0;
      if (memory[SP] & 64)
        flag.Z = 1;
      else
        flag.Z = 0;
      if (memory[SP] & 128)
        flag.S = 1;
      else
        flag.S = 0;
      SP++;
      A = memory[SP++];
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // jp
    case 0xF2:
      if (!flag.S)
        IP = *(unsigned short*)(memory + IP + 1);
      else
        IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    // COM DI IGNORED
    case 0xF3:
      flag.I = 0;
      IP++;
      my_timer_count--;
      execflag = 0;
      retVal = 1;
      break;
    // cp
    case 0xF4:
      if (!flag.S) {
        SP -= 2;
        *(unsigned short*)(memory + SP) = IP + 3;
        IP = *(unsigned short*)(memory + IP + 1);
      } else
        IP += 3;
      execflag = 0;
      retVal = 3;
      break;
    // push psw
    case 0xF5:
      memory[--SP] = A;
      memory[--SP] = 0x02;
      memory[SP] |= flag.C;
      memory[SP] |= (flag.P << 2);
      memory[SP] |= (flag.Z << 6);
      memory[SP] |= (flag.S << 7);
      IP++;
      execflag = 0;
      retVal = 1;
      break;
    // ori
    case 0xF6:
      A |= memory[IP + 1];
      set_flags((int)A);
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // rst 48
    case 0xF7:
      SP -= 2;
      *((unsigned short*)(memory + SP)) = IP;
      IP = 0x0480;
      execflag = 0;
      retVal = 1;
      break;
    // rm
    case 0xF8:
      if (flag.S) {
        IP = *(unsigned short*)(memory + SP);
        SP += 2;
      } else
        IP += 1;
      execflag = 0;
      retVal = 1;
      break;
    // sphl
    case 0xF9:
      IP++;
      SP = (H << 8) + L;
      execflag = 0;
      retVal = 1;
      break;
    // jm
    case 0xFA:
      if (flag.S)
        IP = *(unsigned short*)(memory + IP + 1);
      else
        IP += 3;
      execflag = 0;
      retVal = 1;
      break;
    // COM EI IGNORED
    case 0xFB:
      ret_flag = 1;
      IP++;
      // my_timer_count--;
      execflag = 0;
      retVal = 1;
      break;
    // cm
    case 0xFC:
      if (flag.S) {
        SP -= 2;
        *(unsigned short*)(memory + SP) = IP + 3;
        IP = *(unsigned short*)(memory + IP + 1);
      } else
        IP += 3;
      execflag = 0;
      retVal = 3;
      break;
    //*****no such command!!!*****
    case 0xFD:
      execflag = 0;
      retVal = 0;
      break;
    // cpi
    case 0xFE:
      set_flags(A - memory[IP + 1]);
      IP += 2;
      execflag = 0;
      retVal = 1;
      break;
    // rst 56
    case 0xFF:
      SP -= 2;
      *((unsigned short*)(memory + SP)) = IP;
      IP = 0x0560;
      execflag = 0;
      retVal = 1;
      break;
    default:
      execflag = 0;
      retVal = 0;
      break;
  }
  this->sharedMemory.unlock();
  this->sharedPorts.unlock();
  return retVal;
}

void
VM::set_memory(unsigned char* m)
{
  this->sharedMemory.lock();
  memcpy(this->memory, m, MEMSIZE);
  this->sharedMemory.unlock();
}

void
VM::get_memory(unsigned char* m)
{
  this->sharedMemory.lock();
  memcpy(m, this->memory, MEMSIZE);
  this->sharedMemory.unlock();
}
