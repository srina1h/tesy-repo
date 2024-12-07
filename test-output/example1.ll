; ModuleID = 'tests/example1.c'
source_filename = "tests/example1.c"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx15.0.0"

@.str = private unnamed_addr constant [24 x i8] c"Enter the value of n: \0A\00", align 1, !dbg !0
@.str.1 = private unnamed_addr constant [3 x i8] c"%d\00", align 1, !dbg !7

; Function Attrs: noinline nounwind optnone ssp uwtable(sync)
define i32 @fun(i32 noundef %0) #0 !dbg !21 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
    #dbg_declare(ptr %2, !26, !DIExpression(), !27)
  %3 = load i32, ptr %2, align 4, !dbg !28
  %4 = add nsw i32 %3, 1, !dbg !29
  ret i32 %4, !dbg !30
}

; Function Attrs: noinline nounwind optnone ssp uwtable(sync)
define i32 @main() #0 !dbg !31 {
  %1 = alloca i32, align 4
  %2 = alloca ptr, align 8
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 0, ptr %1, align 4
    #dbg_declare(ptr %2, !34, !DIExpression(), !36)
  store ptr @fun, ptr %2, align 8, !dbg !36
  %6 = load ptr, ptr %2, align 8, !dbg !37
  %7 = call i32 %6(i32 noundef 9), !dbg !37
    #dbg_declare(ptr %3, !38, !DIExpression(), !39)
  store i32 10, ptr %3, align 4, !dbg !39
    #dbg_declare(ptr %4, !40, !DIExpression(), !41)
  %8 = call i32 (ptr, ...) @printf(ptr noundef @.str), !dbg !42
  %9 = call i32 (ptr, ...) @scanf(ptr noundef @.str.1, ptr noundef %4), !dbg !43
    #dbg_declare(ptr %5, !44, !DIExpression(), !46)
  store i32 0, ptr %5, align 4, !dbg !46
  br label %10, !dbg !47

10:                                               ; preds = %17, %0
  %11 = load i32, ptr %5, align 4, !dbg !48
  %12 = load i32, ptr %4, align 4, !dbg !50
  %13 = icmp slt i32 %11, %12, !dbg !51
  br i1 %13, label %14, label %20, !dbg !52

14:                                               ; preds = %10
  %15 = load i32, ptr %3, align 4, !dbg !53
  %16 = add nsw i32 %15, 1, !dbg !55
  store i32 %16, ptr %3, align 4, !dbg !56
  br label %17, !dbg !57

17:                                               ; preds = %14
  %18 = load i32, ptr %5, align 4, !dbg !58
  %19 = add nsw i32 %18, 1, !dbg !58
  store i32 %19, ptr %5, align 4, !dbg !58
  br label %10, !dbg !59, !llvm.loop !60

20:                                               ; preds = %10
  %21 = load i32, ptr %3, align 4, !dbg !63
  ret i32 %21, !dbg !64
}

declare i32 @printf(ptr noundef, ...) #1

declare i32 @scanf(ptr noundef, ...) #1

attributes #0 = { noinline nounwind optnone ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a,+zcm,+zcz" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a,+zcm,+zcz" }

!llvm.dbg.cu = !{!12}
!llvm.module.flags = !{!14, !15, !16, !17, !18, !19}
!llvm.ident = !{!20}

!0 = !DIGlobalVariableExpression(var: !1, expr: !DIExpression())
!1 = distinct !DIGlobalVariable(scope: null, file: !2, line: 11, type: !3, isLocal: true, isDefinition: true)
!2 = !DIFile(filename: "tests/example1.c", directory: "/Users/srinath/Downloads/spurush/repositories/CSC512-Part2-Sub/repository", checksumkind: CSK_MD5, checksum: "fac7859bccd5d87f624d3da676f251e3")
!3 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 192, elements: !5)
!4 = !DIBasicType(name: "char", size: 8, encoding: DW_ATE_signed_char)
!5 = !{!6}
!6 = !DISubrange(count: 24)
!7 = !DIGlobalVariableExpression(var: !8, expr: !DIExpression())
!8 = distinct !DIGlobalVariable(scope: null, file: !2, line: 12, type: !9, isLocal: true, isDefinition: true)
!9 = !DICompositeType(tag: DW_TAG_array_type, baseType: !4, size: 24, elements: !10)
!10 = !{!11}
!11 = !DISubrange(count: 3)
!12 = distinct !DICompileUnit(language: DW_LANG_C11, file: !2, producer: "Homebrew clang version 19.1.3", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, globals: !13, splitDebugInlining: false, nameTableKind: Apple, sysroot: "/Library/Developer/CommandLineTools/SDKs/MacOSX15.sdk", sdk: "MacOSX15.sdk")
!13 = !{!0, !7}
!14 = !{i32 7, !"Dwarf Version", i32 5}
!15 = !{i32 2, !"Debug Info Version", i32 3}
!16 = !{i32 1, !"wchar_size", i32 4}
!17 = !{i32 8, !"PIC Level", i32 2}
!18 = !{i32 7, !"uwtable", i32 1}
!19 = !{i32 7, !"frame-pointer", i32 1}
!20 = !{!"Homebrew clang version 19.1.3"}
!21 = distinct !DISubprogram(name: "fun", scope: !2, file: !2, line: 3, type: !22, scopeLine: 3, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !12, retainedNodes: !25)
!22 = !DISubroutineType(types: !23)
!23 = !{!24, !24}
!24 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!25 = !{}
!26 = !DILocalVariable(name: "temp", arg: 1, scope: !21, file: !2, line: 3, type: !24)
!27 = !DILocation(line: 3, column: 14, scope: !21)
!28 = !DILocation(line: 4, column: 12, scope: !21)
!29 = !DILocation(line: 4, column: 17, scope: !21)
!30 = !DILocation(line: 4, column: 5, scope: !21)
!31 = distinct !DISubprogram(name: "main", scope: !2, file: !2, line: 6, type: !32, scopeLine: 6, spFlags: DISPFlagDefinition, unit: !12, retainedNodes: !25)
!32 = !DISubroutineType(types: !33)
!33 = !{!24}
!34 = !DILocalVariable(name: "fun_ptr", scope: !31, file: !2, line: 7, type: !35)
!35 = !DIDerivedType(tag: DW_TAG_pointer_type, baseType: !22, size: 64)
!36 = !DILocation(line: 7, column: 11, scope: !31)
!37 = !DILocation(line: 8, column: 5, scope: !31)
!38 = !DILocalVariable(name: "c", scope: !31, file: !2, line: 9, type: !24)
!39 = !DILocation(line: 9, column: 9, scope: !31)
!40 = !DILocalVariable(name: "n", scope: !31, file: !2, line: 10, type: !24)
!41 = !DILocation(line: 10, column: 9, scope: !31)
!42 = !DILocation(line: 11, column: 5, scope: !31)
!43 = !DILocation(line: 12, column: 5, scope: !31)
!44 = !DILocalVariable(name: "i", scope: !45, file: !2, line: 13, type: !24)
!45 = distinct !DILexicalBlock(scope: !31, file: !2, line: 13, column: 5)
!46 = !DILocation(line: 13, column: 14, scope: !45)
!47 = !DILocation(line: 13, column: 10, scope: !45)
!48 = !DILocation(line: 13, column: 19, scope: !49)
!49 = distinct !DILexicalBlock(scope: !45, file: !2, line: 13, column: 5)
!50 = !DILocation(line: 13, column: 21, scope: !49)
!51 = !DILocation(line: 13, column: 20, scope: !49)
!52 = !DILocation(line: 13, column: 5, scope: !45)
!53 = !DILocation(line: 14, column: 14, scope: !54)
!54 = distinct !DILexicalBlock(scope: !49, file: !2, line: 13, column: 28)
!55 = !DILocation(line: 14, column: 16, scope: !54)
!56 = !DILocation(line: 14, column: 12, scope: !54)
!57 = !DILocation(line: 15, column: 5, scope: !54)
!58 = !DILocation(line: 13, column: 25, scope: !49)
!59 = !DILocation(line: 13, column: 5, scope: !49)
!60 = distinct !{!60, !52, !61, !62}
!61 = !DILocation(line: 15, column: 5, scope: !45)
!62 = !{!"llvm.loop.mustprogress"}
!63 = !DILocation(line: 16, column: 12, scope: !31)
!64 = !DILocation(line: 16, column: 5, scope: !31)
