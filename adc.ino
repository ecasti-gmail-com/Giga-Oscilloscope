
bool ADC1Initialized = 0;   //Stores information on if ADC1 is Initialized
bool ADC2Initialized = 0;   //Stores information on if ADC2 is Initialized
bool ClockInitialized = 0;  //Stores if clock is initialized
int Prescaler = 0;          //Stores the Calculated Prescaler value
int ADC1Size;               //Size of buffer for ADC
int ADC2Size;               //Size of buffer for ADC
int ADC12Size;              //Size of buffer for ADC

uint16_t* Buffer1Add;  //Address for the buffer
uint16_t* Buffer2Add;  //Address for the buffer

void adcset() {
  reset_bits();
  if (mode == 0) {
    ADCInterleaved(A0, resolution, 0, 50);
    if (oversampling) {
      captureInterleavedValues(SAMPLES * 10, FrameBuffer1);
    } else {
      captureInterleavedValues(SAMPLES, FrameBuffer1);
    }
  } else {
    ADCSimultaneous(A0, A3, resolution, 50, sample_index, 0);
    if (oversampling) {
      captureSimultaneousValues(SAMPLES * 10, FrameBuffer1, FrameBuffer2);
    } else {
      captureSimultaneousValues(SAMPLES, FrameBuffer1, FrameBuffer2);
    }
  }
}



void ADCInterleaved(int ADCChannel, int Resolution, bool Differential, double ClockSpeedMHZ) {

  if (!ClockInitialized) { SystemCLCKInit(ClockSpeedMHZ); };         //Initiate System clock
  ADC1_Init(ADCChannel, Resolution, Differential, sample_index, 0);  //Initiate ADC1
  ADC2_Init(ADCChannel, Resolution, Differential, sample_index, 0);  //Initiate ADC2
  ADCInterleavedConfig(Resolution);                                  //Configure Interleaved mode
  ADC1EN();                                                          //Enable ADC1
  ADC2EN();                                                          //Enable ADC2
  while (!READ_REG(ADC2->ISR & ADC_ISR_ADRDY)) {};                   //Waits for ADC1 to be ready
  while (!READ_REG(ADC1->ISR & ADC_ISR_ADRDY)) {};                   //Waits for ADC2 to be ready
  ADC1_Start();                                                      //Starts the conversions
}

void ADC1_Init(int AdcChannel, int Resolution, bool Differential, int SampleTime, int Samplenum) {
  //GPIOA PORT 1------------------------------------
  /***************ADC not Enabled yet********************/
  CLEAR_BIT(ADC1->CR, ADC_CR_DEEPPWD);     //Wake up from deep sleep
  SET_BIT(ADC1->CR, ADC_CR_ADVREGEN_Msk);  //Enable the voltage generator
  AdcChannel = ADC1PinRemap(AdcChannel);   //Get actual channel number
  //ADC CALIBRATION
  if (Differential) {
    SET_BIT(ADC1->CR, ADC_CR_ADCALDIF);
  };
  SET_BIT(ADC1->CR, ADC_CR_ADCALLIN);
  SET_BIT(ADC1->CR, ADC_CR_ADCAL);
  while (READ_REG(ADC1->CR & ADC_CR_ADCAL)) {};
  SET_BIT(ADC1->CFGR2, ADC_CFGR2_ROVSE);                                                           //Set oversampling
  ADC1->CFGR2 += 65536 * Samplenum;                                                                //Set number of samples
  ADC1->PCSEL = 0xFFFFF;                                                                           //Enable all channels
  ADC1->SQR1 = 64 * AdcChannel;                                                                    //Select channel
  ADC1->DIFSEL = 1048575 * Differential;                                                           //Enable differential mode if needed
  SET_BIT(ADC1->CFGR, ADC_CFGR_OVRMOD_Msk | ADC_CFGR_CONT_Msk | ADC_CFGR_JQM | ADC_CFGR_JDISCEN);  //Enable Continuous mode, Always overwrite data
  ResolutionSet(Resolution);                                                                       //Set resolution
  ADC1->SMPR1 = SampleTime << 12;                                                                  //Set sampletime
  ADC1Initialized = 1;
}


void ADC2_Init(int AdcChannel, int Resolution, bool Differential, int SampleTime, int Samplenum) {
  /***************ADC not Enabled yet********************/
  //Enable the voltage generator  SET_BIT(ADC2->CR, ADC_CR_ADVREGEN_Msk);  //Enable the voltage generator
  CLEAR_BIT(ADC2->CR, ADC_CR_DEEPPWD);
  SET_BIT(ADC2->CR, ADC_CR_ADVREGEN_Msk);  //Enable the voltage generator
  AdcChannel = ADC2PinRemap(AdcChannel);   //Get actual channel number
  //ADC CALIBRATION-----------------------------------
  if (Differential) {
    SET_BIT(ADC2->CR, ADC_CR_ADCALDIF);
  };
  SET_BIT(ADC2->CR, ADC_CR_ADCALLIN);
  SET_BIT(ADC2->CR, ADC_CR_ADCAL);
  while (READ_REG(ADC2->CR & ADC_CR_ADCAL)) {};
  SET_BIT(ADC2->CFGR2, ADC_CFGR2_ROVSE);                                                           //Allow Oversampling
  ADC2->CFGR2 += 65536 * Samplenum;                                                                //Set number of samples
  ADC2->PCSEL = 0xFFFFF;                                                                           //Enable all channels
  ADC2->SQR1 = 64 * AdcChannel;                                                                    //Select a channel
  ADC2->DIFSEL = 1048575 * Differential;                                                           //Enable differential mode if needed
  SET_BIT(ADC2->CFGR, ADC_CFGR_OVRMOD_Msk | ADC_CFGR_CONT_Msk | ADC_CFGR_JQM | ADC_CFGR_JDISCEN);  //Enable Continuous mode, Always overwrite data
  Resolution2Set(Resolution);
  //   ADC2->SMPR1 = SampleTime;
  ADC2->SMPR1 = SampleTime << 15;        //Set RESOLUTION
  ADC2->SMPR1 = SampleTime * 153391689;  //Divide
  ADC2->SMPR2 = SampleTime * 153391689;  //Divide
  ADC2Initialized = 1;
}


void ResolutionSet(int Resolution) {
  //Sets Resolution for the ADC
  CLEAR_BIT(ADC1->CFGR, ADC_CFGR_RES);
  switch (Resolution) {
    case 8:
      SET_BIT(ADC1->CFGR, ADC_CFGR_RES);
      break;
    case 10:
      SET_BIT(ADC1->CFGR, ADC_CFGR_RES_0 | ADC_CFGR_RES_1);
      break;
    case 12:
      SET_BIT(ADC1->CFGR, ADC_CFGR_RES_2 | ADC_CFGR_RES_1);
      break;
    case 14:
      SET_BIT(ADC1->CFGR, ADC_CFGR_RES_0 | ADC_CFGR_RES_2);
      break;
    default:
      break;
  };
}

void Resolution2Set(int Resolution) {
  //Sets Resolution for the ADC
  CLEAR_BIT(ADC2->CFGR, ADC_CFGR_RES);

  switch (Resolution) {
    case 8:
      SET_BIT(ADC2->CFGR, ADC_CFGR_RES);
      break;
    case 10:
      SET_BIT(ADC2->CFGR, ADC_CFGR_RES_0 | ADC_CFGR_RES_1);
      break;
    case 12:
      SET_BIT(ADC2->CFGR, ADC_CFGR_RES_2 | ADC_CFGR_RES_1);
      break;
    case 14:
      SET_BIT(ADC2->CFGR, ADC_CFGR_RES_0 | ADC_CFGR_RES_2);
      break;
    default:
      break;
  };
}

void SystemCLCKInit(double ClockSpeedMHZ) {

#if defined(ARDUINO_GIGA)
  RCC->PLLCKSELR = 0x0200F022;  //Divide 16mhz clock to 1
#elif defined(STM32H747xx)
  RCC->PLLCKSELR = 0x02017022;  //Divide 24mhz clock to 1
#else
#error Unsupported microcontroller, this library currently only works for STM32H747 based arduino boards.
#endif
  int AdcPrescDivision = 1;  //How much does clock get divided
  ClockSpeedMHZ *= 2;        //ADC always further divides clockspeed by 2, compensate

  //Calculate proper prescaler if needed

  if (128 < 1.2 / ClockSpeedMHZ) {
    AdcPrescDivision = 256;
    Prescaler = 11;
  } else if (64 < 1.2 / ClockSpeedMHZ) {
    AdcPrescDivision = 128;
    Prescaler = 10;
  } else if (32 < 1.2 / ClockSpeedMHZ) {
    AdcPrescDivision = 64;
    Prescaler = 9;
  } else if (16 < 1.2 / ClockSpeedMHZ) {
    AdcPrescDivision = 32;
    Prescaler = 8;
  } else if (12 < 1.2 / ClockSpeedMHZ) {
    AdcPrescDivision = 16;
    Prescaler = 7;
  } else if (10 < 1.2 / ClockSpeedMHZ) {
    AdcPrescDivision = 12;
    Prescaler = 6;
  } else if (8 < 1.2 / ClockSpeedMHZ) {
    AdcPrescDivision = 10;
    Prescaler = 5;
  } else if (6 < 1.2 / ClockSpeedMHZ) {
    AdcPrescDivision = 8;
    Prescaler = 4;
  } else if (4 < 1.2 / ClockSpeedMHZ) {
    AdcPrescDivision = 6;
    Prescaler = 3;
  } else if (2 < 1.2 / ClockSpeedMHZ) {
    AdcPrescDivision = 4;
    Prescaler = 2;
  } else if (1 < 1.2 / ClockSpeedMHZ) {
    AdcPrescDivision = 2;
    Prescaler = 1;
  };

  //Calculate dividor to use

  int dividor = ceil(150.0f / ClockSpeedMHZ / AdcPrescDivision);
  if (dividor > 128) {
    dividor = 128;
  };

  //Set generator speed, remain in 150-300mhz range as long as possible

  RCC->PLL2DIVR = 0x01010000;                                             //Sets the generator speed
  RCC->PLL2DIVR += dividor * 512 - 512;                                   //Calculation
  RCC->PLL2DIVR += ceil(ClockSpeedMHZ * dividor * AdcPrescDivision - 4);  //Sets the generator speed

  SET_BIT(RCC->CR, RCC_CR_PLL2ON);             //Enable generator
  SET_BIT(RCC->PLLCFGR, RCC_PLLCFGR_DIVP2EN);  //Enable output


  SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_ADC12EN_Msk | RCC_AHB1ENR_DMA1EN);                                          //Enable ADC 1 and 2 and DMA1
  SET_BIT(RCC->AHB4ENR, RCC_AHB4ENR_GPIOAEN | RCC_AHB4ENR_GPIOBEN | RCC_AHB4ENR_GPIOCEN | RCC_AHB4ENR_ADC3EN);  //Enable GPIOs and ADC3


  //Set boost mode so ADC can run faster

  if (ClockSpeedMHZ > 50) {
    SET_BIT(ADC1->CR, ADC_CR_BOOST);
    SET_BIT(ADC2->CR, ADC_CR_BOOST);
    SET_BIT(ADC3->CR, ADC_CR_BOOST);
  } else if (ClockSpeedMHZ > 25) {
    SET_BIT(ADC1->CR, ADC_CR_BOOST_1);
    SET_BIT(ADC2->CR, ADC_CR_BOOST_1);
    SET_BIT(ADC3->CR, ADC_CR_BOOST_0);
  } else if (ClockSpeedMHZ > 12.5) {
    SET_BIT(ADC1->CR, ADC_CR_BOOST_0);
    SET_BIT(ADC2->CR, ADC_CR_BOOST_0);
    SET_BIT(ADC3->CR, ADC_CR_BOOST_0);
  }
}

void captureInterleavedValues(int Size, uint16_t* BufferAddress) {
  ADC1_Stop();                                               //Stop ADC
  ADC2_Stop();                                               //Stop ADC
  ADC12Size = Size;                                          //Write down the ADC Size
  Buffer1Add = BufferAddress;                                //Write down the buffer address
  SCB_InvalidateDCache_by_Addr(BufferAddress, Size * 2);     //Invalidate the cache so new values get read
  SET_BIT(ADC1->CFGR, ADC_CFGR_DMNGT_0);                     //Set ADC to generate DMA requests
  SET_BIT(ADC2->CFGR, ADC_CFGR_DMNGT_0);                     //Set ADC to generate DMA requests
  SET_BIT(ADC12_COMMON->CCR, ADC_CCR_DAMDF_1);               //Sets DMA for double ADC mode
  ConfigDMA1_S5(Size / 2, 0x4002230C, BufferAddress, 2, 9);  //Configure the relevant DMA stream as required
  ADC1_Start();                                              //Start the ADC
  ADC2_Start();                                              //Start the ADC
}

void reset_bits() {
  //      SET_BIT(ADC12_COMMON->CCR, ADC_CCR_DUAL_0); //Configure Simultaneous mode
  CLEAR_BIT(ADC12_COMMON->CCR, ADC_CCR_DUAL_0 | ADC_CCR_DUAL_1);  //Configures Interleaved mode
                                                                  //SET_BIT(ADC12_COMMON->CCR, 0x00000000);
  CLEAR_BIT(ADC1->CFGR, ADC_CFGR_DMNGT_0);                        //Set ADC to generate DMA requests
  CLEAR_BIT(ADC2->CFGR, ADC_CFGR_DMNGT_0);                        //Set ADC to generate DMA requests
  CLEAR_BIT(ADC12_COMMON->CCR, ADC_CCR_DAMDF_1);                  //Sets DMA for double ADC mode
}
void recaptureInterleavedValues() {
  ADC1_Start();  //Start the ADC
  //Clear the transfer flags
  SET_BIT(DMA1->HIFCR, DMA_HIFCR_CHTIF5);
  SET_BIT(DMA1->HIFCR, DMA_HIFCR_CTCIF5);
  DMA1_Stream5->CR |= 1 << 0;  //Enable the DMA stream

  SCB_InvalidateDCache_by_Addr(Buffer1Add, ADC12Size * 2);  //Invalidate the cache
}

bool TransferInterleavedComplete() {
  return (READ_BIT(DMA1->HISR, DMA_HISR_TCIF5));  //Check if transfer is complete
}

void ADC1_Start() {
  while (!READ_REG(ADC1->ISR & ADC_ISR_ADRDY)) {};  //Wait for ADC to be ready to start
  SET_BIT(ADC1->CR, ADC_CR_ADSTART_Msk);            //Start
}


void ADC2_Start() {
  while (!READ_REG(ADC2->ISR & ADC_ISR_ADRDY)) {};  //Wait for ADC to be ready to start
  SET_BIT(ADC2->CR, ADC_CR_ADSTART_Msk);            //Start
}


void ADCInterleavedConfig(int Resolution) {
  SET_BIT(ADC12_COMMON->CCR, ADC_CCR_DUAL_0 | ADC_CCR_DUAL_1);  //Configures Interleaved mode
}

void ADC1EN() {
  ADC12_COMMON->CCR |= Prescaler << 18;  //Set Prescaler value
  SET_BIT(ADC1->CR, ADC_CR_ADEN_Msk);    //Enable
}

void ADC2EN() {
  ADC12_COMMON->CCR |= Prescaler << 18;  //Set Prescaler value
  SET_BIT(ADC2->CR, ADC_CR_ADEN_Msk);    //Enable
}

int ADC1PinRemap(int Pin) {
  switch (Pin) {
    case 100: return (0);
    case 101: return (1);
    case 56: return (3);
    case A3: return (5);
    case A0: return (4);
    case 5: return (7);
    case A1: return (8);
    case A2: return (9);
    case A6: return (10);
    case 73: return (11);
    case A5: return (12);
    case A4: return (13);
    case 3: return (14);
    case 2: return (15);
    case A7: return (16);
    case 66: return (17);
    case DAC_0: return (18);
    case DAC_1: return (19);
    default: return (-1);
  };
}  //Remap pins to channel IDs

int ADC2PinRemap(int Pin) {
  switch (Pin) {
    case 100: return (0);
    case 101: return (1);
    case 56: return (3);
    case A0: return (4);
    case A3: return (5);
    case 5: return (7);
    case A1: return (8);
    case A2: return (9);
    case A6: return (10);
    case 73: return (11);
    case A5: return (12);
    case A4: return (13);
    case 3: return (14);
    case 2: return (15);
    case DAC_0: return (18);
    case DAC_1: return (19);
    default: return (-1);
  };
}  //Remap pins to channel IDs


void ADC2_Stop() {
  SET_BIT(ADC2->CR, ADC_CR_ADSTP);               //Stop the ADC
  while (READ_REG(ADC2->CR & ADC_CR_ADSTP)) {};  //Wait for ADC to stop
}

void ADC1_Stop() {
  SET_BIT(ADC1->CR, ADC_CR_ADSTP);               //Stop the ADC
  while (READ_REG(ADC1->CR & ADC_CR_ADSTP)) {};  //Wait for ADC to stop
}


void ConfigDMA1_S5(uint16_t Size, int InAddress, uint16_t* OutAddress, int TransferSize, int DmaMUX) {

  //Clears Transfer flags
  SET_BIT(DMA1->HIFCR, DMA_HIFCR_CHTIF5);
  SET_BIT(DMA1->HIFCR, DMA_HIFCR_CTCIF5);

  DMA1_Stream5->NDTR = Size;                  //Sets size of transfer
  DMA1_Stream5->PAR = InAddress;              //Sets the Peripheral address to read from
  DMA1_Stream5->M0AR = (uint32_t)OutAddress;  //Sets the address to write to

  DMA1_Stream5->CR &= 0 << 0;  //Make sure the DMA is off

  DMA1_Stream5->CR = 0x30400;                 //Setup the DMA stream
  DMA1_Stream5->CR |= 0x2800 * TransferSize;  //Set the size (0-8bits, 1-16, 2-32)
  DMAMUX1_Channel5->CCR = (DmaMUX << 0);      //Set the DMAmux to allow the right peripheral to connect

  DMA1_Stream5->CR |= 1 << 0;  //Enable the DMA stream
}

void ConfigDMA1_S6(uint16_t Size, int InAddress, uint16_t* OutAddress, int TransferSize, int DmaMUX) {

  //Clears Transfer flags
  SET_BIT(DMA1->HIFCR, DMA_HIFCR_CHTIF6);
  SET_BIT(DMA1->HIFCR, DMA_HIFCR_CTCIF6);

  DMA1_Stream6->NDTR = Size;                  //Sets size of transfer
  DMA1_Stream6->PAR = InAddress;              //Sets the Peripheral address to read from
  DMA1_Stream6->M0AR = (uint32_t)OutAddress;  //Sets the address to write to

  DMA1_Stream6->CR &= 0 << 0;  //Make sure the DMA is off

  DMA1_Stream6->CR = 0x30400;                 //Setup the DMA stream
  DMA1_Stream6->CR |= 0x2800 * TransferSize;  //Set the size (0-8bits, 1-16, 2-32)
  DMAMUX1_Channel6->CCR = (DmaMUX << 0);      //Set the DMAmux to allow the right peripheral to connect

  DMA1_Stream6->CR |= 1 << 0;  //Enable the DMA stream
}

void ConfigDMA1_S7(uint16_t Size, int InAddress, uint16_t* OutAddress, int TransferSize, int DmaMUX) {

  //Clears Transfer flags
  SET_BIT(DMA1->HIFCR, DMA_HIFCR_CHTIF7);
  SET_BIT(DMA1->HIFCR, DMA_HIFCR_CTCIF7);

  DMA1_Stream7->NDTR = Size;                  //Sets size of transfer
  DMA1_Stream7->PAR = InAddress;              //Sets the Peripheral address to read from
  DMA1_Stream7->M0AR = (uint32_t)OutAddress;  //Sets the address to write to

  DMA1_Stream7->CR &= 0 << 0;  //Make sure the DMA is off

  DMA1_Stream7->CR = 0x30400;                 //Setup the DMA stream
  DMA1_Stream7->CR |= 0x2800 * TransferSize;  //Set the size (0-8bits, 1-16, 2-32)
  DMAMUX1_Channel7->CCR = (DmaMUX << 0);      //Set the DMAmux to allow the right peripheral to connect

  DMA1_Stream7->CR |= 1 << 0;  //Enable the DMA stream
}



void captureSimultaneousValues(int Size, uint16_t* Buffer1Address, uint16_t* Buffer2Address) {
  ADC1_Stop();                                             //Stop ADC
  ADC2_Stop();                                             //Stop ADC
  Buffer1Add = Buffer1Address;                             //Write down the buffer address
  Buffer2Add = Buffer2Address;                             //Write down the buffer address
  ADC12Size = Size;                                        //Write down the ADC Size
  SCB_InvalidateDCache_by_Addr(Buffer1Address, Size * 2);  //Invalidate the cache so new values get read
  SCB_InvalidateDCache_by_Addr(Buffer2Address, Size * 2);  //Invalidate the cache so new values get read
  SET_BIT(ADC1->CFGR, ADC_CFGR_DMNGT_0);                   //Set ADC to generate DMA requests
  SET_BIT(ADC2->CFGR, ADC_CFGR_DMNGT_0);                   //Set ADC to generate DMA requests
  ConfigDMA1_S5(Size, 0x40022040, Buffer1Address, 1, 9);   //Configure the relevant DMA stream as required
  ConfigDMA1_S6(Size, 0x40022140, Buffer2Address, 1, 10);  //Configure the relevant DMA stream as required
  ADC1_Start();                                            //Start the ADC
  ADC2_Start();                                            //Start the ADC
}


void recaptureSimultaneousValues() {
  ADC1_Start();  //Start the ADC
  ADC2_Start();
  //Clear the transfer flags
  SET_BIT(DMA1->HIFCR, DMA_HIFCR_CHTIF5);
  SET_BIT(DMA1->HIFCR, DMA_HIFCR_CTCIF5);
  SET_BIT(DMA1->HIFCR, DMA_HIFCR_CHTIF6);
  SET_BIT(DMA1->HIFCR, DMA_HIFCR_CTCIF6);
  DMA1_Stream5->CR |= 1 << 0;  //Enable the DMA stream
  DMA1_Stream6->CR |= 1 << 0;  //Enable the DMA stream

  SCB_InvalidateDCache_by_Addr(Buffer1Add, ADC12Size * 2);  //Invalidate the cache
  SCB_InvalidateDCache_by_Addr(Buffer2Add, ADC12Size * 2);  //Invalidate the cache
}

bool TransferSimultaneousComplete() {
  if (READ_BIT(DMA1->HISR, DMA_HISR_TCIF5) && READ_BIT(DMA1->HISR, DMA_HISR_TCIF6)) {
    //if (READ_BIT(DMA1->HISR, DMA_HISR_TCIF5)) {
    return (1);  //Check if transfer is complete
  } else {
    return (0);
  }
}

void ADCSimultaneous(int ADC1Channel, int ADC2Channel, int Resolution, double ClockSpeedMHZ, int SampleTime, int Samplenum) {

  if (!ClockInitialized) { SystemCLCKInit(ClockSpeedMHZ); };     //Initiate System clock
  ADC1_Init(ADC1Channel, Resolution, 0, SampleTime, Samplenum);  //Initiate ADC1
  ADC2_Init(ADC2Channel, Resolution, 0, SampleTime, Samplenum);  //Initiate ADC2
  ADCSimultaneousConfig();                                       //Configure Simultaneous mode
  ADC1EN();                                                      //Enable ADC1
  ADC2EN();                                                      //Enable ADC2
  while (!READ_REG(ADC2->ISR & ADC_ISR_ADRDY)) {};               //Waits for ADC1 to be ready
  while (!READ_REG(ADC1->ISR & ADC_ISR_ADRDY)) {};               //Waits for ADC2 to be ready
  ADC1_Start();                                                  //Starts the conversions

}  //Initiates and starts ADC1 and ADC2 in Simultaneous modes.

void ADCSimultaneousConfig() {

  // SET_BIT(ADC12_COMMON->CCR, ADC_CCR_DUAL_0);  //Configure Simultaneous mode
}


void recapture() {
  if (mode == 0) {
    recaptureInterleavedValues();
  } else {
    recaptureSimultaneousValues();
  }
}
