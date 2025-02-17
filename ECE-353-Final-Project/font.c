// 
//  Font data for Microsoft Sans Serif 8pt
// 
#include "font.h"

// Character bitmaps for Microsoft Sans Serif 8pt
const uint8_t fontBitMap[] = 
{
	// @0 '0' (5 pixels wide)
	0x70, //  ### 
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x70, //  ### 
	0x00, //      
	0x00, //      

	// @11 '1' (3 pixels wide)
	0x20, //   #
	0xE0, // ###
	0x20, //   #
	0x20, //   #
	0x20, //   #
	0x20, //   #
	0x20, //   #
	0x20, //   #
	0x20, //   #
	0x00, //    
	0x00, //    

	// @22 '2' (5 pixels wide)
	0x70, //  ### 
	0x88, // #   #
	0x08, //     #
	0x08, //     #
	0x10, //    # 
	0x20, //   #  
	0x40, //  #   
	0x80, // #    
	0xF8, // #####
	0x00, //      
	0x00, //      

	// @33 '3' (5 pixels wide)
	0x70, //  ### 
	0x88, // #   #
	0x08, //     #
	0x08, //     #
	0x30, //   ## 
	0x08, //     #
	0x08, //     #
	0x88, // #   #
	0x70, //  ### 
	0x00, //      
	0x00, //      

	// @44 '4' (5 pixels wide)
	0x10, //    # 
	0x30, //   ## 
	0x30, //   ## 
	0x50, //  # # 
	0x50, //  # # 
	0x90, // #  # 
	0xF8, // #####
	0x10, //    # 
	0x10, //    # 
	0x00, //      
	0x00, //      

	// @55 '5' (5 pixels wide)
	0xF8, // #####
	0x80, // #    
	0x80, // #    
	0xF0, // #### 
	0x88, // #   #
	0x08, //     #
	0x08, //     #
	0x88, // #   #
	0x70, //  ### 
	0x00, //      
	0x00, //      

	// @66 '6' (5 pixels wide)
	0x70, //  ### 
	0x88, // #   #
	0x80, // #    
	0x80, // #    
	0xF0, // #### 
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x70, //  ### 
	0x00, //      
	0x00, //      

	// @77 '7' (5 pixels wide)
	0xF8, // #####
	0x08, //     #
	0x10, //    # 
	0x10, //    # 
	0x20, //   #  
	0x20, //   #  
	0x40, //  #   
	0x40, //  #   
	0x40, //  #   
	0x00, //      
	0x00, //      

	// @88 '8' (5 pixels wide)
	0x70, //  ### 
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x70, //  ### 
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x70, //  ### 
	0x00, //      
	0x00, //      

	// @99 '9' (5 pixels wide)
	0x70, //  ### 
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x78, //  ####
	0x08, //     #
	0x08, //     #
	0x88, // #   #
	0x70, //  ### 
	0x00, //      
	0x00, //      

	// @110 'H' (6 pixels wide)
	0x84, // #    #
	0x84, // #    #
	0x84, // #    #
	0x84, // #    #
	0xFC, // ######
	0x84, // #    #
	0x84, // #    #
	0x84, // #    #
	0x84, // #    #
	0x00, //       
	0x00, //       

	// @121 'S' (5 pixels wide)
	0x70, //  ### 
	0x88, // #   #
	0x80, // #    
	0x80, // #    
	0x70, //  ### 
	0x08, //     #
	0x08, //     #
	0x88, // #   #
	0x70, //  ### 
	0x00, //      
	0x00, //      

	// @132 'c' (5 pixels wide)
	0x00, //      
	0x00, //      
	0x00, //      
	0x70, //  ### 
	0x88, // #   #
	0x80, // #    
	0x80, // #    
	0x88, // #   #
	0x70, //  ### 
	0x00, //      
	0x00, //      

	// @143 'e' (5 pixels wide)
	0x00, //      
	0x00, //      
	0x00, //      
	0x70, //  ### 
	0x88, // #   #
	0xF8, // #####
	0x80, // #    
	0x88, // #   #
	0x70, //  ### 
	0x00, //      
	0x00, //      

	// @154 'g' (5 pixels wide)
	0x00, //      
	0x00, //      
	0x00, //      
	0x78, //  ####
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x78, //  ####
	0x08, //     #
	0xF0, // #### 

	// @165 'h' (5 pixels wide)
	0x80, // #    
	0x80, // #    
	0x80, // #    
	0xB0, // # ## 
	0xC8, // ##  #
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x00, //      
	0x00, //      

	// @176 'i' (1 pixels wide)
	0x80, // #
	0x00, //  
	0x00, //  
	0x80, // #
	0x80, // #
	0x80, // #
	0x80, // #
	0x80, // #
	0x80, // #
	0x00, //  
	0x00, //  

	// @187 'o' (5 pixels wide)
	0x00, //      
	0x00, //      
	0x00, //      
	0x70, //  ### 
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x88, // #   #
	0x70, //  ### 
	0x00, //      
	0x00, //      

	// @198 'r' (2 pixels wide)
	0x00, //   
	0x00, //   
	0x00, //   
	0xC0, // ##
	0x80, // # 
	0x80, // # 
	0x80, // # 
	0x80, // # 
	0x80, // # 
	0x00, //   
	0x00, //   
};

// Character descriptors for Microsoft Sans Serif 8pt
// { [Char width in bits], [Offset into microsoftSansSerif_8ptCharBitmaps in bytes] }
const FONT_CHAR_INFO font[] = 
{
	{5, 0}, 		// 0 
	{3, 11}, 		// 1 
	{5, 22}, 		// 2 
	{5, 33}, 		// 3 
	{5, 44}, 		// 4 
	{5, 55}, 		// 5 
	{5, 66}, 		// 6 
	{5, 77}, 		// 7 
	{5, 88}, 		// 8 
	{5, 99}, 		// 9 
	{0, 0}, 		// : 
	{0, 0}, 		// ; 
	{0, 0}, 		// < 
	{0, 0}, 		// = 
	{0, 0}, 		// > 
	{0, 0}, 		// ? 
	{0, 0}, 		// @ 
	{0, 0}, 		// A 
	{0, 0}, 		// B 
	{0, 0}, 		// C 
	{0, 0}, 		// D 
	{0, 0}, 		// E 
	{0, 0}, 		// F 
	{0, 0}, 		// G 
	{6, 110}, 		// H 
	{0, 0}, 		// I 
	{0, 0}, 		// J 
	{0, 0}, 		// K 
	{0, 0}, 		// L 
	{0, 0}, 		// M 
	{0, 0}, 		// N 
	{0, 0}, 		// O 
	{0, 0}, 		// P 
	{0, 0}, 		// Q 
	{0, 0}, 		// R 
	{5, 121}, 		// S 
	{0, 0}, 		// T 
	{0, 0}, 		// U 
	{0, 0}, 		// V 
	{0, 0}, 		// W 
	{0, 0}, 		// X 
	{0, 0}, 		// Y 
	{0, 0}, 		// Z 
	{0, 0}, 		// [ 
	{0, 0}, 		// 
	{0, 0}, 		// ] 
	{0, 0}, 		// ^ 
	{0, 0}, 		// _ 
	{0, 0}, 		// ` 
	{0, 0}, 		// a 
	{0, 0}, 		// b 
	{5, 132}, 		// c 
	{0, 0}, 		// d 
	{5, 143}, 		// e 
	{0, 0}, 		// f 
	{5, 154}, 		// g 
	{5, 165}, 		// h 
	{1, 176}, 		// i 
	{0, 0}, 		// j 
	{0, 0}, 		// k 
	{0, 0}, 		// l 
	{0, 0}, 		// m 
	{0, 0}, 		// n 
	{5, 187}, 		// o 
	{0, 0}, 		// p 
	{0, 0}, 		// q 
	{2, 198}, 		// r 
};

// Font information for Microsoft Sans Serif 8pt
const FONT_INFO microsoftSansSerif_8ptFontInfo =
{
	2, //  Character height
	'H', //  Start character
	'r', //  End character
	2, //  Width, in pixels, of space character
	//microsoftSansSerif_8ptDescriptors, //  Character descriptor array
	//microsoftSansSerif_8ptBitmaps, //  Character bitmap array
};

