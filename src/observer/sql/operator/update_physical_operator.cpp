#include "sql/operator/update_physical_operator.h"
#include "common/log/log.h"
#include "common/rc.h"
#include "common/value.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"
#include <cstring>

RC UpdatePhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::SUCCESS;
  }

  std::unique_ptr<PhysicalOperator> &child = children_[0];

  RC rc = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  trx_ = trx;

  while (OB_SUCC(rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }

    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
    Record   &record    = row_tuple->record();

    // 构造data
    char *data = record.data();
    const char *new_data = values_[0].data();
    int field_offset = field_->offset();
    int field_len = field_->len();
    memcpy(data + field_offset, new_data, field_len);

    records_.emplace_back(std::move(record));

    datas_.emplace_back(data);
  }

  child->close();

  // 先收集记录再更新
  // 记录的有效性由事务来保证，如果事务不保证删除的有效性，那说明此事务类型不支持并发控制，比如VacuousTrx
  for (int i = 0; i < records_.size(); i ++) {
    Record &record = records_[i];
    char *data = datas_[i];
    rc = trx_->update_record(table_, record, data);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to update record: %s", strrc(rc));
      return rc;
    }
  }

  return RC::SUCCESS;
}

RC UpdatePhysicalOperator::next()
{
  return RC::RECORD_EOF;
}

RC UpdatePhysicalOperator::close()
{
  return RC::SUCCESS;
}