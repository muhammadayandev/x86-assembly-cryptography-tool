; io_helpers.inc - Colored Dashboard Menu, Input, and Windows GUI Output Saving

; Clears screen and shows colored dashboard menu. Returns choice in EAX.
ShowMenu PROC
    MOV  EAX, THEME_NORMAL
    CALL SetTextColor
    CALL Clrscr

    MOV  EAX, THEME_BANNER
    CALL SetTextColor
    MOV  EDX, OFFSET bannerTop
    CALL WriteString

    MOV  EAX, THEME_MENU
    CALL SetTextColor
    MOV  EDX, OFFSET menuOptions
    CALL WriteString

    MOV  EAX, THEME_PROMPT
    CALL SetTextColor
    MOV  EDX, OFFSET msgSelectOpt
    CALL WriteString

    MOV  EAX, THEME_NORMAL
    CALL SetTextColor
    CALL ReadInt
    RET
ShowMenu ENDP

; Displays error message in EDX in red and shows a Windows GUI popup alert
PrintError PROC
    PUSH EAX
    PUSH EBX
    MOV  EAX, THEME_ERROR
    CALL SetTextColor
    CALL WriteString
    MOV  EBX, OFFSET guiTitle
    CALL MsgBox
    MOV  EAX, THEME_NORMAL
    CALL SetTextColor
    POP  EBX
    POP  EAX
    RET
PrintError ENDP

; Reads text from keyboard or file into inputBuffer.
; Returns EAX = 1 if ok, EAX = 0 on error.
ReadInput PROC
    PUSH EBX
    PUSH ECX
    PUSH EDX

    MOV  EAX, THEME_PROMPT
    CALL SetTextColor
    MOV  EDX, OFFSET msgInputSel
    CALL WriteString
    MOV  EAX, THEME_NORMAL
    CALL SetTextColor
    CALL ReadInt

    CMP  EAX, 1
    JE   FromKeyboard
    CMP  EAX, 2
    JE   FromFile

    MOV  EDX, OFFSET errChoice
    CALL PrintError
    MOV  EAX, 0
    JMP  ReadDone

FromKeyboard:
    MOV  EAX, THEME_PROMPT
    CALL SetTextColor
    MOV  EDX, OFFSET msgEnterText
    CALL WriteString
    MOV  EAX, THEME_NORMAL
    CALL SetTextColor
    MOV  EDX, OFFSET inputBuffer
    MOV  ECX, MAX_TEXT
    CALL ReadString
    CMP  EAX, 0
    JE   EmptyErr
    MOV  textLength, EAX
    MOV  BYTE PTR inputBuffer[EAX], 0
    MOV  EAX, 1
    JMP  ReadDone

FromFile:
    MOV  EAX, THEME_PROMPT
    CALL SetTextColor
    MOV  EDX, OFFSET msgEnterFile
    CALL WriteString
    MOV  EAX, THEME_NORMAL
    CALL SetTextColor
    MOV  EDX, OFFSET fileName
    MOV  ECX, 259
    CALL ReadString
    CMP  EAX, 0
    JE   OpenErr

    MOV  EDX, OFFSET fileName
    CALL OpenInputFile
    CMP  EAX, INVALID_HANDLE_VALUE
    JE   OpenErr
    MOV  fileHandle, EAX

    ; Read MAX_TEXT + 1 to check if file is bigger than 4096 bytes
    MOV  EAX, fileHandle
    MOV  EDX, OFFSET inputBuffer
    MOV  ECX, MAX_TEXT + 1
    CALL ReadFromFile
    JC   ReadErr
    PUSH EAX
    MOV  EAX, fileHandle
    CALL CloseFile
    POP  EAX

    CMP  EAX, 0
    JE   EmptyErr
    CMP  EAX, MAX_TEXT
    JG   BigErr

    MOV  textLength, EAX
    MOV  BYTE PTR inputBuffer[EAX], 0
    MOV  EAX, 1
    JMP  ReadDone

OpenErr:
    MOV  EDX, OFFSET errFileOpen
    CALL PrintError
    MOV  EAX, 0
    JMP  ReadDone

ReadErr:
    MOV  EAX, fileHandle
    CALL CloseFile
    MOV  EDX, OFFSET errFileRead
    CALL PrintError
    MOV  EAX, 0
    JMP  ReadDone

EmptyErr:
    MOV  EDX, OFFSET errFileEmpty
    CALL PrintError
    MOV  EAX, 0
    JMP  ReadDone

BigErr:
    MOV  EDX, OFFSET errFileBig
    CALL PrintError
    MOV  EAX, 0

ReadDone:
    POP  EDX
    POP  ECX
    POP  EBX
    RET
ReadInput ENDP

; Converts number in EAX to ASCII digits at [EDI]
AppendDec PROC
    PUSH EAX
    PUSH EBX
    PUSH ECX
    PUSH EDX

    MOV  EBX, 10
    MOV  ECX, 0
DivLoop:
    MOV  EDX, 0
    DIV  EBX
    ADD  DL, '0'
    PUSH EDX
    INC  ECX
    CMP  EAX, 0
    JNE  DivLoop

PopDigits:
    POP  EAX
    MOV  [EDI], AL
    INC  EDI
    LOOP PopDigits

    POP  EDX
    POP  ECX
    POP  EBX
    POP  EAX
    RET
AppendDec ENDP

; Copies null-terminated string from [ESI] to [EDI]
AppendStr PROC
    PUSH EAX
CopyLoop:
    MOV  AL, [ESI]
    CMP  AL, 0
    JE   CopyDone
    MOV  [EDI], AL
    INC  ESI
    INC  EDI
    JMP  CopyLoop
CopyDone:
    POP  EAX
    RET
AppendStr ENDP

; Asks user via Windows GUI Yes/No Popup (MsgBoxAsk) whether to save to output.txt.
; Stack args: [EBP+8] = buffer offset, [EBP+12] = byte count
WriteOutput PROC
    PUSH EBP
    MOV  EBP, ESP
    PUSH EAX
    PUSH EBX
    PUSH ECX
    PUSH EDX

    MOV  EBX, OFFSET guiTitle
    MOV  EDX, OFFSET guiSaveAsk
    CALL MsgBoxAsk
    CMP  EAX, IDYES
    JNE  WriteDone

    MOV  EDX, OFFSET outFileName
    CALL CreateOutputFile
    CMP  EAX, INVALID_HANDLE_VALUE
    JE   WriteErr
    MOV  fileHandle, EAX

    MOV  EAX, fileHandle
    MOV  EDX, [EBP + 8]
    MOV  ECX, [EBP + 12]
    CALL WriteToFile
    JC   CloseAndErr
    CMP  EAX, [EBP + 12]
    JNE  CloseAndErr

    MOV  EAX, fileHandle
    CALL CloseFile

    MOV  EAX, THEME_RESULT
    CALL SetTextColor
    MOV  EDX, OFFSET msgSaved
    CALL WriteString
    MOV  EBX, OFFSET guiTitle
    MOV  EDX, OFFSET guiSavedMsg
    CALL MsgBox
    JMP  WriteDone

CloseAndErr:
    MOV  EAX, fileHandle
    CALL CloseFile
WriteErr:
    MOV  EDX, OFFSET errFileWrite
    CALL PrintError

WriteDone:
    MOV  EAX, THEME_NORMAL
    CALL SetTextColor
    CALL Crlf
    CALL WaitMsg
    POP  EDX
    POP  ECX
    POP  EBX
    POP  EAX
    POP  EBP
    RET  8
WriteOutput ENDP
