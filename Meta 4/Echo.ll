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
 %hello.addr = alloca i1
 store i1 %hello, i1* %hello.addr
 %bol.addr = alloca i1
 %b.addr = alloca i32
 store i1 0, i1* %hello
 store double 2.0, double* @c
 store i1 1, i1* %counter
 call i1 @Echo.coisa.()
 store i1 (null), i1* %counter
 store i1 1, i1* %bol
 store i1 0, i1* %bol
 ret i32 1
}

define void @Echo.main.stringarray(i32 %args.c, i8** %args) {
 %args.c.addr = alloca i32
 %args.addr = alloca i8**
 store i32 %args.c, i32* %args.c.addr
 store i8** %args, i8*** %args.addr
 %a.addr = alloca i32
 %b.addr = alloca i32
 store i32 1, i32* %a
 %1 = load i8**, i8*** %args.addr
 %2 = getelementptr inbounds i8*, i8** %1, i32 1
 %3 = load i8*, i8** %2
 %4 = call i32 @atoi(i8* %3)
 store i32 %4, i32* %b
 %5 = load i32, i32* %b.addr
 %6 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 %5)
 %7 = load i32, i32* %a.addr
 %8 = load i32, i32* %b.addr
 %9 = sub i32 %7, %8
 store i32 %9, i32* %a
 %10 = load i32, i32* %b.addr
 %11 = load i32, i32* %a.addr
 %12 = sub i32 %10, %11
 store i32 %12, i32* %b
 %13 = load i32, i32* %a.addr
 %14 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @str.int, i32 0, i32 0), i32 %13)
 ret void
}

define i1 @Echo.coisa.() {
 ret i1 1
}
