define i32 @main(i1 %cond1, i1 %cond2) {
entry:
  %x = alloca i32
  store i32 1, i32* %x
  br i1 %cond1, label %bb1, label %bb2

bb1:
  store i32 2, i32* %x
  br label %merge

bb2:
  store i32 3, i32* %x
  br i1 %cond2, label %bb3, label %merge

bb3:
  store i32 4, i32* %x
  br label %merge

merge:
  %v = load i32, i32* %x
  ret i32 %v
}
