; ModuleID = 'DCE-test1.ll'
source_filename = "DCE-test1.ll"

define i32 @main() {
entry:
  br label %live

dead:                                             ; No predecessors!
  ret i32 0

live:                                             ; preds = %entry
  ret i32 1
}
