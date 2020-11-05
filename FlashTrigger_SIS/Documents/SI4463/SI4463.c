
#include "RF_si4463.h"
   
unsigned char  Channel;
unsigned char  rate_sever;
unsigned char  Status;
unsigned char  FIF0_RF[20];
unsigned char  FIF0_TF[20];
unsigned char  SPI_Write[20];
unsigned char  abApi_Read[20];
unsigned char  i;
   
unsigned char  ModemTrx1_pn9[] = {7, 0x11, 0x20, 0x03, 0x00,  0x13, 0x00, 0x07};
   
const unsigned char  ModemTrx1[] = {7, 0x11, 0x20, 0x03, 0x00, 0x03, 0x00, 0x07};
   
const unsigned char  ModemTrx1_A[7][14] = {
  {13, 0x11, 0x20, 0x09, 0x03,  0x00, 0x12, 0xC0, 0x04, 0x2D, 0xC6, 0xC0, 0x00, 0x02},
  {13, 0x11, 0x20, 0x09, 0x03,  0x00, 0x27, 0x10, 0x04, 0x2D, 0xC6, 0xC0, 0x00, 0x02},//2.5k
  {13, 0x11, 0x20, 0x09, 0x03,  0x00, 0x4E, 0x20, 0x04, 0x2D, 0xC6, 0xC0, 0x00, 0x02},//5k
  {13, 0x11, 0x20, 0x09, 0x03,  0x00, 0x9C, 0x40, 0x04, 0x2D, 0xC6, 0xC0, 0x00, 0x04},//10K
  {13, 0x11, 0x20, 0x09, 0x03,  0x01, 0x38, 0x80, 0x04, 0x2D, 0xC6, 0xC0, 0x00, 0x06},//20k
  {13, 0x11, 0x20, 0x09, 0x03,  0x01, 0x38, 0x80, 0x08, 0x2D, 0xC6, 0xC0, 0x00, 0x0D},//40k
  {13, 0x11, 0x20, 0x09, 0x03,  0x01, 0xD4, 0xC0, 0x08, 0x2D, 0xC6, 0xC0, 0x00, 0x15},//60k
};
//OK
const unsigned char ModemTx2[7][6] =    {
  {5,0x11, 0x20, 0x01, 0x0C, 0x2F},
  {5,0x11, 0x20, 0x01, 0x0C, 0xBB},//2.5
  {5,0x11, 0x20, 0x01, 0x0C, 0xBB},//5k
  {5,0x11, 0x20, 0x01, 0x0C, 0x19},//10K
  {5,0x11, 0x20, 0x01, 0x0C, 0xd4},//20k
  {5,0x11, 0x20, 0x01, 0x0C, 0xa7},//40k
  {5,0x11, 0x20, 0x01, 0x0C, 0xd8},//60k
};
                                       // 0K
const unsigned char ModemRx1[7][13] = {
  {12, 0x11, 0x20, 0x08, 0x18,   0x01, 0x80, 0x08, 0x03, 0x80, 0x00, 0x32, 0x20},//1.2K
  {12, 0x11, 0x20, 0x08, 0x18,   0x01, 0x80, 0x08, 0x03, 0x80, 0x00, 0x30, 0x20},//2.5K
  {12, 0x11, 0x20, 0x08, 0x18,   0x01, 0x80, 0x08, 0x03, 0x80, 0x00, 0x30, 0x20},//5K
  {12, 0x11, 0x20, 0x08, 0x18,   0x01, 0x80, 0x08, 0x03, 0x80, 0x00, 0x20, 0x10},//9.6k
  {12, 0x11, 0x20, 0x08, 0x18,   0x01, 0x80, 0x08, 0x03, 0x80, 0x00, 0x20, 0x20},//20
  {12, 0x11, 0x20, 0x08, 0x18,   0x01, 0x80, 0x08, 0x03, 0x80, 0x00, 0x10, 0x10},//40k
  {12, 0x11, 0x20, 0x08, 0x18,   0x01, 0x80, 0x08, 0x03, 0x80, 0x00, 0x10, 0x20},//60k
};

const unsigned char ModemRx2[7][14] = {//  OK
  {13, 0x11, 0x20, 0x09, 0x22,  0x03, 0x0D, 0x00, 0xA7, 0xC6, 0x00, 0x54, 0x02, 0xC2},//1.2
  {13, 0x11, 0x20, 0x09, 0x22,  0x02, 0xEE, 0x00, 0xAE, 0xC3, 0x00, 0x57, 0x02, 0xC2},//2.5K
  {13, 0x11, 0x20, 0x09, 0x22,  0x01, 0x77, 0x01, 0x5D, 0x86, 0x00, 0xAF, 0x02, 0xC2},//5K
  {13, 0x11, 0x20, 0x09, 0x22,  0x00, 0xFA, 0x02, 0x0C, 0x4A, 0x01, 0x06, 0x02, 0xC2},//9.6
  {13, 0x11, 0x20, 0x09, 0x22,  0x00, 0xBC, 0x02, 0xBB, 0x0D, 0x01, 0x5D, 0x02, 0xC2},//20
  {13, 0x11, 0x20, 0x09, 0x22,  0x00, 0x7D, 0x04, 0x18, 0x93, 0x02, 0x0C, 0x02, 0xC2},//40
  {13, 0x11, 0x20, 0x09, 0x22,  0x00, 0x7D, 0x04, 0x18, 0x93, 0x02, 0x0C, 0x02, 0xC2},//60k
};

const unsigned char ModemRx3[7][12] = { //OK
  {11, 0x11, 0x20, 0x07, 0x2C,  0x04, 0x36, 0xC0, 0x01, 0x60, 0xB6, 0xC0},//1.2K
  {11, 0x11, 0x20, 0x07, 0x2C, 0x04, 0x36, 0xC0, 0x07, 0x25, 0x62, 0xC0},//2.5K
  {11, 0x11, 0x20, 0x07, 0x2C, 0x04, 0x36, 0xC0, 0x0F, 0x13, 0x66, 0xC0},//5K
  {11, 0x11, 0x20, 0x07, 0x2C, 0x04, 0x36, 0xC0, 0x1D, 0x0D, 0x58, 0xC0},//9.6k
  {11, 0x11, 0x20, 0x07, 0x2C, 0x04, 0x36, 0xC0, 0x3A, 0x08, 0xF6, 0xC0},//20k
  {11, 0x11, 0x20, 0x07, 0x2C, 0x04, 0x36, 0xC0, 0x75, 0x07, 0x41, 0xC0},//40k
  {11, 0x11, 0x20, 0x07, 0x2C, 0x04, 0x36, 0xC0, 0xAF, 0x07, 0x31, 0xC0},//60k
};
                            // OK
const unsigned char ModemRx4[7][6] = {
  {5, 0x11, 0x20, 0x01, 0x35, 0xE2},
  {5, 0x11, 0x20, 0x01, 0x35, 0xE2},//2.5k
  {5, 0x11, 0x20, 0x01, 0x35, 0xE2},//5k
  {5, 0x11, 0x20, 0x01, 0x35, 0xE2},//10k
  {5, 0x11, 0x20, 0x01, 0x35, 0xE2},//20k
  {5, 0x11, 0x20, 0x01, 0x35, 0xE2},//40k
  {5, 0x11, 0x20, 0x01, 0x35, 0xE2},//60k
};                                //

const unsigned char ModemRx4_1[7][14] ={   //OK
  {13, 0x11, 0x20, 0x09, 0x38,  0x11, 0xAB, 0xAB, 0x00, 0x1A, 0x35, 0x55, 0x00, 0x2B},//1.2K
  {13, 0x11, 0x20, 0x09, 0x38,  0x11, 0xA4, 0xA4, 0x00, 0x1A, 0x20, 0x00, 0x00, 0x2B},//2.5K
  {13, 0x11, 0x20, 0x09, 0x38,  0x11, 0x52, 0x52, 0x00, 0x1A, 0x10, 0x00, 0x00, 0x2A},//5K
  {13, 0x11, 0x20, 0x09, 0x38,  0x11, 0x37, 0x37, 0x00, 0x1A, 0xC0, 0x00, 0x00, 0x29},//10
  {13, 0x11, 0x20, 0x09, 0x38,  0x11, 0x29, 0x29, 0x00, 0x1A, 0xA0, 0x00, 0x00, 0x29},//20
  {13, 0x11, 0x20, 0x09, 0x38,  0x11, 0x1B, 0x1B, 0x00, 0x1A, 0xA0, 0x00, 0x00, 0x28},//40
  {13, 0x11, 0x20, 0x09, 0x38,  0x11, 0x1B, 0x1B, 0x00, 0x1A, 0xAA, 0xAB, 0x00, 0x28},//60
};
   
const unsigned char ModemRx7[7][13] = {
  {12, 0x11, 0x20, 0x08, 0x42, 0xA4, 0x02, 0xD6, 0x81, 0x03, 0x9B, 0x01, 0x80},//1.2k
  {12, 0x11, 0x20, 0x08, 0x42, 0xA4, 0x02, 0xD6, 0x83, 0x01, 0x20, 0x01, 0x80},//2.5K
  {12, 0x11, 0x20, 0x08, 0x42, 0xA4, 0x02, 0xD6, 0x83, 0x01, 0x20, 0x01, 0x80},//5K
  {12, 0x11, 0x20, 0x08, 0x42, 0xA4, 0x02, 0xD6, 0x83, 0x01, 0x44, 0x01, 0x80},//10
  {12, 0x11, 0x20, 0x08, 0x42, 0xA4, 0x02, 0xD6, 0x83, 0x01, 0x68, 0x01, 0x80},//20
  {12, 0x11, 0x20, 0x08, 0x42, 0xA4, 0x02, 0xD6, 0x83, 0x02, 0x7F, 0x01, 0x80},//40
  {12, 0x11, 0x20, 0x08, 0x42, 0xA4, 0x02, 0xD6, 0x83, 0x02, 0xAA, 0x01, 0x80},//60
};
                                 // OK
const unsigned char  ModemRx8_RSS[7][6]={
  { 5,0x11, 0x20, 0x01, 0x4E, 0x40},
  { 5,0x11, 0x20, 0x01, 0x4E, 0x40},//2.5K
  { 5,0x11, 0x20, 0x01, 0x4E, 0x40},//5k
  { 5,0x11, 0x20, 0x01, 0x4E, 0x40},//10K
  { 5,0x11, 0x20, 0x01, 0x4E, 0x40},//20k
  { 5,0x11, 0x20, 0x01, 0x4E, 0x40},//40k
  { 5,0x11, 0x20, 0x01, 0x4E, 0x40},//60k
};
                                 //OK
const unsigned char  ModemTrx2[7][6]={
  {5, 0x11, 0x20, 0x01, 0x51, 0x0A},
  {5, 0x11, 0x20, 0x01, 0x51, 0x0A},//2.5k
  {5, 0x11, 0x20, 0x01, 0x51, 0x0A},//5k
  {5, 0x11, 0x20, 0x01, 0x51, 0x0A},//10k
  {5, 0x11, 0x20, 0x01, 0x51, 0x0A},//20k
  {5, 0x11, 0x20, 0x01, 0x51, 0x0A},//40k
  {5, 0x11, 0x20, 0x01, 0x51, 0x0A},//60k
};   
     
const unsigned char ModemRx9[7][17] = {//  OK
  {16,  0x11, 0x21, 0x0C, 0x00,  0xCC, 0xA1, 0x30, 0xA0, 0x21, 0xD1, 0xB9, 0xC9, 0xEA, 0x05, 0x12, 0x11},//1.2
  {16,  0x11, 0x21, 0x0C, 0x00, 0xCC, 0xA1, 0x30, 0xA0, 0x21, 0xD1, 0xB9, 0xC9, 0xEA, 0x05, 0x12, 0x11},//2.5K
  {16,  0x11, 0x21, 0x0C, 0x00, 0xFF, 0xC4, 0x30, 0x7F, 0xF5, 0xB5, 0xB8, 0xDE, 0x05, 0x17, 0x16, 0x0C},//5
  {16,  0x11, 0x21, 0x0C, 0x00, 0xFF, 0xC4, 0x30, 0x7F, 0xF5, 0xB5, 0xB8, 0xDE, 0x05, 0x17, 0x16, 0x0C},//10
  {16,  0x11, 0x21, 0x0C, 0x00, 0xCC, 0xA1, 0x30, 0xA0, 0x21, 0xD1, 0xB9, 0xC9, 0xEA, 0x05, 0x12, 0x11},//20
  {16,  0x11, 0x21, 0x0C, 0x00, 0xFF, 0xBA, 0x0F, 0x51, 0xCF, 0xA9, 0xC9, 0xFC, 0x1B, 0x1E, 0x0F, 0x01},//40
  {16,  0x11, 0x21, 0x0C, 0x00, 0xFF, 0xBA, 0x0F, 0x51, 0xCF, 0xA9, 0xC9, 0xFC, 0x1B, 0x1E, 0x0F, 0x01},//60
};

const unsigned char ModemRx10[7][17] = {//0K
  {16, 0x11, 0x21, 0x0C, 0x0C,  0x0A, 0x04, 0x15, 0xFC, 0x03, 0x00, 0xD6, 0xD0, 0xBE, 0xA3, 0x82, 0x61},//1.2
  {16, 0x11, 0x21, 0x0C, 0x0C, 0x0A, 0x04, 0x15, 0xFC, 0x03, 0x00, 0x0C, 0x01, 0xE4, 0xB9, 0x86, 0x55},//2.5
  {16, 0x11, 0x21, 0x0C, 0x0C, 0x03, 0x00, 0x15, 0xFF, 0x00, 0x00, 0x23, 0x17, 0xF4, 0xC2, 0x88, 0x50},//5K
  {16, 0x11, 0x21, 0x0C, 0x0C, 0x03, 0x00, 0x15, 0xFF, 0x00, 0x00, 0x39, 0x2B, 0x00, 0xC3, 0x7F, 0x3F},//10
  {16, 0x11, 0x21, 0x0C, 0x0C, 0x0A, 0x04, 0x15, 0xFC, 0x03, 0x00, 0x7E, 0x64, 0x1B, 0xBA, 0x58, 0x0B},//20
  {16, 0x11, 0x21, 0x0C, 0x0C, 0xFC, 0xFD, 0x15, 0xFF, 0x00, 0x0F, 0xFF, 0xBA, 0x0F, 0x51, 0xCF, 0xA9},//40k
  {16, 0x11, 0x21, 0x0C, 0x0C, 0xFC, 0xFD, 0x15, 0xFF, 0x00, 0x0F, 0xFF, 0xBA, 0x0F, 0x51, 0xCF, 0xA9},//60l
};
     
const unsigned char ModemRx11[7][17] = {
  {16, 0x11, 0x21, 0x0C, 0x18,  0x41, 0x27, 0x13, 0x06, 0xFF, 0xFD, 0xFD, 0xFD, 0x00, 0x00, 0xF0, 0x0F},//1.2K
  {16, 0x11, 0x21, 0x0C, 0x18, 0x2B, 0x0B, 0xF8, 0xEF, 0xEF, 0xF2, 0xF8, 0xFC, 0x05, 0x00, 0xFF, 0x0F},//2.5
  {16, 0x11, 0x21, 0x0C, 0x18, 0x21, 0xFF, 0xEC, 0xE6, 0xE8, 0xEE, 0xF6, 0xFB, 0x05, 0xC0, 0xFF, 0x0F},//5k
  {16, 0x11, 0x21, 0x0C, 0x18, 0x0C, 0xEC, 0xDC, 0xDC, 0xE3, 0xED, 0xF6, 0xFD, 0x15, 0xC0, 0xFF, 0x0F},//10k
  {16, 0x11, 0x21, 0x0C, 0x18, 0xDD, 0xCE, 0xD6, 0xE6, 0xF6, 0x00, 0x03, 0x03, 0x15, 0xF0, 0x3F, 0x00},//20k
  {16, 0x11, 0x21, 0x0C, 0x18, 0xC9, 0xFC, 0x1B, 0x1E, 0x0F, 0x01, 0xFC, 0xFD, 0x15, 0xFF, 0x00, 0x0F},//40k
  {16, 0x11, 0x21, 0x0C, 0x18, 0xC9, 0xFC, 0x1B, 0x1E, 0x0F, 0x01, 0xFC, 0xFD, 0x15, 0xFF, 0x00, 0x0F},//20k
};

const unsigned char PA_Modem[7][9]={
  {8, 0x11, 0x22, 0x04, 0x00, 0x08, 0x7F, 0x00, 0x3D},
  {8, 0x11, 0x22, 0x04, 0x00, 0x08, 0x7F, 0x00, 0x3D},//2.5k
  {8, 0x11, 0x22, 0x04, 0x00, 0x08, 0x7F, 0x00, 0x3D},//5k
  {8, 0x11, 0x22, 0x04, 0x00, 0x08, 0x7F, 0x00, 0x3D},//10k
  {8, 0x11, 0x22, 0x04, 0x00, 0x08, 0x7F, 0x00, 0x3D},//20k
  {8, 0x11, 0x22, 0x04, 0x00, 0x08, 0x7F, 0x00, 0x3D},//20k
  {8, 0x11, 0x22, 0x04, 0x00, 0x08, 0x7F, 0x00, 0x3D},//60k
};        //  0K

const unsigned char  ModemTrx3[7][12] ={
  {11, 0x11, 0x23, 0x07, 0x00, 0x2C, 0x0E, 0x0B, 0x04, 0x0C, 0x73, 0x03},
  {11, 0x11, 0x23, 0x07, 0x00, 0x2C, 0x0E, 0x0B, 0x04, 0x0C, 0x73, 0x03},//2.5k
  {11, 0x11, 0x23, 0x07, 0x00, 0x2C, 0x0E, 0x0B, 0x04, 0x0C, 0x73, 0x03},//5k
  {11, 0x11, 0x23, 0x07, 0x00, 0x2C, 0x0E, 0x0B, 0x04, 0x0C, 0x73, 0x03},//10k
  {11, 0x11, 0x23, 0x07, 0x00, 0x2C, 0x0E, 0x0B, 0x04, 0x0C, 0x73, 0x03},//20k
  {11, 0x11, 0x23, 0x07, 0x00, 0x2C, 0x0E, 0x0B, 0x04, 0x0C, 0x73, 0x03},//40k
  {11, 0x11, 0x23, 0x07, 0x00, 0x2C, 0x0E, 0x0B, 0x04, 0x0C, 0x73, 0x03},//60k
};
                                       //  OK
const unsigned char  ModemTrx4[7][13] ={
  {12, 0x11, 0x40, 0x08, 0x00, 0x38, 0x0D, 0xDD, 0xDD, 0x88, 0x89, 0x20, 0xFE},
  {12, 0x11, 0x40, 0x08, 0x00, 0x38, 0x0D, 0xDD, 0xDD, 0x88, 0x89, 0x20, 0xFE},//2.5k
  {12, 0x11, 0x40, 0x08, 0x00, 0x38, 0x0D, 0xDD, 0xDD, 0x88, 0x89, 0x20, 0xFE},//5k
  {12, 0x11, 0x40, 0x08, 0x00, 0x38, 0x0D, 0xDD, 0xDD, 0x88, 0x89, 0x20, 0xFE},//10
  {12, 0x11, 0x40, 0x08, 0x00, 0x38, 0x0D, 0xDD, 0xDD, 0x88, 0x89, 0x20, 0xFE},//20k
  {12, 0x11, 0x40, 0x08, 0x00, 0x38, 0x0D, 0xDD, 0xDD, 0x88, 0x89, 0x20, 0xFE},//40k
  {12, 0x11, 0x40, 0x08, 0x00, 0x38, 0x0D, 0xDD, 0xDD, 0x88, 0x89, 0x20, 0xFE},//60k
};
   
ctrlTypefunc_t  lpCtrlfqcfunc = {0,0,0,0,0};
   
void register_rf_func(ctrlTypefunc_t *lprftypefunc)
{   
   if(lprftypefunc->lpSpiByteWritefunc != 0){
      lpCtrlfqcfunc.lpSpiByteWritefunc = lprftypefunc->lpSpiByteWritefunc;
   }
   if(lprftypefunc->lpSpiByteReadfunc != 0){
      lpCtrlfqcfunc.lpSpiByteReadfunc = lprftypefunc->lpSpiByteReadfunc;
   }
   if(lprftypefunc->lpCtrlOpenEnStatus != 0){
      lpCtrlfqcfunc.lpCtrlOpenEnStatus = lprftypefunc->lpCtrlOpenEnStatus;
   }
   if(lprftypefunc->lpSwitchpaStatus != 0){
      lpCtrlfqcfunc.lpSwitchpaStatus = lprftypefunc->lpSwitchpaStatus;
   }
   if(lprftypefunc->lpRecvDataTousr != 0){
      lpCtrlfqcfunc.lpRecvDataTousr = lprftypefunc->lpRecvDataTousr;
   }
}   

void Delay1s(unsigned int ii)
{   
  unsigned char  j;
  while(ii--){
    for(j=0;j<100;j++);
  }   
}   
   
void RF_PN9_TEST_POWER(void)
{
  lpCtrlfqcfunc.lpSwitchpaStatus(txOpen);
  SPI_SendCommand(ModemTrx1_pn9[0],&ModemTrx1_pn9[1]);       // Send API command to the radio IC
  SPI_WaitforCTS();
  SPI_Write[0] = CMD_CHANGE_STATE;      // Use change state command
  SPI_Write[1] = 3;                  // Go to Ready mode
  SPI_SendCommand(2,SPI_Write);       // Send command to the radio IC
  SPI_WaitforCTS();               // Wait for CTS
  SPI_Write[0] = CMD_START_TX;  // Use Tx Start command
  SPI_Write[1] = 0;             // Set channel number
  SPI_Write[2] = 0x30;          // Ready state after Tx, start Tx immediately
  SPI_Write[3] = 0x00;          // Packet fields used, do not enter packet length here
  SPI_Write[4] = 0x00;          // Packet fields used, do not enter packet length here
  SPI_SendCommand(5,SPI_Write);    // Send command to the radio IC
  SPI_WaitforCTS();            // Wait for CTS
}   
   
void SI4463_INIT(unsigned char data_rate)
{
  rate_sever=data_rate;
  // Start the radio
  SPI_Write[0] = CMD_POWER_UP;        // Use API command to power up the radio IC
  SPI_Write[1] = 0x01;                // Write global control registers
  SPI_Write[2] = 0x00;                // Write global control registers
  SPI_SendCommand(3,SPI_Write);       // Send command to the radio IC
  SPI_WaitforCTS();

  // Read ITs, clear pending ones
  SPI_Write[0] = CMD_GET_INT_STATUS;      // Use interrupt status command
  SPI_Write[1] = 0;               // Clear PH_CLR_PEND
  SPI_Write[2] = 0;               // Clear MODEM_CLR_PEND
  SPI_Write[3] = 0;               // Clear CHIP_CLR_PEND
  SPI_SendCommand (4, SPI_Write);            // Send command to the radio IC
  bApi_GetResponse(8, abApi_Read);       // Make sure that CTS is ready then get the response
  // Set TRX parameters of the radio IC; data retrieved from the WDS-generated modem_params.h header file
  for(i=0;i<8;i++){
    SPI_Write[i]=ModemTrx1[i];
  }
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);       // Send API command to the radio IC
  SPI_WaitforCTS();       // Wait for CTS

  for(i=0;i<ModemTx2[data_rate][0]+1;i++){
    SPI_Write[i]=ModemTx2[data_rate][i];
  }
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();
   
  for(i=0;i<ModemRx1[data_rate][0]+1;i++){
    SPI_Write[i]=ModemRx1[data_rate][i];
  }
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);       // Send API command to the radio IC
  SPI_WaitforCTS();                               // Wait for CTS

  for(i=0;i<ModemRx2[data_rate][0]+1;i++){
     SPI_Write[i]=ModemRx2[data_rate][i];
  }
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();
   
  for(i=0;i<ModemRx3[data_rate][0]+1;i++){
    SPI_Write[i]=ModemRx3[data_rate][i];
  }
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();
   
  for(i=0;i<ModemRx4[data_rate][0]+1;i++){
    SPI_Write[i]=ModemRx4[data_rate][i];
  }   
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();
   
  for(i=0;i<ModemRx4_1[data_rate][0]+1;i++){
    SPI_Write[i]=ModemRx4_1[data_rate][i];
  }   
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();
   
   
  for(i=0;i<ModemRx7[data_rate][0]+1;i++){
    SPI_Write[i]=ModemRx7[data_rate][i];
  }
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();
   
  for(i=0;i<ModemTrx1_A[data_rate][0]+1;i++){
    SPI_Write[i]=ModemTrx1_A[data_rate][i];
  }   
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();
   
  for(i=0;i<ModemRx8_RSS[data_rate][0]+1;i++){
    SPI_Write[i]=ModemRx8_RSS[data_rate][i];
  }
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();
   
  for(i=0;i<ModemTrx2[data_rate][0]+1;i++){
    SPI_Write[i]=ModemTrx2[data_rate][i];
  }
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();
   
  for(i=0;i<ModemRx9[data_rate][0]+1;i++){
    SPI_Write[i]=ModemRx9[data_rate][i];
  }   
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();

  for(i=0;i<ModemRx10[data_rate][0]+1;i++){
    SPI_Write[i]=ModemRx10[data_rate][i];
  }   
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();
   
  for(i=0;i<ModemRx11[data_rate][0]+1;i++){
    SPI_Write[i]=ModemRx11[data_rate][i];
  }
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();
   
  for(i=0;i<PA_Modem[data_rate][0]+1;i++){
    SPI_Write[i]=PA_Modem[data_rate][i];
  }
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();

  for(i=0;i<ModemTrx3[data_rate][0]+1;i++){
    SPI_Write[i]=ModemTrx3[data_rate][i];
  }
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();

  for(i=0;i<ModemTrx4[data_rate][0]+1;i++){
    SPI_Write[i]=ModemTrx4[data_rate][i];
  }
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);
  SPI_WaitforCTS();
   
    // Enable the proper interrupts
  SPI_Write[0] = CMD_SET_PROPERTY;        // Use property command
  SPI_Write[1] = PROP_INT_CTL_GROUP;      // Select property group
  SPI_Write[2] = 4;                         // Number of properties to be written
  SPI_Write[3] = PROP_INT_CTL_ENABLE;     // Specify property
  SPI_Write[4] = 0x01;              // INT_CTL: PH interrupt enable
  SPI_Write[5] = 0x38;              // INT_CTL_PH: PH PACKET_SENT, PACKET_RX, CRC2_ERR enabled
  SPI_Write[6] = 0x00;              // INT_CTL_MODEM: -
  SPI_Write[7] = 0x00;              // INT_CTL_CHIP_EN: -
  SPI_SendCommand(8,SPI_Write);        // Send API command to the radio IC
  SPI_WaitforCTS();                // Wait for CTS
  // Configure Fast response registers
  SPI_Write[0] = CMD_SET_PROPERTY;       // Use property command
  SPI_Write[1] = PROP_FRR_CTL_GROUP;     // Select property group
  SPI_Write[2] = 4;                 // Number of properties to be written
  SPI_Write[3] = PROP_FRR_CTL_A_MODE;    // Specify fast response registers
  SPI_Write[4] = 0x04;             // FRR A: PH IT pending
  SPI_Write[5] = 0x06;             // FRR B: Modem IT pending
  SPI_Write[6] = 0x0A;             // FRR C: Latched RSSI
  SPI_Write[7] = 0x00;             // FRR D: disabled
  SPI_SendCommand(8,SPI_Write);        // Send API command to the radio IC
  SPI_WaitforCTS();              // Wait for CTS
  // Set packet content
  // Set preamble length - Rx
  SPI_Write[0] = CMD_SET_PROPERTY;         // Use property command
  SPI_Write[1] = PROP_PREAMBLE_GROUP;      // Select property group
  SPI_Write[2] = 1;                        // Number of properties to be written
  SPI_Write[3] = PROP_PREAMBLE_CONFIG_STD_1;  // Specify property
  SPI_Write[4] = 20;               // 20 bits preamble detection threshold
  SPI_SendCommand(5,SPI_Write);      // Send API command to the radio IC
  SPI_WaitforCTS();         // Wait for CTS
  // Set preamble length - Tx
  SPI_Write[0] = CMD_SET_PROPERTY;        // Use property command
  SPI_Write[1] = PROP_PREAMBLE_GROUP;     // Select property group
  SPI_Write[2] = 1;               // Number of properties to be written
  SPI_Write[3] = PROP_PREAMBLE_TX_LENGTH;   // Specify property
  SPI_Write[4] =  0x05;                       // 5 bytes Tx preamble

  SPI_SendCommand(5,SPI_Write);        // Send command to the radio IC
  SPI_WaitforCTS();
  // Set preamble pattern
  SPI_Write[0] = CMD_SET_PROPERTY;           // Use property command
  SPI_Write[1] = PROP_PREAMBLE_GROUP;        // Select property group
  SPI_Write[2] = 1;                          // Number of properties to be written
  SPI_Write[3] = PROP_PREAMBLE_CONFIG;       // Specify property
  SPI_Write[4] = 0x31;              // Use `1010` pattern, length defined in bytes
  SPI_SendCommand(5,SPI_Write);        // Send API command to the radio IC
  SPI_WaitforCTS();                // Wait for CTS
  // Set sync word
  SPI_Write[0] = CMD_SET_PROPERTY;        // Use property command
  SPI_Write[1] = PROP_SYNC_GROUP;       // Select property group
  SPI_Write[2] = 3;               // Number of properties to be written
  SPI_Write[3] = PROP_SYNC_CONFIG;        // Specify property
  SPI_Write[4] = 0x01;              // SYNC_CONFIG: 2 bytes sync word
  SPI_Write[5] = 0xB4;              // SYNC_BITS_31_24: 1st sync byte: 0x2D; NOTE: LSB transmitted first!
  SPI_Write[6] = 0x2B;              // SYNC_BITS_23_16: 2nd sync byte: 0xD4; NOTE: LSB transmitted first!
  SPI_SendCommand(7,SPI_Write);        // Send command to the radio IC
  SPI_WaitforCTS();                // Wait for CTS
   
  // General packet config (set bit order)
  SPI_Write[0] = CMD_SET_PROPERTY;        // Use property command
  SPI_Write[1] = PROP_PKT_GROUP;        // Select property group
  SPI_Write[2] = 1;               // Number of properties to be written
  SPI_Write[3] = PROP_PKT_CONFIG1;        // Specify property
  SPI_Write[4] = 0x80;              // Separate RX and TX FIELD properties are used, payload data goes MSB first
  SPI_SendCommand(5,SPI_Write);        // Send command to the radio IC
  SPI_WaitforCTS();                // Wait for CTS

  // Set variable packet length
  SPI_Write[0] = CMD_SET_PROPERTY;        // Use property command
  SPI_Write[1] = PROP_PKT_GROUP;        // Select property group
  SPI_Write[2] = 3;               // Number of properties to be written
  SPI_Write[3] = PROP_PKT_LEN;          // Specify property
  SPI_Write[4] = 0x0a;              // PKT_LEN: length is put in the Rx FIFO, FIELD 2 is used for the payload (with variable length)
  SPI_Write[5] = 0x01;              // PKT_LEN_FIELD_SOURCE: FIELD 1 is used for the length information
  SPI_Write[6] = 0x00;              // PKT_LEN_ADJUST: no adjustment (FIELD 1 determines the actual payload length)
  SPI_SendCommand(7,SPI_Write);        // Send command to the radio IC
  SPI_WaitforCTS();                // Wait for CTS
   
  // Set packet fields for Tx (one field is used)
  SPI_Write[0] = CMD_SET_PROPERTY;        // Use property command
  SPI_Write[1] = PROP_PKT_GROUP;        // Select property group
  SPI_Write[2] = 4;               // Number of properties to be written
  SPI_Write[3] = PROP_PKT_FIELD_1_LENGTH_12_8;  // Specify first property
  SPI_Write[4] = 0x00;              // PKT_FIELD_1_LENGTH_12_8: defined later (under bSendPacket() )
  SPI_Write[5] = 0x00;              // PKT_FIELD_1_LENGTH_7_0: defined later (under bSendPacket() )
  SPI_Write[6] = 0x00;              // PKT_FIELD_1_CONFIG : No 4(G)FSK/Whitening/Manchester coding for this field
  SPI_Write[7] = 0xa2;              // PKT_FIELD_1_CRC_CONFIG: Start CRC calc. from this field, send CRC at the end of the field
  SPI_SendCommand(8,SPI_Write);        // Send command to the radio IC
  SPI_WaitforCTS();                // Wait for CTS
   
  // Set packet fields for Rx (two fields are used)
  // FIELD1 is fixed, 1byte length, used for PKT_LEN
  SPI_Write[0] = CMD_SET_PROPERTY;        // Use property command
  SPI_Write[1] = PROP_PKT_GROUP;        // Select property group
  SPI_Write[2] = 4;               // Number of properties to be written
  SPI_Write[3] = PROP_PKT_RX_FIELD_1_LENGTH_12_8; // Specify first property
  SPI_Write[4] = 0x00;              // PKT_RX_FIELD_1_LENGTH_12_8: 1 byte (containing variable packet length info)
  SPI_Write[5] = 0x01;              // PKT_RX_FIELD_1_LENGTH_7_0: 1 byte (containing variable packet length info)
  SPI_Write[6] = 0x00;              // PKT_RX_FIELD_1_CONFIG: No 4(G)FSK/Whitening/Manchester coding for this field
  SPI_Write[7] = 0x82;              // PKT_RX_FIELD_1_CRC_CONFIG: Start CRC calc. from this field, enable CRC calc.
  SPI_SendCommand(8,SPI_Write);        // Send command to the radio IC
                                       // FIELD2 is variable length, contains the actual payload
  SPI_WaitforCTS();                   // Wait for CTS
  SPI_Write[0] = CMD_SET_PROPERTY;        // Use property command
  SPI_Write[1] = PROP_PKT_GROUP;        // Select property group
  SPI_Write[2] = 4;               // Number of properties to be written
  SPI_Write[3] = PROP_PKT_RX_FIELD_2_LENGTH_12_8; // Specify first property
  SPI_Write[4] = 0x00;              // PKT_RX_FIELD_2_LENGTH_12_8: max. field length (variable packet length)
  SPI_Write[5] = 64;     // PKT_RX_FIELD_2_LENGTH_7_0: max. field length (variable packet length)
  SPI_Write[6] = 0x00;              // PKT_RX_FIELD_2_CONFIG: No 4(G)FSK/Whitening/Manchester coding for this field
  SPI_Write[7] = 0x0A;              // PKT_RX_FIELD_2_CRC_CONFIG: check CRC at the end, enable CRC calc.
  SPI_SendCommand(8,SPI_Write);        // Send command to the radio IC
  SPI_WaitforCTS();                // Wait for CTS
   
  // Configure CRC polynomial and seed
  SPI_Write[0] = CMD_SET_PROPERTY;        // Use property command
  SPI_Write[1] = PROP_PKT_GROUP;        // Select property group
  SPI_Write[2] = 1;               // Number of properties to be written
  SPI_Write[3] = PROP_PKT_CRC_CONFIG;     // Specify property
  SPI_Write[4] = 0x83;              // CRC seed: all `1`s, poly: No. 3, 16bit, Baicheva-16
  SPI_SendCommand(5,SPI_Write);        // Send command to the radio IC
  SPI_WaitforCTS();                // Wait for CTS
   
  // Set RSSI latch to sync word
  SPI_Write[0] = CMD_SET_PROPERTY;        // Use property command
  SPI_Write[1] = PROP_MODEM_GROUP;        // Select property group
  SPI_Write[2] = 1;               // Number of properties to be written
  SPI_Write[3] = PROP_MODEM_RSSI_CONTROL;   // Specify property
  SPI_Write[4] = 0x12;              // RSSI average over 4 bits, latches at sync word detect
  SPI_SendCommand(5,SPI_Write);        // Send API command to the radio IC
  SPI_WaitforCTS();                // Wait for CTS
   
  SPI_Write[0] = CMD_GPIO_PIN_CFG;       // Use GPIO pin configuration command
  SPI_Write[1] = 17;                // Configure GPIO0 as Rx state
  SPI_Write[2] = 20;                // Configure GPIO1 as Tx data
  SPI_Write[3] = 0;             // Configure GPIO2 as Tx state
  SPI_Write[4] = 0;             // Configure GPIO3 as Tx data CLK
  SPI_SendCommand(5,SPI_Write);     // Send command to the radio IC
  SPI_WaitforCTS();
  // Adjust XTAL clock frequency
  SPI_Write[0] = CMD_SET_PROPERTY;        // Use property command
  SPI_Write[1] = PROP_GLOBAL_GROUP;       // Select property group
  SPI_Write[2] = 1;               // Number of properties to be written
  SPI_Write[3] = PROP_GLOBAL_XO_TUNE;     // Specify property
  SPI_Write[4] = CAP_BANK_VALUE;        // Set cap bank value to adjust XTAL clock frequency
  SPI_SendCommand(5,SPI_Write);        // Send command to the radio IC
  SPI_WaitforCTS();                // Wait for CTS

  SI4463_RECEIVE();
}
   
void SI4463_SENDPACKET(unsigned char *lpRF,unsigned char LEN)
{
  for(i=0;i<8;i++){
    SPI_Write[i]=ModemTrx1[i];
  }
  SPI_SendCommand(SPI_Write[0],&SPI_Write[1]);     // Send API command to the radio IC
  SPI_WaitforCTS();
  lpCtrlfqcfunc.lpSwitchpaStatus(txOpen);
  FIF0_TF[0]=LEN;
  for(i=0;i<LEN;i++){
    FIF0_TF[i+1]=*lpRF;
    lpRF++;
  }
  bSendPacket(LEN+1,FIF0_TF);
  Status = CMD_TX;
}
   
void power_PA(unsigned char PA)
{
   SPI_Write[0] = CMD_SET_PROPERTY;     // Use property command
   SPI_Write[1] = 0X22;                 // Select property group
   SPI_Write[2] = 1;                        // Number of properties to be written
   SPI_Write[3] = 0X01;                     // Specify property
   SPI_Write[4] = PA;                       // Set cap bank value to adjust XTAL clock frequency
   SPI_SendCommand(5,SPI_Write);         // Send command to the radio IC
   SPI_WaitforCTS();                 // Wait for CTS
}
   
void bApi_ReadRxDataBuffer(unsigned char bRxFifoLength, unsigned char *pbRxFifoData)
{   
   lpCtrlfqcfunc.lpCtrlOpenEnStatus(enClose);                                    // Select radio IC by pulling its nSEL pin low
   lpCtrlfqcfunc.lpSpiByteWritefunc(CMD_READ_RX_FIFO);                // Send Rx read command
   bSpi_SendDataGetResp(bRxFifoLength, pbRxFifoData);  // Write Tx FIFO
   lpCtrlfqcfunc.lpCtrlOpenEnStatus(enOpen);                                 // De-select radio IC by putting its nSEL pin high
}
   
void SI4463_RECEIVE(void)
{
   lpCtrlfqcfunc.lpSwitchpaStatus(rxOpen);
   SPI_Write[0] = CMD_START_RX;    // Use start Rx command
   SPI_Write[1] = Channel;           // Set channel number
   SPI_Write[2] = 0x00;        // Start Rx immediately
   SPI_Write[3] = 0x00;        // Packet fields used, do not enter packet length here
   SPI_Write[4] = 0X00;        // Packet fields used, do not enter packet length here
   SPI_Write[5] = 0x00;        // No change if Rx timeout
   SPI_Write[6] = 0x03;        // Ready state after Rx valid state
   SPI_Write[7] = 0x03;        // Ready state after Rx invalid state
   SPI_SendCommand(8,SPI_Write);    // Send API command to the radio IC
   SPI_WaitforCTS();             // Wait for CTS*/
   Status = CMD_RX;
}
   
void bSendPacket(unsigned char bLength, unsigned char *abPayload)
{   
   unsigned char bTxItState[8];
   SPI_Write[0] = 0x15;        // Use change state command
   SPI_Write[1] = 3;               // Go to Ready mode
   SPI_SendCommand(2,SPI_Write);        // Send command to the radio IC
   SPI_WaitforCTS();
     
   SPI_Write[0] = CMD_CHANGE_STATE;        // Use change state command
   SPI_Write[1] = 3;                   // Go to Ready mode
   SPI_SendCommand(2,SPI_Write);        // Send command to the radio IC
   SPI_WaitforCTS();                // Wait for CTS
   
   bApi_WriteTxDataBuffer(bLength, abPayload);   // Write data to Tx FIFO
   SPI_WaitforCTS();                             // Wait for CTS
   
   // Set TX packet length
   SPI_Write[0] = CMD_SET_PROPERTY;        // Use property command
   SPI_Write[1] = PROP_PKT_GROUP;        // Select property group
   SPI_Write[2] = 2;               // Number of properties to be written
   SPI_Write[3] = PROP_PKT_FIELD_1_LENGTH_12_8; // Specify first property
   SPI_Write[4] = 0x00;
   SPI_Write[5] = bLength;           // Field length (variable packet length info)
   SPI_SendCommand(6,SPI_Write);        // Send command to the radio IC
   SPI_WaitforCTS();                // Wait for CTS
   
   // Read ITs, clear pending ones
   SPI_Write[0] = CMD_GET_INT_STATUS;  // Use interrupt status command
   SPI_Write[1] = 0;                  // Clear PH_CLR_PEND
   SPI_Write[2] = 0;                 // Clear MODEM_CLR_PEND
   SPI_Write[3] = 0;                 // Clear CHIP_CLR_PEND
   SPI_SendCommand(4,SPI_Write);    // Send command to the radio IC
   bApi_GetResponse(8,bTxItState);     // Make sure that CTS is ready then get the response
   
   // Start Tx
   SPI_Write[0] = CMD_START_TX;      // Use Tx Start command
   SPI_Write[1] = Channel;              // Set channel number
   SPI_Write[2] = 0x30;          // Ready state after Tx, start Tx immediately
   SPI_Write[3] = 0x00;             // Packet fields used, do not enter packet length here
   SPI_Write[4] = 0x00;          // Packet fields used, do not enter packet length here
   SPI_SendCommand(5,SPI_Write);    // Send command to the radio IC
   SPI_WaitforCTS();            // Wait for CTS
}
   
void bApi_WriteTxDataBuffer(unsigned char bTxFifoLength, unsigned char *pbTxFifoData)   // Write Tx FIFO
{
   unsigned char bCnt;
   lpCtrlfqcfunc.lpCtrlOpenEnStatus(enClose);                                      // Select radio IC by pulling its nSEL pin low
   lpCtrlfqcfunc.lpSpiByteWritefunc(CMD_TX_FIFO_WRITE);                      // Send Tx write command
   for(bCnt=0;bCnt<bTxFifoLength;bCnt++){
     lpCtrlfqcfunc.lpSpiByteWritefunc(*pbTxFifoData);pbTxFifoData++;
   }
   lpCtrlfqcfunc.lpCtrlOpenEnStatus(enOpen);                                      // De-select radio IC by putting its nSEL pin high
}
   
unsigned char SPI_WaitforCTS(void)
{
  unsigned char  bCtsValue;
  unsigned int   bErrCnt;
  for(bErrCnt=0;bErrCnt<MAX_CTS_RETRY;bErrCnt++){
    lpCtrlfqcfunc.lpCtrlOpenEnStatus(enClose);                          // Select radio IC by pulling its nSEL pin low
    lpCtrlfqcfunc.lpSpiByteWritefunc(CMD_READ_CMD_BUFF);          // Read command buffer; send command byte
    bCtsValue=lpCtrlfqcfunc.lpSpiByteReadfunc();                  // Read command buffer; get CTS value
    lpCtrlfqcfunc.lpCtrlOpenEnStatus(enOpen);                          // If CTS is not 0xFF, put NSS high and stay in waiting loop
    if(bCtsValue==0xff){
      bErrCnt=2501;
    }
  }
  return bErrCnt;
}
   
void SPI_SendCommand(unsigned char LEN, unsigned char *CmdData)   // Send a command + data to the chip
{
  unsigned char ii;
  lpCtrlfqcfunc.lpCtrlOpenEnStatus(enClose);                           // Select radio IC by pulling its nSEL pin low
  for(ii=0;ii<LEN;ii++){
    lpCtrlfqcfunc.lpSpiByteWritefunc(CmdData[ii]);
  }   // Send data array to the radio IC via SPI
  lpCtrlfqcfunc.lpCtrlOpenEnStatus(enOpen);                          // De-select radio IC by putting its nSEL pin high
}
   
void bApi_GetResponse(unsigned char bRespLength, unsigned char *pbRespData) // Get response from the chip (used after a command)
{
   unsigned char  bCtsValue;
   unsigned int   bErrCnt;
   for(bErrCnt=0;bErrCnt<200;bErrCnt++){
     lpCtrlfqcfunc.lpCtrlOpenEnStatus(enClose);                           // Select radio IC by pulling its nSEL pin low
     lpCtrlfqcfunc.lpSpiByteWritefunc(CMD_CTS_READ);          // Read command buffer; send command byte
     bSpi_SendDataGetResp(1, &bCtsValue);
     if(bCtsValue==0xff){bErrCnt=0xff;}else
     { lpCtrlfqcfunc.lpCtrlOpenEnStatus(enOpen);}
   }
   bSpi_SendDataGetResp(bRespLength, pbRespData);  // CTS value ok, get the response data from the radio IC
   lpCtrlfqcfunc.lpCtrlOpenEnStatus(enOpen);                                 // De-select radio IC by putting its nSEL pin high
}
   
void bSpi_SendDataGetResp(unsigned char bDataOutLength, unsigned char *pbDataOut)   // Send data, get the response
{
  unsigned char bCnt;
  // send command and get response from the radio IC
  for (bCnt=0; bCnt<bDataOutLength; bCnt++)
  {
    pbDataOut[bCnt] = lpCtrlfqcfunc.lpSpiByteReadfunc();            // Store data byte that came from the radio IC
  }
}
   
void Reset_FIFO_RXTX (void)
{
  SPI_Write[0]=COM_FIFO_REST;
  SPI_Write[1]=FIFO_RXTX;
  SPI_SendCommand(2,SPI_Write);
  SPI_WaitforCTS();
}
   
//�ж�����Ҫ�����ĺ���
unsigned char buf[512];
void interrupt_si4463(void)
{
  SPI_Write[0] = CMD_GET_INT_STATUS;    // Use interrupt status command
  SPI_Write[1] = 0;           // Clear PH_CLR_PEND
  SPI_Write[2] = 0;           // Clear MODEM_CLR_PEND
  SPI_Write[3] = 0;           // Clear CHIP_CLR_PEND
  SPI_SendCommand(1,SPI_Write);    // Send command to the radio IC
  bApi_GetResponse(8,abApi_Read);      // Make sure that CTS is ready then get the response
   
  if((abApi_Read[2]&0x10) && Status==CMD_RX)//���մ�������
  {
     SPI_Write[0] = CMD_FIFO_INFO;  // Use FIFO info command to get the packet length information
     SPI_SendCommand(1,SPI_Write);  // Send command to the radio IC
     bApi_GetResponse(2,&FIF0_RF[0]);   // Make sure that CTS is ready then get the response
          // Read out the payload after the length
     bApi_ReadRxDataBuffer(FIF0_RF[0],&FIF0_RF[1]);
     for(i=0;i<FIF0_RF[1];i++){
       buf[i]=FIF0_RF[2+i];
     }

     lpCtrlfqcfunc.lpRecvDataTousr(buf,FIF0_RF[1]);

     // Reset_FIFO_RXTX();
     SPI_Write[0] = CMD_GET_INT_STATUS;  // Use interrupt status command
     SPI_Write[1] = 0;        // Clear PH_CLR_PEND
     SPI_Write[2] = 0;        // Clear MODEM_CLR_PEND
     SPI_Write[3] = 0;        // Clear CHIP_CLR_PEND
     SPI_SendCommand(1,SPI_Write);    // Send command to the radio IC
     bApi_GetResponse(8,abApi_Read);      // Make sure that CTS is ready then get the response
     // Start Rx
     SPI_Write[0] = CMD_START_RX;       // Use start Rx command
     SPI_Write[1] = Channel;                  // Set channel number
     SPI_Write[2] = 0x00;               // Start Rx immediately
     SPI_Write[3] = 0x00;               // Packet fields used, do not enter packet length here
     SPI_Write[4] = 0x00;               // Packet fields used, do not enter packet length here
     SPI_Write[5] = 0x00;               // No change if Rx timeout
     SPI_Write[6] = 0x03;               // Ready state after Rx
     SPI_Write[7] = 0x03;               // Ready state after Rx invalid state
     SPI_SendCommand(8,SPI_Write);    // Send API command to the radio IC
     SPI_WaitforCTS();         // Wait for CTS
     Status = CMD_RX;
  }else if((abApi_Read[2]&0x20) && Status==CMD_TX){//���ʹ�������
    lpCtrlfqcfunc.lpSwitchpaStatus(rxOpen);
    SPI_Write[0] = CMD_START_RX;        // Use start Rx command
    SPI_Write[1] = Channel;               // Set channel number
    SPI_Write[2] = 0x00;                // Start Rx immediately
    SPI_Write[3] = 0x00;                // Packet fields used, do not enter packet length here
    SPI_Write[4] = 0x00;                // Packet fields used, do not enter packet length here
    SPI_Write[5] = 0x00;                // No change if Rx timeout
    SPI_Write[6] = 0x03;                // Ready state after Rx
    SPI_Write[7] = 0x03;                // Ready state after Rx invalid state
    SPI_SendCommand(8,SPI_Write);   // Send API command to the radio IC
    SPI_WaitforCTS();               // Wait for CTS
    Status = CMD_RX;
  }else{
    Reset_FIFO_RXTX();
    SPI_Write[0] = CMD_GET_INT_STATUS;  // Use interrupt status command
    SPI_Write[1] = 0;             // Clear PH_CLR_PEND
    SPI_Write[2] = 0;             // Clear MODEM_CLR_PEND
    SPI_Write[3] = 0;             // Clear CHIP_CLR_PEND
    SPI_SendCommand(1,SPI_Write);     // Send command to the radio IC
    bApi_GetResponse(8,abApi_Read);   // Make sure that CTS is ready then get the response
   
    // Start Rx
    SPI_Write[0] = CMD_START_RX;        // Use start Rx command
    SPI_Write[1] = Channel;         // Set channel number
    SPI_Write[2] = 0x00;                // Start Rx immediately
    SPI_Write[3] = 0x00;                // Packet fields used, do not enter packet length here
    SPI_Write[4] = 0x00;                // Packet fields used, do not enter packet length here
    SPI_Write[5] = 0x00;                // No change if Rx timeout
    SPI_Write[6] = 0x03;                // Ready state after Rx
    SPI_Write[7] = 0x03;                // Ready state after Rx invalid state
    SPI_SendCommand(8,SPI_Write);   // Send API command to the radio IC
    SPI_WaitforCTS();               // Wait for CTS
    Status = CMD_RX;
  }
}   
   
   

