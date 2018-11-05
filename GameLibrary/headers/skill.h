#ifndef SKILL_H
#define SKILL_H

#include "iostream"
#include "Modifer.h"
#include <vector>

/**
 * @brief The Target enum
 *
 * Determines whether to apply a skill on yourself or your enemy
 */
enum class Target {
  Self,
  Enemy
};

/**
 * @brief The Skill class
 *
 * Functions as a ability or skill to be applied on an entity
 */
class Skill{
public:
  Skill(std::string skill_name,  std::string skill_description, std::vector<Modifier> modifiers, Target target):
    skill_name_(skill_name), skill_description_(skill_description), modifiers_(modifiers), target_(target){}

  std::vector<Modifier> GetModifiers() { return modifiers_; }
  Target GetTarget() { return target_; }

private:
  std::string skill_name_;
  std::string skill_description_;
  std::vector<Modifier> modifiers_;
  Target target_;
};

#endif // SKILL_H
