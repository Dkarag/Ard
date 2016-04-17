bool crc(String input)
{
  char x ;
  int checksumcalc = 0;
  for (int i = 30; i > -1; i--) {
    x = input.charAt(i);
    if (x == '1')
    {
      checksumcalc = checksumcalc ^ 1;
    }
    else
      checksumcalc = checksumcalc ^ 0;
  }
  x = input.charAt(31);
  int y = x - '0';
  if (y == checksumcalc)
    return true;
  else
    return false;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  unsigned long bitstream1 = 2491444487;
  byte checksumcalc = 0;
  for (int i = 1; i < 32; i++) {
    checksumcalc = checksumcalc ^ ((bitstream1 >> i) & 0x01);
    //  Serial.println(bitstream1 >> i);
  }

  Serial.println(checksumcalc);
  Serial.println(bitstream1 & 0x01);


  checksumcalc = 0;

  String bitstream2 = "10010100100000000110110100000111";
/*  for (int i = 30; i > -1; i--) {
    // Serial.println(bitstream2.charAt(i));
    char x = bitstream2.charAt(i);
    if (x == '1')
    {
    //  Serial.println(" hi");
      checksumcalc = checksumcalc ^ 1;
    }
      else
        checksumcalc = checksumcalc ^ 0;
    
  }
  */


  
  Serial.println(crc(bitstream2));
  delay(10000);
}
