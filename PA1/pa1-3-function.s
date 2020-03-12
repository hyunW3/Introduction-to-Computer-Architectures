swap:
#-----------------------------------------------------
# Your code "START" here
#-----------------------------------------------------
# $a0 t8 a data
# $a1 t9 b data
# $t3 	a address
# $t4 	b address
# swap check
	addi $sp $sp -12
	sw $ra 8($sp)
	sw $s0 4($sp) # for temp
	sw $s1 0($sp)

#swap data
#	addi $s0 $t8 0
#	addi $s1 $t9 0


	lw $t8 0($t3)
	lw $t9 0($t4)
# after swap a : $t9 address $t3 , b $t8 address$t4
	sw $t9 0($t3)
	sw $t8 0($t4)
#	addi $t8 $s1 0
#addi $t9 $s0 0

#	lw $s0 0($t3) 
#	lw $s1 0($t4)
#	sw $s1 0($t3)
#	sw $s0 0($t4)


	lw $ra 8($sp)
	lw $s0 4($sp)
	lw $s1 0($sp)
	addi $sp $sp 12
#-----------------------------------------------------
# Your code "END" here
#-----------------------------------------------------
jr  $ra

partition:
#-----------------------------------------------------
# Your code "START" here
#-----------------------------------------------------
addi $sp $sp -48
sw $ra 44($sp)
sw $t0 40($sp)
sw $t1 36($sp)
sw $t2 32($sp)
sw $t6 28($sp)
sw $t7 24($sp)
sw $a0 20($sp)
sw $a1 16($sp)
sw $s0 12($sp) # pivot
sw $s1 8($sp) # temp
sw $s2 4($sp) # low
sw $s3 0($sp) # high

add $t5 $a0 $0# t5 for a0 left# t5 is 0?
add $t6 $a1 $0 # t6 for a1 right
la $t0 list # t0 in list
li $t3 0
li $t4 0
li $t8 0
li $t9 0 
# a0 left , a1 right
	li $s0 0 # clear value in s0
	move $s2 $t5 # low =left
	add $s3 $t6 1 # high = right +1
	sll $t1 $t5 2 # left *2^2 in t1     t1 for temp
	add $t7 $t1 $t0 # list + t1(left*4)
	lw $s0 0($t7) # data, not address list[left]

do1:

do2:
	addi $s2 $s2 1 # low++
	# to store list[low]
	sll $t1 $s2 2 # low *(2^2)
	add $t1 $t0 $t1 # list +low*4
	add $t3 $t1 $0
	lw $t8 0($t3) # data, not address list[low]
	
	addi $t1 $t6 1 # right +1
	slt $t2 $s2 $t1 # compare regi for t2 low < right+1 (=low =< right)
	beq $t2 $0 do3 # t1 = 1
	#j do3
# &&
#compare
	slt $t2 $t8 $s0 # list[low] < pivot
	bne $t2 0 do2
	# 
do3:
	addi $s3 $s3 -1 # high = high -1
	# for list[high] define
	sll $t1 $s3 2 # high *4
	add $t1 $t0 $t1 # list(t0) +hgih*4
	add $t4 $t1 $0
	lw $t9 0($t4) # data, not address list[high]


	addi $t1 $s3 1 # high + 1 to compare
	slt $t2 $t5 $t1 # left < high+1(left<=high)
	beq $t2 $0 if1 # t2 = 1 -> go do3 // t2 = 0 go if1
#	bne $t5 $s3 if1 # t5 left s3 high

# &&
#compare
	slt $t2 $s0 $t9 # pivot < list[high] 
	bne $t2 0 do3	# t2 == 1 go do3

if1: # low < high	
	slt $t2 $s2 $s3 # low < high
	beq $t2 $0 do1while
	#  swap call & save $a0 $a1

	lw $a0 0($t3)  # a0 data, t3 address
	lw $a1 0($t4) # a1 data, t4 address
#	lw $a0 0($t3)
#	lw $a1 0($t4) # problem
	jal swap

do1while:
	slt $t2 $s2 $s3 # low < high
	bne $t2 $0 do1 # t2 1 
#	add $a0 $t3 $0 # list[low] address
#	add $a1 $t4 $0 # list[high]



Exit2: #list[left], list[high] 
	addi $sp $sp -4
	lw $t3 0($sp)
	addi $t3 $t7 0 # modify t3
	lw $a0 0($t3)
	lw $a1 0($t4)
	jal swap

	sw $t3 0($sp)
	addi $sp $sp 4
#return high
add $v0 $s3 $0
#restore

	lw $ra 44($sp)
	lw $t0 40($sp)
	lw $t1 36($sp)
	lw $t2 32($sp)
	lw $t6 28($sp)
	lw $t7 24($sp)
	lw $a0 20($sp)
	lw $a1 16($sp)
	lw $s0 12($sp)
	lw $s1 8($sp) # temp
	lw $s2 4($sp) # low
	lw $s3 0($sp)
	addi $sp $sp 48
	

#-----------------------------------------------------
# Your code "END" here
#-----------------------------------------------------
jr  $ra

Quick_sort:
#-----------------------------------------------------
# Your code "START" here
#-----------------------------------------------------
# $a0 : left
# $a1 right



	addi $sp $sp -32
	sw $ra 28($sp)
	sw $t0 24($sp)
	sw $t1 20($sp)
	sw $t5 16($sp) # left
	sw $t6 12($sp) # right
	sw $a0 8($sp)
	sw $a1 4($sp)
	sw $s0 0($sp) # for int q
addi $t5 $a0 0 # left 
addi $t6 $a1 0 # right
la $t0 list
# if(left < right)
	li $t1 0	# t1 for compare
	slt $t1 $a0 $a1 # left < right
	beq $t1 $0 Exit1 # if t1 == 0 eixt1
	jal partition
	add $s0 $v0 $0
	
	add $t2 $s0 -1 # t2 = q-1
	
	addi $sp $sp -4
	sw $s0 0($sp)
	move $a0 $t5 # left
	move $a1 $t2 # right
	jal Quick_sort #quicksort(left,q-1)
#save 
	lw $s0 0($sp)
	addi $sp $sp 4 # pair 187

	addi $t2 $s0 1 

	move $a0 $t2 # q+1
	lw $t6 12($sp) # works well
	move $a1 $t6 # right
	jal Quick_sort # quicksort(q+1,right)

Exit1:
	li $t0 0
	lw $ra 28($sp)
	lw $t0 24($sp)
	lw $t1 20($sp)
	lw $t5 16($sp)
	lw $t6 12($sp)
	lw $a0 8($sp)
	lw $a1 4($sp)
	lw $s0 0($sp)
	addi $sp $sp 32
#-----------------------------------------------------
# Your code "END" here
#-----------------------------------------------------
jr  $ra