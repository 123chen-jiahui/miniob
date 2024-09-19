#pragma once

#include "common/rc.h"
#include "sql/stmt/stmt.h"

class Table;
class Db;

class DropTableStmt : public Stmt
{
public:
  DropTableStmt(const std::string &table_name) : table_name_(table_name) {}
  virtual ~DropTableStmt() = default;

  StmtType type() const override { return StmtType::DROP_TABLE; }
  const std::string &table_name() const { return table_name_; }

public:
  static RC create(const DropTableSqlNode &drop_table_sql, Stmt *&stmt);

private:
  std::string table_name_;
};