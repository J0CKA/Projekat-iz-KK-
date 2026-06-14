; ModuleID = 'DSE-test1.ll'
source_filename = "DSE-test1.ll"

define i32 @main() {
entry:
  %x = alloca i32, align 4
  br label %next

next:                                             ; preds = %entry
  store i32 10, i32* %x, align 4
  %v = load i32, i32* %x, align 4
  ret i32 %v
}
