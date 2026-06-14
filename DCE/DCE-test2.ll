; ModuleID = 'DCE-test2.c'
define i32 @main() {
entry:
  %a = add i32 2, 3
  %b = add i32 5, 6
  ret i32 %a
}
