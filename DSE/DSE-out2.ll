; ModuleID = 'DSE-test2.ll'
source_filename = "DSE-test2.ll"

@sink = dso_local global i32 0, align 4

define dso_local void @napredni_test(i32* %p, i32* %q, i32 %uslov) {
entry:
  %p.addr = alloca i32*, align 8
  %q.addr = alloca i32*, align 8
  %uslov.addr = alloca i32, align 4
  store i32* %p, i32** %p.addr, align 8
  store i32* %q, i32** %q.addr, align 8
  store i32 %uslov, i32* %uslov.addr, align 4
  %0 = load i32*, i32** %p.addr, align 8
  store i32 100, i32* %0, align 4
  %1 = load i32*, i32** %q.addr, align 8
  store i32 5, i32* %1, align 4
  %2 = load i32, i32* %uslov.addr, align 4
  %cmp = icmp ne i32 %2, 0
  br i1 %cmp, label %if.then, label %if.end

if.then:                                          ; preds = %entry
  %3 = load i32*, i32** %q.addr, align 8
  %4 = load i32, i32* %3, align 4
  store i32 %4, i32* @sink, align 4
  br label %if.end

if.end:                                           ; preds = %if.then, %entry
  %5 = load i32*, i32** %p.addr, align 8
  store i32 200, i32* %5, align 4
  ret void
}

define dso_local i32 @main() {
entry:
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  store i32 0, i32* %a, align 4
  store i32 0, i32* %b, align 4
  call void @napredni_test(i32* %a, i32* %b, i32 1)
  ret i32 0
}
