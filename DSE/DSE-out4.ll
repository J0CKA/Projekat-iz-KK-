; ModuleID = 'DSE-test4.ll'
source_filename = "DSE-test4.ll"

define i32 @main(i1 %cond) {
entry:
  %x = alloca i32, align 4
  br i1 %cond, label %then, label %else

then:                                             ; preds = %entry
  store i32 10, i32* %x, align 4
  br label %merge

else:                                             ; preds = %entry
  store i32 20, i32* %x, align 4
  br label %merge

merge:                                            ; preds = %else, %then
  %v = load i32, i32* %x, align 4
  ret i32 %v
}
