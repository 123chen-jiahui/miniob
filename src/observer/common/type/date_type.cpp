#include "common/lang/comparator.h"
#include "common/lang/sstream.h"
#include "common/log/log.h"
#include "common/type/data_type.h"
#include "common/type/date_type.h"
#include "common/value.h"
#include "common/lang/limits.h"
#include "common/value.h"
#include <string>
#include <iomanip>

int DateType::compare(const Value &left, const Value &right) const
{
  ASSERT(left.attr_type() == AttrType::DATE, "left type is not date");
  ASSERT(right.attr_type() == AttrType::DATE, "right type is not date");
  date_t left_val  = left.get_date();
  date_t right_val = right.get_date();

  int ly = left_val.year;
  int lm = left_val.month;
  int ld = left_val.day;
  int ry = right_val.year;
  int rm = right_val.month;
  int rd = right_val.day;

  if ((ly > ry) ||
      (ly == ry && lm > rm) ||
      (ly == ry && lm == rm && ld > rd))
      return 1;
  
  if ((ry > ly) ||
      (ry == ly && rm > lm) ||
      (ry == ly && rm == lm && rd > ld))
      return -1;
  
  return 0;
}

RC DateType::set_value_from_str(Value &val, const string &data) const 
{
  RC rc = RC::SUCCESS;
  int  year, month, day;
  char dash;

  std::istringstream deserialize_stream;
  deserialize_stream.clear();
  deserialize_stream.str(data);
  deserialize_stream >> year >> dash >> month >> dash >> day;
  if (!deserialize_stream || !deserialize_stream.eof()) {
    rc = RC::SCHEMA_FIELD_TYPE_MISMATCH;
  } else {
    val.set_date(year, month, day);
  }
  return rc;
}

RC DateType::to_string(const Value &val, string &result) const
{
  stringstream ss;
  date_t date = val.value_.date;
  ss << std::to_string(date.year) << "-";
  ss << std::setw(2) << std::setfill('0') << std::right << std::to_string(date.month) << "-";
  ss << std::setw(2) << std::setfill('0') << std::right << std::to_string(date.day);
  result = ss.str();
  return RC::SUCCESS;
}