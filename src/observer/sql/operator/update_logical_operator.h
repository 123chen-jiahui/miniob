#pragma once

#include "sql/operator/logical_operator.h"
#include "storage/field/field_meta.h"

// 逻辑算子，用于执行update语句
class UpdateLogicalOperator : public LogicalOperator
{
public:
  UpdateLogicalOperator(Table *table, std::vector<Value> values, const FieldMeta *field);
  virtual ~UpdateLogicalOperator() = default;

  LogicalOperatorType type() const override { return LogicalOperatorType::UPDATE; }
  Table *table() const { return table_; }
  const std::vector<Value> &values() const { return values_; }
  std::vector<Value>       &values() { return values_; }
  const FieldMeta *field() const { return field_; }

private:
  Table *table_ = nullptr;
  std::vector<Value> values_;
  const FieldMeta *field_ = nullptr;
};