#include "mocks.h"
#include "tracefs/tracefs.h"
#include "gmock/gmock-nice-strict.h"

namespace bpftrace::test {

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::StrictMock;

void setup_mock_probe_matcher(MockProbeMatcher &matcher)
{
  ON_CALL(matcher, get_symbols_from_traceable_funcs(false)).WillByDefault([]() {
    std::string ksyms = "SyS_read\n"
                        "sys_read\n"
                        "sys_write\n"
                        "my_one\n"
                        "my_two\n";
    auto myval = std::unique_ptr<std::istream>(new std::istringstream(ksyms));
    return myval;
  });

  ON_CALL(matcher, get_symbols_from_traceable_funcs(true)).WillByDefault([]() {
    std::string ksyms = "kernel_mod:func_in_mod\n"
                        "kernel_mod:other_func_in_mod\n"
                        "other_kernel_mod:func_in_mod\n"
                        "vmlinux:queued_spin_lock_slowpath\n";
    auto myval = std::unique_ptr<std::istream>(new std::istringstream(ksyms));
    return myval;
  });

  ON_CALL(matcher, get_symbols_from_file(tracefs::available_events()))
      .WillByDefault([](const std::string &) {
        std::string tracepoints = "sched:sched_one\n"
                                  "sched:sched_two\n"
                                  "sched:foo\n"
                                  "sched_extra:sched_extra\n"
                                  "notsched:bar\n"
                                  "file:filename\n"
                                  "tcp:some_tcp_tp\n"
                                  "btf:tag\n"
                                  "vmlinux:event_rt\n";
        return std::unique_ptr<std::istream>(
            new std::istringstream(tracepoints));
      });

  ON_CALL(matcher, get_raw_tracepoint_symbols()).WillByDefault([]() {
    std::string rawtracepoints = "vmlinux:event_rt\n"
                                 "vmlinux:sched_switch\n";
    return std::unique_ptr<std::istream>(
        new std::istringstream(rawtracepoints));
  });

  ON_CALL(matcher, get_fentry_symbols()).WillByDefault([]() {
    std::string funcs = "vmlinux:func_1\n"
                        "vmlinux:func_2\n"
                        "vmlinux:func_3\n"
                        "vmlinux:queued_spin_lock_slowpath\n";
    return std::unique_ptr<std::istream>(new std::istringstream(funcs));
  });

  std::string sh_usyms = "/bin/sh:first_open\n"
                         "/bin/sh:second_open\n"
                         "/bin/sh:open_as_well\n"
                         "/bin/sh:something_else\n"
                         "/bin/sh:cpp_mangled\n"
                         "/bin/sh:_Z11cpp_mangledi\n"
                         "/bin/sh:_Z11cpp_mangledv\n"
                         "/bin/sh:_Z18cpp_mangled_suffixv\n";
  std::string bash_usyms = "/bin/bash:first_open\n";
  ON_CALL(matcher, get_func_symbols_from_file(_, "/bin/sh"))
      .WillByDefault([sh_usyms](std::optional<int>, const std::string &) {
        return std::unique_ptr<std::istream>(new std::istringstream(sh_usyms));
      });

  ON_CALL(matcher, get_func_symbols_from_file(_, "/bin/*sh"))
      .WillByDefault(
          [sh_usyms, bash_usyms](std::optional<int>, const std::string &) {
            return std::unique_ptr<std::istream>(
                new std::istringstream(sh_usyms + bash_usyms));
          });

  std::string sh_usdts = "/bin/sh:prov1:tp1\n"
                         "/bin/sh:prov1:tp2\n"
                         "/bin/sh:prov2:tp\n"
                         "/bin/sh:prov2:notatp\n"
                         "/bin/sh:nahprov:tp\n";
  std::string bash_usdts = "/bin/bash:prov1:tp3\n";
  ON_CALL(matcher, get_symbols_from_usdt(_, "/bin/sh"))
      .WillByDefault([sh_usdts](std::optional<int>, const std::string &) {
        return std::unique_ptr<std::istream>(new std::istringstream(sh_usdts));
      });
  ON_CALL(matcher, get_symbols_from_usdt(_, "/bin/*sh"))
      .WillByDefault(
          [sh_usdts, bash_usdts](std::optional<int>, const std::string &) {
            return std::unique_ptr<std::istream>(
                new std::istringstream(sh_usdts + bash_usdts));
          });

  ON_CALL(matcher, get_running_bpf_programs()).WillByDefault([]() {
    std::string bpf_progs = "bpf:123:func_1\n"
                            "bpf:123:func_2\n"
                            "bpf:456:func_1\n";
    return std::unique_ptr<std::istream>(new std::istringstream(bpf_progs));
  });
}

void setup_mock_bpftrace(MockBPFtrace &bpftrace)
{
  bpftrace.delta_taitime_ = timespec{};
  // Fill in some default tracepoint struct definitions
  bpftrace.structs.Add("struct _tracepoint_sched_sched_one", 8);
  bpftrace.structs.Lookup("struct _tracepoint_sched_sched_one")
      .lock()
      ->AddField("common_field", CreateUInt64(), 8, std::nullopt, false);

  bpftrace.structs.Add("struct _tracepoint_sched_sched_two", 8);
  bpftrace.structs.Lookup("struct _tracepoint_sched_sched_two")
      .lock()
      ->AddField("common_field",
                 CreateUInt64(),
                 16, // different offset than
                     // sched_one.common_field
                 std::nullopt,
                 false);
  bpftrace.structs.Add("struct _tracepoint_sched_extra_sched_extra", 8);
  bpftrace.structs.Lookup("struct _tracepoint_sched_extra_sched_extra")
      .lock()
      ->AddField("common_field",
                 CreateUInt64(),
                 24, // different offset than
                     // sched_(one|two).common_field
                 std::nullopt,
                 false);
  bpftrace.structs.Add("struct _tracepoint_tcp_some_tcp_tp", 16);
  bpftrace.structs.Lookup("struct _tracepoint_tcp_some_tcp_tp")
      .lock()
      ->AddField(
          "saddr_v6", CreateArray(16, CreateUInt(8)), 8, std::nullopt, false);

  auto ptr_type = CreatePointer(CreateInt8());
  bpftrace.structs.Add("struct _tracepoint_file_filename", 8);
  bpftrace.structs.Lookup("struct _tracepoint_file_filename")
      .lock()
      ->AddField("common_field", CreateUInt64(), 0, std::nullopt, false);
  bpftrace.structs.Lookup("struct _tracepoint_file_filename")
      .lock()
      ->AddField("filename", ptr_type, 8, std::nullopt, false);
  bpftrace.structs.Add("struct _tracepoint_btf_tag", 16);
  auto ptr_type_w_tag = CreatePointer(CreateInt8());
  ptr_type_w_tag.SetBtfTypeTags({ "rcu" });
  auto ptr_type_w_bad_tag = CreatePointer(CreateInt8());
  ptr_type_w_bad_tag.SetBtfTypeTags({ "rcu", "percpu" });
  bpftrace.structs.Lookup("struct _tracepoint_btf_tag")
      .lock()
      ->AddField("parent", ptr_type_w_tag, 8, std::nullopt, false);
  bpftrace.structs.Lookup("struct _tracepoint_btf_tag")
      .lock()
      ->AddField("real_parent", ptr_type_w_bad_tag, 16, std::nullopt, false);

  // Even though this is set to 1 by default, make it 0 here
  // to reduce the amount of repeated generated IR in the codegen tests
  bpftrace.helper_check_level_ = 0;
}

std::unique_ptr<MockBPFtrace> get_mock_bpftrace()
{
  auto bpftrace = std::make_unique<NiceMock<MockBPFtrace>>();
  setup_mock_bpftrace(*bpftrace);

  auto probe_matcher = std::make_unique<NiceMock<MockProbeMatcher>>(
      bpftrace.get());
  setup_mock_probe_matcher(*probe_matcher);
  bpftrace->set_mock_probe_matcher(std::move(probe_matcher));

  bpftrace->feature_ = std::make_unique<MockBPFfeature>(true);

  return bpftrace;
}

std::unique_ptr<MockBPFtrace> get_strict_mock_bpftrace()
{
  auto bpftrace = std::make_unique<StrictMock<MockBPFtrace>>();
  setup_mock_bpftrace(*bpftrace);

  auto probe_matcher = std::make_unique<StrictMock<MockProbeMatcher>>(
      bpftrace.get());
  setup_mock_probe_matcher(*probe_matcher);
  bpftrace->set_mock_probe_matcher(std::move(probe_matcher));

  bpftrace->feature_ = std::make_unique<MockBPFfeature>(true);

  return bpftrace;
}

std::unique_ptr<MockUSDTHelper> get_mock_usdt_helper(int num_locations)
{
  auto usdt_helper = std::make_unique<NiceMock<MockUSDTHelper>>();

  ON_CALL(*usdt_helper, find(_, _, _, _))
      .WillByDefault([num_locations](std::optional<int>,
                                     const std::string &,
                                     const std::string &,
                                     const std::string &) {
        return usdt_probe_entry{
          .path = "",
          .provider = "",
          .name = "",
          .semaphore_offset = 0,
          .num_locations = num_locations,
        };
      });

  return usdt_helper;
}

} // namespace bpftrace::test
