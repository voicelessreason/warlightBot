/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
// Warlight AI Bot            //
// Author: Arlen Strausman    //
// Added 01/08/15             //
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

This is an AI bot designed to play Warlight (an online adaptation of the game Risk).
There are six classes. The bots main logic (strategy) is implemented within the "Bot.cpp" file.

The main strategy involves a function which weights the desirability of each move against the 
feasibility of that move being successful. This function is responsible for deciding where to allocate 
troops and which attacks to make. 

To prevent idle troops, a BFS determines a move that will send excess to the "front lines" the quickest.
