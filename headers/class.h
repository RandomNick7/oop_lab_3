//TODO: Fill up the Animal classes with working methods - *Herbivore -> Carnivore -> Detritivore -> Omnivore

//ALL elements have nothing in common! The "Root" class remains empty.
class Entity{};

//Represents non-living things, such as Tiles and Clouds
class Environment: public Entity{
	public:
		float water = 0;
};

// The building block of the world. Things can exist on them
class Tile: public Environment{
	public:
		int altit = rand()%(3000+1)-500;		//[-500,2500]
		float water = rand()%(6000+1)/10+300;	//[300,900]
		float temp = rand()%(420+1)/10-11;		//[-11,31]
		int entity = 0;
};

// Entity used for water storage and transportation across the world
class Cloud: public Environment{
	public:
		//When over a tile, rain down some amount of water (max 25%);
		void rain(Tile* ground){
			//The higher the altitude and the colder it is, the more it should rain
			float qty = water/4*(ground->altit+500)/3000*(35-ground->temp)/50;
			if(qty > 10 && ground->water+qty<=1000){
				water-=qty;
				ground->water+=qty;
			}
		}
};

//Anything that can contain energy is regarded to as "living"
class Life: public Entity{
	protected:
		int energy;
};

//Includes living things that can grow anywhere and are numerically contained to a single tile, such as Plants and Fungi
class Grower: public Life{
	public:
		int num=0;
		void absorb(Tile* ground){
			if(ground->water-num>0 && energy<10){
				ground->water-=num/5+0.5;
				energy+=num;
			}else{
				if(energy>0){
					energy--;
				}else if(num>0){
					num--;
					ground->water+=3;
					energy++;
				}
			}
		}
		
		virtual void multiply(Tile* ground) = 0;
};

//Anything with health and which can die is regarded as a "Creature". This includes Trees and Animals
class Creature: public Life{
	protected:
		int id;
		int index;
	public:
		int health;
		int x;
		int y;
};

//A grown up plant. Generates clouds through transpiration and food for animals
class Tree: public Creature{
	public:
		int food;
		
		Tree(int i, int j, int s){
			id = 1;
			health = 50;
			energy = 20;
			index = s;
			x=j;
			y=i;
		};
		
		//If the tree has no health, remove it
		void death(Tile* ground, vector<Tree*>* trees){
			if(health == 0){
				ground->entity = 0;
				ground->water += 5+food;
				delete (*trees)[index];
				(*trees)[index] = nullptr;
				(*trees).erase(remove((*trees).begin(), (*trees).end(), nullptr), (*trees).end());
				//Reduce index of all subsequent elements by 1
				for(int i=index;i<(*trees).size();i++){
					(*trees)[i]->index--;
				}
			}
		}
		
		//Convert water to energy 
		void absorb(Tile* ground, vector<Tree*>* trees){
			if(ground->water-3>0 && energy<50){
				ground->water-=3;
				energy++;
			}else{
				if(energy>0){
					energy--;
				}else{
					health--;
					//TODO: Fix death-related segfault (Object removal in Vector fuck-uppening?)
					//FIX: Just store the objects themselves, not the pointers to them. You anyway pass the vector by reference...
					//death(ground,trees);
				}
			}
		}
		
		//Generate food that can be eaten by other animals
		void fruit(){
			if(energy>5 && food<5){
				energy-=3;
				food++;
			}
		}
		
		//Convert energy to potential water
		void transpirate(Cloud* sky){
			if(energy>0 && sky->water<1000){
				sky->water+=5;
				energy--;
			}
		}
};

//The primordial "living" entity. Uses water to grow into trees. Serves as a food source for some animals 
class Plant: public Grower{
	public:
		//Grow based on existing energy and waterity
		void multiply(Tile* ground){
			if(energy>5 && num<=10 && rand()%100<(ground->water-300)/100 && ground->entity==0){
				num++;
				energy-=3+num/5;
			}
		}
		
		//Evolve into a tree once a threshold has been reached
		void grow(Tile* ground, vector<Tree*>* trees, int i, int j){
			if(num>=10 && energy>=20 && ground->entity==0){
				num-= 10;
				energy-=20;
				ground->entity = 1;
				(*trees).push_back(new Tree(i,j,(*trees).size()));
			}
		}
};

//Warning! Methods of classes below are untested!

//An animal's remains. Stationary entity that acts as a limited-time food source
//Do NOT ask why a "Corpse" is part of "The Living".
class Corpse: public Life{
	private:
		int id = 2;
		int index;
		int duration;
	public:
		Corpse(Tile* ground, int n, int d, int e){
			index = n;
			duration = d;
			energy = e;
			ground->entity=id;
		}
		
		void decay(Tile* ground, vector<Corpse*>* bodies){
			duration--;
			if(duration == 0){
				//Remove this body from the vector storing them
				delete (*bodies)[index];
				(*bodies).erase((*bodies).begin()+index);
				ground->entity = 0;
			}
		}
};

//The Plant's evil twin. Grows in water spots and near corpses. Serves as a food source.
class Fungi: public Grower{
	public:
		void multiply(Tile* ground){
			if(energy>3+num && num<=5 && rand()%100<(ground->water-400)/100){
				num++;
				energy-=3+num;
			}
		}
};

//Further classes could have probably been made using polymorphism... They have been done as a sketch, as no interactions are allowed anyway.

class Animal: public Creature{
	protected:
		int attack;
		//Implementing A* would be overkill and would take a lot of time, so we're going with the budget "greedy" version instead
		//Here, when looking for a path, it will go in the shortest direction available
		//If the path straight ahead is occupied, it will look for free neighboring tiles to that destination
		void move(Tile map[H][W], int id, int* x, int* y, int dist_x, int dist_y){
			int x_new = *x;
			int y_new = *y;
			if(dist_x == 0){
				//Horizontal movement
				y_new = (dist_y/abs(dist_y)+*y)%H;
				if(map[y_new][x_new].entity!=0){
					//If tile is occupied, move to the right
					x_new = (x_new+1)%H;
				}
				if(map[y_new][x_new].entity!=0){
					//If that tile is occupied too, move to the left instead
					x_new = (x_new-2)%H;
				}
			}
			if(dist_y == 0){
				//Vertical movement
				x_new=(dist_x/abs(dist_x)+*x)%W;
				if(map[y_new][x_new].entity!=0){
					//If tile is occupied, move above it
					y_new = (y_new+1)%H;
				}
				if(map[y_new][x_new].entity!=0){
					//If that tile is occupied too, move below instead
					y_new = (y_new-2)%H;
				}
			}else{
				//Diagonal movement
				int sgn_x = dist_x/abs(dist_x);
				int sgn_y = dist_y/abs(dist_y);
				x_new = (sgn_x+*x)%W;
				y_new = (sgn_y+*y)%H;
				if(map[y_new][x_new].entity!=0){
					y_new = (y_new-sgn_y)%H;
				}
				if(map[y_new][x_new].entity!=0){
					y_new = (y_new+sgn_y)%H;
					x_new = (x_new+sgn_x)%W;
				}
			}
			if(map[y_new][x_new].entity!=0){
				//If all potential tiles are occupied, don't move
				x_new = *x;
				y_new = *y;
			}
			//Remove self from current tile
			map[*y][*x].entity = 0;
			//Place self on new tile
			*x = x_new;
			*y = y_new;
			map[*y][*x].entity = id;
		}
};

//Animals that consume flora
class Herbivore: public Animal{
	public:
		Herbivore(int pos_x, int pos_y, int s){
			id = 3;
			energy=14;
			health=30;
			attack=1;
			x = pos_x;
			y = pos_y;
			index = s;
		}
		//TODO: Allow target() to loop around the map (add extra stuff at the if() statements)
		void target(Tile map[H][W],vector<Tree*>* trees){
			int target_found = 0;
			if(energy<=15){
				for(int i=0;i<(*trees).size();i++){
					//If there is a tree within 1 tile away...
					Tree* t = (*trees)[i];
					if(abs(x - t->x <= 1) && abs(y - t->y <= 1)){
						//Get energy, damage the tree
						t->health-=attack;
						t->death(&map[t->x][t->y],trees);
						energy+=3;
						target_found = 1;
						break;
					}
				}
				if(!target_found){
					for(int i=0;i<(*trees).size();i++){
						//If there is a tree within 3 tiles away...
						Tree* t = (*trees)[i];
						if(abs(x - t->x <= 3) && abs(y - t->y <= 3)){
							//Move towards it
							move(map,id,&x,&y,t->x,t->y);
							target_found = 1;
							break;
						}
					}
				}
				if(!target_found){
					//Just move randomly if didn't find anything
					move(map,id,&x,&y,rand()%W-x,rand()%H-y);
				}
			}else if(energy>15){
				breed();
				//Set breeding flag to "on"
				//Look for another animal of the same kind with the flag on within range
				//If found, move towards them
				//Else, move randomly
			}
		}

		void death(Tile* ground, vector<Herbivore*>* a_herb, vector<Corpse*>* bodies){
			if(health == 0){
				delete (*a_herb)[index];
				(*a_herb).erase((*a_herb).begin()+index);
				(*bodies).push_back(new Corpse(ground,(*bodies).size(),30,energy+10));
			}
		}
		
		void hunger(Tile* ground, vector<Herbivore*>* a_herb, vector<Corpse*>* bodies){
			if(energy>0){
				energy--;
			}else if(health>0){
				health--;
			}else{
				death(ground, a_herb, bodies);
			}
		}
				
		//TODO: Make 2 animals generate another animal given enough energy
		void breed(){};
		
};

//Classes below are under construction!
//Note to self: Use Class Herbivores as an example for making the methods

//Animals that consumes other animals
class Carnivore: public Animal{
	public:
		Carnivore(int pos_x, int pos_y){
			id = 5;
			energy=12;
			health=20;
			attack=4;
			x = pos_x;
			y = pos_y;
		}
		
		void target(Tile map[H][W],vector<Herbivore*>* a_herb){
			if(energy<=15){
			}else if(energy>15){
			}
		}
		
		void breed(){};
		
		void death(Tile* ground, vector<Carnivore*>* a_carn, vector<Corpse*>* bodies){
			if(health == 0){
				delete (*a_carn)[index];
				(*a_carn).erase((*a_carn).begin()+index);
				(*bodies).push_back(new Corpse(ground,(*bodies).size(),30,energy+10));
			}
		}
		
		void hunger(Tile* ground, vector<Carnivore*>* a_carn, vector<Corpse*>* bodies){
			if(energy>0){
				energy--;
			}else if(health>0){
				health--;
			}else{
				death(ground, a_carn, bodies);
			}
		}
		
};


//Animals that primarily consume dead organic material
class Detritivore: public Animal{
	public:
		Detritivore(int pos_x, int pos_y){
			id = 3;
			energy = 11;
			health = 10;
			attack = 2;
			x = pos_x;
			y = pos_y;
		}
	
		void target(){
			if(energy<=15){
			}else if(energy>15){
			}
		}
		
		void breed(){};
		
		void death(Tile* ground, vector<Detritivore*>* a_detr, vector<Corpse*>* bodies){
			if(health == 0){
				delete (*a_detr)[index];
				(*a_detr).erase((*a_detr).begin()+index);
				(*bodies).push_back(new Corpse(ground,(*bodies).size(),30,energy+10));
			}
		}
		
		void hunger(Tile* ground, vector<Detritivore*>* a_detr, vector<Corpse*>* bodies){
			if(energy>0){
				energy--;
			}else if(health>0){
				health--;
			}else{
				death(ground, a_detr, bodies);
			}
		}
};

//Animals that consumes other animals
class Omnivore: public Animal{
	public:
		Omnivore(int pos_x, int pos_y){
			id = 6;
			energy=13;
			health=25;
			attack=3;
			x = pos_x;
			y = pos_y;
		}
		
		void target(){
			if(energy<=15){
			}else if(energy>15){
			}
		}
		
		void breed(){};
		
		void death(Tile* ground, vector<Omnivore*>* a_omni, vector<Corpse*>* bodies){
			if(health == 0){
				delete (*a_omni)[index];
				(*a_omni).erase((*a_omni).begin()+index);
				(*bodies).push_back(new Corpse(ground,(*bodies).size(),30,energy+10));
			}
		}
		
		void hunger(Tile* ground, vector<Omnivore*>* a_omni, vector<Corpse*>* bodies){
			if(energy>0){
				energy--;
			}else if(health>0){
				health--;
			}else{
				death(ground, a_omni, bodies);
			}
		}
};
