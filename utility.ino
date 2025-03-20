void copy_buffer() {
  for (int i = 0; i < SAMPLES; i++) {
    FrameBuffer_Screen1[i] = FrameBuffer1[i];
  }
  if (mode > 0) {
    for (int i = 0; i < SAMPLES; i++) {
      FrameBuffer_Screen2[i] = FrameBuffer2[i];
    }
  }
}

void downsampling() {
  int avg, i, j;
  for (i = 0; i < SAMPLES; i++) {
    avg = 0;
    for (j = 0; j < 10; j++) {
      avg += FrameBuffer1[i * 10 + j];
    }
    FrameBuffer_Screen1[i] = int(avg / 10);
  }
  if (mode > 0) {
    for (i = 0; i < SAMPLES; i++) {
      avg = 0;
      for (j = 0; j < 10; j++) {
        avg += FrameBuffer2[i * 10 + j];
      }
      FrameBuffer_Screen2[i] = int(avg / 10);
    }
  }
}



