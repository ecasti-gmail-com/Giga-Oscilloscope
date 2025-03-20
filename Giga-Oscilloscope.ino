//#include "STMSpeeduino.h"
#include "KickFFT.h"
#include "Arduino_GigaDisplay_GFX.h"
#include "Arduino_GigaDisplayTouch.h"
#include "SDRAMGFXcanvas.h"
GigaDisplay_GFX display;
Arduino_GigaDisplayTouch Touch;
SDRAMGFXcanvas16 scope(512, 256);
SDRAMGFXcanvas16 fft_screen(512, 192);
Adafruit_GFX *pgfx = &display;


#define fftsize 512
#define SAMPLES fftsize * 2 


uint32_t mag[fftsize];
int16_t input[fftsize];
uint16_t FrameBuffer1[SAMPLES * 10];
uint16_t FrameBuffer2[SAMPLES * 10];
uint16_t FrameBuffer_Screen1[SAMPLES];
uint16_t FrameBuffer_Screen2[SAMPLES];
int resolution = 8;
int mode = 1;

uint8_t sample_index = 4;
uint8_t sample_time[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };
int frequency[] = { 2000000, 1750000, 940000, 600000, 330000, 175000, 30000, 15000 };
float period[] = {1.9,2.25,4.15,6.75,11.65,22,125,250};
String frequency_string[] = { "2Mhz", "1.75Mhz", "940Khz", "600Khz", "330Khz", "175Khz", "30Khz", "15Khz" };
String frequency_string_ov[] = { "200Khz", "175Khz", "94Khz", "60Khz", "33Khz", "17.5Khz", "3Khz", "1.5Khz" };


int frequency_dual[] = { 1000000, 875000, 470000, 300000, 165000, 85000, 15000, 7500 };
float period_dual[] = {3.8,4.5,8.25,13.5,22.75,44.25,250,500};
String frequency_string_dual[] = { "1Mhz", "875Khz", "470Khz", "300Khz", "165Khz", "85Khz", "15Khz", "7500Hz" };
String frequency_string_ov_dual[] = { "100Khz", "87.5Khz", "47Khz", "30Khz", "16.5Khz", "8.5Khz", "1.5Khz", "750Hz" };

int f_index;
bool interleaved = true;
bool oversampling = false;

int offset;
int vmin;
long t1, told, tc;

int touch_x;
int touch_y;
const float Fs = 25000000.8;  //Hz

uint16_t startIndex = 0;
uint16_t endIndex = 0;
bool tfton = false;



void setup() {

  Serial.begin(115200);

  adcset();
  display.setRotation(1);
  display.begin();

  RefreshDisplay();
  scope.begin();
  fft_screen.begin();
  pgfx = &scope;
  Touch.begin();
}

void loop() {
 if (mode == 0) {
  while (!TransferInterleavedComplete()) {}
 } else {
  while (!TransferSimultaneousComplete()){}
 } 

  if (oversampling) {
    downsampling();
  } else {
    copy_buffer();
  }
  recapture();
  if ( mode < 2 ){
  drawScope();
  } else {
    drawXY();
  }

  if (tfton) {
    reset_fft();
    prepare_samples();
    calc_fft();
    drawFFT();
  }
  GetInput();
}
