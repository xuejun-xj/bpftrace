#include <algorithm>
#include <array>

#include "arch.h"

// SP + 8 points to the first argument that is passed on the stack
enum { ARG0_STACK = 8 };

namespace bpftrace::arch {

// clang-format off
static std::array<std::string, 27> registers = {
  "r15",
  "r14",
  "r13",
  "r12",
  "bp",
  "bx",
  "r11",
  "r10",
  "r9",
  "r8",
  "ax",
  "cx",
  "dx",
  "si",
  "di",
  "orig_ax",
  "ip",
  "cs",
  "flags",
  "sp",
  "ss",
};

static std::array<std::string, 6> arg_registers = {
  "di",
  "si",
  "dx",
  "cx",
  "r8",
  "r9",
};
// clang-format on

int offset(std::string reg_name)
{
  auto *it = std::ranges::find(registers, reg_name);
  if (it == registers.end())
    return -1;
  return distance(registers.begin(), it);
}

int max_arg()
{
  return arg_registers.size() - 1;
}

int arg_offset(int arg_num)
{
  return offset(arg_registers.at(arg_num));
}

int ret_offset()
{
  return offset("ax");
}

int pc_offset()
{
  return offset("ip");
}

int sp_offset()
{
  return offset("sp");
}

int arg_stack_offset()
{
  return ARG0_STACK / 8;
}

std::string name()
{
  return "x86_64";
}

const std::unordered_set<std::string> &watchpoint_modes()
{
  // See intel developer manual, Volume 3, section 17.2.4.
  static std::unordered_set<std::string> valid_modes = {
    "rw",
    "w",
    "x",
  };
  return valid_modes;
}

int get_kernel_ptr_width()
{
  return 64;
}

} // namespace bpftrace::arch
