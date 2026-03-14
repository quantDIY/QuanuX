#pragma once

#include "quanux/annex/IStorageResolver.hpp"
#include <string>

namespace quanux {
namespace annex {

/**
 * @brief GCP BigQuery Resolver acting as a GraphQL-to-SQL Transpiler.
 * Interprets GraphQL requests and transpiles them into parameterized Google BigQuery SQL.
 * Operates purely on the Read Path leveraging serverless compute.
 */
class BigQuery_HDF5Resolver_GCP : public IStorageResolver {
public:
    BigQuery_HDF5Resolver_GCP(const std::string& project_id, const std::string& dataset_id);
    virtual ~BigQuery_HDF5Resolver_GCP();

    // IStorageResolver Implementation
    void broadcast_live_markdown(const std::string& markdown) override;
    std::string get_historical_analytics(const std::string& query) override;

private:
    std::string m_project_id;
    std::string m_dataset_id;

    // Internal helper for transpilation
    std::string transpile_graphql_to_sql(const std::string& graphql_query) const;
};

} // namespace annex
} // namespace quanux
