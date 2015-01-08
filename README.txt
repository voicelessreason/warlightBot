/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// Warlight AI Bot            //
// Author: Arlen Strausman    //
// Added 01/08/15             //
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

This is an AI bot designed to play Warlight (an online adaptation of the game Risk). It is currently 
playing ranked games on theaigames.com. This was my senior project at Goucher College.

My bot is built upon a starter bot template designed by MasterJos (Parser.cpp, Region.cpp, SuperRegion.cpp, Main.cpp)
There are six classes. The bots main logic (strategy) is implemented within the "Bot.cpp" file, 
and the main function is located in main.cpp. 

The main strategy involves a function which weighs the desirability of each move against the 
feasibility of that move being successful. This function is responsible for deciding where to allocate 
troops and which attacks to make. 

To prevent idle troops, a BFS determines a move that will send excess to the "front lines" the quickest.
