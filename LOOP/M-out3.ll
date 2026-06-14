; ModuleID = 'M-test3.ll'
source_filename = "M-test3.ll"

define i32 @main() {
entry:
  br label %loop

loop:                                             ; preds = %loop, %entry
  %i = phi i32 [ 1, %entry ], [ %next, %loop ]
  %next = mul i32 %i, 2
  %cmp = icmp slt i32 %next, 1000
  br i1 %cmp, label %loop, label %exit

exit:                                             ; preds = %loop
  %i.lcssa = phi i32 [ %i, %loop ]
  %result = add i32 %i.lcssa, 0
  ret i32 %result
}
