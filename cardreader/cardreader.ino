// http://emssa.net/source/images/new_pdf/appnote_2.pdf
const int BADGE_LENGTH = 19;
const int START = 5;
const int cardPresentPin = 13;
const int clockPin = 3;
const int dataPin = 2;

unsigned int count = 0;
byte data[BADGE_LENGTH];
unsigned long milli;
unsigned long lastmilli;

void setup()
{
  Serial.begin(9600);
  milli = millis();
  lastmilli = 0;
  count = 0;

  for(int i = 0; i < BADGE_LENGTH; i++)
    data[i] = 0x0;

  pinMode(cardPresentPin, INPUT);
  pinMode(dataPin, INPUT);
  pinMode(clockPin, INPUT);
  attachInterrupt(1, readBit, RISING);
}

void loop()
{
  milli = millis();
  if((milli - lastmilli) > 25){
    if (count != 0)
    {
      Serial.println(count);
      printDataRead(data);
      printBadgeNumber(data);

      count = 0;
    }
    lastmilli = milli;
  }
}

void readBit()
{
  boolean low = digitalRead(dataPin) == LOW;
  int index = (count / 5) - START;

  if(index >= 0 && index < BADGE_LENGTH){
    int remainder = count % 5;
    if(remainder == 4){
      data[index] >>= 4;
    }
    else {
      data[index] >>= 1;

      if(low){
        data[index] |= B10000000;
      }
    }
  }

  count++;
  lastmilli = milli;
}

void printDataRead(byte d[]){
  for(int i = 0; i < BADGE_LENGTH; i++){
    Serial.print(d[i], HEX);
  }
  Serial.println();
}

void printBadgeNumber(byte data[]){
  // The position just before the F sentinel:
  int last = 16;
  // Store our re-grouped hex values here
  byte sets[9] = {0,0,0,0,0,0,0,0,0};
  
  //Remove parity bit at end
  sets[8] |= data[last] >> 1    & B00000011;
  sets[8] |= data[last-1] << 2  & B00001100;

  sets[7] |= data[last-1] >> 2  & B00000001;
  sets[7] |= data[last-2] << 1  & B00001110;

  sets[6] |= data[last-3]       & B00000111;
  sets[6] |= data[last-4] << 3  & B00001000;

  sets[5] |= data[last-4] >> 1  & B00000011;
  sets[5] |= data[last-5] << 2  & B00001100;
  
  sets[4] |= data[last-5] >> 2  & B00000001;
  sets[4] |= data[last-6] << 1  & B00001110;
  
  sets[3] |= data[last-7]       & B00000111;
  sets[3] |= data[last-8] << 3  & B00001000;

  sets[2] |= data[last-8] >> 1  & B00000011;
  sets[2] |= data[last-9] << 2  & B00001100;
  
  sets[1] |= data[last-9] >> 2  & B00000001;
  sets[1] |= data[last-10] << 1 & B00001110;
  
  sets[0] |= data[last-11];
  
  //Serial.print('ALL: ');
  for(int i = 0; i < 9; i++){
    Serial.print(sets[i], HEX);
  }
  Serial.println();

  //Serial.print('Badge Number: ');  
  int badgenumber = sets[8] + 16 * sets[7] + 256 * sets[6] + 4096 * sets[5];
  Serial.println(badgenumber);
}
