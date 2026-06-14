; ModuleID = 'DSE-test3.ll'
source_filename = "DSE-test3.ll"

define i32 @main(i1 %cond1, i1 %cond2) {
entry:
  %x = alloca i32, align 4
  br i1 %cond1, label %bb1, label %bb2

bb1:                                              ; preds = %entry
  store i32 2, i32* %x, align 4
  br label %merge

bb2:                                              ; preds = %entry
  store i32 3, i32* %x, align 4
  br i1 %cond2, label %bb3, label %merge

bb3:                                              ; preds = %bb2
  store i32 4, i32* %x, align 4
  br label %merge

merge:                                            ; preds = %bb3, %bb2, %bb1
  %v = load i32, i32* %x, align 4
  ret i32 %v
}
