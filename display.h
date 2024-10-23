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


// |Mode Setup|------------------------------------------------------------------------------------------


enum Mode // Just add knobs before last NUM_ADC_CHANNELS as needed
{
	record,
	edit,
	keyboard,
	sampler // Enum allows for auto tracking the amount
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

void PrintMenu()
{
	if(mode == record)
	{
		lcd.Print("Record Mode     ");
	}
	else if(mode == edit)
	{
		lcd.Print("Edit Mode       ");
	}
	else if(mode == keyboard)
	{
		lcd.Print("Keyboard Mode   ");
	}
	else if(mode == sampler)
	{
		lcd.Print("Sampler Mode    ");
	}
}

void UpdateMenu(int menuSelect)
{
	if(menuSelect == 0)
	{
		mode = record;
	}
	else if(menuSelect == 1)
	{
		mode = edit;
	}
	else if(menuSelect == 2)
	{
		mode = keyboard;
	}
	else if(menuSelect == 3)
	{
		mode = sampler;
	}

}



