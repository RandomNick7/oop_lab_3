//TODO !Important: Empty class methods are meant to be interactions. They haven't been added yet as they aren't required, but should be added ASAP
//TODO: Spawn the animals with a specific delay (call Noah's Arc function)
//TODO: Make terrain altitude, temperature influence animal movement directly
//TODO: Malloc everything and pass by reference to functions to avoid making the stack explode (especially with larger worlds). It will make helper_fun redundant, too
//TODO: Come up with a clever way to add all drawn layers on top of each other
//TODO: Make rain cool down the ground, evaporation heat it up slightly, add some sort of temperature shift or "wind"

#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <algorithm>

using namespace std;

#define W 30		//World Width
#define H 15		//World Height (2D)

int timer = 0;

#include "class.h"
#include "helper_fun.h"
#include "world_fun.h"

int main(){
	srand(time(0));
	Tile world[H][W];
	Cloud sky[H][W];
	Plant grass[H][W];
	Fungi shrooms[H][W];
	vector<Tree*> trees;
	vector<Corpse*> bodies;
	vector<Herbivore*> a_herb;
	
	smoothen(world);
	//noahs_arc(&world[0],&a_herb);
	
	while(true){
		form_weather(&world[0],sky);
		water_transfer(world);
		grow_grass(&world[0],&grass[0],&trees);
		grow_trees(&world[0],&sky[0],&trees);
		grow_shrooms(&world[0],&shrooms[0]);
		//animal_behavior(&world[0],&a_herb,&trees,&bodies);	//Under construction!
		show_world(world,sky,grass,shrooms);
		timer++;
		//Update 20 times per second
		this_thread::sleep_for(chrono::milliseconds(50));
	}
}
