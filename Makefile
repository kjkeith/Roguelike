dungeon: dungeon.o priority.o Monster.o ParsedMonster.o MonsterParser.o Item.o ParsedItem.o ItemParser.o 
	g++ dungeon.o priority.o Monster.o ParsedMonster.o MonsterParser.o Item.o ParsedItem.o ItemParser.o -o dungeon -lncurses
clean:
	rm -f dungeon *.o
