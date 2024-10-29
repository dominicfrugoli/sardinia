// ================================
// COMPANY: Feni Design
// AUTHOR: Dominic Frugoli
// CREATE DATE: 10/04/2024
// PROJECT: Fall 2024 Independent Project
// HEADER FILE FOR USE WITH: "sardinia.cpp"
// SPECIAL NOTES: Code is meant for Electrosmith Daisy microcontroller
// ================================


using namespace daisy;
using namespace daisy::seed;


// |Pin Declarations|------------------------------------------------------------------------------------------


#define PIN_LCD_RS D28 // LCD: pin 8
#define PIN_LCD_EN D27 // LCD: pin 9
#define PIN_LCD_D4 D26 // LCD: D4
#define PIN_LCD_D5 D25 // LCD: D5
#define PIN_LCD_D6 D24 // LCD: D6
#define PIN_LCD_D7 D23 // LCD: D7

#define PLAY_MODE 0
#define RECORD_MODE 1


// |Mode Setup|------------------------------------------------------------------------------------------


enum Mode // Just add knobs before last NUM_ADC_CHANNELS as needed
{
	record,
	play
};


// |Object Instantiations|------------------------------------------------------------------------------------------


LcdHD44780 lcd;
enum Mode mode = record;


// |Function Declarations|------------------------------------------------------------------------------------------


void InitLCD()
{
    LcdHD44780::Config lcd_config;
	lcd_config.cursor_on = false;
	lcd_config.cursor_blink = false;
	lcd_config.rs = PIN_LCD_RS;
	lcd_config.en = PIN_LCD_EN;
	lcd_config.d4 = PIN_LCD_D4;
	lcd_config.d5 = PIN_LCD_D5;
	lcd_config.d6 = PIN_LCD_D6;
	lcd_config.d7 = PIN_LCD_D7;
	lcd.Init(lcd_config);
	lcd.Clear();
}

void PrintMenu(float currenteffect, bool effectstates[])
{
	//Print Mode Readout
	lcd.SetCursor(0, 0);
	if(mode == record)
	{
		lcd.Print("Record Mode     ");
	}
	else if(mode == play)
	{
		lcd.Print("Play Mode       ");
	}

	// Print Effect Readout
	lcd.SetCursor(1, 0);
	if(currenteffect == 0)
	{
		lcd.Print("Overdrive:   ");
		if(effectstates[0])
		{
			lcd.Print(" On");
		}
		else
		{
			lcd.Print("Off");
		}
	}
	else if(currenteffect == 1)
	{
		lcd.Print("Chorus:      ");
		if(effectstates[1])
		{
			lcd.Print(" On");
		}
		else
		{
			lcd.Print("Off");
		}
	}
	else if(currenteffect == 2)
	{
		lcd.Print("Low-Pass:    ");
		if(effectstates[2])
		{
			lcd.Print(" On");
		}
		else
		{
			lcd.Print("Off");
		}
	}

	// For Testing Values
	/*
	lcd.SetCursor(1, 0);
	lcd.PrintInt(testValFloat);
	lcd.SetCursor(1, 8);
	lcd.PrintInt(testValInt);
	*/
}

void UpdateMode(int menuSelect)
{
	if(menuSelect == 0)
	{
		mode = record;
	}
	else if(menuSelect == 1)
	{
		mode = play;
	}
}






