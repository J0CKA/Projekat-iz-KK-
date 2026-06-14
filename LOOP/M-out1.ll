; ModuleID = 'M-test1.ll'
source_filename = "M-test1.ll"

define i32 @main() {
entry:
  br label %exit

exit:                                             ; preds = %entry
  ret i32 0
}
