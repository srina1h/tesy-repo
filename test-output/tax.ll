; ModuleID = 'tests/tax.c'
source_filename = "tests/tax.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.str = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str.1 = private unnamed_addr constant [31 x i8] c"I need a non-negative number: \00", align 1
@.str.2 = private unnamed_addr constant [63 x i8] c"Welcome to the United States 1040 federal income tax program.\0A\00", align 1
@.str.3 = private unnamed_addr constant [65 x i8] c"(Note: this isn't the real 1040 form. If you try to submit your\0A\00", align 1
@.str.4 = private unnamed_addr constant [47 x i8] c"taxes this way, you'll get what you deserve!\0A\0A\00", align 1
@.str.5 = private unnamed_addr constant [60 x i8] c"Answer the following questions to determine what you owe.\0A\0A\00", align 1
@.str.6 = private unnamed_addr constant [32 x i8] c"Total wages, salary, and tips? \00", align 1
@.str.7 = private unnamed_addr constant [48 x i8] c"Taxable interest (such as from bank accounts)? \00", align 1
@.str.8 = private unnamed_addr constant [64 x i8] c"Unemployment compensation, qualified state tuition, and Alaska\0A\00", align 1
@.str.9 = private unnamed_addr constant [27 x i8] c"Permanent Fund dividends? \00", align 1
@.str.10 = private unnamed_addr constant [32 x i8] c"Your adjusted gross income is: \00", align 1
@.str.11 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@.str.12 = private unnamed_addr constant [65 x i8] c"Enter <1> if your parents or someone else can claim you on their\00", align 1
@.str.13 = private unnamed_addr constant [32 x i8] c" return. \0AEnter <0> otherwise: \00", align 1
@.str.14 = private unnamed_addr constant [54 x i8] c"Enter <1> if you are single, <0> if you are married: \00", align 1
@.str.15 = private unnamed_addr constant [57 x i8] c"Enter <1> if your spouse can be claimed as a dependant, \00", align 1
@.str.16 = private unnamed_addr constant [19 x i8] c"enter <0> if not: \00", align 1
@.str.17 = private unnamed_addr constant [25 x i8] c"Your taxable income is: \00", align 1
@.str.18 = private unnamed_addr constant [50 x i8] c"Enter the amount of Federal income tax withheld: \00", align 1
@.str.19 = private unnamed_addr constant [53 x i8] c"Enter <1> if you get an earned income credit (EIC); \00", align 1
@.str.20 = private unnamed_addr constant [20 x i8] c"enter 0 otherwise: \00", align 1
@.str.21 = private unnamed_addr constant [55 x i8] c"OK, I'll give you a thousand dollars for your credit.\0A\00", align 1
@.str.22 = private unnamed_addr constant [36 x i8] c"Your total tax payments amount to: \00", align 1
@.str.23 = private unnamed_addr constant [30 x i8] c"Your total tax liability is: \00", align 1
@.str.24 = private unnamed_addr constant [43 x i8] c"Congratulations, you get a tax refund of $\00", align 1
@.str.25 = private unnamed_addr constant [38 x i8] c"Bummer. You owe the IRS a check for $\00", align 1
@.str.26 = private unnamed_addr constant [29 x i8] c"Thank you for using ez-tax.\0A\00", align 1

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @getinput() #0 !dbg !10 {
  %1 = alloca i32, align 4
  call void @llvm.dbg.declare(metadata i32* %1, metadata !15, metadata !DIExpression()), !dbg !16
  store i32 -1, i32* %1, align 4, !dbg !17
  br label %2, !dbg !18

2:                                                ; preds = %11, %0
  %3 = load i32, i32* %1, align 4, !dbg !19
  %4 = icmp sgt i32 0, %3, !dbg !20
  br i1 %4, label %5, label %12, !dbg !18

5:                                                ; preds = %2
  %6 = call i32 (i8*, ...) @__isoc99_scanf(i8* noundef getelementptr inbounds ([3 x i8], [3 x i8]* @.str, i64 0, i64 0), i32* noundef %1), !dbg !21
  %7 = load i32, i32* %1, align 4, !dbg !23
  %8 = icmp sgt i32 0, %7, !dbg !25
  br i1 %8, label %9, label %11, !dbg !26

9:                                                ; preds = %5
  %10 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([31 x i8], [31 x i8]* @.str.1, i64 0, i64 0)), !dbg !27
  br label %11, !dbg !29

11:                                               ; preds = %9, %5
  br label %2, !dbg !18, !llvm.loop !30

12:                                               ; preds = %2
  %13 = load i32, i32* %1, align 4, !dbg !33
  ret i32 %13, !dbg !34
}

; Function Attrs: nofree nosync nounwind readnone speculatable willreturn
declare void @llvm.dbg.declare(metadata, metadata, metadata) #1

declare i32 @__isoc99_scanf(i8* noundef, ...) #2

declare i32 @printf(i8* noundef, ...) #2

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 !dbg !35 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  %7 = alloca i32, align 4
  %8 = alloca i32, align 4
  %9 = alloca i32, align 4
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  %13 = alloca i32, align 4
  %14 = alloca i32, align 4
  %15 = alloca i32, align 4
  %16 = alloca i32, align 4
  %17 = alloca i32, align 4
  %18 = alloca i32, align 4
  %19 = alloca i32, align 4
  %20 = alloca i32, align 4
  %21 = alloca i32, align 4
  %22 = alloca i32, align 4
  %23 = alloca i32, align 4
  %24 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  call void @llvm.dbg.declare(metadata i32* %2, metadata !36, metadata !DIExpression()), !dbg !37
  call void @llvm.dbg.declare(metadata i32* %3, metadata !38, metadata !DIExpression()), !dbg !39
  call void @llvm.dbg.declare(metadata i32* %4, metadata !40, metadata !DIExpression()), !dbg !41
  call void @llvm.dbg.declare(metadata i32* %5, metadata !42, metadata !DIExpression()), !dbg !43
  call void @llvm.dbg.declare(metadata i32* %6, metadata !44, metadata !DIExpression()), !dbg !45
  call void @llvm.dbg.declare(metadata i32* %7, metadata !46, metadata !DIExpression()), !dbg !47
  call void @llvm.dbg.declare(metadata i32* %8, metadata !48, metadata !DIExpression()), !dbg !49
  call void @llvm.dbg.declare(metadata i32* %9, metadata !50, metadata !DIExpression()), !dbg !51
  call void @llvm.dbg.declare(metadata i32* %10, metadata !52, metadata !DIExpression()), !dbg !53
  call void @llvm.dbg.declare(metadata i32* %11, metadata !54, metadata !DIExpression()), !dbg !55
  call void @llvm.dbg.declare(metadata i32* %12, metadata !56, metadata !DIExpression()), !dbg !57
  call void @llvm.dbg.declare(metadata i32* %13, metadata !58, metadata !DIExpression()), !dbg !59
  call void @llvm.dbg.declare(metadata i32* %14, metadata !60, metadata !DIExpression()), !dbg !61
  call void @llvm.dbg.declare(metadata i32* %15, metadata !62, metadata !DIExpression()), !dbg !63
  call void @llvm.dbg.declare(metadata i32* %16, metadata !64, metadata !DIExpression()), !dbg !65
  call void @llvm.dbg.declare(metadata i32* %17, metadata !66, metadata !DIExpression()), !dbg !67
  call void @llvm.dbg.declare(metadata i32* %18, metadata !68, metadata !DIExpression()), !dbg !69
  call void @llvm.dbg.declare(metadata i32* %19, metadata !70, metadata !DIExpression()), !dbg !71
  call void @llvm.dbg.declare(metadata i32* %20, metadata !72, metadata !DIExpression()), !dbg !73
  call void @llvm.dbg.declare(metadata i32* %21, metadata !74, metadata !DIExpression()), !dbg !75
  call void @llvm.dbg.declare(metadata i32* %22, metadata !76, metadata !DIExpression()), !dbg !77
  call void @llvm.dbg.declare(metadata i32* %23, metadata !78, metadata !DIExpression()), !dbg !79
  call void @llvm.dbg.declare(metadata i32* %24, metadata !80, metadata !DIExpression()), !dbg !81
  %25 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([63 x i8], [63 x i8]* @.str.2, i64 0, i64 0)), !dbg !82
  %26 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([65 x i8], [65 x i8]* @.str.3, i64 0, i64 0)), !dbg !83
  %27 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([47 x i8], [47 x i8]* @.str.4, i64 0, i64 0)), !dbg !84
  %28 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([60 x i8], [60 x i8]* @.str.5, i64 0, i64 0)), !dbg !85
  %29 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([32 x i8], [32 x i8]* @.str.6, i64 0, i64 0)), !dbg !86
  %30 = call i32 @getinput(), !dbg !87
  store i32 %30, i32* %2, align 4, !dbg !88
  %31 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([48 x i8], [48 x i8]* @.str.7, i64 0, i64 0)), !dbg !89
  %32 = call i32 @getinput(), !dbg !90
  store i32 %32, i32* %3, align 4, !dbg !91
  %33 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([64 x i8], [64 x i8]* @.str.8, i64 0, i64 0)), !dbg !92
  %34 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([27 x i8], [27 x i8]* @.str.9, i64 0, i64 0)), !dbg !93
  %35 = call i32 @getinput(), !dbg !94
  store i32 %35, i32* %4, align 4, !dbg !95
  %36 = load i32, i32* %2, align 4, !dbg !96
  %37 = load i32, i32* %3, align 4, !dbg !97
  %38 = add nsw i32 %36, %37, !dbg !98
  %39 = load i32, i32* %4, align 4, !dbg !99
  %40 = add nsw i32 %38, %39, !dbg !100
  store i32 %40, i32* %5, align 4, !dbg !101
  %41 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([32 x i8], [32 x i8]* @.str.10, i64 0, i64 0)), !dbg !102
  %42 = load i32, i32* %5, align 4, !dbg !103
  %43 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.11, i64 0, i64 0), i32 noundef %42), !dbg !103
  %44 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([65 x i8], [65 x i8]* @.str.12, i64 0, i64 0)), !dbg !104
  %45 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([32 x i8], [32 x i8]* @.str.13, i64 0, i64 0)), !dbg !105
  %46 = call i32 @getinput(), !dbg !106
  store i32 %46, i32* %14, align 4, !dbg !107
  %47 = load i32, i32* %14, align 4, !dbg !108
  %48 = icmp ne i32 0, %47, !dbg !110
  br i1 %48, label %49, label %91, !dbg !111

49:                                               ; preds = %0
  %50 = load i32, i32* %2, align 4, !dbg !112
  %51 = add nsw i32 %50, 250, !dbg !114
  store i32 %51, i32* %16, align 4, !dbg !115
  store i32 700, i32* %17, align 4, !dbg !116
  %52 = load i32, i32* %17, align 4, !dbg !117
  store i32 %52, i32* %18, align 4, !dbg !118
  %53 = load i32, i32* %18, align 4, !dbg !119
  %54 = load i32, i32* %16, align 4, !dbg !121
  %55 = icmp slt i32 %53, %54, !dbg !122
  br i1 %55, label %56, label %58, !dbg !123

56:                                               ; preds = %49
  %57 = load i32, i32* %16, align 4, !dbg !124
  store i32 %57, i32* %18, align 4, !dbg !126
  br label %58, !dbg !127

58:                                               ; preds = %56, %49
  %59 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([54 x i8], [54 x i8]* @.str.14, i64 0, i64 0)), !dbg !128
  %60 = call i32 @getinput(), !dbg !129
  store i32 %60, i32* %15, align 4, !dbg !130
  %61 = load i32, i32* %15, align 4, !dbg !131
  %62 = icmp ne i32 0, %61, !dbg !133
  br i1 %62, label %63, label %64, !dbg !134

63:                                               ; preds = %58
  store i32 7350, i32* %19, align 4, !dbg !135
  br label %64, !dbg !137

64:                                               ; preds = %63, %58
  %65 = load i32, i32* %15, align 4, !dbg !138
  %66 = icmp eq i32 0, %65, !dbg !140
  br i1 %66, label %67, label %68, !dbg !141

67:                                               ; preds = %64
  store i32 4400, i32* %19, align 4, !dbg !142
  br label %68, !dbg !144

68:                                               ; preds = %67, %64
  %69 = load i32, i32* %18, align 4, !dbg !145
  store i32 %69, i32* %20, align 4, !dbg !146
  %70 = load i32, i32* %20, align 4, !dbg !147
  %71 = load i32, i32* %19, align 4, !dbg !149
  %72 = icmp sgt i32 %70, %71, !dbg !150
  br i1 %72, label %73, label %75, !dbg !151

73:                                               ; preds = %68
  %74 = load i32, i32* %19, align 4, !dbg !152
  store i32 %74, i32* %20, align 4, !dbg !154
  br label %75, !dbg !155

75:                                               ; preds = %73, %68
  store i32 0, i32* %21, align 4, !dbg !156
  %76 = load i32, i32* %15, align 4, !dbg !157
  %77 = icmp eq i32 %76, 0, !dbg !159
  br i1 %77, label %78, label %86, !dbg !160

78:                                               ; preds = %75
  %79 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([57 x i8], [57 x i8]* @.str.15, i64 0, i64 0)), !dbg !161
  %80 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([19 x i8], [19 x i8]* @.str.16, i64 0, i64 0)), !dbg !163
  %81 = call i32 @getinput(), !dbg !164
  store i32 %81, i32* %24, align 4, !dbg !165
  %82 = load i32, i32* %24, align 4, !dbg !166
  %83 = icmp eq i32 0, %82, !dbg !168
  br i1 %83, label %84, label %85, !dbg !169

84:                                               ; preds = %78
  store i32 2800, i32* %21, align 4, !dbg !170
  br label %85, !dbg !172

85:                                               ; preds = %84, %78
  br label %86, !dbg !173

86:                                               ; preds = %85, %75
  %87 = load i32, i32* %20, align 4, !dbg !174
  %88 = load i32, i32* %21, align 4, !dbg !175
  %89 = add nsw i32 %87, %88, !dbg !176
  store i32 %89, i32* %22, align 4, !dbg !177
  %90 = load i32, i32* %22, align 4, !dbg !178
  store i32 %90, i32* %6, align 4, !dbg !179
  br label %91, !dbg !180

91:                                               ; preds = %86, %0
  %92 = load i32, i32* %14, align 4, !dbg !181
  %93 = icmp eq i32 0, %92, !dbg !183
  br i1 %93, label %94, label %105, !dbg !184

94:                                               ; preds = %91
  %95 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([54 x i8], [54 x i8]* @.str.14, i64 0, i64 0)), !dbg !185
  %96 = call i32 @getinput(), !dbg !187
  store i32 %96, i32* %15, align 4, !dbg !188
  %97 = load i32, i32* %15, align 4, !dbg !189
  %98 = icmp ne i32 0, %97, !dbg !191
  br i1 %98, label %99, label %100, !dbg !192

99:                                               ; preds = %94
  store i32 12950, i32* %6, align 4, !dbg !193
  br label %100, !dbg !195

100:                                              ; preds = %99, %94
  %101 = load i32, i32* %15, align 4, !dbg !196
  %102 = icmp eq i32 0, %101, !dbg !198
  br i1 %102, label %103, label %104, !dbg !199

103:                                              ; preds = %100
  store i32 7200, i32* %6, align 4, !dbg !200
  br label %104, !dbg !202

104:                                              ; preds = %103, %100
  br label %105, !dbg !203

105:                                              ; preds = %104, %91
  %106 = load i32, i32* %5, align 4, !dbg !204
  %107 = load i32, i32* %6, align 4, !dbg !205
  %108 = sub nsw i32 %106, %107, !dbg !206
  store i32 %108, i32* %7, align 4, !dbg !207
  %109 = load i32, i32* %7, align 4, !dbg !208
  %110 = icmp slt i32 %109, 0, !dbg !210
  br i1 %110, label %111, label %112, !dbg !211

111:                                              ; preds = %105
  store i32 0, i32* %7, align 4, !dbg !212
  br label %112, !dbg !214

112:                                              ; preds = %111, %105
  %113 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([25 x i8], [25 x i8]* @.str.17, i64 0, i64 0)), !dbg !215
  %114 = load i32, i32* %7, align 4, !dbg !216
  %115 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.11, i64 0, i64 0), i32 noundef %114), !dbg !216
  %116 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([50 x i8], [50 x i8]* @.str.18, i64 0, i64 0)), !dbg !217
  %117 = call i32 @getinput(), !dbg !218
  store i32 %117, i32* %8, align 4, !dbg !219
  %118 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([53 x i8], [53 x i8]* @.str.19, i64 0, i64 0)), !dbg !220
  %119 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([20 x i8], [20 x i8]* @.str.20, i64 0, i64 0)), !dbg !221
  %120 = call i32 @getinput(), !dbg !222
  store i32 %120, i32* %23, align 4, !dbg !223
  store i32 0, i32* %9, align 4, !dbg !224
  %121 = load i32, i32* %23, align 4, !dbg !225
  %122 = icmp ne i32 0, %121, !dbg !227
  br i1 %122, label %123, label %125, !dbg !228

123:                                              ; preds = %112
  %124 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([55 x i8], [55 x i8]* @.str.21, i64 0, i64 0)), !dbg !229
  store i32 1000, i32* %9, align 4, !dbg !231
  br label %125, !dbg !232

125:                                              ; preds = %123, %112
  %126 = load i32, i32* %9, align 4, !dbg !233
  %127 = load i32, i32* %8, align 4, !dbg !234
  %128 = add nsw i32 %126, %127, !dbg !235
  store i32 %128, i32* %10, align 4, !dbg !236
  %129 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([36 x i8], [36 x i8]* @.str.22, i64 0, i64 0)), !dbg !237
  %130 = load i32, i32* %10, align 4, !dbg !238
  %131 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.11, i64 0, i64 0), i32 noundef %130), !dbg !238
  %132 = load i32, i32* %7, align 4, !dbg !239
  %133 = mul nsw i32 %132, 28, !dbg !240
  %134 = add nsw i32 %133, 50, !dbg !241
  %135 = sdiv i32 %134, 100, !dbg !242
  store i32 %135, i32* %11, align 4, !dbg !243
  %136 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([30 x i8], [30 x i8]* @.str.23, i64 0, i64 0)), !dbg !244
  %137 = load i32, i32* %11, align 4, !dbg !245
  %138 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.11, i64 0, i64 0), i32 noundef %137), !dbg !245
  %139 = load i32, i32* %10, align 4, !dbg !246
  %140 = load i32, i32* %11, align 4, !dbg !247
  %141 = sub nsw i32 %139, %140, !dbg !248
  store i32 %141, i32* %12, align 4, !dbg !249
  %142 = load i32, i32* %12, align 4, !dbg !250
  %143 = icmp slt i32 %142, 0, !dbg !252
  br i1 %143, label %144, label %145, !dbg !253

144:                                              ; preds = %125
  store i32 0, i32* %12, align 4, !dbg !254
  br label %145, !dbg !256

145:                                              ; preds = %144, %125
  %146 = load i32, i32* %12, align 4, !dbg !257
  %147 = icmp sgt i32 %146, 0, !dbg !259
  br i1 %147, label %148, label %152, !dbg !260

148:                                              ; preds = %145
  %149 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([43 x i8], [43 x i8]* @.str.24, i64 0, i64 0)), !dbg !261
  %150 = load i32, i32* %12, align 4, !dbg !263
  %151 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.11, i64 0, i64 0), i32 noundef %150), !dbg !263
  br label %152, !dbg !264

152:                                              ; preds = %148, %145
  %153 = load i32, i32* %11, align 4, !dbg !265
  %154 = load i32, i32* %10, align 4, !dbg !266
  %155 = sub nsw i32 %153, %154, !dbg !267
  store i32 %155, i32* %13, align 4, !dbg !268
  %156 = load i32, i32* %13, align 4, !dbg !269
  %157 = icmp sge i32 %156, 0, !dbg !271
  br i1 %157, label %158, label %162, !dbg !272

158:                                              ; preds = %152
  %159 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([38 x i8], [38 x i8]* @.str.25, i64 0, i64 0)), !dbg !273
  %160 = load i32, i32* %13, align 4, !dbg !275
  %161 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([4 x i8], [4 x i8]* @.str.11, i64 0, i64 0), i32 noundef %160), !dbg !275
  br label %162, !dbg !276

162:                                              ; preds = %158, %152
  %163 = load i32, i32* %13, align 4, !dbg !277
  %164 = icmp slt i32 %163, 0, !dbg !279
  br i1 %164, label %165, label %166, !dbg !280

165:                                              ; preds = %162
  store i32 0, i32* %13, align 4, !dbg !281
  br label %166, !dbg !283

166:                                              ; preds = %165, %162
  %167 = call i32 (i8*, ...) @printf(i8* noundef getelementptr inbounds ([29 x i8], [29 x i8]* @.str.26, i64 0, i64 0)), !dbg !284
  %168 = load i32, i32* %1, align 4, !dbg !285
  ret i32 %168, !dbg !285
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }
attributes #1 = { nofree nosync nounwind readnone speculatable willreturn }
attributes #2 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!2, !3, !4, !5, !6, !7, !8}
!llvm.ident = !{!9}

!0 = distinct !DICompileUnit(language: DW_LANG_C99, file: !1, producer: "Ubuntu clang version 14.0.0-1ubuntu1.1", isOptimized: false, runtimeVersion: 0, emissionKind: FullDebug, splitDebugInlining: false, nameTableKind: None)
!1 = !DIFile(filename: "tests/tax.c", directory: "/home/spurush/project/CSC512-Part2-Sub", checksumkind: CSK_MD5, checksum: "724166c919dd0006d021e0b1aea84166")
!2 = !{i32 7, !"Dwarf Version", i32 5}
!3 = !{i32 2, !"Debug Info Version", i32 3}
!4 = !{i32 1, !"wchar_size", i32 4}
!5 = !{i32 7, !"PIC Level", i32 2}
!6 = !{i32 7, !"PIE Level", i32 2}
!7 = !{i32 7, !"uwtable", i32 1}
!8 = !{i32 7, !"frame-pointer", i32 2}
!9 = !{!"Ubuntu clang version 14.0.0-1ubuntu1.1"}
!10 = distinct !DISubprogram(name: "getinput", scope: !1, file: !1, line: 6, type: !11, scopeLine: 7, flags: DIFlagPrototyped, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!11 = !DISubroutineType(types: !12)
!12 = !{!13}
!13 = !DIBasicType(name: "int", size: 32, encoding: DW_ATE_signed)
!14 = !{}
!15 = !DILocalVariable(name: "a", scope: !10, file: !1, line: 8, type: !13)
!16 = !DILocation(line: 8, column: 9, scope: !10)
!17 = !DILocation(line: 9, column: 7, scope: !10)
!18 = !DILocation(line: 10, column: 5, scope: !10)
!19 = !DILocation(line: 10, column: 16, scope: !10)
!20 = !DILocation(line: 10, column: 14, scope: !10)
!21 = !DILocation(line: 12, column: 2, scope: !22)
!22 = distinct !DILexicalBlock(scope: !10, file: !1, line: 11, column: 5)
!23 = !DILocation(line: 13, column: 10, scope: !24)
!24 = distinct !DILexicalBlock(scope: !22, file: !1, line: 13, column: 6)
!25 = !DILocation(line: 13, column: 8, scope: !24)
!26 = !DILocation(line: 13, column: 6, scope: !22)
!27 = !DILocation(line: 15, column: 6, scope: !28)
!28 = distinct !DILexicalBlock(scope: !24, file: !1, line: 14, column: 2)
!29 = !DILocation(line: 16, column: 2, scope: !28)
!30 = distinct !{!30, !18, !31, !32}
!31 = !DILocation(line: 17, column: 5, scope: !10)
!32 = !{!"llvm.loop.mustprogress"}
!33 = !DILocation(line: 19, column: 12, scope: !10)
!34 = !DILocation(line: 19, column: 5, scope: !10)
!35 = distinct !DISubprogram(name: "main", scope: !1, file: !1, line: 22, type: !11, scopeLine: 23, spFlags: DISPFlagDefinition, unit: !0, retainedNodes: !14)
!36 = !DILocalVariable(name: "line1", scope: !35, file: !1, line: 24, type: !13)
!37 = !DILocation(line: 24, column: 9, scope: !35)
!38 = !DILocalVariable(name: "line2", scope: !35, file: !1, line: 24, type: !13)
!39 = !DILocation(line: 24, column: 16, scope: !35)
!40 = !DILocalVariable(name: "line3", scope: !35, file: !1, line: 24, type: !13)
!41 = !DILocation(line: 24, column: 23, scope: !35)
!42 = !DILocalVariable(name: "line4", scope: !35, file: !1, line: 24, type: !13)
!43 = !DILocation(line: 24, column: 30, scope: !35)
!44 = !DILocalVariable(name: "line5", scope: !35, file: !1, line: 24, type: !13)
!45 = !DILocation(line: 24, column: 37, scope: !35)
!46 = !DILocalVariable(name: "line6", scope: !35, file: !1, line: 24, type: !13)
!47 = !DILocation(line: 24, column: 44, scope: !35)
!48 = !DILocalVariable(name: "line7", scope: !35, file: !1, line: 24, type: !13)
!49 = !DILocation(line: 24, column: 51, scope: !35)
!50 = !DILocalVariable(name: "line8", scope: !35, file: !1, line: 24, type: !13)
!51 = !DILocation(line: 24, column: 58, scope: !35)
!52 = !DILocalVariable(name: "line9", scope: !35, file: !1, line: 24, type: !13)
!53 = !DILocation(line: 24, column: 65, scope: !35)
!54 = !DILocalVariable(name: "line10", scope: !35, file: !1, line: 25, type: !13)
!55 = !DILocation(line: 25, column: 2, scope: !35)
!56 = !DILocalVariable(name: "line11", scope: !35, file: !1, line: 25, type: !13)
!57 = !DILocation(line: 25, column: 10, scope: !35)
!58 = !DILocalVariable(name: "line12", scope: !35, file: !1, line: 25, type: !13)
!59 = !DILocation(line: 25, column: 18, scope: !35)
!60 = !DILocalVariable(name: "dependant", scope: !35, file: !1, line: 25, type: !13)
!61 = !DILocation(line: 25, column: 26, scope: !35)
!62 = !DILocalVariable(name: "single", scope: !35, file: !1, line: 25, type: !13)
!63 = !DILocation(line: 25, column: 37, scope: !35)
!64 = !DILocalVariable(name: "a", scope: !35, file: !1, line: 25, type: !13)
!65 = !DILocation(line: 25, column: 45, scope: !35)
!66 = !DILocalVariable(name: "b", scope: !35, file: !1, line: 25, type: !13)
!67 = !DILocation(line: 25, column: 48, scope: !35)
!68 = !DILocalVariable(name: "c", scope: !35, file: !1, line: 25, type: !13)
!69 = !DILocation(line: 25, column: 51, scope: !35)
!70 = !DILocalVariable(name: "d", scope: !35, file: !1, line: 25, type: !13)
!71 = !DILocation(line: 25, column: 54, scope: !35)
!72 = !DILocalVariable(name: "e", scope: !35, file: !1, line: 25, type: !13)
!73 = !DILocation(line: 25, column: 57, scope: !35)
!74 = !DILocalVariable(name: "f", scope: !35, file: !1, line: 25, type: !13)
!75 = !DILocation(line: 25, column: 60, scope: !35)
!76 = !DILocalVariable(name: "g", scope: !35, file: !1, line: 25, type: !13)
!77 = !DILocation(line: 25, column: 63, scope: !35)
!78 = !DILocalVariable(name: "eic", scope: !35, file: !1, line: 26, type: !13)
!79 = !DILocation(line: 26, column: 2, scope: !35)
!80 = !DILocalVariable(name: "spousedependant", scope: !35, file: !1, line: 26, type: !13)
!81 = !DILocation(line: 26, column: 7, scope: !35)
!82 = !DILocation(line: 28, column: 5, scope: !35)
!83 = !DILocation(line: 29, column: 5, scope: !35)
!84 = !DILocation(line: 30, column: 5, scope: !35)
!85 = !DILocation(line: 32, column: 5, scope: !35)
!86 = !DILocation(line: 34, column: 5, scope: !35)
!87 = !DILocation(line: 35, column: 13, scope: !35)
!88 = !DILocation(line: 35, column: 11, scope: !35)
!89 = !DILocation(line: 36, column: 5, scope: !35)
!90 = !DILocation(line: 37, column: 13, scope: !35)
!91 = !DILocation(line: 37, column: 11, scope: !35)
!92 = !DILocation(line: 38, column: 5, scope: !35)
!93 = !DILocation(line: 39, column: 5, scope: !35)
!94 = !DILocation(line: 40, column: 13, scope: !35)
!95 = !DILocation(line: 40, column: 11, scope: !35)
!96 = !DILocation(line: 41, column: 13, scope: !35)
!97 = !DILocation(line: 41, column: 19, scope: !35)
!98 = !DILocation(line: 41, column: 18, scope: !35)
!99 = !DILocation(line: 41, column: 25, scope: !35)
!100 = !DILocation(line: 41, column: 24, scope: !35)
!101 = !DILocation(line: 41, column: 11, scope: !35)
!102 = !DILocation(line: 42, column: 5, scope: !35)
!103 = !DILocation(line: 43, column: 5, scope: !35)
!104 = !DILocation(line: 45, column: 5, scope: !35)
!105 = !DILocation(line: 46, column: 5, scope: !35)
!106 = !DILocation(line: 47, column: 17, scope: !35)
!107 = !DILocation(line: 47, column: 15, scope: !35)
!108 = !DILocation(line: 48, column: 14, scope: !109)
!109 = distinct !DILexicalBlock(scope: !35, file: !1, line: 48, column: 9)
!110 = !DILocation(line: 48, column: 11, scope: !109)
!111 = !DILocation(line: 48, column: 9, scope: !35)
!112 = !DILocation(line: 50, column: 6, scope: !113)
!113 = distinct !DILexicalBlock(scope: !109, file: !1, line: 49, column: 5)
!114 = !DILocation(line: 50, column: 12, scope: !113)
!115 = !DILocation(line: 50, column: 4, scope: !113)
!116 = !DILocation(line: 51, column: 4, scope: !113)
!117 = !DILocation(line: 52, column: 6, scope: !113)
!118 = !DILocation(line: 52, column: 4, scope: !113)
!119 = !DILocation(line: 53, column: 6, scope: !120)
!120 = distinct !DILexicalBlock(scope: !113, file: !1, line: 53, column: 6)
!121 = !DILocation(line: 53, column: 10, scope: !120)
!122 = !DILocation(line: 53, column: 8, scope: !120)
!123 = !DILocation(line: 53, column: 6, scope: !113)
!124 = !DILocation(line: 55, column: 10, scope: !125)
!125 = distinct !DILexicalBlock(scope: !120, file: !1, line: 54, column: 2)
!126 = !DILocation(line: 55, column: 8, scope: !125)
!127 = !DILocation(line: 56, column: 2, scope: !125)
!128 = !DILocation(line: 57, column: 2, scope: !113)
!129 = !DILocation(line: 58, column: 11, scope: !113)
!130 = !DILocation(line: 58, column: 9, scope: !113)
!131 = !DILocation(line: 59, column: 11, scope: !132)
!132 = distinct !DILexicalBlock(scope: !113, file: !1, line: 59, column: 6)
!133 = !DILocation(line: 59, column: 8, scope: !132)
!134 = !DILocation(line: 59, column: 6, scope: !113)
!135 = !DILocation(line: 61, column: 8, scope: !136)
!136 = distinct !DILexicalBlock(scope: !132, file: !1, line: 60, column: 2)
!137 = !DILocation(line: 62, column: 2, scope: !136)
!138 = !DILocation(line: 63, column: 11, scope: !139)
!139 = distinct !DILexicalBlock(scope: !113, file: !1, line: 63, column: 6)
!140 = !DILocation(line: 63, column: 8, scope: !139)
!141 = !DILocation(line: 63, column: 6, scope: !113)
!142 = !DILocation(line: 65, column: 8, scope: !143)
!143 = distinct !DILexicalBlock(scope: !139, file: !1, line: 64, column: 2)
!144 = !DILocation(line: 66, column: 2, scope: !143)
!145 = !DILocation(line: 67, column: 6, scope: !113)
!146 = !DILocation(line: 67, column: 4, scope: !113)
!147 = !DILocation(line: 68, column: 6, scope: !148)
!148 = distinct !DILexicalBlock(scope: !113, file: !1, line: 68, column: 6)
!149 = !DILocation(line: 68, column: 10, scope: !148)
!150 = !DILocation(line: 68, column: 8, scope: !148)
!151 = !DILocation(line: 68, column: 6, scope: !113)
!152 = !DILocation(line: 70, column: 10, scope: !153)
!153 = distinct !DILexicalBlock(scope: !148, file: !1, line: 69, column: 2)
!154 = !DILocation(line: 70, column: 8, scope: !153)
!155 = !DILocation(line: 71, column: 2, scope: !153)
!156 = !DILocation(line: 72, column: 4, scope: !113)
!157 = !DILocation(line: 73, column: 6, scope: !158)
!158 = distinct !DILexicalBlock(scope: !113, file: !1, line: 73, column: 6)
!159 = !DILocation(line: 73, column: 13, scope: !158)
!160 = !DILocation(line: 73, column: 6, scope: !113)
!161 = !DILocation(line: 75, column: 6, scope: !162)
!162 = distinct !DILexicalBlock(scope: !158, file: !1, line: 74, column: 2)
!163 = !DILocation(line: 76, column: 6, scope: !162)
!164 = !DILocation(line: 77, column: 24, scope: !162)
!165 = !DILocation(line: 77, column: 22, scope: !162)
!166 = !DILocation(line: 78, column: 15, scope: !167)
!167 = distinct !DILexicalBlock(scope: !162, file: !1, line: 78, column: 10)
!168 = !DILocation(line: 78, column: 12, scope: !167)
!169 = !DILocation(line: 78, column: 10, scope: !162)
!170 = !DILocation(line: 80, column: 5, scope: !171)
!171 = distinct !DILexicalBlock(scope: !167, file: !1, line: 79, column: 6)
!172 = !DILocation(line: 81, column: 6, scope: !171)
!173 = !DILocation(line: 82, column: 2, scope: !162)
!174 = !DILocation(line: 83, column: 6, scope: !113)
!175 = !DILocation(line: 83, column: 10, scope: !113)
!176 = !DILocation(line: 83, column: 8, scope: !113)
!177 = !DILocation(line: 83, column: 4, scope: !113)
!178 = !DILocation(line: 85, column: 10, scope: !113)
!179 = !DILocation(line: 85, column: 8, scope: !113)
!180 = !DILocation(line: 86, column: 5, scope: !113)
!181 = !DILocation(line: 87, column: 14, scope: !182)
!182 = distinct !DILexicalBlock(scope: !35, file: !1, line: 87, column: 9)
!183 = !DILocation(line: 87, column: 11, scope: !182)
!184 = !DILocation(line: 87, column: 9, scope: !35)
!185 = !DILocation(line: 89, column: 2, scope: !186)
!186 = distinct !DILexicalBlock(scope: !182, file: !1, line: 88, column: 5)
!187 = !DILocation(line: 90, column: 11, scope: !186)
!188 = !DILocation(line: 90, column: 9, scope: !186)
!189 = !DILocation(line: 91, column: 11, scope: !190)
!190 = distinct !DILexicalBlock(scope: !186, file: !1, line: 91, column: 6)
!191 = !DILocation(line: 91, column: 8, scope: !190)
!192 = !DILocation(line: 91, column: 6, scope: !186)
!193 = !DILocation(line: 93, column: 12, scope: !194)
!194 = distinct !DILexicalBlock(scope: !190, file: !1, line: 92, column: 2)
!195 = !DILocation(line: 94, column: 2, scope: !194)
!196 = !DILocation(line: 95, column: 11, scope: !197)
!197 = distinct !DILexicalBlock(scope: !186, file: !1, line: 95, column: 6)
!198 = !DILocation(line: 95, column: 8, scope: !197)
!199 = !DILocation(line: 95, column: 6, scope: !186)
!200 = !DILocation(line: 97, column: 12, scope: !201)
!201 = distinct !DILexicalBlock(scope: !197, file: !1, line: 96, column: 2)
!202 = !DILocation(line: 98, column: 2, scope: !201)
!203 = !DILocation(line: 99, column: 5, scope: !186)
!204 = !DILocation(line: 101, column: 13, scope: !35)
!205 = !DILocation(line: 101, column: 21, scope: !35)
!206 = !DILocation(line: 101, column: 19, scope: !35)
!207 = !DILocation(line: 101, column: 11, scope: !35)
!208 = !DILocation(line: 102, column: 9, scope: !209)
!209 = distinct !DILexicalBlock(scope: !35, file: !1, line: 102, column: 9)
!210 = !DILocation(line: 102, column: 15, scope: !209)
!211 = !DILocation(line: 102, column: 9, scope: !35)
!212 = !DILocation(line: 104, column: 8, scope: !213)
!213 = distinct !DILexicalBlock(scope: !209, file: !1, line: 103, column: 5)
!214 = !DILocation(line: 105, column: 5, scope: !213)
!215 = !DILocation(line: 106, column: 5, scope: !35)
!216 = !DILocation(line: 107, column: 5, scope: !35)
!217 = !DILocation(line: 109, column: 5, scope: !35)
!218 = !DILocation(line: 110, column: 13, scope: !35)
!219 = !DILocation(line: 110, column: 11, scope: !35)
!220 = !DILocation(line: 111, column: 5, scope: !35)
!221 = !DILocation(line: 112, column: 5, scope: !35)
!222 = !DILocation(line: 113, column: 11, scope: !35)
!223 = !DILocation(line: 113, column: 9, scope: !35)
!224 = !DILocation(line: 114, column: 11, scope: !35)
!225 = !DILocation(line: 115, column: 14, scope: !226)
!226 = distinct !DILexicalBlock(scope: !35, file: !1, line: 115, column: 9)
!227 = !DILocation(line: 115, column: 11, scope: !226)
!228 = !DILocation(line: 115, column: 9, scope: !35)
!229 = !DILocation(line: 117, column: 2, scope: !230)
!230 = distinct !DILexicalBlock(scope: !226, file: !1, line: 116, column: 5)
!231 = !DILocation(line: 118, column: 8, scope: !230)
!232 = !DILocation(line: 119, column: 5, scope: !230)
!233 = !DILocation(line: 120, column: 13, scope: !35)
!234 = !DILocation(line: 120, column: 21, scope: !35)
!235 = !DILocation(line: 120, column: 19, scope: !35)
!236 = !DILocation(line: 120, column: 11, scope: !35)
!237 = !DILocation(line: 121, column: 5, scope: !35)
!238 = !DILocation(line: 122, column: 5, scope: !35)
!239 = !DILocation(line: 124, column: 15, scope: !35)
!240 = !DILocation(line: 124, column: 21, scope: !35)
!241 = !DILocation(line: 124, column: 26, scope: !35)
!242 = !DILocation(line: 124, column: 32, scope: !35)
!243 = !DILocation(line: 124, column: 12, scope: !35)
!244 = !DILocation(line: 125, column: 5, scope: !35)
!245 = !DILocation(line: 126, column: 5, scope: !35)
!246 = !DILocation(line: 128, column: 14, scope: !35)
!247 = !DILocation(line: 128, column: 22, scope: !35)
!248 = !DILocation(line: 128, column: 20, scope: !35)
!249 = !DILocation(line: 128, column: 12, scope: !35)
!250 = !DILocation(line: 129, column: 9, scope: !251)
!251 = distinct !DILexicalBlock(scope: !35, file: !1, line: 129, column: 9)
!252 = !DILocation(line: 129, column: 16, scope: !251)
!253 = !DILocation(line: 129, column: 9, scope: !35)
!254 = !DILocation(line: 131, column: 9, scope: !255)
!255 = distinct !DILexicalBlock(scope: !251, file: !1, line: 130, column: 5)
!256 = !DILocation(line: 132, column: 5, scope: !255)
!257 = !DILocation(line: 133, column: 9, scope: !258)
!258 = distinct !DILexicalBlock(scope: !35, file: !1, line: 133, column: 9)
!259 = !DILocation(line: 133, column: 16, scope: !258)
!260 = !DILocation(line: 133, column: 9, scope: !35)
!261 = !DILocation(line: 135, column: 2, scope: !262)
!262 = distinct !DILexicalBlock(scope: !258, file: !1, line: 134, column: 5)
!263 = !DILocation(line: 136, column: 2, scope: !262)
!264 = !DILocation(line: 137, column: 5, scope: !262)
!265 = !DILocation(line: 139, column: 14, scope: !35)
!266 = !DILocation(line: 139, column: 21, scope: !35)
!267 = !DILocation(line: 139, column: 20, scope: !35)
!268 = !DILocation(line: 139, column: 12, scope: !35)
!269 = !DILocation(line: 140, column: 9, scope: !270)
!270 = distinct !DILexicalBlock(scope: !35, file: !1, line: 140, column: 9)
!271 = !DILocation(line: 140, column: 16, scope: !270)
!272 = !DILocation(line: 140, column: 9, scope: !35)
!273 = !DILocation(line: 142, column: 2, scope: !274)
!274 = distinct !DILexicalBlock(scope: !270, file: !1, line: 141, column: 5)
!275 = !DILocation(line: 143, column: 2, scope: !274)
!276 = !DILocation(line: 144, column: 5, scope: !274)
!277 = !DILocation(line: 145, column: 9, scope: !278)
!278 = distinct !DILexicalBlock(scope: !35, file: !1, line: 145, column: 9)
!279 = !DILocation(line: 145, column: 16, scope: !278)
!280 = !DILocation(line: 145, column: 9, scope: !35)
!281 = !DILocation(line: 147, column: 9, scope: !282)
!282 = distinct !DILexicalBlock(scope: !278, file: !1, line: 146, column: 5)
!283 = !DILocation(line: 148, column: 5, scope: !282)
!284 = !DILocation(line: 150, column: 5, scope: !35)
!285 = !DILocation(line: 151, column: 1, scope: !35)
