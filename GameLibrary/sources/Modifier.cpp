#include "../headers/Modifier.h"
#include <cmath>
#include <stdexcept>

/**
 * Initialize a modifier from its appropriate json object
 * @param data
 */
Modifier::Modifier(QJsonObject data)
{
  if(data.contains("type") &&
     data.contains("application_type") &&
     data.contains("amount") && data["amount"].isDouble()){
     type_ = static_cast<ModifierType>(data["type"].toInt());
     application_type_  = static_cast<ModifierOperation>(data["application_type"].toInt());
     amount_ = data["amount"].toDouble();
    }
  else{
      qWarning("Json modifier initialization is missing one or more parameters.");
    }
}

/**
 * @brief Modifier::GetModifiedStat
 * @param stat The base stat to change
 * @param min The minimum the stat can be (keep from going negative for example)
 * @param max The maximum the stat can be
 * @param reverse If true, will do the opposite effect of the modifier
 * @return The modified stat value
 */
float Modifier::GetModifiedStat(float stat, float min, float max, bool reverse){
  float new_stat;
  if(application_type_ == ModifierOperation::Additive){
      new_stat = reverse ? stat - amount_ : stat + amount_;
    }
  else{
      new_stat = reverse? stat / amount_ : stat * amount_;
    }

  // Check Bounds
  if(new_stat > max){
      return max;
    }
  else if(new_stat < min){
      return min;
    }

  return new_stat;
}

/**
 * Saves the modifier in a QJsonObject
 * @return The QJsonObject
 */
QJsonObject Modifier::Write()
{
  QJsonObject data;
  data["type"] = static_cast<int>(type_);
  data["application_type"] = static_cast<int>(application_type_);
  data["amount"] = static_cast<double>(amount_);
  return data;
}

/**
 * Returns the description of what the modifier does
 * @return
 */
QString Modifier::ToString()
{
  QString adjusted_string = application_type_ == ModifierOperation::Multiplicative ? QString::number((amount_ - 1.0) * 100) + "%" : QString::number(amount_);
  switch(type_){
    case ModifierType::Health:
      return "Changes health by " + adjusted_string;
    case ModifierType::Magic:
      return "Changes magic by " + adjusted_string;
    case ModifierType::MaxHealth:
      return "Changes max health by " + adjusted_string;
    case ModifierType::MaxMagic:
      return "Change max magic by " + adjusted_string;
    }
}

/**
 * If the modifiers are the same type add the amounts together
 * @param m1
 * @return
 */
Modifier Modifier::operator+(const Modifier &m1)
{
  if(!(m1 == *this)){
      throw std::invalid_argument("Modifiers cannot be added.");
    }
  else{
      return Modifier(type_, application_type_, m1.amount_ + amount_);
    }
}

/**
 * Checks to see if the modifiers are the same type and operation_type
 * @param m1
 * @param m2
 * @return
 */
bool operator==(const Modifier &m1, const Modifier &m2)
{
  return m1.type_ == m2.type_ && m1.application_type_ == m2.application_type_;
}
