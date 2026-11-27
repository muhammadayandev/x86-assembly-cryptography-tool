INCLUDE Irvine32.inc

MAX_TEXT = 4096
MAX_KEY  = 128

; Clean Console Color Themes
THEME_BANNER = white + (blue * 16)
THEME_MENU   = lightCyan + (black * 16)
THEME_PROMPT = yellow + (black * 16)
THEME_RESULT = lightGreen + (black * 16)
THEME_ERROR  = lightRed + (black * 16)
THEME_NORMAL = lightGray + (black * 16)

.data
bannerTop    BYTE "========================================================",13,10
             BYTE "       COAL CRYPTOGRAPHY TOOL  (BCS-3C | GROUP G1)      ",13,10
             BYTE "========================================================",13,10,0

menuOptions  BYTE 13,10
             BYTE "   [1]  Encrypt Text / File (Caesar or Vigenere)",13,10
             BYTE "   [2]  Decrypt Text / File (Caesar or Vigenere)",13,10
             BYTE "   [3]  Caesar Brute Force Attack (All 26 Shifts)",13,10
             BYTE "   [4]  Letter Frequency Analysis (A - Z)",13,10
             BYTE "   [5]  Character & File Statistics",13,10
             BYTE "   [6]  Exit Application",13,10,13,10
             BYTE "--------------------------------------------------------",13,10,0

msgSelectOpt BYTE "Select option (1-6): ",0
msgCipherSel BYTE 13,10,"Select Cipher (1 = Caesar, 2 = Vigenere): ",0
msgInputSel  BYTE 13,10,"Select Input Method (1 = Keyboard, 2 = File): ",0
msgEnterText BYTE "Enter text: ",0
msgEnterFile BYTE "Enter input filename (e.g. input.txt): ",0
msgEnterCKey BYTE "Enter Caesar key (1-25): ",0
msgEnterVKey BYTE "Enter Vigenere keyword (letters only): ",0
msgResult    BYTE 13,10,"==================== OUTPUT RESULT =====================",13,10,0

; Windows GUI Popup Titles and Messages (for Irvine32 MsgBox & MsgBoxAsk)
guiTitle     BYTE "COAL Cryptography Tool - Group G1",0
guiSaveAsk   BYTE "Operation completed!",13,10,13,10
             BYTE "Do you want to save this result to output.txt?",0
guiSavedMsg  BYTE "Result has been saved to output.txt successfully!",0
msgSaved     BYTE 13,10,"[OK] Saved to output.txt.",13,10,0

errChoice    BYTE "Invalid option. Please enter a valid number.",13,10,0
errKey       BYTE "Invalid key. Caesar: 1-25, Vigenere: letters A-Z only.",13,10,0
errFileOpen  BYTE "Error: Could not open input file.",13,10,0
errFileRead  BYTE "Error: Could not read input file.",13,10,0
errFileEmpty BYTE "Error: Input cannot be empty.",13,10,0
errFileBig   BYTE "Error: File is larger than 4096 bytes.",13,10,0
errFileWrite BYTE "Error: Could not write to output.txt.",13,10,0

strShift     BYTE "Shift ",0
strColonSp   BYTE " : ",0
lblTotal     BYTE "Total characters   : ",0
lblUpper     BYTE "Uppercase letters  : ",0
lblLower     BYTE "Lowercase letters  : ",0
lblDigits    BYTE "Digits             : ",0
lblSpaces    BYTE "Spaces             : ",0
lblSpecial   BYTE "Special characters : ",0

outFileName  BYTE "output.txt",0
fileName     BYTE 260 DUP(0)
inputBuffer  BYTE MAX_TEXT + 2 DUP(0)
outputBuffer BYTE 120000 DUP(0)
keyBuffer    BYTE MAX_KEY + 1 DUP(0)

textLength   DWORD 0
outLength    DWORD 0
keyLength    DWORD 0
menuChoice   DWORD 0
caesarKey    DWORD 0
modeFlag     DWORD 0        ; 0 = Encrypt, 1 = Decrypt
fileHandle   DWORD 0

frequencyArray DWORD 26 DUP(0)

totalCount   DWORD 0
upperCount   DWORD 0
lowerCount   DWORD 0
digitCount   DWORD 0
spaceCount   DWORD 0
specialCount DWORD 0

.code
INCLUDE io_helpers.inc
INCLUDE caesar.inc
INCLUDE vigenere.inc
INCLUDE bruteforce.inc
INCLUDE stats.inc

main PROC
MenuLoop:
    CALL ShowMenu
    MOV  menuChoice, EAX

    CMP  EAX, 1
    JE   DoEncrypt
    CMP  EAX, 2
    JE   DoDecrypt
    CMP  EAX, 3
    JE   DoBrute
    CMP  EAX, 4
    JE   DoFreq
    CMP  EAX, 5
    JE   DoStats
    CMP  EAX, 6
    JE   ExitApp

    MOV  EDX, OFFSET errChoice
    CALL PrintError
    JMP  MenuLoop

DoEncrypt:
    MOV  modeFlag, 0
    JMP  PickCipher

DoDecrypt:
    MOV  modeFlag, 1

PickCipher:
    MOV  EAX, THEME_PROMPT
    CALL SetTextColor
    MOV  EDX, OFFSET msgCipherSel
    CALL WriteString
    MOV  EAX, THEME_NORMAL
    CALL SetTextColor
    CALL ReadInt
    CMP  EAX, 1
    JE   RunCaesar
    CMP  EAX, 2
    JE   RunVigenere
    MOV  EDX, OFFSET errChoice
    CALL PrintError
    JMP  MenuLoop

RunCaesar:
    CALL ReadInput
    CMP  EAX, 0
    JE   MenuLoop
GetCKey:
    MOV  EAX, THEME_PROMPT
    CALL SetTextColor
    MOV  EDX, OFFSET msgEnterCKey
    CALL WriteString
    MOV  EAX, THEME_NORMAL
    CALL SetTextColor
    CALL ReadInt
    CMP  EAX, 1
    JL   BadCKey
    CMP  EAX, 25
    JG   BadCKey
    MOV  caesarKey, EAX
    CALL CaesarCipher

    MOV  EAX, THEME_RESULT
    CALL SetTextColor
    MOV  EDX, OFFSET msgResult
    CALL WriteString
    MOV  EDX, OFFSET outputBuffer
    CALL WriteString
    CALL Crlf

    PUSH textLength
    PUSH OFFSET outputBuffer
    CALL WriteOutput
    JMP  MenuLoop
BadCKey:
    MOV  EDX, OFFSET errKey
    CALL PrintError
    JMP  GetCKey

RunVigenere:
    CALL ReadInput
    CMP  EAX, 0
    JE   MenuLoop
GetVKey:
    MOV  EAX, THEME_PROMPT
    CALL SetTextColor
    MOV  EDX, OFFSET msgEnterVKey
    CALL WriteString
    MOV  EAX, THEME_NORMAL
    CALL SetTextColor
    MOV  EDX, OFFSET keyBuffer
    MOV  ECX, MAX_KEY
    CALL ReadString
    MOV  keyLength, EAX
    CALL NormalizeKey
    CMP  EAX, 0
    JE   BadVKey
    CALL VigenereCipher

    MOV  EAX, THEME_RESULT
    CALL SetTextColor
    MOV  EDX, OFFSET msgResult
    CALL WriteString
    MOV  EDX, OFFSET outputBuffer
    CALL WriteString
    CALL Crlf

    PUSH textLength
    PUSH OFFSET outputBuffer
    CALL WriteOutput
    JMP  MenuLoop
BadVKey:
    MOV  EDX, OFFSET errKey
    CALL PrintError
    JMP  GetVKey

DoBrute:
    CALL ReadInput
    CMP  EAX, 0
    JE   MenuLoop
    CALL BruteForce
    JMP  MenuLoop

DoFreq:
    CALL ReadInput
    CMP  EAX, 0
    JE   MenuLoop
    CALL FrequencyAnalysis
    JMP  MenuLoop

DoStats:
    CALL ReadInput
    CMP  EAX, 0
    JE   MenuLoop
    CALL FileStats
    JMP  MenuLoop

ExitApp:
    MOV  EAX, THEME_NORMAL
    CALL SetTextColor
    INVOKE ExitProcess, 0
main ENDP

END main
