define i32 @main(i32 %argc, i8** %argv) {
 call void @gcd2.main.stringarray(i32 %argc, i8** %argv)
 ret i32 0
}

declare i32 @printf(i8*, ...)
declare i32 @atoi(i8*)
@str.1 = private unnamed_addr constant [32 x i8] c"Error: two parameters required.\00"
@str.str = private unnamed_addr constant [4 x i8] c"%s\0A\00"
@str.int = private unnamed_addr constant [4 x i8] c"%d\0A\00"
@str.double = private unnamed_addr constant [7 x i8] c"%.16E\0A\00"
@str.true = private unnamed_addr constant [6 x i8] c"true\0A\00"
@str.false = private unnamed_addr constant [7 x i8] c"false\0A\00"

@gcd2.gcd = common global i32 0

define void @gcd2.main.stringarray(i32 %args.c.param, i8** %args.param) {
 %args.c = alloca i32
 %args = alloca i8**
 store i32 %args.c.param, i32* %args.c
 store i8** %args.param, i8*** %args
 %a = alloca i32
 %b = alloca i32
 %1 = load i32, i32* %args.c
 %2 = sub i32 %1, 1
 %3 = icmp sge i32 %2, 2
 br i1 %3, label %if.true.1, label %if.false.1
if.true.1:
 %4 = load i8**, i8*** %args
 %5 = add i32 1, 0
 %6 = getelementptr inbounds i8*, i8** %4, i32 %5
 %7 = load i8*, i8** %6
 %8 = call i32 @atoi(i8* %7)
 store i32 %8, i32* %a
 %9 = load i8**, i8*** %args
 %10 = add i32 1, 1
 %11 = getelementptr inbounds i8*, i8** %9, i32 %10
 %12 = load i8*, i8** %11
 %13 = call i32 @atoi(i8* %12)
 store i32 %13, i32* %b
 %14 = load i32, i32* %a
 %15 = load i32, i32* %b
 %16 = call i32 @gcd2.gcd.intint(i32 %14, i32 %15)
 store i32 %16, i32* @gcd2.gcd
 %17 = load i32, i32* @gcd2.gcd
 %18 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 %17)
 br label %if.end.1
if.false.1:
 %19 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.str, i32 0, i32 0), [32 x i8]* @str.1)
 br label %if.end.1
if.end.1:
 ret void
}

define i32 @gcd2.gcd.intint(i32 %a.param, i32 %b.param) {
 %a = alloca i32
 store i32 %a.param, i32* %a
 %b = alloca i32
 store i32 %b.param, i32* %b
 %1 = load i32, i32* %a
 %2 = icmp eq i32 %1, 0
 br i1 %2, label %if.true.2, label %if.false.2
if.true.2:
 %3 = load i32, i32* %b
 ret i32 %3
 br label %if.end.2
if.false.2:
 br label %while.start.3
while.start.3:
 %5 = load i32, i32* %b
 %6 = icmp sgt i32 %5, 0
 br i1 %6, label %while.true.3, label %while.end.3
while.true.3:
 %7 = load i32, i32* %a
 %8 = load i32, i32* %b
 %9 = icmp sgt i32 %7, %8
 br i1 %9, label %if.true.4, label %if.false.4
if.true.4:
 %10 = load i32, i32* %a
 %11 = load i32, i32* %b
 %12 = sub i32 %10, %11
 store i32 %12, i32* %a
 br label %if.end.4
if.false.4:
 %13 = load i32, i32* %b
 %14 = load i32, i32* %a
 %15 = sub i32 %13, %14
 store i32 %15, i32* %b
 br label %if.end.4
if.end.4:
 br label %while.start.3
while.end.3:
 %16 = load i32, i32* %a
 ret i32 %16
 br label %if.end.2
if.end.2:
 ret i32 0
}
