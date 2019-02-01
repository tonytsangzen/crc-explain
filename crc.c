#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>

//USB specification
#define CRC_POLY  0x5   //x4+x2+1
#define CRC_LEN   5     
#define CRC_INIT  0x1f 
#define DATA_LEN  11    // 7bit address  +  4 bit endpoint  MSB first

/*
 *  Help function:
 *  Revert input data bit order 
 */
uint32_t revert_bit_order(uint32_t data, int len){
  
  uint32_t mask = 0x1;
  uint32_t ret = 0x0;
  
  for(int i = 0; i < len;  i++){
    ret <<= 1;
    if(data & mask)
      ret |= 0x1;
    mask <<= 1;
  }
  return ret;
}

/*
 * Help function:
 * xor one bit in diffent position 
 */
int bit_xor(uint32_t s1, int pos1, uint32_t s2, int pos2){
  
    return ((s1 >> pos1) & 0x1) ^ ((s2 >> pos2) &0x1);
}


/*
 * Help function:
 *  Dump binary data
 */
void dump(uint32_t data, int start, int end, int fcolor, int bcolor){

  uint32_t mask = 0x1 << (start - 1);

  for(int i = start ; i > end; i--){

      printf("\033[1m\033[%d;%dm%s\033[0m", bcolor, fcolor, (data&mask)?"1":"0");

      mask>>=1;
  }
}

/*
 *  Help function:
 *  loop type some char 
 */
void type(char c, int len){
  for(int i = 0; i < len; i++)
    printf("%c", c);
}

/*
 *  show step resule
 *  line 2: polynomial
 *  line 3: indication xor flag
 *  line 4: data register shift right, crc register shift left
 *  line 6: output register
 */
void step_show(uint32_t crc_reg, uint32_t crc_poly, uint32_t input){
  static int step = 0;
  int xor=bit_xor(crc_reg, 4, input, 0);

  printf("\033[2J");
  printf("%c[%d;%dH",27, 0, 0);
  //line 1
  type('-', (DATA_LEN + CRC_LEN + 16)/2);
  printf("step %2d",step);
  type('-', (DATA_LEN + CRC_LEN + 16)/2);
  printf("\n");

  //line 2
  type(' ',  DATA_LEN+2);
  dump(crc_poly, CRC_LEN, 0, 33, 40);
  printf("<==this is poly    \n");

  //line 3
  type(' ', DATA_LEN + 2 + (CRC_LEN - 3) / 2);
  if(xor)
    printf("\033[1m\033[40;39mXOR\033[0m");
  else
    printf("   ");
  type(' ',  (CRC_LEN - 3)/2 + 20);
  printf("\n");

  //line 4 data shift reg add crc shift reg
  type(' ', step);
  dump(input, DATA_LEN - step, 1, 34, 47);
  dump(input, (step==DATA_LEN)?0:1, 0, 31, 44);
  printf("->");
  dump(crc_reg, CRC_LEN, CRC_LEN - 1, 31, 44);
  dump(crc_reg, CRC_LEN - 1, 0, 39, 47);
  printf("<==this is crc reg \n");

  //line 5
  type(' ', DATA_LEN + 2 + (CRC_LEN - 3) / 2);
  printf("NOT");
  type(' ',  (CRC_LEN - 3)/2 + 20);
  printf("\n");

  //line 6 result
  type(' ', DATA_LEN + 2);
  dump(~crc_reg, CRC_LEN, 0, 39, 47);
  printf("<==this is output  \n");

  //line 7 end line
  type('-', DATA_LEN + CRC_LEN + 22);
  printf("\n");

  step++;
  getchar();
}

/*
 * Main function 
 */
int main(int argc, char* argv[])
{
  uint32_t addr = 0x70;
  uint32_t ep = 0x4;

  //initialize crc reg
  uint32_t crc_reg = CRC_INIT;
  uint32_t crc_poly = CRC_POLY;

  //usb  package
  if(argc >= 3){
    addr =strtol(argv[1], NULL, 10);
    ep = strtol(argv[2], NULL, 10);
  }
  uint32_t input = ep << 7 | addr;


  for(int i = 0; i < DATA_LEN; i++){
      //show this step start status 
      step_show(crc_reg, crc_poly, input);

      //data lsb xor crc msb 
      if(bit_xor(crc_reg, (CRC_LEN - 1), input, 0)){
        crc_reg = (crc_reg << 1) ^ crc_poly;
      }else{
        crc_reg <<= 1;
      }

      input >>= 1;
  }
  //show the result
  step_show(crc_reg, crc_poly, input);
}
