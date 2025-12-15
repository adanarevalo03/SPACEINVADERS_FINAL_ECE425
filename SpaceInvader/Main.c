#include "SpaceInvader.h"
#include "Test.h"


// Compile game only
#define DISABLE_TESTS


#define TEST_DISPLAY
//#define TEST_ADC
//#define TEST_DAC
//#define TEST_GPTM1
//#define RUN_ALL_TESTS



int main() {
	
	#ifdef DISABLE_TESTS // Run game
	
	GameInit();
	while (1) {
		GameLoop();
	}
	
	#else // Enter test mode
		
		#if defined(TEST_DISPLAY) || defined(RUN_ALL_TESTS)
			Test_Display_Init();
			Test_Display();
		#endif
	
		#if defined(TEST_ADC) || defined(RUN_ALL_TESTS)
			Test_ADC_Init();
			while(1)
				Test_ADC();
		#endif
	
		#if defined(TEST_DAC) || defined(RUN_ALL_TESTS)
			Test_DAC_Init();
			while(1)
				Test_DAC();
		#endif
		
		#if defined(TEST_GPTM1) || defined(RUN_ALL_TESTS)
			Test_GPTM1_Init();
			while (1);
		#endif
	
	#endif
}