#include "TM4C123.h"                    // Device header
//#include "images.h"
// 
//  Image data for tank
// 

const uint_8 tankBitmaps =
{
	0x6E, 0xA1, 0x6D, //  ## ### # #    # ## ## #
	0xA3, 0xE7, 0x97, // # #   #####  ####  # ###
	0x59, 0x60, 0x7A, //  # ##  # ##      #### # 
	0xE7, 0xC3, 0xD5, // ###  #####    #### # # #
	0xBC, 0xA0, 0xF4, // # ####  # #     #### #  
	0xAD, 0x23, 0x19, // # # ## #  #   ##   ##  #
	0x41, 0x80, 0x82, //  #     ##       #     # 
	0x80, 0x00, 0x01, // #                      #
	0x40, 0x00, 0x02, //  #                    # 
	0x80, 0x00, 0x02, // #                     # 
	0xC0, 0x00, 0x03, // ##                    ##
	0x00, 0x00, 0x03, //                       ##
	0xC0, 0x00, 0x03, // ##                    ##
	0x00, 0x00, 0x03, //                       ##
	0x40, 0x00, 0x03, //  #                    ##
	0xC0, 0x00, 0x02, // ##                    # 
	0xC0, 0x00, 0x01, // ##                     #
	0x80, 0x00, 0x00, // #                       
	0xC0, 0x00, 0x03, // ##                    ##
	0x80, 0x00, 0x01, // #                      #
	0x80, 0x00, 0x01, // #                      #
	0x00, 0x00, 0x01, //                        #
	0xC0, 0x00, 0x01, // ##                     #
	0x40, 0x6D, 0x83, //  #       ## ## ##     ##
};

// Bitmap sizes for tank
const uint_8 tankWidthPages = 3;

const uint_8 tankHeightPixels = 24;

// 
//  Image data for tank_small
// 

const uint_8 tank_smallBitmaps =
{
	0xB6, 0x56, // # ## ##  # # ## 
	0x4A, 0x7D, //  #  # #  ##### #
	0xEC, 0x4E, // ### ##   #  ### 
	0xB4, 0x55, // # ## #   # # # #
	0x80, 0x21, // #         #    #
	0x80, 0x00, // #               
	0x00, 0x01, //                #
	0x80, 0x01, // #              #
	0x80, 0x01, // #              #
	0x00, 0x00, //                 
	0x00, 0x00, //                 
	0x80, 0x01, // #              #
	0x80, 0x01, // #              #
	0x80, 0x00, // #               
	0x00, 0x01, //                #
	0x80, 0x21, // #         #    #
};

// Bitmap sizes for tank_small
const uint_8 tank_smallWidthPages = 2;
const uint_8 tank_smallHeightPixels = 16;
