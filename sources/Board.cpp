#include<iostream>
#include<sstream>

#include<vector>
#include<algorithm>
#include<stdlib.h>

#include "../headers/Board.h"
#include "../headers/Tile.h"



/*

  Creates a board with a given amount of layers with given dimensions

  @param  (int) layers - The number of rendering layers needed
          (int) width - Number of rooms wide the dungeon is
          (int) height - Number of rooms high the dungeon is
*/
Board::Board(int layers, int width, int height){

  // Initiate Tile reference objeccts for flyweight pattern 
  empty_tile_ref_ = new EmptyTile();
  wall_tile_ref_ = new WallTile();
  player_tile_ = new PlayerTile();

  // Initiate Command objects for command pattern
  up_command_ = new UpCommand();
  right_command_ = new RightCommand();
  down_command_ = new DownCommand();
  left_command_ = new LeftCommand();

  layers_ = layers;

  width_ = width;
  height_ = height;

  width_res_ = width * 6 + 1;
  height_res_ = height * 6 + 1;
 
  std::vector< std::vector<Tile*> > blank_board;
  std::vector< std::vector< std::vector<Tile*> > > temp_board;
 
  for(int i = 0; i < height_res_; i++){
    std::vector<Tile*> blank_row(width_res_, wall_tile_ref_);
    blank_board.push_back( blank_row );
  }

  temp_board.push_back(blank_board);

  blank_board.clear();

  for(int i = 0; i < height_res_; i++){
    std::vector<Tile*> blank_row(width_res_, empty_tile_ref_);
    blank_board.push_back( blank_row );
  }

  for(int i = 1; i < layers; i++){
    temp_board.push_back(blank_board);
  }

  temp_board[1][3][3] = player_tile_;

  board_ = temp_board;

}

/*
    Creates the Dungeon using a modified maze generation algorithm

    The Dungeon is generated by first using a temporary dungeon which assumes that each room is only 1x1. It then generates a maze using thise. Once the maze is generated it's transfered to the actual dungeon and rooms are added
*/
void Board::GenerateDungeon(){
  
  /*
     -------------------------------------------
      Part 1 - Setup for simple Maze generation
     -------------------------------------------
  */

  std::vector< std::vector<TileType> > dungeon;

  int temp_width = width_*2+1;
  int temp_height = height_*2+1;

  for(int i = 0; i < temp_height; i++){
    std::vector<TileType> blank_row(temp_width, TileType::Empty);
    dungeon.push_back( blank_row );
  }

  /*
     -------------------------------------------
      Part 2 - Generate maze using radnom Kruskel's algorithm
     -------------------------------------------
  */

  // stores all walls seperating two empty cells(rooms)
  std::vector<Position> walls;

  // stores the cells that are connected in sets
  // every cell will start in its own set
  std::vector< std::vector<Position> > cells;

  // create empty cells and populate walls and cells vector
  for(int x = 0; x < temp_width; x++){
    for(int y = 0; y < temp_height; y++){

      if( (x%2 == 0) || (y%2 == 0) ){
        dungeon[y][x] = TileType::Wall;
        // check if wall is not a perimeter wall
        if( x != 0 && y != 0 && x+1 != temp_width && y+1 != temp_height && ((x%2 == 0) != (y%2 == 0))){
          Position wall_pos(x,y);
          walls.push_back(wall_pos);
        }
      }else{
        std::vector<Position> cell_set;
        Position cell_pos(x, y);
        cell_set.push_back(cell_pos);
        cells.push_back(cell_set);
      }

    } 
  }
  
  // shuffle the walls for randomness
  std::random_shuffle(walls.begin(), walls.end());

  std::cout << std::endl;

  // while we still ahve walls to check
  while(walls.size()){

	  Position cur = walls.back();
	
	  bool side = true;
	  if(cur.y_%2 == 0){
	    side = false;
	  } 
	
	  Position cell1;
	  Position cell2;
	
	  // get the cells that the current wall seperate
	  if(side){
	    cell1.x_ = cur.x_ - 1;
	    cell1.y_ = cur.y_;
	
	    cell2.x_ = cur.x_ + 1;
	    cell2.y_ = cur.y_;
	  }else{
	    cell1.x_ = cur.x_;
	    cell1.y_ = cur.y_ - 1;
	
	    cell2.x_ = cur.x_;
	    cell2.y_ = cur.y_ + 1;
	  } 

    int cell1_place;
    int cell2_place;

	  for(unsigned int i = 0; i < cells.size(); i++){
	    if( std::find(cells[i].begin(), cells[i].end(), cell1) != cells[i].end() ){
	      cell1_place = i;
	    } 
	    if( std::find(cells[i].begin(), cells[i].end(), cell2) != cells[i].end() ){
	      cell2_place = i;
	    } 
	  }  

    // if the two cells are not in the same set
    if(cell1_place != cell2_place){
      
      // set wall to empty to connect cells
      dungeon[cur.y_][cur.x_] = TileType::Empty;       

      // store the vector cell2 in a temp value for clean code
      std::vector<Position> cell2_set = cells[cell2_place];

      // combine cell1 and cell2's sets
      cells[cell1_place].insert( cells[cell1_place].end(), cell2_set.begin(), cell2_set.end() );

      // free memory of values in cell2_set
      cells[cell2_place].clear();

      // remove cell2's vector from cells
      cells.erase( cells.begin() + cell2_place );

    } 
    
	
	  walls.pop_back();
	}  
	
  walls.clear();
  cells.clear();	

  /*
     -------------------------------------------
      Part 3 - Apply dungeon to actual board
     -------------------------------------------
  */

  for(int x = 0; x < temp_width; x++){
    for(int y = 0; y < temp_height; y++){
      if(dungeon[y][x] == TileType::Empty){
         
        // if this is the position of a room
        if( (x%2 == 1) && (y%2 == 1) ){

          Position room_center(x*3,y*3);
  
          int room_width = std::rand() % 4;
          int room_height = std::rand() % 4;

          // make room at position
          for(int i = room_center.x_-room_width; i <= room_center.x_+room_width; i++){
            for(int j = room_center.y_-room_height; j <= room_center.y_+room_height; j++){
              board_[0][j][i] = empty_tile_ref_;
            }
          }

        } 
        // if this is the position of a hallway
        else if( (x%2==1) || (y%2 == 1)  ){
          
          bool side = true;
          if(y%2 == 0){
            side = false;
          } 


          if(side){

            int x_start = (x-1)*3;

            int x_end = (x+1)*3;

            for(int i = x_start; i < x_end; i++){
              board_[0][y*3][i] = empty_tile_ref_;  
            }
          }else{
            int y_start = (y-1)*3;

            int y_end = (y+1)*3;

            for(int i = y_start; i < y_end; i++){
              board_[0][i][x*3] = empty_tile_ref_;  
            }
            
          }

        }
 
      } 
    }
  }

  // Make sure boarder is all walls
  std::vector<Tile*> wall_row(width_res_, wall_tile_ref_);
  board_[0][0] = wall_row;
  board_[0][height_res_-1] = wall_row;

  for(int i = 0; i < height_res_; i++){
    board_[0][i][0] = wall_tile_ref_;
    board_[0][i][width_res_-1] = wall_tile_ref_;
  }

}
/*
    Used to move the player that is cotrolled by the user
*/
void Board::MovePlayer(ActionType action_type){
  int type = static_cast<int>(action_type);
  Position old_pos =  player_tile_->get_position(); 
  board_[1][old_pos.y_][old_pos.x_] = empty_tile_ref_;
  switch(type){
    case 101: // up
      if(GetTileAtPosition(0, player_tile_->get_position() + Position(0,-1)) != TileType::Wall){
        up_command_->execute(player_tile_);
      }
      break;
    case 102: // right
      if(GetTileAtPosition(0, player_tile_->get_position() + Position(1,0)) != TileType::Wall){
        right_command_->execute(player_tile_);
      }
      break;
    case 103: // down
      if(GetTileAtPosition(0, player_tile_->get_position() + Position(0,1)) != TileType::Wall){
        down_command_->execute(player_tile_);
      }
      break;
    case 104: // left
      if(GetTileAtPosition(0, player_tile_->get_position() + Position(-1,0)) != TileType::Wall){
        left_command_->execute(player_tile_);
      }
      break;
  }
  Position new_pos =  player_tile_->get_position(); 
  board_[1][new_pos.y_][new_pos.x_] = player_tile_;
}

/*
    Get the tile on the board on a specific layer at a position

    @param  (int layer) - The layer that you are looking at
            (Position pos) - The position (x,y) that you want the tile type of

    @return (TileType) - The tile type at a give point
*/
TileType Board::GetTileAtPosition(int layer, Position pos){
  if(pos.x_ >= 0 && pos.y_ >= 0 && pos.x_ < width_res_ && pos.y_ < height_res_){
      return board_[layer][pos.y_][pos.x_]->get_type();
  }
  throw std::invalid_argument( "index out of range" );
}
