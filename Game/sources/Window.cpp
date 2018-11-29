#include "headers/Window.h"
#include "ui_mainwindow.h"

#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <iostream>
#include <array>

#include "./headers/Tile.h"

Window::Window(QWidget *parent, int window_x, int window_y) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Initialize the scene, which will hold all the objects to render
    scene_ = new QGraphicsScene();
    ui->graphicsView->setScene(scene_);

    // Keep scrollbars from appearing
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Keep the scene from resizing
    ui->graphicsView->setFixedSize(window_x, window_y);
    scene_->setSceneRect(0, 0, window_x, window_y);

    ui->itemFoundDialogue->setVisible(false);

    // Hold all the images
    sprite_sheet_ = QPixmap(":/images/Sprites.png");
    dungeon_sheet_ = QPixmap(":/images/tiles_dungeon_v1.1.png");
    player_sheet_ = QPixmap(":/images/Player.png");
    slime_sheet_ = QPixmap(":/images/Slime.png");

}

Window::~Window()
{
    delete ui;
}

void Window::keyPressEvent(QKeyEvent *event)
{
    emit KeyPressSignal(event);
}

/**
 * When called, re-renders the board. NOTE: currently doesn't delete the memory created
 *
 * @param tileInfo A 3 dimensional qlist representing the board's tiles to render
 */
void Window::UpdateBoard(std::vector< std::vector< std::vector<Tile*> > > tile_info)
{
  scene_->clear();
  for(unsigned y = 0; y < tile_info[0].size(); y++){
    for(unsigned int x = 0; x < tile_info[0][y].size(); x++){
      //Generate dungeon sprites
      Tile* dungeon_tile = tile_info[0][y][x];

      int tile_pos_x = 3;
      int tile_pos_y = 0;

      QGraphicsPixmapItem * pixmap;
      // get dungeon sprite
      if(dungeon_tile->get_type() == TileType::Wall){
        std::array<bool, 4> walls = {true, true, true, true};

        if(y <= 0 ){
          walls[0] = false;
        }else if(tile_info[0][y-1][x]->get_type() != TileType::Wall){ //check above
          walls[0] = false;
        }

        if(x >= tile_info[0][y].size()-1){
          walls[1] = false;
        }else if(tile_info[0][y][x+1]->get_type() != TileType::Wall){ // check right
          walls[1] = false;
        }

        if(y >= tile_info[0].size()-1){
          walls[2] = false;
        }else if(tile_info[0][y+1][x]->get_type() != TileType::Wall){ // check below
          walls[2] = false;
        }

        if(x <= 0){
          walls[3] = false;
        }else if(tile_info[0][y][x-1]->get_type() != TileType::Wall){ // check left
          walls[3] = false;
        }

        pixmap = GetWallSprite(walls);

      }else{
        pixmap = GetDungeonSprite(dungeon_tile);

      }

      pixmap->setPos(x * dungeon_sprite_size_ * dungeon_tile_scale_ , y * dungeon_sprite_size_ * dungeon_tile_scale_ );
      pixmap->setScale(pixmap->scale() * dungeon_tile_scale_);
      scene_->addItem(pixmap);

      // find the tile type to get sprite of
      Tile* tile = tile_info[1][y][x];
      for(unsigned int l = 1; l < tile_info.size(); l++){
          Tile cur_sq = (*tile_info[l][y][x]);
          if((cur_sq == TileType::Empty) == false){
            tile = tile_info[l][y][x];
          }
        }


      // Create and add the tile to the scene

      if(tile->get_type() != TileType::Empty){
        pixmap = GetDungeonSprite(tile);

        pixmap->setPos(x * dungeon_sprite_size_ * dungeon_tile_scale_ , y * dungeon_sprite_size_ * dungeon_tile_scale_ );
        pixmap->setScale(pixmap->scale() * dungeon_tile_scale_);
        scene_->addItem(pixmap);
      }

    }
  }
}

QGraphicsPixmapItem* Window::GetWallSprite(std::array<bool, 4> wall_sides){
  int tile_pos_x = 15;
  int tile_pos_y = 8;

  std::array<bool, 4> all_sides = {true, true, true, true}; // all sides are walls

  std::array<bool, 4> up_right = {true, true, false, false}; // up-right corner
  std::array<bool, 4> up_left = {true, false, false, true}; // up-left corner
  std::array<bool, 4> down_right = {false, true, true, false}; // down-right corner
  std::array<bool, 4> down_left = {false, false, true, true}; // down-left corner

  std::array<bool, 4> up_right_left = {true, true, false, true}; // up-right-left intersection
  std::array<bool, 4> down_right_left = {false, true, true, true}; // down-right-left intersection
  std::array<bool, 4> up_down_right = {true, true, true, false}; // up-down-right intersection
  std::array<bool, 4> up_down_left = {true, false, true, true}; // up-down-left intersection

  std::array<bool, 4> right_left = {false, true, false, true}; // right-left
  std::array<bool, 4> up_down = {true, false, true, false}; // up-down

  std::array<bool, 4> right_end = {false, false, false, true}; // right-end
  std::array<bool, 4> left_end = {false, true, false, false}; // ledr-end

  std::array<bool, 4> up_end = {false, false, true, false}; // up-end
  std::array<bool, 4> down_end = {true, false, false, false}; // down-end


  if(wall_sides == all_sides){
    tile_pos_x = 3;
    tile_pos_y = 21;
  }else if(wall_sides == down_right_left){
    tile_pos_x = 3;
    tile_pos_y = 21;
  }else if(wall_sides == up_right_left){
    tile_pos_x = 3;
    tile_pos_y = 22;
  }else if(wall_sides == up_down_right){
    tile_pos_x = 3;
    tile_pos_y = 23;
  }else if(wall_sides == up_right){
    tile_pos_x = 1;
    tile_pos_y = 22;
  }else if(wall_sides == up_left){
    tile_pos_x = 2;
    tile_pos_y = 22;
  }else if(wall_sides == down_right){
    tile_pos_x = 1;
    tile_pos_y = 21;
  }else if(wall_sides == down_left){
    tile_pos_x = 2;
    tile_pos_y = 21;
  }else if(wall_sides == up_down_left){
    tile_pos_x = 1;
    tile_pos_y = 23;
  }else if(wall_sides == right_left){
    tile_pos_x = 2;
    tile_pos_y = 20;
  }else if(wall_sides == up_down){
    tile_pos_x = 0;
    tile_pos_y = 22;
  }else if(wall_sides == right_end){
    tile_pos_x = 3;
    tile_pos_y = 20;
  }else if(wall_sides == left_end){
    tile_pos_x = 1;
    tile_pos_y = 20;
  }else if(wall_sides == up_end){
    tile_pos_x = 0;
    tile_pos_y = 21;
  }else if(wall_sides == down_end){
    tile_pos_x = 0;
    tile_pos_y = 23;
  }

  QGraphicsPixmapItem * pixmap = new QGraphicsPixmapItem();
  pixmap->setPixmap(dungeon_sheet_.copy(tile_pos_x * dungeon_sprite_size_, tile_pos_y * dungeon_sprite_size_, dungeon_sprite_size_, dungeon_sprite_size_));

  return pixmap;
}


QGraphicsPixmapItem* Window::GetDungeonSprite(Tile* tile){
  int tile_pos_x = 15;
  int tile_pos_y = 8;

  if(tile->get_type() == TileType::Player){
    tile_pos_x = 0;
    tile_pos_y = 0;

    QGraphicsPixmapItem * pixmap = new QGraphicsPixmapItem();
    pixmap->setPixmap(player_sheet_.copy(tile_pos_x * dungeon_sprite_size_, tile_pos_y * dungeon_sprite_size_, dungeon_sprite_size_, dungeon_sprite_size_));
    return pixmap;
  }

  if(tile->get_type() == TileType::Enemy){
    tile_pos_x = 0;
    tile_pos_y = 0;

    QGraphicsPixmapItem * pixmap = new QGraphicsPixmapItem();
    pixmap->setPixmap(slime_sheet_.copy(tile_pos_x * dungeon_sprite_size_, tile_pos_y * dungeon_sprite_size_, dungeon_sprite_size_, dungeon_sprite_size_));
    return pixmap;
  }

  if(tile->get_type() == TileType::Empty){
    tile_pos_x = 0;
    tile_pos_y = 4;
  }else if(tile->get_type() == TileType::Exit){
    tile_pos_x = 9;
    tile_pos_y = 16;
  }else if(tile->get_type() == TileType::Void){
    tile_pos_x = 3;
    tile_pos_y = 0;
  }


  QGraphicsPixmapItem * pixmap = new QGraphicsPixmapItem();
  pixmap->setPixmap(dungeon_sheet_.copy(tile_pos_x * dungeon_sprite_size_, tile_pos_y * dungeon_sprite_size_, dungeon_sprite_size_, dungeon_sprite_size_));

  return pixmap;
}

/**
 * Updates the sidebar that shows the player health, magic, and speed - called whenever a battle is finished
 *
 * @param player
 */
void Window::UpdatePlayerStats(Entity &player)
{
  ui->playerHealth->setText("Health: " + QString::number((int)player.GetHealth()) + " / " + QString::number((int)player.GetMaxHealth()));
  ui->playerMagic->setText("Magic: " + QString::number((int)player.GetMagic()) + " / " + QString::number(((int)player.GetMaxMagic())));
}

void Window::UpdateLevel(int level)
{
  ui->currentLevel->setText("Current Level: " + QString::number(level));
}

/**
 * Update the sidebar equipment panel with the player's equipment
 *
 * @param items
 */
void Window::UpdateItems(std::map<EquipType, Item> items)
{
  for(std::pair<EquipType, Item> kv : items){
      QLabel * equipment_icon;

      // Determine which image to update based on the equipment type
      switch (kv.first) {
        case EquipType::Weapon:
          equipment_icon = ui->equipmentImage0;
          break;
        case EquipType::Armor:
          equipment_icon = ui->equipmentImage1;
          break;
        case EquipType::Trinket:
          equipment_icon = ui->equipmentImage2;
          break;
        case EquipType::Special:
          equipment_icon = ui->equipmentImage3;
          break;
        }

      // Set the EPIC tooltip
      equipment_icon->setToolTip(ItemToHTML(kv.second));
      equipment_icon->setPixmap(kv.second.GetIcon());
    }
}

/**
 * Formats the item modifiers and skill to html for the tooltip
 * @param item
 * @return
 */
QString Window::ItemToHTML(Item item)
{
  QString item_string =
      "<html> \
      <head/> \
      <body align='center'> \
        <b><p style='color:goldenrod; font-size: 20px;'>" + item.GetName() + "</p></b>"
        "<b><p>Item Level: " + QString::number(item.GetLevel()) + "</p></b>";

  for(Modifier mod : item.GetModifiers()){
      item_string += "<b><p style='color:darkslateblue'>" + mod.ToString() + "</p></b>";
    }

  if(item.HasSkill()){
      item_string += "<b><p style='color:darkslateblue'>Unlock Skill: " + item.GetSkill().GetName() + "</p></b>";
    }

  item_string +=
        "<p>" + item.GetDescription() + "</p>" +
      "</body>"
      "</html>";

  return item_string;
}

/**
 * Whenever an item is found, this dialogue window will pop to let the player
 * decide if they want the item or not
 *
 * @param item The item to be equipped
 */
void Window::EnableItemDropUI(Item new_item, std::map<EquipType, Item> equipment)
{
  ui->itemFoundDialogue->setEnabled(true);
  ui->itemFoundDialogue->setVisible(true);

  // Set the images and tooltip of the newly found item
  ui->newItemImage->setToolTip(ItemToHTML(new_item));
  ui->newItemImage->setPixmap(QPixmap(new_item.GetIcon()));

  // Set the currently equipped item
  ui->currentItemImage->setToolTip(ItemToHTML(equipment.at(new_item.GetEquipType())));
  ui->currentItemImage->setPixmap(QPixmap(equipment.at(new_item.GetEquipType()).GetIcon()));
}

void Window::on_save_button_clicked()
{
    emit SaveGameSignal();
}

void Window::on_load_button_clicked()
{
    emit LoadGameSignal();
}

void Window::on_equipButton_clicked()
{
    ui->itemFoundDialogue->setEnabled(false);
    ui->itemFoundDialogue->setVisible(false);
    emit EquipItemSignal(true);
}

void Window::on_throwAwayButton_clicked()
{
    ui->itemFoundDialogue->setEnabled(false);
    ui->itemFoundDialogue->setVisible(false);
    emit EquipItemSignal(false);
}
