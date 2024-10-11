#pragma once

#include "sql/operator/logical_operator.h"
#include "storage/field/field_meta.h"

// 逻辑算子，用于执行update语句
class UpdateLogicalOperator : public LogicalOperator
{
public:
  UpdateLogicalOperator(Table *table, Value *value, const FieldMeta *field);
  virtual ~UpdateLogicalOperator() = default;

  LogicalOperatorType type() const override { return LogicalOperatorType::UPDATE; }
  Table *table() const { return table_; }
  Value *value() const { return value_; }
  const FieldMeta *field() const { return field_; }

private:
  Table *table_ = nullptr;
  Value *value_ = nullptr;
  const FieldMeta *field_ = nullptr;
};