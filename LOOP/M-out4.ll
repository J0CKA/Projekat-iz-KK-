; ModuleID = 'M-test4.ll'
source_filename = "M-test4.ll"

define i32 @idealna_mrtva_petlja() {
entry:
  %privremena_suma = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %privremena_suma, align 4
  store i32 0, i32* %i, align 4
  br label %loop

loop:                                             ; preds = %body, %entry
  %i1 = load i32, i32* %i, align 4
  %cmp = icmp slt i32 %i1, 100
  br i1 %cmp, label %body, label %exit

body:                                             ; preds = %loop
  %ps = load i32, i32* %privremena_suma, align 4
  %add1 = add i32 %ps, 5
  %mul1 = mul i32 %add1, 2
  %sub = sub i32 %mul1, %i1
  store i32 %sub, i32* %privremena_suma, align 4
  %i2 = add i32 %i1, 1
  store i32 %i2, i32* %i, align 4
  br label %loop

exit:                                             ; preds = %loop
  ret i32 42
}

define void @petlja_sa_store_efektom(i32* %niz) {
entry:
  %i = alloca i32, align 4
  store i32 0, i32* %i, align 4
  br label %loop

loop:                                             ; preds = %body, %entry
  %i1 = load i32, i32* %i, align 4
  %cmp = icmp slt i32 %i1, 50
  br i1 %cmp, label %body, label %exit

body:                                             ; preds = %loop
  %ptr = getelementptr i32, i32* %niz, i32 %i1
  store i32 10, i32* %ptr, align 4
  %i2 = add i32 %i1, 1
  store i32 %i2, i32* %i, align 4
  br label %loop

exit:                                             ; preds = %loop
  ret void
}

define i32 @petlja_gde_vrednost_bezi_van() {
entry:
  %koristan = alloca i32, align 4
  %i = alloca i32, align 4
  store i32 0, i32* %koristan, align 4
  store i32 0, i32* %i, align 4
  br label %loop

loop:                                             ; preds = %body, %entry
  %i1 = load i32, i32* %i, align 4
  %cmp = icmp slt i32 %i1, 10
  br i1 %cmp, label %body, label %exit

body:                                             ; preds = %loop
  %k = load i32, i32* %koristan, align 4
  %add = add i32 %k, 3
  store i32 %add, i32* %koristan, align 4
  %i2 = add i32 %i1, 1
  store i32 %i2, i32* %i, align 4
  br label %loop

exit:                                             ; preds = %loop
  %k2 = load i32, i32* %koristan, align 4
  %out = add i32 %k2, 5
  ret i32 %out
}

define void @ugnjezdene_petlje() {
entry:
  %i = alloca i32, align 4
  %j = alloca i32, align 4
  store i32 0, i32* %i, align 4
  br label %outer

outer:                                            ; preds = %outer_inc, %entry
  %i1 = load i32, i32* %i, align 4
  %cmp1 = icmp slt i32 %i1, 10
  br i1 %cmp1, label %inner_check, label %exit

inner_check:                                      ; preds = %outer
  store i32 0, i32* %j, align 4
  br label %inner

inner:                                            ; preds = %inner_body, %inner_check
  %j1 = load i32, i32* %j, align 4
  %cmp2 = icmp slt i32 %j1, 20
  br i1 %cmp2, label %inner_body, label %outer_inc

inner_body:                                       ; preds = %inner
  %tmp = add i32 %j1, 2
  %j2 = add i32 %j1, 1
  store i32 %j2, i32* %j, align 4
  br label %inner

outer_inc:                                        ; preds = %inner
  %i2 = add i32 %i1, 1
  store i32 %i2, i32* %i, align 4
  br label %outer

exit:                                             ; preds = %outer
  ret void
}
