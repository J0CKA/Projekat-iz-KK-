define i32 @main(i1 %cond) {
entry:
  %x = alloca i32
  store i32 5, i32* %x
  br i1 %cond, label %then, label %else

then:
  store i32 10, i32* %x
  br label %merge

else:
  store i32 20, i32* %x
  br label %merge

merge:
  %v = load i32, i32* %x
  ret i32 %v
}
