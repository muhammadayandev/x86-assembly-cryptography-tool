; vigenere.inc - Key Normalization and Vigenere Cipher

; Checks if keyBuffer has only letters and converts lowercase to uppercase.
; Returns EAX = 1 if valid, EAX = 0 if invalid.
NormalizeKey PROC
    PUSH ECX
    PUSH ESI

    MOV  ECX, keyLength
    CMP  ECX, 0
    JE   KeyInvalid

    MOV  ESI, 0
KeyLoop:
    MOV  AL, keyBuffer[ESI]
    CMP  AL, 'A'
    JL   KeyInvalid
    CMP  AL, 'Z'
    JLE  KeyCharOk
    CMP  AL, 'a'
    JL   KeyInvalid
    CMP  AL, 'z'
    JG   KeyInvalid
    SUB  AL, 32
    MOV  keyBuffer[ESI], AL

KeyCharOk:
    INC  ESI
    LOOP KeyLoop

    MOV  EAX, 1
    JMP  KeyExit

KeyInvalid:
    MOV  EAX, 0

KeyExit:
    POP  ESI
    POP  ECX
    RET
NormalizeKey ENDP

VigenereCipher PROC
    PUSH EAX
    PUSH EBX
    PUSH ECX
    PUSH ESI
    PUSH EDI

    MOV  ECX, textLength
    MOV  ESI, 0
    MOV  EDI, 0

VigLoop:
    MOV  AL, inputBuffer[ESI]

    CMP  AL, 'A'
    JL   VigStore
    CMP  AL, 'Z'
    JLE  VigUpper
    CMP  AL, 'a'
    JL   VigStore
    CMP  AL, 'z'
    JLE  VigLower
    JMP  VigStore

VigUpper:
    MOV  BL, keyBuffer[EDI]
    SUB  BL, 'A'
    SUB  AL, 'A'
    CMP  modeFlag, 0
    JNE  VigUpperDec
    ADD  AL, BL
    CMP  AL, 26
    JL   VigUpperWrap
    SUB  AL, 26
    JMP  VigUpperWrap
VigUpperDec:
    ADD  AL, 26
    SUB  AL, BL
    CMP  AL, 26
    JL   VigUpperWrap
    SUB  AL, 26
VigUpperWrap:
    ADD  AL, 'A'
    JMP  NextKeyChar

VigLower:
    MOV  BL, keyBuffer[EDI]
    SUB  BL, 'A'
    SUB  AL, 'a'
    CMP  modeFlag, 0
    JNE  VigLowerDec
    ADD  AL, BL
    CMP  AL, 26
    JL   VigLowerWrap
    SUB  AL, 26
    JMP  VigLowerWrap
VigLowerDec:
    ADD  AL, 26
    SUB  AL, BL
    CMP  AL, 26
    JL   VigLowerWrap
    SUB  AL, 26
VigLowerWrap:
    ADD  AL, 'a'

NextKeyChar:
    INC  EDI
    CMP  EDI, keyLength
    JL   VigStore
    MOV  EDI, 0

VigStore:
    MOV  outputBuffer[ESI], AL
    INC  ESI
    DEC  ECX
    JNZ  VigLoop

    MOV  BYTE PTR outputBuffer[ESI], 0
    MOV  outLength, ESI

    POP  EDI
    POP  ESI
    POP  ECX
    POP  EBX
    POP  EAX
    RET
VigenereCipher ENDP
