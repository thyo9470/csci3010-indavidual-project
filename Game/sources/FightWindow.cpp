﻿#include "../headers/FightWindow.h"

#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QTime>
#include <iostream>
#include <ui_fightwindow.h>

void delay(int delay_time)
{
    QTime dieTime= QTime::currentTime().addMSecs(delay_time);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


FightWindow::FightWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::FightWindow)
{
  ui->setupUi(this);

  // Initialize the scene, which will hold all the objects to render
  scene_ = new QGraphicsScene();
  ui->backgroundView->setScene(scene_);

  // Keep scrollbars from appearing
  ui->backgroundView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  ui->backgroundView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // Hold all the images
  sprite_sheet_ = QPixmap(":/images/Sprites.png");
  player_sheet_ = QPixmap(":/images/Player.png");
  slime_sheet_ = QPixmap(":/images/Slime.png");

  //load backgroundr
  QLabel* background = new QLabel();
  background->setGeometry(10, 10, 781, 421);
  background->setPixmap(QPixmap(":/images/souterrain.png"));
  background->setScaledContents(true);
  scene_->addWidget(background);

  // hide exit button
  ui->exitButton->setVisible(false);

  // hide game over dialogue
  ui->gameOverBox->setVisible(false);

  //set player and enemy position
  player_position_x_ = 100;
  //player_position_y_ = 6.0*scene_->height()/10.0;
  player_position_y_ = 5.0*scene_->height()/10.0;

  //enemy_position_x_ = scene_->width()-170;
  enemy_position_x_ = scene_->width()-230;
  //enemy_position_y_ = 6.0*scene_->height()/10.0;
  enemy_position_y_ = 5.3*scene_->height()/10.0;

  // connect pressed button
  connect(ui->skillButton_1, &QPushButton::pressed, this, &FightWindow::ButtonPressedSlot);
  connect(ui->skillButton_2, &QPushButton::pressed, this, &FightWindow::ButtonPressedSlot);
  connect(ui->skillButton_3, &QPushButton::pressed, this, &FightWindow::ButtonPressedSlot);
  connect(ui->skillButton_4, &QPushButton::pressed, this, &FightWindow::ButtonPressedSlot);
  connect(ui->runButton, &QPushButton::pressed, this, &FightWindow::ButtonPressedSlot);
  connect(ui->exitButton, &QPushButton::pressed, this, &FightWindow::ExitButtonPressed);
  connect(ui->gameOverButton, &QPushButton::pressed, this, &FightWindow::GameOverButtonPressed);

}

FightWindow::~FightWindow(){
    delete ui;
}

void FightWindow::ButtonPressedSlot(){
  QObject *senderObj = sender(); // This will give Sender object
  int skill_id = senderObj->property("skill").value<int>();  //objectName();

  emit ButtonPressedSignal(skill_id);
}

/**
 * Return to the board when the exit button is pressed
 */
void FightWindow::ExitButtonPressed()
{
  emit ToBoardSignal();
}

/**
 * When the game over button is pressed, return to the main menu
 */
void FightWindow::GameOverButtonPressed()
{
  emit ToBoardSignal();
  emit GameOverSignal();
}

/**
 * @brief FightWindow::UpdateFightWindow
 *
 * Updates the fight window GUI
 *
 * @param battle_sim - The current state of the battle_sim
 */

void FightWindow::UpdateFightWindow(BattleSim* battle_sim){

  // if it's the end of a battle show exit button otherwise hide it
  if(battle_sim->GetState() == BattleState::End){
    ui->exitButton->setVisible(true);
    SetActionsVisible(false);
    }else if(battle_sim->GetState() == BattleState::Active){
    ui->exitButton->setVisible(false);
    ui->gameOverBox->setVisible(false);
    SetActionsVisible(true);
    }

  // removes old enemy and player pixmap from scene
  for(auto& item :scene_->items()){
    if( item->zValue() == 1){
      scene_->removeItem(item);
    }
  }

  //update log
  std::vector<std::string> log = battle_sim->GetLog();
  std::string log_formatted = "";
  for(std::string message : log){
    log_formatted += "\n" + message;
  }
  ui->dialogLabel->setText(QString::fromStdString(log_formatted));

  //updatee player info
  Entity* player = battle_sim->GetPlayer();
  ui->playerHealthBar->setMaximum(player->GetMaxHealth());
  ui->playerHealthBar->setValue(player->GetHealth());
  ui->playerMagicBar->setMaximum(player->GetMaxMagic());
  ui->playerMagicBar->setValue(player->GetMagic());

  // update skill info
  std::vector<Skill> player_skills = player->GetSkills();
  std::vector<QPushButton*> button_skills = {ui->skillButton_1, ui->skillButton_2, ui->skillButton_3, ui->skillButton_4};
  for(unsigned int i = 0; i < 4; i++){
      if(i < player_skills.size()){
        button_skills[i]->setText(player_skills[i].GetName());
        button_skills[i]->setToolTip(player_skills[i].GetDescription());
        }
      else{
          button_skills[i]->setText("Empty");
          button_skills[i]->setToolTip("");
        }
  }

  int row = 0;
  // Create and add the tile to the scene
  QGraphicsPixmapItem * pixmap_player = new QGraphicsPixmapItem();
  pixmap_player->setZValue(1);
  pixmap_player->setPixmap(player_sheet_.copy(0, 0, dungeon_sprite_size_, dungeon_sprite_size_));
  pixmap_player->setPos( player_position_x_ , player_position_y_);
  pixmap_player->setScale(pixmap_player->scale() * tile_scale_);
  scene_->addItem(pixmap_player);

  //update enemy info
  Entity* enemy = battle_sim->GetEnemy();
  ui->enemyHealthBar->setMaximum(enemy->GetMaxHealth());
  ui->enemyHealthBar->setValue(enemy->GetHealth());
  ui->enemyMagicBar->setMaximum(enemy->GetMaxMagic());
  ui->enemyMagicBar->setValue(enemy->GetMagic());

  // Create and add the tile to the scene
  QGraphicsPixmapItem * pixmap_enemy = new QGraphicsPixmapItem();
  pixmap_enemy->setZValue(1);
  pixmap_enemy->setPixmap(slime_sheet_.copy(0, 0, dungeon_sprite_size_, dungeon_sprite_size_));
  pixmap_enemy->setPos( enemy_position_x_ , enemy_position_y_);
  pixmap_enemy->setScale(pixmap_enemy->scale() * tile_scale_);
  scene_->addItem(pixmap_enemy);

  //AnimateAttack(pixmap_player);

}

void FightWindow::AnimateAttack(QGraphicsPixmapItem* obj){

  int distance = 5;

  for(int i = 0; i < distance; i++){
    delay(250);
    obj->setOffset(i, 0);
  }

}

/**
 * Shows the game over dialogue
 * @param enemy_name
 * @param level
 */
void FightWindow::ShowGameOver(QString enemy_name, int level)
{
  ui->gameOverDesc->setText("You were slain by a " + enemy_name + " at level " + QString::number(level) + ".");
  ui->gameOverBox->setVisible(true);
  SetActionsVisible(false);
}

/**
 * Sets all action button visibility
 * @param visible
 */
void FightWindow::SetActionsVisible(bool visible)
{
  ui->skillButton_1->setVisible(visible);
  ui->skillButton_2->setVisible(visible);
  ui->skillButton_3->setVisible(visible);
  ui->skillButton_4->setVisible(visible);
  ui->runButton->setVisible(visible);
}
