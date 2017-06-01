define i32 @main(i32 %argc, i8** %argv) {
 call void @Echo.main.stringarray(i32 %argc, i8** %argv)
 ret i32 0
}

declare i32 @printf(i8*, ...)
declare i32 @atoi(i8*)
@str.str = private unnamed_addr constant [4 x i8] c"%s\0A\00"
@str.int = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@str.double = private unnamed_addr constant [7 x i8] c"%.16E\0A\00"
@str.true = private unnamed_addr constant [6 x i8] c"true\0A\00"
@str.false = private unnamed_addr constant [7 x i8] c"false\0A\00"

@c = common global double 0.0
@counter = common global i1 0

define i32 @Echo.main.boolean(i1 %hello) {
 %hello = alloca i1
 store i1 %hello, i1* %hello
 %a = alloca i32
 %b = alloca i32
 %c = alloca double
 %d = alloca double
 store i32 1, i32* %a
 store i32 1, i32* %b
 store double 1.0, double* %c
 store double 1.0, double* %d
 %1 = load i32, i32* %a
 %2 = load double, double* %c
 %3 = sitofp i32 %1 to double
 %4 = fcmp oge double %3, %2
 br i1 %4, label %true.1, label %false.1
true.1:
 %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.true, i1 0, i1 0))
 br label %end.1
false.1:
 %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.false, i1 0, i1 0))
 br label %end.1
end.1:
 %7 = load i32, i32* %b
 %8 = load double, double* %d
 %9 = sitofp i32 %7 to double
 %10 = fcmp oge double %9, %8
 br i1 %10, label %true.2, label %false.2
true.2:
 %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([6 x i8], [6 x i8]* @str.true, i1 0, i1 0))
 br label %end.2
false.2:
 %12 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.false, i1 0, i1 0))
 br label %end.2
end.2:
 ret i32 1
}

define void @Echo.main.stringarray(i32 %args.c, i8** %args) {
 %args.c = alloca i32
 %args = alloca i8**
 store i32 %args.c, i32* %args.c
 store i8** %args, i8*** %args
 call i32 @Echo.main.boolean(i1 1)
 ret void
}
