/*Author: Tornike Tchabukiani
 * For ATLAS EXPERIMENT
 * 19.05.2023 *
 */

/* -------------------------------------------Includes ---------------------------------------------------------*/
#include "main.h"
#include "stdio.h"

I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;
UART_HandleTypeDef huart2;


uint8_t Cmd_End[3] = {0xFF,0xFF,0xFF};  // command end sequence for Nextion
uint8_t Rx_Data[5]; //data from display
int16_t reading;
unsigned char ADSwrite[6];
float voltage[4]; //stores raw data from ADC(ADS1115 - CH0,CH1,CH2,CH3)
float current[4]; // stores current values from hall-effect based current sensors
const float voltageConv = 6.114 / 32768.0; //converts raw data to actual voltage values

#define sideA_VoltageADC_Address 0x49 //U4 IC on the PCB
#define sideA_CurrentADC_Address 0x48 //U3 IC on the PCB
#define sideB_VoltageADC_Address 0x4A //U2 IC on the PCB
#define sideB_CurrentADC_Address 0x4B //U1 IC on the PCB

#define currentSensorVoltage 4.80 //change accordingly
float CurrentSensorOffSet = currentSensorVoltage/2;

/*-------------------------------------------function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C2_Init(void);

void NEXTION_SendString (char *ID, float string); //Sends Data to Display
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);// UARD Tx ISR
void ADS1115(int ADS1115_ADDRESS);//reading data from ADCs

//Main function
int main(void)
{

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART2_UART_Init();
  MX_I2C2_Init();

  //Turn all 8 relayis to ON State
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 1);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 1);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 1);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);

  //UART Rx Interrupt
  HAL_UART_Receive_IT(&huart2, Rx_Data, 2);


  while (1)
  {
	  //Reading all ADCs and Displaing data to 5 inch Nextion display
	  ADS1115(sideA_CurrentADC_Address);
	  ADS1115(sideA_VoltageADC_Address);
	  ADS1115(sideB_CurrentADC_Address);
	  ADS1115(sideB_VoltageADC_Address);
  }

}


void NEXTION_SendString (char *ID, float string)
{
	char buf[20];
	int len = sprintf (buf, "%s.txt=\"%.2f\"", ID, string);
	HAL_UART_Transmit(&huart2, (uint8_t *)buf, len, 200);
	HAL_UART_Transmit(&huart2, Cmd_End, 3, 100);
}

//UART RX ISR
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	  HAL_UART_Receive_IT(huart, Rx_Data, 2);

	  if (Rx_Data[0] == 0x02) // if the dual state button is pressed
	  {
		  // Get the value
		  if (Rx_Data[1] == 0) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 1);
		  if (Rx_Data[1] == 1) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, 0);
	  } else if (Rx_Data[0] == 0x03) // if the dual state button is pressed
	  {
		  // Get the value
		  if (Rx_Data[1] == 0) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 1);
		  if (Rx_Data[1] == 1) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_11, 0);
	  }else if (Rx_Data[0] == 0x04) // if the dual state button is pressed
	  {
		  // Get the value
		  if (Rx_Data[1] == 0) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 1);
		  if (Rx_Data[1] == 1) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, 0);
	  }else if (Rx_Data[0] == 0x05) // if the dual state button is pressed
	  {
		  // Get the value
		  if (Rx_Data[1] == 0) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
		  if (Rx_Data[1] == 1) HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 0);
	  }else if (Rx_Data[0] == 0x06) // if the dual state button is pressed
	  {
		  // Get the value
		  if (Rx_Data[1] == 0) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 1);
		  if (Rx_Data[1] == 1) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, 0);
	  }else if (Rx_Data[0] == 0x07) // if the dual state button is pressed
	  {
		  // Get the value
		  if (Rx_Data[1] == 0) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 1);
		  if (Rx_Data[1] == 1) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, 0);
	  }else if (Rx_Data[0] == 0x08) // if the dual state button is pressed
	  {
		  // Get the value
		  if (Rx_Data[1] == 0) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1);
		  if (Rx_Data[1] == 1) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0);
	  }else if (Rx_Data[0] == 0x09) // if the dual state button is pressed
	  {
		  // Get the value
		  if (Rx_Data[1] == 0) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 1);
		  if (Rx_Data[1] == 1) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, 0);
	  }
}

//reading and display data from four ADS1115s
void ADS1115(int ADS1115_ADDRESS){

	for(int i=0; i< 4; i++){
				ADSwrite[0] = 0x01;

				switch(i){
					case(0):
						ADSwrite[1] = 0xC1;
					break;
					case(1):
						ADSwrite[1] = 0xD1;
					break;
					case(2):
						ADSwrite[1] = 0xE1;
					break;
					case(3):
						ADSwrite[1] = 0xF1;
					break;
				}
				ADSwrite[2] = 0x83;

				HAL_I2C_Master_Transmit(&hi2c1, ADS1115_ADDRESS << 1, ADSwrite, 3, 100);
				ADSwrite[0] = 0x00;
				HAL_I2C_Master_Transmit(&hi2c1, ADS1115_ADDRESS << 1 , ADSwrite, 1 ,100);
				HAL_Delay(20);

				HAL_I2C_Master_Receive(&hi2c1, ADS1115_ADDRESS <<1, ADSwrite, 2, 100);
				reading = (ADSwrite[0] << 8 | ADSwrite[1] );
				if(reading < 0) {
					reading = 0;
				}
				voltage[i] = reading * voltageConv;
				if(ADS1115_ADDRESS == 0x48 ){
					current[i]=(voltage[i]-CurrentSensorOffSet)/0.185;
				}else if (ADS1115_ADDRESS == 0x4B){
					current[0]=(voltage[0]-(CurrentSensorOffSet+0.02))/0.185;
					current[1]=(voltage[1]-(CurrentSensorOffSet+0.02))/0.185;
					current[2]=(voltage[2]-(CurrentSensorOffSet+0.01))/0.185;
					current[3]=(voltage[3]-(CurrentSensorOffSet+0.03))/0.185;
				}
			}
	if(ADS1115_ADDRESS == 0x48){
		if(current[0]<=0.2 ){
			current[0]=0;
			NEXTION_SendString("t4", current[0]);
		}else{
			NEXTION_SendString("t4", current[0]);
		}
		if(current[1]<=0.2 ){
			current[1]=0;
			NEXTION_SendString("t16", current[1]);
			NEXTION_SendString("t5", current[1]);

		}else{
			NEXTION_SendString("t16", current[1]);
			NEXTION_SendString("t5", current[1]);
		}
		if(current[2]<=0.2 ){
			current[2]=0;
			NEXTION_SendString("t6", current[2]);

		}else{
			NEXTION_SendString("t6", current[2]);
		}
		if(current[3]<=0.2 ){
			current[3]=0;
			NEXTION_SendString("t7", current[3]);

		}else{
			NEXTION_SendString("t7", current[3]);
		}

	} else if (ADS1115_ADDRESS == 0x49){
		NEXTION_SendString("t0", voltage[0]*3.0);
		NEXTION_SendString("t1", voltage[1]*3.0);
		NEXTION_SendString("t2", voltage[2]*3.0);
		NEXTION_SendString("t3", voltage[3]*3.0);
	}else if (ADS1115_ADDRESS == 0x4A){
		NEXTION_SendString("t8", voltage[0]*3.0);
		NEXTION_SendString("t9", voltage[1]*3.0);
		NEXTION_SendString("t10", voltage[2]*3.0);
		NEXTION_SendString("t11", voltage[3]*3.0);
	}else if(ADS1115_ADDRESS == 0x4B){
		if(current[0]<=0.2 ){
			current[0]=0;
			NEXTION_SendString("t12", current[0]);
		}else{
			NEXTION_SendString("t12", current[0]);
		}
		if(current[1]<=0.2 ){

			current[1]=0;
			NEXTION_SendString("t13", current[1]);
		}else{
			NEXTION_SendString("t13", current[1]);
		}
		if(current[2]<=0.2 ){
			current[2]=0;
			NEXTION_SendString("t14", current[2]);
		}else{
			NEXTION_SendString("t14", current[2]);
		}
		if(current[3]<=0.2 ){

			current[3]=0;
			NEXTION_SendString("t15", current[3]);

		}else{
			NEXTION_SendString("t15", current[3]);
		}
	}
}



/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
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
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 60;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_ENABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

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
  huart2.Init.BaudRate = 9600;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 PB14 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA8 PA11 PA12 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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
