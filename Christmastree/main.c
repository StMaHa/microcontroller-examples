/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body of project Christmas tree for STM32F103
  ******************************************************************************
  */
#include <stdbool.h>
#include <stdlib.h>
#include "main.h"

#define STAR_LED01   0x00800000
#define STAR_LED02   0x00400000
#define STAR_LED03   0x00200000
#define STAR_LED04   0x00100000
#define STAR_LED05   0x00080000
#define STAR_LED06   0x00040000
#define STAR_LED07   0x00020000
//				     0x00010000 not used, should stay 0 (due to ULN2003 7bit Transistor Array)
#define STAR_LED08   0x00008000
#define STAR_LED09   0x00004000
#define STAR_LED10   0x00002000
#define STAR_LED11   0x00001000
#define STAR_LED12   0x00000800
#define STAR_LED13   0x00000400
#define STAR_LED14   0x00000200
//				     0x00000100 not used, should stay 0 (due to ULN2003 7bit Transistor Array)
#define STAR_LED15   0x00000080
#define STAR_LED16   0x00000040
#define STAR_LED17   0x00000020
#define STAR_LED18   0x00000010
#define STAR_LED19   0x00000008
#define STAR_LED20   0x00000004
//				     0x00000002 not used, should stay 0 (Star has only 20 LED's)
//				     0x00000001 not used, should stay 0 (due to ULN2003 7bit Transistor Array)
#define MAX_BIT_COUNT_STAR   24  // 3 x 74HC595 (8bit shift register)

#define TREE_LED01   0x00008000
#define TREE_LED02   0x00004000
#define TREE_LED03   0x00002000
#define TREE_LED04   0x00001000
#define TREE_LED05   0x00000800
#define TREE_LED06   0x00000400
#define TREE_LED07   0x00000200
//				     0x00000100 not used, should stay 0 (due to ULN2003 7bit Transistor Array)
#define TREE_LED08   0x00000080
#define TREE_LED09   0x00000040
#define TREE_LED10   0x00000020
#define TREE_LED11   0x00000010
//				     0x00000008 not used, should stay 0 (Tree has only 11 LED's)
//				     0x00000004 not used, should stay 0 (Tree has only 11 LED's)
//				     0x00000002 not used, should stay 0 (Tree has only 11 LED's)
//				     0x00000001 not used, should stay 0 (due to ULN2003 7bit Transistor Array)
#define MAX_BIT_COUNT_TREE   16  // 2 x 74HC595 (8bit shift register)

#define MAX_BIT_COUNT        MAX_BIT_COUNT_TREE + MAX_BIT_COUNT_STAR

#define MAX_SCENARIO_ELEMENTS   22  // Max. numbers of lights + state all off + state all on
#define MAX_SCENARIOS			10  // Max. numbers of scenarios

static const int STAR_LED_LIST[] = { STAR_LED01, STAR_LED02, STAR_LED03, STAR_LED04, STAR_LED05, STAR_LED06, STAR_LED07, STAR_LED08, STAR_LED09, STAR_LED10,
		                             STAR_LED11, STAR_LED12, STAR_LED13, STAR_LED14, STAR_LED15, STAR_LED16, STAR_LED17, STAR_LED18, STAR_LED19, STAR_LED20 };
static const int STAR_LED_LEFT_LIST[] = { STAR_LED11, STAR_LED12, STAR_LED13, STAR_LED14, STAR_LED15, STAR_LED16, STAR_LED17, STAR_LED18, STAR_LED19, STAR_LED20 };
static const int STAR_LED_RIGHT_LIST[] = { STAR_LED01, STAR_LED02, STAR_LED03, STAR_LED04, STAR_LED05, STAR_LED06, STAR_LED07, STAR_LED08, STAR_LED09, STAR_LED10 };

static const int TREE_LED_LIST[] = { TREE_LED01, TREE_LED02, TREE_LED03, TREE_LED04, TREE_LED05, TREE_LED06, TREE_LED07, TREE_LED08, TREE_LED09, TREE_LED10, TREE_LED11 };
static const int TREE_LED_MIDDLE_LIST[] = { TREE_LED01, TREE_LED04, TREE_LED07, TREE_LED08, TREE_LED10 };
static const int TREE_LED_LEFT_LIST[] = { TREE_LED02, TREE_LED05, TREE_LED09 };
static const int TREE_LED_RIGHT_LIST[] = { TREE_LED03, TREE_LED06, TREE_LED11 };
static const int TREE_LED_ALLAROUND_LIST[] = { TREE_LED01, TREE_LED03, TREE_LED06, TREE_LED11, TREE_LED10, TREE_LED09, TREE_LED05, TREE_LED02};

void SystemClock_Config(void);
void CheckForNextScenario(void);
static void MX_GPIO_Init(void);

typedef struct tLIGHT_SCENARIO
{
	unsigned int unMaxIterations;
	unsigned int unIterationNo;
	unsigned int unElementNo;
	unsigned int unMaxElements;
	unsigned int unScenario[MAX_SCENARIO_ELEMENTS];
}LIGHT_SCENARIO;

typedef struct tLIGHT_MODULE
{
	unsigned int unMaxBitCount;
	unsigned int unScenarioNo;
	unsigned int unMaxScenarios;
	LIGHT_SCENARIO scenarios[MAX_SCENARIOS];
}LIGHT_MODULE;

typedef struct tSHIFT_REGISTER
{
	bool fLatchClock;
	unsigned int unMaxBitCount;
	bool fBits[MAX_BIT_COUNT];
}SHIFT_REGISTER;

SHIFT_REGISTER shiftRegister;
LIGHT_MODULE lightStar;
LIGHT_MODULE lightTree;

void Scenario_PrepareStar(void)
{
	unsigned int unScenarioNo = 0;
	unsigned int unLedCount = sizeof(STAR_LED_LIST)/sizeof(STAR_LED_LIST[0]);
	lightStar.unMaxBitCount = MAX_BIT_COUNT_STAR;

	// Scenario: Blinking Star
	lightStar.scenarios[unScenarioNo].unMaxIterations = 5;
	lightStar.scenarios[unScenarioNo].unMaxElements = 2;
	lightStar.scenarios[unScenarioNo].unElementNo = 0;
	for(int i = 0; i < unLedCount; i++)
		lightStar.scenarios[unScenarioNo].unScenario[0] |= STAR_LED_LIST[i];
	lightStar.scenarios[unScenarioNo].unScenario[1] = 0;
	unScenarioNo++;

	// Scenario: Running Light
	lightStar.scenarios[unScenarioNo].unMaxIterations = 1;
	lightStar.scenarios[unScenarioNo].unMaxElements = unLedCount;
	for(int i = 0; i < lightStar.scenarios[unScenarioNo].unMaxElements; i++)
		lightStar.scenarios[unScenarioNo].unScenario[i] = STAR_LED_LIST[i];
	unScenarioNo++;

	// Scenario: Switching Lights
	lightStar.scenarios[unScenarioNo].unMaxIterations = 7;
	lightStar.scenarios[unScenarioNo].unMaxElements = 2;
	lightStar.scenarios[unScenarioNo].unElementNo = 0;
	for(int i = 0; i < unLedCount; i = i + 2)
		lightStar.scenarios[unScenarioNo].unScenario[0] |= STAR_LED_LIST[i];
	for(int i = 1; i < unLedCount; i = i + 2)
		lightStar.scenarios[unScenarioNo].unScenario[1] |= STAR_LED_LIST[i];
	unScenarioNo++;

	// Scenario: Running Two Lights
	lightStar.scenarios[unScenarioNo].unMaxIterations = 2;
	lightStar.scenarios[unScenarioNo].unMaxElements = unLedCount / 2;
	lightStar.scenarios[unScenarioNo].unElementNo = 0;
	for(int i = 0; i < lightStar.scenarios[unScenarioNo].unMaxElements; i++)
		lightStar.scenarios[unScenarioNo].unScenario[i] = STAR_LED_LIST[i] | STAR_LED_LIST[i + lightStar.scenarios[unScenarioNo].unMaxElements];
	unScenarioNo++;

	// Scenario: Increase/decrease Lights
	unScenarioNo++;
	lightStar.scenarios[unScenarioNo].unMaxIterations = 5;
	unsigned int unHalfElementCount = unLedCount / 2;
    // 1. All lights off
	int i = 0;
	lightStar.scenarios[unScenarioNo].unScenario[i] = 0;
	// 2. - 11. Increase Lights
	i++;
	for(int n = 0; n < unHalfElementCount; i++, n++)
		lightStar.scenarios[unScenarioNo].unScenario[i] = lightStar.scenarios[unScenarioNo].unScenario[i - 1] |
														  STAR_LED_LIST[n] |
														  STAR_LED_LIST[n + unHalfElementCount];
	// State all LED off + 10 LEDs switched on = 11 states
    // 12. All lights on
	for(int n = 0; n < unLedCount; n++)
		lightStar.scenarios[unScenarioNo].unScenario[i] |= STAR_LED_LIST[n];
	// 13. - 22. Decrease Lights
	i++;
	for(int n = 0; n < unHalfElementCount; n++, i++)
		lightStar.scenarios[unScenarioNo].unScenario[i] = lightStar.scenarios[unScenarioNo].unScenario[i - 1] &
														  ~STAR_LED_LIST[n] &
														  ~STAR_LED_LIST[n + unHalfElementCount];
	lightStar.scenarios[unScenarioNo].unMaxElements = i;
	unScenarioNo++;

	lightStar.unMaxScenarios = unScenarioNo;  // unScenarioNo starting at 0
}

void Scenario_PrepareTree(void)
{
	unsigned int unScenarioNo = 0;
	unsigned int unLedCount = sizeof(TREE_LED_LIST)/sizeof(TREE_LED_LIST[0]);
	unsigned int unLedMiddleCount = sizeof(TREE_LED_MIDDLE_LIST)/sizeof(TREE_LED_MIDDLE_LIST[0]);
	unsigned int unLedLeftCount = sizeof(TREE_LED_LEFT_LIST)/sizeof(TREE_LED_LEFT_LIST[0]);
	unsigned int unLedRightCount = sizeof(TREE_LED_RIGHT_LIST)/sizeof(TREE_LED_RIGHT_LIST[0]);
	unsigned int unLedAllAroundCount = sizeof(TREE_LED_ALLAROUND_LIST)/sizeof(TREE_LED_ALLAROUND_LIST[0]);
	lightTree.unMaxBitCount = MAX_BIT_COUNT_TREE;

	// Scenario: Blinking Star
	lightTree.scenarios[unScenarioNo].unMaxIterations = 5;
	lightTree.scenarios[unScenarioNo].unMaxElements = 2;
	lightTree.scenarios[unScenarioNo].unElementNo = 0;
	for(int i = 0; i < unLedCount; i++)
		lightStar.scenarios[unScenarioNo].unScenario[0] |= TREE_LED_LIST[i];
	lightTree.scenarios[unScenarioNo].unScenario[1] = 0;
	unScenarioNo++;

	// Scenario: Running Light
	lightTree.scenarios[unScenarioNo].unMaxIterations = 1;
	lightTree.scenarios[unScenarioNo].unMaxElements = unLedCount;
	for(int i = 0; i < lightTree.scenarios[unScenarioNo].unMaxElements; i++)
		lightTree.scenarios[unScenarioNo].unScenario[i] = TREE_LED_LIST[i];
	unScenarioNo++;

	// Scenario: Switching Lights
	lightTree.scenarios[unScenarioNo].unMaxIterations = 7;
	lightTree.scenarios[unScenarioNo].unMaxElements = 2;
	lightTree.scenarios[unScenarioNo].unElementNo = 0;
	for(int i = 0; i < unLedMiddleCount; i++)
		lightTree.scenarios[unScenarioNo].unScenario[0] |= TREE_LED_MIDDLE_LIST[i];
	for(int i = 0; i < unLedLeftCount; i++)
		lightTree.scenarios[unScenarioNo].unScenario[1] |= TREE_LED_LEFT_LIST[i];
	for(int i = 0; i < unLedRightCount; i++)
		lightTree.scenarios[unScenarioNo].unScenario[1] |= TREE_LED_RIGHT_LIST[i];
	unScenarioNo++;

	// Scenario: Running Light Around
	lightTree.scenarios[unScenarioNo].unMaxIterations = 2;
	int i = 0;
	for(; i < unLedAllAroundCount; i++)
		lightTree.scenarios[unScenarioNo].unScenario[i] = TREE_LED_ALLAROUND_LIST[i] | TREE_LED04 | TREE_LED07 | TREE_LED08;
	lightTree.scenarios[unScenarioNo].unMaxElements = i;
	unScenarioNo++;
	
	// Scenario: Increase/decrease Lights
	unScenarioNo++;
	lightTree.scenarios[unScenarioNo].unMaxIterations = 5;
    i = 0;
	// Increase Lights
	for(int n = 0; n < unLedCount; n++)
	{
		lightTree.scenarios[unScenarioNo].unScenario[i] = lightTree.scenarios[unScenarioNo].unScenario[i - 1] | TREE_LED_LIST[n];
		i++;
	}
    // All lights on
	for(int n = 0; n < unLedCount; n++)
		lightTree.scenarios[unScenarioNo].unScenario[i] |= TREE_LED_LIST[n];
	// Decrease Lights
	i++;
	for(int n = unLedCount - 1; n >= 0; n--)
	{
		lightTree.scenarios[unScenarioNo].unScenario[i] = lightTree.scenarios[unScenarioNo].unScenario[i - 1] & ~TREE_LED_LIST[n];
		i++;
	}
	lightTree.scenarios[unScenarioNo].unMaxElements = i;
	unScenarioNo++;

    // All lights on
	for(int n = 0; n < unLedCount; n++)
		lightTree.scenarios[unScenarioNo].unScenario[n] |= TREE_LED_LIST[n];
	unScenarioNo++;

	lightTree.unMaxScenarios = unScenarioNo;
}

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	SystemCoreClockUpdate();

	// Configure system tick relative to latch clock (led blinking frequency)
	SysTick_Config(SystemCoreClock / 6UL);

	Scenario_PrepareStar();
	Scenario_PrepareTree();

	CheckForNextScenario();
	while (1){};
}

void ShiftBits(void)
{
	// Set latch pin to low
	HAL_GPIO_WritePin(GPIOB, LATCH_CLK_Pin, GPIO_PIN_RESET);
	for(unsigned int unBitNumber = 0; unBitNumber <= shiftRegister.unMaxBitCount; unBitNumber++)
	{
		// Send data bit
		GPIO_PinState ledState = (GPIO_PinState)shiftRegister.fBits[unBitNumber];
		HAL_GPIO_WritePin(GPIOB, SER_DATA_Pin, ledState);
		HAL_GPIO_WritePin(GPIOB, SER_DATA_Pin, ledState);

		// Repeat state to get a longer state
		HAL_GPIO_WritePin(GPIOB, SHIFT_CLK_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOB, SHIFT_CLK_Pin, GPIO_PIN_SET);

		// Set data pin to low
		HAL_GPIO_WritePin(GPIOB, SER_DATA_Pin, GPIO_PIN_RESET);

		// Set clock pin low
		HAL_GPIO_WritePin(GPIOB, SHIFT_CLK_Pin, GPIO_PIN_RESET);
	}
	// Set latch pin to high will activate the output of all shift register
	HAL_GPIO_WritePin(GPIOB, LATCH_CLK_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, LATCH_CLK_Pin, GPIO_PIN_SET);
	// Set clock pin to high to read the latch pin
	HAL_GPIO_WritePin(GPIOB, SHIFT_CLK_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SHIFT_CLK_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, SHIFT_CLK_Pin, GPIO_PIN_RESET);
}

/**
 *
 */
void SetNextScenario(LIGHT_MODULE* pScenario, unsigned int* punData)
{
	// Get actual scenario
	unsigned int unScenarioNo = pScenario->unScenarioNo;
	// Get actual element of scenario
	unsigned int unElementNo = pScenario->scenarios[unScenarioNo].unElementNo;
	// Get data of element
	*punData = pScenario->scenarios[unScenarioNo].unScenario[unElementNo];
	// Prepare for next iteration
	pScenario->scenarios[unScenarioNo].unElementNo++;
	if (pScenario->scenarios[unScenarioNo].unElementNo >= pScenario->scenarios[unScenarioNo].unMaxElements)
	{
		pScenario->scenarios[unScenarioNo].unElementNo = 0;
		pScenario->scenarios[unScenarioNo].unIterationNo++;
	}
	if (pScenario->scenarios[unScenarioNo].unIterationNo >= pScenario->scenarios[unScenarioNo].unMaxIterations)
	{
		pScenario->scenarios[unScenarioNo].unIterationNo = 0;
		pScenario->unScenarioNo++;
	}
	if (pScenario->unScenarioNo >= pScenario->unMaxScenarios)
		pScenario->unScenarioNo = 0;
}

/**
 * Convert and merge 32bit bit mask (LED state) values of all LED modules into one array
 * of the virtual shift register
 */
void CheckForNextScenario(void)
{
	unsigned int unData;
	unsigned int unBitIndex = 0;

	SetNextScenario(&lightStar, &unData);
	for(unsigned int n = 0; n < lightStar.unMaxBitCount; n++)
	{
		shiftRegister.fBits[unBitIndex] = (unData >> n) & 0x00000001;
		unBitIndex++;
	}

	SetNextScenario(&lightTree, &unData);
	for(unsigned int n = 0; n < lightTree.unMaxBitCount; n++)
	{
		shiftRegister.fBits[unBitIndex] = (unData >> n) & 0x00000001;
		unBitIndex++;
	}

	shiftRegister.unMaxBitCount = lightTree.unMaxBitCount + lightStar.unMaxBitCount;
}

void SysTick_Handler(void)
{
	ShiftBits();
	CheckForNextScenario();
	shiftRegister.fLatchClock = !shiftRegister.fLatchClock;
	HAL_GPIO_WritePin(GPIOC, BUILDIN_LED_Pin, shiftRegister.fLatchClock);
}
