int findmin() {
  int xmin = 5;
  for (int i = 6; i < 256; i++) {
    if (FrameBuffer_Screen1[i] < FrameBuffer_Screen1[xmin]) {
      xmin = i;
    }
  }
  return xmin;
}



void drawScope() {
  offset = findmin();
  // int offset = 0;
  scope.fillScreen(BLACK);
  scope.drawFastHLine(0, 128, 512, RED);
  scope.drawFastHLine(0, 127, 512, RED);
  scope.drawFastHLine(0, 64, 512, RED);
  scope.drawFastHLine(0, 192, 512, RED);
  for (int i = 63; i < 512; i += 64) {
    scope.drawFastVLine(i, 0, 256, RED);
  }
 for (int i= 0;i<512; i++){
      switch (resolution) {
      case 8:
        pgfx->drawLine(i, 256 - (FrameBuffer_Screen1[i + offset]), i + 1, 256 - (FrameBuffer_Screen1[i + offset + 1]), GREEN);

        break;
      case 10:
        pgfx->drawLine(i, 256 - (FrameBuffer_Screen1[i + offset] / 4), i + 1, 256 - (FrameBuffer_Screen1[i + offset + 1] / 4), GREEN);
        break;
      case 12:
        pgfx->drawLine(i, 256 - (FrameBuffer_Screen1[i + offset] / 16), i + 1, 256 - (FrameBuffer_Screen1[i + offset + 1] / 16), GREEN);
        break;
      case 14:
        pgfx->drawLine(i, 256 - (FrameBuffer_Screen1[i + offset] / 64), i + 1, 256 - (FrameBuffer_Screen1[i + offset + 1] / 64), GREEN);
        break;
      case 16:
        pgfx->drawLine(i, 256 - (FrameBuffer_Screen1[i + offset] / 256), i + 1, 256 - (FrameBuffer_Screen1[i + offset + 1] / 256), GREEN);
        break;
    }
}
  if (mode ==1) {
    for (int i = 0; i < 512; i++) {
    switch (resolution) {
      case 8:
        pgfx->drawLine(i, 256 - (FrameBuffer_Screen2[i + offset]), i + 1, 256 - (FrameBuffer_Screen2[i + offset + 1]), ORANGE);
        break;
      case 10:
        pgfx->drawLine(i, 256 - (FrameBuffer_Screen2[i + offset] / 4), i + 1, 256 - (FrameBuffer_Screen2[i + offset + 1] / 4), ORANGE);
        break;
      case 12:
        pgfx->drawLine(i, 256 - (FrameBuffer_Screen2[i + offset] / 16), i + 1, 256 - (FrameBuffer_Screen2[i + offset + 1] / 16), ORANGE);
        break;
      case 14:
        pgfx->drawLine(i, 256 - (FrameBuffer_Screen2[i + offset] / 64), i + 1, 256 - (FrameBuffer_Screen2[i + offset + 1] / 64), ORANGE);
        break;
      case 16:
        pgfx->drawLine(i, 256 - (FrameBuffer_Screen2[i + offset] / 256), i + 1, 256 - (FrameBuffer_Screen2[i + offset + 1] / 256), ORANGE);
        break;        
    }
  }
  }
  display.drawRGBBitmap(10, 10, scope.getBuffer(), 512, 256);
}


void drawXY() {
  offset = findmin();
  // int offset = 0;
  scope.fillScreen(BLACK);
  scope.drawRect(128, 0, 256, 255, RED);
  scope.drawFastHLine(128, 128, 256, RED);
  scope.drawFastHLine(127, 127, 256, RED);
  scope.drawFastVLine(256, 0, 256, RED);
  scope.drawFastVLine(255, 0, 256, RED);
 

for (int i = 0; i < 511; i++) {
    switch (resolution) {
      case 8:
       // scope.drawPixel(FrameBuffer_Screen1[i + offset]+128, FrameBuffer_Screen2[i + offset], GREEN);
        pgfx->drawLine(FrameBuffer_Screen1[i + offset]+128,  FrameBuffer_Screen2[i + offset],FrameBuffer_Screen1[i + offset+1]+128,  FrameBuffer_Screen2[i + offset+1] , GREEN);
        break;
      case 10:
       // scope.drawPixel((FrameBuffer_Screen1[i + offset]/4)+128, FrameBuffer_Screen2[i + offset]/4, GREEN);
        pgfx->drawLine((FrameBuffer_Screen1[i + offset]/4)+128, FrameBuffer_Screen2[i + offset]/4,(FrameBuffer_Screen1[i + offset+1]/4)+128, FrameBuffer_Screen2[i + offset+1]/4, GREEN);
        break;
      case 12:
      //  pgfx->drawLine(i, 256 - (FrameBuffer_Screen1[i + offset] / 16), i + 1, 256 - (FrameBuffer_Screen1[i + offset + 1] / 16), GREEN);
          scope.drawPixel((FrameBuffer_Screen1[i + offset]/16)+128, FrameBuffer_Screen2[i + offset]/16, GREEN);
        break;
      case 14:
      //  pgfx->drawLine(i, 256 - (FrameBuffer_Screen1[i + offset] / 64), i + 1, 256 - (FrameBuffer_Screen1[i + offset + 1] / 64), GREEN);
          scope.drawPixel((FrameBuffer_Screen1[i + offset]/64)+128, FrameBuffer_Screen2[i + offset]/64, GREEN);
        break;
      case 16:
        scope.drawPixel((FrameBuffer_Screen1[i + offset]/256)+128, FrameBuffer_Screen2[i + offset]/256, GREEN);
     //   pgfx->drawLine(i, 256 - (FrameBuffer_Screen1[i + offset] / 256), i + 1, 256 - (FrameBuffer_Screen1[i + offset + 1] / 256), GREEN);
        break;
    }
}
  
  display.drawRGBBitmap(10, 10, scope.getBuffer(), 512, 256);
}

void drawFFT() {
  double max_sample = 0;
  int y, l;
  int scan;
  int f_index;
  int freq;
  double avg = 0;
  char frequency_string[10];
  fft_screen.fillScreen(BLACK);
  fft_screen.drawFastHLine(0, 128, 512, RED);
  fft_screen.drawFastHLine(0, 127, 512, RED);
  fft_screen.drawFastHLine(0, 64, 512, RED);
  for (int i = 63; i < 512; i += 64) {
    fft_screen.drawFastVLine(i, 1, 185, RED);
  }

  for (scan = 1; scan < fftsize / 2; scan++) {
    avg += mag[scan];
    if (mag[scan] > max_sample) {
      max_sample = mag[scan];
      f_index = scan;
    }
  }
  avg /= (fftsize / 2);
  for (scan = 1; scan < fftsize / 2; scan++) {
    l = int((190 * mag[scan] / max_sample) * log(192));
    y = 192 - l;
    fft_screen.drawFastVLine(scan * 2, y, l, GREEN);
    fft_screen.drawFastVLine((scan * 2) + 1, y, l, GREEN);
  }
  display.drawRGBBitmap(10, 276, fft_screen.getBuffer(), 512, 192);
  display.setTextColor(GREEN);
  display.setTextSize(2);
  display.setCursor(544, 110);
  display.print("Freq: ");
  display.setTextColor(BLUE);
  if (avg > (max_sample / 10)) {
    sprintf(frequency_string, "Noise");
  } else {
     if (mode == 0) {



    if (oversampling) {
      freq = (float(frequency[sample_index]) / 320.0) * f_index;
      Serial.println(freq);
    } else {
      freq = (float(frequency[sample_index]) / 32.0) * f_index;
      Serial.println(freq);
    }
    if (freq > 20000) {
      sprintf(frequency_string, "%d Khz", int(freq / 1000));
    } else {
      sprintf(frequency_string, "%d Hz", freq);
    }
     } else  {

    if (oversampling) {
      freq = (float(frequency_dual[sample_index]) / 320.0) * f_index;
      Serial.println(freq);
    } else {
      freq = (float(frequency_dual[sample_index]) / 32.0) * f_index;
      Serial.println(freq);
    }
    if (freq > 20000) {
      sprintf(frequency_string, "%d Khz", int(freq / 1000));
    } else {
      sprintf(frequency_string, "%d Hz", freq);
    }
     }
  
  
  }
  display.fillRect(600, 110, 150, 18, BLACK);
  display.print(frequency_string);
}
