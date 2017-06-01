define i32 @main(i32 %argc, i8** %argv) {
 call void @Echo.main.stringarray(i32 %argc, i8** %argv)
 ret i32 0
}

declare i32 @printf(i8*, ...)
declare i32 @atoi(i8*)
@str.1 = private unnamed_addr constant [5 x i8] c"----\00"
@str.2 = private unnamed_addr constant [5 x i8] c"----\00"
@str.str = private unnamed_addr constant [4 x i8] c"%s\0A\00"
@str.int = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@str.double = private unnamed_addr constant [7 x i8] c"%.16E\0A\00"
@str.true = private unnamed_addr constant [6 x i8] c"true\0A\00"
@str.false = private unnamed_addr constant [7 x i8] c"false\0A\00"

@Echo.c = common global double 0.0
@Echo.counter = common global i1 0

define double @Echo.main.int(i32 %hello.param) {
 %hello = alloca i32
 store i32 %hello.param, i32* %hello
 %a = alloca i32
 %b = alloca i32
 %d = alloca double
 store i32 1, i32* %a
 store i32 0, i32* %b
 %1 = sitofp i32 1 to double
 store double %1, double* @Echo.c
 %2 = sitofp i32 2 to double
 store double %2, double* %d
 %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.str, i32 0, i32 0), [5 x i8]* @str.1)
 br label %dowhile.true.1
dowhile.true.1:
 %4 = load i32, i32* %a
 %5 = icmp slt i32 %4, 9
 br label %dowhile.start.1
dowhile.start.1:
 %6 = load i32, i32* %a
 %7 = icmp eq i32 %6, 5
 br i1 %7, label %if.true.2, label %if.false.2
if.true.2:
 %8 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.str, i32 0, i32 0), [5 x i8]* @str.2)
 %9 = sitofp i32 1 to double
 ret double %9
 br label %if.end.2
if.false.2:
 br label %if.end.2
if.end.2:
 %11 = load i32, i32* %a
 %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 %11)
 %13 = load i32, i32* %a
 %14 = add i32 %13, 1
 store i32 %14, i32* %a
 %15 = load i32, i32* %a
 %16 = icmp slt i32 %15, 9
 br i1 %16, label %dowhile.true.1, label %dowhile.end.1
dowhile.end.1:
 %17 = sitofp i32 1 to double
 ret double %17
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
