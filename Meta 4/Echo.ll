define i32 @main(i32 %argc, i8** %argv) {
 call void @Echo.main.stringarray(i32 %argc, i8** %argv)
 ret i32 0
}

declare i32 @printf(i8*, ...)
@str.1 = private unnamed_addr constant [4 x i8] c"ola\00"
@str.str = private unnamed_addr constant [4 x i8] c"%s\0A\00"
@str.int = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@str.double = private unnamed_addr constant [7 x i8] c"%.16E\0A\00"
@str.true = private unnamed_addr constant [6 x i8] c"true\0A\00"
@str.false = private unnamed_addr constant [7 x i8] c"false\0A\00"

@c = common global double 0.0
define i32 @Echo.main.intdouble(i32 %as, double %bs) {
 %a = alloca i32
 %b = alloca i32
 store double 2.0, double* @c
 store i32 1, i32* %a
 store i32 200, i32* %b
 %1 = mul i32 -1, 1
 %2 = sitofp i32 %1 to double
 store double %2, double* @c
 %3 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.str, i32 0, i32 0), [4 x i8]* @str.1)
 %4 = load double, double* @c
 %5 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([7 x i8], [7 x i8]* @str.double, i32 0, i32 0), double %4)
 %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 10)
 ret i32 1
}
define void @Echo.main.stringarray(i32 %args.c, i8** %args) {
 %aa = alloca i32
 %1 = load i32, i32* %aa
 %2 = sitofp i32 20 to double
 call i32 @Echo.main.intdouble(i32 %1, double %2)
 ret void
}
