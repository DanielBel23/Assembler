;1 missing comma
MAIN: add    r3    LIST
;-----------------------------

;2 illegal comma
add ,   r3  ,  LIST
;-----------------------------

;3 consecutive comma
add  r3  ,  ,  LIST
;-----------------------------

;4 consecutive comma and missing comma ?
add ,  , r3    LIST
;-----------------------------

;5 extra text
add  r3 ,LIST fd
;-----------------------------

;6 too many arguments
add  r3 ,LIST ,r4
;-----------------------------

;7 too little arguments
add  r3
;-----------------------------

;8 ??
add  r1 		,r3 ,
;-----------------------------

;9invalid string
STR: .string "abcd" fdf
;-----------------------------

;10 duplicate label
MAIN: add    r3 ,   LIST
;-----------------------------

;11 invalid label name - starts with Integer
9main: prn #7
;-----------------------------

;12 invalid label name - reserved word (command)
string: sub r1, r4
;-----------------------------

;13 invalid label name - macro name
macr        m_macr
cmp r3, #-6
bne END
endmacr
m_macr: sub r1, r4
;-----------------------------

;14 integer out of range
prn #50000000000
;-------------------

;15 invalid operand name
jmp *r9
;-------------------


