define i32 @main() {
entry:
  %x = alloca i32
  store i32 5, i32* %x
  br label %next

next:
  store i32 10, i32* %x
  %v = load i32, i32* %x
  ret i32 %v
}
