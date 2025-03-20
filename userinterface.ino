void RefreshDisplay() {
  Serial.println("Refresh Display");
  display.fillScreen(WHITE);
  display.setTextSize(3);
  display.setTextColor(RED);
  display.fillRect(10, 10, 512, 256, BLACK);
  display.drawRect(9, 9, 514, 258, RED);
  display.drawRect(8, 8, 516, 260, RED);

  if (tfton) {
    display.fillRect(10, 276, 512, 192, BLACK);
    display.drawRect(9, 275, 514, 194, RED);
    display.drawRect(8, 274, 516, 196, RED);
  }
  refresh_dashboard();

  drawbuttons();
}

void refresh_dashboard() {
  display.setTextColor(GREEN);
  display.setTextSize(2);
  display.fillRect(532, 10, 260, 128, BLACK);
  display.drawRect(531, 9, 262, 130, RED);
  display.drawRect(530, 8, 264, 132, RED);
  display.setCursor(544, 20);
  display.print("Sample Time: ");
  display.print(sample_index);

  display.setCursor(544, 50);
  display.print("T/Div: ");
  if (mode == 0) {
    if (oversampling) {
      display.print(int(period[sample_index] * 10));
    } else {
      display.print(period[sample_index]);
    }
  } else {
    if (oversampling) {
      display.print(int(period_dual[sample_index] * 10));
    } else {
      display.print(period_dual[sample_index]);
    }
  }
  display.print(" ns");
  display.setCursor(544, 80);
  display.print("Oversampling: ");
  if (oversampling) {
    display.print("On");
  } else {
    display.print("Off");
  }
}


void toggle_tft() {
  if (tfton) {
    tfton = false;
  } else {
    tfton = true;
  }
  RefreshDisplay();
}

void toggle_Oversampling() {
  if (oversampling) {
    oversampling = false;
  } else {
    oversampling = true;
  }
  drawbuttons();
  refresh_dashboard();
  adcset();
}


void toggle_resolution() {
  switch (resolution) {
    case 8:
      resolution = 10;
      break;
    case 10:
      resolution = 12;
      break;
    case 12:
      resolution = 14;
      break;
    case 14:
      resolution = 16;
      break;
    case 16:
      resolution = 8;
      break;
  }
  drawbuttons();
  adcset();
}

void toggle_sample_time(bool increase) {
  if (increase) {
    sample_index++;
    if (sample_index > 7) {
      sample_index = 7;
    }
  } else {
    sample_index--;
    if ((mode > 0) && (sample_index == 0)) {
      sample_index = 1;
    }
    if (sample_index > 250) {
      sample_index = 0;
    }
  }
  refresh_dashboard();
  adcset();
}

void drawbuttons() {
  display.setTextColor(BLACK);
  display.setTextSize(2);
  display.setCursor(544, 300);
  if (tfton) {
    display.fillRoundRect(532, 276, 100, 80, 10, GREEN);
    display.print("TFT On");
  } else {
    display.fillRoundRect(532, 276, 100, 80, 10, RED);
    display.print("TFT Off");
  }
  display.setCursor(664, 300);
  if (oversampling) {

    display.fillRoundRect(652, 276, 100, 80, 10, GREEN);
    display.print("X 10");
  } else {
    display.fillRoundRect(652, 276, 100, 80, 10, RED);
    display.print("X 10");
  }
  display.setCursor(544, 400);
  display.fillRoundRect(532, 376, 100, 80, 10, CYAN);
  display.print("TDiv +");
  display.setCursor(664, 400);
  display.fillRoundRect(652, 376, 100, 80, 10, CYAN);
  display.print("TDiv -");

  display.setCursor(544, 190);
  display.fillRoundRect(532, 176, 100, 80, 10, CYAN);
  display.print("Mode");
  display.setCursor(544, 220);
  switch (mode) {
    case 0:
      display.print("Interlv");
      break;
    case 1:
      display.print("Dual");
      break;
    case 2:
      display.print("X/Y");
      break;
  }
  display.setCursor(664, 190);
  display.fillRoundRect(652, 176, 100, 80, 10, CYAN);
  display.print("Res");
  display.setCursor(664, 220);
  display.print(resolution);
  display.print(" Bit");
}

void toggle_mode() {
  switch (mode) {
    case 0:
      mode = 1;
      break;
    case 1:
      mode = 2;
      if (sample_index == 0) {
        sample_index = 1;
      }
      break;
    case 2:
      mode = 0;
      if (sample_index == 0) {
        sample_index = 1;
      }
      break;
  }
  drawbuttons();
  adcset();
}

void GetInput() {
  uint8_t contact;
  GDTpoint_t myPoint[1];
  contact = Touch.getTouchPoints(myPoint);
  if (contact > 0) {
    Serial.print("X: ");
    Serial.println(touch_x);
    Serial.print("Y: ");
    Serial.println(touch_y);
    touch_x = myPoint[0].y;
    touch_y = 480 - myPoint[0].x;
    Touch.end();
    if ((touch_x > 532) && (touch_x < 632) && (touch_y > 276) && (touch_y < 356)) {

      toggle_tft();
    }

    if ((touch_x > 652) && (touch_x < 752) && (touch_y > 276) && (touch_y < 356)) {
      toggle_Oversampling();
    }

    if ((touch_x > 532) && (touch_x < 632) && (touch_y > 376) && (touch_y < 456)) {
      toggle_sample_time(true);
    }

    if ((touch_x > 652) && (touch_x < 752) && (touch_y > 376) && (touch_y < 456)) {
      toggle_sample_time(false);
    }
    if ((touch_x > 652) && (touch_x < 752) && (touch_y > 176) && (touch_y < 256)) {
      toggle_resolution();
    }
    if ((touch_x > 532) && (touch_x < 632) && (touch_y > 176) && (touch_y < 256)) {

      toggle_mode();
    }
    Touch.begin();
  }
}
