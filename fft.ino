void reset_fft() {
  int i;
  for (i = 0; i < fftsize; i++) {
    mag[i] = 0;
  }
}

void prepare_samples() {
  int i;
  for (i = 0; i < fftsize; i++) {
    input[i] = FrameBuffer_Screen1[i+offset];
  }
}
void calc_fft() {
 KickFFT<int16_t>::fft(512, input, mag);
}