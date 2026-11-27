; bruteforce.inc - Caesar Brute Force (Shifts 0 to 25)

BruteForce PROC
    PUSH EAX
    PUSH EBX
    PUSH ECX
    PUSH EDX
    PUSH ESI
    PUSH EDI

    MOV  EAX, THEME_RESULT
    CALL SetTextColor
    MOV  EDX, OFFSET msgResult
    CALL WriteString

    MOV  modeFlag, 1
    MOV  EBX, 0
    LEA  EDI, outputBuffer + 4100

BruteLoop:
    MOV  caesarKey, EBX
    CALL CaesarCipher

    MOV  EDX, OFFSET strShift
    CALL WriteString
    MOV  EAX, EBX
    CALL WriteDec
    MOV  EDX, OFFSET strColonSp
    CALL WriteString
    MOV  EDX, OFFSET outputBuffer
    CALL WriteString
    CALL Crlf

    ; Save line into report buffer
    MOV  ESI, OFFSET strShift
    CALL AppendStr
    MOV  EAX, EBX
    CALL AppendDec
    MOV  ESI, OFFSET strColonSp
    CALL AppendStr

    MOV  ESI, 0
    MOV  ECX, textLength
CopyLineLoop:
    MOV  AL, outputBuffer[ESI]
    MOV  [EDI], AL
    INC  ESI
    INC  EDI
    LOOP CopyLineLoop

    MOV  BYTE PTR [EDI], 13
    INC  EDI
    MOV  BYTE PTR [EDI], 10
    INC  EDI

    INC  EBX
    CMP  EBX, 26
    JL   BruteLoop

    LEA  EAX, outputBuffer + 4100
    SUB  EDI, EAX
    PUSH EDI
    PUSH EAX
    CALL WriteOutput

    POP  EDI
    POP  ESI
    POP  EDX
    POP  ECX
    POP  EBX
    POP  EAX
    RET
BruteForce ENDP
