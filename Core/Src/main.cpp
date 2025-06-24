/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <errno.h>
#include <sys/unistd.h>

#include <cstdint>
#include <cstddef>
#include <new>
#include <map>

#include "queue.h"
#include "semphr.h"

#include <tgmath.h>
#include <Thermistor.h>
#include <CanId.h>
#include <CanMessageGenericParser.h>
#include <String.h>
#include <string.h>
using namespace std;

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define  F_CLK_TIM_INPUT 168000000UL
#define PIN_NAME_LENGTH 5
#define BOARD_DEFAULT_ADDRESS 88;
#define EEPROM_DEVICE_ADDRESS 0b10101110
#define EEPROM_WRITE_READ_TIMEOUT 20000
#define EEPROM_MEM_SIZE 2
#define EEPROM_STORAGE_STATE_SIZE sizeof(uint16_t)
#define EEPROM_ADDRESS_SIZE sizeof(uint16_t)
#define EEPROM_THERMISTORS_STATUS_SIZE sizeof(uint64_t)

#define EEPROM_CONFIGURATION_SIGNATURE "iwpThrmCfg8"
#define EEPROM_CONFIGURATION_SIGNATURE_SIZE sizeof(EEPROM_CONFIGURATION_SIGNATURE)
#define EEPROM_CONFIGURATION_SIGNATURE_ADDRESS 0


// Structure to hold calculated PWM parameters
struct PwmConfig {
    uint16_t prescaler; // PSC register value (actual divider is prescaler + 1)
    uint16_t arr;       // ARR register value (period is arr + 1)
    uint16_t ccr;       // CCR register value (pulse width)
    bool success;       // True if a valid configuration was found
    uint32_t actualFreq; // Calculated actual frequency
    float actualDuty;    // Calculated actual duty cycle
};

struct TemperatureReadings {
	uint32_t pinNumber;
	uint32_t sensorNumber;
	double temperature;
	bool hasError;
	bool hasReadings;

	TemperatureReadings(uint32_t pinNumber, uint32_t sensorNumber, double temperature, bool hasError)
		: pinNumber(pinNumber), sensorNumber(sensorNumber), temperature(temperature), hasError(false) {};
	TemperatureReadings() {
		hasError = false;
		hasReadings = false;
	};
};

struct ThermistorConfiguration {
	uint32_t sensorNumber;
	string pinName; //[PIN_NAME_LENGTH + 1];
	float tParam;
	float bParam;
	float cParam;
	float rParam;

	ThermistorConfiguration() {
		cParam = 0;
	}
};

enum SavedConfigurationState {
	emptyEEPROM,
	providedSignatureAndBoardAddress
};

volatile uint32_t adcValues[] = {0, 0, 0, 0, 0, 0};
const uint32_t adcBufferLength = sizeof(adcValues) / sizeof(adcValues[0]);

const string pinNames[adcBufferLength] = {
		"temp0",
		"temp1",
		"temp2",
		"temp3",
		"temp4",
		"temp5"
};

struct  __attribute__((packed)) ThermistorSavedConfiguration {
	uint32_t sensorNumber = 255;
	float thermistorResistanceAt25 = R_THERMISTOR_DEFAULT;
	float betaValue = BETTA_DEFAULT;
	float cCoefficient = 0;
	float seriesResistorValue = R_BALANCE_DEFAULT;

	ThermistorSavedConfiguration(uint32_t sensorNumber): sensorNumber(sensorNumber) {};
};



struct __attribute__((packed)) BoardConfiguration {
	uint8_t boardSignature[EEPROM_CONFIGURATION_SIGNATURE_SIZE] = {0};
	SavedConfigurationState configurationState = SavedConfigurationState::emptyEEPROM;
	uint16_t boardAddress;

	BoardConfiguration() {}
};

enum TemperatureError {
	ok,
	shortCircuit,
	shortToVcc,
	shortToGround,
	openCircuit,
	timeout,
	ioError,
	hardwareError,
	notReady,
	invalidOutputNumber,
	busBusy,
	badResponse,
	unknownPort,
	notInitialised,
	unknownSensor,
	overOrUnderVoltage,
	badVref,
	badVssa,
	unknownError
};


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define T_SENSORS_COUNT 64
#define MAX_REDUCED_STRING_LENGTH 21
#define MAX_STRING_LENGTH 60
#define TEMPERATURE_READINGS_QUEUE_LENGTH(itemsPerSensor) 6 * itemsPerSensor
#define INITIALIZE_PERIFERIAL_QUEUE_LENGTH 4
#define THERMISTOR_NAME "thermistor"


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
DMA_HandleTypeDef hdma_adc1;

FDCAN_HandleTypeDef hfdcan1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim8;
TIM_HandleTypeDef htim15;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;

UART_HandleTypeDef huart2;

Thermistor* theremistors[adcBufferLength];

/* Definitions for defaultTask */

/* USER CODE BEGIN PV */
osThreadId_t sensorConfigurationHTaskHandler;
TaskHandle_t initializePeriferialTaskHendler;
const osThreadAttr_t temperatureSensorReadingTask_attributes = {
  .name = "TemperatureSensorReading",
  .stack_size = 128 * 4
};

const osThreadAttr_t temperatureSensorSendingTask_attributes = {
  .name = "TemperatureSensorSending",
  .stack_size = 128 * 8
};

FDCAN_RxHeaderTypeDef RxHeader;
FDCAN_TxHeaderTypeDef TxHeader;
uint8_t RxData[120];

QueueHandle_t xInitializePeriferialQueue;

uint32_t boardAddress;



map<string, uint32_t> pinNamesMap = {
		{pinNames[0], 0},
		{pinNames[1], 1},
		{pinNames[2], 2},
		{pinNames[3], 3},
		{pinNames[4], 4},
		{pinNames[5], 5}
};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC1_Init(void);
static void MX_FDCAN1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM6_Init(void);

/* USER CODE BEGIN PFP */
static void FDCAN1_StartWithFilters(void);
void StartTemperatureSensorReadingTask(void *argument);
void StartTemperatureSendingTask(void *argument);
void StartInitializePeriferialTask(void *argument);
void InitializeBoardAddress(uint32_t address);
void InitializeThermistors();
void InitializeConfiguration();
PwmConfig calculatePwmConfig(uint32_t pwmFreq, float dutyCyclePercent);
static void MX_TIM1_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr);
static void MX_TIM2_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr);
static void MX_TIM3_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr);
static void MX_TIM4_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr);
static void MX_TIM8_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr);
static void MX_TIM15_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr);
static void MX_TIM16_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr);
static void MX_TIM17_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void printHexArray(const uint8_t* arr, size_t size) {
	    for (size_t i = 0; i < size; ++i) {
	        printf("%02x ", arr[i]); // %02x ensures two-digit hex output with leading zeros
	    }
	    printf("\r\n");
	}

	void print_uint8_array_hex(const uint8_t *arr, size_t len) {
		printf("Can Data: ");
		char str[len + 1]; // +1 for null terminator
		   for (size_t i = 0; i < len; i++) {
			   str[i] = arr[i] > 32 ? (char)arr[i] : ' ';
		   }
		   str[len] = '\0'; // Null-terminate the string
		   printf("%s\r\n", str);
	   }

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_FDCAN1_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  HAL_ADC_Stop(&hadc1);
  HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
  HAL_ADC_Start_DMA(&hadc1, (uint32_t*)&adcValues, adcBufferLength);

  PwmConfig pwm1Cfg = calculatePwmConfig(12000, 86);
  MX_TIM1_Init(pwm1Cfg.prescaler, pwm1Cfg.arr, pwm1Cfg.ccr);

  InitializeConfiguration();


//  htim1.co

   /* USER CODE END 2 */

  while(1);
  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */

  xInitializePeriferialQueue = xQueueCreate(INITIALIZE_PERIFERIAL_QUEUE_LENGTH, sizeof(ThermistorConfiguration));


  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */

  sensorConfigurationHTaskHandler = osThreadNew(StartTemperatureSensorReadingTask, (void*)&adcValues, &temperatureSensorReadingTask_attributes);
  sensorConfigurationHTaskHandler = osThreadNew(StartTemperatureSendingTask, NULL, &temperatureSensorSendingTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  continue;
	  //printf("ADC Value: 1: %f, 2: %f, 3: %f, 4: %f\r\n", trm1.getTempCelsius(), trm2.getTempCelsius(), trm3.getTempCelsius(), trm4.getTempCelsius());
//	  printf("Ping...%u\r\n", counter++);
	  CanMessageSensorTemperatures tempRep;
	  tempRep.whichSensors = 0;
	  tempRep.whichSensors |= (uint64_t)1u << 54;
	  tempRep.temperatureReports[0].SetTemperature(22.642);
	  CanId canTemp;

	  canTemp.SetRequest(CanMessageType::sensorTemperaturesReport, boardAddress, CanId::BroadcastAddress);

		TxHeader.Identifier = canTemp.GetWholeId();
		TxHeader.IdType = FDCAN_EXTENDED_ID;
		TxHeader.TxFrameType = FDCAN_DATA_FRAME;
		TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
		TxHeader.FDFormat = FDCAN_FD_CAN;
		TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
		TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS; // FDCAN_STORE_TX_EVENTS;
	//					TxHeader.MessageMarker = 8;
		TxHeader.MessageMarker = 0;




		uint32_t dataLen = tempRep.GetActualDataLength(1);
		TxHeader.DataLength = dataLen;

//		printf("FDCAN Temperature report: ");
//		printHexArray((uint8_t*)&stdrepl, 60);

		if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t*)&tempRep) != HAL_OK) {
				/* Transmission request Error */
				Error_Handler();

		}

		printf("FDCAN: Src: %u, Dst: %u, MsgType: %u, isRequest: %u, isResponse: %u\r\n",
				canTemp.Src(),
				canTemp.Dst(),
				canTemp.MsgType(),
				canTemp.IsRequest(),
				canTemp.IsResponse());
				printHexArray((uint8_t*)&tempRep, 60);


	  HAL_Delay(1000);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 21;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_AnalogWDGConfTypeDef AnalogWDGConfig = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.GainCompensation = 0;
  hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.NbrOfConversion = 6;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = ENABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
  hadc1.Init.OversamplingMode = ENABLE;
  hadc1.Init.Oversampling.Ratio = ADC_OVERSAMPLING_RATIO_4;
  hadc1.Init.Oversampling.RightBitShift = ADC_RIGHTBITSHIFT_2;
  hadc1.Init.Oversampling.TriggeredMode = ADC_TRIGGEREDMODE_SINGLE_TRIGGER;
  hadc1.Init.Oversampling.OversamplingStopReset = ADC_REGOVERSAMPLING_CONTINUED_MODE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analog WatchDog 1
  */
  AnalogWDGConfig.WatchdogNumber = ADC_ANALOGWATCHDOG_1;
  AnalogWDGConfig.WatchdogMode = ADC_ANALOGWATCHDOG_ALL_REG;
  AnalogWDGConfig.ITMode = ENABLE;
  AnalogWDGConfig.HighThreshold = 4095;
  AnalogWDGConfig.LowThreshold = 0;
  AnalogWDGConfig.FilteringConfig = ADC_AWD_FILTERING_NONE;
  if (HAL_ADC_AnalogWDGConfig(&hadc1, &AnalogWDGConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_92CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_2;
  sConfig.Rank = ADC_REGULAR_RANK_2;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = ADC_REGULAR_RANK_3;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_4;
  sConfig.Rank = ADC_REGULAR_RANK_4;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_5;
  sConfig.Rank = ADC_REGULAR_RANK_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_6;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief FDCAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_FDCAN1_Init(void)
{

  /* USER CODE BEGIN FDCAN1_Init 0 */

  /* USER CODE END FDCAN1_Init 0 */

  /* USER CODE BEGIN FDCAN1_Init 1 */

  /* USER CODE END FDCAN1_Init 1 */
  hfdcan1.Instance = FDCAN1;
  hfdcan1.Init.ClockDivider = FDCAN_CLOCK_DIV1;
  hfdcan1.Init.FrameFormat = FDCAN_FRAME_FD_NO_BRS;
  hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
  hfdcan1.Init.AutoRetransmission = ENABLE;
  hfdcan1.Init.TransmitPause = DISABLE;
  hfdcan1.Init.ProtocolException = DISABLE;
  hfdcan1.Init.NominalPrescaler = 12;
  hfdcan1.Init.NominalSyncJumpWidth = 8;
  hfdcan1.Init.NominalTimeSeg1 = 11;
  hfdcan1.Init.NominalTimeSeg2 = 2;
  hfdcan1.Init.DataPrescaler = 1;
  hfdcan1.Init.DataSyncJumpWidth = 8;
  hfdcan1.Init.DataTimeSeg1 = 11;
  hfdcan1.Init.DataTimeSeg2 = 2;
  hfdcan1.Init.StdFiltersNbr = 0;
  hfdcan1.Init.ExtFiltersNbr = 0;
  hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
  if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN FDCAN1_Init 2 */

  /* USER CODE END FDCAN1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x20B21E5A;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = prescaler;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = arr;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
  HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = prescaler;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = arr;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = prescaler;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = arr;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = prescaler;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = arr;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim4) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim4, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */
  HAL_TIM_MspPostInit(&htim4);

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 2563;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65521;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

}

/**
  * @brief TIM8 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM8_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = prescaler;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = arr;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim8) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim8, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim8, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */

  /* USER CODE END TIM8_Init 2 */
  HAL_TIM_MspPostInit(&htim8);

}

/**
  * @brief TIM15 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM15_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr)
{

  /* USER CODE BEGIN TIM15_Init 0 */

  /* USER CODE END TIM15_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM15_Init 1 */

  /* USER CODE END TIM15_Init 1 */
  htim15.Instance = TIM15;
  htim15.Init.Prescaler = prescaler;
  htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim15.Init.Period = aar;
  htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim15.Init.RepetitionCounter = 0;
  htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_PWM_Init(&htim15) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim15, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim15, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM15_Init 2 */

  /* USER CODE END TIM15_Init 2 */
  HAL_TIM_MspPostInit(&htim15);

}

/**
  * @brief TIM16 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM16_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr)
{

  /* USER CODE BEGIN TIM16_Init 0 */

  /* USER CODE END TIM16_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM16_Init 1 */

  /* USER CODE END TIM16_Init 1 */
  htim16.Instance = TIM16;
  htim16.Init.Prescaler = prescaler;
  htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim16.Init.Period = arr;
  htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim16.Init.RepetitionCounter = 0;
  htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim16) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim16, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim16, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM16_Init 2 */

  /* USER CODE END TIM16_Init 2 */
  HAL_TIM_MspPostInit(&htim16);

}

/**
  * @brief TIM17 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM17_Init(uint32_t prescaler, uint32_t arr, uint32_t ccr)
{

  /* USER CODE BEGIN TIM17_Init 0 */

  /* USER CODE END TIM17_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM17_Init 1 */

  /* USER CODE END TIM17_Init 1 */
  htim17.Instance = TIM17;
  htim17.Init.Prescaler = prescaler;
  htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim17.Init.Period = arr;
  htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim17.Init.RepetitionCounter = 0;
  htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim17) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = ccr;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim17, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim17, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM17_Init 2 */

  /* USER CODE END TIM17_Init 2 */
  HAL_TIM_MspPostInit(&htim17);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMAMUX1_CLK_ENABLE();
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pins : TACH01_Pin TACH04_Pin TACH05_Pin */
  GPIO_InitStruct.Pin = TACH01_Pin|TACH04_Pin|TACH05_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : TACH00_Pin TACH07_Pin TACH02_Pin */
  GPIO_InitStruct.Pin = TACH00_Pin|TACH07_Pin|TACH02_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : TACH03_Pin TACH06_Pin */
  GPIO_InitStruct.Pin = TACH03_Pin|TACH06_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

typedef uint16_t CanRequestId;

// Enumeration to specify the result of attempting to process a GCode command


// Helper class to manage CAN message buffer pointers, to ensure they get released if an exception occurs

enum class GCodeResult : uint8_t
{
	notFinished,					// we haven't finished processing this command
	ok,								// we have finished processing this code in the current state, and if the GCodeState is 'normal' then we have finished it completely
	warning,						// the command succeeded but a warning was generated
	warningNotSupported,			// the command is not supported, but for this command we issue a warning not an error
	error,							// general error, the reason will be written to the associated reply buffer
	errorNotSupported,
	notSupportedInCurrentMode,
	stopped,						// we are halted because of an emergency stop
	badOrMissingParameter,
	remoteInternalError,			// only used if CAN expansion is supported
	m291Cancelled,
	// The following are only used of CAN expansion is supported
	noCanBuffer,					// we failed to allocate a CAN buffer to send a message to an expansion board
	canResponseTimeout				// timed out waiting for a response to a CAN message - the associated reply buffer may contain more info
};

constexpr ParamDescriptor M308NewParams[] =
{
	FLOAT_PARAM('T'),
	FLOAT_PARAM('B'),
	FLOAT_PARAM('C'),
	FLOAT_PARAM('R'),
	INT16_PARAM('L'),
	INT16_PARAM('H'),
	UINT8_PARAM('F'),
	UINT8_PARAM('S'),
	UINT8_PARAM('W'),
	CHAR_PARAM('K'),
	REDUCED_STRING_PARAM('Y'),
	REDUCED_STRING_PARAM('P'),
	FLOAT16_PARAM('U'),
	FLOAT16_PARAM('V'),
	END_PARAMS
};

GCodeResult ProcessM308(const CanMessageGeneric& msg, ThermistorConfiguration& config, const string reply) noexcept
{
	CanMessageGenericParser parser(msg, M308NewParams);

	if (parser.GetUintParam('S', config.sensorNumber))
	{
		if (config.sensorNumber < T_SENSORS_COUNT)
		{
			//char sensorPinName[MAX_REDUCED_STRING_LENGTH] = {0};
			string sensorType;

			// ToDo: cut pin name to 5 symbols

			bool isPFound = false;
			if (parser.GetStringParam('P', config.pinName)) {

				config.pinName[5] = 0;

				for (uint32_t i = 0; i < adcBufferLength; i++) {
					if (pinNames[i] == config.pinName) {
						isPFound = true;
						break;
					}
				}
			}

			bool isYFound = false;
			if (parser.GetStringParam('Y', sensorType)) {
				if (sensorType == THERMISTOR_NAME) { // ToDo: provide case insensitive comparison
					isYFound = true;

				}
				if (!isYFound) {
					//return GCodeResult::error;
				}
			}

			parser.GetFloatParam('T', config.tParam);
			parser.GetFloatParam('B', config.bParam);
			parser.GetFloatParam('C', config.cParam);
			parser.GetFloatParam('R', config.rParam);


//			const auto sensor = FindSensor(sensorNum);
//			if (sensor.IsNull())
//			{
//				reply.printf("Sensor %u does not exist", sensorNum);
//				return GCodeResult::error;
//			}
//			return sensor->Configure(parser, reply);
			return GCodeResult::ok;
		}
		else
		{
//			reply.copy("Sensor number out of range");
			return GCodeResult::ok;
		}
	}

//	reply.copy("Missing sensor number parameter");
	return GCodeResult::error;
}

//template<class T> T* SetupRequestMessage(CanRequestId rid, CanAddress src, CanAddress dest, CanMessageType msgType) noexcept {
//		id.SetRequest(msgType, src, dest);
//		dataLength = sizeof(T);
//		marker = 0;
//		extId = 1;
//		fdMode = 1;
//		useBrs = 0;
//		remote = 0;
//		reportInFifo = 0;
//		spare = 0;
//		T* rslt = reinterpret_cast<T*>(&msg);
//		rslt->SetRequestId(rid);
//		return rslt;
//}

extern "C" {
	int _write(int file, char *ptr, int len) {
		HAL_StatusTypeDef hstatus;

		if (file != STDOUT_FILENO && file != STDERR_FILENO) {
			errno = EBADF;
			return -1;
		}

		hstatus = HAL_UART_Transmit(&huart2, (uint8_t *)ptr, len, HAL_MAX_DELAY);
		if (hstatus == HAL_OK) {
			return len;
		} else {
			errno = EIO;
			return -1;
		}
	}

	void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {

		if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET) {

			/* Retrieve Rx messages from RX FIFO0 */
			if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) {
				Error_Handler();
			}

			CanMessageGeneric* data = reinterpret_cast<CanMessageGeneric*>(&RxData);
			CanId can;
			can.SetReceivedId(RxHeader.Identifier);

			if (false && can.MsgType() == CanMessageType::timeSync) {
				CanMessageTimeSync* timeSync = reinterpret_cast<CanMessageTimeSync*>(&RxData);
				printf("TimeSync: timeSent: %u, lastTimeSent: %u, lastTimeAcknowledgeDelay: %u, isPrinting: %u, zero: %u, realTime: %u\r\n",
						timeSync->timeSent,
						timeSync->lastTimeSent,
						timeSync->lastTimeAcknowledgeDelay,
						timeSync->isPrinting,
						timeSync->zero,
						timeSync->realTime);
			}

			if (false && (can.Dst() == 121 || can.Src() == 121) && can.MsgType() == CanMessageType::sensorTemperaturesReport) {
				printf("FDCAN: Src: %u, Dst: %u, MsgType: %u, isRequest: %u, isResponse: %u\r\n",
										can.Src(),
										can.Dst(),
										can.MsgType(),
										can.IsRequest(),
										can.IsResponse());
				printHexArray(data->data, 60);
			}

			if (can.Dst() == boardAddress && can.MsgType() == CanMessageType::m308New) {

				const string reply = "";
				ThermistorConfiguration thermistorConfig;
				GCodeResult parceResult = ProcessM308(*data, thermistorConfig, reply);

				BaseType_t xHigherPriorityTaskWoken = pdFALSE;
				xQueueSendFromISR(xInitializePeriferialQueue, &thermistorConfig, &xHigherPriorityTaskWoken);

				printf("FDCAN: Src: %u, Dst: %u, MsgType: %u, isRequest: %u, isResponse: %u\r\n",
						can.Src(),
						can.Dst(),
						can.MsgType(),
						can.IsRequest(),
						can.IsResponse());

				if (true) {
					CanId can2;

					can2.SetResponse(CanMessageType::standardReply, boardAddress, 0);

					TxHeader.Identifier = can2.GetWholeId();
					TxHeader.IdType = FDCAN_EXTENDED_ID;
					TxHeader.TxFrameType = FDCAN_DATA_FRAME;
					TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
					TxHeader.FDFormat = FDCAN_FD_CAN;
					TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
					TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS; // FDCAN_STORE_TX_EVENTS;
					TxHeader.MessageMarker = 0;

					CanMessageStandardReply stdrepl;
					stdrepl.SetRequestId(data->requestId);
					stdrepl.resultCode = (uint32_t)parceResult;
					stdrepl.fragmentNumber = 0;
					stdrepl.moreFollows = 0;


					uint32_t dataLen = stdrepl.GetActualDataLength(0);
					TxHeader.DataLength = dataLen;

					printf("FDCAN Response: ");
					//printHexArray((uint8_t*)&stdrepl, 60);

					if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &TxHeader, (uint8_t*)&stdrepl) != HAL_OK) {
							/* Transmission request Error */
							Error_Handler();

					}
				}
			}
	  }
	}
}

static void FDCAN1_StartWithFilters() {
//	FDCAN_FilterTypeDef sFilterConfig;
//	sFilterConfig.IdType = FDCAN_STANDARD_ID;
//	sFilterConfig.FilterIndex = 0;
//	sFilterConfig.FilterType = FDCAN_FILTER_RANGE;
//	sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
//	sFilterConfig.FilterID1 = 0;
//	sFilterConfig.FilterID2 = 0; //0x1FFFFFFF;
//	if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) {
//		/* Filter configuration Error */
//		printf("[CAN] Unable to configure!\n");
//	}

	if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) {
		/* Start Error */
		printf("[CAN] Unable to start!\n");
	}

	if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) {
		/* Notification Error */
		printf("[CAN] Unable to activate the CAN interrupt!\n");
	}

	HAL_NVIC_SetPriority(FDCAN1_IT0_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(FDCAN1_IT0_IRQn);
}

void StartTemperatureSensorReadingTask(void *argument)
{
	FDCAN1_StartWithFilters();

	ThermistorConfiguration thermistorConfig;
    uint32_t currentSensorMeasurements = 0;
    const TickType_t xTicksToWait = pdMS_TO_TICKS(100);

  for(;;)
  {
	  if (xQueueReceive(xInitializePeriferialQueue, &thermistorConfig, xTicksToWait) == pdPASS) {
		  if (pinNamesMap.contains(thermistorConfig.pinName)) {
			  const uint32_t thermistorIndex = pinNamesMap[thermistorConfig.pinName];
			  Thermistor* thermistor = theremistors[thermistorIndex];

			  thermistor->setBettaParameterValue(thermistorConfig.bParam);
			  thermistor->setCCoefficientValue(thermistorConfig.cParam);
			  thermistor->setSeriesResistorValue(thermistorConfig.rParam);

			  thermistor->setSensorNumberValue(thermistorConfig.sensorNumber);
		  }
	  }


	  Thermistor* thermistor = (Thermistor*)theremistors[currentSensorMeasurements];
	  if (thermistor->isInitialized) {
		  thermistor->updateTemperature();
	  }

	  currentSensorMeasurements++;
	  if (currentSensorMeasurements >= adcBufferLength) {
			  currentSensorMeasurements = 0;
	  }

	  osDelay(1);
  }
  /* USER CODE END 5 */
}

void StartTemperatureSendingTask(void *argument)
{
	FDCAN_TxHeaderTypeDef txBroadcastHeader;

	txBroadcastHeader.IdType = FDCAN_EXTENDED_ID;
	txBroadcastHeader.TxFrameType = FDCAN_DATA_FRAME;
	txBroadcastHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
	txBroadcastHeader.FDFormat = FDCAN_FD_CAN;
	txBroadcastHeader.BitRateSwitch = FDCAN_BRS_OFF;
	txBroadcastHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
	txBroadcastHeader.MessageMarker = 0;

  for(;;) {


	  CanMessageSensorTemperatures tempBroadcast;
	  tempBroadcast.whichSensors = 0;

	  uint32_t initializedSensorsCount = 0;
	  for (uint32_t i = 0; i < adcBufferLength; i++) {
		  if (!theremistors[i]->isInitialized) {
			  continue;
		  }

		  const uint8_t semsorNumber = theremistors[i]->getSensorNumberValue();
		  const float temperature = (float)theremistors[i]->getLastKnownTemperatureC();
		  tempBroadcast.whichSensors |= (uint64_t)1u << semsorNumber;
		  tempBroadcast.temperatureReports[initializedSensorsCount].SetTemperature(temperature);
		  tempBroadcast.temperatureReports[initializedSensorsCount].errorCode = TemperatureError::ok;

		  initializedSensorsCount++;
	  }

	  if (initializedSensorsCount > 0) {
		  CanId canId;
		  canId.SetRequest(CanMessageType::sensorTemperaturesReport, boardAddress, CanId::BroadcastAddress);


		  txBroadcastHeader.Identifier = canId.GetWholeId();
		  txBroadcastHeader.DataLength = tempBroadcast.GetActualDataLength(1) + 2;

		  if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &txBroadcastHeader, (uint8_t*)&tempBroadcast) != HAL_OK) {
			  Error_Handler();
		  }

	  }
	  osDelay(500);
  }

}

void InitializeBoardAddress(uint32_t address) {
	boardAddress = address;
}

HAL_StatusTypeDef readFromEEPROM(uint16_t address, uint8_t* buf, uint16_t length) {
	HAL_StatusTypeDef status = HAL_I2C_Mem_Read(
			&hi2c1,
			EEPROM_DEVICE_ADDRESS,
			address,
			EEPROM_MEM_SIZE,
			buf,
			length,
			EEPROM_WRITE_READ_TIMEOUT
		);
	HAL_Delay(100);
	return status;
}

HAL_StatusTypeDef writeToEEPROM(uint16_t address, uint8_t* buf, uint16_t length) {
	HAL_StatusTypeDef status = HAL_I2C_Mem_Write(
		  &hi2c1,
		  EEPROM_DEVICE_ADDRESS,
		  address,
		  EEPROM_MEM_SIZE,
		  buf,
		  length,
		  EEPROM_WRITE_READ_TIMEOUT
	  );
	HAL_Delay(1000);

	return status;
}

void InitializeConfiguration() {
	BoardConfiguration configuration;
	const uint32_t configSize = sizeof(configuration);
	if (readFromEEPROM(EEPROM_CONFIGURATION_SIGNATURE_ADDRESS, (uint8_t*)&configuration, configSize) != HAL_OK) {
		Error_Handler();
	}

	if (memcmp(EEPROM_CONFIGURATION_SIGNATURE, &configuration.boardSignature, EEPROM_CONFIGURATION_SIGNATURE_SIZE) != 0) {
		memcpy(&configuration.boardSignature, EEPROM_CONFIGURATION_SIGNATURE, EEPROM_CONFIGURATION_SIGNATURE_SIZE);
		configuration.configurationState = SavedConfigurationState::providedSignatureAndBoardAddress;
		configuration.boardAddress = BOARD_DEFAULT_ADDRESS;
		// Write signature

		uint32_t bytesLeftToWrite = configSize;
		uint8_t data[configSize];
		memcpy(data, &configuration, configSize);

		for (uint32_t i = 0; i < configSize; i += 64) {
			uint32_t bytesToWrite = bytesLeftToWrite > 64 ? 64 : bytesLeftToWrite;
			bytesLeftToWrite -= 64;

			uint16_t toAddress = EEPROM_CONFIGURATION_SIGNATURE_ADDRESS + i;
			if (writeToEEPROM(toAddress, &data[i], bytesToWrite) != HAL_OK) {
				Error_Handler();
			}
		}
	}


	switch (configuration.configurationState) {
		case SavedConfigurationState::emptyEEPROM:
			// Fatal, we should not be there
			Error_Handler();
			break;
		case SavedConfigurationState::providedSignatureAndBoardAddress:
 			InitializeBoardAddress(configuration.boardAddress);
			break;
		default:
			Error_Handler();
			break;
	}

	while(1) {
		HAL_Delay(10);
	}

///WriteData = 0x11;

//	HAL_I2C_Mem_Write(&hi2c1, EEPROM_DEVICE_ADDRESS, MemAddress, MemAddSize, &WriteData, Size, EEPROM_WRITE_READ_TIMEOUT);
//	HAL_Delay(10);
//	HAL_I2C_Mem_Read(&hi2c1, EEPROM_DEVICE_ADDRESS, MemAddress, MemAddSize, &ReadData, Size, EEPROM_WRITE_READ_TIMEOUT);

}

void InitializeThermistors() {
	for (uint32_t i = 0; i < adcBufferLength; i++) {
		volatile uint32_t *adcValue = &adcValues[i];
		Thermistor* theremistor = new Thermistor(
				adcValue,
				pinNames[i]
			);
		theremistors[i] = theremistor;
	}
}

// Function to calculate Prescaler, ARR, and CCR based on specific rules
PwmConfig calculatePwmConfig(uint32_t pwmFreq, float dutyCyclePercent) {
    PwmConfig config = {0, 0, 0, false, 0, 0.0f};

//    // --- Input Validation ---
//    if (dutyCyclePercent < 0.0f || dutyCyclePercent > 100.0f) {
//        std::cerr << "Error: Duty cycle must be between 0 and 100." << std::endl;
//        return config;
//    }
//    if (pwmFreq == 0) {
//        std::cerr << "Error: PWM frequency cannot be zero." << std::endl;
//        return config;
//    }

    // --- Determine Prescaler based on your rule ---
    uint32_t prescalerVal; // This will be 0 or 1, which always fits in uint16_t
    if (pwmFreq >= 3000) {
        prescalerVal = 0; // Prescaler + 1 = 1 (No division)
    } else if (pwmFreq >= 1300 && pwmFreq < 3000) { // desired_pwm_freq < 3000 Hz
    	prescalerVal = 1; // Prescaler + 1 = 2 (Divide by 2)
    } else if (pwmFreq >= 860 && pwmFreq < 1300) { // desired_pwm_freq < 3000 Hz
    	prescalerVal = 2; // Prescaler + 1 = 2 (Divide by 2)
    } else {
    	// We want to achieve this F_PWM with a 16-bit ARR (max 65535).
		// ARR = (F_CLK_TIM_INPUT / ((prescaler_val + 1) * F_PWM)) - 1
		// We need (prescaler_val + 1) >= F_CLK_TIM_INPUT / (F_PWM * (ARR_max + 1))
		// So, calculate the minimum required prescaler_plus_1
		float minPrescalerPlus1Float = (1.0f * F_CLK_TIM_INPUT) / (pwmFreq * 65536.0f);

		// Round up to the next integer to ensure ARR fits and frequency is not lower
		prescalerVal = static_cast<uint32_t>(std::ceil(minPrescalerPlus1Float)) - 1;

		// Ensure calculated prescaler does not exceed 16-bit limit
//		if (prescaler_val > 65535) {
//			std::cerr << "Error: Calculated prescaler (" << prescaler_val
//					  << ") exceeds 16-bit limit. Cannot achieve desired frequency with 16-bit timer." << std::endl;
//			config.success = false;
//			return config;
//		}
    }

    // --- Calculate ARR (Auto-Reload Register) ---
    // (ARR + 1) = F_CLK_TIM_INPUT / ((prescalerVal + 1) * F_PWM)
    // Note: Use 1.0f * F_CLK_TIM_INPUT to force float division early for better precision
    float periodTotalTicksFloat = (1.0f * F_CLK_TIM_INPUT) / ((prescalerVal + 1) * pwmFreq);

    // Check if the calculated period is too small (meaning desired_pwm_freq is too high)
//    if (period_total_ticks_float < 1.0f) {
//        std::cerr << "Error: Desired PWM frequency (" << pwmFreq
//                  << " Hz) is too high for this clock and prescaler. Minimum ARR+1 is 1." << std::endl;
//        return config;
//    }

    uint32_t calculatedArrTemp = static_cast<uint32_t>(std::round(periodTotalTicksFloat)) - 1;

    // --- Check if ARR is within 16-bit limits (0 to 65535) ---
//    if (calculated_arr_temp > 65535 || calculatedArrTemp == 0xFFFFFFFF) { // 0xFFFFFFFF for unsigned underflow
//        std::cerr << "Error: Calculated ARR (" << calculatedArrTemp
//                  << ") exceeds 16-bit limit or is too small (e.g., negative after conversion)." << std::endl;
//        std::cerr << "Consider reducing PWM frequency. Max theoretical ARR is 65535." << std::endl;
//        return config;
//    }

    // Store the valid calculated parameters
    config.prescaler = static_cast<uint16_t>(prescalerVal);
    config.arr = static_cast<uint16_t>(calculatedArrTemp);

    // --- Calculate CCR (Capture/Compare Register) ---
    // CCR = round((Desired Duty Cycle / 100.0) * (ARR + 1))
    config.ccr = static_cast<uint16_t>(std::round((dutyCyclePercent / 100.0f) * (config.arr + 1)));

    // Safety check: ensure CCR does not exceed ARR + 1 (for 100% duty cycle)
    if (config.ccr > (config.arr + 1)) {
        config.ccr = config.arr + 1;
    }

    // --- Calculate Actual Freq and Duty (for verification) ---
    config.actualFreq = F_CLK_TIM_INPUT / ((config.prescaler + 1) * (config.arr + 1));
    config.actualDuty = (static_cast<float>(config.ccr) / (config.arr + 1)) * 100.0f;

    config.success = true;
    return config;
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM7 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM7)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
