.global _start

.section .text

_start:
	b check_input
	
check_input:
	// Find input length
	ldr r0, =input // Loads input memory address into register0
	mov r1, #1 // Load what byte we are on into register, we will start on index 2
find_position_of_null:
	add r1, r1, #1 // Increment index
	ldrb r2, [r0, r1] // Load one letter into register
	cmp r2, #0 // Check if it's the null byte
	bne find_position_of_null // Branch to check next letter
	b check_palindrome
	
check_palindrome:
	// Check whether input is a palindrome or not
	r2, #0xffffffff // Set tail tracker register, r1 will be used to track head
head: 
	sub r1, r1, #1 // Decrement head index
	cmp r1, r2 // Check if head and tail have met
	beq palindrome_found // head and tail have met, this is a palindrome
	ldrb r3, [r0, r1] // Load current byte into register
	
	// Skip spaces
	cmp r3, #0x20 // Check if it is space " "
	beq head // Go to next byte if it's a space
	
	// Make letters capitalized
	cmp r3, #0x61 // Check if it is "a"
	blt .+8	// Branch over if its less than 0x61 (not lowercase)
	sub r3, r3, #0x20 // Make this lowercase letter uppercase
tail:
	add r2, r2, #1
	cmp r1, r2 // Check if head and tail have met
	beq palindrome_found // head and tail have met, this is a palindrome
	ldrb r4, [r0,r2] // Load current byte into register
	
	// Skip spaces
	cmp r4, #0x20 // Check if it is space " "
	beq tail // Go to next byte if it's a space
	
	// Make letters capitalized
	cmp r4, #0x61 // Check if it is "a"
	blt .+8	// Branch over if its less than 0x61 (not lowercase)
	sub r4, r4, #0x20 // Make this lowercase letter uppercase
	
	// Compare letters
	cmp r3, r4 // Check if letters are equal
	bne palindrom_not_found // They are not equal, this is not a palindrome
	b head // Check next characters
	
palindrome_found:
	// Switch on only the 5 rightmost LEDs
	ldr r0, =0xff200000
	mov r1, #0x1f // 5 most right leds
	str r1, [r0]
	
	// Write 'Palindrome detected' to UART
	ldr r1, =palindrome_string // Set address of string
	b write_jtag
	
palindrom_not_found:
	// Switch on only the 5 leftmost LEDs
	ldr r0, =0xff200000
	mov r1, #0x3e0 // 5 most left leds
	str r1, [r0]
	
	// Write 'Not a palindrome' to UART
	ldr r1, =not_palindrome_string // Set address of string
	b write_jtag

write_jtag:
	ldr r0, =0xff201000 // Uart register address
	mov r3, #0xffffffff // Set register to track which byte we are on, will become 0
write_jtag_next_char:
	add r3, r3, #1	
	ldrb r2, [r1, r3] // Load one byte of the message into r2
	cmp r2, #0 // Check if that was the null byte
	beq exit
	str r2, [r0] // Print the one byte to uart
	b write_jtag_next_char
		
exit:
	b exit	

.section .data
.align
	// This is the input you are supposed to check for a palindrome
	// You can modify the string during development, however you
	// are not allowed to change the label 'input'!
	//input: .asciz "level"
	//input: .asciz "8448"
    //input: .asciz "KayAk"
    //input: .asciz "step on no pets"
    input: .asciz "Never odd or even"
	palindrome_string: .asciz "Palindrome detected\n"
	not_palindrome_string: .asciz "Not a palindrome\n"
	


.end