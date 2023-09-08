It's our final assignment! What did I do for my choose-my-own adventure? 
First off, I implemented the use of lights to expand the character's view in the game. 
Simply pick up a torch, equip it to your torch slot, and LIGHTS! CAMERA! ACTION!
You can also do two different types of ranged damage! If you have a ranged weapon, you can target and shoot a monster!

You, however, will need to add the ranged weapon to the object_description. AND THEN FIND ONE!
I recommend doing an item with 2+1d8 damage, is ranged, and has a rarity of 100. You'll see them everywhere. 
But don't worry if you don't have a ranged weapon, you can still cast the "Poison Ball" spell for 2d8 damage! Wow, magic is awesome!
Poison Ball is also a great option because it offers splash damage! All monsters surrounding the center target are hit as well!

You can look at a monster's description using 'L', which pulls up a cursor system. When you press 't' on a target, you can see that monster's description!
Press ',' to pick up items. 
You can press 'i' to pull up your inventory!
You can press 'e' to pull up equipped items! 
You can press 'm' to see a list of monsters, their status, and their location!
Press 'I' to get the chance to select and view item in inventory.
Press 'w' to add an item from inventory to equipment.

For debugging purposes:

Simply press 'f' to toggle the fog of war on and off.

You can now teleport as well! To do so press 'g', move your character, and press 't' to select that location.

You can also teleport to a random location by first pressing 'g', then 'r'.

To run the file(s):

1. Compile the program using the Makefile included in the tarball.

2.1. Simply call "./dungeon" in the console to create a dungeon without saving it.

2.2. To generate and save a new dungeon, call "./dungeon --save" in the console. (Previous issues with the max value of rooms still apply.)
 
2.3. To load a saved dungeon/file, call "./dungeon --load" in the console.

2.4. To load and resave a saved dungeon, call either "./dungeon --load --save" or "./dungeon --save --load" in the console.

2.5. Call "--nummon <number>" to start the dungeon with <number> monsters. If this argument is not used, their will be 1-10 monsters on the map.
