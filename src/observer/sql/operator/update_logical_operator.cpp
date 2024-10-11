#include "sql/operator/update_logical_operator.h"
#include "common/value.h"

UpdateLogicalOperator::UpdateLogicalOperator(Table *table, Value *value, const FieldMeta *field) : table_(table), value_(value), field_(field) {}