define i32 @main(i32 %argc, i8** %argv) {
 call void @Echo.main.stringarray(i32 %argc, i8** %argv)
 ret i32 0
}

declare i32 @printf(i8*, ...)
declare i32 @atoi(i8*)
@str.1 = private unnamed_addr constant [5 x i8] c"----\00"
@str.2 = private unnamed_addr constant [5 x i8] c"----\00"
@str.3 = private unnamed_addr constant [4 x i8] c"ola\00"
@str.str = private unnamed_addr constant [4 x i8] c"%s\0A\00"
@str.int = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@str.double = private unnamed_addr constant [7 x i8] c"%.16E\0A\00"
@str.true = private unnamed_addr constant [6 x i8] c"true\0A\00"
@str.false = private unnamed_addr constant [7 x i8] c"false\0A\00"

@Echo.c = common global double 0.0
@Echo.counter = common global i1 0

define void @Echo.main.() {
 ret void
 ret void
}

define double @Echo.main.int(i32 %hello.param) {
 %hello = alloca i32
 store i32 %hello.param, i32* %hello
 %a = alloca i32
 %b = alloca i32
 %d = alloca double
 %boo = alloca i1
 store i32 1, i32* %a
 store i32 0, i32* %b
 store double 1.e2, double* @Echo.c
 store double 0.0, double* %d
 store double 1.2, double* @Echo.c
 %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.str, i32 0, i32 0), [5 x i8]* @str.1)
 br label %dowhile.true.1
dowhile.true.1:
 %2 = load i32, i32* %a
 %3 = icmp slt i32 %2, 9
 br label %dowhile.start.1
dowhile.start.1:
 %4 = load i32, i32* %a
 %5 = icmp eq i32 %4, 5
 br i1 %5, label %if.true.2, label %if.false.2
if.true.2:
 %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.str, i32 0, i32 0), [5 x i8]* @str.2)
 %7 = sitofp i32 1 to double
 ret double %7
 br label %if.end.2
if.false.2:
 br label %if.end.2
if.end.2:
 %9 = load i32, i32* %a
 %10 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 %9)
 %11 = load i32, i32* %a
 %12 = add i32 %11, 1
 store i32 %12, i32* %a
 %13 = load i32, i32* %a
 %14 = icmp slt i32 %13, 9
 br i1 %14, label %dowhile.true.1, label %dowhile.end.1
dowhile.end.1:
 store i1 1, i1* %boo
 %15 = call i32 @Echo.cenas.()
 %16 = load i1, i1* %boo
 br i1 %16, label %or.end.1, label %or.false.1
or.false.1:
 br i1 0, label %and.true.2, label %and.end.2
and.true.2:
 br label %and.end.2
and.end.2:
 %17 = phi i1 [0, %and.true.2], [false, %or.false.1]
 br label %or.end.1
or.end.1:
 %18 = phi i1 [%17, %and.end.2], [true, %dowhile.end.1]
 br i1 %18, label %or.end.3, label %or.false.3
or.false.3:
 br label %or.end.3
or.end.3:
 %19 = phi i1 [1, %or.false.3], [true, %or.end.1]
 br i1 %19, label %boolean.true.1, label %boolean.false.1
boolean.true.1:
 %20 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.true, i1 0, i1 0))
 br label %boolean.end.1
boolean.false.1:
 %21 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.false, i1 0, i1 0))
 br label %boolean.end.1
boolean.end.1:
 %22 = load i1, i1* %boo
 br i1 %22, label %or.end.4, label %or.false.4
or.false.4:
 br i1 0, label %and.true.5, label %and.end.5
and.true.5:
 br label %and.end.5
and.end.5:
 %23 = phi i1 [0, %and.true.5], [false, %or.false.4]
 br label %or.end.4
or.end.4:
 %24 = phi i1 [%23, %and.end.5], [true, %boolean.end.1]
 br i1 %24, label %or.end.6, label %or.false.6
or.false.6:
 br label %or.end.6
or.end.6:
 %25 = phi i1 [1, %or.false.6], [true, %or.end.4]
 br i1 %25, label %if.true.3, label %if.false.3
if.true.3:
 %26 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.str, i32 0, i32 0), [4 x i8]* @str.3)
 br label %if.end.3
if.false.3:
 %27 = call i32 @Echo.cenas.()
 br label %if.end.3
if.end.3:
 %28 = sitofp i32 2 to double
 ret double %28
 %30 = load i1, i1* %boo
 br i1 %30, label %or.end.7, label %or.false.7
or.false.7:
 br i1 0, label %and.true.8, label %and.end.8
and.true.8:
 br label %and.end.8
and.end.8:
 %31 = phi i1 [0, %and.true.8], [false, %or.false.7]
 br label %or.end.7
or.end.7:
 %32 = phi i1 [%31, %and.end.8], [true, %29]
 br i1 %32, label %or.end.9, label %or.false.9
or.false.9:
 br label %or.end.9
or.end.9:
 %33 = phi i1 [1, %or.false.9], [true, %or.end.7]
 br i1 %33, label %boolean.true.2, label %boolean.false.2
boolean.true.2:
 %34 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.true, i1 0, i1 0))
 br label %boolean.end.2
boolean.false.2:
 %35 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.false, i1 0, i1 0))
 br label %boolean.end.2
boolean.end.2:
 br label %dowhile.true.4
dowhile.true.4:
 %36 = load i32, i32* %a
 %37 = icmp sge i32 %36, 1
 br label %dowhile.start.4
dowhile.start.4:
 %38 = load double, double* @Echo.c
 %39 = sitofp i32 1 to double
 %40 = fcmp oeq double %38, %39
 br i1 %40, label %if.true.5, label %if.false.5
if.true.5:
 %41 = load double, double* @Echo.c
 %42 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.double, i32 0, i32 0), double %41)
 br label %if.end.5
if.false.5:
 br label %if.end.5
if.end.5:
 %43 = load i32, i32* %a
 %44 = sub i32 %43, 1
 store i32 %44, i32* %a
 %45 = load i32, i32* %a
 %46 = icmp sge i32 %45, 1
 br i1 %46, label %dowhile.true.4, label %dowhile.end.4
dowhile.end.4:
 br label %dowhile.true.6
dowhile.true.6:
 %47 = load i32, i32* %a
 %48 = icmp sge i32 %47, 1
 br label %dowhile.start.6
dowhile.start.6:
 %49 = load i32, i32* %a
 %50 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 %49)
 %51 = load i32, i32* %a
 %52 = sub i32 %51, 1
 store i32 %52, i32* %a
 %53 = load i32, i32* %a
 %54 = icmp sge i32 %53, 1
 br i1 %54, label %dowhile.true.6, label %dowhile.end.6
dowhile.end.6:
 br label %while.start.7
while.start.7:
 %55 = load i32, i32* %a
 %56 = icmp sge i32 %55, 1
 br i1 %56, label %while.true.7, label %while.end.7
while.true.7:
 %57 = load i32, i32* %a
 %58 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 %57)
 %59 = load i32, i32* %a
 %60 = sub i32 %59, 1
 store i32 %60, i32* %a
 br label %while.start.7
while.end.7:
 %61 = load i32, i32* %a
 %62 = icmp eq i32 %61, 1
 br i1 %62, label %if.true.8, label %if.false.8
if.true.8:
 %63 = load i32, i32* %a
 %64 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 %63)
 br label %if.end.8
if.false.8:
 %65 = load i32, i32* %b
 %66 = icmp eq i32 %65, 1
 br i1 %66, label %if.true.9, label %if.false.9
if.true.9:
 %67 = load i32, i32* %b
 %68 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 %67)
 br label %if.end.9
if.false.9:
 %69 = load double, double* %d
 %70 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.double, i32 0, i32 0), double %69)
 br label %if.end.9
if.end.9:
 br label %if.end.8
if.end.8:
 store i1 0, i1* %boo
 %71 = load i32, i32* %a
 %72 = add i32 %71, 1
 %73 = load i32, i32* %b
 %74 = add i32 %72, %73
 %75 = sitofp i32 %74 to double
 store double %75, double* @Echo.c
 br i1 0, label %and.true.10, label %and.end.10
and.true.10:
 %76 = load i32, i32* %a
 %77 = sub i32 %76, 2
 store i32 %77, i32* %a
 %78 = icmp ne i32 %77, 0
 br label %and.end.10
and.end.10:
 %79 = phi i1 [%78, %and.true.10], [false, %if.end.8]
 br i1 %79, label %or.end.11, label %or.false.11
or.false.11:
 %80 = mul i32 -1, 10
 store i32 %80, i32* %b
 %81 = icmp ne i32 %80, 0
 br label %or.end.11
or.end.11:
 %82 = phi i1 [%81, %or.false.11], [true, %and.end.10]
 br i1 %82, label %or.end.12, label %or.false.12
or.false.12:
 store i32 110, i32* %b
 %83 = icmp eq i32 110, 0
 br label %or.end.12
or.end.12:
 %84 = phi i1 [%83, %or.false.12], [true, %or.end.11]
 br i1 %84, label %boolean.true.3, label %boolean.false.3
boolean.true.3:
 %85 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.true, i1 0, i1 0))
 br label %boolean.end.3
boolean.false.3:
 %86 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.false, i1 0, i1 0))
 br label %boolean.end.3
boolean.end.3:
 br i1 0, label %or.end.13, label %or.false.13
or.false.13:
 %87 = mul i32 -1, 10
 store i32 %87, i32* %b
 %88 = icmp ne i32 %87, 0
 br label %or.end.13
or.end.13:
 %89 = phi i1 [%88, %or.false.13], [true, %boolean.end.3]
 br i1 %89, label %or.end.14, label %or.false.14
or.false.14:
 store i32 110, i32* %b
 %90 = icmp eq i32 110, 0
 br label %or.end.14
or.end.14:
 %91 = phi i1 [%90, %or.false.14], [true, %or.end.13]
 br i1 %91, label %boolean.true.4, label %boolean.false.4
boolean.true.4:
 %92 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.true, i1 0, i1 0))
 br label %boolean.end.4
boolean.false.4:
 %93 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.false, i1 0, i1 0))
 br label %boolean.end.4
boolean.end.4:
 %94 = load i32, i32* %a
 %95 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 %94)
 %96 = load double, double* @Echo.c
 %97 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.double, i32 0, i32 0), double %96)
 %98 = load i1, i1* %boo
 br i1 %98, label %and.true.15, label %and.end.15
and.true.15:
 %99 = load i32, i32* %b
 %100 = add i32 %99, 1
 %101 = sitofp i32 %100 to double
 store double %101, double* %d
 %102 = sitofp i32 0 to double
 %103 = fcmp une double %101, %102
 br label %and.end.15
and.end.15:
 %104 = phi i1 [%103, %and.true.15], [false, %boolean.end.4]
 br i1 %104, label %boolean.true.5, label %boolean.false.5
boolean.true.5:
 %105 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.true, i1 0, i1 0))
 br label %boolean.end.5
boolean.false.5:
 %106 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.false, i1 0, i1 0))
 br label %boolean.end.5
boolean.end.5:
 %107 = load double, double* %d
 %108 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.double, i32 0, i32 0), double %107)
 %109 = load i1, i1* %boo
 br i1 %109, label %or.end.16, label %or.false.16
or.false.16:
 br i1 0, label %and.true.17, label %and.end.17
and.true.17:
 br label %and.end.17
and.end.17:
 %110 = phi i1 [0, %and.true.17], [false, %or.false.16]
 br label %or.end.16
or.end.16:
 %111 = phi i1 [%110, %and.end.17], [true, %boolean.end.5]
 br i1 %111, label %or.end.18, label %or.false.18
or.false.18:
 br label %or.end.18
or.end.18:
 %112 = phi i1 [1, %or.false.18], [true, %or.end.16]
 br i1 %112, label %boolean.true.6, label %boolean.false.6
boolean.true.6:
 %113 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.true, i1 0, i1 0))
 br label %boolean.end.6
boolean.false.6:
 %114 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.false, i1 0, i1 0))
 br label %boolean.end.6
boolean.end.6:
 %115 = sitofp i32 1 to double
 ret double %115
 ret double 0.0
}

define void @Echo.main.stringarray(i32 %args.c.param, i8** %args.param) {
 %args.c = alloca i32
 %args = alloca i8**
 store i32 %args.c.param, i32* %args.c
 store i8** %args.param, i8*** %args
 %a = alloca i32
 %b = alloca double
 %1 = call i32 @Echo.cenas.()
 store i32 %1, i32* %a
 %2 = load i32, i32* %a
 %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 %2)
 %4 = load i32, i32* %a
 %5 = call double @Echo.coisas.intint(i32 %4, i32 10)
 store double %5, double* %b
 %6 = load double, double* %b
 %7 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.double, i32 0, i32 0), double %6)
 ret void
 ret void
}

define i32 @Echo.cenas.() {
 ret i32 0
}

define double @Echo.coisas.intint(i32 %a.param, i32 %b.param) {
 %a = alloca i32
 store i32 %a.param, i32* %a
 %b = alloca i32
 store i32 %b.param, i32* %b
 %1 = load i32, i32* %a
 %2 = load i32, i32* %b
 %3 = add i32 %1, %2
 %4 = sitofp i32 %3 to double
 ret double %4
 ret double 0.0
}
