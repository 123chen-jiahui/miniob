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
// Created by Wangyunlai on 2024/05/29.
//

#include "sql/expr/aggregator.h"
#include "common/log/log.h"
#include "common/type/attr_type.h"
#include "common/type/data_type.h"

RC SumAggregator::accumulate(const Value &value)
{
  if (value_.attr_type() == AttrType::UNDEFINED) {
    value_ = value;
    return RC::SUCCESS;
  }
  
  ASSERT(value.attr_type() == value_.attr_type(), "type mismatch. value type: %s, value_.type: %s", 
        attr_type_to_string(value.attr_type()), attr_type_to_string(value_.attr_type()));
  
  Value::add(value, value_, value_);
  return RC::SUCCESS;
}

RC SumAggregator::evaluate(Value& result)
{
  result = value_;
  return RC::SUCCESS;
}

// 求最小值
RC MinAggregator::accumulate(const Value &value)
{
  // 目前的所有类型都支持比较运算
  if (value_.attr_type() == AttrType::UNDEFINED) { // 设初值
    value_ = value;
    return RC::SUCCESS;
  }

  ASSERT(value.attr_type() == value_.attr_type(), "type mismatch. value type: %s, value_.type: %s", 
        attr_type_to_string(value.attr_type()), attr_type_to_string(value_.attr_type()));
  
  if (value.compare(value_) < 0) {
    value_ = value;
  }
  return RC::SUCCESS;
}

RC MinAggregator::evaluate(Value &result)
{
  result = value_;
  return RC::SUCCESS;
}

// 求最大值，同最小值
RC MaxAggregator::accumulate(const Value &value)
{
  if (value_.attr_type() == AttrType::UNDEFINED) {
    value_ = value;
    return RC::SUCCESS;
  }

  ASSERT(value.attr_type() == value_.attr_type(), "type mismatch. value type: %s, value_.type: %s", 
        attr_type_to_string(value.attr_type()), attr_type_to_string(value_.attr_type()));
  
  if (value.compare(value_) > 0) {
    value_ = value;
  }
  return RC::SUCCESS;
}

RC MaxAggregator::evaluate(Value &result)
{
  result = value_;
  return RC::SUCCESS;
}

// 计数
RC CountAggregator::accumulate(const Value &value)
{
  // count并不关心value的值，每进一次这个函数，就让计数器加1
  Value v((int)1);
  if (value_.attr_type() == AttrType::UNDEFINED) { // 第一次进入
    value_ = v;
  } else {
    Value::add(v, value_, value_);
  }
  return RC::SUCCESS;
}

RC CountAggregator::evaluate(Value &result)
{
  result = value_;
  return RC::SUCCESS;
}

// 平均值
RC AvgAggregator::accumulate(const Value &value)
{
  // date_type不能算平均值
  if (value_.attr_type() == AttrType::UNDEFINED) {
    if (value.attr_type() == AttrType::DATE) {
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    } else {
      value_ = Value((float)0.0f);
      Value::add(value, value_, value_);
      count_++;
      return RC::SUCCESS;
    }
  }
  Value::add(value, value_, value_);
  count_++;
  return RC::SUCCESS;
}

RC AvgAggregator::evaluate(Value &result)
{
  result.set_type(AttrType::FLOATS);
  Value::divide(value_, Value((int)count_), result);
  return RC::SUCCESS;
}