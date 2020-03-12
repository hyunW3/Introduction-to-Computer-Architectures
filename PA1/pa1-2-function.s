product:
#-----------------------------------------------------
# Your code "START" here
#-----------------------------------------------------
# should tmp1 tmp2 tmp3 declare
# return product(n-1)
## mul product malfunction A[i][k], not A[j][k]
add $sp $sp -36
sw $ra 32($sp) # return address
sw $s3 28($sp) # 10
sw $s4 24($sp) # A
sw $s5 20($sp) # B
sw $s6 16($sp) # C
sw $a0 12($sp) # N
sw $s0 8($sp) # i
sw $s1 4($sp) # j
sw $s2 0($sp) # k

la $s3 10 # MAX = 10
la $s4 A
la $s5 B
la $s6 C
la $t2 DIM# initiallizer
lw $t2 0($t2)
# i =0; j=0; k=0;
 

# use t1 for temp(compare)
  li $t1 1 # for compare 1
  beq $t1 $a0 Exit # n == 1
  li $t3 0 # initializer
# 3 for loop
    move $s0 $0 # i=0
ForLoop1:
	move $t1 $0
    slt $t1 $s0 $t2 # i < DIM 
    beq $t1 $0 ForEND1
    move $s1 $0 # j=0
ForLoop2:
    slt $t1 $s1 $t2 # j < DIM
    beq $t1 $0 ForEND2
    move $s2 $0 # k=0
ForLoop3:
    slt $t1 $s2 $t2 # k < DIM t3=1
    beq $t1 $0 ForEND3

## insert code here
#A[i][k] / t5 tmp regis for A
  addi $t5, $0, 4 
  mul $t5, $t5, $s2
  addi $t6, $0, 40
  mul $t6, $t6, $s0
  add $t5, $t5, $t6
  add $t5, $t5, $s4 # 
  add $t3 $t5 $0 # tmp1 for $t3 address
  lw $t3 0($t3) # store data, not address
#B[k][j] / t5 tmp regis for A
  addi $t5, $0, 4 
  mul $t5, $t5, $s1
  addi $t6, $0, 40
  mul $t6, $t6, $s2
  add $t5, $t5, $t6
  add $t5, $t5, $s5 # 
  add $t4 $t5 $0 # tmp2 for t4
  lw $t4 0($t4) # from address of B[k][j]
#C[i][j] / t5 tmp regis for A
  addi $t5, $0, 4 
  mul $t5, $t5, $s1
  addi $t6, $0, 40
  mul $t6, $t6, $s0 # i* 40
  add $t5, $t5, $t6
  add $t5, $t5, $s6 # tc for 5
  add $t7 $t5 $0 # tmp3 for t7

############################# mul is no problem
	 lw $t9 0($t7) # store t7(C[i][j]) in t9
# C += A*B
	li $t8 0
	mul $t8 $t3 $t4
	add $t9 $t9 $t8
	sw $t9 0($t7)  # store the result data to t7(address of C[i][j])
	li $t9 0
    addi $s2 $s2 1
    j ForLoop3
ForEND3:
    addi $s1 $s1 1

    j ForLoop2
ForEND2:
    addi $s0 $s0 1
    j ForLoop1
ForEND1:

# Next 2 for loop

    move $s0 $0 # i=0
ForLoop4:
    slt $t1 $s0 $t2 # i < DIM 
    beq $t1 $0 ForEND4
    move $s1 $0 # j=0
    li $t8 0
ForLoop5:
    slt $t1 $s1 $t2 # j < DIM
    beq $t1 $0 ForEND5


# C[i][j] =0;
# B[i][j]
  addi $t5, $0, 4 
  mul $t5, $t5, $s1
  addi $t6, $0, 40
  mul $t6, $t6, $s0
  add $t5, $t5, $t6
  add $t5, $t5, $s5 # 
    add $t3 $t5 $0 # tmp1 : $t3
# C[i][j]
  addi $t5, $0, 4 
  mul $t5, $t5, $s1
  addi $t6, $0, 40
  mul $t6, $t6, $s0
  add $t5, $t5, $t6
  add $t5, $t5, $s6 # 
   add $t4 $t5 $0 # tmp2 for t3

# B[i][j] = C[i][j]
	#lw $t7 0($t3) # B
   lw $t8 0($t4) # load data C[i][j] from address
   #printf

   sw $t8 0($t3) # B[i][j]t3 address = C[i][j](t8 data)
#   li $v0 1
#   move $a0 $t8
 #  syscall
#   li $v0 4
 #  la $a0 SPACE
 #  syscall
   #sw $0 0($t4) #C[i][j] = 0;

  addi $s1 $s1 1
  j ForLoop5
ForEND5:


  addi $s0 $s0 1
  j ForLoop4
ForEND4:

#return product(n-1)
addi $a0 $a0 -1 # n = n-1
jal product
Exit: # return;
lw $ra 32($sp) # return address
lw $s3 28($sp) # 10
lw $s4 24($sp) # A
lw $s5 20($sp) # B
lw $s6 16($sp) # C
lw $a0 12($sp) # n
lw $s0 8($sp) # i
lw $s1 4($sp) # j
lw $s2 0($sp) # k
addi $sp $sp 36
#-----------------------------------------------------
# Your code "END" here
#-----------------------------------------------------
jr  $ra

printResult:
#-----------------------------------------------------
# Your code "START" here
#-----------------------------------------------------
	addi $sp $sp -16
	sw $ra 12($sp)
	sw $s5 8($sp)
	sw $s0 4($sp) # i
	sw $s1 0($sp) # j
	la $s5 B
    move $s0 $0 # i=0



ForLoop_1:
    slt $t3 $s0 $t2 # i < DIM 
    beq $t3 $0 ForEND_1
    move $s1 $0 # j=0
ForLoop_2:
    slt $t3 $s1 $t2 # j < DIM
    beq $t3 $0 ForEND_2
 # B[i][j]
  addi $t5, $0, 4 
  mul $t5, $t5, $s1 # s1 : j
  addi $t6, $0, 40
  mul $t6, $t6, $s0 # s0 : i
  add $t5, $t5, $t6
  add $t5, $t5, $s5  # tmp1 : $t5 - B[i][j] address

 	li $v0 1 # print int
 	#add $a0 $zero $t7 #
 	lw $t7 0($t5)
 	#move $a0 $t7 
 	add $a0 $t7 $0 
 	#lw $a0 0($t7)
 	syscall
 	li $v0 4
 	la $a0, SPACE
 	syscall

  	addi $s1 $s1 1
  	j ForLoop_2
ForEND_2:
	la $a0, ENTER
	syscall

  addi $s0 $s0 1
  j ForLoop_1
ForEND_1:

	lw $ra 12($sp)
	lw $s5 8($sp)
	lw $s0 4($sp) # i
	lw $s1 0($sp) # j
	addi $sp $sp 12


#-----------------------------------------------------
# Your code "END" here
#-----------------------------------------------------
jr  $ra