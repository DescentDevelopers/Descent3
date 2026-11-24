/*
 * Descent 3
 * Copyright (C) 2024 Parallax Software
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "x86sandbox.h"

using x86sandbox::HostCallHandler;
using x86sandbox::Sandbox;
using x86sandbox::Status;

namespace {

// The host side of test_dll.c's API table.
struct TestApi : HostCallHandler {
  uint32_t last_read = 0;
  int32_t HostCall(Sandbox &sb, uint32_t index, uint32_t esp) override {
    switch (index) {
    case 0: // int add(int, int)
      return static_cast<int32_t>(sb.Arg(esp, 0) + sb.Arg(esp, 1));
    case 1: { // int call_back(void *fn, int arg): calls back into the guest
      uint32_t r = 0;
      Status s = sb.CallPointer(sb.Arg(esp, 0), {sb.Arg(esp, 1)}, &r);
      if (s != Status::Ok) {
        sb.AbortGuest(s);
        return 0;
      }
      return static_cast<int32_t>(r);
    }
    case 2: // float getf(void)
      sb.ReturnDouble(1.25);
      return 0;
    case 3: { // int read(int addr): reads guest memory through the checked accessor
      uint32_t v = 0;
      if (!sb.Read32(sb.Arg(esp, 0), &v)) {
        sb.AbortGuest(Status::BadArgs);
        return 0;
      }
      return static_cast<int32_t>(v);
    }
    }
    sb.AbortGuest();
    return 0;
  }
};

std::vector<uint8_t> ReadFile(const char *path) {
  std::ifstream f(path, std::ios::binary);
  return std::vector<uint8_t>((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
}

class X86Sandbox : public ::testing::Test {
protected:
  void SetUp() override {
#ifndef X86SANDBOX_TEST_DLL
    GTEST_SKIP() << "test DLL not built (needs clang, lld-link and llvm-dlltool)";
#else
    dll_ = ReadFile(X86SANDBOX_TEST_DLL);
    ASSERT_FALSE(dll_.empty());
    sb_ = Sandbox::Create();
    ASSERT_NE(sb_, nullptr);
    sb_->SetHandler(&api_);
    ASSERT_EQ(sb_->Load(dll_.data(), dll_.size()), Status::Ok) << sb_->LastError();
    uint32_t fp = sb_->Alloc(16);
    ASSERT_NE(fp, 0u);
    for (uint32_t i = 0; i < 4; i++)
      ASSERT_TRUE(sb_->Write32(fp + 4 * i, sb_->ApiStub(i)));
    uint32_t attached = 0;
    ASSERT_EQ(sb_->Call(Export("Init", 4), {fp}, &attached), Status::Ok) << sb_->LastError();
    ASSERT_EQ(attached, 7u) << "DllMain did not run before the first call";
#endif
  }

  uint32_t Export(const char *name, int arg_bytes) {
    std::string decorated = std::string("_") + name + "@" + std::to_string(arg_bytes);
    uint32_t a = sb_->FindExport(decorated.c_str());
    EXPECT_NE(a, 0u) << decorated;
    return a;
  }

  uint32_t CallOk(const char *name, int arg_bytes, std::initializer_list<uint32_t> args) {
    uint32_t r = 0;
    Status s = sb_->Call(Export(name, arg_bytes), args, &r);
    EXPECT_EQ(s, Status::Ok) << name << ": " << sb_->LastError();
    return r;
  }

  // Calls a function that must stop the module with `want`, then checks the module is dead.
  // If `in_error` is given, the error message must contain it.
  void ExpectFatal(const char *name, int arg_bytes, std::initializer_list<uint32_t> args, Status want,
                   const char *in_error = nullptr) {
    sb_->SetInstructionBudget(5000000);
    Status s = sb_->Call(Export(name, arg_bytes), args);
    EXPECT_EQ(s, want) << name << ": " << x86sandbox::StatusName(s) << ": " << sb_->LastError();
    if (in_error)
      EXPECT_NE(sb_->LastError().find(in_error), std::string::npos) << sb_->LastError();
    EXPECT_TRUE(sb_->IsDead());
    EXPECT_EQ(sb_->Call(Export("Add3", 12), {1, 1, 1}), Status::Dead);
  }

  std::vector<uint8_t> dll_;
  std::unique_ptr<Sandbox> sb_;
  TestApi api_;
};

TEST_F(X86Sandbox, CallsExports) { EXPECT_EQ(CallOk("Add3", 12, {1, 2, 39}), 42u); }

TEST_F(X86Sandbox, CallsTheHostApi) { EXPECT_EQ(CallOk("UseApi", 8, {20, 1}), 42u); }

TEST_F(X86Sandbox, FormatsWithSprintfImport) {
  double d = 2.5;
  uint32_t parts[2];
  memcpy(parts, &d, 8);
  uint32_t s = CallOk("Fmt", 12, {7, parts[0], parts[1]});
  std::string out;
  ASSERT_TRUE(sb_->ReadString(s, out, 200));
  EXPECT_EQ(out, "v=7 d=2.500 s=ok x=beef");
}

TEST_F(X86Sandbox, RunsX87Code) { EXPECT_EQ(CallOk("FloatMath", 4, {10}), 15u); }

TEST_F(X86Sandbox, ReturnsDoublesFromImports) { EXPECT_EQ(CallOk("SinTimes1000", 4, {30}), 499u); }

TEST_F(X86Sandbox, ReturnsFloatsFromTheHost) { EXPECT_EQ(CallOk("GetF100", 0, {}), 125u); }

TEST_F(X86Sandbox, NestsGuestCalls) {
  EXPECT_EQ(CallOk("Nested", 4, {9}), 82u);
  EXPECT_EQ(CallOk("Recurse", 4, {5}), 5u);
}

TEST_F(X86Sandbox, UsesTheGuestHeap) { EXPECT_EQ(CallOk("Strings", 0, {}), 6u); }

TEST_F(X86Sandbox, QsortCallsBackIntoTheGuest) { EXPECT_EQ(CallOk("Sort", 0, {}), 123579u); }

// About 5 million comparisons for a heapsort; billions for a quadratic sort.
TEST_F(X86Sandbox, QsortSortsLargeArraysWithinTheBudget) {
  sb_->SetInstructionBudget(300000000);
  EXPECT_EQ(CallOk("SortMany", 4, {150000}), 1u);
}

// The comparison function is guest code: whatever it returns or changes, qsort
// must return and stay inside the array.
TEST_F(X86Sandbox, QsortSurvivesHostileComparisonFunctions) {
  EXPECT_EQ(CallOk("SortHostile", 4, {0}), 1u) << "random results";
  EXPECT_EQ(CallOk("SortHostile", 4, {1}), 1u) << "writes into the array";
}

TEST_F(X86Sandbox, RefusesFileAccess) { EXPECT_EQ(CallOk("OpenFile", 0, {}), 1u); }

TEST_F(X86Sandbox, ReportsUnsupportedImports) {
  bool found = false;
  for (auto &imp : sb_->Imports())
    if (imp.first == "kernel32!GetDiskFreeSpaceA") {
      found = true;
      EXPECT_FALSE(imp.second);
    }
  EXPECT_TRUE(found);
}

TEST_F(X86Sandbox, HostAccessorsStayInsideGuestMemory) {
  uint32_t size = sb_->GuestSize();
  EXPECT_EQ(sb_->Ptr(size - 4, 4) != nullptr, true);
  EXPECT_EQ(sb_->Ptr(size - 3, 4), nullptr);
  EXPECT_EQ(sb_->Ptr(0xfffffffcu, 8), nullptr);
  uint32_t v;
  EXPECT_FALSE(sb_->Read32(size, &v));
  // A string that runs off the end of guest memory is rejected.
  ASSERT_TRUE(sb_->Write(size - 1, "A", 1));
  std::string s;
  EXPECT_FALSE(sb_->ReadString(size - 1, s, 100));
  ASSERT_TRUE(sb_->Write(size - 1, "", 1));
  EXPECT_TRUE(sb_->ReadString(size - 1, s, 100));
  EXPECT_EQ(s, "");
}

TEST_F(X86Sandbox, HostReadOfABadGuestPointerStopsTheGuest) {
  ExpectFatal("HostRead", 4, {0xfffffff0u}, Status::BadArgs);
}

TEST_F(X86Sandbox, ImportsTheCrtdllRuntime) { EXPECT_EQ(CallOk("CrtdllImports", 0, {}), 1111u); }
TEST_F(X86Sandbox, ResolvesImportsAtRunTime) { EXPECT_EQ(CallOk("RuntimeImports", 0, {}), 1111u); }
TEST_F(X86Sandbox, IsBadReadPtrFollowsThePageMapping) { EXPECT_EQ(CallOk("BadPointers", 0, {}), 1111u); }
TEST_F(X86Sandbox, ReallocInPlaceDoesNotMove) { EXPECT_EQ(CallOk("ReallocInPlace", 0, {}), 111u); }
TEST_F(X86Sandbox, ComparesStrings) { EXPECT_EQ(CallOk("CompareStrings", 0, {}), 111u); }
TEST_F(X86Sandbox, DebugBreakStops) { ExpectFatal("CallDebugBreak", 0, {}, Status::Aborted); }

TEST_F(X86Sandbox, RunawayLoopHitsTheBudget) { ExpectFatal("Loop", 0, {}, Status::Budget); }
TEST_F(X86Sandbox, BudgetErrorNamesTheRegisters) {
  sb_->SetInstructionBudget(1000000);
  ASSERT_EQ(sb_->Call(Export("Loop", 0), {}), Status::Budget);
  EXPECT_NE(sb_->LastError().find(" eax="), std::string::npos) << sb_->LastError();
}

// Each iteration of a REP instruction counts against the budget: 1000 rep stosd
// of 50000 dwords are 50 million iterations but only a few thousand instructions.
TEST_F(X86Sandbox, RepIterationsCountAgainstTheBudget) {
  sb_->SetInstructionBudget(5000000);
  EXPECT_EQ(sb_->Call(Export("RepStosLoop", 4), {1000}), Status::Budget) << sb_->LastError();
}

// A REP instruction stopped at the end of a chunk resumes where it left off.
TEST_F(X86Sandbox, RepInstructionsResumeAcrossChunks) {
  sb_->SetInstructionBudget(50000000);
  uint32_t sum = 0;
  for (int i = 0; i < 200000; i++)
    sum = sum * 31 + (i < 150001 ? static_cast<uint8_t>(i * 7) : 0);
  EXPECT_EQ(CallOk("RepMovs", 4, {150001}), sum);
  EXPECT_EQ(CallOk("RepScas", 4, {123457}), 123457u);
  EXPECT_EQ(CallOk("RepCmps", 4, {98765}), 98765u);
}
TEST_F(X86Sandbox, UndefinedInstructionStops) { ExpectFatal("Trap", 0, {}, Status::Exception); }
TEST_F(X86Sandbox, PrivilegedInstructionStops) { ExpectFatal("Privileged", 0, {}, Status::Exception); }
TEST_F(X86Sandbox, SoftwareInterruptStops) { ExpectFatal("SoftwareInterrupt", 0, {}, Status::Exception); }
TEST_F(X86Sandbox, SegmentLoadStops) { ExpectFatal("LoadSegment", 0, {}, Status::Exception); }
TEST_F(X86Sandbox, IoWindowAccessStops) { ExpectFatal("WildWrite", 0, {}, Status::MemFault, "below the stack"); }

// Guest memory is mapped with paging, and every other address faults.
TEST_F(X86Sandbox, NullPointerReadStops) { ExpectFatal("ReadAt", 4, {0}, Status::MemFault, "a null pointer"); }
TEST_F(X86Sandbox, NullPointerWriteStops) { ExpectFatal("WriteAt", 4, {0xfffc}, Status::MemFault, "a null pointer"); }
TEST_F(X86Sandbox, ReadPastTheEndStops) {
  ExpectFatal("ReadAt", 4, {0x08000000}, Status::MemFault, "past the end of guest memory");
}
TEST_F(X86Sandbox, WritePastTheEndStops) {
  ExpectFatal("WriteAt", 4, {0x7ffffff0}, Status::MemFault, "past the end of guest memory");
}
TEST_F(X86Sandbox, PageTablesAreNotWritable) {
  // The page tables start where the guest's memory ends.
  ExpectFatal("WriteAt", 4, {sb_->GuestSize()}, Status::MemFault, "past the end of guest memory");
}
TEST_F(X86Sandbox, GapAboveTheStackStops) {
  ExpectFatal("WriteAt", 4, {0x00300000}, Status::MemFault, "above the stack");
}
TEST_F(X86Sandbox, JumpOutsideGuestMemoryStops) {
  ExpectFatal("CallAt", 4, {0x08000000}, Status::MemFault, "past the end of guest memory");
}
TEST_F(X86Sandbox, OutToAnUnknownPortStops) { ExpectFatal("OutToOtherPort", 0, {}, Status::Aborted); }
TEST_F(X86Sandbox, CallingAnUnsupportedImportStops) { ExpectFatal("MissingImport", 0, {}, Status::MissingImport); }
TEST_F(X86Sandbox, DeepHostRecursionStops) { ExpectFatal("Recurse", 4, {100}, Status::Nesting); }
TEST_F(X86Sandbox, GuestStackOverflowStops) {
  ExpectFatal("StackOverflow", 0, {}, Status::MemFault, "below the stack");
}

TEST(X86SandboxImage, RejectsNonPeData) {
  auto sb = Sandbox::Create();
  ASSERT_NE(sb, nullptr);
  std::vector<uint8_t> junk(4096, 0x41);
  EXPECT_EQ(sb->Load(junk.data(), junk.size()), Status::BadImage);
}

TEST(X86SandboxImage, RejectsTruncatedAndCorruptedImages) {
#ifndef X86SANDBOX_TEST_DLL
  GTEST_SKIP() << "test DLL not built";
#else
  std::vector<uint8_t> dll = ReadFile(X86SANDBOX_TEST_DLL);
  ASSERT_GT(dll.size(), 1024u);
  for (size_t cut : {size_t(64), size_t(300), dll.size() / 2}) {
    auto sb = Sandbox::Create();
    EXPECT_NE(sb->Load(dll.data(), cut), Status::Ok) << "truncated at " << cut;
  }
  // Flip bytes across the headers and tables: the loader must reject or load, never trap.
  for (size_t at = 0; at < 1024; at += 7) {
    std::vector<uint8_t> bad = dll;
    bad[at] ^= 0xff;
    auto sb = Sandbox::Create();
    sb->SetInstructionBudget(1000000);
    Status s = sb->Load(bad.data(), bad.size());
    EXPECT_NE(s, Status::Trap) << "byte " << at;
  }
#endif
}

} // namespace
