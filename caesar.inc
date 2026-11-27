; caesar.inc - Caesar Cipher Encryption and Decryption

CaesarCipher PROC
    PUSH EAX
    PUSH EBX
    PUSH ECX
    PUSH ESI

    MOV  ECX, textLength
    MOV  ESI, 0
    MOV  BL, BYTE PTR caesarKey

CaesarLoop:
    MOV  AL, inputBuffer[ESI]

    CMP  AL, 'A'
    JL   StoreChar
    CMP  AL, 'Z'
    JLE  UpperCaesar
    CMP  AL, 'a'
    JL   StoreChar
    CMP  AL, 'z'
    JLE  LowerCaesar
    JMP  StoreChar

UpperCaesar:
    SUB  AL, 'A'
    CMP  modeFlag, 0
    JNE  UpperDec
    ADD  AL, BL
    CMP  AL, 26
    JL   UpperDone
    SUB  AL, 26
    JMP  UpperDone
UpperDec:
    ADD  AL, 26
    SUB  AL, BL
    CMP  AL, 26
    JL   UpperDone
    SUB  AL, 26
UpperDone:
    ADD  AL, 'A'
    JMP  StoreChar

LowerCaesar:
    SUB  AL, 'a'
    CMP  modeFlag, 0
    JNE  LowerDec
    ADD  AL, BL
    CMP  AL, 26
    JL   LowerDone
    SUB  AL, 26
    JMP  LowerDone
LowerDec:
    ADD  AL, 26
    SUB  AL, BL
    CMP  AL, 26
    JL   LowerDone
    SUB  AL, 26
LowerDone:
    ADD  AL, 'a'

StoreChar:
    MOV  outputBuffer[ESI], AL
    INC  ESI
    DEC  ECX
    JNZ  CaesarLoop

    MOV  BYTE PTR outputBuffer[ESI], 0
    MOV  outLength, ESI

    POP  ESI
    POP  ECX
    POP  EBX
    POP  EAX
    RET
CaesarCipher ENDP
