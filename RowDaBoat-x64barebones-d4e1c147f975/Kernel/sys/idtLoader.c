// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <idtLoader.h>

#pragma pack(push) /* Push of current alignment */
#pragma pack(1)    /* aligns next structures to 1 byte */

/* Interruption descriptor */
typedef struct
{
  uint16_t offset_l, selector;
  uint8_t cero, access;
  uint16_t offset_m;
  uint32_t offset_h, other_cero;
} DESCR_INT;

#pragma pack(pop) /* restores current alignment */

DESCR_INT *idt = (DESCR_INT *)0; // IDT with 255 entries

static void setup_IDT_entry(int index, uint64_t offset);

void load_idt()
{
  _cli();
  setup_IDT_entry(0x00, (uint64_t)&_exception0Handler);
  setup_IDT_entry(0x01, (uint64_t)&_exception1Handler);
  setup_IDT_entry(0x02, (uint64_t)&_exception2Handler);
  setup_IDT_entry(0x03, (uint64_t)&_exception3Handler);
  setup_IDT_entry(0x04, (uint64_t)&_exception4Handler);
  setup_IDT_entry(0x05, (uint64_t)&_exception5Handler);
  setup_IDT_entry(0x06, (uint64_t)&_exception6Handler);
  setup_IDT_entry(0x07, (uint64_t)&_exception7Handler);
  setup_IDT_entry(0x08, (uint64_t)&_exception8Handler);
  setup_IDT_entry(0x09, (uint64_t)&_exception9Handler);
  setup_IDT_entry(0x0A, (uint64_t)&_exception10Handler);
  setup_IDT_entry(0x0B, (uint64_t)&_exception11Handler);
  setup_IDT_entry(0x0C, (uint64_t)&_exception12Handler);
  setup_IDT_entry(0x0D, (uint64_t)&_exception13Handler);
  setup_IDT_entry(0x0E, (uint64_t)&_exception14Handler);
  setup_IDT_entry(0x0F, (uint64_t)&_exception15Handler);
  setup_IDT_entry(0x10, (uint64_t)&_exception16Handler);
  setup_IDT_entry(0x11, (uint64_t)&_exception17Handler);
  setup_IDT_entry(0x12, (uint64_t)&_exception18Handler);
  setup_IDT_entry(0x13, (uint64_t)&_exception19Handler);
  setup_IDT_entry(0x14, (uint64_t)&_exception20Handler);
  setup_IDT_entry(0x20, (uint64_t)&_irq00Handler);
  setup_IDT_entry(0x21, (uint64_t)&_irq01Handler);
  setup_IDT_entry(0x80, (uint64_t)&_sysCallHandler); // User interruptions

  setupSysCalls();

  // Only keyboard and timer tick interruptions enabled
  picMasterMask(0xFC);
  picSlaveMask(0xFF);

  _sti();
}

static void setup_IDT_entry(int index, uint64_t offset)
{
  idt[index].selector = 0x08;
  idt[index].offset_l = offset & 0xFFFF;
  idt[index].offset_m = (offset >> 16) & 0xFFFF;
  idt[index].offset_h = (offset >> 32) & 0xFFFFFFFF;
  idt[index].access = ACS_INT;
  idt[index].cero = 0;
  idt[index].other_cero = (uint64_t)0;
}
