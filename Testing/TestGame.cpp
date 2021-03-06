#include <QtTest/QtTest>


#include "headers/Board.h"
#include "headers/Entity.h"
#include "headers/Modifier.h"
#include "headers/Skill.h"
#include "headers/Tile.h"
#include "headers/BattleAgent.h"
#include "headers/Item.h"
#include "headers/Entityfactory.h"
#include <vector>
#include <QJsonObject>

class TestGame : public QObject
{
  Q_OBJECT

private slots:
  void TestBoardInit();
  void TestBoardInit_data();
  void TestSkillModifiers();
  void TestMinimax();
  void TestEquipment();
  void TestModiferString();
  QJsonObject GetTestStats();
};

/*
 * Test the board's robustness to being initialized with different sizes
 * */
void TestGame::TestBoardInit()
{
  QFETCH(int, layers);
  QFETCH(int, rooms_x);
  QFETCH(int, rooms_y);
  try {
    Board * board = new Board(layers, rooms_x, rooms_y);
    QVERIFY(true);
  } catch (const std::exception& e) {
    QFAIL("Board threw an exception");
  }
}

void TestGame::TestBoardInit_data()
{
  QTest::addColumn<int>("layers");
  QTest::addColumn<int>("rooms_x");
  QTest::addColumn<int>("rooms_y");

  QTest::newRow("small board") << 4 << 3 << 3;
  QTest::newRow("medium board") << 4 << 5 << 5;
  QTest::newRow("large board") << 4 << 10 << 10;
}

/**
 * @brief TestGame::TestSkillModifiers
 *
 * Tests the functionalities of the ApplySkill() function
 */
void TestGame::TestSkillModifiers()
{
  Entity hippo_goose(GetTestStats());

  std::vector<Modifier> mods;
  Modifier damage_mod(ModifierType::Health, ModifierOperation::Additive, -10);
  Modifier drain_mod(ModifierType::Magic, ModifierOperation::Multiplicative, 0.9f);
  mods.push_back(damage_mod);
  mods.push_back(drain_mod);

  // the 'target' parameter is irrelevant for this test
  Skill attack_skill("", "", mods, 10, Target::Self);

  hippo_goose.ApplySkill(attack_skill);

  QCOMPARE(hippo_goose.GetHealth(), static_cast<float>(90));
  QCOMPARE(hippo_goose.GetMagic(), static_cast<float>(90));
}


void TestGame::TestMinimax(){
  Entity* player = EntityFactory::GeneratePlayer();

  Entity* enemy = EntityFactory::GeneratePlayer();

  BattleAgent* agent = new BattleAgent(player, enemy);

  Skill best_move = agent->GetEnemyMove(3);

  QVERIFY(true);
}

/**
 * Tests item creation and equipping items
 */
void TestGame::TestEquipment()
{
  Entity* player = new Entity(GetTestStats());

  // Direct mods for the item equip
  std::vector<Modifier> mods;
  Modifier increase_health(ModifierType::MaxHealth, ModifierOperation::Additive, 10);
  mods.push_back(increase_health);

  // Mods for the skill for the item
  std::vector<Modifier> recover_mods;
  int recover_cost = 0;
  Modifier recover_mod(ModifierType::Magic, ModifierOperation::Additive, 20);
  recover_mods.push_back(recover_mod);

  Skill recover_skill("Recover", "Regain 10 magic", recover_mods, recover_cost, Target::Self);
  Item item(1, "The legendary claw of destiny", ":)", mods , EquipType::Weapon, recover_skill, "");
  player->EquipItem(item);

  // Test that equipping the item had the intended effects
  QCOMPARE(player->GetMaxHealth(), static_cast<float>(110));
  QCOMPARE(player->GetHealth(), static_cast<float>(110));
  QCOMPARE(player->GetSkills()[0].get_name(), QString("Recover"));

  // Test the unequip ability by replacing the previous item with no mods
  std::vector<Modifier> none;
  Item no_mods(1, "The legendary claw of destiny with no mods", ":)", none, EquipType::Weapon, recover_skill, "");
  player->EquipItem(no_mods);

  QCOMPARE(player->GetMaxHealth(), static_cast<float>(100));
  QCOMPARE(player->GetHealth(), static_cast<float>(100));

  // Direct mods for the item equip
  std::vector<Modifier> more_mods;
  Modifier multiplicative_health(ModifierType::MaxHealth, ModifierOperation::Multiplicative, 1.1);
  more_mods.push_back(multiplicative_health);

  // Test that equipping an item twice doesn't doing anything terrible
  Item item2(1, "Multiplicative Item", ":)", more_mods , EquipType::Weapon, recover_skill, "");
  player->EquipItem(item2);
  player->EquipItem(item2);

  QCOMPARE(player->GetMaxHealth(), static_cast<float>(110));

  // Deal damage to the player
  std::vector<Modifier> damage_mods;
  damage_mods.push_back(Modifier(ModifierType::Health, ModifierOperation::Multiplicative, 0.5));
  Skill damage_skill("Damage", "", damage_mods, 0, Target::Enemy);
  player->ApplySkill(damage_skill);

  QCOMPARE(player->GetHealth(), static_cast<float>(55));

  // See how equipping a diffent item changes things
  player->EquipItem(item);

  QCOMPARE(player->GetMaxHealth(), static_cast<float>(110));
  QCOMPARE(player->GetHealth(), static_cast<float>(55));

}

void TestGame::TestModiferString()
{
  Modifier mod(ModifierType::Health, ModifierOperation::Additive, -5);
  QCOMPARE(mod.ToString(), QString("Changes health by -5"));
  mod = Modifier(ModifierType::Health, ModifierOperation::Multiplicative, 0.5);
  QCOMPARE(mod.ToString(), QString("Changes health by -50%"));
  mod = Modifier(ModifierType::Health, ModifierOperation::Multiplicative, 1.5);
  QCOMPARE(mod.ToString(), QString("Changes health by 50%"));
}

/**
 * @brief TestGame::GetTestStats
 * @return A object containing all the stats an entity needs
 */
QJsonObject TestGame::GetTestStats()
{
  // make entites
  QJsonObject entity_data;
  entity_data["max_health"] = 100;
  entity_data["max_magic"] = 100;
  entity_data["strength"] = 100;
  entity_data["speed"] = 100;
  entity_data["sprite_index"] = 2;
  return entity_data;
}

QTEST_APPLESS_MAIN(TestGame)

#include "TestGame.moc"
