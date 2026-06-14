; ModuleID = 'M-test2.c'
declare i32 @printf(i8*, ...)

@.str = private constant [4 x i8] c"%d\0A\00"

define i32 @main() {
entry:
  br label %loop

loop:
  %i = phi i32 [0, %entry], [%next, %loop]
  %next = add i32 %i, 1

  call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str, i32 0, i32 0), i32 %i)

  %cmp = icmp slt i32 %next, 5
  br i1 %cmp, label %loop, label %exit

exit:
  ret i32 0
}
