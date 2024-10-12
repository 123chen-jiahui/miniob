#include "sql/operator/update_logical_operator.h"
#include "common/value.h"

UpdateLogicalOperator::UpdateLogicalOperator(Table *table, std::vector<Value> values, const FieldMeta *field) : table_(table), values_(values), field_(field) {}