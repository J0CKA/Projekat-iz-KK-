; ModuleID = 'M-test2.ll'
source_filename = "M-test2.ll"

@.str = private constant [4 x i8] c"%d\0A\00"

declare i32 @printf(i8*, ...)

define i32 @main() {
entry:
  br label %loop

loop:                                             ; preds = %loop, %entry
  %i = phi i32 [ 0, %entry ], [ %next, %loop ]
  %next = add i32 %i, 1
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %i)
  %cmp = icmp slt i32 %next, 5
  br i1 %cmp, label %loop, label %exit

exit:                                             ; preds = %loop
  ret i32 0
}
