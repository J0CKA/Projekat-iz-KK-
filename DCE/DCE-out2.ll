; ModuleID = 'DCE-test2.ll'
source_filename = "DCE-test2.ll"

define i32 @main() {
entry:
  %a = add i32 2, 3
  ret i32 %a
}
