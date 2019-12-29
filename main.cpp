#include <Arduino.h>
#include <Keypad.h>
#include <Eventually.h>

const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};
byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8};    //connect to the column pinouts of the keypad
int speakerPin = 9;
int Leds[] = {13, 12, 16, 10, 17,15};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

int length = 26;
char notes[] = "eeeeeeegcde fffffeeeeddedg";
int beats[] = {1, 1, 2, 1, 1, 2, 1, 1, 1, 1, 4, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2};

int winnerBeats[]={1,1,1,1,1,4,2};
char winnerSongNotes[]="fffbaC ";

void playTone(int tonee, int duration, int ledIndex);
void playNote(char note, int duration);
void playKeypadNote(char number, int duration);
bool guessSong();
bool playSong(int songNum);
bool isNoteCorrect(char key, int allBtnPushes);
void resetCounterVariables();
void winnerSong();
void loserSong();
void blinkLed(int indexLed, int duration);

int tempo = 300;
int buttonPush = 0;
int allBtnPushes = 0;
int correctBtnPushes = 0;
int wrongBtnPushes = 0;
int maxBtnPushes = 26;
EvtManager mgr;

void setup()
{
  pinMode(speakerPin, OUTPUT);
  pinMode(Leds[0], OUTPUT);
  pinMode(Leds[1], OUTPUT);
  pinMode(Leds[2], OUTPUT);
  pinMode(Leds[3], OUTPUT);
  pinMode(Leds[4], OUTPUT);
  pinMode(Leds[5], OUTPUT);
  Serial.begin(9600);
  playSong(1);
}

void playTone(int tonee, int duration, int ledIndex)
{
  tone(speakerPin, tonee, duration);
  blinkLed(ledIndex,duration);
  delay(duration);
}

void playKeypadNote(char keypadNumber, int duration)
{
  char numbers[] = { '1', '2', '3', '4', '5', '6', '7', '8'};
  int tones[] = {262, 294, 330, 349, 392, 440, 494, 523};

  if (keypadNumber == '*')
  {
    delay(4 * tempo);
  }
  else
  {
    for (int i = 0; i < 8; i++)
    {
      if (numbers[i] == keypadNumber)
      {
        playTone(tones[i], duration, i);
      }
    }
  }
}

void playNote(char note, int duration)
{
  char names[] = {'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C'};
  int tones[] = {262, 294, 330, 349, 392, 440, 494, 523};

  for (int i = 0; i < 8; i++)
  {
    if (note == names[i])
    {
      playTone(tones[i], duration, i);
      }
  }
}

bool guessSong()
{
  mgr.resetContext();
  
  while (allBtnPushes < maxBtnPushes)
  {
    char key = keypad.getKey();
    if (key)
    {
      allBtnPushes += 1;
     
      Serial.println("All button pushes:");
      Serial.println(allBtnPushes);

      playKeypadNote(key, 1 * tempo);

      if (isNoteCorrect(key, allBtnPushes) == true)
      {
        correctBtnPushes += 1;

        Serial.println("Correct button pushes:");
        Serial.println(correctBtnPushes);
      }
    }
  }
  if (allBtnPushes == correctBtnPushes && allBtnPushes == maxBtnPushes)
  {
    winnerSong();
    resetCounterVariables();
    return playSong(2);
  }
  else if (allBtnPushes != correctBtnPushes && allBtnPushes == maxBtnPushes)
  {
    loserSong();
    resetCounterVariables();
    return playSong(1);
  }
}

bool isNoteCorrect(char keypadNumber, int allBtnPushes)
{
  Serial.println("keypad number");
  Serial.println(keypadNumber);
  
  char keypadNums[] = { '1', '2', '3', '4', '5', '6', '7','8', '*'};
  char names[] = {'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C', ' '};
  int arrKeypadNumsLength = sizeof(keypadNums) / sizeof(keypadNums[0]);
  int indexOfKey;
  for (int i = 0; i < arrKeypadNumsLength; i++)
  {
    if (keypadNums[i] == keypadNumber)
    {
      indexOfKey = i;
      Serial.println("notes[allbtnps]");
      Serial.println(notes[allBtnPushes-1]);

    if (notes[allBtnPushes-1] == names[i])
    {
      return true;
    }
    else
    {
      return false;
    }
    }
  }    
  return false;
}

bool playSong(int songNum)
{
  mgr.resetContext();
  if(songNum==2)
{
  for (int i = 0; i < 7; i++)
  {
    if (winnerSongNotes[i] == ' ')
    {
      delay(winnerBeats[i] * tempo);
    }
    else
    {
      playNote(winnerSongNotes[i], winnerBeats[i] * 100);
    }
    delay(tempo / 2);
  }
}
  for (int i = 0; i < length; i++)
  {
    if (notes[i] == ' ')
    {
      delay(beats[i] * tempo);
    }
    else
    {
      playNote(notes[i], beats[i] * tempo);
    }
   delay(tempo / 2);
  }

  mgr.addListener(new EvtTimeListener(200, false, (EvtAction)guessSong));

  return true;
}

void resetCounterVariables()
{
  allBtnPushes = 0;
  correctBtnPushes = 0;
  wrongBtnPushes = 0;
}
void winnerSong()
{
  playSong(2);
}
void loserSong()
{
  tone(speakerPin, 2000, 900);
  delay(1000);
  tone(speakerPin, 1200, 900);
  delay(1200);
}
void blinkLed(int indexLed, int duration)
{
  digitalWrite(Leds[indexLed], HIGH);
  delay(100);
  digitalWrite(Leds[indexLed], LOW); 
}

USE_EVENTUALLY_LOOP(mgr)
