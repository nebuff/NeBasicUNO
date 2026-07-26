#include <Arduino.h>

#define MAX_LINE_LEN 64
#define NUM_VARS 26

char inputBuffer[MAX_LINE_LEN];
uint8_t bufferPos = 0;
int32_t variables[NUM_VARS]; // Stores variables A-Z

enum Token : uint8_t {
  TOK_UNKNOWN = 0,
  TOK_PRINT,
  TOK_LET,
  TOK_HELP,
  TOK_HIGH,
  TOK_LOW,
  TOK_TOGGLE,
  TOK_READ,
  TOK_AREAD,
  TOK_AWRITE,
  TOK_PEEK,
  TOK_POKE,
  TOK_VARS,
  TOK_RAND,
  TOK_CLEAR,
  TOK_DELAY,
  TOK_TONE,
  TOK_NOTONE,
  TOK_FREE
};

void processCommand(char* line);
Token parseToken(char* word);
void executePrint(char* args);
void executeLet(char* args);
void executePinWrite(char* args, uint8_t state);
void executePinToggle(char* args);
void executePinRead(char* args);
void executeAnalogRead(char* args);
void executeAnalogWrite(char* args);
void executePeek(char* args);
void executePoke(char* args);
void executeVars();
void executeRand(char* args);
void executeClear();
void executeDelay(char* args);
void executeTone(char* args);
void executeNoTone(char* args);
void executeFree();
int32_t parseValue(char* str);

void setup() {
  Serial.begin(9600);
  while (!Serial) { ; } // Wait for serial port connection
  
  executeClear();

  Serial.println(F("\nNeBasic v1.0"));
  Serial.print(F("> "));
}

void loop() {
  while (Serial.available() > 0) {
    char c = Serial.read();

    // Echo character back to terminal
    Serial.write(c);

    if (c == '\r' || c == '\n') {
      Serial.println();
      if (bufferPos > 0) {
        inputBuffer[bufferPos] = '\0';
        processCommand(inputBuffer);
        bufferPos = 0;
      }
      Serial.print(F("> "));
    } else if (c == '\b' || c == 0x7F) { // Handle backspace
      if (bufferPos > 0) {
        bufferPos--;
        Serial.print(F("\b \b"));
      }
    } else {
      if (bufferPos < MAX_LINE_LEN - 1) {
        inputBuffer[bufferPos++] = c;
      }
    }
  }
}

Token parseToken(char* word) {
  if (strcasecmp(word, "PRINT") == 0 || strcasecmp(word, "?") == 0) return TOK_PRINT;
  if (strcasecmp(word, "LET") == 0) return TOK_LET;
  if (strcasecmp(word, "HELP") == 0) return TOK_HELP;
  if (strcasecmp(word, "HIGH") == 0) return TOK_HIGH;
  if (strcasecmp(word, "LOW") == 0) return TOK_LOW;
  if (strcasecmp(word, "TOGGLE") == 0) return TOK_TOGGLE;
  if (strcasecmp(word, "READ") == 0) return TOK_READ;
  if (strcasecmp(word, "AREAD") == 0) return TOK_AREAD;
  if (strcasecmp(word, "AWRITE") == 0) return TOK_AWRITE;
  if (strcasecmp(word, "PEEK") == 0) return TOK_PEEK;
  if (strcasecmp(word, "POKE") == 0) return TOK_POKE;
  if (strcasecmp(word, "VARS") == 0) return TOK_VARS;
  if (strcasecmp(word, "RAND") == 0) return TOK_RAND;
  if (strcasecmp(word, "CLEAR") == 0) return TOK_CLEAR;
  if (strcasecmp(word, "DELAY") == 0) return TOK_DELAY;
  if (strcasecmp(word, "TONE") == 0) return TOK_TONE;
  if (strcasecmp(word, "NOTONE") == 0) return TOK_NOTONE;
  if (strcasecmp(word, "FREE") == 0) return TOK_FREE;
  return TOK_UNKNOWN;
}

void processCommand(char* line) {
  // Trim leading whitespace
  while (*line == ' ') line++;
  if (*line == '\0') return;

  // Extract first word (command keyword)
  char* spacePos = strchr(line, ' ');
  char commandStr[16] = {0};
  
  if (spacePos != NULL) {
    uint8_t len = spacePos - line;
    if (len > 15) len = 15;
    strncpy(commandStr, line, len);
    commandStr[len] = '\0';
  } else {
    strncpy(commandStr, line, 15);
  }

  Token tok = parseToken(commandStr);
  char* args = (spacePos != NULL) ? spacePos + 1 : (char*)"";

  switch (tok) {
    case TOK_PRINT:
      executePrint(args);
      break;

    case TOK_LET:
      executeLet(args);
      break;

    case TOK_HELP:
      Serial.println(F("NeBasic Commands:"));
      Serial.println(F(" PRINT <expr> | LET <A-Z> = <expr> | VARS | CLEAR"));
      Serial.println(F(" HIGH <pin> | LOW <pin> | TOGGLE <pin> | READ <pin>"));
      Serial.println(F(" AREAD <pin> | AWRITE <pin> <val> | TONE <pin> <freq>"));
      Serial.println(F(" NOTONE <pin> | DELAY <ms> | RAND <var> <max>"));
      Serial.println(F(" PEEK <addr> | POKE <addr> <val> | FREE"));
      break;

    case TOK_HIGH:
      executePinWrite(args, HIGH);
      break;

    case TOK_LOW:
      executePinWrite(args, LOW);
      break;

    case TOK_TOGGLE:
      executePinToggle(args);
      break;

    case TOK_READ:
      executePinRead(args);
      break;

    case TOK_AREAD:
      executeAnalogRead(args);
      break;

    case TOK_AWRITE:
      executeAnalogWrite(args);
      break;

    case TOK_PEEK:
      executePeek(args);
      break;

    case TOK_POKE:
      executePoke(args);
      break;

    case TOK_VARS:
      executeVars();
      break;

    case TOK_RAND:
      executeRand(args);
      break;

    case TOK_CLEAR:
      executeClear();
      Serial.println(F("OK"));
      break;

    case TOK_DELAY:
      executeDelay(args);
      break;

    case TOK_TONE:
      executeTone(args);
      break;

    case TOK_NOTONE:
      executeNoTone(args);
      break;

    case TOK_FREE:
      executeFree();
      break;

    default:
      Serial.println(F("Syntax Error"));
      break;
  }
}

int32_t parseValue(char* str) {
  while (*str == ' ') str++;
  if ((str[0] >= 'A' && str[0] <= 'Z') && str[1] == '\0') {
    return variables[str[0] - 'A'];
  }
  if ((str[0] >= 'a' && str[0] <= 'z') && str[1] == '\0') {
    return variables[str[0] - 'a'];
  }
  return atol(str);
}

void executePrint(char* args) {
  Serial.println(parseValue(args));
}

void executeLet(char* args) {
  while (*args == ' ') args++;

  char varName = args[0];
  if ((varName < 'A' || varName > 'Z') && (varName < 'a' || varName > 'z')) {
    Serial.println(F("Err: Invalid Variable"));
    return;
  }

  char* equalsSign = strchr(args, '=');
  if (equalsSign == NULL) {
    Serial.println(F("Err: Expected '='"));
    return;
  }

  int32_t val = parseValue(equalsSign + 1);
  uint8_t index = (varName >= 'a') ? (varName - 'a') : (varName - 'A');
  variables[index] = val;
}

void executePinWrite(char* args, uint8_t state) {
  uint8_t pin = parseValue(args);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, state);
}

void executePinToggle(char* args) {
  uint8_t pin = parseValue(args);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, !digitalRead(pin));
}

void executePinRead(char* args) {
  uint8_t pin = parseValue(args);
  pinMode(pin, INPUT_PULLUP);
  Serial.println(digitalRead(pin));
}

void executeAnalogRead(char* args) {
  uint8_t pin = parseValue(args);
  Serial.println(analogRead(pin));
}

void executeAnalogWrite(char* args) {
  char* spacePos = strchr(args, ' ');
  if (spacePos == NULL) {
    Serial.println(F("Err: Expected <pin> <val>"));
    return;
  }
  uint8_t pin = parseValue(args);
  uint8_t val = parseValue(spacePos + 1);
  pinMode(pin, OUTPUT);
  analogWrite(pin, val);
}

void executePeek(char* args) {
  uint16_t addr = parseValue(args);
  uint8_t* ptr = (uint8_t*)addr;
  Serial.println(*ptr);
}

void executePoke(char* args) {
  char* spacePos = strchr(args, ' ');
  if (spacePos == NULL) {
    Serial.println(F("Err: Expected <addr> <val>"));
    return;
  }
  uint16_t addr = parseValue(args);
  uint8_t val = parseValue(spacePos + 1);
  uint8_t* ptr = (uint8_t*)addr;
  *ptr = val;
}

void executeVars() {
  for (uint8_t i = 0; i < NUM_VARS; i++) {
    if (variables[i] != 0) {
      Serial.print((char)('A' + i));
      Serial.print(F(" = "));
      Serial.println(variables[i]);
    }
  }
}

void executeRand(char* args) {
  while (*args == ' ') args++;
  char varName = args[0];
  if ((varName < 'A' || varName > 'Z') && (varName < 'a' || varName > 'z')) {
    Serial.println(F("Err: Invalid Variable"));
    return;
  }

  char* spacePos = strchr(args, ' ');
  if (spacePos == NULL) {
    Serial.println(F("Err: Expected <var> <max>"));
    return;
  }

  int32_t maxVal = parseValue(spacePos + 1);
  uint8_t index = (varName >= 'a') ? (varName - 'a') : (varName - 'A');
  variables[index] = random(0, maxVal);
}

void executeClear() {
  for (uint8_t i = 0; i < NUM_VARS; i++) {
    variables[i] = 0;
  }
}

void executeDelay(char* args) {
  uint32_t ms = parseValue(args);
  delay(ms);
}

void executeTone(char* args) {
  char* spacePos = strchr(args, ' ');
  if (spacePos == NULL) {
    Serial.println(F("Err: Expected <pin> <freq>"));
    return;
  }
  uint8_t pin = parseValue(args);
  uint16_t freq = parseValue(spacePos + 1);
  tone(pin, freq);
}

void executeNoTone(char* args) {
  uint8_t pin = parseValue(args);
  noTone(pin);
}

void executeFree() {
  extern int __heap_start, *__brkval;
  int v;
  int freeRam = (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
  Serial.print(freeRam);
  Serial.println(F(" bytes"));
}
