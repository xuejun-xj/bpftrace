#include "common.h"

namespace bpftrace::test::codegen {

TEST(codegen, struct_short)
{
  test("struct Foo { short x; }"
       "kprobe:f"
       "{"
       "  $foo = *(struct Foo*)arg0;"
       "  @x = $foo.x;"
       "}",
       std::string(NAME) + "_1");

  test("struct Foo { short x; }"
       "kprobe:f"
       "{"
       "  $foo = (struct Foo*)arg0;"
       "  @x = $foo->x;"
       "}",
       std::string(NAME) + "_2");
}

} // namespace bpftrace::test::codegen
