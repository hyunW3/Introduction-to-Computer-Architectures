GCD:
#-----------------------------------------------------
# Your code "START" here
#-----------------------------------------------------

# argument n =$a0 , m-$a1
lw $a0 8($sp) 
lw $a1 12($sp)

# set i,j,ret

	addi $sp $sp -16
	sw $ra 12($sp)
	sw $s0 8($sp) # store i
	sw $s1 4($sp) # store j
	sw $s2 0($sp) # store ret

	slt $t0 $a0 $a1 # n<m t0 =1
	bne $t0 $0 else1 # t0 != $0 (t0==1)n<m -> else1
	move $s1 $a1  # (n>m) j == m
	j L1
else1: # if t0 ==1(n<m)
	move $s1 $a0 #  j = n
L1:
	# for loop enter
	move $s0 $s1 # i=j
ForLoop:
	div $a0 $s0
	mfhi $t0 # store remainder in t0
	div $a1 $s0 # 
	mfhi $t1 # remainder in t1
	bne $t0 $0 else2
	bne $t1 $0 else2
	move $s2 $s0 ## remainder of two is 0
	j LoopEND
else2:
	addi $s0 $s0 -1
	#slt $t2 $0 $s0  # 0<i -> t2 =1
	#bne $t2 $0 ForLoop #  if t2 != 0(t2==1) go ForLoop
	#j LoopEND 
	slt $t2 $s0 $0  # 0<i -> t2 =1
	beq $t2 $0 ForLoop #  if t2 != 0(t2==1) go ForLoop
	j LoopEND 
LoopEND:
	move $v0 $s2
	lw $ra 12($sp)
	lw $s0 8($sp) # restore i
	lw $s1 4($sp) # restore j
	lw $s2 0($sp) # restore ret
	addi $sp $sp 16
#-----------------------------------------------------
# Your code "END" here
#-----------------------------------------------------
jr  $ra

LCM:
#-----------------------------------------------------
# Your code "START" here
#-----------------------------------------------------
	addi $sp $sp -16
	sw $ra 12($sp)
	sw $s0 8($sp) # store i
	sw $s1 4($sp) # store j
	sw $s2 0($sp) # store ret

	slt $t0 $a1 $a0 # m<n t0 =1
	bne $t0 $0 else3 # t0 != $0 (t0==1)n<m -> else3
	move $s1 $a1  # (n>m) j == m
	j L2
else3: # if t0 ==1(n<m)
	move $s1 $a0 #  j = n
L2:
	# for loop enter
	move $s0 $s1 # i=j
ForLoop2:
	div $s0 $a0 # i/n
	mfhi $t0 # stor#e remainder in t0
	div $s0 $a1 # i/m
	mfhi $t1 # remainder in t1
	bne $t0 $0 else4
	bne $t1 $0 else4
	move $s2 $s0 ## remainder of two is 0
	j LoopEND2
else4:
	add $s0 $s0 $s1
	#slt $t2 $0 $s0  # 0<i -> t2 =1
	#bne $t2 $0 ForLoop #  if t2 != 0(t2==1) go ForLoop
	#j LoopEND 
	j ForLoop2
LoopEND2:
	move $v0 $s2
	lw $ra 12($sp)
	lw $s0 8($sp) # restore i
	lw $s1 4($sp) # restore j
	lw $s2 0($sp) # restore ret
	addi $sp $sp 16
#-----------------------------------------------------
# Your code "END" here
#-----------------------------------------------------
jr  $ra