; =========================
; idealna_mrtva_petlja
; =========================

define i32 @idealna_mrtva_petlja() {
entry:
  %privremena_suma = alloca i32
  %i = alloca i32

  store i32 0, i32* %privremena_suma
  store i32 0, i32* %i
  br label %loop

loop:
  %i1 = load i32, i32* %i
  %cmp = icmp slt i32 %i1, 100
  br i1 %cmp, label %body, label %exit

body:
  %ps = load i32, i32* %privremena_suma

  %add1 = add i32 %ps, 5
  %mul1 = mul i32 %add1, 2
  %sub = sub i32 %mul1, %i1

  store i32 %sub, i32* %privremena_suma

  %i2 = add i32 %i1, 1
  store i32 %i2, i32* %i

  br label %loop

exit:
  ret i32 42
}

; =========================
; petlja_sa_store_efektom
; =========================

define void @petlja_sa_store_efektom(i32* %niz) {
entry:
  %i = alloca i32
  store i32 0, i32* %i
  br label %loop

loop:
  %i1 = load i32, i32* %i
  %cmp = icmp slt i32 %i1, 50
  br i1 %cmp, label %body, label %exit

body:
  %ptr = getelementptr i32, i32* %niz, i32 %i1
  store i32 10, i32* %ptr

  %i2 = add i32 %i1, 1
  store i32 %i2, i32* %i

  br label %loop

exit:
  ret void
}

; =========================
; petlja_gde_vrednost_bezi_van
; =========================

define i32 @petlja_gde_vrednost_bezi_van() {
entry:
  %koristan = alloca i32
  %i = alloca i32

  store i32 0, i32* %koristan
  store i32 0, i32* %i
  br label %loop

loop:
  %i1 = load i32, i32* %i
  %cmp = icmp slt i32 %i1, 10
  br i1 %cmp, label %body, label %exit

body:
  %k = load i32, i32* %koristan
  %add = add i32 %k, 3
  store i32 %add, i32* %koristan

  %i2 = add i32 %i1, 1
  store i32 %i2, i32* %i

  br label %loop

exit:
  %k2 = load i32, i32* %koristan
  %out = add i32 %k2, 5
  ret i32 %out
}

; =========================
; ugnjezdene_petlje
; =========================

define void @ugnjezdene_petlje() {
entry:
  %i = alloca i32
  %j = alloca i32

  store i32 0, i32* %i
  br label %outer

outer:
  %i1 = load i32, i32* %i
  %cmp1 = icmp slt i32 %i1, 10
  br i1 %cmp1, label %inner_check, label %exit

inner_check:
  store i32 0, i32* %j
  br label %inner

inner:
  %j1 = load i32, i32* %j
  %cmp2 = icmp slt i32 %j1, 20
  br i1 %cmp2, label %inner_body, label %outer_inc

inner_body:
  %tmp = add i32 %j1, 2

  %j2 = add i32 %j1, 1
  store i32 %j2, i32* %j

  br label %inner

outer_inc:
  %i2 = add i32 %i1, 1
  store i32 %i2, i32* %i

  br label %outer

exit:
  ret void
}
