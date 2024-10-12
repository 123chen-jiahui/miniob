/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/update_stmt.h"
#include "common/rc.h"
#include "common/type/attr_type.h"
#include "common/type/data_type.h"
#include "storage/db/db.h"
#include "common/log/log.h"
#include "storage/index/index_meta.h"
#include "storage/table/table.h"
#include "storage/table/table_meta.h"
#include "sql/stmt/filter_stmt.h"
#include <cstdint>

UpdateStmt::UpdateStmt(Table *table, const Value *values, int value_amount, FilterStmt *filter_stmt, const FieldMeta *field_meta)
    : table_(table), values_(values), value_amount_(value_amount), filter_stmt_(filter_stmt), field_meta_(field_meta)
{}

UpdateStmt::~UpdateStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
}

// 参考InsertStmt::create和DeleteStmt::create
RC UpdateStmt::create(Db *db, const UpdateSqlNode &update, Stmt *&stmt)
{
  // TODO
  const char *table_name = update.relation_name.c_str();
  if (nullptr == db || nullptr == table_name) {
    LOG_WARN("invalid argument. db=%p, table_name=%p",
        db, table_name);
    return RC::INVALID_ARGUMENT; 
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // 检查attribute
  // 检查field是否存在
  const char *field_name = update.attribute_name.c_str();
  const TableMeta &table_meta = table->table_meta();
  const FieldMeta *field_meta = table_meta.field(field_name);
  if (nullptr == field_meta) { // field不存在
    LOG_WARN("no such field. db=%s, table_name=f%s, field_name=%s", db->name(), table_name, field_name);
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }

  // insert语句没有对field类型进行检查，但是测试用例里有
  AttrType field_type = field_meta->type();
  AttrType value_type = update.value.attr_type();
  if (field_type != value_type) {
    LOG_WARN("field type mismatch. Want %s, but %s", attr_type_to_string(field_type), attr_type_to_string(value_type));
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }


  // 获取value（只有一个）
  const Value *values = &update.value;

  // condition的检查工作会在FilterStmt中完成

  // 创建FilterStmt
  std::unordered_map<std::string, Table *> table_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_name), table));
  
  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(db, table, &table_map, update.conditions.data(), static_cast<int>(update.conditions.size()), filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create filter statement. rc=%d:%s", rc, strrc(rc));
    return rc;
  }


  stmt = new UpdateStmt(table, values, 1, filter_stmt, field_meta);
  return RC::SUCCESS;
  // stmt = nullptr;
  // return RC::INTERNAL;
}
