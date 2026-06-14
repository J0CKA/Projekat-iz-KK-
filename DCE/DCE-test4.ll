; ModuleID = 'DCE-test4.c'
declare i32 @printf(i8*, ...)

@.str = private constant [4 x i8] c"Hi\0A\00"

define i32 @main() {
entry:
  %a = add i32 1, 2
  %b = add i32 3, 4
  call i32 (i8*, ...) @printf(i8* getelementptr([4 x i8], [4 x i8]* @.str, i32 0, i32 0))
  ret i32 %a
}
