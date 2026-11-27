; stats.inc - Letter Frequency Analysis and Character Statistics

FrequencyAnalysis PROC
    PUSH EAX
    PUSH EBX
    PUSH ECX
    PUSH EDX
    PUSH ESI
    PUSH EDI

    MOV  ECX, 26
    MOV  ESI, 0
ResetFreq:
    MOV  frequencyArray[ESI * 4], 0
    INC  ESI
    LOOP ResetFreq

    MOV  ECX, textLength
    MOV  ESI, 0
CountFreq:
    MOV  EAX, 0
    MOV  AL, inputBuffer[ESI]
    CMP  AL, 'A'
    JL   SkipFreq
    CMP  AL, 'Z'
    JLE  UpperFreq
    CMP  AL, 'a'
    JL   SkipFreq
    CMP  AL, 'z'
    JG   SkipFreq
    SUB  AL, 'a'
    JMP  AddCount
UpperFreq:
    SUB  AL, 'A'
AddCount:
    INC  frequencyArray[EAX * 4]
SkipFreq:
    INC  ESI
    DEC  ECX
    JNZ  CountFreq

    MOV  EAX, THEME_RESULT
    CALL SetTextColor
    MOV  EDX, OFFSET msgResult
    CALL WriteString
    LEA  EDI, outputBuffer
    MOV  EBX, 0

PrintFreq:
    MOV  AL, BL
    ADD  AL, 'A'
    CALL WriteChar
    MOV  [EDI], AL
    INC  EDI

    MOV  EDX, OFFSET strColonSp
    CALL WriteString
    MOV  ESI, OFFSET strColonSp
    CALL AppendStr

    MOV  EAX, frequencyArray[EBX * 4]
    CALL WriteDec
    CALL Crlf
    CALL AppendDec

    MOV  BYTE PTR [EDI], 13
    INC  EDI
    MOV  BYTE PTR [EDI], 10
    INC  EDI

    INC  EBX
    CMP  EBX, 26
    JL   PrintFreq

    LEA  EAX, outputBuffer
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
FrequencyAnalysis ENDP

; Counts total, uppercase, lowercase, digits, spaces (ASCII 32),
; and special characters (punctuation, tabs, CR/LF line breaks).
FileStats PROC
    PUSH EAX
    PUSH ECX
    PUSH EDX
    PUSH ESI
    PUSH EDI

    MOV  totalCount, 0
    MOV  upperCount, 0
    MOV  lowerCount, 0
    MOV  digitCount, 0
    MOV  spaceCount, 0
    MOV  specialCount, 0

    MOV  ECX, textLength
    MOV  totalCount, ECX
    MOV  ESI, 0

StatsLoop:
    MOV  AL, inputBuffer[ESI]

    CMP  AL, 'A'
    JL   CheckDigit
    CMP  AL, 'Z'
    JLE  AddUpper
    CMP  AL, 'a'
    JL   AddSpecial
    CMP  AL, 'z'
    JLE  AddLower
    JMP  AddSpecial

CheckDigit:
    CMP  AL, '0'
    JL   CheckSpace
    CMP  AL, '9'
    JLE  AddDigit

CheckSpace:
    CMP  AL, ' '
    JE   AddSpace
    JMP  AddSpecial

AddUpper:
    INC  upperCount
    JMP  NextChar
AddLower:
    INC  lowerCount
    JMP  NextChar
AddDigit:
    INC  digitCount
    JMP  NextChar
AddSpace:
    INC  spaceCount
    JMP  NextChar
AddSpecial:
    INC  specialCount

NextChar:
    INC  ESI
    DEC  ECX
    JNZ  StatsLoop

    MOV  EAX, THEME_RESULT
    CALL SetTextColor
    MOV  EDX, OFFSET msgResult
    CALL WriteString
    LEA  EDI, outputBuffer

    MOV  EDX, OFFSET lblTotal
    MOV  EAX, totalCount
    CALL WriteStatLine

    MOV  EDX, OFFSET lblUpper
    MOV  EAX, upperCount
    CALL WriteStatLine

    MOV  EDX, OFFSET lblLower
    MOV  EAX, lowerCount
    CALL WriteStatLine

    MOV  EDX, OFFSET lblDigits
    MOV  EAX, digitCount
    CALL WriteStatLine

    MOV  EDX, OFFSET lblSpaces
    MOV  EAX, spaceCount
    CALL WriteStatLine

    MOV  EDX, OFFSET lblSpecial
    MOV  EAX, specialCount
    CALL WriteStatLine

    LEA  EAX, outputBuffer
    SUB  EDI, EAX
    PUSH EDI
    PUSH EAX
    CALL WriteOutput

    POP  EDI
    POP  ESI
    POP  EDX
    POP  ECX
    POP  EAX
    RET
FileStats ENDP

WriteStatLine PROC
    CALL WriteString
    CALL WriteDec
    CALL Crlf
    MOV  ESI, EDX
    CALL AppendStr
    CALL AppendDec
    MOV  BYTE PTR [EDI], 13
    INC  EDI
    MOV  BYTE PTR [EDI], 10
    INC  EDI
    RET
WriteStatLine ENDP
