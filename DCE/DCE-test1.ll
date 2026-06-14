; ModuleID = 'DCE-test1.c'
define i32 @main() {
entry:
  br label %live

dead:
  %x = add i32 1, 2
  ret i32 0

live:
  ret i32 1
}
