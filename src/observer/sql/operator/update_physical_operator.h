#pragma once

#include "sql/operator/physical_operator.h"
#include "storage/field/field_meta.h"

// update的物理算子
class UpdatePhysicalOperator : public PhysicalOperator
{
public:
  UpdatePhysicalOperator(Table *table, std::vector<Value> &&values, const FieldMeta *field): table_(table), values_(std::move(values)), field_(field) {}

  virtual ~UpdatePhysicalOperator() = default;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::UPDATE; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override { return nullptr; }

private:
  Table *table_ = nullptr;
  std::vector<Value> values_;
  Trx *trx_ = nullptr;
  const FieldMeta *field_ = nullptr;
  std::vector<Record> records_; // 记录要更新的records
  std::vector<char *> datas_;   // 记录新的record data
};