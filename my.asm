; Turn on alphabet
	alpha "01a"
; Skip to state 0
	bra !state0

; STATE 0 ---------------
!state0right
	right
!state0
	cmp '0'
	brae !state0right
	cmp blank
	brae !state10
	cmp '1'
	brae !state1right
	fail

; STATE 1 --------------
!state1right
	right
!state1
	cmp '0'
	brae !state1right
	cmp '1'
	brae !state1right
	cmp blank
	brae !state2right
	fail

; STATE 2 -------------
!state2right
	right
!state2
	cmp 'a'
	brae !state2right
	cmp blank
	brae !state3lefta
	fail

; STATE 3 -------------
!state3lefta
	draw 'a'
	left
	bra !state3
!state3left
	left
!state3
	cmp 'a'
	brae !state3left
	cmp blank
	bra !state4left
	fail

; STATE 4 --------------
!state4left
	left
	bra !state4
!state4left1
	draw '1'
	left
!state4
	cmp '0'
	brae !state4left1
	cmp '1'
	brae !state5left0
	fail

; STATE 5 ------------
!state5left0
	draw '0'
	left
	bra !state5
!state5left1
	draw '1'
	left
!state5
	cmp '0'
	brae !state5left0
	cmp '1'
	brae !state5left1
	cmp blank
	brae !state6right
	fail

; STATE 6 -------------
!state6right
	right
!state6
	cmp '0'
	brae !state6right
	cmp '1'
	brae !state1right
	cmp blank
	brae !state7left
	fail

; STATE 7 --------------
!state7left
	left
	bra !state7
!state7leftblank
	draw blank
	left
!state7
	cmp blank
	brane !state7leftblank
;	brae !state8right
; 	(fall through is has same effect)

; STATE 8 -------------
!state8right
	right
!state8
	cmp blank
	brae !state8right
	cmp 'a'
	brae !halt
	fail

; STATE 9 --------------
; this state has no effect
; just call halt

; STATE 10 -------------
!state10left
	left
	bra !state10
!state10leftblank
	draw blank
	left
!state10
	cmp '0'
	brae !state10left
	cmp '1'
	brae !state10left
	cmp blank
	brae !haltleft
	fail

; STATE HALT -------------
!haltleft
	left
!halt
	halt
