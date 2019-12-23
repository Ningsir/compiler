.globl main0 
.text
main0: 
jal main 
li $v0,10
syscall
test:
sw $a0, 4($sp)
li $t3, 1
sw $t3, 12($sp)
lw $t1, 12($sp)
move $t3, $t1
sw $t3, 8($sp)
label0:
li $t3, 0
sw $t3, 16($sp)
lw $t1, 4($sp)
lw $t2, 16($sp)
bge $t1,$t2, label1
j label2
label1:
li $t3, 0
sw $t3, 20($sp)
lw $t1, 4($sp)
lw $t2, 20($sp)
beq $t1,$t2, label3
j label4
label3:
j label2
label4:
lw $t1, 8($sp)
lw $t2, 4($sp)
mul $t3, $t1, $t2
sw $t3, 20($sp)
lw $t1, 20($sp)
move $t3, $t1
sw $t3, 8($sp)
li $t3, 1
sw $t3, 24($sp)
lw $t1, 4($sp)
lw $t2, 24($sp)
sub $t3, $t1, $t2
sw $t3, 28($sp)
lw $t1, 28($sp)
move $t3, $t1
sw $t3, 4($sp)
j label0
label2:
lw $v0, 8($sp)
jr $ra
main:
li $t3, 6
sw $t3, 24($sp)
lw $a0, 24($sp)
sw $ra, 0($sp)
jal test
sw $v0, 28($sp)
lw $ra, 0($sp)
lw $t1, 28($sp)
move $t3, $t1
sw $t3, 20($sp)
li $t3, 1
sw $t3, 32($sp)
lw $v0, 32($sp)
jr $ra
