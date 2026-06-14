; ModuleID = 'DCE-test3.ll'
source_filename = "DCE-test3.ll"

define i32 @main() {
entry:
  %a = add i32 10, 20
  %b = add i32 %a, 5
  %c = mul i32 %b, 2
  ret i32 %c
}
