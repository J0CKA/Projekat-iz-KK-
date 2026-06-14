; ModuleID = 'M-test3.c'
define i32 @main() {
entry:
  br label %loop

loop:
  %i = phi i32 [1, %entry], [%next, %loop]
  %next = mul i32 %i, 2
  %cmp = icmp slt i32 %next, 1000
  br i1 %cmp, label %loop, label %exit

exit:
  %result = add i32 %i, 0
  ret i32 %result
}
