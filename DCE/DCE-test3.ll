; ModuleID = 'DCE-test3.c'
define i32 @main() {
entry:
  %a = add i32 10, 20
  %b = add i32 %a, 5
  %c = mul i32 %b, 2
  %d = add i32 100, 200
  ret i32 %c
}
