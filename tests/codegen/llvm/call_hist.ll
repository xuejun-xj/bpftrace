; ModuleID = 'bpftrace'
source_filename = "bpftrace"
target datalayout = "e-m:e-p:64:64-i64:64-i128:128-n32:64-S128"
target triple = "bpf-pc-linux"

; Function Attrs: nounwind
declare i64 @llvm.bpf.pseudo(i64 %0, i64 %1) #0

define i64 @"kprobe:f"(i8* %0) section "s_kprobe:f_1" {
entry:
  %initial_value = alloca i64, align 8
  %lookup_elem_val = alloca i64, align 8
  %"@x_key" = alloca i64, align 8
  %get_pid_tgid = call i64 inttoptr (i64 14 to i64 ()*)()
  %1 = lshr i64 %get_pid_tgid, 32
  %log2 = call i64 @log2(i64 %1)
  %2 = bitcast i64* %"@x_key" to i8*
  call void @llvm.lifetime.start.p0i8(i64 -1, i8* %2)
  store i64 %log2, i64* %"@x_key", align 8
  %pseudo = call i64 @llvm.bpf.pseudo(i64 1, i64 0)
  %lookup_elem = call i8* inttoptr (i64 1 to i8* (i64, i64*)*)(i64 %pseudo, i64* %"@x_key")
  %3 = bitcast i64* %lookup_elem_val to i8*
  call void @llvm.lifetime.start.p0i8(i64 -1, i8* %3)
  %map_lookup_cond = icmp ne i8* %lookup_elem, null
  br i1 %map_lookup_cond, label %lookup_success, label %lookup_failure

lookup_success:                                   ; preds = %entry
  %cast = bitcast i8* %lookup_elem to i64*
  %4 = load i64, i64* %cast, align 8
  %5 = add i64 %4, 1
  store i64 %5, i64* %cast, align 8
  br label %lookup_merge

lookup_failure:                                   ; preds = %entry
  %6 = bitcast i64* %initial_value to i8*
  call void @llvm.lifetime.start.p0i8(i64 -1, i8* %6)
  store i64 1, i64* %initial_value, align 8
  %pseudo1 = call i64 @llvm.bpf.pseudo(i64 1, i64 0)
  %update_elem = call i64 inttoptr (i64 2 to i64 (i64, i64*, i64*, i64)*)(i64 %pseudo1, i64* %"@x_key", i64* %initial_value, i64 1)
  %7 = bitcast i64* %initial_value to i8*
  call void @llvm.lifetime.end.p0i8(i64 -1, i8* %7)
  br label %lookup_merge

lookup_merge:                                     ; preds = %lookup_failure, %lookup_success
  %8 = bitcast i64* %lookup_elem_val to i8*
  call void @llvm.lifetime.end.p0i8(i64 -1, i8* %8)
  %9 = bitcast i64* %"@x_key" to i8*
  call void @llvm.lifetime.end.p0i8(i64 -1, i8* %9)
  ret i64 0
}

; Function Attrs: alwaysinline
define internal i64 @log2(i64 %0) #1 section "helpers" {
entry:
  %1 = alloca i64, align 8
  %2 = alloca i64, align 8
  %3 = bitcast i64* %2 to i8*
  call void @llvm.lifetime.start.p0i8(i64 -1, i8* %3)
  store i64 %0, i64* %2, align 8
  %4 = bitcast i64* %1 to i8*
  call void @llvm.lifetime.start.p0i8(i64 -1, i8* %4)
  store i64 0, i64* %1, align 8
  %5 = load i64, i64* %2, align 8
  %6 = icmp slt i64 %5, 0
  br i1 %6, label %hist.is_less_than_zero, label %hist.is_not_less_than_zero

hist.is_less_than_zero:                           ; preds = %entry
  %7 = load i64, i64* %1, align 8
  ret i64 %7

hist.is_not_less_than_zero:                       ; preds = %entry
  %8 = load i64, i64* %2, align 8
  %9 = icmp eq i64 %8, 0
  br i1 %9, label %hist.is_zero, label %hist.is_not_zero

hist.is_zero:                                     ; preds = %hist.is_not_less_than_zero
  store i64 1, i64* %1, align 8
  %10 = load i64, i64* %1, align 8
  ret i64 %10

hist.is_not_zero:                                 ; preds = %hist.is_not_less_than_zero
  store i64 2, i64* %1, align 8
  %11 = load i64, i64* %2, align 8
  %12 = icmp sge i64 %11, 4294967296
  %13 = zext i1 %12 to i64
  %14 = shl i64 %13, 5
  %15 = lshr i64 %11, %14
  store i64 %15, i64* %2, align 8
  %16 = load i64, i64* %1, align 8
  %17 = add i64 %16, %14
  store i64 %17, i64* %1, align 8
  %18 = load i64, i64* %2, align 8
  %19 = icmp sge i64 %18, 65536
  %20 = zext i1 %19 to i64
  %21 = shl i64 %20, 4
  %22 = lshr i64 %18, %21
  store i64 %22, i64* %2, align 8
  %23 = load i64, i64* %1, align 8
  %24 = add i64 %23, %21
  store i64 %24, i64* %1, align 8
  %25 = load i64, i64* %2, align 8
  %26 = icmp sge i64 %25, 256
  %27 = zext i1 %26 to i64
  %28 = shl i64 %27, 3
  %29 = lshr i64 %25, %28
  store i64 %29, i64* %2, align 8
  %30 = load i64, i64* %1, align 8
  %31 = add i64 %30, %28
  store i64 %31, i64* %1, align 8
  %32 = load i64, i64* %2, align 8
  %33 = icmp sge i64 %32, 16
  %34 = zext i1 %33 to i64
  %35 = shl i64 %34, 2
  %36 = lshr i64 %32, %35
  store i64 %36, i64* %2, align 8
  %37 = load i64, i64* %1, align 8
  %38 = add i64 %37, %35
  store i64 %38, i64* %1, align 8
  %39 = load i64, i64* %2, align 8
  %40 = icmp sge i64 %39, 4
  %41 = zext i1 %40 to i64
  %42 = shl i64 %41, 1
  %43 = lshr i64 %39, %42
  store i64 %43, i64* %2, align 8
  %44 = load i64, i64* %1, align 8
  %45 = add i64 %44, %42
  store i64 %45, i64* %1, align 8
  %46 = load i64, i64* %2, align 8
  %47 = icmp sge i64 %46, 2
  %48 = zext i1 %47 to i64
  %49 = shl i64 %48, 0
  %50 = lshr i64 %46, %49
  store i64 %50, i64* %2, align 8
  %51 = load i64, i64* %1, align 8
  %52 = add i64 %51, %49
  store i64 %52, i64* %1, align 8
  %53 = load i64, i64* %1, align 8
  ret i64 %53
}

; Function Attrs: argmemonly nofree nosync nounwind willreturn
declare void @llvm.lifetime.start.p0i8(i64 immarg %0, i8* nocapture %1) #2

; Function Attrs: argmemonly nofree nosync nounwind willreturn
declare void @llvm.lifetime.end.p0i8(i64 immarg %0, i8* nocapture %1) #2

attributes #0 = { nounwind }
attributes #1 = { alwaysinline }
attributes #2 = { argmemonly nofree nosync nounwind willreturn }
